
#include <GL\glew.h>
#include <glfw\glfw3.h>

#include <vector>

#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>
#include <Parallelizing\JobManager.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <Platform\PlatformFile.h>

using namespace Engine::Common;
using namespace Engine::Debugging;
using namespace Engine::Parallelizing;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Platform;

DynamicSizeAllocator allocator("String Allocator", &DefaultAllocator::GetInstance(), 1024 * 1024);

template<typename T> T *Allocate(uint32 Count)
{
	return reinterpret_cast<T*>(AllocateMemory(&allocator, Count * sizeof(T)));
}

template<typename T> void Deallocate(T *Ptr)
{
	allocator.Deallocate(reinterpret_cast<byte*>(Ptr));
}

void Initialize()
{
	Assert(glfwInit() == GLFW_TRUE, "GLFW initialization failed");

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* CreateWindow()
{
	GLFWwindow *window = glfwCreateWindow(1024, 768, "Test Rendering", nullptr, nullptr);

	Assert(window != nullptr, "Window creation failed");

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	Assert(glewInit() == GLEW_OK, "GLEW initialization failed");

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return window;
}

uint32 LoadShader(cstr VertexShader, cstr FragmentShader)
{
	uint32 vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	uint32 fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertShaderID, 1, &VertexShader, nullptr);
	glCompileShader(vertShaderID);

	glShaderSource(fragShaderID, 1, &FragmentShader, nullptr);
	glCompileShader(fragShaderID);

	int32 result;
	int32 infoLogLength;

	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength != 0)
	{
		str info = Allocate<char>(infoLogLength + 1);

		glGetShaderInfoLog(vertShaderID, infoLogLength, nullptr, info);

		Assert(false, info);

		Deallocate(info);
	}

	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength != 0)
	{
		str info = Allocate<char>(infoLogLength + 1);

		glGetShaderInfoLog(fragShaderID, infoLogLength, nullptr, info);

		Assert(false, info);

		Deallocate(info);
	}

	uint32 programID = glCreateProgram();
	glAttachShader(programID, vertShaderID);
	glAttachShader(programID, fragShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength != 0)
	{
		str info = Allocate<char>(infoLogLength + 1);

		glGetProgramInfoLog(programID, infoLogLength, nullptr, info);

		Assert(false, info);

		Deallocate(info);
	}

	glDetachShader(programID, vertShaderID);
	glDetachShader(programID, fragShaderID);

	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);

	return programID;
}

uint32 LoadShaderFromFile(cstr VertexShader, cstr FragmentShader)
{
	PlatformFile::Handle vertFile = PlatformFile::Open(VertexShader, PlatformFile::OpenModes::Input);
	Assert(vertFile != 0, "Couldn't open vertex shader file");
	uint32 vertFileSize = PlatformFile::Size(vertFile);
	str vertShader = Allocate<char>(vertFileSize + 1);
	//Assert(PlatformFile::Read(vertFile, vertShader, vertFileSize), "Couldn't read vertex shader file");
	PlatformFile::Read(vertFile, vertShader, vertFileSize);
	PlatformFile::Close(vertFile);

	PlatformFile::Handle fragFile = PlatformFile::Open(FragmentShader, PlatformFile::OpenModes::Input);
	Assert(vertFile != 0, "Couldn't open fragment shader file");
	uint32 fragFileSize = PlatformFile::Size(fragFile);
	str fragShader = Allocate<char>(fragFileSize + 1);
	//Assert(PlatformFile::Read(fragFile, fragShader, fragFileSize), "Couldn't read fragment shader file");
	PlatformFile::Read(fragFile, fragShader, fragFileSize);
	PlatformFile::Close(fragFile);

	uint32 programID = LoadShader(vertShader, fragShader);

	Deallocate(vertShader);
	Deallocate(fragShader);

	return programID;
}

void main()
{
	Initialize();
	GLFWwindow *window = CreateWindow();

	uint32 programID = LoadShaderFromFile("E:/1.vert", "E:/1.frag");

	uint32 vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	const float32 vertexBufferData[] = {
		-1.0F, -1.0F, 0.0F,
		1.0F, -1.0F, 0.0F,
		0.0F, 1.0F, 0.0F
	};

	uint32 vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == GLFW_FALSE);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(programID);

	glfwTerminate();
}