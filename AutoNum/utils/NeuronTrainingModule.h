#pragma once

enum images_input
{
	ZERO_IMAGE,
	ONE_IMAGE,
	TWO_IMAGE,
	THREE_IMAGE,
	FOUR_IMAGE,
	FIVE_IMAGE,
	SIX_IMAGE,
	SEVEN_IMAGE,
	EIGHT_IMAGE,
	NINE_IMAGE,
	A_IMAGE,
	B_IMAGE,
	C_IMAGE,
	E_IMAGE,
	H_IMAGE,
	K_IMAGE,
	M_IMAGE,
	P_IMAGE,
	T_IMAGE,
	X_IMAGE,
	Y_IMAGE,
	GARBAGE
};

const int NEURON_IMAGE_COLS = 40;
const int NEURON_IMAGE_ROWS = 52;

void NeuronTrainingModule();
string GetClassName(int);