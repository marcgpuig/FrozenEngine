#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <map>

// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL
#include <SOIL2\SOIL2.h>

// Other includes
#include "simplexNoise.h"
#include "Shader.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "worldMap.h"
#include "lightManager.h"

#define PI 3.14159265359
#define STATIC			0
#define MOUSE_MOVEMENT	1

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))

// Window dimensions
extern GLFWwindow* window;
extern const GLuint WIDTH, HEIGHT;

// Keyboard
extern bool keys[];
extern int state;

// Mouse
extern GLfloat lastX, lastY;

// Deltatime
extern GLfloat deltaTime;		// Time between current frame and last frame
extern GLfloat lastFrame;		// Time of last frame

//Wireframe
extern bool wireframe;

// Camera
extern glm::mat4 projection;
extern glm::mat4 view;
extern bool firstMouse;
extern GLuint cameraMode;
extern Camera camera;

// Light
extern int lightMovmentType;
extern float dayTime;

//Map
extern worldMap *globalMap;
#endif