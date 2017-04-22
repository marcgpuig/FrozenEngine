#include "terraGen.h"


terraGen::terraGen() : octaves(8), persistence(0.5), scale(0.01), loBound(-0.5f), hiBound(0.5f)
{
}


terraGen::~terraGen()
{
}


GLfloat terraGen::heightInPoint(float x, float y)
{
	//return (GLfloat)scaled_raw_noise_2d(loBound, hiBound, x, y);
	return (GLfloat)scaled_octave_noise_2d(octaves, persistence, scale, loBound, hiBound, x, y);
}