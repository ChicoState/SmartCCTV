/**
 * File Name:  faceFilter.hpp
 * Created By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  5/15/20
 *s
 * Modified By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Modified On:  5/17/20
 *
 * Description:
 * This class is used to run image recogntition on a Mat object, searching for faces in the frame.
 * Each instance of this class is to correspond to a single camera or video file.
 */

#ifndef FACEFILTER_HPP
#define FACEFILTER_HPP

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <vector>
#include <iomanip>

class FaceFilter
{
public:
	FaceFilter();
	bool runRecognition(cv::Mat &frame);
    
private:
	cv::CascadeClassifier cascade;
	std::vector<cv::Rect> boxes;
};
#endif
