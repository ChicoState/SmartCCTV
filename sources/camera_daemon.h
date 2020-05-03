/**
 * File Name:  camera_daemon.h
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  3/21/20
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


#endif  /* CAMERA_DAEMON_H */

