#include "stdafx.h"

struct InsideDistance
{
	int top;
	int bottom;
	int distance;
};

const int PLATO_MIN_LENGTH = 10;
const int PLATO_DELTA_Y = 2;
const int PLATO_DELTA_LOCAL = 4;
const int PLATO_LOCAL_LENGTH = 5;

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

static InsideDistance SetDistance(int** contourMatrix, int col, int height)
{
	InsideDistance dist = {-1,-1,0};
	int rowTop = height * 0.5, rowBottom = height * 0.5;

	while (rowTop >= 0)
	{
		if (contourMatrix[rowTop][col] >= 0)
		{
			dist.top = contourMatrix[rowTop][col];
			break;
		}
		else
			rowTop--;
	}

	while (rowBottom < height)
	{
		if (contourMatrix[rowBottom][col] >= 0)
		{
			dist.bottom = contourMatrix[rowBottom][col];
			break;
		}
		else
			rowBottom++;
	}
	dist.distance = rowBottom - rowTop + 1;
	return dist;
}

static void GetDistances(int** contourMatrix, vector<InsideDistance>& dist, int width, int height)
{
	int i, j;
	InsideDistance curDist;

	for (j = 0; j < width; j++)
	{
		curDist = SetDistance(contourMatrix, j, height);
		dist.push_back(curDist);
	}

}

static bool CheckDistance(vector<InsideDistance>& distTotal, int cur)
{
	int i;
	bool bOk = true;
	for (i = PLATO_LOCAL_LENGTH; i > 0; i--)
		if (abs(distTotal[cur - i].distance - distTotal[cur - i + 1].distance) > PLATO_DELTA_Y)
		{
			bOk = false;
			break;
		}

	return  bOk && abs(distTotal[cur].distance - distTotal[cur - PLATO_LOCAL_LENGTH].distance) <= PLATO_DELTA_LOCAL &&
			distTotal[cur].bottom != -1 && distTotal[cur].top != -1;
}

static int GetPlato(vector<InsideDistance>& distTotal, vector<int>& platoes)
{
	int i;
	bool bPlatoStarted = false;
	int nCurPlatoLength = 0;

	for (i = PLATO_LOCAL_LENGTH; i < distTotal.size(); i++)
	{
		if (CheckDistance(distTotal, i))
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
	return platoes.size();
}

static void MakeLines(vector<vector<Point>>& contours, vector<InsideDistance>& distTotal, vector<int>& platoes, vector<Vec2f>& lines)
{

}

Mat ContourAnalysis(vector<vector<Point>>& contours, vector<Vec2f>& lines, int width, int height)
{
	int** contourMatrix = nullptr;
	vector<InsideDistance> distTotal;
	vector<int> platoes;
	int nPlato = 0;

	CreateContourMatrix(contourMatrix, width, height);
	FillContourMatrix(contourMatrix, contours);

	GetDistances(contourMatrix, distTotal, width, height);

	nPlato = GetPlato(distTotal, platoes);

	if (nPlato > 0)
	{
		MakeLines(contours, distTotal, platoes, lines);
	}
	int j = 0;
	Mat histo = Mat::zeros(Size(width, height), CV_8U);
	Scalar color = Scalar(155, 155, 155);
	for (int i = 0; i < width; i++)
	{
		if (nPlato > 0 && j < nPlato && i == platoes[j])
			color = Scalar(255, 255, 255);
		if (nPlato > 0 && j < nPlato && i == platoes[j + 1])
		{
			color = Scalar(155, 155, 155);
			j += 2;
		}
		line(histo, Point(i, height - 1), Point(i, height - distTotal[i].distance), color, 1, CV_AA);
	}

	DeleteContourMatrix(contourMatrix, height);

	return histo;
}