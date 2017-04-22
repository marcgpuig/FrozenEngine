#include "chunk.h"
#include "ResourceManager.h"

// calcul de normal amb vertex que no estan en el mateix chunk
glm::vec3 chunk::getAdjvertex(int i, int j, GLfloat heightPlane[CHUNK_SIDE_POINTS + 2][CHUNK_SIDE_POINTS + 2][3])
{
	if (i < 0 && i >= CHUNK_SIDE_POINTS && j < 0 && j >= CHUNK_SIDE_POINTS)
	{
		GLfloat x, y, z;
		x = float(i - float(CHUNK_SIDE_POINTS / 2.0))*distanceBtwPoints;
		y = float(j - float(CHUNK_SIDE_POINTS / 2.0))*distanceBtwPoints;
		z = parentMap->terrainGenerator.heightInPoint(i, j);
		return glm::vec3(x, y, z);
	}
	else return glm::vec3(heightPlane[i][j][0], heightPlane[i][j][1], heightPlane[i][j][2]);
}

glm::vec3 chunk::gouraud(int i, int j, GLfloat heightPlane[CHUNK_SIDE_POINTS + 2][CHUNK_SIDE_POINTS + 2][3])
{
	glm::vec3 result, vertex1, vertex2, vertex3, vertex4, vertex5, vertex6, vertex7;

	
	vertex1 = glm::vec3(heightPlane[i][j][0], heightPlane[i][j][1], heightPlane[i][j][2]);
	vertex2 = glm::vec3(heightPlane[i+1][j][0], heightPlane[i + 1][j][1], heightPlane[i + 1][j][2]);
	vertex3 = glm::vec3(heightPlane[i][j+1][0], heightPlane[i][j+1][1], heightPlane[i][j+1][2]);
	vertex4 = glm::vec3(heightPlane[i-1][j+1][0], heightPlane[i - 1][j + 1][1], heightPlane[i - 1][j + 1][2]);
	vertex5 = glm::vec3(heightPlane[i-1][j][0], heightPlane[i - 1][j][1], heightPlane[i - 1][j][2]);
	vertex6 = glm::vec3(heightPlane[i][j-1][0], heightPlane[i][j - 1][1], heightPlane[i][j - 1][2]);
	vertex7 = glm::vec3(heightPlane[i+1][j-1][0], heightPlane[i + 1][j - 1][1], heightPlane[i + 1][j - 1][2]);

	/*vertex1 = glm::vec3(getAdjvertex(i,	  j,   heightPlane));
	vertex2 = glm::vec3(getAdjvertex(i+1, j,   heightPlane));
	vertex3 = glm::vec3(getAdjvertex(i,   j+1, heightPlane));
	vertex4 = glm::vec3(getAdjvertex(i-1, j+1, heightPlane));
	vertex5 = glm::vec3(getAdjvertex(i-1, j,   heightPlane));
	vertex6 = glm::vec3(getAdjvertex(i,   j-1, heightPlane));
	vertex7 = glm::vec3(getAdjvertex(i+1, j-1, heightPlane));*/

	result  = glm::normalize(glm::cross(vertex3 - vertex1, vertex2 - vertex1));
	result += glm::normalize(glm::cross(vertex4 - vertex1, vertex3 - vertex1));
	result += glm::normalize(glm::cross(vertex5 - vertex1, vertex4 - vertex1));
	result += glm::normalize(glm::cross(vertex6 - vertex1, vertex5 - vertex1));
	result += glm::normalize(glm::cross(vertex7 - vertex1, vertex6 - vertex1));
	result += glm::normalize(glm::cross(vertex2 - vertex1, vertex7 - vertex1));

	result /= 6.0;

	return result;
}

chunk::chunk(int x, int y, worldMap *map)
{
	distanceBtwPoints = 0.1;
	parentMap = map;

	worldPosX = x, worldPosY = y;
	worldStartPosX = worldPosX * CHUNK_RESOLUTION;
	worldStartPosY = worldPosY * CHUNK_RESOLUTION;

	generate();
	genVBO();
	addSprites();
}

chunk::chunk()
{
}

chunk::~chunk()
{
	// TODO //////////////////////

	/*glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);*/
}

GLfloat chunk::getVertexHeight(uint8_t x, uint8_t y)
{
	return heightMap[x+1][y+1];
}

void chunk::addSprites()
{
	//std::rand() % int(worldStartPosX + (distanceBtwPoints * CHUNK_RESOLUTION)) + worldStartPosX;
	Sprite grass(ResourceManager::GetTexture("grass_sprite"), ResourceManager::GetShader("sprite"), glm::vec3(00.0), glm::vec2(1.0), GLfloat(0.0), glm::vec3(1.0));
	sprites.push_front(grass);
}

void chunk::genHeigts()
{
	for (int i = 0; i < CHUNK_SIDE_POINTS+2; ++i)
	{
		for (int j = 0; j < CHUNK_SIDE_POINTS+2; ++j)
		{
			heightMap[i][j] = parentMap->terrainGenerator.heightInPoint(worldStartPosX + i, 
																		worldStartPosY + j);
		}
	}
}

void chunk::generate()
{
	/*
	Pos				= 3(x,y,z)
	Normal			= 3(x,y,z)
	TextureCoord	= 2(x,y)
	*/

	GLint chunkPointSize = (3 + 3 + 2);
										// Chunk Resolution					| points in rect  | atributes of 
										// example (64*64 quads)			| 6 (2 triangles) | each point
	const GLint chunkVertexBufferSize = CHUNK_RESOLUTION * CHUNK_RESOLUTION * POINTS_IN_RECT  * chunkPointSize;
	chunkVertexBuffer = new GLfloat[chunkVertexBufferSize];
	
	GLfloat textureFactor = 1.0 / GLfloat(CHUNK_SIDE_POINTS / GLfloat(TEXTURE_REPEATS));
	GLfloat heightPlane[CHUNK_SIDE_POINTS+2][CHUNK_SIDE_POINTS+2][3];

	genHeigts();

	for (int i = 0; i < CHUNK_SIDE_POINTS+2; ++i)
	{
		for (int j = 0; j < CHUNK_SIDE_POINTS+2; ++j)
		{
			heightPlane[i][j][0] = float(i - float(CHUNK_SIDE_POINTS / 2.0))*distanceBtwPoints;
			heightPlane[i][j][1] = float(j - float(CHUNK_SIDE_POINTS / 2.0))*distanceBtwPoints;
			heightPlane[i][j][2] = heightMap[i][j];
		}
	}

	for (int i = 0; i < CHUNK_RESOLUTION; ++i)
	{
		for (int j = 0; j < CHUNK_RESOLUTION; ++j)
		{
			GLint arrayPos = i * (CHUNK_RESOLUTION * POINTS_IN_RECT * chunkPointSize) + j * POINTS_IN_RECT * chunkPointSize;
			GLint ii = i + 1, jj = j + 1; // +1 to get normals of vertex in adjacent chunks, because heightPlane is size-augmented +2

			glm::vec3 vertex1 = glm::vec3(heightPlane[ii][jj][0], heightPlane[ii][jj][1], heightPlane[ii][jj][2]);
			glm::vec3 normalVertex1 = gouraud(ii, jj, heightPlane);
			glm::vec3 vertex2 = glm::vec3(heightPlane[ii + 1][jj][0], heightPlane[ii + 1][jj][1], heightPlane[ii + 1][jj][2]);
			glm::vec3 normalVertex2 = gouraud(ii + 1, jj, heightPlane);
			glm::vec3 vertex3 = glm::vec3(heightPlane[ii][jj + 1][0], heightPlane[ii][jj + 1][1], heightPlane[ii][jj + 1][2]);
			glm::vec3 normalVertex3 = gouraud(ii, jj + 1, heightPlane);
			glm::vec3 vertex4 = glm::vec3(heightPlane[ii + 1][jj + 1][0], heightPlane[ii + 1][jj + 1][1], heightPlane[ii + 1][jj + 1][2]);
			glm::vec3 normalVertex4 = gouraud(ii + 1, jj + 1, heightPlane);

			//face 1
			//vertex 1
			chunkVertexBuffer[arrayPos + 0] = vertex1.x;
			chunkVertexBuffer[arrayPos + 1] = vertex1.y;
			chunkVertexBuffer[arrayPos + 2] = vertex1.z;
			//normals 1
			chunkVertexBuffer[arrayPos + 3] = normalVertex1.x;
			chunkVertexBuffer[arrayPos + 4] = normalVertex1.y;
			chunkVertexBuffer[arrayPos + 5] = normalVertex1.z;
			//textureCoord 1
			chunkVertexBuffer[arrayPos + 6] = (i)      * textureFactor;
			chunkVertexBuffer[arrayPos + 7] = (j)      * textureFactor;

			//vertex 2
			chunkVertexBuffer[arrayPos + 8] = vertex2.x;
			chunkVertexBuffer[arrayPos + 9] = vertex2.y;
			chunkVertexBuffer[arrayPos + 10] = vertex2.z;
			//normals 2
			chunkVertexBuffer[arrayPos + 11] = normalVertex2.x;
			chunkVertexBuffer[arrayPos + 12] = normalVertex2.y;
			chunkVertexBuffer[arrayPos + 13] = normalVertex2.z;
			//textureCoord 2
			chunkVertexBuffer[arrayPos + 14] = (i + 1) * textureFactor;
			chunkVertexBuffer[arrayPos + 15] = (j)     * textureFactor;

			//vertex 3
			chunkVertexBuffer[arrayPos + 16] = vertex3.x;
			chunkVertexBuffer[arrayPos + 17] = vertex3.y;
			chunkVertexBuffer[arrayPos + 18] = vertex3.z;
			//normals 3
			chunkVertexBuffer[arrayPos + 19] = normalVertex3.x;
			chunkVertexBuffer[arrayPos + 20] = normalVertex3.y;
			chunkVertexBuffer[arrayPos + 21] = normalVertex3.z;
			//textureCoord 3
			chunkVertexBuffer[arrayPos + 22] = (i)     * textureFactor;
			chunkVertexBuffer[arrayPos + 23] = (j + 1) * textureFactor;

			//face 2
			//vertex 2
			chunkVertexBuffer[arrayPos + 24] = vertex2.x;
			chunkVertexBuffer[arrayPos + 25] = vertex2.y;
			chunkVertexBuffer[arrayPos + 26] = vertex2.z;
			//normals 2
			chunkVertexBuffer[arrayPos + 27] = normalVertex2.x;
			chunkVertexBuffer[arrayPos + 28] = normalVertex2.y;
			chunkVertexBuffer[arrayPos + 29] = normalVertex2.z;
			//textureCoord 2
			chunkVertexBuffer[arrayPos + 30] = (i + 1) * textureFactor;
			chunkVertexBuffer[arrayPos + 31] = (j)	   * textureFactor;

			//vertex 4
			chunkVertexBuffer[arrayPos + 32] = vertex4.x;
			chunkVertexBuffer[arrayPos + 33] = vertex4.y;
			chunkVertexBuffer[arrayPos + 34] = vertex4.z;
			//normals 4
			chunkVertexBuffer[arrayPos + 35] = normalVertex4.x;
			chunkVertexBuffer[arrayPos + 36] = normalVertex4.y;
			chunkVertexBuffer[arrayPos + 37] = normalVertex4.z;
			//textureCoord 4
			chunkVertexBuffer[arrayPos + 38] = (i + 1) * textureFactor;
			chunkVertexBuffer[arrayPos + 39] = (j + 1) * textureFactor;

			//vertex 3
			chunkVertexBuffer[arrayPos + 40] = vertex3.x;
			chunkVertexBuffer[arrayPos + 41] = vertex3.y;
			chunkVertexBuffer[arrayPos + 42] = vertex3.z;
			//normals 3
			chunkVertexBuffer[arrayPos + 43] = normalVertex3.x;
			chunkVertexBuffer[arrayPos + 44] = normalVertex3.y;
			chunkVertexBuffer[arrayPos + 45] = normalVertex3.z;
			//textureCoord 3
			chunkVertexBuffer[arrayPos + 46] = (i)     * textureFactor;
			chunkVertexBuffer[arrayPos + 47] = (j + 1) * textureFactor;
		}
	}
}

void chunk::load(std::string file)
{
}

void chunk::genVBO()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	// 1. Bind Vertex Array Object
	glBindVertexArray(this->VAO);
	// 2. Copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, CHUNK_RESOLUTION * CHUNK_RESOLUTION * POINTS_IN_RECT * (3+3+2) * sizeof(GLfloat), chunkVertexBuffer, GL_STATIC_DRAW);
	// 3. Then set our vertex attributes pointers
	// vertex (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normals (x, y, z)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// textureCoords (x, y)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void chunk::render_sprites()
{
	std::list<Sprite>::iterator it;
	for (it = sprites.begin(); it != sprites.end(); it++)
	{
		it->render();
	}
}

void chunk::render()
{
	GLfloat distanceBtwChunks = 6.4;
	glm::mat4 modelMatrix;
	//modelMatrix = glm::rotate(modelMatrix, float(glm::pi<float>() / 2.0), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(worldPosX*distanceBtwChunks, worldPosY*distanceBtwChunks, 0.0));

	ResourceManager::GetShader("terrainShader").Use();
	// matrix
	ResourceManager::GetShader("terrainShader").SetMatrix4("model", modelMatrix);
	ResourceManager::GetShader("terrainShader").SetMatrix4("view", view);
	ResourceManager::GetShader("terrainShader").SetMatrix4("projection", projection);
	// atributes
	ResourceManager::GetShader("terrainShader").SetVector3f("viewPos", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
	
	// sun atributes
	ResourceManager::GetShader("terrainShader").SetVector3f("sun.direction", lightManager::sun.direction);

	ResourceManager::GetShader("terrainShader").SetVector3f("sun.ambient",	 lightManager::sun.ambient*glm::vec3(0.2));
	ResourceManager::GetShader("terrainShader").SetVector3f("sun.diffuse",	 lightManager::sun.diffuse);
	ResourceManager::GetShader("terrainShader").SetVector3f("sun.specular",  lightManager::sun.specular);

	ResourceManager::GetShader("terrainShader").SetInteger("N_of_lights",    lightManager::getPointLightNum());

	// pointLight atributes
	for (GLint i = 0; i < lightManager::getPointLightNum(); i++)
	{
		std::string li("light[" + std::to_string(i) + std::string("]."));
		ResourceManager::GetShader("terrainShader").SetVector3f((li + "position").c_str(), lightManager::lights[i].position);

		ResourceManager::GetShader("terrainShader").SetVector3f((li + "ambient").c_str(), lightManager::lights[i].ambient);
		ResourceManager::GetShader("terrainShader").SetVector3f((li + "diffuse").c_str(), lightManager::lights[i].diffuse);
		ResourceManager::GetShader("terrainShader").SetVector3f((li + "specular").c_str(), lightManager::lights[i].specular);

		ResourceManager::GetShader("terrainShader").SetFloat((li + "constant").c_str(), lightManager::lights[i].constant);
		ResourceManager::GetShader("terrainShader").SetFloat((li + "linear").c_str(), lightManager::lights[i].linear);
		ResourceManager::GetShader("terrainShader").SetFloat((li + "quadratic").c_str(), lightManager::lights[i].quadratic);
	}

	//texture0
	glActiveTexture(GL_TEXTURE0);
	ResourceManager::GetTexture("dirt").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_0", 0);
	glActiveTexture(GL_TEXTURE1);
	ResourceManager::GetTexture("dirt_h").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_0_h", 1);
	glActiveTexture(GL_TEXTURE2);
	ResourceManager::GetTexture("dirt_s").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_0_s", 2);

	//texture1
	glActiveTexture(GL_TEXTURE3);
	ResourceManager::GetTexture("grass").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_1", 3);
	glActiveTexture(GL_TEXTURE4);
	ResourceManager::GetTexture("grass_h").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_1_h", 4);
	glActiveTexture(GL_TEXTURE5);
	ResourceManager::GetTexture("grass_s").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_1_s", 5);

	//texture2
	glActiveTexture(GL_TEXTURE6);
	ResourceManager::GetTexture("moss").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_2", 6);
	glActiveTexture(GL_TEXTURE7);
	ResourceManager::GetTexture("moss_h").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_2_h", 7);
	glActiveTexture(GL_TEXTURE8);
	ResourceManager::GetTexture("moss_s").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_2_s", 8);

	//texture3
	glActiveTexture(GL_TEXTURE9);
	ResourceManager::GetTexture("rock").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_3", 9);
	glActiveTexture(GL_TEXTURE10);
	ResourceManager::GetTexture("rock_h").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_3_h", 10);
	glActiveTexture(GL_TEXTURE11);
	ResourceManager::GetTexture("rock_s").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("texture_3_s", 11);

	glActiveTexture(GL_TEXTURE12);
	ResourceManager::GetTexture("alpha").Bind();
	ResourceManager::GetShader("terrainShader").SetInteger("alpha", 12);
	
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, CHUNK_RESOLUTION * CHUNK_RESOLUTION * POINTS_IN_RECT);
	glBindVertexArray(0);
}
