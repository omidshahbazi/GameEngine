
#include <gl\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Framework.h"

Framework *g_Framework = nullptr;

Framework::Framework(void) :
	m_Window(nullptr)
{
	g_Framework = this;
}

void Framework::Initialize(void)
{
	g_Framework = this;

	GLenum res1 = glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Framework::Deinitialize(void)
{
	if (m_DeinitializeCallback != nullptr)
		m_DeinitializeCallback();

	glfwTerminate();
}

bool Framework::CreateWindow(const char *Title, unsigned int Width, unsigned int Height)
{
	GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

	if (window == nullptr)
		return false;

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow *Window, int Width, int Height) -> void { g_Framework->HandleDeviceResize(Width, Height); });

	HandleDeviceResize(Width, Height);

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
	if (m_InitializeCallback != nullptr)
		m_InitializeCallback();

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

		ProcessInput();

		if (m_UpdateCallback != nullptr)
			m_UpdateCallback(frameTime);

		glClear(GL_COLOR_BUFFER_BIT);

		if (m_RenderCallback != nullptr)
			m_RenderCallback(frameTime);

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

void Framework::HandleDeviceResize(int Width, int Height)
{
	glViewport(0, 0, Width, Height);

	if (m_DeviceResizedCallback != nullptr)
		m_DeviceResizedCallback(Width, Height);
}

void  Framework::ProcessInput(void)
{
	GLFWwindow *window = reinterpret_cast<GLFWwindow*>(m_Window);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		static bool wireframe = false;
		wireframe = !wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, (wireframe ? GL_LINE : GL_FILL));
	}
}