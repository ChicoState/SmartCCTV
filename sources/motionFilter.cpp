/**
 * File Name:  motionFilter.cpp
 * Created By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  5/17/20
 *
 * Modified By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Modified On:  5/18/20
 *
 * Description:
 * This class is used to run motion detection on a Mat object, searching for differences between consecutive frames. 
 * Each instance of this class is to correspond to a single camera or video file.
 */
#include "low_level_cctv_daemon_apis.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <unistd.h>
#include "motionFilter.hpp"
#include <syslog.h>  /* for syslog() */
#define log_facility LOG_LOCAL0

extern Daemon_data daemon_data;

MotionFilter::MotionFilter()
{
	//No initialization currently necessary.
}

//Prepares the frame for motion detection analysis
void MotionFilter::convertFrame(cv::Mat &frame)
{
	cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame, frame, cv::Size(21, 21), 0);
}

bool MotionFilter::differentFrames(cv::Mat oldFrame, cv::Mat newFrame)
{
	cv::Mat frameDifference, frameThreshold;
    std::vector<std::vector<cv::Point>> contours;
    
	/**
	* Difference between pixels is used to detect "motion"
	* contours is used to hold the contour of a motion area
	* a contour's area is used to determine the scale of the motion
	**/
	cv::absdiff(oldFrame, newFrame, frameDifference);
	cv::threshold(frameDifference, frameThreshold, 25.0, 255.0, cv::THRESH_BINARY);
	cv::dilate(frameThreshold, frameThreshold, cv::Mat(), cv::Point(-1,-1), 2);
	cv::findContours(frameThreshold, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for(size_t i = 0; i< contours.size(); i++) 
	{
		if(cv::contourArea(contours[i]) > 10)
		{
			return true;
		}
	}
			
	return false;
}

bool MotionFilter::runDetection(cv::Mat newFrame)
{
	//Algorithm skips the first frame
	if(!initialized)
	{
		oldFrame = newFrame;
		convertFrame(oldFrame);
		initialized = true;
		return false;
	}
	
	convertFrame(newFrame);
	if(differentFrames(oldFrame, newFrame))
	{
		oldFrame = newFrame.clone();
		return true;
	}
	oldFrame = newFrame;
	return false;
}
