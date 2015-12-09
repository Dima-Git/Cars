#ifndef _DIFFERENCE_ANALYZER_H_
#define _DIFFERENCE_ANALYZER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <vector>

class DifferenceAnalyzer {
	
public:
	
	enum Stage { 
		STAGE_OLD, 
		STAGE_NEW, 
		STAGE_DIFF, 
		STAGE_THRESH, 
		STAGE_BLUR,
		STAGE_GRAY, 
		STAGE_DRAW,
		STAGE_COUNT
	};
	
	struct Change {
		enum { 
			CHANGE_APPEARED,
			CHANGE_DISAPPEARED,
			CHANGE_REPLACED 
		} type;
		cv::Rect rect;
	};
	
	DifferenceAnalyzer(cv::Mat image, const char * configPath) {
		loadCalibration(configPath);
		stages = std::vector<cv::Mat>(STAGE_COUNT);
		image.copyTo(stages[STAGE_OLD]);
		image.copyTo(stages[STAGE_NEW]);
		analyze();
	}
	
	int takeNextImage(cv::Mat image) {
		stages[STAGE_NEW].copyTo(stages[STAGE_OLD]);
		image.copyTo(stages[STAGE_NEW]);
		return analyze();
	}
	
	void getStage(cv::Mat & image, size_t stage) const {
		if ( stage >= 0 && stage < stages.size() )
			stages[stage].copyTo(image);
	}
	
	std::vector<Change> getChanges() const {
		return changes;
	}
	
private:
	
	std::vector<cv::Mat> stages;
	
	std::vector<Change> changes;
	
	int var_area_min;
	int var_area_max;
	int var_thresh;
	int var_thresh_type;
	int var_median_ksize;
	int var_gaussian_ksizex;
	int var_gaussian_ksizey;
	
	void loadCalibration(const char * configPath) {
		std::ifstream config(configPath);
		if ( !config.good() ) {
			std::cerr << "Calibration config not found, using default values instead." << std::endl;
			var_area_min = 1900;
			var_area_max = 5000;
			var_thresh = 20;
			var_thresh_type = cv::THRESH_TOZERO;
			var_median_ksize = 3;
			var_gaussian_ksizex = 3;
			var_gaussian_ksizey = 3;
			return;
		}
		std::string line;
		config >> var_area_min; std::getline(config,line);
		config >> var_area_max; std::getline(config,line);
		config >> var_thresh; std::getline(config,line);
		config >> var_thresh_type; std::getline(config,line);
		config >> var_median_ksize; std::getline(config,line);
		config >> var_gaussian_ksizex; std::getline(config,line);
		config >> var_gaussian_ksizey;
		config.close();
	}
	
	int analyze() {
		int answerShift = 0;
		changes.clear();
		
		cv::absdiff(stages[STAGE_OLD], stages[STAGE_NEW], stages[STAGE_DIFF]);
		cv::threshold(stages[STAGE_DIFF], stages[STAGE_THRESH], var_thresh, 255, var_thresh_type);
		cv::medianBlur(stages[STAGE_THRESH], stages[STAGE_BLUR], var_median_ksize);
		cv::GaussianBlur(stages[STAGE_BLUR], stages[STAGE_BLUR], 
			cv::Size(var_gaussian_ksizex, var_gaussian_ksizey), 0);
		cv::cvtColor(stages[STAGE_BLUR], stages[STAGE_GRAY], cv::COLOR_RGB2GRAY);
		stages[STAGE_GRAY].copyTo(stages[STAGE_DRAW]);
		
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(stages[STAGE_DRAW], contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		stages[STAGE_NEW].copyTo(stages[STAGE_DRAW]);
		cv::drawContours(stages[STAGE_DRAW], contours, -1, cv::Scalar(255,255,0));
		for ( size_t i = 0 ; i < contours.size() ; ++ i ) {
			double area = cv::contourArea(contours[i]);
			if ( area >= var_area_min && area <= var_area_max ) {
				cv::Rect rect = cv::boundingRect(contours[i]);
				cv::Mat hsv_old, hsv_new;
				
				cv::cvtColor(stages[STAGE_OLD](rect), hsv_old, cv::COLOR_RGB2HSV);
				cv::cvtColor(stages[STAGE_NEW](rect), hsv_new, cv::COLOR_RGB2HSV);
				
				cv::Scalar sd_old, m_old, sd_new, m_new;
				cv::meanStdDev(stages[STAGE_OLD](rect), m_old, sd_old);
				cv::meanStdDev(stages[STAGE_NEW](rect), m_new, sd_new);
				
				double dp_old = sd_old.dot(sd_old);
				double dp_new = sd_new.dot(sd_new);
				
				Change change;
				change.rect = rect;
				
				const double dp_thresh = 2000.0; // deviation threshhold
				
				if ( dp_old < dp_thresh && dp_new > dp_thresh ) { // Object appeared
					cv::rectangle(stages[STAGE_DRAW], rect, cv::Scalar(0,255,0));
					answerShift ++;
					change.type = Change::CHANGE_APPEARED;
				}
				else if ( dp_new < dp_thresh &&  dp_old > dp_thresh ) { // Object disappeared
					cv::rectangle(stages[STAGE_DRAW], rect, cv::Scalar(0,0,255));
					answerShift --;
					change.type = Change::CHANGE_DISAPPEARED;
				}
				else { // Object replacement
					cv::rectangle(stages[STAGE_DRAW], rect, cv::Scalar(255,0,0));
					change.type = Change::CHANGE_REPLACED;
				}
				
				cv::putText(stages[STAGE_DRAW], ((std::stringstream&)(std::stringstream()<<dp_old)).str(), rect.tl(), 6, 1, cv::Scalar(0,255,255));
				cv::putText(stages[STAGE_DRAW], ((std::stringstream&)(std::stringstream()<<dp_new)).str(), rect.br(), 6, 1, cv::Scalar(255,255,0));
				changes.push_back(change);
			}
		}
		return answerShift;
	}
	
};

#endif // _DIFFERENCE_ANALYZER_H_