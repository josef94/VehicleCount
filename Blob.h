#pragma once
// Blob.h

#ifndef MY_BLOB
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

///////////////////////////////////////////////////////////////////////////////////////////////////
class Blob {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	vector<Point> currentContour;
	Rect currentBoundingRect;
	vector<Point> centerPositions;
	double dblCurrentDiagonalSize;
	double dblCurrentAspectRatio;
	bool blnCurrentMatchFoundOrNewBlob;
	bool blnStillBeingTracked;
	int intNumOfConsecutiveFramesWithoutAMatch;
	Point predictedNextPosition;

	// function prototypes ////////////////////////////////////////////////////////////////////////
	Blob(vector<Point> _contour);
	void predictNextPosition(void);

};

#endif    // MY_BLOB


