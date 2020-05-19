/**
 * File Name:   high_level_cctv_daemon_apis.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  4/11/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/18/20
 *
 * Description:
 * This file contains definitions of functions of the SmartCCTV Daemon's external API.
 * The high level daemon API, which is the API used by outside code to call the daemon and interact with it.
 */

#include "high_level_cctv_daemon_apis.h"
#include "low_level_cctv_daemon_apis.h"
#include "write_message.h"

#include <sys/types.h>
#include <sys/stat.h>   /* for umask(), mode permissions constants */
#include <fcntl.h>      /* for O_* constants, open() */
#include <signal.h>     /* for kill() */
#include <unistd.h>     /* for fork() */
#include <errno.h>      /* for errno */
#include <syslog.h>     /* for syslog() */
#include <cstdlib>      /* for exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <cstdio>       /* for fopen(), fdopen(), fclose(), fseek(), fgetc(), fscanf(), FILE */
#include <cctype>       /* for isdigit() */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"


extern Daemon_data daemon_data;

void Daemon_facade::set_daemon_info(const char* home_directory)
{
    daemon_data.home_directory = home_directory;
}


int Daemon_facade::run_daemon(bool enable_human_detection, bool enable_motion_detection, bool enable_outlines, int cameraNumber)
{
    // User has requested to start the SmartCCTV daemon.
    daemon_data.enable_human_detection = enable_human_detection;
    daemon_data.enable_motion_detection = enable_motion_detection;
    daemon_data.enable_outlines = enable_outlines;
    daemon_data.cameraNumber = cameraNumber;

    enum return_states { SUCCESS, DAEMON_ALREADY_RUNNING, PERMISSIONS_ERROR };

    if (!checkPidFile(true)) {
        syslog(log_facility | LOG_ERR, "Error: A SmartCCTV Daemon is already running with PID %d", daemon_data.camera_daemon_pid);
        // Close the PID file, since it was opened in checkPidFile() function.
        if (fclose(daemon_data.pid_file_pointer) == EOF) {
            syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_data.pid_file_name);
        }

        return DAEMON_ALREADY_RUNNING;
    }

    // Reset the umask so that you have no problems creating the file with the desired permissions.
    umask(0);
    // The process ID file is like a lock file. It must be created before the daemon starts up.
    // Create the process ID file and obtain a handle to it.
    // Do that before you become a daemon, becuase if the file was failed to create,
    // you can't write the PID of the daemon process into that file.
    if ( (daemon_data.pid_file_descriptor = open(daemon_data.pid_file_name, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
        syslog(log_facility | LOG_ERR, "Error: Not able to create the file %s : %m", daemon_data.pid_file_name);

        return PERMISSIONS_ERROR;
    }
    if ( (daemon_data.pid_file_pointer = fdopen(daemon_data.pid_file_descriptor, "w")) == NULL) {
        syslog(log_facility | LOG_ERR, "Error: Not able to open the file %s : %m", daemon_data.pid_file_name);

        return PERMISSIONS_ERROR;
    }
    syslog(log_facility | LOG_NOTICE, "PID file created successfully");

    // At the point where you want to create a daemon, you're going to do a fork().
    // If you're the child, you're going to call this function from which you're never going to return.
    // The child will call this function from which it will never return.
    // It will be inside this function for it's entire life time.
    // The parent will continue on and print a confirmation message to the user.
    if (fork() == 0) {
        becomeDaemon();
    }

    // After creating the PID file
    // Close the PID file in the GUI process.
    /*
    if (close(daemon_data.pid_file_descriptor) == -1) {
        syslog(log_facility | LOG_ERR, "Error: GUI process could not close PID file %s : %m", daemon_data.pid_file_name);
    }
    daemon_data.pid_file_pointer = nullptr;
    */

    syslog(log_facility | LOG_NOTICE, "Starting SmartCCTV Daemon");
    return SUCCESS;
}


bool Daemon_facade::kill_daemon()
{
    // User has requested to stop the SmartCCTV daemon.

    if (!checkPidFile(false)) {
        syslog(log_facility | LOG_ERR, "Error: A SmartCCTV Daemon not already running.");

        return false;
    } else {
        // Close the PID file, since it was opened in checkPidFile() function.
        if (fclose(daemon_data.pid_file_pointer) == EOF) {
            syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_data.pid_file_name);
        }
    }

    syslog(log_facility | LOG_NOTICE, "Killing SmartCCTV Daemon");
    syslog(log_facility | LOG_NOTICE, "Removing PID file");
    kill(daemon_data.camera_daemon_pid, SIGINT);
    // The daemon will remove the PID file by itself.

    return true;
}


bool Daemon_facade::is_daemon_running()
{
    // if (A PID file created by the daemon exists) {
    if (!checkPidFile(true)) {
        // A SmartCCTV Daemon is already running.

        // Close the PID file, since it was opened in checkPidFile() function.
        if (fclose(daemon_data.pid_file_pointer) == EOF) {
            syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_data.pid_file_name);
        }

        return true;
    } else {
        // A SmartCCTV Daemon not already running.
        return false;
    }
}


void Daemon_facade::remove_pid_file(FILE* pid_file_pointer)
{
    if (fclose(pid_file_pointer) == EOF) {
        syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_data.pid_file_name);
    }
    if (unlink(daemon_data.pid_file_name) == -1) {
        syslog(log_facility | LOG_ERR, "Error: Could not unlink PID file %s : %m", daemon_data.pid_file_name);
        syslog(log_facility | LOG_WARNING, "System Admin please take a look at this.");
    }
}


bool Daemon_facade::checkPidFile(bool turn_on)
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

                write_message("SmartCCTV has been tampered.");
                // Since the tampered PID file was already deleted, assume that it doesn't exist (state == false).
                return false != turn_on;
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

            write_message("SmartCCTV has been tampered.");
            // Since the tampered PID file was already deleted, assume that it doesn't exist (state == false).
            return false != turn_on;

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


#pragma GCC diagnostic pop

