#ifndef _CONFIG_LOADER_H_
#define _CONFIG_LOADER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "CameraImageStream.h"
#include "ArrayImageStream.h"

ImageStream * loadConfig(const char * config_path)
{
	ImageStream * pstream = 0;
	std::ifstream config(config_path);
	std::string stream_type, line;
	std::vector<cv::Mat> images;
	int array_size = 0;
	
	std::getline(config, stream_type);
	if ( stream_type == "array" ) {
		config >> array_size;
		std::getline(config, line); //Skip newline
		std::getline(config, line);
		for ( int i = 1 ; i <= array_size ; ++ i ) {
			std::string path = line;
			path.replace(path.find("*"), 1, 
				((std::stringstream&)(std::stringstream() << i)).str());
			images.push_back(cv::imread(path));
		}
		pstream = new ArrayImageStream(images);
	} else if ( stream_type == "list" ) {
		config >> array_size;
		std::getline(config, line); //Skip newline
		for ( int i = 1 ; i <= array_size ; ++ i ) {
			std::getline(config, line);
			images.push_back(cv::imread(line));
		}
		pstream = new ArrayImageStream(images);
	} else if ( stream_type == "camera" ) {
		pstream = new CameraImageStream(0);
	}
	
	return pstream;
}

#endif // _CONFIG_LOADER_H_
