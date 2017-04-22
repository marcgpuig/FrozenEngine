#include "Sprite.h"

Sprite::Sprite(Texture2D &newTexture, Shader &newShader, glm::vec3 newposition, glm::vec2 newsize, GLfloat newrotate, glm::vec3 newcolor)
{
	this->shader = &newShader;
	this->texture = &newTexture;
	this->position = newposition;
	this->size = newsize;
	this->rotate = newrotate;
	this->color = newcolor;

	// Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos			 // Tex
		0.0f, 1.0f, 0.0, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


Sprite::~Sprite()
{
}

void Sprite::render()
{
	// Prepare transformations
	this->shader->Use();
	glm::mat4 model;
	model = glm::translate(model, position);  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	this->shader->SetMatrix4("model", model);
	this->shader->SetMatrix4("view", view);
	this->shader->SetMatrix4("projection", projection);

	// Render textured quad
	this->shader->SetVector3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture->Bind();
	//this->shader->SetInteger("image", 0);

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
