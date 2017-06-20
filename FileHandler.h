#pragma once
//FileHandler.h

#ifndef MY_FILEHANDLER
#define MY_FILEHANDLER

#include<fstream> 
#include<iostream>
#include <ctime>
#include <time.h>
#include <vector>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void writeCSV(int countIndex, string folder, string csvName, int pictureNumber, int picureCount, bool direction, int vehicleCount, string cropName, string blurName, string picName, Rect roi);
string calculateTimestamp(string fold, int picN);
string intToString(int a);
int getIndexOfLastLine(string filename);
void calculateDirectionCount(string filename);
int stringToInt(string s);

#endif