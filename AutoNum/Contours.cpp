#include "stdafx.h"

struct InsideDistance
{
	int top;
	int row_top;
	int bottom;
	int row_bottom;
	int distance;
};

const int PLATO_MIN_LENGTH = 10;
const int PLATO_DELTA_Y = 2;
const int PLATO_DELTA_LOCAL = 4;
const int PLATO_LOCAL_LENGTH = 5;

enum directions {
	DIST_TOP = 1,
	DIST_BOTTOM,
	DIST_LEFT,
	DIST_RIGHT
};

//Contour matrix is a combination of image (points coordinates) and contours (contour number).
//Such construction will minimize time of contour analysis: matrix is passed the single time 
//instead of passing some number of lists several times. 
static void CreateContourMatrix(int**& contourMatrix, int width, int height)
{
	int i, j;

	contourMatrix = new int*[height];

	for (i = 0; i < height; i++)
	{
		contourMatrix[i] = new int[width];
	}

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			contourMatrix[i][j] = -1;
}

static void FillContourMatrix(int**& contourMatrix, vector<vector<Point>>& contours)
{
	int i, j, row, col;

	for (i = 0; i < contours.size(); i++)
		for (j = 0; j < contours[i].size(); j++)
		{
			row = contours[i][j].y;
			col = contours[i][j].x;
			contourMatrix[row][col] = i;
		}
}

static void DeleteContourMatrix(int**& contourMatrix, int height)
{
	int i;

	for (i = 0; i < height; i++)
	{
		if (contourMatrix[i])
			delete[] contourMatrix[i];
	}
	if (contourMatrix)
		delete[] contourMatrix;
}

//Set of functions to pass through contour matrix and distances between upper and bottom sides of auto number.
static InsideDistance SetDistance(int** contourMatrix, int col, int height)
{
	InsideDistance dist = {-1, height * 0.5,-1, height * 0.5,0};

	while (dist.row_top >= 0)
	{
		if (contourMatrix[dist.row_top][col] >= 0)
		{
			dist.top = contourMatrix[dist.row_top][col];
			break;
		}
		else
			dist.row_top--;
	}

	while (dist.row_bottom < height)
	{
		if (contourMatrix[dist.row_bottom][col] >= 0)
		{
			dist.bottom = contourMatrix[dist.row_bottom][col];
			break;
		}
		else
			dist.row_bottom++;
	}
	dist.distance = dist.row_bottom - dist.row_top + 1;
	return dist;
}

static void GetDistances(int** contourMatrix, vector<InsideDistance>& dist, int width, int height)
{
	int i, j;
	InsideDistance cur_distance;

	for (j = 0; j < width; j++)
	{
		cur_distance = SetDistance(contourMatrix, j, height);
		dist.push_back(cur_distance);
	}

}

//Set of functions to transform distances into set of platoes - stationary parts distances histogramm.
static bool CheckDistance(vector<InsideDistance>& distances, int cur)
{
	int i;
	bool bOk = true;
	for (i = PLATO_LOCAL_LENGTH; i > 0; i--)
		if (abs(distances[cur - i].distance - distances[cur - i + 1].distance) > PLATO_DELTA_Y)
		{
			bOk = false;
			break;
		}

	return  bOk && abs(distances[cur].distance - distances[cur - PLATO_LOCAL_LENGTH].distance) <= PLATO_DELTA_LOCAL &&
			distances[cur].bottom != -1 && distances[cur].top != -1;
}

static int GetPlato(vector<InsideDistance>& distances, vector<int>& platoes)
{
	int i;
	bool bPlatoStarted = false;
	int nCurPlatoLength = 0;

	for (i = PLATO_LOCAL_LENGTH; i < distances.size(); i++)
	{
		if (CheckDistance(distances, i))
		{
			if (bPlatoStarted)
				nCurPlatoLength++;
			else
			{
				bPlatoStarted = true;
				nCurPlatoLength = PLATO_LOCAL_LENGTH;
			}
		}
		else
		{
			if (nCurPlatoLength >= PLATO_MIN_LENGTH)
			{
				platoes.push_back(i - nCurPlatoLength);
				platoes.push_back(i);
			}
			nCurPlatoLength = 0;
			bPlatoStarted = false;
		}

	}
	return platoes.size() / 2;
}

static int GetLongestPlato(vector<int>& platoes)
{
	size_t i;
	int maxlen = 0;
	int maxnum = 0;

	for (i = 0; i < platoes.size() / 2; i++)
	{
		if (maxlen < platoes[2 * i + 1] - platoes[2 * i])
		{
			maxlen = platoes[2 * i + 1] - platoes[2 * i];
			maxnum = i;
		}
	}
	return maxnum;
}

//Set of functions to build lines based on points which belong to contours which bounder the longest plato.
static void ApproxLine(vector<Point>& line_points, Vec2f& coefs)
{
	double a, b;
	int i, n = line_points.size();

	double sumX = 0, sumX2 = 0, sumXY = 0, sumY = 0;

	for (i = 0; i < n; i++)
	{
		sumX += line_points[i].x;
		sumX2 += (line_points[i].x * line_points[i].x);
		sumY += line_points[i].y;
		sumXY += (line_points[i].x * line_points[i].y);
	}

	a = ((double)n*sumXY - sumX*sumY) / ((double)n*sumX2 - sumX*sumX);
	b = (sumY - a*sumX) / (double)n;

	coefs[0] = a;
	coefs[1] = b;
}

static void MakeLines(vector<vector<Point>>& contours, vector<InsideDistance>& distances, vector<int>& platoes, vector<Vec2f>& coefs)
{
	int i;
	int longest_plato = GetLongestPlato(platoes);

	vector<Point> topLine, bottomLine;

	for (i = platoes[2 * longest_plato]; i <= platoes[2 * longest_plato + 1]; i++)
	{
		topLine.push_back(Point(i, distances[i].row_top));
		bottomLine.push_back(Point(i, distances[i].row_bottom));
	}
	ApproxLine(topLine, coefs[0]);
	ApproxLine(bottomLine, coefs[1]);
}

static Mat DrawHistogramm(vector<InsideDistance>& distances, vector<int>& platoes, int width, int height)
{
	int i, j = 0;
	Mat histo = Mat::zeros(Size(width, height), CV_8U);
	Scalar color = Scalar(155, 155, 155);
	int nPlato = platoes.size() / 2;

	for (i = 0; i < width; i++)
	{
		if (nPlato > 0 && j < nPlato && i == platoes[j])
			color = Scalar(255, 255, 255);
		if (nPlato > 0 && j < nPlato && i == platoes[j + 1])
		{
			color = Scalar(155, 155, 155);
			j += 2;
		}
		line(histo, Point(i, height - 1), Point(i, height - distances[i].distance), color, 1, CV_AA);
	}
	return histo;
}

//Main function in contour analisys module.
//contours - contours of cropped area with number
//line_coefs - returned value with coefficients of boundary lines of number
Mat ContourAnalysis(vector<vector<Point>>& contours, vector<Vec2f>& line_coefs, int width, int height)
{
	int** contourMatrix = nullptr;
	vector<InsideDistance> distances;
	vector<int> platoes;
	int nPlato = 0;
	Mat histo;

	CreateContourMatrix(contourMatrix, width, height);
	FillContourMatrix(contourMatrix, contours);

	GetDistances(contourMatrix, distances, width, height);

	nPlato = GetPlato(distances, platoes);

	histo = DrawHistogramm(distances, platoes, width, height);

	if (nPlato > 0)
	{
		MakeLines(contours, distances, platoes, line_coefs);
	}

	DeleteContourMatrix(contourMatrix, height);

	return histo;
}