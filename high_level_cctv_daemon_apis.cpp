/**
 * File Name:   high_level_cctv_daemon_apis.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  4/11/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  4/13/20
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
#include <cstdio>       /* for perror(), fclose() */
#include <iostream>     /* for cout, cerr, clog, endl */

using std::cout;
using std::cerr;
using std::clog;
using std::endl;


extern Daemon_data daemon_data;

int run_daemon()
{
    // User has requested to start the SmartCCTV daemon.
    enum return_states { SUCCESS, DAEMON_ALREADY_RUNNING, PERMISSIONS_ERROR };

    if (!checkPidFile(true)) {
        cerr << "Error: A SmartCCTV Daemon is already running with PID " << daemon_data.camera_daemon_pid << endl;
        // Close the PID file, since it was opened in checkPidFile() function.
        if (fclose(daemon_data.pid_file_pointer) == EOF) {
            cerr << "Error: Could not close PID file ";
            perror(daemon_data.pid_file_name);
        }
        //exit(EXIT_FAILURE);
        return DAEMON_ALREADY_RUNNING;
    }

    // Reset the umask so that you have no problems creating the file with the desired permissions.
    umask(0);
    // The process ID file is like a lock file. It must be created before the daemon starts up.
    // Create the process ID file and obtain a handle to it.
    // Do that before you become a daemon, becuase if the file was failed to create,
    // you can't write the PID of the daemon process into that file.
    if ( (daemon_data.pid_file_descriptor = open(daemon_data.pid_file_name, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
        cerr << "Error: Not able to create the file ";
	    perror(daemon_data.pid_file_name);
        //exit(EXIT_FAILURE);
        return PERMISSIONS_ERROR;
    }
    if ( (daemon_data.pid_file_pointer = fdopen(daemon_data.pid_file_descriptor, "w")) == NULL) {
        cerr << "Error: Not able to open the file ";
	    perror(daemon_data.pid_file_name);
        //exit(EXIT_FAILURE);
        return PERMISSIONS_ERROR;
    }
    cout << "PID file created successfully" << endl;

    // At the point where you want to create a daemon, you're going to do a fork().
    // If you're the child, you're going to call this function from which you're never going to return.
    // The child will call this function from which it will never return.
    // It will be inside this function for it's entire life time.
    // The parent will continue on and print a confirmation message to the user.
    if (fork() == 0) {
        becomeDaemon();
    }

    cout << "Starting SmartCCTV Daemon" << endl;
    return SUCCESS;
}


bool kill_daemon()
{
    // User has requested to stop the SmartCCTV daemon.

    if (!checkPidFile(false)) {
        cerr << "Error: A SmartCCTV Daemon not already running." << endl;
        //exit(EXIT_FAILURE);
        return false;
    } else {
        // Close the PID file, since it was opened in checkPidFile() function.
        if (fclose(daemon_data.pid_file_pointer) == EOF) {
            cerr << "Error: Could not close PID file ";
            perror(daemon_data.pid_file_name);
        }
    }

    cout << "Killing SmartCCTV Daemon" << endl;
    cout << "Removing PID file" << endl;
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
            cerr << "Error: Could not close PID file ";
            perror(daemon_data.pid_file_name);
        }

        return true;
    } else {
        // A SmartCCTV Daemon not already running.
        return false;
    }
}

