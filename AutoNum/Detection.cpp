#include "stdafx.h"

Mat Detection(Mat& input, std::vector<cv::Rect>& areas)
{
	CascadeClassifier autonumClassifier;
	Mat inputCopy = input.clone();
	Mat inputGray;
	
	string haarXML = "./config/cascade.xml";


	cvtColor(inputCopy, inputGray, COLOR_RGB2GRAY);
	equalizeHist(inputGray, inputGray);
	
	if (autonumClassifier.load(haarXML))
	{
		autonumClassifier.detectMultiScale(inputGray, areas, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60, 20));

		for (size_t i = 0; i < areas.size(); i++)
		{
			rectangle(inputCopy, areas[i], Scalar(0, 0, 255), 4, 8, 0);
		}
	}

	return inputCopy;

}