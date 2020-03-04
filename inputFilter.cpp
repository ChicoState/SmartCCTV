/**
 * File Name:  inputFilter.cpp
 * Created By:  Slavik Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  2/29/20
 *
 * Modified By: < >
 * Modified On:
 *
 * Description:
 *.Opens a video stream and applies a motion detection and human recognition filter
 * Only stores output if both motion and humans are detected
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <motionFilter.cpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

int main(int argc; char* argv[])
{
   const bool debug = true;
   int frameCount = 0;
   bool showOutput = false;
   Mat inFrame, outFrame;

   HumanFilter humanFilter;
   vector<Rect> humanBoxes;
   vector<double> weights;

   vector<Rect> motionBoxes;

   //video(0) opens a video stream from a camera.
   //video("filename.mp4") will play from a file instead
   //PLEASE RESTORE TO 0 BEFORE YOU PUSH!!!
   VideoCapture video(0);
   if(!video.isOpened())
   {
      cout << "Could not open video stream" << endl;
      return 1;
   }

   video >> inFrame;
   while(!inFrame.empty())
   {
      if(debug) { cout << endl << "Frame " << frameCount << endl; }

      humanBoxes.clear();
      weights.clear();
      motionBoxes.clear();
      
      if(humanFilter.detectPeople(inFrame, humanBoxes, weights))
      {
         if(debug)
         {
            for(size_t i = 0; i < humanBoxes.size(); i++)
            {
               cout << humanBoxes[i].x << ", " << humanBoxes[i].y << ", " << weights[i] << endl;
            }
            cout << "Total humans found: " << humanBoxes.size() << endl;
         }

      }
      if(showOutput)
      {
         outFrame = inFrame.clone();
         for(size_t i = 0; i < humanBoxes.size(); i++)
         {
            rectangle(outFrame, humanBoxes[i], Scalar(0, 0, 255), 3);
         }
         imshow("Output", outFrame);
      }
      frameCount++;
   }

   destroyAllWindows();
   return 0;
}
