
#include <gl\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Framework.h"

void OnWindowResized(GLFWwindow *Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);

	//projectionMat = glm::ortho(0.0F, (float)Width, (float)Height, (float)0, -1.0F, 10.0F);
}

void ProcessInput(GLFWwindow *Window)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);
	else if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		static bool wireframe = false;
		wireframe = !wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, (wireframe ? GL_LINE : GL_FILL));
	}
}

void Framework::Initialize(void)
{
	GLenum res1 = glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Framework::Deinitialize(void)
{
	glfwTerminate();
}

bool Framework::CreateWindow(const char *Title, unsigned int Width, unsigned int Height)
{
	GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

	if (window == nullptr)
		return false;

	glfwSetFramebufferSizeCallback(window, OnWindowResized);

	OnWindowResized(window, Width, Height);

	glfwMakeContextCurrent(window);

	m_Window = window;

	glewInit();

	return (window != nullptr);
}

void Framework::SetClearColor(Color Color)
{
	glClearColor(Color.GetNormalizeR(), Color.GetNormalizeG(), Color.GetNormalizeB(), Color.GetNormalizeA());
}

void Framework::Run(void)
{
	//glm::mat4 projectionMat;

	//glm::mat4 modelMat(1.0F);
	//modelMat = glm::translate(modelMat, glm::vec3(100.0F, 100.0F, 0.0F));
	//modelMat = glm::scale(modelMat, glm::vec3(100.0F, 100.0F, 1.0F));
	//modelMat = glm::rotate(modelMat, glm::radians(45.0F), glm::vec3(0, 0, 1));;

	//glm::mat4 viewMat(1.0F);
	//viewMat = glm::translate(viewMat, glm::vec3(100.0F, 0.0F, 0.0F));

	//glm::mat4 mvpMat = projectionMat * viewMat * modelMat;

	float lastFrameTime = glfwGetTime();;
	float deltaTime = 0.0F;
	float lastReportedFPSFrameTime = lastFrameTime;
	unsigned int frameCount = 0;
	float totalDeltaTime = 0;
	float fps = 0.0F;

	GLFWwindow *window = reinterpret_cast<GLFWwindow*>(m_Window);

	while (!glfwWindowShouldClose(window))
	{
		float frameTime = glfwGetTime();

		ProcessInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		//rendering

		glfwSwapBuffers(window);
		glfwPollEvents();

		deltaTime = frameTime - lastFrameTime;
		lastFrameTime = frameTime;
		totalDeltaTime += deltaTime;

		++frameCount;

		if (frameTime - lastReportedFPSFrameTime >= 1.0F)
		{
			float averageFrameTime = totalDeltaTime / frameCount;
			fps = 1 / averageFrameTime;

			std::cout << "FPS : " << fps << " Frame Time : " << averageFrameTime << std::endl;

			lastReportedFPSFrameTime = frameTime;
			frameCount = 0;
			totalDeltaTime = 0.0F;
		}
	}
}