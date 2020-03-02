/**
 * File Name:  MotionFilter.cpp
 * Created By:  Slavik Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  2/29/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 *.Used for human detection algorithms.
 */

HumanFilter::HumanFilter()
{
    descriptor.setSVMDetector(HogDescriptor::getDefaultPeopleDetector());
}

bool HumanFilter::detectPeople(const Mat &frame, vector<Rect> &boxes, vector<Rect> &weights)
{
   descriptor.detectMultiScale(frame, boxes, weights);
   return (boxes.size() > 0);
}
