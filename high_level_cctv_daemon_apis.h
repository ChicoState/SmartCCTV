/**
 * File Name:   high_level_cctv_daemon_apis.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  4/11/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  4/13/20
 *
 * Description:
 * This file contains declarations of functions of the SmartCCTV Daemon's external API.
 * The high level daemon API, which is the API used by outside code to call the daemon and interact with it.
 */

#ifndef HIGH_LEVEL_CCTV_DAEMON_APIS_H
#define HIGH_LEVEL_CCTV_DAEMON_APIS_H

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


/**
 * This function is used for passing some winformation from the MainWindow to the daemon.
 *
 * This function is called only in the GUI process.
 *
 * @param const char* home_directory - The MainWindow class gives the daemon information about the $HOME directory.
 */
void set_daemon_info(const char* home_directory);


/**
 * This function turns on the daemon if it is not already running.
 *
 * This function is called only in the GUI process.
 *
 * @return int - 0 if it succeeded running the daemon
 *               1 if it failed because the daemon was already running
 *               2 if it failed because you don't have permissions to run the daemon
 *                              (it failed to create and/or open the PID file)
 */
int run_daemon();


/**
 * This function kills the daemon if it is already running.
 *
 * This function is called only in the GUI process.
 *
 * @return bool - true if it succeeded killing the daemon
 *                false if it did not succeed
 *         The only reason it did not succeed is because the deamon was not already running.
 */
bool kill_daemon();


/**
 * This function is called only in the GUI process.
 *
 * @return bool - true if the daemon is already running
 *                false if the daemon is not already running
 */
bool is_daemon_running();


#endif  /* HIGH_LEVEL_CCTV_DAEMON_APIS_H */

