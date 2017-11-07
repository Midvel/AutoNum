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
	
	original = OpenImage();

	normalized = NormalizeAutonum(original);

	//calcHist

//	namedWindow("Result", WINDOW_AUTOSIZE);
	//imshow("Result", normalized);

//	waitKey(1000);
//	getchar();
	return 0;
}

