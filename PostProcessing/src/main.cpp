// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Imgui
#include "Headers/imgui/imgui.h"
#include "Headers/imgui/imgui_impl_glfw.h"
#include "Headers/imgui/imgui_impl_opengl3.h"
#include "Headers/imgui/implot.h"
// Other
#include <array>
#include <thread>
#include <iostream>
#include <filesystem>
// My headers
#include "Headers/Textures/Textures.hpp"
#include "Headers/Shaders/Shader.hpp"
#include "Headers/IO/Input.hpp"

using namespace IO;

namespace fs = std::filesystem;

int main() {
#pragma region init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#pragma endregion

#pragma region Window and Context
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Engine", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create window" << std::endl;
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	stbi_set_flip_vertically_on_load(true);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	std::array<float, 100> frames;
	frames.fill(0.0f);
	int frameNum = 0;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
#pragma endregion

#pragma region Shader
	std::string currentPath = fs::current_path().string();

	std::cout << currentPath << std::endl;

	std::string VertexPath = currentPath + "\\src\\Shaders\\Shader.vert";
	std::string FragPath = currentPath + "\\src\\Shaders\\Shader.frag";

	Shader shader(VertexPath, FragPath);
#pragma endregion

#pragma region Texture
	std::string imagePath = currentPath + "\\src\\Textures\\Miaws1.jpg";

	int width, height;
	unsigned int image = loadTexture(imagePath, width, height);
#pragma endregion

#pragma region Quad
	float size = 1.0f;
	float vertices[] = {
		-size, -size, 0.0f, // Bottom right
		size, -size, 0.0f, // Bottom left
		size,  size, 0.0f, // Top left
		-size,  size, 0.0f // Top Right
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
#pragma endregion

#pragma region Time Variables
	float myTime = 0.0f;
	float lastTime = 0.0f;
	float dt = 0.0f;
	float tempTime;
#pragma endregion

#pragma region Main Loop
	while (!glfwWindowShouldClose(window)) {
#pragma region Time
		myTime = static_cast<float>(glfwGetTime());
		dt = myTime - lastTime;
		lastTime = myTime;
#pragma endregion

#pragma region Update
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		processInput(window);
#pragma endregion

#pragma region Render

#pragma region Clear Buffers
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
#pragma endregion

#pragma region Viewport size
		float scale = (float)SCR_HEIGHT / static_cast<float>(height);

		int viewportWidth = width * scale;
		int viewportHeight = SCR_HEIGHT;

		int Xoffset = (SCR_WIDTH - viewportWidth) / 2;

		glViewport(Xoffset, 0, viewportWidth, viewportHeight);
#pragma endregion

#pragma region Drawing image
		shader.use();
		shader.setVec2("uResolution", viewportWidth, viewportHeight);
		shader.setVec2("uOffset", Xoffset, 0.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#pragma endregion

#pragma region GUI
		{
			ImGui::Begin("Performance");

			ImGui::SeparatorText("Framerate");

			ImGui::Text("FPS: ");
			ImGui::SameLine();

			ImGui::Text(std::to_string(static_cast<int>(io.Framerate)).c_str());

			ImGui::SameLine();
			ImGui::Text(std::string(" (" + std::to_string(dt * 1000.0f) + "ms)").c_str());

			if (frameNum >= frames.size()) frameNum = 0;
			frames[frameNum] = std::round(io.Framerate);
			++frameNum;

			ImGui::PlotLines("Frames", frames.data(), frames.size(), 0, NULL, 0.0f);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion
		glfwSwapBuffers(window);
#pragma endregion
	}
#pragma endregion

#pragma region Terminate

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
#pragma endregion
}
