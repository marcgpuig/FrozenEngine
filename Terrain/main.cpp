// http://www.opengl-tutorial.org/
// http://learnopengl.com/
// http://qiita.com/shimacpyon/items/e5d2a4e2019273345c37
// http://gamedev.stackexchange.com/questions/4157/what-are-some-programming-design-patterns-that-are-useful-in-game-development
// http://patriciogonzalezvivo.com/2015/thebookofshaders/

#ifndef MAIN_C
#define MAIN_C
#include "global.h"
#include "ImGui\imgui.h"
#include "ImGui\examples\opengl3_example\imgui_impl_glfw_gl3.h"

#include "colorFunctions.h"
#include "Sprite.h"

GLfloat lastTime = glfwGetTime();
GLint nbFrames = 0;

// Sensitivity
GLfloat sensitivity = 0.05;

//Noise Parameters
const float octaves = 6;
const float persistence = 0.5;
const float scale = 0.01;
const float loBound = 0.5;
const float hiBound = -0.5;

//ImVec4 lightColor(1.0, 0.0, 0.0, 0.0);

//////////////////////////////////////////////////////////////////////////////////////
//								   GLFW CALLBACKS									//
//////////////////////////////////////////////////////////////////////////////////////

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	state = action;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;

		if (cameraMode == MOUSE_MOVEMENT) // mouse cliked
		{
			//camera.ProcessMouseMovement(xoffset, yoffset);
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		cameraMode = MOUSE_MOVEMENT;
	else
		cameraMode = STATIC;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset/10.0);
}
//////////////////////////////////////////////////////////////////////////////////////
//									 INTERFACE										//
//////////////////////////////////////////////////////////////////////////////////////
void main_interface() 
{
	ImGui_ImplGlfwGL3_NewFrame();
	{
		// Top menu:
		static bool showMainMenu = true;

		// possible opened interfaces:
		static bool showTerrainMenu = false;
		static bool showIlluminationMenu = false;
		static bool showCameraMenu = false;
		static bool showPlayerMenu = false;
		static bool showNPCMenu = false;

		if (showMainMenu)
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open Configuration", "TO DO")) {}
					ImGui::Separator();
					if (ImGui::MenuItem("Save Configuration", "TO DO")) {}
					ImGui::Separator();
					if (ImGui::MenuItem("Close Application")) { glfwSetWindowShouldClose(window, GL_TRUE); }
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("View"))
				{
					if (ImGui::BeginMenu("Render mode:"))
					{
						if (ImGui::MenuItem("Fill", "F1")) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
						if (ImGui::MenuItem("Wireframe", "F2")) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
						if (ImGui::MenuItem("Points", "F3")) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); }
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Window"))
				{
					ImGui::MenuItem("Lightning", "", &showIlluminationMenu);
					if (ImGui::MenuItem("Terrain", "", &showTerrainMenu));
					if (ImGui::MenuItem("Camera", "", &showCameraMenu));
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
		}
		if (showIlluminationMenu) { lightManager::renderInterface(&dayTime); }
		if (showTerrainMenu) { globalMap->renderInterface(); }
		if (showCameraMenu) { camera.renderInterface(); }
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//									 FUNCTIONS										//
//////////////////////////////////////////////////////////////////////////////////////
double CosineInterpolate(double y1, double y2, double mu)
{
	double mu2 = (1 - cos(mu*PI)) / 2;
	return(y1*(1 - mu2) + y2*mu2);
}

void showFPS(GLFWwindow* window, GLfloat refresh, bool type = true)
{
	// Measure speed
	GLfloat currentTime = glfwGetTime();
	nbFrames++;

	if (currentTime - lastTime >= refresh) // print every 1 second
	{
		if (type)
		{
			std::string title = "mapViewer - FPS: " + std::to_string(nbFrames);
			glfwSetWindowTitle(window, title.c_str());
		}
		else
		{
			std::string title = "mapViewer - msPF: " + std::to_string(1000.0 / GLfloat(nbFrames));
			glfwSetWindowTitle(window, title.c_str());
		}
		nbFrames = 0;
		lastTime += 1.0;
	}
}
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(UP, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_R])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_F])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
}
void checkCameraRotationMode()
{

}
void cameraRotationMode()
{
	Do_Movement();
	if (cameraMode == MOUSE_MOVEMENT)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if(cameraMode == STATIC)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void wireframeCheck()
{
	if (keys[GLFW_KEY_F1])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (keys[GLFW_KEY_F2])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (keys[GLFW_KEY_F3])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}

void processKeyboard()
{
	cameraRotationMode();
	wireframeCheck();
}

//////////////////////////////////////////////////////////////////////////////////////
//									   MAIN		     								//
//////////////////////////////////////////////////////////////////////////////////////
// The MAIN function, from here we start the application and run the game loop
int main(int argc, char** argv)
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);
	
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	
	// LOAD SHADERS
	ResourceManager::LoadShader("resources/shaders/AmbDifSpec.vert", "resources/shaders/AmbDifSpec.frag", nullptr, "AmbDifSpec");
	ResourceManager::LoadShader("resources/shaders/terrain.vert", "resources/shaders/terrain.frag", nullptr, "terrainShader");
	ResourceManager::LoadShader("resources/shaders/lamp.vert", "resources/shaders/lamp.frag", nullptr, "lampShader");
	ResourceManager::LoadShader("resources/shaders/default.vert", "resources/shaders/default.frag", nullptr, "defaultShader");
	ResourceManager::LoadShader("resources/shaders/sprite.vert", "resources/shaders/sprite.frag", nullptr, "sprite");
	
	// LOAD TEXTURES
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_dirt02_512.png", GL_FALSE, "dirt");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_dirt02_512_h.png", GL_TRUE, "dirt_h");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_dirt02_512_s.png", GL_TRUE, "dirt_s");
	
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_rock_03.png", GL_FALSE, "rock");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_rock_03_h.png", GL_TRUE, "rock_h");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_rock_03_s.png", GL_TRUE, "rock_s");

	ResourceManager::LoadTexture("resources/textures/WOW/6tj_felfuzzymoss01_512.png", GL_FALSE, "moss");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_felfuzzymoss01_512_h.png", GL_TRUE, "moss_h");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_felfuzzymoss01_512_s.png", GL_TRUE, "moss_s");

	ResourceManager::LoadTexture("resources/textures/WOW/6tj_fernygrass_01_512.png", GL_FALSE, "grass");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_fernygrass_01_512_h.png", GL_TRUE, "grass_h");
	ResourceManager::LoadTexture("resources/textures/WOW/6tj_fernygrass_01_512_s.png", GL_TRUE, "grass_s");

	ResourceManager::LoadTexture("resources/textures/alpha.jpg", GL_FALSE, "alpha");

	ResourceManager::LoadTexture("resources/textures/grass.png", GL_TRUE, "grass_sprite");

	// ADD LIGHTS

	lightManager::addLight("light_1", glm::vec3(0.0), glm::vec3(1.0, 0.5, 0.0));
	lightManager::addLight("light_2", glm::vec3(2.0), glm::vec3(1.0, 1.0, 1.0));

	GLfloat vertices[] = {
		// Vertex			  // Noramls	
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	
	// VBO -> vertex buffer
	// VAO -> vertex array
	GLuint VBO, lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(lightVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// position atribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// normal atribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	// Uncommenting this call will result in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	globalMap = new worldMap(2, 2);
	//glm::vec2 newposition, glm::vec2 newsize, GLfloat newrotate, glm::vec3 newcolor
	Sprite test(ResourceManager::GetTexture("grass_sprite"), ResourceManager::GetShader("sprite"), glm::vec3(0.0), glm::vec2(1.0), GLfloat(0.0), glm::vec3(1.0));

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame ////////////////////////////////////////////////////
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// INPUT ///////////////////////////////////////////////////////////////////////////////////
		glfwPollEvents();
		processKeyboard();

		// FPS /////////////////////////////////////////////////////////////////////////////////////
		showFPS(window, 1.0, 1);

		// ANIMATION ///////////////////////////////////////////////////////////////////////////////
		lightManager::update(camera, dayTime);

		// RENDER //////////////////////////////////////////////////////////////////////////////////
		// Clear the colorbuffer
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);
		view = camera.GetViewMatrix();

		// LIGHT_BOX ///////////////////////////////////////////////////////////////////////////////
		ResourceManager::GetShader("lampShader").Use();
		ResourceManager::GetShader("lampShader").SetMatrix4("view", view);
		ResourceManager::GetShader("lampShader").SetMatrix4("projection", projection);

		glBindVertexArray(lightVAO);
		for (GLint i = 0; i < lightManager::getPointLightNum(); i++)
		{
			glm::mat4 lightModel;
			lightModel = glm::translate(lightModel, lightManager::lights[i].position);
			lightModel = glm::scale(lightModel, glm::vec3(0.1f));

			ResourceManager::GetShader("lampShader").SetMatrix4("model", lightModel);
			ResourceManager::GetShader("lampShader").SetVector3f("lightColor", lightManager::lights[i].ambient);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
		// TERRAIN ////////////////////////////////////////////////////////////////////////////////
		globalMap->render();
		test.render();
		// INTERFACE ///////////////////////////////////////////////////////////////////////////////
		main_interface();
		ImGui::Render();
		

		// SWAP BUFFERS ///////////////////////////////////////////////////////////////////////////
		glfwSwapBuffers(window);
	}
	// de-allocate all resources
	ResourceManager::ClearAll();
	lightManager::clearAll();

	ImGui_ImplGlfwGL3_Shutdown();
	
	delete globalMap;
	
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

#endif