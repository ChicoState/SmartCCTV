/**
 * File Name:  camera.cpp
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

#include <opencv2/imgcodecs.hpp>
#include "camera.hpp"

Camera::Camera(int cameraID)
{
	this->cameraID = cameraID; 
	readFilePath = "";
    recording = false;
    streamDir = "/tmp/SmartCCTV_livestream/camera" + std::to_string(cameraID) + "/"; //Placeholder- replace the string literal address with daemon_data.home_directory
	videoSaveDir = "/home/slavik/SmartCCTV/camera" + std::to_string(cameraID) + "/"; //Placeholder- replace with "daemon_data.home_directory"
	//std::cout << "Prepared to open camera." << std::endl;
	cap.open(cameraID);
	if (!cap.isOpened())
    {
		//daemon_data.daemon_exit_status = EXIT_FAILURE;
		//terminate_daemon(0);
		return; //Placeholder
    }
}

Camera::Camera(std::string readFilePath)
{
	this->readFilePath = readFilePath; 
	cameraID = -1;
    recording = false;
    streamDir = "/tmp/SmartCCTV_livestream/camera" + std::to_string(cameraID) + "/"; //Placeholder- replace the string literal address with daemon_data.home_directory
    videoSaveDir = "/home/slavik/SmartCCTV/camera" + std::to_string(cameraID) + "/"; //Placeholder- replace with "daemon_data.home_directory"
    
    cap.open(readFilePath);
	if (!cap.isOpened())
    {
		//daemon_data.daemon_exit_status = EXIT_FAILURE;
		//terminate_daemon(0);
		return;//Placeholder
    }
}

void Camera::clearExpiredFrames()
{
	auto now = std::chrono::high_resolution_clock::now();
	int x = 0;
	for(size_t i = 0; i < frameBackCapture.size(); i++)
	{
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - frameBackCapture[i].start);
		if(duration.count() > 10)
		{
			//std::cout << "Cleared an expired frame" << std::endl;
			frameBackCapture.erase(frameBackCapture.begin() + i);
			i--;
			x++;
		}
		else
		{
			break;
		}
	}
	if(x > 0)
	{
		//std::cout << "Cleared " << x << " expired frames. Buffer at " << frameBackCapture.size() << std::endl;
	}
}

void Camera::saveToStream(cv::Mat frame, int x)
{
	std::string imageFileName = streamDir + std::to_string(x) + ".bmp";
	//std::cout << "Saving frame to stream as " << imageFileName << std::endl;
	imwrite(imageFileName, frame);
}

void Camera::saveFrameToBuffer(cv::Mat frame)
{
	frameContainer container;
	container.frame = frame;
	container.start = std::chrono::high_resolution_clock::now();
	frameBackCapture.push_back(container);
	//std::cout << "Saving frame to buffer. Buffer at " << frameBackCapture.size() << std::endl;
}

void Camera::saveVideo()
{
	if(frameBackCapture.size() < 1)
	{
		//Trying to write an empty video, this is an error state
		std::cout << "Error: Attempted to save empty video" << std::endl;
		syslog(log_facility | LOG_NOTICE, "Error: Attempting to save empty video");
		exit (EXIT_FAILURE); //Placeholder- Replace with the following:
		//daemon_data.daemon_exit_status = EXIT_FAILURE;
		//terminate_daemon(0);
	}
	
	auto point = std::chrono::high_resolution_clock::now();
	auto s = std::chrono::duration_cast<std::chrono::seconds>(point.time_since_epoch());
	std::time_t t = s.count();
	std::string videoFileName = std::ctime(&t);
	videoFileName.pop_back();
	videoFileName.append(".avi");
	std::string fullVideoString = videoSaveDir + videoFileName;
	cv::VideoWriter video(fullVideoString, CV_FOURCC('M','J','P','G'), 10, cv::Size(frameBackCapture[0].frame.cols, frameBackCapture[0].frame.rows));
	
	for(size_t i = 0; i < frameBackCapture.size(); i++)
	{
		video.write(frameBackCapture[i].frame);
	}
	
	syslog(log_facility | LOG_NOTICE, ("Saved " + fullVideoString).c_str());
	//std::cout << "Saved " << fullVideoString << std::endl;
	frameBackCapture.clear();
}

void Camera::checkRecordingLength()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - recordingStartTime);
	if(duration.count() > 15)
	{
		//std::cout << "Max recording length reached. Saving video." << std::endl;
		recording = false;
		saveVideo();
	}
}

void Camera::record()
{
	int x = 0;
	cv::Mat frame;
	while(true)
	{
		if(!recording)
		{
			clearExpiredFrames();
		}
		
		cap >> frame;
		if(frame.empty())
		{
			syslog(log_facility | LOG_NOTICE, ("Error: Corrupt frame on camera " + std::to_string(cameraID)).c_str());
			//std::cout << "Error: Corrupt frame on camera " << cameraID << std::endl;
			return;
		}
		
		bool humanFound = humanFilter.runRecognition(frame);
		//bool faceFound = faceFilter.runRecognition(frame);
		//bool motionDetected = motionFilter.runDetection(frame);
		
		//Enabling debug will let you view the output live
		if(debug)
		{
			cv::imshow("SmartCCTV Camera " + std::to_string(cameraID), frame);
			char c=(char)cv::waitKey(25);
			if(c==27)
				break;
		}
		
		if(true) //Placeholder- replace with if(daemon_data.is_livestreaming) or equivalent
		{
			saveToStream(frame, x);
		}
		
		if(humanFound) //Placeholder- replace with if((humanFound || faceFound) && motionDetected && (!recording))
		{
			if(!recording)
			{
				//std::cout << "DETECTION EVENT!!!" << std::endl;
				recordingStartTime = std::chrono::high_resolution_clock::now();
				recording = true;
			}
		}
		
		if(recording)
		{
			checkRecordingLength();
		}
		
		saveFrameToBuffer(frame);
		x++;
	}
	
	cap.release();
	cv::destroyAllWindows();
}
