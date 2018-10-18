
#include <gl\glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include "Framework.h"
#include "GameObject.h"
#include "Component.h"
#include "Camera.h"

Framework *g_Framework = nullptr;

unsigned int GetKeyID(Keys Key)
{
	switch (Key)
	{
	case Keys::Back: return GLFW_KEY_BACKSPACE;
	case Keys::Tab: return GLFW_KEY_TAB;
	case Keys::Enter: return GLFW_KEY_ENTER;
	case Keys::CapsLock: return GLFW_KEY_CAPS_LOCK;
	case Keys::Escape: return GLFW_KEY_ESCAPE;
	case Keys::Space: return GLFW_KEY_SPACE;
	case Keys::Prior: return GLFW_KEY_PAGE_UP;
	case Keys::Next: return GLFW_KEY_PAGE_DOWN;
	case Keys::End: return GLFW_KEY_END;
	case Keys::Home: return GLFW_KEY_HOME;
	case Keys::Left: return GLFW_KEY_LEFT;
	case Keys::Up: return GLFW_KEY_UP;
	case Keys::Right: return GLFW_KEY_RIGHT;
	case Keys::Down: return GLFW_KEY_DOWN;
	case Keys::Insert: return GLFW_KEY_INSERT;
	case Keys::Delete: return GLFW_KEY_DELETE;
	case Keys::D0: return GLFW_KEY_0;
	case Keys::D1: return GLFW_KEY_1;
	case Keys::D2: return GLFW_KEY_2;
	case Keys::D3: return GLFW_KEY_3;
	case Keys::D4: return GLFW_KEY_4;
	case Keys::D5: return GLFW_KEY_5;
	case Keys::D6: return GLFW_KEY_6;
	case Keys::D7: return GLFW_KEY_7;
	case Keys::D8: return GLFW_KEY_8;
	case Keys::D9: return GLFW_KEY_9;
	case Keys::A: return GLFW_KEY_A;
	case Keys::B: return GLFW_KEY_B;
	case Keys::C: return GLFW_KEY_C;
	case Keys::D: return GLFW_KEY_D;
	case Keys::E: return GLFW_KEY_E;
	case Keys::F: return GLFW_KEY_F;
	case Keys::G: return GLFW_KEY_G;
	case Keys::H: return GLFW_KEY_H;
	case Keys::I: return GLFW_KEY_I;
	case Keys::J: return GLFW_KEY_J;
	case Keys::K: return GLFW_KEY_K;
	case Keys::L: return GLFW_KEY_L;
	case Keys::M: return GLFW_KEY_M;
	case Keys::N: return GLFW_KEY_N;
	case Keys::O: return GLFW_KEY_O;
	case Keys::P: return GLFW_KEY_P;
	case Keys::Q: return GLFW_KEY_Q;
	case Keys::R: return GLFW_KEY_R;
	case Keys::S: return GLFW_KEY_S;
	case Keys::T: return GLFW_KEY_T;
	case Keys::U: return GLFW_KEY_U;
	case Keys::V: return GLFW_KEY_V;
	case Keys::W: return GLFW_KEY_W;
	case Keys::X: return GLFW_KEY_X;
	case Keys::Y: return GLFW_KEY_Y;
	case Keys::Z: return GLFW_KEY_Z;
	case Keys::Multiply: return GLFW_KEY_KP_MULTIPLY;
	case Keys::Add: return GLFW_KEY_KP_ADD;
	case Keys::Subtract: return GLFW_KEY_KP_SUBTRACT;
	case Keys::Decimal: return GLFW_KEY_KP_DECIMAL;
	case Keys::Divide: return GLFW_KEY_KP_DIVIDE;
	case Keys::F1: return GLFW_KEY_F1;
	case Keys::F2: return GLFW_KEY_F2;
	case Keys::F3: return GLFW_KEY_F3;
	case Keys::F4: return GLFW_KEY_F4;
	case Keys::F5: return GLFW_KEY_F5;
	case Keys::F6: return GLFW_KEY_F6;
	case Keys::F7: return GLFW_KEY_F7;
	case Keys::F8: return GLFW_KEY_F8;
	case Keys::F9: return GLFW_KEY_F9;
	case Keys::F10: return GLFW_KEY_F10;
	case Keys::F11: return GLFW_KEY_F11;
	case Keys::F12: return GLFW_KEY_F12;
	case Keys::F13: return GLFW_KEY_F13;
	case Keys::F14: return GLFW_KEY_F14;
	case Keys::F15: return GLFW_KEY_F15;
	case Keys::F16: return GLFW_KEY_F16;
	case Keys::F17: return GLFW_KEY_F17;
	case Keys::F18: return GLFW_KEY_F18;
	case Keys::F19: return GLFW_KEY_F19;
	case Keys::F20: return GLFW_KEY_F20;
	case Keys::F21: return GLFW_KEY_F21;
	case Keys::F22: return GLFW_KEY_F22;
	case Keys::F23: return GLFW_KEY_F23;
	case Keys::F24: return GLFW_KEY_F24;
	case Keys::LShiftKey: return GLFW_KEY_LEFT_SHIFT;
	case Keys::RShiftKey: return GLFW_KEY_RIGHT_SHIFT;
	case Keys::LControlKey: return GLFW_KEY_LEFT_CONTROL;
	case Keys::RControlKey: return GLFW_KEY_RIGHT_CONTROL;
	case Keys::LMenu: return GLFW_KEY_MENU;
	case Keys::RMenu: return GLFW_KEY_MENU;
	case Keys::OemSemicolon: return GLFW_KEY_SEMICOLON;
	case Keys::Oemcomma: return GLFW_KEY_COMMA;
	case Keys::OemMinus: return GLFW_KEY_MINUS;
	case Keys::OemPeriod: return GLFW_KEY_PERIOD;
	case Keys::OemOpenBrackets: return GLFW_KEY_RIGHT_BRACKET;
	case Keys::OemCloseBrackets: return GLFW_KEY_LEFT_BRACKET;
	case Keys::OemQuotes: return GLFW_KEY_APOSTROPHE;
	case Keys::OemBackslash: return GLFW_KEY_BACKSLASH;
	}

	return GLFW_KEY_UNKNOWN;
}

Framework::Framework(void) :
	m_Window(nullptr),
	m_Time(0.0F),
	m_DeltaTime(0.0F),
	m_FPS(0.0F),
	m_ClearColor(0, 0, 0, 1),
	m_WireframeEnabled(false)
{
	g_Framework = this;

	ResetKeyStates(m_WasKeyDown);
	ResetKeyStates(m_IsKeyDown);
}

void Framework::Initialize(void)
{
	g_Framework = this;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
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
	m_ClearColor = Color;
	glClearColor(m_ClearColor.GetNormalizeR(), m_ClearColor.GetNormalizeG(), m_ClearColor.GetNormalizeB(), m_ClearColor.GetNormalizeA());
}

void Framework::SetWireframeEnabled(bool Enabled)
{
	m_WireframeEnabled = Enabled;
	glPolygonMode(GL_FRONT_AND_BACK, (m_WireframeEnabled ? GL_LINE : GL_FILL));
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

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

void Framework::Shutdown(void)
{
	GLFWwindow *window = reinterpret_cast<GLFWwindow*>(m_Window);

	glfwSetWindowShouldClose(window, true);
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
	m_DeviceWidth = Width;
	m_DeviceHeight = Height;

	glViewport(0, 0, m_DeviceWidth, m_DeviceHeight);
	m_ProjectionMatrix = glm::ortho(0.0F, (float)m_DeviceWidth, (float)m_DeviceHeight, (float)0, -1.0F, 10.0F);

	if (m_DeviceResizedCallback != nullptr)
		m_DeviceResizedCallback(m_DeviceWidth, m_DeviceHeight);
}

void Framework::ProcessInput(void)
{
	GLFWwindow *window = reinterpret_cast<GLFWwindow*>(m_Window);

	for (int i = (int)Keys::None; i < (int)Keys::COUNT; ++i)
	{
		m_WasKeyDown[i] = m_IsKeyDown[i];

		m_IsKeyDown[i] = (glfwGetKey(window, GetKeyID((Keys)i)) == GLFW_PRESS);
	}
}

void Framework::ResetKeyStates(bool *KeyArray)
{
	memset(KeyArray, 0, (int)Keys::COUNT);
}