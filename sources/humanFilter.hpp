/**
 * File Name:  humanFilter.hpp
 * Created By:  Svyatoslav Chukhlebov <slavikchukhlebov@mail.csuchico.edu>
 * Created On:  4/25/20
 *
 * Modified By:  Svyatoslav Chukhlebov <slavikchukhlebov@mail.csuchico.edu>
 * Modified On:  4/29/20
 *
 * Description:
 * This class is used to run image recogntition on a Mat object, searching for humans in the frame.
 * Each instance of this class is to correspond to a single camera or video file.
 */

#ifndef HUMANFILTER_HPP
#define HUMANFILTER_HPP

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <vector>
#include <iomanip>

class HumanFilter
{
public:
	HumanFilter();
	bool runRecognition(cv::Mat &frame);
    
private:
	cv::HOGDescriptor hog;
	std::vector<cv::Rect> boxes;
};
#endif
