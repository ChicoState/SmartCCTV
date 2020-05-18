/**
 * File Name:  camera_daemon.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/17/20
 *
 * Description:
 * This function contains the definition of the camera_deamon() function,
 * the function that will acutally get executed when the deamon runs.
 * This function should contain the main functionality of the daemon.
 * Put any code you want the daemon to execute in thise function.
 */


#include "camera_daemon.h"
#include "low_level_cctv_daemon_apis.h"
#include "camera.hpp"
#include "write_message.h"

#include <sys/types.h>
#include <signal.h>  /* for sigemptyset(), kill(), signal constants */
#include <syslog.h>  /* for syslog() */
#include <unistd.h>  /* for sleep() */
#include <vector>    /* for std::vector */

using std::vector;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"


extern Daemon_data daemon_data;
extern vector<Camera*> cameras;

const int cameraNumber = 0;

void camera_daemon()
{
    syslog(log_facility | LOG_NOTICE, "The camera daemon has started running.");

    syslog(log_facility | LOG_NOTICE, "enable human detection: %d", daemon_data.enable_human_detection);
    syslog(log_facility | LOG_NOTICE, "enable motion detection: %d", daemon_data.enable_motion_detection);
    syslog(log_facility | LOG_NOTICE, "enable outlines: %d", daemon_data.enable_outlines);

    daemon_data.is_live_stream_running = check_live_stream();
    syslog(log_facility | LOG_NOTICE, "daemon_data.is_live_stream_running = %d", daemon_data.is_live_stream_running);
    syslog(log_facility | LOG_NOTICE, "daemon_data.live_stream_viewer_pid = %d", daemon_data.live_stream_viewer_pid);

    // This sets up the signal handler for when the LiveStream Viewer starts up.
    struct sigaction action2;
    action2.sa_handler = livestream_viewer_starts_up;
    sigemptyset(&action2.sa_mask);
    sigaddset(&action2.sa_mask, SIGTSTP);
    sigaddset(&action2.sa_mask, SIGHUP);
    sigaddset(&action2.sa_mask, SIGUSR1);
    sigaddset(&action2.sa_mask, SIGUSR2);
    sigaddset(&action2.sa_mask, SIGCONT);
    sigaddset(&action2.sa_mask, SIGCHLD);
    action2.sa_flags = 0;
    sigaction(SIGUSR1, &action2, nullptr);

    // This sets up the signal handler for when the LiveStream Viewer shuts down.
    struct sigaction action3;
    action3.sa_handler = livestream_viewer_shuts_down;
    sigemptyset(&action3.sa_mask);
    sigaddset(&action3.sa_mask, SIGTSTP);
    sigaddset(&action3.sa_mask, SIGHUP);
    sigaddset(&action3.sa_mask, SIGUSR1);
    sigaddset(&action3.sa_mask, SIGUSR2);
    sigaddset(&action3.sa_mask, SIGCONT);
    sigaddset(&action3.sa_mask, SIGCHLD);
    action3.sa_flags = 0;
    sigaction(SIGUSR2, &action3, nullptr);

    Camera cam(cameraNumber);
    cameras.push_back(&cam);
    // The LiveStream process recieves SIGUSR1 when the daemon starts up.
    if (daemon_data.live_stream_viewer_pid) {
        kill(daemon_data.live_stream_viewer_pid, SIGUSR1);
    }
    cam.record();
	
    syslog(log_facility | LOG_NOTICE, "The camera daemon has completed running.");

    terminate_daemon(0);
}


void livestream_viewer_starts_up(int)
{
    syslog(log_facility | LOG_NOTICE, "livestream_viewer_starts_up()");
    daemon_data.is_live_stream_running = check_live_stream();
}


void livestream_viewer_shuts_down(int)
{
    syslog(log_facility | LOG_NOTICE, "livestream_viewer_shuts_down()");
    daemon_data.is_live_stream_running = false;
}


bool check_live_stream()
{
    // The name of the LiveStream Viewer's PID file.
    const char* livestream_pid_file_name = "/tmp/LiveStream_viewer_pid";
    FILE* livestream_pid_file_pointer = nullptr;

    // Try to open the PID file, check if the LiveStream Viewer is already running.
    if ( (livestream_pid_file_pointer = fopen(livestream_pid_file_name, "r")) != nullptr) {
        syslog(log_facility | LOG_NOTICE, "A PID file of LiveStream Viewer already exists.");
        syslog(log_facility | LOG_NOTICE, "Checking if the LiveStream Viewer is running.");

        char digit = '\0';
        // Check if the PID file contains a valid process ID of the LiveStream viewer.
        // Check if all the characters in the PID file are digits.
        // Check if the PID file was not tampered.
        while (true) {
            digit = fgetc(livestream_pid_file_pointer);

            if (digit == EOF)
                break;

            if (digit == '\0' || digit == '\n')
                continue;

            if (!isdigit(digit)) {
                syslog(log_facility | LOG_ERR, "Error: Invalid PID file: text is not a process id");
                syslog(log_facility | LOG_ERR, "Invalid PID file %s", livestream_pid_file_name);

                syslog(log_facility | LOG_ERR, "LiveStream PID file has been tampered.");

                if (fclose(livestream_pid_file_pointer) == EOF) {
                    syslog(log_facility | LOG_ERR, "Error: Could not close LiveStream PID file %s : %m", livestream_pid_file_name);
                }

                return false;
            }
        }

        int livestream_pid = 0;
        fseek(livestream_pid_file_pointer, 0, SEEK_SET);
        fscanf(livestream_pid_file_pointer, "%d", &livestream_pid);

        // Check if the PID file contains a valid process ID of the LiveStream Viewer.
        // If the PID file is indeed all digits, we must then make sure that it represents a valid process ID.
        // kill() with a signal of 0, doesn't send a signal to the process, but it checks if that process exists.
        // If that process doesn't exist, then kill() will return -1 and errno will be set.
        // This only works if you're the user owning that process, or if you are the root user.
        if (kill(livestream_pid, 0) == -1) {
            syslog(log_facility | LOG_ERR, "Error: Invalid PID file: defunct process %d", livestream_pid);
            syslog(log_facility | LOG_ERR, "Invalid PID file %s", livestream_pid_file_name);

            syslog(log_facility | LOG_ERR, "LiveStream PID file has been tampered.");

            if (fclose(livestream_pid_file_pointer) == EOF) {
                syslog(log_facility | LOG_ERR, "Error: Could not close LiveStream PID file %s : %m", livestream_pid_file_name);
            }

            return false;

        // If that process does exist, it means that the LiveStream Viewer is running.
        } else {
            if (fclose(livestream_pid_file_pointer) == EOF) {
                syslog(log_facility | LOG_ERR, "Error: Could not close PID file %s : %m", livestream_pid_file_name);
            }
            daemon_data.live_stream_viewer_pid = livestream_pid;
            return true;
        }
    } else {
        return false;
    }
}


#pragma GCC diagnostic pop

