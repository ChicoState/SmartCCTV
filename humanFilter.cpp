/**
 * File Name:  humanFilter.cpp
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

#include "humanFilter.hpp"

HumanFilter::HumanFilter()
{
	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

bool HumanFilter::runRecognition(cv::Mat &frame)
{
	boxes.clear();
	//The third value is used to set detection threshold (higher = less false positives, more false negatives)
	//Recommended value between 1.3 and 1.7
	hog.detectMultiScale(frame, boxes, 1.7, cv::Size(8,8), cv::Size(), 1.05, 2, false);
	
	if(boxes.size() < 1)
	{
		return false;
	}
	
	for (size_t i = 0; i < boxes.size(); i++)
    {
        cv::Rect &rect = boxes[i];
        
        rect.x += cvRound(rect.width*0.1);
        rect.width = cvRound(rect.width*0.8);
        rect.y += cvRound(rect.height*0.07);
        rect.height = cvRound(rect.height*0.8);
        
        if(true) //placeholder- replace with if(daemon_data.enable_boxes) once daemon_data API is ready
        {
			rectangle(frame, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2);
		}
    }
	return true;
}
