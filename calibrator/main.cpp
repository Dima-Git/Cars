#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

enum Stage { STAGE_OLD, 
	     STAGE_NEW, 
	     STAGE_DIFF, 
	     STAGE_THRESH, 
	     STAGE_MEDIANBLUR,
	     STAGE_GAUSSIANBLUR,
	     STAGE_GRAY, 
	     STAGE_DRAW,
	     STAGE_COUNT };

std::vector<cv::Mat> stages(STAGE_COUNT);

/* Variables */
int var_area_min = 1900;
int var_area_max = 5000;
int var_thresh = 20;
int var_thresh_type = cv::THRESH_TOZERO;
int var_median_ksize = 1;
int var_gaussian_ksizex = 1;
int var_gaussian_ksizey = 1;
/*************/

void analyze() {
	cv::absdiff(stages[STAGE_OLD], stages[STAGE_NEW], stages[STAGE_DIFF]);
	cv::threshold(stages[STAGE_DIFF], stages[STAGE_THRESH], var_thresh, 255, var_thresh_type);
	cv::medianBlur(stages[STAGE_THRESH], stages[STAGE_MEDIANBLUR], var_median_ksize*2+1);
	cv::GaussianBlur(stages[STAGE_MEDIANBLUR], stages[STAGE_GAUSSIANBLUR], 
		cv::Size(var_gaussian_ksizex*2+1, var_gaussian_ksizey*2+1), 0);
	cv::cvtColor(stages[STAGE_GAUSSIANBLUR], stages[STAGE_GRAY], cv::COLOR_RGB2GRAY);
	stages[STAGE_GRAY].copyTo(stages[STAGE_DRAW]);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(stages[STAGE_DRAW], contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	stages[STAGE_NEW].copyTo(stages[STAGE_DRAW]);
	cv::drawContours(stages[STAGE_DRAW], contours, -1, cv::Scalar(255,255,0));
	for ( int i = 0 ; i < contours.size() ; ++ i ) {
		double area = cv::contourArea(contours[i]);
		if ( area >= var_area_min && area <= var_area_max ) {
			cv::Rect rect = cv::boundingRect(contours[i]);
			cv::rectangle(stages[STAGE_DRAW], rect, cv::Scalar(0,255,0));
		}
	}
}

void trackFunc(int value, void * data) {
	analyze();
	//cv::imshow("STAGE_OLD", stages[STAGE_OLD]);
	//cv::imshow("STAGE_NEW", stages[STAGE_NEW]);
	cv::imshow("STAGE_DIFF", stages[STAGE_DIFF]);
	cv::imshow("STAGE_MEDIANBLUR", stages[STAGE_MEDIANBLUR]);
	cv::imshow("STAGE_GAUSSIANBLUR", stages[STAGE_GAUSSIANBLUR]);
	//cv::imshow("STAGE_GRAY", stages[STAGE_GRAY]);
	cv::imshow("STAGE_DRAW", stages[STAGE_DRAW]);
}

void onMouse(int e, int x, int y, int f, void * data) {
	if ( e == 1) {
		std::ofstream config("calibration.cfg");
		config << var_area_min        	    << " : area_min"        << std::endl;
		config << var_area_max        	    << " : area_max"        << std::endl;
		config << var_thresh          	    << " : thresh"          << std::endl;
		config << var_thresh_type     	    << " : thresh_type"     << std::endl;
		config << (var_median_ksize*2+1)    << " : median_ksize"    << std::endl;
		config << (var_gaussian_ksizex*2+1) << " : gaussian_ksizex" << std::endl;
		config << (var_gaussian_ksizey*2+1) << " : gaussian_ksizey" << std::endl;
		config.close();
		std::cout << "Configuration saved to calibration.cfg" << std::endl;
	}
}

int main(int argc, char * argv[])
{
	if ( argc < 3 ) {
		std::cerr << "Input files not specified" << std::endl;
		return 1;
	}
	stages[STAGE_OLD] = cv::imread(argv[1]);
	stages[STAGE_NEW] = cv::imread(argv[2]);
	
	if ( !stages[STAGE_OLD].data || !stages[STAGE_NEW].data ) {
		std::cerr << "Input files load error." << std::endl;
		return 2;
	}
	
	cv::namedWindow("Control window", cv::WINDOW_AUTOSIZE);
	//cv::namedWindow("STAGE_OLD", cv::WINDOW_AUTOSIZE);
	//cv::namedWindow("STAGE_NEW", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("STAGE_DIFF", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("STAGE_MEDIANBLUR", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("STAGE_GAUSSIANBLUR", cv::WINDOW_AUTOSIZE);
	//cv::namedWindow("STAGE_GRAY", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("STAGE_DRAW", cv::WINDOW_AUTOSIZE);
	
	cv::createTrackbar("Area min", "Control window", &var_area_min, 8000, trackFunc);
	cv::createTrackbar("Area max", "Control window", &var_area_max, 8000, trackFunc);
	cv::createTrackbar("Thresh", "Control window", &var_thresh, 255, trackFunc);
	cv::createTrackbar("Thresh type", "Control window", &var_thresh_type, 4, trackFunc);
	cv::createTrackbar("Median ksize (value*2+1)", "Control window", &var_median_ksize, 5, trackFunc);
	cv::createTrackbar("Gaussian ksizex (value*2+1)", "Control window", &var_gaussian_ksizex, 5, trackFunc);
	cv::createTrackbar("Gaussian ksizey (value*2+1)", "Control window", &var_gaussian_ksizey, 5, trackFunc);
	
	cv::Mat saveImage = cv::Mat::zeros(40, 400, CV_8UC3);
	cv::putText(saveImage, "Click here to save to 'calibration.cfg'", cv::Point(20,25), 
		cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,255,0), 1.5);
	cv::setMouseCallback("Control window", onMouse);
	cv::imshow("Control window", saveImage);
	
	trackFunc(0, 0);
	
	cv::waitKey(0);
	
	return 0;
}