#include "../stdafx.h"
#include "NeuronTrainingModule.h"
#include "ctime"

const int IMAGES_CLASSES = 22;

const int ZERO_IMAGES_INPUT = 1301;
const int ONE_IMAGES_INPUT = 864;
const int TWO_IMAGES_INPUT = 657;
const int THREE_IMAGES_INPUT = 669;
const int FOUR_IMAGES_INPUT = 909;
const int FIVE_IMAGES_INPUT = 694;
const int SIX_IMAGES_INPUT = 713;
const int SEVEN_IMAGES_INPUT = 799;
const int EIGHT_IMAGES_INPUT = 621;
const int NINE_IMAGES_INPUT = 841;
const int A_IMAGES_INPUT = 869;
const int B_IMAGES_INPUT = 752;
const int C_IMAGES_INPUT = 633;
const int E_IMAGES_INPUT = 574;
const int H_IMAGES_INPUT = 560;
const int K_IMAGES_INPUT = 696;
const int M_IMAGES_INPUT = 520;
const int P_IMAGES_INPUT = 575;
const int T_IMAGES_INPUT = 542;
const int X_IMAGES_INPUT = 614;
const int Y_IMAGES_INPUT = 640;
const int GARBAGE_INPUT = 511;

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

static string CLASS_NAME[IMAGES_CLASSES] = {
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"E",
	"H",
	"K",
	"M",
	"P",
	"T",
	"X",
	"Y",
	"GARBAGE"
};

string GetClassName(int number)
{
	return CLASS_NAME[number];
}



static void NeuroReadInputImages(vector<Mat>& inputImages)
{
	int i, j;
	Mat tmp;
	string fullname;
	string folder = "utils/samples/Symbols/";
	char number[5] = "";
	char subfolder[5] = "";
	string suffix = ".bmp";

	for (i = 0; i < IMAGES_CLASSES; i++)
	{
		for (j = 0; j < IMAGES_NUMBER[i]; j++)
		{
			fullname = folder;
			sprintf_s(subfolder, "%d/", i);
			fullname += string(subfolder);
			sprintf_s(number, "%d", j);
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
	inputImages.clear();
	return input;
}

static void NeuroPrepareResponses(Mat_<float>& responses, const Mat& input)
{
	int i, classCount = 0, row, sum = IMAGES_NUMBER[0];
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
	//layerSizes(0, 1) = 2 * inputNum + 1;
	layerSizes(0, 1) = 100;
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

static void TimestampOut(time_t diff)
{
	bool bPrev = false;
	struct tm timeinfo;

	gmtime_s(&timeinfo, &diff);
	if (timeinfo.tm_yday > 0)
	{
		bPrev = true;
		cout << timeinfo.tm_yday << " days ";
	}
	if (bPrev || timeinfo.tm_hour > 0)
	{
		bPrev = true;
		cout << timeinfo.tm_hour << " hours ";
	}
	if (bPrev || timeinfo.tm_min > 0)
	{
		bPrev = true;
		cout << timeinfo.tm_min << " min ";
	}
	cout << timeinfo.tm_sec << " sec\n";
}

void NeuronTrainingModule()
{
	Ptr<ANN_MLP> network;
	vector<Mat> inputImages;
	Mat input, result;
	int i;
	time_t t_sum = 0;
	time_t start;
	time_t end;
	time_t diff;
	int classes_sum = 0;
	
	time(&start);
	cout << "1. Load images ... \n";
	NeuroReadInputImages(inputImages);
	cout << "Images ready: ";
	time(&end);
	diff = end - start;
	t_sum += diff;
	TimestampOut(diff);

	time(&start);
	cout << "2. Prepare images for network ... \n";
	input = NeuroPrepareInput(inputImages);
	cout << "Images resized and converted: ";
	time(&end);
	diff = end - start;
	t_sum += diff;
	TimestampOut(diff);

	time(&start);
	cout << "3. Prepare train responses ... \n";

	Mat_<float> responses(input.rows, IMAGES_CLASSES);
	NeuroPrepareResponses(responses, input);
	cout << "Responses prepared: ";
	time(&end);
	diff = end - start;
	t_sum += diff;
	TimestampOut(diff);

	time(&start);
	cout << "4. Configure network ... \n";
	NeuroPrepareNetwork(network, input.cols, responses.cols);
	cout << "Network prepared: ";
	time(&end);
	diff = end - start;
	t_sum += diff;
	TimestampOut(diff);

	time(&start);
	cout << "5. Prepare train data ... \n";
	Ptr<TrainData> trainData = TrainData::create(input, ROW_SAMPLE, responses);
	cout << "Train data created: ";
	time(&end);
	diff = end - start;
	t_sum += diff;
	TimestampOut(diff);
	
	time(&start);
	cout << "6. Training ... \n";
	network->train(trainData);//, ANN_MLP::TrainFlags::NO_INPUT_SCALE + ANN_MLP::TrainFlags::NO_OUTPUT_SCALE);
	time(&end);
	diff = end - start;
	t_sum += diff;
	TimestampOut(diff);
	cout << "Network trained: ";

	if (network->isTrained())
	{
		time(&start);
		printf("Predict training data:\n");
		for (i = 0; i < IMAGES_CLASSES; i++)
		{
			classes_sum += IMAGES_NUMBER[i];
			cout << CLASS_NAME[i] << " : \n";
			network->predict(input.row(classes_sum - 10), result);
			cout << result << "\n";
		}
		cout << "Checked: ";
		time(&end);
		diff = end - start;
		t_sum += diff;
		TimestampOut(diff);
	}
	network->save("config/network.dat");
	cout << "Network saved\n";
	cout << "Total time: ";
	TimestampOut(t_sum);
}