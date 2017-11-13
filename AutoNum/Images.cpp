#include "stdafx.h"

static Mat IMAGES[IMG_TOTAL];

void SetImage(Mat& image, int number)
{
	IMAGES[number] = image;
}


Mat& GetImage(int number)
{
	return IMAGES[number];
}

