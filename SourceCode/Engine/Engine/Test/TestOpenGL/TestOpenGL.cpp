
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
#include <Platform\PlatformMemory.h>

using namespace Engine::Common;
using namespace Engine::Debugging;
using namespace Engine::Parallelizing;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Platform;

DynamicSizeAllocator allocator("String Allocator", RootAllocator::GetInstance(), 10 * MegaByte);

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_VERSION_MINOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_VERSION_MAJOR);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* CreateWindow()
{
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Test OpenGL", nullptr, nullptr);

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
	char8 *vertShader = Allocate<char8>(vertFileSize + 1);
	//Assert(PlatformFile::Read(vertFile, vertShader, vertFileSize), "Couldn't read vertex shader file");
	PlatformFile::Read(vertFile, vertShader, vertFileSize);
	PlatformFile::Close(vertFile);

	PlatformFile::Handle fragFile = PlatformFile::Open(FragmentShader, PlatformFile::OpenModes::Input);
	Assert(vertFile != 0, "Couldn't open fragment shader file");
	uint32 fragFileSize = PlatformFile::Size(fragFile);
	char8 *fragShader = Allocate<char8>(fragFileSize + 1);
	//Assert(PlatformFile::Read(fragFile, fragShader, fragFileSize), "Couldn't read fragment shader file");
	PlatformFile::Read(fragFile, fragShader, fragFileSize);
	PlatformFile::Close(fragFile);

	uint32 programID = LoadShader(vertShader, fragShader);

	Deallocate(vertShader);
	Deallocate(fragShader);

	return programID;
}

bool LoadBitmapImage(cstr FilePath, str *BitmapData, uint32 &Width, uint32 &Height)
{
	const uint32 HEADER_SIZE = 54;

	PlatformFile::Handle file = PlatformFile::Open(FilePath, PlatformFile::OpenModes::Input);
	Assert(file != 0, "Couldn't open bitmap file");

	uint32 size = PlatformFile::Size(file);

	str data = Allocate<char8>(size);
	PlatformFile::Read(file, data, size);

	PlatformFile::Close(file);

	bool result = true;

	if (data[0] == 'B' && data[1] == 'M')
	{
		uint32 dataPos = *(reinterpret_cast<uint32*>(&data[10]));
		if (dataPos == 0)
			dataPos = HEADER_SIZE;

		Width = *(reinterpret_cast<int32*>(&data[18]));
		Height = *(reinterpret_cast<int32*>(&data[22]));

		uint32 imageSize = Width * Height;

		*BitmapData = Allocate<char8>(imageSize);
		PlatformMemory::Copy(&data[dataPos], *BitmapData, imageSize);
	}
	else
		result = false;

	Deallocate(data);

	return result;
}

bool LoadBitmapTexture(cstr Data, uint32 Width, uint32 Height, uint32 *TextureID)
{
	glGenTextures(1, TextureID);
	glBindTexture(GL_TEXTURE_2D, *TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE, Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return true;
}

bool LoadBitmapTextureFromFile(cstr FilePath, uint32 *TextureID)
{
	str data = nullptr;
	uint32 width;
	uint32 height;
	if (!LoadBitmapImage(FilePath, &data, width, height))
		return false;

	bool result = LoadBitmapTexture(data, width, height, TextureID);

	Deallocate(data);

	return result;
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

	const float32 uvBufferData[] = {
	   0.000059f, 1.0f - 0.000004f,
	   0.000103f, 1.0f - 0.336048f,
	   0.335973f, 1.0f - 0.335903f,
	   1.000023f, 1.0f - 0.000013f,
	   0.667979f, 1.0f - 0.335851f,
	   0.999958f, 1.0f - 0.336064f,
	   0.667979f, 1.0f - 0.335851f,
	   0.336024f, 1.0f - 0.671877f,
	   0.667969f, 1.0f - 0.671889f,
	   1.000023f, 1.0f - 0.000013f,
	   0.668104f, 1.0f - 0.000013f,
	   0.667979f, 1.0f - 0.335851f,
	   0.000059f, 1.0f - 0.000004f,
	   0.335973f, 1.0f - 0.335903f,
	   0.336098f, 1.0f - 0.000071f,
	   0.667979f, 1.0f - 0.335851f,
	   0.335973f, 1.0f - 0.335903f,
	   0.336024f, 1.0f - 0.671877f,
	   1.000004f, 1.0f - 0.671847f,
	   0.999958f, 1.0f - 0.336064f,
	   0.667979f, 1.0f - 0.335851f,
	   0.668104f, 1.0f - 0.000013f,
	   0.335973f, 1.0f - 0.335903f,
	   0.667979f, 1.0f - 0.335851f,
	   0.335973f, 1.0f - 0.335903f,
	   0.668104f, 1.0f - 0.000013f,
	   0.336098f, 1.0f - 0.000071f,
	   0.000103f, 1.0f - 0.336048f,
	   0.000004f, 1.0f - 0.671870f,
	   0.336024f, 1.0f - 0.671877f,
	   0.000103f, 1.0f - 0.336048f,
	   0.336024f, 1.0f - 0.671877f,
	   0.335973f, 1.0f - 0.335903f,
	   0.667969f, 1.0f - 0.671889f,
	   1.000004f, 1.0f - 0.671847f,
	   0.667979f, 1.0f - 0.335851f
	};

	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0F), ASPECT_RATIO, 0.1F, 100.0F);
	glm::mat4 viewMat = glm::lookAt(glm::vec3(4.0F, 3.0F, 3.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 1.0F, 0.0F));
	glm::mat4 modelMat = glm::mat4(1.0F);

	uint32 vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	uint32 uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);

	uint32 texID = 0;
	LoadBitmapTextureFromFile("E:/1.bmp", &texID);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 mvp = projectionMat * viewMat * modelMat;
		uint32 matID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(matID, 1, GL_FALSE, &mvp[0][0]);

		glUseProgram(programID);

		glBindTexture(GL_TEXTURE_2D, texID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
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