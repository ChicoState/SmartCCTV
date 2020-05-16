/**
 * File Name:  camera_daemon.cpp
 * Created By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Created On:  3/03/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/02/20
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
#include "write_message.h"
#include <syslog.h>  /* for syslog() */
#include <unistd.h>  /* for sleep() */
#include <vector>    /* for std::vector */

using std::vector;


extern Daemon_data daemon_data;
extern vector<Camera*> cameras;

void camera_daemon()
{
    syslog(log_facility | LOG_NOTICE, "The camera daemon has started running.");

    syslog(log_facility | LOG_NOTICE, "enable human detection: %d", daemon_data.enable_human_detection);
    syslog(log_facility | LOG_NOTICE, "enable motion detection: %d", daemon_data.enable_motion_detection);
    syslog(log_facility | LOG_NOTICE, "enable outlines: %d", daemon_data.enable_outlines);

    Camera cam(1);
    cameras.push_back(&cam);
    cam.record();
	
    syslog(log_facility | LOG_NOTICE, "The camera daemon has completed running.");

    terminate_daemon(0);
}


