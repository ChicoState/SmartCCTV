/**
 * File Name:  MotionFilter.hpp
 * Created By:  Slavik Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  2/29/2020
 *
 * Modified By:  Konstantin Rebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/03/2020
 *
 * Description:
 *.Used for motion detection algorithms.
 */
#ifndef MOTIONFILTER_HPP
#define MOTIONFILTER_HPP

#include <opencv2/core/types.hpp>        /* for cv::Rect */
#include <opencv2/tracking/tracker.hpp>  /* for cv::Tracker */
#include <opencv2/core/mat.hpp>          /* for cv::Mat */

using cv::Rect;
using cv::Tracker;
using cv::Mat;


class MotionFilter
{
public:
   MotionFilter();
   // DO NOT use new keyword in C++ as a variable name!
   Rect detect(Mat old_one, Mat new_one);
private:
   //Tracker tracker;
   Mat newFrame, oldFrame;

};

#endif  /* MOTIONFILTER_HPP */

