#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Camera.h"
#include <GLFW\glfw3.h>
#include <map>

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))

struct SunLight {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct pointLight {
	std::string name;
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	GLint state;
};

#define MAX_N_LIGHTS 25

class lightManager
{
public:
	static SunLight sun;
	static pointLight lights[MAX_N_LIGHTS];

	static GLuint VBO, lightVAO;
	static void genVO();

	static GLint getPointLightNum();

	static GLint addLight(std::string name, glm::vec3 pos, glm::vec3 color);
	static void renderLightBoxes();
	static void clearAll();
	static void clearLight(GLint);
	
	static void update(Camera camera, float dayTime);
	static void updateSun(float dayTime);

	static void renderInterface(float* dayTime);
private:
	lightManager();
	static GLint pointLightNum;
};

