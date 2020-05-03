#include "Utility.h"

float Lerpf(float x, float y, float f)
{
	return (x * (1.0f - f)) + (y * f);
}

float RandomFloat()
{
	float r = (float)rand() / (double)RAND_MAX;
	return r;
}

float RandomFloatInRange(float lower, float upper)
{
	return lower + static_cast<float>(rand() / (static_cast<float>(RAND_MAX / (upper - lower))));
}
