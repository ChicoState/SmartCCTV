/**
 * File Name:  camera.cpp
 * Created By:  Svyatoslav Chukhlebov <slavikchukhlebov@mail.csuchico.edu>
 * Created On:  4/25/20
 *
 * Modified By:  KonstantinRebrov <krebrov@mail.csuchico.edu>
 * Modified On:  5/02/20
 *
 * Description:
 * This class is used to run image recogntition on a Mat object, searching for humans in the frame.
 * Each instance of this class is to correspond to a single camera or video file.
 */

#include "low_level_cctv_daemon_apis.h"
#include "write_message.h"
#include "camera.hpp"
#include <opencv2/imgcodecs.hpp>
#include <sys/stat.h>   /* for mkdir() */
#include <sys/types.h>  /* for permissions constatnts */
#include <syslog.h>     /* for syslog() */
#include <string>       /* for std::string, std::to_string() */
#include <cstring>      /* for strerror() */
#include <errno.h>      /* for errno */

using std::string;
using std::to_string;


extern Daemon_data daemon_data;

int mkpath(const string& path, size_t start, mode_t mode)
{
    size_t path_length = path.length();

    size_t i = start;
    while (i < path_length) {
        // get to the next directory
        while (path[i] != '/')
            ++i;

        string directory = path.substr(0, i);
        int return_value = mkdir(directory.c_str(), mode);
        const char* const error_message = strerror(errno);
        if (return_value == -1) {
            // if you couldn't create that directory because it already exists
            if (errno == EEXIST) {
                return 0;
            } else {
                syslog(log_facility | LOG_ERR, "Failed to create %s : %s", directory.c_str(), error_message);
                return return_value;
            }
        }

        ++i;  // jump over the '/'
    }

    return 0;
}


Camera::Camera(int cameraID)
{
    this->cameraID = cameraID; 

    recording = false;
    streamDir = "/tmp/SmartCCTV_livestream/camera" + std::to_string(cameraID) + "/";
    videoSaveDir = daemon_data.home_directory;
    videoSaveDir += "/SmartCCTV_recordings/camera" + std::to_string(cameraID) + "/";
	
    // TODO: remove these temporary lines
    syslog(log_facility | LOG_NOTICE, "streamDir = %s", streamDir.c_str());
    syslog(log_facility | LOG_NOTICE, "videoSaveDir = %s", videoSaveDir.c_str());

    if (mkpath(videoSaveDir, 17, S_IRWXU) == -1) {
        string message = "SmartCCTV could not create ";
        message += videoSaveDir;
        write_message(message);

        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    } else {
        syslog(log_facility | LOG_NOTICE, "Creating %s", videoSaveDir.c_str());
    }

    if (mkpath(streamDir, 5, S_IRWXU) == -1) {
        string message = "SmartCCTV could not create the live stream.";
        write_message(message);

        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    } else {
        syslog(log_facility | LOG_NOTICE, "Creating %s", streamDir.c_str());
    }
	
    cap.open(cameraID);
    if (!cap.isOpened())
   	{
        string message = "SmartCCTV failed to open camera";
        message += to_string(cameraID);
        write_message(message);

        syslog(log_facility | LOG_ERR, "Failed to open camera%d", cameraID);

        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    } else {
        syslog(log_facility | LOG_NOTICE, "Creating camera%d", cameraID);
    }
}


Camera::Camera(std::string readFilePath)
{
    this->readFilePath = readFilePath; 

    cameraID = -1;
    recording = false;

    streamDir = "/tmp/SmartCCTV_livestream/camera" + std::to_string(cameraID) + "/";
    videoSaveDir = daemon_data.home_directory;
    videoSaveDir += "/camera" + std::to_string(cameraID) + "/";
    
    cap.open(readFilePath);
    if (!cap.isOpened())
    {
        string message = "SmartCCTV failed to open ";
        message += readFilePath;
        write_message(message);

        syslog(log_facility | LOG_ERR, "Failed to open media file %s", readFilePath.c_str());

        daemon_data.daemon_exit_status = EXIT_FAILURE;
        terminate_daemon(0);
    } else {
        syslog(log_facility | LOG_NOTICE, "Opening media file %s", readFilePath.c_str());
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

        string message = "SmartCCTV: something has gone wrong with saving the video.";
        write_message(message);

		//std::cout << "Error: Attempted to save empty video" << std::endl;
		syslog(log_facility | LOG_ERR, "Error: Attempting to save empty video");
        return;
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
	
	syslog(log_facility | LOG_NOTICE, "Saved a video %s", fullVideoString.c_str());
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


void Camera::finalize()
{
    cap.release();
	cv::destroyAllWindows();
}


void Camera::record()
{
	syslog(log_facility | LOG_NOTICE, "Camera recording.");

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
            string message = "SmartCCTV encountered an error.";
            write_message(message);

			syslog(log_facility | LOG_ERR, "Error: Corrupt frame on camera.");
			//std::cout << "Error: Corrupt frame on camera " << cameraID << std::endl;

			daemon_data.daemon_exit_status = EXIT_FAILURE;
    	    terminate_daemon(0);
		}
		
		syslog(log_facility | LOG_NOTICE, "Running Human Recognition.");
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
			syslog(log_facility | LOG_NOTICE, "Saving frame to livestream dir");
			saveToStream(frame, x);
		}
		
		if(humanFound) //Placeholder- replace with if((humanFound || faceFound) && motionDetected && (!recording))
		{
			if(!recording)
			{
				//std::cout << "DETECTION EVENT!!!" << std::endl;
				recordingStartTime = std::chrono::high_resolution_clock::now();
				recording = true;
				syslog(log_facility | LOG_NOTICE, "Human found!!!");

			}
		}
		else
		{
				syslog(log_facility | LOG_NOTICE, "Human not found...");
		}
		
		if(recording)
		{
			checkRecordingLength();
		}
		
		saveFrameToBuffer(frame);
		x++;
		//syslog(log_facility | LOG_NOTICE, "Through the loop...");

	}
	
	cap.release();
	cv::destroyAllWindows();
}

