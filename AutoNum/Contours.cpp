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

const int CONTOUR_DELTA_LOCAL = 4;

enum directions {
	DIST_TOP = 1,
	DIST_BOTTOM,
	DIST_LEFT,
	DIST_RIGHT
};

enum signs {
	SIGN_NEG = -1,
	SIGN_ZERO = 0,
	SIGN_POS = 1
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
	int j;
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
/*static bool CheckFalseDirection(int direction, int x, int xStart)
{
	if (direction == DIST_LEFT)
		return x < xStart;
	else if (direction == DIST_RIGHT)
		return x > xStart;
	else
		throw "CheckFalseDirection(): wrong direction.";
}

static bool CheckTrueDirection(int direction, int x, int xStart)
{
	if (direction == DIST_LEFT)
		return x > xStart;
	else if (direction == DIST_RIGHT)
		return x < xStart;
	else
		throw "CheckTrueDirection(): wrong direction.";
}

static bool CheckSign(vector<Point>& contour, int startNum, int sign, int direction)
{
	bool bChecked = false;
	int i = startNum + sign;

	while (i != startNum)
	{
		if (i == contour.size() && sign == SIGN_POS)
			i = 0;
		else if (i == -1 && sign == SIGN_NEG)
			i = contour.size() - 1;

		if (CheckFalseDirection(direction, contour[i].x,contour[startNum].x))
			break;
		else if (CheckTrueDirection(direction, contour[i].x, contour[startNum].x))
		{
			bChecked = true;
			break;
		}
		i += sign;
	}
	return bChecked;
}

static int GetContourCicleSign(vector<Point>& contour, int startNum, int direction)
{
	int i;
	bool bSign = false;
	int sign;

	sign = SIGN_POS;
	bSign = CheckSign(contour, startNum, sign, direction);

	if (!bSign)
	{
		sign = SIGN_NEG;
		bSign = CheckSign(contour, startNum, sign, direction);
	}

	if (!bSign)
		sign = SIGN_ZERO;

	return sign;
}*/

/*static bool CheckLineEdge(vector<Point>& contour, int sign, int cur)
{
	int i;
	bool bOk = true;


	if (abs(contour[i].x - contour[cur].x) <= 1 && abs(contour[i].y - contour[cur].y) <= CONTOUR_DELTA_LOCAL )

	for (i = PLATO_LOCAL_LENGTH; i > 0; i--)
		if (abs(distances[cur - i].distance - distances[cur - i + 1].distance) > PLATO_DELTA_Y)
		{
			bOk = false;
			break;
		}

	return  bOk && abs(distances[cur].distance - distances[cur - PLATO_LOCAL_LENGTH].distance) <= PLATO_DELTA_LOCAL &&
		distances[cur].bottom != -1 && distances[cur].top != -1;
}*/

/*static void GetLineEdgePoints(vector<Point>& line, vector<Point>& contour, int xStart, int yStart, int direction)
{
	int i, startNum = 0;
	Point pStart, pTmp;
	bool bFound = false;
	int sign;

	while (startNum < contour.size() && contour[startNum].x != xStart && contour[startNum].y != yStart)
	{
		startNum++;
	}
	if (startNum < contour.size())
		pStart = contour[startNum];
	else
		throw "GetLineEdgePoints(): no point in the contour.";

	sign = GetContourCicleSign(contour, startNum, direction);

	//for (i = 0; i < )

}*/

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
	int col;
	int longest_plato = GetLongestPlato(platoes);
	int leftTopContourNum = distances[2 * longest_plato].top;
	int rightTopContourNum = distances[2 * longest_plato + 1].top;
	int leftBottomContourNum = distances[2 * longest_plato].bottom;
	int rightBottomContourNum = distances[2 * longest_plato + 1].bottom;
	int platoStart = platoes[2 * longest_plato];
	int platoEnd = platoes[2 * longest_plato + 1];
	vector<Point> topLine, bottomLine;

	//GetLineEdgePoints(topLine, contours[leftTopContourNum], platoStart, distances[platoStart].row_top, DIST_LEFT);

	for (col = platoStart; col <= platoEnd; col++)
	{
		topLine.push_back(Point(col, distances[col].row_top));
		bottomLine.push_back(Point(col, distances[col].row_bottom));
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