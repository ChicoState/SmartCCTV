#include "high_level_cctv_daemon_apis.h"

#include <getopt.h>     /* for getopt_long() */
#include <cstdlib>      /* for abort(), exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <iostream>     /* for ostream, cout, cerr, clog, endl */

using std::cin;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::ostream;


const char* program_name = nullptr;

/**
 * This function prints the usage information to ostream& os and then exits the program
 * with EXIT_STATUS.
 */
void print_usage(ostream& os, int EXIT_STATUS)
{
    os << "Usage:\n" << program_name << " options" << endl;
    os << "valid options are:" << endl;
    os << "  -h  --help   Display this usage information." << endl;
    os << "  -s  --start  Start the SmartCCTV Daemon." << endl;
    os << "  -x  --stop   Kill the SmartCCTV Daemon." << endl;
    os.flush();
    exit(EXIT_STATUS);
}


int main(int argc, char* argv[])
{
    // Remember the name of the program, to incorporate in messages.
    // The name is stored in argv[0];
    program_name = argv[0];

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
	    // User has requested to start the SmartCCTV daemon.
        run_daemon();
        break;
	case 'x':  // -x or --stop
	    // User has requested to stop the SmartCCTV daemon.
        kill_daemon();
	    break;
	case '?':  // The user specified an invalid option.
	case -1:   // There are no options.
	    // Print usage information to standard error, and exit the program with abnormal termination.
	    print_usage(cerr, EXIT_FAILURE);
	    break;
	default:  // Something else: unexpected.
	    // Kill the process.
	    abort();
    };

    exit(EXIT_SUCCESS);
}

