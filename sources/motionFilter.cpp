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
	initialized = false;
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

std::string MotionFilter::putFrameInfo(cv::Mat frame, std::string outPut)
{
	outPut.append(std::to_string(frame.rows));
	outPut.append(" ");
	outPut.append(std::to_string(frame.cols));
	outPut.append(" ");
	outPut.append(std::to_string(frame.depth()));
	outPut.append(" ");
	outPut.append(std::to_string(frame.channels()));
	outPut.append(" ");
	outPut.append(std::to_string(frame.type()));
	return outPut;
}

bool MotionFilter::runDetection(cv::Mat &frame)
{
	cv::Mat newFrame = frame.clone();
	convertFrame(newFrame);
	//Algorithm skips the first frame
	if(!initialized)
	{
		oldFrame = newFrame;
		initialized = true;
		return false;
	}
	
	//putText(frame, putFrameInfo(frame, "Rcv Frame: "), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,255),2);
	//putText(frame, putFrameInfo(newFrame, "New Frame: "), cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,255),2);
	//putText(frame, putFrameInfo(oldFrame, "Old Frame: "), cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,255),2);
	if(differentFrames(oldFrame, newFrame))
	{
		oldFrame = newFrame;
		if(daemon_data.enable_outlines)
		{
			putText(frame, "+", cv::Point(12, 24), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,255),2);
		}
		return true;
	}
	oldFrame = newFrame;
	if(daemon_data.enable_outlines)
	{
		putText(frame, "-", cv::Point(12, 24), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,255),2);
	}
	return false;
}
