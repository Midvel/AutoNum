#include "stdafx.h"

static Mat HoughFiltering(Mat& gray)
{
	Mat filtered = gray.clone();

	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 5);
	equalizeHist(filtered, filtered);
//	Laplacian(filtered, filtered, filtered.depth(), 7);
	medianBlur(filtered, filtered, 5);
	medianBlur(filtered, filtered, 5);
	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 3);

	return filtered;
}

static Mat HoughEdging(Mat& filtered)
{
	Mat edged, canny;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int i;

	Canny(filtered, canny, 50, 200, 3);

	findContours(canny, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));

	edged = Mat::zeros(canny.size(), canny.type());
	for (i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 300 )
			drawContours(edged, contours, i, CV_RGB(255,255,255), 1, 8, hierarchy, 0, Point());
	}


	return edged;
}

static void HoughTransform(Mat& original, vector<Point2f>& linePoints)
{
	size_t i;
	Mat gray, filtered, edged;
	Point pt1, pt2;
	vector<Vec2f> lines;
	float rho, theta, x0, y0;

	cvtColor(original, gray, COLOR_BGR2GRAY);
	
	filtered = HoughFiltering(gray);

	edged = HoughEdging(filtered);
	

/*	HoughLines(edged, lines, 100, CV_PI / 180, 50, 0, 0, CV_PI / 4,  3*CV_PI / 4);

	Mat color;
	cvtColor(edged, color, COLOR_GRAY2BGR);
	for (i = 0; i < lines.size(); i++)
	{
		rho = lines[i][0];
		theta = lines[i][1];
		double a = cos(theta), b = sin(theta);

		x0 = rho * cos(theta);
		y0 = rho * sin(theta);

		linePoints.push_back(Point2f(x0, y0));
		linePoints.push_back(Point2f(x0 + gray.cols * sin(theta), y0 - gray.rows * cos(theta)));
		
		pt1.x = cvRound(x0);
		pt1.y = cvRound(y0);
		cout << "Point1 " << pt1.x << " " << pt1.y << "\n";
		
		pt2.x = cvRound(x0 + gray.cols * sin(theta));
		pt2.y = cvRound(y0 - gray.rows * cos(theta));
		cout << "Point2 " << pt2.x << " " << pt2.y << "\n";
		
		line(original, pt1, pt2, Scalar(0, 0, 255), 1, CV_AA);
	}*/

	namedWindow("Gray", WINDOW_AUTOSIZE);
	imshow("Gray", filtered);

	namedWindow("Edge", WINDOW_AUTOSIZE);
	imshow("Edge", edged);

	namedWindow("Lines", WINDOW_AUTOSIZE);
	imshow("Lines", original);

	waitKey();
	getchar();

	return;
}



Mat NormalizeAutonum( Mat& original )
{
	Mat gray;
	Mat edge, edge_color, aphin;
	Mat normalized;
	vector<Point2f> linePoints;
	

	HoughTransform( original, linePoints );



/*	cvtColor(edge, edge_color, COLOR_GRAY2BGR);
	aphin = edge_color.clone();



	vector<Point2f> input;
	vector<Point2f> output;

	input.push_back(Point2f(linesPoints[0]));
	input.push_back(Point2f(linesPoints[1]));
	input.push_back(Point2f(linesPoints[2]));
	output.push_back(Point2f(linesPoints[0]));
	output.push_back(Point2f(linesPoints[1].x, linesPoints[0].y));
	output.push_back(Point2f(linesPoints[2]));

	Mat warpMat = getAffineTransform(input, output);

	warpAffine(gray, aphin, warpMat, edge.size(), INTER_LINEAR, BORDER_REFLECT_101);

	Mat cropped(aphin, Rect(0, linesPoints[0].y + 10, aphin.cols, linesPoints[2].y - linesPoints[0].y - 10));

	Mat contrast;
	medianBlur(cropped, contrast, 3);
	equalizeHist(contrast, contrast);

	Mat bin;

	threshold(contrast, bin, 100, 255, THRESH_BINARY_INV);*/

	Mat bin;

	return bin;
}