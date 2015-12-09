#include <iostream>
#include "ConfigLoader.h"
#include "DifferenceAnalyzer.h"
#include "RectanglesTracker.h"

cv::Mat frame;
RectanglesTracker tracker;

void mouse(int event, int x, int y, int flags, void * userdata)
{
	static cv::Point p;
	cv::Mat temp_frame = frame.clone();
	if ( event == cv::EVENT_MOUSEMOVE ) {
		if ( flags & cv::EVENT_FLAG_LBUTTON ) {
			rectangle(temp_frame, cv::Rect(p, cv::Point(x, y)), cv::Scalar(255,100,100));
		}
	} else if ( event == cv::EVENT_LBUTTONDOWN ) {
		p = cv::Point(x, y);
	} else if ( event == cv::EVENT_LBUTTONUP ) {
		tracker.processRect(cv::Rect(p, cv::Point(x, y)), 1);
	}

	std::vector<cv::Rect> rects = tracker.getRects();
	for ( size_t i = 0 ; i < rects.size() ; ++ i ) { 
		cv::rectangle(temp_frame, rects[i], cv::Scalar(100, 255, 100));
	}
	cv::imshow("main", temp_frame);
}

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
	
	pstream->getImage(frame);
	
	// Select initial cars
	cv::namedWindow("main", cv::WINDOW_AUTOSIZE);
	cv::imshow("main", frame);
	cv::setMouseCallback("main", mouse, NULL);
	for ( ; ; ) {
		short key = cv::waitKey(0);
		if ( key == '\n' )
			break;
		if ( key == 'r' ) {
			tracker.reset();
			cv::imshow("main", frame);
		}
	}
	cv::setMouseCallback("main", NULL, NULL);
	
	DifferenceAnalyzer analyzer(frame, (argc >= 4) ? argv[3] : "calibrator/calibration.cfg");
	
	int delay = 0;
	if ( argc >= 5 )
		sscanf(argv[4], "%d", &delay);
	
	for ( int stage = 0 ; stage < DifferenceAnalyzer::STAGE_COUNT ; ++ stage )
		cv::namedWindow(((std::stringstream&)(std::stringstream()<<stage)).str(), cv::WINDOW_AUTOSIZE);
	
	bool running = true;
	while (running && pstream->getImage(frame)) {
		
		analyzer.takeNextImage(frame);
		std::vector<DifferenceAnalyzer::Change> changes = analyzer.getChanges();
		for ( size_t i = 0 ; i < changes.size() ; ++ i ) {
			std::cout << changes[i].rect << " | " << changes[i].type << std::endl;
			if ( tracker.processRect(changes[i].rect, changes[i].type) ) {
				carsCount += changes[i].type;
			} else {
				cv::rectangle(frame, changes[i].rect, cv::Scalar(100,100,255));
			}
		}
		std::vector<cv::Rect> rects = tracker.getRects();
		for ( size_t i = 0 ; i < rects.size() ; ++ i ) {
			cv::rectangle(frame, rects[i], cv::Scalar(100, 255, 100));
		}
		std::cout << "Cars count : " << carsCount << std::endl;
		cv::imshow("main", frame);
		
		for ( int stage = 0 ; stage < DifferenceAnalyzer::STAGE_COUNT ; ++ stage ) {
			analyzer.getStage(frame, stage);
			cv::imshow(((std::stringstream&)(std::stringstream()<<stage)).str(), frame);
		}
		
		for ( bool spaced = false ; !spaced ; ) {
			switch ((short)cv::waitKey(delay)) {
			case -1 :
				spaced = true;
				break;
			case 27 :
				running = false;
				spaced = true;
				break;
			case 32 : 
				spaced = true;
				break;
			}
		}
	}
	
	return 0;
}