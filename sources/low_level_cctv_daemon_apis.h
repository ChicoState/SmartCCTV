/**
 * File Name:   low_level_cctv_daemon_apis.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  2/27/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/18/20
 *
 * Description:
 * This file contains declarations of functions of the SmartCCTV Daemon's internal API.
 * These functions are mainly concerned with setting up the daemon.
 */
#ifndef LOW_LEVEL_CCTV_DAEMON_APIS_H
#define LOW_LEVEL_CCTV_DAEMON_APIS_H

#include <cstdio>       /* for FILE */

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


/**
 * This struct contains all the data of the daemon that might need to be accessed globally.
 * It is done like this because signal handler functions maybe called, which do not accept
 * custom parameters.
 */
struct Daemon_data {
    const char* pid_file_name;     // The path to the PID file.
    int pid_file_descriptor;       // A descriptor to this file.
    FILE* pid_file_pointer;        // A pointer to this file.
    int camera_daemon_pid;         // The PID of the daemon.
    const char* home_directory;    // The path to the home directory, $HOME.
    bool enable_human_detection;   // whether to enable human detection
    bool enable_motion_detection;  // whether to enable motion detection
    bool enable_outlines;          // whether to draw outlines
    bool is_live_stream_running;   // is live stream viewer process currently running
    int live_stream_viewer_pid;    // The PID of the LiveStreamViewer
    int cameraNumber;              // An integer identifying which camera to use
    int daemon_exit_status;        // The exit status of the daemon, to use in terminate_daemon(), assumed EXIT_SUCCESS.
};


/**
 * This function turns the calling process into the camera daemon.
 *
 * This function is called only in the daemon process.
 *
 * Becoming a daemon requires the following steps:
 * - forking itself once and killing the parent process to sever the connection between itself and the shell.
 * - resets the session ID and the process group ID.
 * - close all open file descriptors and connect the standard file descriptors into /dev/null
 * - move the daemon process into the root directory.
 * - reset the umask to a known value.
 * - reset the environmental variables to a known value.
 * - write the PID of the daemon process into the PID file.
 * - setup the logging for communication with the outside world.
 * - setup the signal handler for when the process is terminated.
 * - call camera_daemon() function inside of which the daemon will be in it's entire lifetime.
 */
void becomeDaemon();


/**
 * This is a signal handler that gets activated whenever the camera daemon recieves the following signals:
 * - SIGINT
 * - SIGTERM
 * - SIGQUIT
 *
 * This function closes and removes the PID file.
 * Then it terminates the camera daemon.
 *
 * This function is called only in the daemon process.
 */
void terminate_daemon(int);


#endif  /* LOW_LEVEL_CCTV_DAEMON_APIS_H */

