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
	int train = 0;
	
	if (0 == train)
	{
		original = OpenImage();
		SetImage(original, IMG_ORIGINAL);


		normalized = NormalizeAutonum(original);

	//	Decomposition(normalized, components);

			namedWindow("One", WINDOW_AUTOSIZE);

			imshow("One", GetImage(IMG_ORIGINAL));
			imshow("Two", GetImage(IMG_NORM_LINES_FILTERED));
			imshow("Three", GetImage(IMG_NORM_LINES_EDGED));
			imshow("Four", GetImage(IMG_NORM_LINES_PLATOES));
			imshow("Five", GetImage(IMG_NORM_LINES));

			waitKey();
			getchar();

		//Recognition(components);
	}
	else if (1 == train)
	{
		NeuronTrainingModule();
	}
	else
	{
		HaarTrainingModule();
	}
	return 0;
}

