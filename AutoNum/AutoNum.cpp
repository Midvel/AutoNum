// AutoNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

static Mat OpenImage()
{
	Mat image;
	string filename;
/*	cout << "Input image:\n>> ";

	getline(cin, filename);

	if (filename.size() > 0)
		image = imread(filename.c_str(), IMREAD_UNCHANGED);
	else
		throw "No path entered";*/

	image = imread("6.bmp", IMREAD_UNCHANGED);

	return image;
}

int main()
{
	Mat original;
	Mat normalized;
	vector<Mat> components;
	
	original = OpenImage();
	SetImage(original, IMG_ORIGINAL);


	normalized = NormalizeAutonum(original);

	Decomposition(normalized, components);

	namedWindow("Bin", WINDOW_AUTOSIZE);
//	imshow("Bin", GetImage(IMG_NORM_LINES_FILTERED));

//	imshow("Histo", GetImage(IMG_NORM_LINES_EDGED));
//	imshow("Decomposed", GetImage(IMG_NORM_LINES_PLATOES));
//	imshow("Lines", GetImage(IMG_NORM_LINES));

	imshow("Bin", GetImage(IMG_NORM_BIN));

	imshow("Histo", GetImage(IMG_NORM_BIN_HISTO));
	imshow("Decomposed", GetImage(IMG_NORM_CUTLINES));

	waitKey();
	getchar();
	return 0;
}

