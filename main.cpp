#include <iostream>
#include <opencv2/opencv.hpp>
#include "CameraImageStream.h"

int main(int argc, char * argv[])
{
	const cv::String windowName = "Window";
	CameraImageStream stream(0);
	cv::Mat frame;
	
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
	
	bool running = true;
	while (running) {
		
		stream.getImage(frame);
		
		cv::imshow(windowName, frame);
		
		switch (cv::waitKey(0)) {
		case -1 :
			break;
		case 27 :
			running = false;
			break;
		}
	}
	
	return 0;
}