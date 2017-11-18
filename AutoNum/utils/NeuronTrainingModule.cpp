#include "../stdafx.h"
#include "NeuronTrainingModule.h"

const int IMAGES_CLASSES = 22;

const int ZERO_IMAGES_INPUT = 200;
const int ONE_IMAGES_INPUT = 200;
const int TWO_IMAGES_INPUT = 200;
const int THREE_IMAGES_INPUT = 200;
const int FOUR_IMAGES_INPUT = 200;
const int FIVE_IMAGES_INPUT = 200;
const int SIX_IMAGES_INPUT = 200;
const int SEVEN_IMAGES_INPUT = 200;
const int EIGHT_IMAGES_INPUT = 200;
const int NINE_IMAGES_INPUT = 200;
const int A_IMAGES_INPUT = 200;// 867;
const int B_IMAGES_INPUT = 200;
const int C_IMAGES_INPUT = 200;
const int E_IMAGES_INPUT = 200;
const int H_IMAGES_INPUT = 200;
const int K_IMAGES_INPUT = 200;// 696;
const int M_IMAGES_INPUT = 200;
const int P_IMAGES_INPUT = 200;
const int T_IMAGES_INPUT = 200;
const int X_IMAGES_INPUT = 200;
const int Y_IMAGES_INPUT = 200;
const int GARBAGE_INPUT = 200;

static int IMAGES_NUMBER[IMAGES_CLASSES] = {
	ZERO_IMAGES_INPUT,
	ONE_IMAGES_INPUT,
	TWO_IMAGES_INPUT,
	THREE_IMAGES_INPUT,
	FOUR_IMAGES_INPUT,
	FIVE_IMAGES_INPUT,
	SIX_IMAGES_INPUT,
	SEVEN_IMAGES_INPUT,
	EIGHT_IMAGES_INPUT,
	NINE_IMAGES_INPUT,
	A_IMAGES_INPUT,
	B_IMAGES_INPUT,
	C_IMAGES_INPUT,
	E_IMAGES_INPUT,
	H_IMAGES_INPUT,
	K_IMAGES_INPUT,
	M_IMAGES_INPUT,
	P_IMAGES_INPUT,
	T_IMAGES_INPUT,
	X_IMAGES_INPUT,
	Y_IMAGES_INPUT,
	GARBAGE_INPUT
};

static void NeuroReadInputImages(vector<Mat>& inputImages)
{
	int i, j;
	Mat tmp;
	string fullname;
	string folder = "samples/Symbols/";
	char number[5] = "";
	char subfolder[5] = "";
	string suffix = ".bmp";

	for (i = 0; i < IMAGES_CLASSES; i++)
	{
		for (j = 0; j < IMAGES_NUMBER[i]; j++)
		{
			fullname = folder;
			sprintf(subfolder, "%d/", i);
			fullname += string(subfolder);
			sprintf(number, "%d", j);
			fullname += string(number);
			fullname += suffix;
			tmp = imread(fullname, IMREAD_GRAYSCALE);
			inputImages.push_back(tmp);
		}
	}
//	folder = "samples/Symbols/15/";
//	tmp = imread("samples/Symbols/15/00.bmp", IMREAD_GRAYSCALE);
//	inputImages.push_back(tmp);
}

static Mat NeuroPrepareInput(vector<Mat>& inputImages)
{
	int i;
	Mat input, tmp, resized;

	for (i = 0; i < inputImages.size(); i++)
	{
		resize(inputImages[i], resized, Size(NEURON_IMAGE_COLS, NEURON_IMAGE_ROWS), 0, 0, CV_INTER_AREA);
		resized.convertTo(resized, CV_32FC1, 1.0 / 255.0);
		tmp = resized.reshape(0, 1);
		input.push_back(tmp);
	}

	return input;
}

static void NeuroPrepareResponses(Mat_<float>& responses, const Mat& input)
{
	int i, classCount = 0, row, respNumber, sum = IMAGES_NUMBER[0];
	for (row = 0; row < input.rows; row++)
	{
		if (row >= sum)
		{
			classCount++;
			sum += IMAGES_NUMBER[classCount];
		}
		for (i = 0; i < IMAGES_CLASSES; i++)
		{
			if ( i == classCount)
				responses(row, i) = 1.0;
			else
				responses(row, i) = 0.0;
		}
	}
}

static void NeuroPrepareNetwork(Ptr<ANN_MLP>& network, int inputNum, int outputNum)
{
	Mat_<int> layerSizes(1, 3);

	layerSizes(0, 0) = inputNum;
	layerSizes(0, 1) = 100;
	//layerSizes(0, 2) = 100;
	layerSizes(0, 2) = outputNum;

	network = ANN_MLP::create();
	network->setLayerSizes(layerSizes);
	network->setActivationFunction(ANN_MLP::SIGMOID_SYM, 0, 0);
	network->setTrainMethod(ANN_MLP::BACKPROP, 0.0001);
	TermCriteria termCrit = TermCriteria(
		TermCriteria::Type::MAX_ITER //| TermCriteria::Type::EPS,
		, 300 //(int) INT_MAX
		, 0.000001
	);
	network->setTermCriteria(termCrit);
}

void NeuronTrainingModule()
{
	Ptr<ANN_MLP> network;
	vector<Mat> inputImages;
	Mat input, result;
	int i;

	NeuroReadInputImages(inputImages);
	input = NeuroPrepareInput(inputImages);

	Mat_<float> responses(input.rows, IMAGES_CLASSES);
	NeuroPrepareResponses(responses, input);
	
	NeuroPrepareNetwork(network, input.cols, responses.cols);

	Ptr<TrainData> trainData = TrainData::create(input, ROW_SAMPLE, responses);

	network->train(trainData);//, ANN_MLP::TrainFlags::NO_INPUT_SCALE + ANN_MLP::TrainFlags::NO_OUTPUT_SCALE);

	if (network->isTrained())
	{
		printf("Predict training data:\n");
		for (i = 0; i < IMAGES_CLASSES; i++)
		{
			network->predict(input.row(200*i), result);
			cout << result << endl;
		}
	}
	network->save("config/network.dat");
}