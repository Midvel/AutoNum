// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>

#include <opencv2\opencv.hpp>
#include <opencv2\ml\ml.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

#include "utils/NeuronTrainingModule.h"
#include "utils/HaarTrainingModule.h"

enum imgnums
{
	IMG_ORIGINAL = 0,
	IMG_NORM_LINES_FILTERED,
	IMG_NORM_LINES_EDGED,
	IMG_NORM_LINES_PLATOES,
	IMG_NORM_LINES,
	IMG_NORM_AFFINE,
	IMG_NORMALIZED,
	IMG_NORM_BIN,
	IMG_NORM_BIN_HISTO,
	IMG_NORM_CUTLINES,
	IMG_TOTAL
};

void SetImage(cv::Mat&, int);
cv::Mat& GetImage(int);

Mat ContourAnalysis(vector<vector<Point>>&, vector<Vec2f>&, int, int);

Mat NormalizeAutonum(cv::Mat&);

void Decomposition(Mat&, vector<Mat>&);

string Recognition(vector<Mat>&);
