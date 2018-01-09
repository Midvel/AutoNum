#include "../stdafx.h"
#include "HaarTrainingModule.h"
#include <cstdio>

static void CreateFileWithSamples(int type)
{
	FILE* fp;
	int i, num;
	Mat tmp;
	string file, filename, filesuffix = ".dat";
	string mainfolder = "utils/", folder = "samples/Numbers/", sample_folder, suffix = ".bmp";
	string openfile, writefile;
	char number[5] = "";

	sample_folder = filename = string(SAMPLES[type].folder);
	file = mainfolder + filename + filesuffix;
	num = SAMPLES[type].number;

	fopen_s(&fp, file.c_str(), "w");
	
	for (i = 0; i < num; i++)
	{
		openfile = mainfolder;
		writefile = mainfolder;
		openfile += folder;
		openfile += sample_folder;
		writefile += sample_folder;
		openfile += "/";
		writefile += "/";
		sprintf_s(number, "%d", i);
		openfile += string(number);
		writefile += string(number);
		openfile += suffix;
		writefile += suffix;
		tmp = imread(openfile, IMREAD_GRAYSCALE);
		if (type == GOOD)
			fprintf(fp, "%s 1 0 0 %d %d\n", (sample_folder + string("/") + string(number) + suffix).c_str(), tmp.cols-1, tmp.rows-1);
		else
			fprintf(fp, "%s\n", (sample_folder + string("/") + string(number) + suffix).c_str());
		imwrite(writefile, tmp);
	}
	fclose(fp);
}

void HaarTrainingModule()
{
	CreateFileWithSamples(BAD);
	CreateFileWithSamples(GOOD);
}