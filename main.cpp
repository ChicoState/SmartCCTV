#include "camera_daemon.h"
#include "cctv_daemon_apis.h"
#include <sys/types.h>
#include <sys/stat.h>   /* for umask(), mode permissions constants */
#include <fcntl.h>      /* for O_* constants, open() */
#include <signal.h>     /* for kill() */
#include <unistd.h>     /* for fork() */
#include <getopt.h>     /* for getopt_long() */
#include <errno.h>      /* for errno */
#include <cstdlib>      /* for exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <cstdio>       /* for perror(), fclose(), fgetc() */
#include <cctype>       /* for isdigit() */
#include <iostream>     /* for cout, cerr, clog, endl */
#include <string>       /* for string */

using std::cin;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::ostream;
using std::string;

extern Daemon_data daemon_data;


int main(int argc, char* argv[])
{
    // Remember the name of the program, to incorporate in messages.
    // The name is stored in argv[0];
    daemon_data.program_name = argv[0];

    // There should be two command line arguments:
    // The program_name and the single option.
    if (argc != 2) {
        // Print usage information to standard error, and exit the program with abnormal termination.
        print_usage(cerr, EXIT_FAILURE);
    }

    /* Define the valid command line options. */
    // A string listing valid short options letters.
    const char* const short_options = "hsx";
    // An array of structs describing valid long options.
    // option is a data type included in getopt.h
    const option long_options[] = {
        { "help",  0, NULL, 'h' },
	{ "start", 0, NULL, 's' },
	{ "stop",  0, NULL, 'x' },
	{ NULL,    0, NULL, 0 }    // required at end of array
    };

    // This function processes the command line options using the data structures defined above.
    // It returns an int holding a character value identifying the command line option that
    // the user input.
    int next_option = getopt_long(argc, argv, short_options, long_options, NULL);

    switch (next_option) {
        case 'h':  // -h or --help
	    // User has requested usage information.
	    // Print it to standard output, and exit the program with normal termination.
	    print_usage(cout, EXIT_SUCCESS);
	    break;
	case 's':  // -s or --start
	{
	    // User has requested to start the SmartCCTV daemon.

	    if (!checkPidFile(true)) {
	        cerr << "Error: A SmartCCTV Daemon is already running with PID " << daemon_data.camera_daemon_pid << endl;
		// Close the PID file, since it was opened in checkPidFile() function.
                if (fclose(daemon_data.pid_file_pointer) == EOF) {
                    cerr << "Error: Could not close PID file ";
                    perror(daemon_data.pid_file_name);
	        }
	        exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	    }
	    if ( (daemon_data.pid_file_pointer = fdopen(daemon_data.pid_file_descriptor, "w")) == NULL) {
	        cerr << "Error: Not able to open the file ";
		perror(daemon_data.pid_file_name);
		exit(EXIT_FAILURE);
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
	}
	break;
	case 'x':  // -x or --stop
	    // User has requested to stop the SmartCCTV daemon.

	    if (!checkPidFile(false)) {
	        cerr << "Error: A SmartCCTV Daemon not already running." << endl;
	        exit(EXIT_FAILURE);
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

	    break;
	case '?':  // The user specified an invalid option.
	case -1:   // There are no options.
	    // User has started the executable without providing any options.
	    // Print usage information to standard error, and exit the program with abnormal termination.
	    print_usage(cerr, EXIT_FAILURE);
	    break;
	default:  // Something else: unexpected.
	    // Kill the process.
	    abort();
    };

    exit(EXIT_SUCCESS);
}

