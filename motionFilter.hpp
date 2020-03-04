/**
 * File Name:  MotionFilter.hpp
 * Created By:  Slavik Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  2/29/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 *.Used for motion detection algorithms.
 */

class MotionFilter
{
public:
   MotionFilter();
   Rect detect(Mat old, Mat new);
private:
   Tracker tracker;
   Mat newFrame, oldFrame;

}
