#include "stdafx.h"

void FindRange(vector<int>& histo, int& upper, int& lower)
{
	int i;
	upper = lower = histo.size() / 2;

	for (i = histo.size() / 2; i >= 0; i--)
	{
		if (histo[i] == 0)
			upper = i;
	}
	for (i = histo.size() / 2; i < histo.size(); i++)
	{
		if (histo[i] == 0)
			lower = i;
	}
}

int GetHistoRange(vector<int>& histo, int size)
{
	int begin = 0, end = histo.size() - 1;
	int range = end - begin + 1;
	int upper, lower;

	FindRange(histo, upper, lower);

	while (range > size)
	{
		if (upper > begin)
		{
			if (upper - begin > end - lower)
				begin++;
			else
				end--;
		}
		else if (lower < end)
		{
			if (upper - begin > end - lower)
				begin++;
			else
				end--;
		}
		else if (histo[end] < histo[begin])
		{
			end--;
		}
		else
		{
			begin++;
		}
		range = end - begin + 1;
	}
	return begin;
}

static void HistoVerAnalisys(Mat& component)
{
	vector<int> histogramm;
	int begin = 0;
	int i, j = 0;
	Mat bin;
	cv::Mat_<uchar> bin2;

	threshold(component, bin, 100, 255, THRESH_BINARY_INV);

	for (i = 0; i < bin.rows; i++)
		histogramm.push_back(0);

	bin2 = bin;

	for (j = 0; j < bin.cols; j++)
		for (i = 0; i < bin.rows; i++)
		{
			if (bin2(i, j) > 0)
				histogramm[i]++;
		}

	begin = GetHistoRange(histogramm, NEURON_IMAGE_ROWS);
	component = component.rowRange(begin, begin + NEURON_IMAGE_ROWS);
}

static void HistoHorAnalisys(Mat& component )
{
	vector<int> histogramm;
	int begin = 0;
	int i, j = 0;
	Mat bin;
	cv::Mat_<uchar> bin2;

	threshold(component, bin, 100, 255, THRESH_BINARY_INV);

	for (i = 0; i < bin.cols; i++)
		histogramm.push_back(0);

	bin2 = bin;

	for (i = 0; i < bin.rows; i++)
		for (j = 0; j < bin.cols; j++)
		{
			if (bin2(i, j) > 0)
				histogramm[j]++;
		}

	begin = GetHistoRange(histogramm, NEURON_IMAGE_COLS);
	component = component.colRange(begin, begin + NEURON_IMAGE_COLS);
}

static Mat ComponentPreparing(Mat& component)
{
	Mat prepared = component.clone();
	
	if (prepared.cols > NEURON_IMAGE_COLS)
		HistoHorAnalisys(prepared);

	if (prepared.rows > NEURON_IMAGE_ROWS)
		HistoVerAnalisys(prepared);

	return prepared;
}

static void Gradation(cv::Mat& grad)
{
	int row, col;

	for (row = 0; row < grad.rows; row++)
	{
		for (col = 0; col < grad.cols; col++)
		{
			grad.at<uchar>(row, col) = saturate_cast<uchar>(2.2*(grad.at<uchar>(row, col)));
		}
	}
	medianBlur(grad, grad, 7);
}

int Recognition(vector<Mat>& components)
{
	int i;
	Mat pred, resized, mask;	
	Ptr<ANN_MLP> network;

	network = ANN_MLP::load("config/network.dat");
	
	resized = ComponentPreparing(components[0]);
	//imwrite("00.bmp", resized);
	
	//cv::GaussianBlur(resized, mask, cv::Size(0, 0), 3);
	//cv::addWeighted(mask, 1.5, resized, -0.2, 0, resized);
	//medianBlur(resized, resized, 3);
	//medianBlur(resized, resized, 3);
	//medianBlur(resized, resized, 3);
	//threshold(resized, resized, 100, 255, THRESH_TOZERO);
	//Gradation(resized);
	namedWindow("Bin", WINDOW_AUTOSIZE);

	imshow("One", resized);

	waitKey();
	getchar();
	
//	resize(components[0], resized, Size(52, 40), 0, 0, CV_INTER_AREA);

	resized.convertTo(resized, CV_32FC1, 1.0 / 255.0);

	pred = resized.reshape(0, 1);
	
	if (network->isTrained())
	{
		printf("Predict one-vector:\n");
		Mat result;
		network->predict(pred, result);
		Mat_<float> pres = result;
		for (i = 0; i < result.cols; i++)
		cout << "Class: " << i << " value: " << pres(0,i) << endl;
	}

	return 0;
}
