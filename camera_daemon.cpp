/**
 * File Name:  camera_daemon.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 * This function contains the definitions of the deamon functions,
 * the functions that will acutally get executed when the deamon runs.
 * These functions should contain the main functionality of the daemon.
 * Put any code you want the daemon to execute in these functions.
 */

#include "camera_daemon.h"
#include <syslog.h>  /* for syslog() */
#include <unistd.h>  /* for sleep() */


void camera_daemon()
{
    syslog(LOG_LOCAL0 | LOG_NOTICE, "The camera daemon is starting up.");

    // Put code for the camera daemon here.
    sleep(1800);
}


void listener_daemon()
{
    syslog(LOG_LOCAL0 | LOG_NOTICE, "The listener daemon is starting up.");
    sleep(1800);
}

