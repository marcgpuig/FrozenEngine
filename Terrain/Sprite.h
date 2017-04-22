#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "global.h"
#include "texture.h"
#include "Shader.h"

class Sprite
{
public:
	Sprite(Texture2D &newTexture, Shader &newShader, glm::vec3 newposition, glm::vec2 newsize, GLfloat newrotate, glm::vec3 newcolor);
	~Sprite();
	void render();

private:
	Shader* shader;
	Texture2D* texture;
	GLuint quadVAO;
	glm::vec3 position;
	glm::vec2 size; 
	GLfloat rotate;
	glm::vec3 color;
};

#endif