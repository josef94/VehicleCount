#include "FileHandler.h"

int directionCountRight;
int directionCountLeft;

///////////////////////////////////////////////////////////////////////////////////////////////////

void writeCSV(int lastLine, string folder, string csvName, int pictureNumber, int picureCount, bool direction, int vehicleCount, string cropName, string blurName, string picName, Rect roi) {
	string limiter = ";\t";
	int index;
	char s[128];
	bool newFile = false;
	ifstream input_stream;
	input_stream.open(csvName.c_str(), ios::in);
	if (!input_stream) {
		newFile = true;
	}

	fstream file;
	file.open(csvName.c_str(), ios::app);
	if (newFile) {
		file << "Index" + limiter + "Timestamp" + limiter + "ReleventPictures" + limiter + "Direction" + limiter + "DirectionCount" + limiter + "CropName" + limiter + "BlurName" + limiter + "PicName" + limiter + "Roi X" + limiter + "Roi Y" + limiter + "Roi Width" + limiter + "Roi Height\n";
	}

	file << intToString(lastLine) + limiter;

	string timestamp = calculateTimestamp(folder, pictureNumber);
	file << timestamp + limiter;
	file << intToString(picureCount) + limiter;
	if (direction) {
		file << "R" + limiter;
		file << intToString(directionCountRight+1) + limiter;
	}
	else {
		file << "L" + limiter;
		file << intToString(directionCountLeft+1) + limiter;
	}
	file << cropName + limiter;
	file << blurName + limiter;
	file << picName;
	sprintf(s, ";\t%i", roi.x);
	file << s;
	sprintf(s, ";\t%i", roi.y);
	file << s;
	sprintf(s, ";\t%i", roi.width);
	file << s;
	sprintf(s, ";\t%i", roi.height);
	file << s;
	file << ";\n";
	file.close();
}


///////////////////////////////////////////////////////////////////////////////////////////////////

string calculateTimestamp(string fold, int picN) {
	int day, month, year, hour, min, sec;
	int nFps = 25;
	sscanf(fold.c_str(), "%d.%d.%d_%d.%d.%d", &day, &month, &year, &hour, &min, &sec);

	struct tm time = { 0 };
	time.tm_year = year - 1900;
	time.tm_mon = month - 1;
	time.tm_mday = day;
	time.tm_hour = hour-1;
	time.tm_min = min;
	time.tm_sec = sec + picN / nFps;

	char buff[20];
	time_t calculatedTime = mktime(&time);
	
	strftime(buff,20, "%d.%m.%Y %H:%M:%S", localtime(&calculatedTime));

	return buff;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

string intToString(int a) {
	ostringstream temp;
	temp << a;
	return temp.str();
}


///////////////////////////////////////////////////////////////////////////////////////////////////

int getIndexOfLastLine(string filename){
	ifstream input_stream; 
	input_stream.open(filename.c_str(), ios::in);
	string line, lastLine;
	while (getline(input_stream, line)) {
		lastLine = line;
	}
	if (lastLine == "") {
		return 0;
	}
	else {
		line = lastLine;
		if (line.find(";\tL;\t") != string::npos){
			line = line.substr(line.find(";\tL;\t") + 5, line.find(";\tcrop") - 1);
			directionCountLeft = stringToInt(line);
		}
		else if(line.find(";\tR;\t") != string::npos) {
			line = line.substr(line.find(";\tR;\t") + 5, line.find(";\tcrop") - 1);
			directionCountRight = stringToInt(line);
		}
		return stringToInt(lastLine.substr(0, lastLine.find('\t')));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calculateDirectionCount(string filename) {
	ifstream input_stream;
	input_stream.open(filename.c_str(), ios::in);
	string line, lastLine;
	while (getline(input_stream, line)) {
		lastLine = line;

		if (line.find(";\tL;\t") != string::npos) {
			line = line.substr(line.find(";\tL;\t") + 5, line.find(";\tcrop") - 1);
			directionCountLeft = stringToInt(line);
		}

		else if (line.find(";\tR;\t") != string::npos) {
			line = line.substr(line.find(";\tR;\t") + 5, line.find(";\tcrop") - 1);
			directionCountRight = stringToInt(line);
		}
	}
	if (lastLine == "") {
		directionCountLeft = 0;
		directionCountRight = 0;
	}

}

int stringToInt(string s) {
	int integer;
	stringstream ss(s);
	ss >> integer;
	return integer;
}


