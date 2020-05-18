/**
 * File Name:  motionFilter.hpp
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

#ifndef MOTIONFILTER_HPP
#define MOTIONFILTER_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <unistd.h>

class MotionFilter
{
private:
	cv::Mat oldFrame;
	bool initialized;
	void convertFrame(cv::Mat &frame);
	bool differentFrames(cv::Mat oldFrame, cv::Mat newFrame);
	std::string putFrameInfo(cv::Mat frame, std::string outPut);
public:
	MotionFilter();
	bool runDetection(cv::Mat &frame);
};
#endif
