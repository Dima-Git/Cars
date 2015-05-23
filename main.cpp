#include <iostream>
#include <opencv2/opencv.hpp>
#include "ArrayImageStream.h"

int main(int argc, char * argv[])
{
	const cv::String windowName = "Window";
	std::vector<cv::Mat> images;
	images.push_back(cv::Mat::ones(240, 320, CV_32FC1) * 0.00);
	images.push_back(cv::Mat::ones(240, 320, CV_32FC1) * 0.25);
	images.push_back(cv::Mat::ones(240, 320, CV_32FC1) * 0.50);
	images.push_back(cv::Mat::ones(240, 320, CV_32FC1) * 0.75);
	images.push_back(cv::Mat::ones(240, 320, CV_32FC1) * 1.00);
	ArrayImageStream stream(images);
	cv::Mat frame;
	
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
	
	bool running = true;
	while (running && stream.getImage(frame)) {
		
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