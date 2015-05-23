#ifndef _CAMERA_IMAGE_STREAM_H_
#define _CAMERA_IMAGE_STREAM_H_

#include "ImageStream.h"

class CameraImageStream : public ImageStream {
	
public:
	
	CameraImageStream(int device) {
		capture.open(device);
	}

	bool getImage(cv::Mat & image) {
		for ( int i = 0 ; i < 8 ; ++ i )
			capture.grab(); // Skip hardware camera buffer
		return capture.retrieve(image);
	}
	
private:
	
	cv::VideoCapture capture;
	
};

#endif // _CAMERA_IMAGE_STREAM_H_
