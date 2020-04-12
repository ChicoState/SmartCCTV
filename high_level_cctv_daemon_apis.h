/**
 * File Name:   high_level_cctv_daemon_apis.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  4/11/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  4/12/20
 *
 * Description:
 * This file contains declarations of functions of the SmartCCTV Daemon's external API.
 * The high level daemon API, which is the API used by outside code to call the daemon and interact with it.
 */

#ifndef HIGH_LEVEL_CCTV_DAEMON_APIS_H
#define HIGH_LEVEL_CCTV_DAEMON_APIS_H


/**
 * This function turns on the daemon if it is not already running.
 *
 * @return bool - true if it succeeded running the daemon
 *                false if it did not succeed
 *         There are two reasons why it could'nt have succeeded:
 *           - Either the daemon was already running OR
 *           - it failed to create and/or open the PID file.
 */
bool run_daemon();


/**
 * This function kills the daemon if it is already running.
 *
 * @return bool - true if it succeeded killing the daemon
 *                false if it did not succeed
 *         The only reason it did not succeed is because the deamon was not already running.
 */
bool kill_daemon();


/**
 * @return bool - true if the daemon is already running
 *                false if the daemon is not already running
 */
bool is_daemon_running();


#endif  /* HIGH_LEVEL_CCTV_DAEMON_APIS_H */

