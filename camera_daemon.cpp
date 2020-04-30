/**
 * File Name:  camera_daemon.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By:  Svyatoslav Chukhlebov <slavikchukhlebov@mail.csuchico.edu>
 * Modified On:  4/29/20
 *
 * Description:
 * This function contains the definition of the camera_deamon() function,
 * the function that will acutally get executed when the deamon runs.
 * This function should contain the main functionality of the daemon.
 * Put any code you want the daemon to execute in thise function.
 */

#include "camera_daemon.h"
#include "low_level_cctv_daemon_apis.h"
#include "camera.hpp"
#include <syslog.h>  /* for syslog() */
#include <unistd.h>  /* for sleep() */


extern Daemon_data daemon_data;

void camera_daemon()
{
	syslog(log_facility | LOG_NOTICE, "The camera daemon has started running.");

	Camera cam(0);
	cam.record();
	
	syslog(log_facility | LOG_NOTICE, "The camera daemon has completed running.");
}

