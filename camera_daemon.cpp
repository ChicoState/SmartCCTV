/**
 * File Name:  camera_daemon.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  3/21/20
 *
 * Description:
 * This function contains the definition of the camera_deamon() function,
 * the function that will acutally get executed when the deamon runs.
 * This function should contain the main functionality of the daemon.
 * Put any code you want the daemon to execute in thise function.
 */

#include "camera_daemon.h"
#include <syslog.h>  /* for syslog() */
#include <unistd.h>  /* for sleep() */


void camera_daemon()
{
    syslog(LOG_LOCAL0 | LOG_NOTICE, "The camera daemon has started running.");

    /*
     * TODO: Replace this code below with the actual code for the camera daemon,
     * what you want the camera daemon to do.
     */

    while (true) {
        sleep(5);
        syslog(LOG_LOCAL0 | LOG_NOTICE, "Motion was detected.");
    }
}

