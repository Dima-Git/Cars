#ifndef _IMAGE_STREAM_H_
#define _IMAGE_STREAM_H_

#include <opencv2/opencv.hpp>

class ImageStream {
	
public:
	virtual bool getImage(cv::Mat & image) = 0;
	virtual ~ImageStream() {}
	
};

#endif // _IMAGE_STREAM_H_
