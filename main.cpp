#include <iostream>
#include "ConfigLoader.h"

int main(int argc, char * argv[])
{
	if ( argc == 1 ) {
		std::cerr << "Config file not specified." << std::endl;
		return 1;
	}
	
	ImageStream * pstream = loadConfig(argv[1]);
	
	if ( !pstream ) {
		std::cerr << "Config file '" << argv[1] << "' read error." << std::endl;
		return 2;
	}
	
	
	const cv::String windowName = "Window";
	std::vector<cv::Mat> images;
	cv::Mat frame;
	
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
	
	bool running = true;
	while (running && pstream->getImage(frame)) {
		
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