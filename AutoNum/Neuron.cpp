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

string ResultToString(Mat result)
{
	string recognized;
	int i;
	int chosenClass;
	float chosenValue = 0;
	Mat_<float> pres = result;

	chosenClass = GARBAGE;
	chosenValue = pres(0, GARBAGE);

	for (i = 0; i < result.cols - 1; i++)
	{
		if (pres(0, i) > chosenValue)
		{
			chosenClass = i;
			chosenValue = pres(0, i);
		}
	}

	if (chosenValue < 0.5)
	{
		if (chosenValue < pres(0, GARBAGE))
			chosenClass = GARBAGE;
	}

	if (chosenClass == GARBAGE)
	{
		recognized = "*";
	}
	else
	{
		recognized = GetClassName(chosenClass);
	}

	return recognized;
}

string Recognition(vector<Mat>& components)
{
	string recognized;
	string resstr;
	int i, j;
	Mat pred, resized, mask;	
	Ptr<ANN_MLP> network;
	FILE* fp;
	network = ANN_MLP::load("config/network.dat");
	fp = fopen("results.txt", "w");
	
	for (i = 0; i < components.size(); i++)
	{
		//resized = ComponentPreparing(components[0]);
		resize(components[i], resized, Size(NEURON_IMAGE_COLS, NEURON_IMAGE_ROWS), 0, 0, CV_INTER_AREA);
		//Pre-filtering?

		resized.convertTo(resized, CV_32FC1, 1.0 / 255.0);

		pred = resized.reshape(0, 1);

		if (network->isTrained())
		{
			Mat result;
			network->predict(pred, result);
			Mat_<float> pres = result;
			fprintf(fp, "Predict for fragment %d:\n", i);
			for (j = 0; j < result.cols; j++)
			{
				fprintf(fp, "Class \<%s\> value: %f\n", GetClassName(j).c_str(), pres(0, j));
			}
			resstr = ResultToString(result);
			recognized += resstr;
			fprintf(fp, "Chosen class: <%s>", resstr.c_str());
			fprintf(fp, "\n");
		}
	}
	fprintf(fp, "Result: %s", recognized.c_str());
	fprintf(fp, "\n");
	fclose(fp);

	return recognized;
}
