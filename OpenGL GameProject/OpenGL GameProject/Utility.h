#pragma once
#include <random>

const unsigned int SCR_WIDTH = 1152;
const unsigned int SCR_HEIGHT = 648;
static int CENTRE_X = (int)(SCR_WIDTH * 0.5);
static int CENTRE_Y = (int)(SCR_HEIGHT * 0.5);

float Lerpf(float x, float y, float f);
float RandomFloat();
float RandomFloatInRange(float lower, float upper);

