#include "stdafx.h"

/*-----------------------------------------------------------------------------------------------*/

//Pack of functions to get supporting lines for auto number.
static Mat SupportingLinesFiltering(Mat& gray)
{
	Mat filtered = gray.clone();

	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 5);
	equalizeHist(filtered, filtered);
	medianBlur(filtered, filtered, 5);
	medianBlur(filtered, filtered, 5);
	medianBlur(filtered, filtered, 3);
	medianBlur(filtered, filtered, 3);

	return filtered;
}

static Mat SupportingLinesCountours(Mat& filtered, vector<vector<Point>>& contours)
{
	Mat edged, canny;
	vector<Vec4i> hierarchy;
	int i;

	Canny(filtered, canny, 50, 200, 3);

	findContours(canny, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));

	edged = Mat::zeros(canny.size(), canny.type());
	for (i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 300)
		{
			drawContours(edged, contours, i, CV_RGB(255, 255, 255), 1, 8, hierarchy, 0, Point());
		}
		else
		{
			contours.erase(contours.begin() + i);
			i--;
		}

	}
	return edged;
}

static Mat DrawSupportingLines(Mat& original, vector<Vec2f>& line_coefs)
{
	Mat lines = original.clone();
	float a1 = line_coefs[0][0];
	float b1 = line_coefs[0][1];
	float a2 = line_coefs[1][0];
	float b2 = line_coefs[1][1];

	line(lines, Point(0, b1), Point(lines.cols - 1, a1 * (lines.cols - 1) + b1), Scalar(255, 0, 0), 1, CV_AA);
	line(lines, Point(0, b2), Point(lines.cols - 1, a2 * (lines.cols - 1) + b2), Scalar(255, 0, 0), 1, CV_AA);

	return lines;
}
static void GetSupportingLines(Mat& original, vector<Vec2f>& line_coefs)
{
	Mat gray, filtered, edged, histo, lines;
	vector<vector<Point>> contours;
	

	cvtColor(original, gray, COLOR_BGR2GRAY);
	
	filtered = SupportingLinesFiltering(gray);
	SetImage(filtered, IMG_NORM_LINES_FILTERED);

	edged = SupportingLinesCountours(filtered, contours);
	SetImage(edged, IMG_NORM_LINES_EDGED);

	histo = ContourAnalysis(contours, line_coefs, filtered.cols, filtered.rows);
	SetImage(histo, IMG_NORM_LINES_PLATOES);

	lines = DrawSupportingLines(original, line_coefs);
	SetImage(lines, IMG_NORM_LINES);
	
	
	return;
}

/*-----------------------------------------------------------------------------------------------*/

//Pack of functions to provide affine tranformation on auto number if it is turned.
static void GetAffineInput(vector<Point2f>& input, vector<Vec2f>& line_coefs, int width)
{
	float a_top = line_coefs[0][0], b_top = line_coefs[0][1];
	float a_bottom = line_coefs[1][0], b_bottom = line_coefs[1][1];
	float a_perp, b_perp;

	Point2f P_left, P_right, P_bottom;

	P_left.x = width / 4.0;
	P_left.y = a_top * P_left.x + b_top;

	P_right.x = 3.0 * width / 4.0;
	P_right.y = a_top * P_right.x + b_top;

	//get line perpendicular to the top line throuh the central point
	a_perp = -1.0 / a_top;
	b_perp = (a_top * width * 0.5 + b_top) + width * 0.5 / a_top;

	//get point of intersection of perpendicular and the bottom line
	P_bottom.x = (b_perp - b_bottom) / (a_bottom - a_perp);
	P_bottom.y = a_bottom * P_bottom.x + b_bottom;

	input.push_back(P_left);
	input.push_back(P_right);
	input.push_back(P_bottom);
}

static void GetAffineOutput(vector<Point2f>& output, vector<Point2f>& input, float coef)
{
	Point2f P_left, P_right, P_bottom;
	float angle = (-1.0) * atanf(coef);

	P_left = input[0];
	
	P_right.y = P_left.y;
	P_right.x = (input[1].x - P_left.x)*cos(angle) - (input[1].y - P_left.y)*sin(angle) + P_left.x;

	P_bottom.x = (input[2].x - P_left.x)*cos(angle) - (input[2].y - P_left.y)*sin(angle) + P_left.x;
	P_bottom.y = (input[2].x - P_left.x)*sin(angle) + (input[2].y - P_left.y)*cos(angle) + P_left.y;

	output.push_back(P_left);
	output.push_back(P_right);
	output.push_back(P_bottom);
}


static Mat MakeAffine(Mat& original, vector<Vec2f>& line_coefs)
{
	vector<Point2f> input, output;
	Mat transform, affine, cropped;

	affine = original.clone();

	if ((int)round(line_coefs[0][0] * 1000) != 0)
	{
		GetAffineInput(input, line_coefs, original.cols);
		GetAffineOutput(output, input, line_coefs[0][0]);

		transform = getAffineTransform(input, output);

		warpAffine(original, affine, transform, original.size(), INTER_LINEAR, BORDER_REFLECT_101);

		if (output[0].y < 0)
			output[0].y = 0;
		if (output[2].y > affine.rows)
			output[2].y = affine.rows;
		cropped = affine.rowRange(output[0].y, output[2].y);
	}
	else
	{
		cropped = affine.rowRange(line_coefs[0][1], line_coefs[1][1]);
	}

	return cropped;
}

/*-----------------------------------------------------------------------------------------------*/

//Main function in normalization module. Provides all operation on image with auto number area to get 
//image prepared to neuronet processing.
Mat NormalizeAutonum( Mat& original )
{
	Mat affine, contrast, gray, cropped, bin, histo;
	Mat normalized;
	vector<Vec2f> line_coefs(2); //coefficients for supporting lines 0 - top line, 1 - bottom line

	GetSupportingLines(original, line_coefs);
	
	affine = MakeAffine(original, line_coefs);

	cropped = affine.rowRange(2, affine.rows - 2);
	SetImage(cropped, IMG_NORM_AFFINE);

	cvtColor(cropped, gray, COLOR_RGB2GRAY);
	
	medianBlur(gray, contrast, 3);
	equalizeHist(contrast, normalized);
	SetImage(normalized, IMG_NORMALIZED);

	return normalized;
}