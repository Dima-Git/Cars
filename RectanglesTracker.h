#ifndef _RECTANGLES_TRACKER_H_
#define _RECTANGLES_TRACKER_H_

#include <opencv2/opencv.hpp>
#include <vector>

class RectanglesTracker {

public:

	RectanglesTracker() {}

	std::vector<cv::Rect> getRects() const {
		return rects;
	}

	void reset() {
		rects.clear();
	}

	bool processRect(cv::Rect rect, int type) {
		const double overlap_thresh = 0.2;
		switch (type) {
		case 1: // Add
			rects.push_back(rect);
			return true;
		case 0: // Modify
			if ( rects.size() > 0 ) {
				size_t idx = mostOverlapped(rect);
				if ( overlapPart(rects[idx], rect) > overlap_thresh ) {
					rects[idx] = rect;
					return true;
				}
			}
			return false;
		case -1: // Remove
			if ( rects.size() > 0 ) {
				size_t idx = mostOverlapped(rect);
				if ( overlapPart(rects[idx], rect) > overlap_thresh ) {
					rects.erase(rects.begin() + idx);
					return true;
				}
			}
			return false;
		}
		return false;
	}

private:

	std::vector<cv::Rect> rects;

	double overlapPart(cv::Rect a, cv::Rect b) {
		return (double)(a & b).area() / (double)(a.area() + b.area() + (a & b).area());
	}

	size_t mostOverlapped(cv::Rect r) {
		size_t idx = 0;
		for ( size_t i = 0 ; i < rects.size() ; ++ i )
			if ( (rects[i] & r).area() > (rects[idx] & r).area() )
				idx = i;
		return idx;
	}

};

#endif // _RECTANGLES_TRACKER_H_

