/**
 * File Name:   high_level_cctv_daemon_apis.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  4/11/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/18/20
 *
 * Description:
 * This file contains declarations of functions of the SmartCCTV Daemon's external API.
 * The high level daemon API, which is the API used by outside code to call the daemon and interact with it.
 */

#ifndef HIGH_LEVEL_CCTV_DAEMON_APIS_H
#define HIGH_LEVEL_CCTV_DAEMON_APIS_H

#include <cstdio>       /* for FILE */

// You can change this to make the syslog() output to a different file.
#define log_facility LOG_LOCAL0


class Daemon_facade {
  public:
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
     * @param bool enable_human_detection - whether to enable human detection
     *
     * @param bool enable_motion_detection - whether to enable motion detection
     *
     * @param bool enable_outlines - whether to draw outlines
     *
     * @aram int cameraNumber - An integer identifying which camera to use.
     *
     * @return int - 0 if it succeeded running the daemon
     *               1 if it failed because the daemon was already running
     *               2 if it failed because you don't have permissions to run the daemon
     *                              (it failed to create and/or open the PID file)
     */
    int run_daemon(bool enable_human_detection, bool enable_motion_detection, bool enable_outlines, int cameraNumber);

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

  private:
    /**
     * This function deletes the PID file.
     * It first closes the passed in FILE* and then it unlinks the actual file.
     *
     * This function is called only in the GUI process.
     *
     * @param FILE* pid_file_pointer - A pointer to the PID file.
     */
    void remove_pid_file(FILE* pid_file_pointer);

    /**
     * This function checks if the PID file exists and if it is valid.
     * If the PID file is invalid (either it does not contain a process ID or there is no such process)
     * then an error message is printed to the screen, and the PID file is removed, after which it is treated
     * as if the PID file did not exist.
     * If the PID file is invalid, then it is assumed that it has been tampererd.
     *
     * If either the PID file does not exist, or it exists and it is valid,
     * a bool value is returned depeding on bool turn_on.
     *
     * This function is called only in the GUI process.
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
     */
    bool checkPidFile(bool turn_on);
};


#endif  /* HIGH_LEVEL_CCTV_DAEMON_APIS_H */

