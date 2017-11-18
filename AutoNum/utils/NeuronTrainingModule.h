#pragma once

enum images_input
{
	A_IMAGE = 0,
	K_IMAGE = 1,
	GARBAGE = 2
};

const int NEURON_IMAGE_COLS = 40;
const int NEURON_IMAGE_ROWS = 52;

void NeuronTrainingModule();