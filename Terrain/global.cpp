#include "global.h"


// Window dimensions
GLFWwindow* window;
const GLuint WIDTH = 1650, HEIGHT = 900;

// Keyboard
bool keys[1024];
int state;

// Mouse
GLfloat lastX = 400, lastY = 300;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//Wireframe
bool wireframe = false;

// Camera
glm::mat4 projection;
glm::mat4 view;
bool firstMouse = true;
GLuint cameraMode = STATIC;
Camera camera(glm::vec3(0.0f, 2.0f, 2.0f));

// Light
float dayTime = 12.0f;

//Map
worldMap *globalMap;