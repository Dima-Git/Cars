#include <iostream>
#include "ConfigLoader.h"
#include "DifferenceAnalyzer.h"

int main(int argc, char * argv[])
{
	if ( argc < 2 ) {
		std::cerr << "Config file not specified." << std::endl;
		return 1;
	}
	
	ImageStream * pstream = loadConfig(argv[1]);
	
	if ( !pstream ) {
		std::cerr << "Config file '" << argv[1] << "' read error." << std::endl;
		return 2;
	}
	
	int carsCount = 0;
	if ( argc >= 3 )
		sscanf(argv[2], "%d", &carsCount);
	
	std::vector<cv::Mat> images;
	cv::Mat frame;
	pstream->getImage(frame);
	DifferenceAnalyzer analyzer(frame, (argc>=4)?argv[3]:"calibrator/calibration.cfg");
	
	for ( int stage = 0 ; stage < DifferenceAnalyzer::STAGE_COUNT ; ++ stage )
		cv::namedWindow(((std::stringstream&)(std::stringstream()<<stage)).str(), cv::WINDOW_AUTOSIZE);
	
	bool running = true;
	while (running && pstream->getImage(frame)) {
		
		carsCount += analyzer.takeNextImage(frame);
		std::cout << "Cars count : " << carsCount << std::endl;  
		for ( int stage = 0 ; stage < DifferenceAnalyzer::STAGE_COUNT ; ++ stage ) {
			analyzer.getStage(frame, stage);
			cv::imshow(((std::stringstream&)(std::stringstream()<<stage)).str(), frame);
		}
		
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