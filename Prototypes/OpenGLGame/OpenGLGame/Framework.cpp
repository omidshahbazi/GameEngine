
#include <gl\glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include "Framework.h"
#include "GameObject.h"
#include "Component.h"
#include "Camera.h"

Framework *g_Framework = nullptr;

Framework::Framework(void) :
	m_Window(nullptr),
	m_Time(0.0F),
	m_DeltaTime(0.0F),
	m_FPS(0.0F)
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

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow *Window, int Width, int Height) { g_Framework->HandleDeviceResize(Width, Height); });

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

	float lastFrameTime = glfwGetTime();
	float lastReportedFPSFrameTime = lastFrameTime;
	unsigned int frameCount = 0;
	float totalDeltaTime = 0;

	GLFWwindow *window = reinterpret_cast<GLFWwindow*>(m_Window);

	while (!glfwWindowShouldClose(window))
	{
		m_Time = glfwGetTime();

		ProcessInput();

		glm::mat4 vpMat = m_ProjectionMatrix * m_CameraGameObject->GetLocalTransformMatrix();

		for (auto gameObject : m_GameObjects)
			gameObject->UpdateWorldMatrix(vpMat);

		for (auto component : m_Components)
			component->Update();

		if (m_UpdateCallback != nullptr)
			m_UpdateCallback();

		glClear(GL_COLOR_BUFFER_BIT);

		for (auto component : m_Components)
			component->Render();

		if (m_RenderCallback != nullptr)
			m_RenderCallback();

		glfwSwapBuffers(window);
		glfwPollEvents();

		m_DeltaTime = m_Time - lastFrameTime;
		lastFrameTime = m_Time;
		totalDeltaTime += m_DeltaTime;

		++frameCount;

		if (m_Time - lastReportedFPSFrameTime >= 1.0F)
		{
			float averageFrameTime = totalDeltaTime / frameCount;
			m_FPS = 1 / averageFrameTime;

			std::cout << "FPS : " << m_FPS << " Frame Time : " << averageFrameTime << std::endl;

			lastReportedFPSFrameTime = m_Time;
			frameCount = 0;
			totalDeltaTime = 0.0F;
		}
	}
}

GameObject *Framework::CreateGameObject(void)
{
	GameObject *obj = new GameObject(this);

	m_GameObjects.push_back(obj);

	return obj;
}

void Framework::OnComponentAdded(Component *Component)
{
	m_Components.push_back(Component);

	if (dynamic_cast<Camera*>(Component) != nullptr)
		m_CameraGameObject = Component->GetGameObject();
}

void Framework::HandleDeviceResize(int Width, int Height)
{
	glViewport(0, 0, Width, Height);
	m_ProjectionMatrix = glm::ortho(0.0F, (float)Width, (float)Height, (float)0, -1.0F, 10.0F);

	if (m_DeviceResizedCallback != nullptr)
		m_DeviceResizedCallback(Width, Height);
}

void Framework::ProcessInput(void)
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