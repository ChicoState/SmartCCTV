/**
 * File Name:  cctv_daemon_apis.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  2/27/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 * This file contains declarations of functions of the SmartCCTV Daemon's internal API.
 * These functions are mainly concerned with setting up the daemon.
 */
#ifndef CCTV_DAEMON_APIS_H
#define CCTV_DAEMON_APIS_H

#include <sys/types.h>
#include <sys/stat.h>   /* for umask(), mode permissions constants */
#include <sys/wait.h>   /* for wait() */
#include <fcntl.h>      /* for O_* constants, open() */
#include <signal.h>     /* for kill() */
#include <unistd.h>     /* for close(), unlink(), fork(), setsid(), sysconf(), chdir(), getpid() */
#include <getopt.h>     /* for getopt_long() */
#include <errno.h>      /* for errno */
#include <syslog.h>     /* for openlog(), syslog(), closelog() */
#include <cstdlib>      /* for exit(), atexit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <cstdio>       /* for perror(), fopen(), fclose(), fseek(), fgetc(), fscanf(), fprintf() */
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


/**
 * This struct contains all the data of the daemon that might need to be accessed globally.
 * It is done like this because signal handler functions maybe called, which do not accept
 * custom parameters.
 */
struct Daemon_data {
    /* The name of the program. */
    const char* program_name;
    /* The path to the PID file. */
    const char* pid_file_name;
    int pid_file_descriptor;
    FILE* pid_file_pointer;
    int camera_daemon_pid;
    int listener_daemon_pid;
};


/**
 * This function prints the usage information to ostream& os and then exits the program
 * with EXIT_STATUS.
 */
void print_usage(ostream& os, int EXIT_STATUS);


/**
 * This function deletes the PID file.
 * It first closes the passed in FILE* and then it unlinks the actual file.
 *
 * @param FILE* pid_file_pointer - A pointer to the PID file.
 */
void remove_pid_file(FILE* pid_file_pointer);


/**
 * This function checks if the PID file exists and if it is valid.
 * If the PID file is invalid (either it does not contain a process ID or there is no such process)
 * then an error message is printed to the screen and the program is exited.
 *
 * If either the PID file does not exist, or it exists and it is valid,
 * a bool value is returned depeding on bool turn_on.
 *
 * @param bool turn_on - This is a command to turn the daemon on, or turn it off.
 *             turn_on == true if the daemon is being turned on.
 *             turn_on == false if the daemon is being turned off.
 *        If you want to turn the daemon on, and the daemon is already on, the function returns false,
 *        else it returns true.
 *        If you want to turn the daemon off, and the daemon is already off (not running), the runction returns false,
 *        else it returns true.
 *
 * @return bool - It returns false if the command specified cannot be completed.
 *                It returns true if the command specified can be completed.
 *                If there is an error reading the PID file, this function does not return, instead it
 *                terminates the program.
 */
bool checkPidFile(bool turn_on);


/**
 * This function turns the calling process into the camera daemon, and it forks the listener daemon off of it.
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
 * - call camera_daemon() function inside of which the daemon will be in it's entire lifetime.
 */
void becomeDaemon();


/**
 * This is a signal handler that gets activated whenever the camera daemon recieves the following signals:
 * - SIGINT
 * - SIGTERM
 * - SIGQUIT
 *
 * This function terminates the listener daemon.
 * It closes and removes the PID file.
 * Then it terminates the camera daemon.
 */
void terminate_daemon(int);


/**
 * This is a signal handler that gets activated whenever the camera daemon recieves SIGCHLD.
 * So the SIGCHLD signal gets send whenever the listener process exits.
 *
 * This function promptly reaps the zombie child.
 * It closes and removes the PID file.
 * Then it terminates the camera daemon with the exit status of the listener process.
 */
void reap_child(int);

#endif  /* CCTV_DAEMON_APIS_H */

