#include "../stdafx.h"
#include "HaarTrainingModule.h"
#include <cstdio>

const int GOOD_SAMPLES = 301;
const int BAD_SAMPLES = 401;

enum sample_type {
	BAD = 0,
	GOOD
};

static void CreateFileWithSamples(int type)
{
	FILE* fp;
	int i, num;
	Mat tmp;
	string folder = "samples/Numbers/", suffix = ".bmp", dir, name, file;
	char number[5] = "";

	if (type == BAD)
	{
		file = folder + "Bad.dat";
		dir = "Bad/";
		num = BAD_SAMPLES;
	}
	else
	{
		file = folder + "Good.dat";
		dir =  "Good/";
		num = GOOD_SAMPLES;
	}

	fp = fopen(file.c_str(), "w");
	
	for (i = 0; i < num; i++)
	{
		sprintf(number, "%d", i);
		name = folder;
		name += dir;
		name += string(number);
		name += suffix;
		tmp = imread(name, IMREAD_GRAYSCALE);
		if (type == GOOD)
			fprintf(fp, "./%s 1 0 0 %d %d\n", (dir + string(number) + suffix).c_str(), tmp.cols-1, tmp.rows-1);
		else
			fprintf(fp, "./%s\n", (dir + string(number) + suffix).c_str());
	}
	fclose(fp);
}

void HaarTrainingModule()
{
	CreateFileWithSamples(BAD);
	CreateFileWithSamples(GOOD);
}