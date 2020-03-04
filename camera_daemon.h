/**
 * File Name:  camera_daemon.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 * This function contains the headers of the deamon functions,
 * the functions that will acutally get executed when the deamon runs.
 * These functions should contain the main functionality of the daemon.
 * Put any code you want the daemon to execute in these functions.
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
 * This function is run when the listener daemon starts up.
 * The listener daemon remains in this function for it's entire life time.
 * It never returns. The way this would be achieved is using a forever loop.
 * The daemon runs in the background forever, unless it gets shut down from
 * the command line.
 *
 * Put any code that you want the listener daemon to execute in this function.
 */
void listener_daemon();

#endif  /* CAMERA_DAEMON_H */

