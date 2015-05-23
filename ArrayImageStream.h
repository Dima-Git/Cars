#ifndef _ARRAY_IMAGE_STREAM_H_
#define _ARRAY_IMAGE_STREAM_H_

#include "ImageStream.h"
#include <vector>

class ArrayImageStream : public ImageStream {
	
public:
	
	ArrayImageStream(std::vector<cv::Mat> array) {
		images = array;
		index = 0;
	}

	bool getImage(cv::Mat & image) {
		if ( index < images.size() ) {
			images[index++].copyTo(image);
			return true;
		}
		return false;
	}
	
private:
	
	std::vector<cv::Mat> images;
	int index;
	
};

#endif // _ARRAY_IMAGE_STREAM_H_
