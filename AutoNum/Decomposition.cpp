#include "stdafx.h"

const int DECOM_LOCAL_LENGTH = 10;

static Mat DrawBinHisto(Mat& bin, vector<int>& histogramm)
{
	int i, j = 0;
	Mat histo = Mat::zeros(bin.size(), CV_8U);
	Scalar color = Scalar(255, 255, 255);

	for (i = 0; i < bin.cols; i++)
		histogramm.push_back(0);

	cv::Mat_<uchar> bin2 = bin;

	for (i = 0; i < bin.rows; i++)
		for (j = 0; j < bin.cols; j++)
		{
			if (bin2(i, j) > 0)
				histogramm[j]++;
		}

	for (i = 0; i < bin.cols; i++)
	{
		circle(histo, Point(i, histogramm[i] - 1), 1, Scalar(255, 255, 255), FILLED, LINE_8);
	}
	return histo;
}

static Mat CropDecomposed(Mat& normalized, vector<int>& histogramm)
{
	Mat cropped;
	int i;
	int leftSide = 0, rightSide = histogramm.size();

	for (i = 0; i < histogramm.size(); i++)
	{
		if (histogramm[i] <= 7 * normalized.rows / 8)
		{
			leftSide = i;
			break;
		}
	}
	for (i = histogramm.size() - 1; i >= 0; i--)
	{
		if (histogramm[i] <= 7 * normalized.rows / 8)
		{
			rightSide = i;
			break;
		}
	}
	histogramm.erase(histogramm.begin() + rightSide, histogramm.end());
	histogramm.erase(histogramm.begin(), histogramm.begin() + leftSide);

	cropped = normalized.colRange(leftSide, rightSide);
	return cropped;
}

static bool CheckLocal(vector<int>& histogramm, int num)
{
	bool bCheck = true;
	int i;

	i = num - DECOM_LOCAL_LENGTH;
	if (i < 0)
	{
		i = 0;
	}

	for (; bCheck && i < num; i++)
		if (histogramm[i] < histogramm[num])
			bCheck = false;

	i = num + DECOM_LOCAL_LENGTH;
	if (i >= histogramm.size())
	{
		i = histogramm.size() - 1;
	}

	for (; bCheck && i > num; i--)
		if (histogramm[i] < histogramm[num])
			bCheck = false;

	return bCheck;
}

static Mat CutDecomposed(Mat& cropped, vector<int>& histogramm, vector<int>& cutPoints)
{
	Mat decomposed = cropped.clone();
	int i, num, nStart = -1, nEnd = 0;
	vector<int> checkedPoints;

	cvtColor(decomposed, decomposed, CV_GRAY2RGB);

	for (i = 0; i < histogramm.size(); i++)
	{
		if (CheckLocal(histogramm, i))
		{
			checkedPoints.push_back(i);
//			line(decomposed, Point(i, 0), Point(i, decomposed.rows), Scalar(255, 0, 0), 1, CV_AA);
		}
	}

	for (i = 0; i < checkedPoints.size() - 1; i++)
	{
		if (checkedPoints[i] == checkedPoints[i + 1] - 1)
		{
			if (nStart == -1)
			{
				nStart = i;
				nEnd = i + 1;
			}
			else
			{
				nEnd = i + 1;
			}
		}
		else
		{
			if (nStart == -1)
			{
				if (histogramm[checkedPoints[i]] < cropped.rows / 4 - 4)
					cutPoints.push_back(checkedPoints[i]);
			}
			else
			{
				num = nStart + (nEnd - nStart) / 2;
				//num = nEnd;
				if (histogramm[checkedPoints[num]] < cropped.rows / 4 - 4)
					cutPoints.push_back(checkedPoints[num]);
				nStart = -1;
				nEnd = -1;
			}
		}
	}

	for (i = 0; i < cutPoints.size(); i++)
	{
		line(decomposed, Point(cutPoints[i], 0), Point(cutPoints[i], decomposed.rows), Scalar(0, 0, 255), 1, CV_AA);
	}

	return decomposed;
}

static Mat AnalizeHistogramm(Mat& normalized, vector<int>& histogramm, vector<int>& cutPoints)
{
	Mat cropped, decomposed;

	cropped = CropDecomposed(normalized, histogramm);

	decomposed = CutDecomposed(cropped, histogramm, cutPoints);
	SetImage(decomposed, IMG_NORM_CUTLINES);

	return cropped;
}

void Decomposition(Mat& normalized, vector<Mat>& components)
{
	int i;
	Mat bin, histo, cropped;
	vector<int> histogramm;
	vector<int> cutPoints;

	medianBlur(normalized, bin, 3);
	threshold(bin, bin, 100, 255, THRESH_BINARY_INV);
	SetImage(bin, IMG_NORM_BIN);

	histo = DrawBinHisto(bin, histogramm);
	SetImage(histo, IMG_NORM_BIN_HISTO);

	cropped = AnalizeHistogramm(normalized, histogramm, cutPoints);

	if (cutPoints.size() > 0)
	{
		for (i = 0; i < cutPoints.size() - 1; i++)
		{
			if (cutPoints[i + 1] - cutPoints[i] > 15)
			{
				components.push_back(cropped.colRange(cutPoints[i], cutPoints[i + 1]));
			}
		}
	}

	return;
}