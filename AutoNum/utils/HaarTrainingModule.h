#pragma once

const int GOOD_SAMPLES = 814;
const int BAD_SAMPLES = 1001;

enum sample_type {
	BAD = 0,
	GOOD
};

struct input_type {
	char* folder;
	int number;
};
static input_type SAMPLES[2] = {
	{ "Bad", BAD_SAMPLES },
	{"Good", GOOD_SAMPLES}
};

void HaarTrainingModule();