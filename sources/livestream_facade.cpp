#include "livestream_facade.h"
#include "livestream_window.h"
#include "write_message.h"

#include <fcntl.h>      /* for O_* constants, open() */
#include <unistd.h>     /* for close(), unlink(), fork(), setsid(), chdir(), getpid(), sleep() */
#include <signal.h>     /* for sigemptyset(), signal constants */
#include <errno.h>      /* for errno */
#include <syslog.h>     /* for openlog(), syslog(), closelog() */
#include <dirent.h>     /* for opendir(), readdir(), closedir() */
#include <cstdlib>      /* for exit(), atexit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <cstdio>       /* for FILE, fclose(), fprintf() */
#include <cstring>      /* for strncmp() */
#include <string>       /* for std::string */

using std::string;
using std::size_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wreturn-type"


LiveStream_window* liveStream_window_ptr = nullptr;
int exit_code = EXIT_SUCCESS;

// This is supposed to be the private data of the LiveStream_facade class,
// however it is inside a global struct instead because that data will be accessed by signal handler functions,
// which are required to be stand alone global functions and cannot be member functions of a particular class.
struct LiveStream_viewer_data {
    string streamDir;                  // The parent directory into which live stream images are saved.
    string default_images_dir;          // The directory where default images are stored.
    const char* my_pid_file_name;      // The path to the LiveStream Viewer process's PID file.
    int pid_file_descriptor;           // A descriptor to this file.
    FILE* pid_file_pointer;            // A pointer to this file.
    int daemon_process_pid;            // The PID of the SmartCCTV camera daemon.
    bool SmartCCTV_daemon_is_running;  // Is the daemon proces running or not?
} liveStream_viewer_data = {
    // Set the default values for the data members.
    .streamDir = "/tmp/SmartCCTV_livestream/",  // The parent directory into which live stream images are saved.
    .default_images_dir = "$HOME/SmartCCTV_recordings/default_images",  // The directory where default images are stored.
    .my_pid_file_name = "/tmp/LiveStream_viewer_pid",  // The path to the LiveStream Viewer process's PID file.
    .pid_file_descriptor = 0,                          // A descriptor to this file.
    .pid_file_pointer = nullptr,                       // A pointer to this file.
    .daemon_process_pid = 0,                           // The PID of the SmartCCTV camera daemon.
    .SmartCCTV_daemon_is_running = false               // Is the daemon proces running or not?
};


void terminate_livestream(int)
{
    if (liveStream_window_ptr != nullptr) {
        liveStream_window_ptr->finalize();
    }

    //  Tell the daemon that the LiveStream Viewer is shutting down, so it should stop saving images
    // into that direcotry.
    if (liveStream_viewer_data.daemon_process_pid) {
        syslog(log_facility | LOG_WARNING, "sending signal to %d", liveStream_viewer_data.daemon_process_pid);
        kill(liveStream_viewer_data.daemon_process_pid, SIGUSR2);
    }

    if (close(liveStream_viewer_data.pid_file_descriptor) == -1) {
        syslog(log_facility | LOG_ERR, "Error: could not close the PID file.");
    }

    if (unlink(liveStream_viewer_data.my_pid_file_name) == -1) {
        syslog(log_facility | LOG_ERR, "Error: Could not unlink PID file %s : %m", liveStream_viewer_data.my_pid_file_name);
        syslog(log_facility | LOG_WARNING, "System Admin please take a look at this.");
    } else {
        syslog(log_facility | LOG_NOTICE, "Removed the PID file successfully.");
    }

    syslog(log_facility | LOG_NOTICE, "The LiveStream Viewer is turning off.");

    exit(exit_code);
}


void camera_daemon_starts_up(int)
{
    syslog(log_facility | LOG_NOTICE, "camera_daemon_starts_up()");
    int daemon_pid = get_daemon_pid();
    liveStream_viewer_data.daemon_process_pid = (daemon_pid != -1) ? daemon_pid : 0;
    // The SmartCCTV camera daemon already knows that the LiveStream Viewer process is up and running.

    // Check the LiveStream Viewer can:
    // - get the daemon's PID to send it signals
    // - find the camera directory to open images
    liveStream_viewer_data.SmartCCTV_daemon_is_running = liveStream_viewer_data.daemon_process_pid && find_camera_directory();
    liveStream_window_ptr->set_streamDir(liveStream_viewer_data.streamDir);
}


void camera_daemon_shuts_down(int)
{
    syslog(log_facility | LOG_NOTICE, "camera_daemon_shuts_down()");
    liveStream_viewer_data.SmartCCTV_daemon_is_running = false;
}


// The constructor is run in the GUI process.
LiveStream_facade::LiveStream_facade()
 : private_data(&liveStream_viewer_data), my_pid(0)
{
    //
}


int LiveStream_facade::run_livestream_viewer(const string& home_directory)
{
    liveStream_viewer_data.default_images_dir = home_directory;
    liveStream_viewer_data.default_images_dir += "/SmartCCTV_recordings/default_images/";

    enum return_states { SUCCESS, ALREADY_RUNNING, PERMISSIONS_ERROR };

    if (is_livestream_running()) {
        syslog(log_facility | LOG_ERR, "Error: A LiveStream Viewer is already running with PID %d", my_pid);
        return ALREADY_RUNNING;
    }

    // The process ID file is like a lock file. It must be created before the LiveStream Viewer starts up.
    // Create the process ID file and obtain a handle to it.
    if ( (private_data->pid_file_descriptor = open(private_data->my_pid_file_name, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
        syslog(log_facility | LOG_ERR, "Error: Not able to create the file %s : %m", private_data->my_pid_file_name);

        return PERMISSIONS_ERROR;
    }
    if ( (private_data->pid_file_pointer = fdopen(private_data->pid_file_descriptor, "w")) == NULL) {
        syslog(log_facility | LOG_ERR, "Error: Not able to open the file %s : %m", private_data->my_pid_file_name);

        return PERMISSIONS_ERROR;
    }
    syslog(log_facility | LOG_NOTICE, "LiveStream Viewer's PID file created successfully");

    if (fork() == 0) {
        become_livestream_process();
    }

    // After creating the PID file
    // Close the PID file in the GUI process.
    if (close(private_data->pid_file_descriptor) == -1) {
        syslog(log_facility | LOG_ERR, "Error: GUI process could not close PID file %s : %m", private_data->my_pid_file_name);
    }
    private_data->pid_file_pointer = nullptr;

    syslog(log_facility | LOG_NOTICE, "Starting Live Stream Viewer Process");
    return SUCCESS;
}


bool LiveStream_facade::is_livestream_running()
{
    // Try to open the PID file, check if the livestream viewer is already running.
    if ( (private_data->pid_file_pointer = fopen(private_data->my_pid_file_name, "r")) != nullptr) {
        syslog(log_facility | LOG_NOTICE, "A LiveStream PID file already exists.");
        syslog(log_facility | LOG_NOTICE, "Checking if the LiveStream Viewer is running.");

        char digit = '\0';
        // Check if the PID file contains a valid process ID of the LiveStream Viewer.
        // Check if all the characters in the PID file are digits.
        // Check if the PID file was not tampered.
        while (true) {
            digit = fgetc(private_data->pid_file_pointer);
            if (digit == EOF)
                break;

            if (digit == '\0' || digit == '\n')
                continue;

            if (!isdigit(digit)) {
                syslog(log_facility | LOG_ERR, "Error: Invalid PID file: text is not a process id");
                syslog(log_facility | LOG_WARNING, "Removing Invalid PID file %s", private_data->my_pid_file_name);
                remove_my_pid_file();

                write_message("LiveStream Viewer has been tampered.");

                if (fclose(private_data->pid_file_pointer) == EOF) {
                    syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", private_data->my_pid_file_name);
                }
                // Since the tampered PID file was already deleted, assume that it doesn't exist.
                return false;
            }
        }

        my_pid = 0;
        fseek(private_data->pid_file_pointer, 0, SEEK_SET);
        fscanf(private_data->pid_file_pointer, "%d", &my_pid);

        // Check if the PID file contains a valid process ID of the LiveStream process.
        // If the PID file is indeed all digits, we must then make sure that it represents a valid process ID.
        // kill() with a signal of 0, doesn't send a signal to the process, but it checks if that process exists.
        // If that process doesn't exist, then kill() will return -1 and errno will be set.
        // This only works if you're the user owning that process, or if you are the root user.
        if (kill(my_pid, 0) == -1) {
            syslog(log_facility | LOG_ERR, "Error: Invalid PID file : %m");
            syslog(log_facility | LOG_WARNING, "Removing PID file for defunct process %d", my_pid);
            remove_my_pid_file();

            write_message("LiveStream Viewer has been tampered.");

            if (fclose(private_data->pid_file_pointer) == EOF) {
                syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", private_data->my_pid_file_name);
            }
            // Since the tampered PID file was already deleted, assume that it doesn't exist.
            return false;

        // If that process does exist, it means that the LiveStream Viewer is already running, so we can't
        // start a new such process.
        } else {
            if (fclose(private_data->pid_file_pointer) == EOF) {
                syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", private_data->my_pid_file_name);
            }
            return true;
        }
    } else {
        return false;
    }
}


void LiveStream_facade::remove_my_pid_file()
{
    if (unlink(private_data->my_pid_file_name) == -1) {
        syslog(log_facility | LOG_ERR, "Error: Could not unlink PID file %s : %m", private_data->my_pid_file_name);
        syslog(log_facility | LOG_WARNING, "System Admin please take a look at this.");
    }
}


void LiveStream_facade::become_livestream_process()
{
    // The process does yet another fork.
    // The parent process (original child) dies, making the forked process a grandchild.
    // By having the first child process die, we've severed the relationship between the
    // child process and the original parent process.
    // In order to become a background process, you need to detatch that process from the shell.
    // The LiveStream viewer process has to be a grandchild of the shell process, not just a child.
    // That is because the shell cannot reap grandchildren, only children.
    if (fork() != 0) {
        exit(EXIT_SUCCESS);
    }

    closelog();
    openlog("SmartCCTV_LiveStream_Viewer", LOG_PID, log_facility);
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
        syslog(log_facility | LOG_CRIT, "LiveStream Viewer unexpected failure.");
        write_message("LiveStream Viewer unexpected failure.");
        exit_code = EXIT_FAILURE;
        terminate_livestream(0);
    }

    // Then we have to reopen stdin, stdout, and stderr because these three file descriptors
    // have to be open by the POSIX standard.
    // Open all three to /dev/null, which is a black hole.
    // This is because the daemon is detatched from the terminal, it cannot communicate via the
    // standard file descriptors.
    open("/dev/null", O_RDWR);  // fd 0  STDIN
    open("/dev/null", O_RDWR);  // fd 1  STDOUT
    open("/dev/null", O_RDWR);  // fd 3  STDERR

    // Write the PID of the LiveStream Viewer process to it's PID file.
    my_pid = getpid();
    fprintf(private_data->pid_file_pointer, "%d", my_pid);
    fclose(private_data->pid_file_pointer);
    close(private_data->pid_file_descriptor);

    // This sets up the signal handler for when the process is terminated.
    struct sigaction action1;
    action1.sa_handler = terminate_livestream;
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

    // This sets up the signal handler for when the SmartCCTV camera daemon starts up.
    struct sigaction action2;
    action2.sa_handler = camera_daemon_starts_up;
    sigemptyset(&action2.sa_mask);
    sigaddset(&action2.sa_mask, SIGTSTP);
    sigaddset(&action2.sa_mask, SIGHUP);
    sigaddset(&action2.sa_mask, SIGUSR1);
    sigaddset(&action2.sa_mask, SIGUSR2);
    sigaddset(&action2.sa_mask, SIGCONT);
    sigaddset(&action2.sa_mask, SIGCHLD);
    action2.sa_flags = 0;
    sigaction(SIGUSR1, &action2, nullptr);

    // This sets up the signal handler for when the SmartCCTV camera daemon shuts down.
    struct sigaction action3;
    action3.sa_handler = camera_daemon_shuts_down;
    sigemptyset(&action3.sa_mask);
    sigaddset(&action3.sa_mask, SIGTSTP);
    sigaddset(&action3.sa_mask, SIGHUP);
    sigaddset(&action3.sa_mask, SIGUSR1);
    sigaddset(&action3.sa_mask, SIGUSR2);
    sigaddset(&action3.sa_mask, SIGCONT);
    sigaddset(&action3.sa_mask, SIGCHLD);
    action3.sa_flags = 0;
    sigaction(SIGUSR2, &action3, nullptr);

    open_viewer_window();
}


void LiveStream_facade::open_viewer_window()
{
    syslog(log_facility | LOG_NOTICE, "The LiveStream Viewer has started running.");

    // Send a signal to the daemon only after you're sure that you can get the pid out,
    // that is has not been tampered.
    int daemon_pid = get_daemon_pid();
    liveStream_viewer_data.daemon_process_pid = (daemon_pid != -1) ? daemon_pid : 0;
    if (liveStream_viewer_data.daemon_process_pid) {
        // The SmartCCTV camera daemon recieves SIGUSR1 when the LiveStream viewer starts up
        // to start saving live stream images into the directory.
        kill(liveStream_viewer_data.daemon_process_pid, SIGUSR1);
    }

    // Check the LiveStream Viewer can:
    // - get the daemon's PID to send it signals
    // - find the camera directory to open images
    liveStream_viewer_data.SmartCCTV_daemon_is_running = liveStream_viewer_data.daemon_process_pid && find_camera_directory();

    LiveStream_window liveStream_window(private_data->streamDir, private_data->default_images_dir, private_data->SmartCCTV_daemon_is_running);
    liveStream_window_ptr = &liveStream_window;

    liveStream_window.open();

    while (true) {
        syslog(log_facility | LOG_NOTICE, "LiveStream Viewer is up and running.");
        sleep(5);
    }
}


bool find_camera_directory()
{
    // Try to find if the camera folder exists.
    // If it exists, it can be opened by the LiveStream_window.

    // Check if the streamDir already has the camera directory appended to the end.
    const char* const streamDir_cstr = liveStream_viewer_data.streamDir.c_str();
    size_t streamDir_len = liveStream_viewer_data.streamDir.length();
    if (strncmp("camera", streamDir_cstr + (streamDir_len - 7), 6) == 0) {
        return true;
    }

    bool camera_folder_found = false;

    if (auto dir = opendir(liveStream_viewer_data.streamDir.c_str())) {
        while (auto f = readdir(dir)) {
            if (!f->d_name || f->d_name[0] == '.') {
                continue;  // Skip everything that starts with a dot
            } else if (strncmp("camera", f->d_name, 6) == 0) {
                liveStream_viewer_data.streamDir += f->d_name;
                camera_folder_found = true;
                break;
            }
        }
        closedir(dir);
    }

    return camera_folder_found;
}


int get_daemon_pid()
{
    // The name of the SmartCCTV camera daemon's PID file.
    const char* daemon_pid_file_name = "/tmp/SmartCCTV_daemon_pid";
    FILE* daemon_pid_file_pointer = nullptr;

    // Try to open the PID file, check if the SmartCCTV camera daemon is already running.
    if ( (daemon_pid_file_pointer = fopen(daemon_pid_file_name, "r")) != nullptr) {
        syslog(log_facility | LOG_NOTICE, "A PID file of SmartCCTV Daemon already exists.");
        syslog(log_facility | LOG_NOTICE, "Checking if the SmartCCTV Daemon is running.");

        char digit = '\0';
        // Check if the PID file contains a valid process ID of the SmartCCTV Daemon.
        // Check if all the characters in the PID file are digits.
        // Check if the PID file was not tampered.
        while (true) {
            digit = fgetc(daemon_pid_file_pointer);
            if (digit == EOF)
                break;

            if (digit == '\0' || digit == '\n')
                continue;

            if (!isdigit(digit)) {
                syslog(log_facility | LOG_ERR, "Error: Invalid PID file: text is not a process id");
                syslog(log_facility | LOG_ERR, "Invalid PID file %s", daemon_pid_file_name);

                write_message("Can't open LiveStream Viewer: SmartCCTV has been tampered.");

                if (fclose(daemon_pid_file_pointer) == EOF) {
                    syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_pid_file_name);
                }

                exit_code = EXIT_FAILURE;
                terminate_livestream(0);
            }
        }

        int daemon_pid = 0;
        fseek(daemon_pid_file_pointer, 0, SEEK_SET);
        fscanf(daemon_pid_file_pointer, "%d", &daemon_pid);

        // Check if the PID file contains a valid process ID of the SmartCCTV camera daemon.
        // If the PID file is indeed all digits, we must then make sure that it represents a valid process ID.
        // kill() with a signal of 0, doesn't send a signal to the process, but it checks if that process exists.
        // If that process doesn't exist, then kill() will return -1 and errno will be set.
        // This only works if you're the user owning that process, or if you are the root user.
        if (kill(daemon_pid, 0) == -1) {
            syslog(log_facility | LOG_ERR, "Error: Invalid PID file %s : %m", daemon_pid_file_name);
            syslog(log_facility | LOG_WARNING, "Defunct process %d", daemon_pid);

            write_message("Can't open LiveStream Viewer: SmartCCTV has been tampered.");

            if (fclose(daemon_pid_file_pointer) == EOF) {
                syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_pid_file_name);
            }

            exit_code = EXIT_FAILURE;
            terminate_livestream(0);

        // If that process does exist, it means that the SmartCCTV camera daemon exists.
        } else {
            if (fclose(daemon_pid_file_pointer) == EOF) {
                syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", daemon_pid_file_name);
            }
            return daemon_pid;
        }
    } else {
        return -1;
    }
}


#pragma GCC diagnostic pop

