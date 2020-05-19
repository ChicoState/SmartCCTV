/**
 * File Name:  faceFilter.cpp
 * Created By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  5/15/20
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/18/20
 *
 * Description:
 * This class is used to run image recogntition on a Mat object, searching for faces in the frame.
 * Each instance of this class is to correspond to a single camera or video file.
 */
 
#include "write_message.h"
#include "low_level_cctv_daemon_apis.h"
#include "faceFilter.hpp"
#include <syslog.h>  /* for syslog() */
#include <cstdlib>   /* for getenv(), EXIT_FAILURE */
#include <string>    /* for std::string */

using std::string;

#define log_facility LOG_LOCAL0

extern Daemon_data daemon_data;

FaceFilter::FaceFilter()
{
    const string error_message = "Cannot find cascade.xml for FaceFilter";

    const char* SmartCCTV_Project_dir = nullptr;
    SmartCCTV_Project_dir = getenv("SmartCCTV_Project_dir");
    if (SmartCCTV_Project_dir == nullptr) {
        //Error state! Exit the daemon
        syslog(log_facility | LOG_ERR, "Error: $SmartCCTV_Project_dir environmental varaible not set : failed to identify project directory");
        syslog(log_facility | LOG_CRIT, "%s", error_message.c_str());
        write_message("Cannot find project configuration files.");
        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    }
    string fullPath = SmartCCTV_Project_dir;
    fullPath.append("/cascade.xml");
	
	if (!cascade.load(fullPath))
    {
        //Error state! Exit the daemon
        syslog(log_facility | LOG_ERR, "Could not open %s", fullPath.c_str());
        syslog(log_facility | LOG_CRIT, "%s", error_message.c_str());
        write_message("Cannot find project configuration files.");
        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    }
}

bool FaceFilter::runRecognition(cv::Mat &frame)
{
    boxes.clear();
    cv::Mat gray, smallImg;

    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    equalizeHist(gray, gray);
    cascade.detectMultiScale(gray, boxes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    
    if(boxes.size() < 1)
    {
		return false;
	}
    
    if(daemon_data.enable_outlines)
    {
		for(size_t i = 0; i < boxes.size(); i++)
		{
			cv::Rect &rect = boxes[i];        
			rect.x += cvRound(rect.width*0.1);
			rect.width = cvRound(rect.width*0.8);
			rect.y += cvRound(rect.height*0.07);
			rect.height = cvRound(rect.height*0.8);
			rectangle(frame, rect.tl(), rect.br(), cv::Scalar(255, 0, 0), 2);
		}
	}
    
    return true;
}
