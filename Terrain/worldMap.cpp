#include "worldMap.h"
#include "chunk.h"
//imgui
#include "ImGui/imgui.h"
#include "ImGui/examples/opengl3_example/imgui_impl_glfw_gl3.h"

static terraGen terrainGenerator;

worldMap::worldMap(uint8_t _width = 1, uint8_t _height = 1) : width(_width), height(_height)
{
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			map[i][j] = chunk(i, j, this);
		}
	}

	generateHeightMapImage();
}

worldMap::~worldMap()
{
	// TODO //////////////////////
}

void worldMap::render()
{
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			//map[i][j].render_sprites();
			map[i][j].render();
		}
	}
}

void worldMap::generateHeightMapImage()
{
	long long int image_size = width * height * CHUNK_RESOLUTION * CHUNK_RESOLUTION * 3;
	GLubyte *data = new GLubyte[image_size];

	for (int ii = 0; ii < height; ii++)
	{
		for (int jj = 0; jj < width; jj++)
		{
			for (int i = 0; i < CHUNK_RESOLUTION; ++i)
			{
				for (int j = 0; j < CHUNK_RESOLUTION; j++)
				{
					GLfloat pixelHeight = map[jj][ii].getVertexHeight(j, i) + abs(terrainGenerator.loBound);
					int index = ii*(width*CHUNK_RESOLUTION*CHUNK_RESOLUTION*3) + jj*(CHUNK_RESOLUTION*3) + i*(width*CHUNK_RESOLUTION*3) + j*3;
					data[index]		= GLubyte(pixelHeight * 255);
					data[index + 1] = GLubyte(pixelHeight * 255);
					data[index + 2] = GLubyte(pixelHeight * 255);
					//camera.Position;
				}
			}
		}
	}

	ResourceManager::LoadTexture(data, CHUNK_RESOLUTION*width, CHUNK_RESOLUTION*height, false, "height");

	delete[] data;
}

void worldMap::renderInterface()
{
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Terrain info");
		
		ImGui::Text("Size: %d x %d", this->width, this->height);
		ImGui::Separator();

		ImGui::Text("Height Map");

		ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
		//int imageSize = (canvas_size.x<canvas_size.y) ? canvas_size.x : canvas_size.y;
		ImVec2 imageSize;
		imageSize = ImVec2(canvas_size.x, ResourceManager::GetTexture("height").Height / (ResourceManager::GetTexture("height").Width / canvas_size.x));

		ImGui::Image((void*)ResourceManager::GetTexture("height").ID,
			imageSize,
			ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 0));

		ImGui::Separator();

		ImGui::Text("Preview:");
		const int noisePlotSubd = 400;
		static float arr[noisePlotSubd];
		for (int i = 0; i < noisePlotSubd; ++i)
		{
			arr[i] = terrainGenerator.heightInPoint(50, float(i + 50));
		}
		canvas_size = ImGui::GetContentRegionAvail();
		ImGui::PlotLines("", arr, IM_ARRAYSIZE(arr), noisePlotSubd, "", terrainGenerator.loBound, terrainGenerator.hiBound, ImVec2(canvas_size.x, 50));

		ImGui::Text("Parameters:");
		ImGui::SliderInt("Octaves", (int*)&terrainGenerator.octaves, 1, 10);
		ImGui::SliderFloat("Persistence", (float*)&terrainGenerator.persistence, 0.0, 1.0);
		ImGui::SliderFloat("Scale", (float*)&terrainGenerator.scale, 0.0, 0.1);

		ImGui::Button("Generate", ImVec2(canvas_size.x, 25));

		ImGui::End();
}