#ifndef TERRAGEN_H
#define TERRAGEN_H

#include <iostream>
#include "simplexNoise.h"
#include <GL/glew.h>; // Include glew to get all the required OpenGL headers

class terraGen
{
public:
	uint8_t octaves;
	GLfloat persistence, scale;
	GLfloat loBound, hiBound;

	terraGen();
	~terraGen();

	GLfloat heightInPoint(float x, float y);
};

#endif