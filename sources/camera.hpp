/**
 * File Name:  camera.hpp
 * Created By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Created On:  4/25/20
 *
 * Modified By:  Svyatoslav Chukhlebov <schukhlebov@mail.csuchico.edu>
 * Modified On:  4/29/20
 *
 * Description:
 * This class is used to run image recogntition on a Mat object, searching for humans in the frame.
 * Each instance of this class is to correspond to a single camera or video file.
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <vector>
#include <chrono>
#include <syslog.h>  /* for syslog() */
#include "humanFilter.hpp"
#include "faceFilter.hpp"
#include "motionFilter.hpp"
#define log_facility LOG_LOCAL0

//using namespace std;
//using namespace cv;

struct frameContainer
{
	cv::Mat frame;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

class Camera
{
	public:
	Camera(int cameraID);
	Camera(std::string filePath);
	void record();
    void finalize();
	
	private:
	int cameraID;
	bool recording;
	std::vector<frameContainer> frameBackCapture;
	std::string readFilePath;
	std::string streamDir;
	std::string videoSaveDir;
	std::chrono::time_point<std::chrono::high_resolution_clock> recordingStartTime;
	cv::VideoCapture cap;
	void saveFrameToBuffer(cv::Mat frame);
	void clearExpiredFrames();
	void saveToStream(cv::Mat frame, int x);
	void saveVideo();
	void checkRecordingLength();
	HumanFilter humanFilter;
	FaceFilter faceFilter;
	MotionFilter motionFilter;
	const bool debug = false;
};
#endif
