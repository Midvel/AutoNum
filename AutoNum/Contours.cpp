#include "stdafx.h"

struct InsideDistance
{
	int top;		//contour num
	int row_top;	//y
	int bottom;		//contour num
	int row_bottom;	//y
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

/*-----------------------------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------------------------*/

//Set of functions to build lines based on points which belong to contours which bounder the longest plato.
int ContourIndex(int i, int size, int direction)
{
	if (direction == DIST_RIGHT)
	{
		i++;
		if (i == size)
			i = 0;
	}
	else
	{
		i--;
		if (i == -1)
			i = size - 1;
	}
	return i;
}

bool ContourEdge(vector<Point>& contour, int i, int iPrev, int edge_direction)
{
	if (edge_direction = DIST_LEFT)
		return contour[i].x > contour[iPrev].x;
	else
		return contour[i].x < contour[iPrev].x;
}
int GetContourEdge(vector<Point>& contour, int lineEnd, int contour_direction, int edge_direction)
{
	int iPrev, i = lineEnd;
	int lineStart = lineEnd;

	do
	{
		iPrev = i;
		i = ContourIndex(i, contour.size(), contour_direction);

		if ( ContourEdge(contour, i, iPrev, edge_direction))
			break;
		else
			lineStart = i;

	} while (i != lineEnd);
	return lineStart;
}

void GetContourEdgePoints(vector<Point>& line_points, vector<Point>& contour, Point edgePoint, Point nextPoint, int edge_direction, double averageDxDy)
{
	int i, edgePointNum = -1, nextPointNum = -1;
	int contour_direction;
	int lineBegin;
	double curDxDy = 0;

	for (i = 0; i < contour.size(); i++)
	{
		if (contour[i].x == edgePoint.x && contour[i].y == edgePoint.y)
			edgePointNum = i;
		else if (contour[i].x == nextPoint.x && contour[i].y == nextPoint.y)
			nextPointNum = i;
		if (edgePointNum != -1 && nextPointNum != -1)
			break;
	}

	contour_direction = nextPointNum > edgePointNum ? DIST_LEFT : DIST_RIGHT;

	lineBegin = GetContourEdge(contour, edgePointNum, contour_direction, edge_direction);

	for (i = ContourIndex(edgePointNum, contour.size(), contour_direction); i != lineBegin; i = ContourIndex(i, contour.size(), contour_direction))
	{
		if (edgePoint.y - contour[i].y != 0)
			curDxDy = (double)(edgePoint.x - contour[i].x) / ((edgePoint.y - contour[i].y)*(edgePoint.y - contour[i].y));
		if (edgePoint.y - contour[i].y != 0 && abs(curDxDy / averageDxDy) < 1)
			break;
		line_points.push_back(contour[i]);
	}
}

/*-----------------------------------------------------------------------------------------------*/

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
	int platoStart = platoes[2 * longest_plato] - 1;
	int platoEnd = platoes[2 * longest_plato + 1] - 1;
	int leftTopContourNum = distances[platoStart].top;
	int rightTopContourNum = distances[platoEnd].top;
	int leftBottomContourNum = distances[platoStart].bottom;
	int rightBottomContourNum = distances[platoEnd].bottom;
	vector<Point> topLine, bottomLine;
	int tmp;

	double averageTopDxDy = 1, averageBottomDxDy = 1;

	if (distances[platoEnd].row_top != distances[platoStart].row_top)
	{
		tmp = distances[platoEnd].row_top - distances[platoStart].row_top;
		averageTopDxDy = (double)(platoEnd - platoStart) / (tmp*tmp);

	}
	if (distances[platoEnd].row_bottom != distances[platoStart].row_bottom)
	{
		tmp = distances[platoEnd].row_bottom - distances[platoStart].row_bottom;
		averageBottomDxDy = (double)(platoEnd - platoStart) / (tmp*tmp);
	}

	GetContourEdgePoints(topLine, contours[leftTopContourNum], 
									Point(platoStart, distances[platoStart].row_top), 
									Point(platoStart + 1, distances[platoStart + 1].row_top), DIST_LEFT, averageTopDxDy);
	GetContourEdgePoints(bottomLine, contours[leftBottomContourNum],
									Point(platoStart, distances[platoStart].row_bottom),
									Point(platoStart + 1, distances[platoStart + 1].row_bottom), DIST_LEFT, averageBottomDxDy);

	for (col = platoStart; col <= platoEnd; col++)
	{
		topLine.push_back(Point(col, distances[col].row_top));
		bottomLine.push_back(Point(col, distances[col].row_bottom));
	}

	GetContourEdgePoints(topLine, contours[rightTopContourNum],
									Point(platoEnd, distances[platoEnd].row_top),
									Point(platoEnd - 1, distances[platoEnd - 1].row_top), DIST_RIGHT, averageTopDxDy);
	GetContourEdgePoints(bottomLine, contours[rightBottomContourNum],
									Point(platoEnd, distances[platoEnd].row_bottom),
									Point(platoEnd - 1, distances[platoEnd - 1].row_bottom), DIST_RIGHT, averageBottomDxDy);

	ApproxLine(topLine, coefs[0]);
	ApproxLine(bottomLine, coefs[1]);
}

/*-----------------------------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------------------------*/

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