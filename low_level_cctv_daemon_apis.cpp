/**
 * File Name:   low_level_cctv_daemon_apis.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  2/27/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  4/16/20
 *
 * Description:
 * This file contains definitions of functions of the SmartCCTV Daemon's internal API.
 * These functions are mainly concerned with setting up the daemon.
 */

#include "low_level_cctv_daemon_apis.h"
#include "camera_daemon.h"

#include <sys/types.h>
#include <sys/stat.h>   /* for umask(), mode permissions constants */
#include <fcntl.h>      /* for O_* constants, open() */
#include <signal.h>     /* for kill() */
#include <unistd.h>     /* for close(), unlink(), fork(), setsid(), sysconf(), chdir(), getpid(), sleep() */
#include <errno.h>      /* for errno */
#include <syslog.h>     /* for openlog(), syslog(), closelog() */
#include <cstdlib>      /* for exit(), atexit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <cstdio>       /* for fopen(), fclose(), fseek(), fgetc(), fscanf(), fprintf() */
#include <cstring>      /* for strerror() */
#include <cctype>       /* for isdigit() */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"


/**
 * This struct contains all the data of the daemon that might need to be accessed globally.
 * It is done like this because signal handler functions maybe called, which do not accept
 * custom parameters.
 * The Daemon_data is volatile because it is a global object that can be changed by multiple processes,
 * both the GUI process and the daemon process. So we want to disable cache optimizations.
 */
volatile Daemon_data daemon_data = {
    .pid_file_name = "/tmp/SmartCCTV_daemon_pid",  // The path to the PID file.
    .pid_file_descriptor = 0,                      // A descriptor to this file.
    .pid_file_pointer = nullptr,                   // A pointer to this file.
    .camera_daemon_pid = 0,                        // The PID of the daemon.
    .home_directory = nullptr,                     // The path to the home directory, $HOME.
    .daemon_exit_status = EXIT_SUCCESS  // The exit status of the daemon, to use in terminate_daemon(), assumed EXIT_SUCCESS.
};


void remove_pid_file(FILE* pid_file_pointer)
{
    if (fclose(pid_file_pointer) == EOF) {
        syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_data.pid_file_name);
    }
    if (unlink(daemon_data.pid_file_name) == -1) {
        syslog(log_facility | LOG_ERR, "Error: Could not unlink PID file %s : %m", daemon_data.pid_file_name);
        syslog(log_facility | LOG_WARNING, "System Admin please take a look at this.");
    }
}


bool checkPidFile(bool turn_on)
{
    //enum { ON, OFF } state = OFF;
    bool state = false;
    // Try to open the PID file, check if the daemon is already running.
    // If the daemon is not already running, then set the state to OFF,
    // we can return from the function and start up the daemon.
    // If a PID file exists, we must check is the daemon already running
    // or is the PID file bad.
    // If the daemon is already running, then set the state to ON.
    if ( (daemon_data.pid_file_pointer = fopen(daemon_data.pid_file_name, "r")) != nullptr) {
        syslog(log_facility | LOG_NOTICE, "A PID file already exists.");
        syslog(log_facility | LOG_NOTICE, "Checking if the SmartCCTV Daemon is running.");

        char digit = '\0';
        // Check if the PID file contains a valid process ID of the daemon process.
        // Check if all the characters in the PID file are digits.
        // Check if the PID file was not tampered.
        while (true) {
            digit = fgetc(daemon_data.pid_file_pointer);
            if (digit == EOF)
                break;

            if (digit == '\0' || digit == '\n')
                continue;

            if (!isdigit(digit)) {
                syslog(log_facility | LOG_ERR, "Error: Invalid PID file: text is not a process id");
                syslog(log_facility | LOG_WARNING, "Removing Invalid PID file %s", daemon_data.pid_file_name);
                remove_pid_file(daemon_data.pid_file_pointer);
                // for creating a greyed out effect in the GUI window
                sleep(8);
                exit(EXIT_FAILURE);
            }
        }

        int daemon_pid = 0;
        fseek(daemon_data.pid_file_pointer, 0, SEEK_SET);
        fscanf(daemon_data.pid_file_pointer, "%d", &daemon_pid);
        daemon_data.camera_daemon_pid = daemon_pid;

        // Check if the PID file contains a valid process ID of the daemon process.
        // If the PID file is indeed all digits, we must then make sure that it represents a valid process ID.
        // kill() with a signal of 0, doesn't send a signal to the process, but it checks if that process exists.
        // If that process doesn't exist, then kill() will return -1 and errno will be set.
        // This only works if you're the user owning that process, or if you are the root user.
        if (kill(daemon_pid, 0) == -1) {
            syslog(log_facility | LOG_ERR, "Error: Invalid PID file : %m");
            syslog(log_facility | LOG_WARNING, "Removing PID file for defunct process %d", daemon_pid);
            remove_pid_file(daemon_data.pid_file_pointer);
            // for creating a greyed out effect in the GUI window
            sleep(8);
            exit(EXIT_FAILURE);
        // If that process does exist, it means that the SmartCCTV daemon is already running, so we can't
        // start a new daemon process running.
        } else {
            state = true;
        }
    } else {
        state = false;
    }

    return state != turn_on;
}


void becomeDaemon()
{
    // The process does yet another fork.
    // The parent process (original child) dies, making the forked process a grandchild.
    // By having the first child process die, we've severed the relationship between the
    // child process and the original parent process.
    // In order to become a deamon, you need to detatch that process from the shell.
    // The daemon process has to be a grandchild of the shell process, not just a child.
    // That is because the shell cannot reap grandchildren, only children.
    if (fork() != 0) {
        exit(EXIT_SUCCESS);
    }

    closelog();
    // Because the daemon is a background process without access to the terminal,
    // the only way it can communicate with the outside world is through the logger.
    // Using the syslog is de facto way of implementing logging in Linux/Unix.
    openlog("SmartCCTV_Daemon", LOG_PID, log_facility);
    atexit(closelog);

    // By default a child process inherits it's parent session ID and process group ID.
    // The daemon process inherited it's parent session ID and process group ID from the shell.
    // The daemon process is bound to the shell theough the session ID and process group ID.
    // When the shell exits, all the processes in that same session recieve a SIGHUP,
    // which by default terminates the process.
    // setsid() makes the session ID and process group ID be the same as the PID of this process.
    // This creates a new session, of which this process is the process group leader.
    // The new session is detatched from the controlling terminal.
    if (setsid() == -1) {
        syslog(log_facility | LOG_ERR, "Error: Could not change the session ID and process group ID : %m");
        syslog(log_facility | LOG_CRIT, "SmartCCTV Daemon unexpected failure.");
        // This time, it is actually the daemon process which gets killed,
        // if it can't change the session ID and process group ID.
        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    }

    // sysconf(_SC_OPEN_MAX) is the number of all possible file descriptors
    // This closes all possible open file descriptors.
    /**
    long max_open_file_descriptors = sysconf(_SC_OPEN_MAX);
    for (long i = 0; i < max_open_file_descriptors; ++i) {
        if (i != daemon_data.pid_file_descriptor) {
            close(i);
        }
    }
    **/

    // Then we have to reopen stdin, stdout, and stderr because these three file descriptors
    // have to be open by the POSIX standard.
    // Open all three to /dev/null, which is a black hole.
    // This is because the daemon is detatched from the terminal, it cannot communicate via the
    // standard file descriptors.
    open("/dev/null", O_RDWR);  // fd 0  STDIN
    open("/dev/null", O_RDWR);  // fd 1  STDOUT
    open("/dev/null", O_RDWR);  // fd 3  STDERR

    // Get the daemon process out of whatever weird directory it is into the root directory.
    // This means that the user can just open any terminal window anywhere in the file system
    // to start the daemon. Also, if the daemon is started in a directory that will later become
    // unmounted, this will not affect the daemon because it will get out of that directory.
    // If the daemon is in the root directory, you would have to use absolute paths for each accessed file.
    chdir("/");

    // The umask determines the default file permissions that happen when a new file is created.
    // It specifies what the user, group, other CAN'T do. It's a mask over the file permissions.
    // The umask is inherited from the parent process to the child process.
    // Get rid of any masking bits that the daemon process would have inherited from the shell.
    // You set the umask to a determined value instead of trusting the shell.
    // The daemon process would need to create files, and if the umask has been tampered with,
    // it would compromize the daemon's ability to do so.
    umask(0);

    // Write the PID of the daemon process to the PID file.
    daemon_data.camera_daemon_pid = getpid();
    fprintf(daemon_data.pid_file_pointer, "%d", daemon_data.camera_daemon_pid);
    fclose(daemon_data.pid_file_pointer);
    close(daemon_data.pid_file_descriptor);

    // This sets up the signal handler for when the process is terminated.
    struct sigaction action1;
    action1.sa_handler = terminate_daemon;
    sigemptyset(&action1.sa_mask);
    sigaddset(&action1.sa_mask, SIGINT);
    sigaddset(&action1.sa_mask, SIGQUIT);
    sigaddset(&action1.sa_mask, SIGTERM);
    sigaddset(&action1.sa_mask, SIGTSTP);
    sigaddset(&action1.sa_mask, SIGHUP);
    sigaddset(&action1.sa_mask, SIGUSR1);
    sigaddset(&action1.sa_mask, SIGUSR2);
    sigaddset(&action1.sa_mask, SIGCONT);
    sigaddset(&action1.sa_mask, SIGCHLD);
    action1.sa_flags = 0;
    sigaction(SIGINT, &action1, nullptr);
    sigaction(SIGTERM, &action1, nullptr);
    sigaction(SIGQUIT, &action1, nullptr);

    camera_daemon();
}


void terminate_daemon(int)
{
    if (close(daemon_data.pid_file_descriptor) == -1) {
        syslog(log_facility | LOG_ERR, "Error: could not close the PID file.");
    }

    if (unlink(daemon_data.pid_file_name) == -1) {
        syslog(log_facility | LOG_ERR, "Error: could not remove the PID file.");
    } else {
        syslog(log_facility | LOG_NOTICE, "Removed the PID file successfully.");
    }

    syslog(log_facility | LOG_NOTICE, "The camera daemon is turning off.");

    exit(daemon_data.daemon_exit_status);
}


#pragma GCC diagnostic pop

