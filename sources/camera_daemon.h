/**
 * File Name:  camera_daemon.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/17/20
 *
 * Description:
 * This file contains the header of the camera_deamon() function,
 * the function that will acutally get executed when the deamon runs.
 * This function should contain the main functionality of the daemon.
 * Put any code you want the daemon to execute in this function.
 */
#ifndef CAMERA_DAEMON_H
#define CAMERA_DAEMON_H

/**
 * This function is run when the camera daemon starts up.
 * The camera daemon remains in this function for it's entire life time.
 * It never returns. The way this would be achieved is using a forever loop.
 * The daemon runs in the background forever, unless it gets shut down from
 * the command line.
 *
 * Put any code that you want the camera daemon to execute in this function.
 */
void camera_daemon();


/**
 * The camera daemon process recieves SIGUSR1 when the livestream viewer starts up.
 * This function handles that signal.
 * It tells SmartCCTV Camera to start saving images into the livestream directory.
 */
void livestream_viewer_starts_up(int);


/**
 * The camera daemon process recieves SIGUSR2 when the livestream viewer shuts down.
 * This function handles that signal.
 * It tells SmartCCTV Camera to stop saving images into the livestream directory.
 */
void livestream_viewer_shuts_down(int);


/**
 * This is a helper function for the camera daemon.
 * It checks to se if the LiveStream Viewer process is running or not.
 *
 * if the LiveStream Viewer process is currently running:
 *   daemon_data.live_stream_viewer_pid is set to the PID of the LiveStream Viewer process
 *   @return true
 *
 * if the LiveStream Viewer process is not currently running:
 *   @return false
 */
bool check_live_stream();


#endif  /* CAMERA_DAEMON_H */

