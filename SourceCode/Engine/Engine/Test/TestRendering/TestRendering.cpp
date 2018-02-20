
#include <GL\glew.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

#include <vector>

#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>
#include <Parallelizing\JobManager.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformFile.h>

using namespace Engine::Common;
using namespace Engine::Debugging;
using namespace Engine::Parallelizing;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Platform;

DynamicSizeAllocator allocator("String Allocator", &RootAllocator::GetInstance(), 1024 * 1024);

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

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
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Test Rendering", nullptr, nullptr);

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
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	static const GLfloat colorBufferData[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0F), ASPECT_RATIO, 0.1F, 100.0F);
	glm::mat4 viewMat = glm::lookAt(glm::vec3(4.0F, 3.0F, 3.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 1.0F, 0.0F));
	glm::mat4 modelMat = glm::mat4(1.0F);

	uint32 vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	uint32 colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 mvp = projectionMat * viewMat * modelMat;
		uint32 matID = glGetUniformLocation(programID, "MVP");

		glUniformMatrix4fv(matID, 1, GL_FALSE, &mvp[0][0]);

		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == GLFW_FALSE);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(programID);

	glfwTerminate();
}