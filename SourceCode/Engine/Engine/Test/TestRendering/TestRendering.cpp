
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Color.h>
#include <Rendering\IDevice.h>

#include <glfw\glfw3.h>

using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

void main()
{
	RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());

	DeviceInterfarce *device = rendering->CreateDevice(DeviceInterfarce::Type::OpenGL);

	device->Initialize();
	device->SetSampleCount(4);
	device->SetForwardCompatible(true);
	device->SetProfilingEnabled(true);
	device->SetClearColor(Color(0, 0, 0));

	Window *window = device->CreateWindow(WIDTH, HEIGHT, "Test Rendering");
	GLFWwindow *windowHandle = reinterpret_cast<GLFWwindow*>(window->GetHandle());

	do
	{
		device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer);

		glfwSwapBuffers(windowHandle);

		glfwPollEvents();

	} while (glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(windowHandle) == GLFW_FALSE);

	RenderingManager::Destroy();
}