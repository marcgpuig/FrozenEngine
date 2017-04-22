/**********************************************************************
** chunk.h

** Hi ha arrays amb +2 al tamany, aixo es perque es necesiten els
** vertex contigus dels chunks adjacents per calcular les normals
** dels vertex dels laterals
**********************************************************************/

#pragma once
#ifndef CHUNK_H
#define CHUNK_H
// GLM Mathematics
#include "global.h"
#include "worldMap.h"
#include "Sprite.h"
#include <string.h>
#include <list>

#define CHUNK_RESOLUTION	64		// faces in chunk
//#define CHUNK_SIDE_POINTS	65		// vertex per edge in chunk (CHUNK_RESOLUTION+1)
#define POINTS_IN_RECT		6		// one square has 6 points (in 2 triangles)

const GLint CHUNK_SIDE_POINTS = CHUNK_RESOLUTION + 1; // vertex per edge in chunk

class worldMap; //forward declaration
class chunk
{
private:
	//glm::mat4 modelMatrix;
	worldMap *parentMap;
	uint32_t id;
	uint8_t worldPosX, worldPosY;
	uint32_t worldStartPosX, worldStartPosY;

	GLfloat distanceBtwPoints;

	GLfloat *chunkVertexBuffer;
	GLuint VAO, VBO;

	GLfloat heightMap[CHUNK_SIDE_POINTS+2][CHUNK_SIDE_POINTS+2];

	std::list<Sprite> sprites;

public:
	chunk(int x, int y, worldMap *map);
	chunk();
	~chunk();

	void generate();
	void load(std::string);
	void genHeigts();
	void addSprites();
	void genVBO();
	void render();
	void render_sprites();

	GLfloat getVertexHeight(uint8_t x, uint8_t y);

	glm::vec3 getAdjvertex(int i, int j, GLfloat heightPlane[CHUNK_SIDE_POINTS + 2][CHUNK_SIDE_POINTS + 2][3]); // +2 to generate normals of vertex in adjacent chunks
	glm::vec3 gouraud(int i, int j, GLfloat heightPlane[CHUNK_SIDE_POINTS + 2][CHUNK_SIDE_POINTS + 2][3]);
};

#endif