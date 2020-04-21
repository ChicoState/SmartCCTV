/**
 * File Name:   high_level_cctv_daemon_apis.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  4/11/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  4/16/20
 *
 * Description:
 * This file contains definitions of functions of the SmartCCTV Daemon's external API.
 * The high level daemon API, which is the API used by outside code to call the daemon and interact with it.
 */

#include "high_level_cctv_daemon_apis.h"
#include "low_level_cctv_daemon_apis.h"

#include <sys/types.h>
#include <sys/stat.h>   /* for umask(), mode permissions constants */
#include <fcntl.h>      /* for O_* constants, open() */
#include <signal.h>     /* for kill() */
#include <unistd.h>     /* for fork() */
#include <errno.h>      /* for errno */
#include <syslog.h>     /* for syslog() */
#include <cstdio>       /* for fclose() */


extern Daemon_data daemon_data;

void set_daemon_info(const char* home_directory)
{
    daemon_data.home_directory = home_directory;
}


int run_daemon()
{
    // User has requested to start the SmartCCTV daemon.

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
    // Close the PID file in the GUI process only.
    // In the daemon process the PID file remains open.
    if (close(daemon_data.pid_file_descriptor) == -1) {
        syslog(log_facility | LOG_ERR, "Error: GUI process could not close PID file %s : %m", daemon_data.pid_file_name);
    }
    daemon_data.pid_file_pointer = nullptr;

    syslog(log_facility | LOG_NOTICE, "Starting SmartCCTV Daemon");
    return SUCCESS;
}


bool kill_daemon()
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


bool is_daemon_running()
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

