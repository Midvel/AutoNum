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

	image = imread("20.bmp", IMREAD_UNCHANGED);

	return image;
}

int main()
{
	double coef;
	const double ethalonHeight = 100;
	int cols;
	int rows;
	Mat original;
	Mat normalized;
	Mat detected;
	Mat cut;
	vector<Mat> components;
	std::vector<cv::Rect> areas;
	int train = 1;
	string recognized;
	
	if (0 == train)
	{
		namedWindow("One", WINDOW_AUTOSIZE);
		original = OpenImage();
		SetImage(original, IMG_ORIGINAL);

		detected = Detection(original, areas);
		SetImage(detected, IMG_DETECTED);


		imshow("One", GetImage(IMG_DETECTED));
		waitKey();
		getchar();

		cut = original.colRange(areas[0].x, areas[0].x + areas[0].width);
		cut = cut.rowRange(areas[0].y, areas[0].y + areas[0].height);

		coef = ethalonHeight / cut.rows;
		cols = cut.cols;
		rows = cut.rows;
		resize(cut, cut, cv::Size(coef * cols, ethalonHeight));

		normalized = NormalizeAutonum(cut);

		Decomposition(normalized, components);

		recognized = Recognition(components);
		cout << recognized.c_str() << "\n";

		//		namedWindow("One", WINDOW_AUTOSIZE);

		//		imshow("One", GetImage(IMG_DETECTED));
		//		imshow("Zero", cut);
		//		imshow("Two", GetImage(IMG_NORM_LINES_FILTERED));
		//		imshow("Three", GetImage(IMG_NORM_LINES_EDGED));
		//		imshow("Four", GetImage(IMG_NORM_LINES_PLATOES));
		//		imshow("Five", GetImage(IMG_NORM_LINES));
		//		imshow("Six", GetImage(IMG_NORM_AFFINE));
		//		imshow("Five", GetImage(IMG_NORMALIZED));
		//		imshow("Six", GetImage(IMG_NORM_BIN));
		//		imshow("Seven", GetImage(IMG_NORM_BIN_HISTO));
		imshow("Eight", GetImage(IMG_NORM_CUTLINES));

		waitKey();
		getchar();
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

