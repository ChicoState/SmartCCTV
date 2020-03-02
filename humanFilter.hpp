/**
 * File Name:  HumanFilter.hpp
 * Created By:  Slavik Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  2/29/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 *.Used for human detection algorithms.
 */

class HumanFilter
{
public:
   HumanFilter();

   //Populates boxes, weights vectors with possible detections, returns true if found
   bool detectPeople(const Mat &frame, vector<Rect> &boxes, vector<Rect> &weights);

private:
   HogDescriptor descriptor;
}
