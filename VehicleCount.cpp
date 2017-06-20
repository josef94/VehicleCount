
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Blob.h"
#include "FileHandler.h"

using namespace std;
using namespace cv;

// global variables ///////////////////////////////////////////////////////////////////////////////

// PATH //
//string folder = "18.06.2017_22.54.51";
//string path = "../../Frames/" + folder + "/";
string pictureName = "crop%03d.tif";
string pathSaveROIPicture = "../../Crops/";
char fileName[256] = "../../Vehicles.csv";

vector<String> filenames;
int countRelevantPictures = 0;
int distanceThresh = 5;

int carCountLTR = 0;
int carCountRTL = 0;
bool calculated = false;

//to save Picture for WebSite
int countSaveWeb = 29;
double fps;

// function prototypes ////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, vector<Blob> &existingBlobs);
double distanceBetweenPoints(Point point1, Point point2);
int checkIfBlobsCrossedTheLine(vector<Blob> &blobs, int &intHorizontalLinePosition);
int saveVehicle(vector<Blob> &blobs, Mat &imgFrame2Copy, Mat &imgFrame2Copy2, int k, bool direction, int vehicleCount, int relevantPictures);
int getNumberFromName(string name, int i);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char* argv[]) {
	//// VARIABLES ////
	Mat imgFrame1, imgFrame2, imgDifference, imgFrame1Copy, imgFrame2Copy, structuringElement5x5;
	vector<Blob> blobs, currentFrameBlobs;
	bool blnFirstFrame = true;
	int thresholdValue = 30;
	vector<vector <Point > > contours;

	// CarCount
	Point crossingLine[2];
	int countLinePosition;
	
	string path = "../../Frames/" + argv[1} + "/";

	//// PROGRAM START ////
	calculateDirectionCount(fileName);
	glob(path, filenames, false);
	size_t k = 0;
	if (filenames.size() == 0) {                                                 // if unable to open video file
		cout << "error reading pictures" << endl << endl;      // show error message
		return(0);                                                              // and exit program
	}
	else {
		cout << "processing: ";
	}

	imgFrame1 = imread(filenames[0]);

	// Position of countingline
	countLinePosition = (int)round((double)imgFrame1.cols * 0.5);
	crossingLine[0].x = countLinePosition;
	crossingLine[0].y = 0;
	crossingLine[1].x = countLinePosition;
	crossingLine[1].y = imgFrame1.rows - 1;

	// go trough, frame by frame
	while (k < filenames.size()) {

		//Calculate Ammount of relevant pictures
		countRelevantPictures = 1;
		int iterator = k;
		int number1 = getNumberFromName(filenames[iterator], iterator);
		iterator++;
		if (iterator == filenames.size()) {
			break;
		}
		int number2 = getNumberFromName(filenames[iterator], iterator);
		while (abs(number1 - number2) <= distanceThresh) {
			countRelevantPictures++;
			iterator++;
			if (number1 < number2) {
				number1 = getNumberFromName(filenames[iterator], iterator);
			}
			else {
				number2 = getNumberFromName(filenames[iterator], iterator);
			}
		}
		if (countRelevantPictures < 5) {
			k = k + countRelevantPictures;
		}
		else {
			calculated = false;
			cout << ".";

			// Process
			imgFrame1 = imread(filenames[k]);
			k++;
			imgFrame2 = imread(filenames[k]);
			for (int i = 1; i < countRelevantPictures; i++) {
				imgFrame1Copy = imgFrame1.clone(); 																//30
				imgFrame2Copy = imgFrame2.clone();																//30

				cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);											//60
				cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);											//60
				GaussianBlur(imgFrame1Copy, imgFrame1Copy, Size(3, 3), 0);										//25
				GaussianBlur(imgFrame2Copy, imgFrame2Copy, Size(3, 3), 0);										//25

				absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);											//10
				threshold(imgDifference, imgDifference, thresholdValue, 255.0, CV_THRESH_BINARY);				//5
				structuringElement5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));							//0

				for (unsigned int i = 0; i < 2; i++) {															//500
					dilate(imgDifference, imgDifference, structuringElement5x5);
					dilate(imgDifference, imgDifference, structuringElement5x5);
					erode(imgDifference, imgDifference, structuringElement5x5);
				}

				findContours(imgDifference, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);						//40
				vector< vector <Point> > convexHulls(contours.size());												//0

				for (unsigned int i = 0; i < contours.size(); i++) {
					convexHull(contours[i], convexHulls[i]);
				}
				for (int i = 0; i < convexHulls.size(); i++){
					Blob possibleBlob(convexHulls[i]);

					// 1. minimum size of Box 
					if (possibleBlob.currentBoundingRect.area() > 8000 && // 10000
						possibleBlob.dblCurrentAspectRatio > 0.6 && // 0.8
						possibleBlob.dblCurrentAspectRatio < 4.5 && // 4.0
						possibleBlob.currentBoundingRect.width > 100 && //120
						possibleBlob.currentBoundingRect.height > 60 && // 80
						possibleBlob.dblCurrentDiagonalSize > 250 && // 300
						(contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
						currentFrameBlobs.push_back(possibleBlob);
					}
				}

				if (blnFirstFrame == true) {
					for (int i = 0; i < currentFrameBlobs.size(); i++) {
						Blob possibleBlob(currentFrameBlobs[i]);
					}
				}
				else {
					matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
				}

				int checkedCrossing = checkIfBlobsCrossedTheLine(blobs, countLinePosition);

				if (checkedCrossing == 1) {
					saveVehicle(blobs, imgFrame2, imgFrame1, k, true, carCountRTL, countRelevantPictures);
					checkedCrossing = -1;
				}
				else if (checkedCrossing == 2) {
					saveVehicle(blobs, imgFrame2, imgFrame1, k, false, carCountLTR, countRelevantPictures);
					checkedCrossing = -1;
				}

				currentFrameBlobs.clear();

				imgFrame1 = imgFrame2.clone();           // move frame 1 up to where frame 2 is
				k++;
				if (k < filenames.size()) {
					imgFrame2 = imread(filenames[k]);

				}
				else {
					cout << "end of pictures\n";
					break;
				}
				blnFirstFrame = false;
			}
		}

	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs) {
	for (int i = 0; i < existingBlobs.size(); i++) {
		existingBlobs[i].blnCurrentMatchFoundOrNewBlob = false;
		existingBlobs[i].predictNextPosition();
	}

	for (int j = 0; j < currentFrameBlobs.size(); j++) {
		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {
			if (existingBlobs[i].blnStillBeingTracked == true) {
				double dblDistance = distanceBetweenPoints(currentFrameBlobs[j].centerPositions.back(), existingBlobs[i].predictedNextPosition);
				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlobs[j].dblCurrentDiagonalSize * 0.5) {
			addBlobToExistingBlobs(currentFrameBlobs[j], existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlobs[j], existingBlobs);
		}
	}
	for (int i = 0; i < existingBlobs.size();i++) {
		if (existingBlobs[i].blnCurrentMatchFoundOrNewBlob == false) {
			existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlobs[i].blnStillBeingTracked = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addBlobToExistingBlobs(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex) {
	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addNewBlob(Blob &currentFrameBlob, vector<Blob> &existingBlobs) {
	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
	existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double distanceBetweenPoints(Point point1, Point point2) {
	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);
	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int checkIfBlobsCrossedTheLine(vector<Blob> &blobs, int &intHorizontalLinePosition) {
	int lineOffset = 30;
	for (int i = 0; i < blobs.size();i++){

		if (blobs[i].blnStillBeingTracked == true && blobs[i].centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blobs[i].centerPositions.size() - 2;
			int currFrameIndex = (int)blobs[i].centerPositions.size() - 1;

			if (!calculated) {
				if (blobs[i].centerPositions[currFrameIndex].x < intHorizontalLinePosition + lineOffset && blobs[i].centerPositions[prevFrameIndex].x >= intHorizontalLinePosition - lineOffset) {
					carCountRTL++;
					calculated = true;
					return 1;
				}else if (blobs[i].centerPositions[currFrameIndex].x > intHorizontalLinePosition - lineOffset && blobs[i].centerPositions[prevFrameIndex].x <= intHorizontalLinePosition + lineOffset) {
					carCountLTR++;
					calculated = true;
					return 2;
				}
			}
		}

	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int saveVehicle(vector<Blob> &blobs, Mat &imgFrame2Copy, Mat &imgFrame2Copy2, int k, bool direction, int vehicleCount, int relevantPictures) {
	/* Set Region of Interest*/
		for (unsigned int i = 0; i < blobs.size(); i++) {
			if (blobs[i].blnStillBeingTracked == true) {

				Rect roi;
				roi.x = blobs[i].currentBoundingRect.x;
				roi.y = blobs[i].currentBoundingRect.y;

				roi.width = blobs[i].currentBoundingRect.width;
				roi.height = blobs[i].currentBoundingRect.height;

				int countIndex = getIndexOfLastLine(fileName) + 1;
				Mat temp1, temp2;
				/* Save Pictures */
				char picN[256];
				sprintf(picN, "pic%03i.tif", countIndex);
				imwrite(pathSaveROIPicture + picN, imgFrame2Copy);

				Mat crop = imgFrame2Copy(roi);
				char cropN[256];
				sprintf(cropN, "crop%03i.tif", countIndex);
				imwrite(pathSaveROIPicture + cropN, crop);

				char blurN[256];
				sprintf(blurN, "blur%03i.tif", countIndex);
				cvtColor(imgFrame2Copy, temp1, CV_BGR2GRAY);
				cvtColor(imgFrame2Copy2, temp2, CV_BGR2GRAY);
				Mat blur = temp1 - temp2;
				imwrite(pathSaveROIPicture + blurN, blur);

				// k, 
				writeCSV(countIndex, folder, fileName, getNumberFromName(filenames[k], k), relevantPictures, direction, vehicleCount, cropN, blurN, picN, roi);
				}

		}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

int getNumberFromName(string name, int i) {
	int number;
	string trim = filenames[i].substr(filenames[i].find("pic") + 3);
	trim = trim.substr(0, trim.find(".tif"));
	stringstream ss(trim);
	ss >> number;
	return number;
}
