#pragma once
#ifndef WORDMAP_H
#define WORDMAP_H

#include <iostream>
#include <map>

#include "terraGen.h"

#define CHUNK_RESOLUTION 64
#define TEXTURE_REPEATS 8

class chunk; //forward declaration
class worldMap
{
private:
	uint8_t id;
	uint8_t height, width;

	std::map<GLint, std::map<GLint, chunk>> map;

public:
	worldMap(uint8_t, uint8_t);
	~worldMap();

	terraGen terrainGenerator;

	uint8_t getId() { return id; }
	uint8_t getHeight() { return height; }
	uint8_t getWidth() { return width; }
	void generateHeightMapImage();
	void render();
	void renderInterface();
};

#endif