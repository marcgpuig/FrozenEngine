#include "lightManager.h"
#include "ImGui\imgui.h"
#include "ImGui\examples\opengl3_example\imgui_impl_glfw_gl3.h"

// Instantiate static variables
SunLight lightManager::sun;
pointLight lightManager::lights[MAX_N_LIGHTS];
GLint lightManager::pointLightNum = 0;
GLuint lightManager::VBO, lightManager::lightVAO;

lightManager::lightManager()
{
	sun.direction = glm::vec3(0.0);

	sun.ambient = glm::vec3(1.0);
	sun.diffuse = glm::vec3(1.0);
	sun.specular = glm::vec3(1.0);

	this->genVO();
}

void lightManager::genVO()
{
	
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
}

GLint lightManager::getPointLightNum()
{
	return pointLightNum;
}

void lightManager::updateSun(float dayTime)
{
	/*
	* dayTime - Interval [0,24]
	* cos((-12/12)*PI) = -1; cos((0/12)*PI) = 1; cos((12/12)*PI) = -1;
	*/
	glm::vec3 sunNewPos;
	sunNewPos.x = sin(((dayTime - 12.0) / 12.0) * glm::pi<float>());
	sunNewPos.y = cos(((dayTime - 12.0) / 12.0) * glm::pi<float>());
	sunNewPos.z = -sunNewPos.y*0.35;
	lightManager::sun.direction = sunNewPos;

	// color sun
	/*
	std::vector<glm::vec3> colorLights;
	colorLights.push_back(glm::vec3(0.25, 0.61, 1.0));
	colorLights.push_back(glm::vec3(0.79, 0.89, 1.0));
	colorLights.push_back(glm::vec3(1.0, 0.58, 0.16));
	colorLights.push_back(glm::vec3(1.0, 0.77, 0.56));
	colorLights.push_back(glm::vec3(1.0, 1.0, 1.0));
	colorLights.push_back(glm::vec3(1.0, 0.77, 0.56));
	colorLights.push_back(glm::vec3(1.0, 0.58, 0.16));
	colorLights.push_back(glm::vec3(0.79, 0.89, 1.0));
	colorLights.push_back(glm::vec3(0.25, 0.61, 1.0));

	float color = dayTime/float(24.0 / float(colorLights.size()));

	rgb actualLightRGB, nextLightRGB, resultRGB;
	hsv actualLightHSV, nextLightHSV, resultHSV;

	actualLightRGB.r = colorLights[color  ].r;
	actualLightRGB.g = colorLights[color  ].g;
	actualLightRGB.b = colorLights[color  ].b;
	nextLightRGB.r   = colorLights[color+1].r;
	nextLightRGB.g   = colorLights[color+1].g;
	nextLightRGB.b   = colorLights[color+1].b;

	actualLightHSV = rgb2hsv(actualLightRGB);
	nextLightHSV   = rgb2hsv(nextLightRGB);

	float min = color;
	float max = color + (24.0 / float(colorLights.size()));
	float mu = (dayTime - min) / (max - min);

	resultHSV.h = CosineInterpolate(actualLightHSV.h, nextLightHSV.h, mu);
	resultHSV.s = CosineInterpolate(actualLightHSV.s, nextLightHSV.s, mu);
	resultHSV.v = CosineInterpolate(actualLightHSV.v, nextLightHSV.v, mu);

	resultRGB = hsv2rgb(resultHSV);

	glm::vec3 sunNewLight;
	sunNewLight.r = resultRGB.r;
	sunNewLight.g = resultRGB.g;
	sunNewLight.b = resultRGB.b;

	sunColor = sunNewLight;*/
	// https://www.desmos.com/calculator/zs5wfg9uem

	glm::vec3 sunNewLight;

	sunNewLight.r = 0.5 + cos(((dayTime - 12.0) / 12.0) * glm::pi<float>())*0.5;
	sunNewLight.g = 0.5 + cos(((dayTime - 12.0) / 12.0) * glm::pi<float>())*0.5;
	sunNewLight.b = 1.0;

	lightManager::sun.ambient = sunNewLight;
	lightManager::sun.diffuse = sunNewLight;
	lightManager::sun.specular = sunNewLight;
}

void lightManager::update(Camera camera, float dayTime)
{
	for (int i = 0; i < pointLightNum; i++)
	{
		// sense moviment
		if (lights[i].state == 0)
		{

		}
		// moviment tipus 1 (equacio simple)
		else if (lights[i].state == 1)
		{
			lights[i].position.x += sin(glfwGetTime()) / 50.0f;
			lights[i].position.y += cos(glfwGetTime()) / 50.0f;
			lights[i].position.z += (0.5 + sin(glfwGetTime()))/600.0f;
		}
		// moviment tipus 2 (camera)
		else if (lights[i].state == 2)
		{
			lights[i].position = glm::vec3(camera.Position.x, camera.Position.y + 1.0, camera.Position.z);
		}
	}
	updateSun(dayTime);
}

void lightManager::renderInterface(float* dayTime)
{
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Lighting");
	if (ImGui::CollapsingHeader("Sun"))
	{
		ImGui::Spacing();
		static bool animateTime = false;
		ImGui::Checkbox("Animate", &animateTime);
		if (animateTime) *dayTime = (*dayTime >= 24.0) ? 0.0 : *dayTime + 0.01;
		ImGui::SliderFloat("Day time", dayTime, 0.0, 24.0, "%.3f h");
	}
	if (ImGui::CollapsingHeader("Point light"))
	{
		static GLint selectedLight = 0;
		std::string lights_names("");
		for (GLint i = 0; i < lightManager::getPointLightNum(); i++)
		{
			lights_names += lightManager::lights[i].name + '\0';
		}

		ImGui::Combo("Point Lights", &selectedLight, lights_names.c_str());

		ImGui::Separator();
		ImGui::Text("Position:");
		ImGui::Combo("Type", &lights[selectedLight].state, "Static\0Animated\0On camera\0");
		if (lights[selectedLight].state == 0)
		{
			ImGui::SliderFloat("X", (float*)&lightManager::lights[selectedLight].position.x, 0.0, 50.0);
			ImGui::SliderFloat("Y", (float*)&lightManager::lights[selectedLight].position.y, 0.0, 20.0);
			ImGui::SliderFloat("Z", (float*)&lightManager::lights[selectedLight].position.z, 0.0, 50.0);
		}
		ImGui::Separator();
		ImGui::ColorEdit3("Ambient", (float*)&lightManager::lights[selectedLight].ambient);
		ImGui::ColorEdit3("Diffuse", (float*)&lightManager::lights[selectedLight].diffuse);
		ImGui::ColorEdit3("Specular", (float*)&lightManager::lights[selectedLight].specular);

		ImGui::DragFloat("Constant", (float*)&lightManager::lights[selectedLight].constant, 0.001, 0.0);
		ImGui::DragFloat("Linear", (float*)&lightManager::lights[selectedLight].linear, 0.001, 0.0);
		ImGui::DragFloat("Quadratic", (float*)&lightManager::lights[selectedLight].quadratic, 0.001, 0.0);

		ImGui::Separator();
		ImGui::Text("New Light:");
		static char newLightName[128] = "";
		ImGui::InputText("Name", newLightName, IM_ARRAYSIZE(newLightName));

		ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(5 / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(5 / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("New Point Light", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			if (newLightName == "")
			{
				if (ImGui::BeginPopupModal("Error:"))
				{
					ImGui::Text(".\nYou must specify a name for the new light.\n\n");
					ImGui::Separator();

					if (ImGui::Button("Got it!", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
					ImGui::EndPopup();
				}
			}
			else
			{
				lightManager::addLight(std::string(newLightName), glm::vec3(2.0), glm::vec3(1.0));
				memset(newLightName, 0, sizeof(newLightName));
			}
		}
		ImGui::PopStyleColor(3);
		if (ImGui::Button("Delete Point Light", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			lightManager::clearLight(selectedLight);
		}
		
	}
	ImGui::End();
}

GLint lightManager::addLight(std::string name, glm::vec3 pos, glm::vec3 color)
{
	if (name == "\0")
	{
		return NULL;
	}
	if ((pointLightNum + 1) >= MAX_N_LIGHTS)
		return MAX_N_LIGHTS-1;

	pointLight newLight;

	newLight.name		= name;
	newLight.position	= pos;
	newLight.ambient	= color;
	newLight.diffuse	= color;
	newLight.specular	= color;
	// Choosing the right values: http://learnopengl.com/#!Lighting/Light-casters
	newLight.constant	= 1.0;
	newLight.linear		= 0.22;
	newLight.quadratic	= 0.20;
	newLight.state		= 0;

	lights[pointLightNum++] = newLight;
	return pointLightNum;
}

void lightManager::renderLightBoxes()
{

}

void lightManager::clearAll()
{	
	pointLightNum = 0;

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
}

void lightManager::clearLight(GLint pos)
{
	for (GLint i = pos+1; i < pointLightNum; i++)
	{
		lights[i - 1] = lights[i];
	}
	pointLightNum--;

}