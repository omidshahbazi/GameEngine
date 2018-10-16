
#include <GL\glew.h>
#include <glfw\glfw3.h>

struct Color
{
public:
	float R, G, B, A;
};

class Shader
{
public:
	Shader(const char *VertexShader, const char *FragmentShader)
	{
		unsigned int vertShader = Compile(GL_VERTEX_SHADER, VertexShader);
		unsigned int fragShader = Compile(GL_FRAGMENT_SHADER, FragmentShader);

		m_ProgramID = glCreateProgram();
		glAttachShader(m_ProgramID, vertShader);
		glAttachShader(m_ProgramID, fragShader);
		glLinkProgram(m_ProgramID);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}

public:
	void Use(void)
	{
		glUseProgram(m_ProgramID);
	}

	void SetFloat(const char *Name, float Value)
	{
		Use();

		glUniform1f(glGetUniformLocation(m_ProgramID, Name), Value);
	}

	void SetColor(const char *Name, const Color &Value)
	{
		Use();

		glUniform4f(glGetUniformLocation(m_ProgramID, Name), Value.R, Value.G, Value.B, Value.A);
	}

private:
	unsigned int Compile(unsigned int Type, const char *Shader)
	{
		unsigned int shader = glCreateShader(Type);
		glShaderSource(shader, 1, &Shader, nullptr);
		glCompileShader(shader);

		int result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (result == 0)
		{
			int infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength != 0)
			{
				const int ERROR_LENGTH = 1024;
				static char errorInfo[ERROR_LENGTH];
				glGetShaderInfoLog(shader, 1024, nullptr, errorInfo);

				return -1;
			}
		}

		return shader;
	}

private:
	unsigned int m_ProgramID;
};

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const Color CLEAR_COLOR = { 0.2F, 0.2F, 0.2F, 1.0F };

const char *VERTEX_SHADER = "#version 330 core\nlayout(location = 0) in vec3 inPosition;layout(location = 1) in vec2 inUV;out vec2 outUV;void main(){gl_Position = vec4(inPosition, 1.0);outUV=inUV;}";
const char *FRAGMENT_SHADER = "#version 330 core\nout vec4 FragColor;in vec2 outUV;uniform float Time;void main(){FragColor = vec4(sin(Time), outUV.x, outUV.y, 1);}";

void OnWindowResized(GLFWwindow *Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
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

void InitializeGLFW(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void InitializeGLEW(void)
{
	glewInit();
}

GLFWwindow *CreateWindow(int Width, int Height, const char *Title)
{
	GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

	if (window == nullptr)
		return nullptr;

	glfwSetFramebufferSizeCallback(window, OnWindowResized);

	OnWindowResized(window, Width, Height);

	glfwMakeContextCurrent(window);

	return window;
}

unsigned int CreateTriangleVAO()
{
	float vertices[] =
	{
		-0.5F, -0.5F, 0.0F,
		0.5F, -0.5F, 0.0F,
		0.0F, 0.5F, 0.0F
	};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	return vao;
}

unsigned int CreateQuadEBO()
{
	// Vertex Layout |Position:x,y,z|UV:u,v|

	float vertices[] =
	{
		-0.5F,	-0.5F,	0.0F,	0.0F,	0.0F,
		-0.5F,	0.5F,	0.0F,	0.0F,	1.0F,
		0.5F,	0.5F,	0.0F,	1.0F,	1.0F,
		0.5F,	-0.5F,	0.0F,	1.0F,	0.0F
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int positionElementIndex = 0;
	unsigned int uvElementIndex = 1;
	unsigned int positionElementCount = 3;
	unsigned int uvElementCount = 2;
	unsigned int positionElementSize = positionElementCount * sizeof(float);
	unsigned int uvElementSize = uvElementCount * sizeof(float);
	unsigned int vertexElementSize = positionElementSize + uvElementSize;

	glVertexAttribPointer(positionElementIndex, positionElementCount, GL_FLOAT, false, vertexElementSize, (void*)0);
	glEnableVertexAttribArray(positionElementIndex);

	glVertexAttribPointer(uvElementIndex, uvElementCount, GL_FLOAT, false, vertexElementSize, (void*)positionElementSize);
	glEnableVertexAttribArray(uvElementIndex);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

void main()
{
	InitializeGLFW();

	GLFWwindow *window = CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Game");

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);

	InitializeGLEW();

	unsigned int mesh = CreateQuadEBO();
	Shader shader(VERTEX_SHADER, FRAGMENT_SHADER);

	glClearColor(CLEAR_COLOR.R, CLEAR_COLOR.G, CLEAR_COLOR.B, CLEAR_COLOR.A);

	while (!glfwWindowShouldClose(window))
	{
		float nowTime = glfwGetTime();

		ProcessInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		shader.SetFloat("Time", nowTime);

		glBindVertexArray(mesh);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

Finalize:
	glfwTerminate();

	return;
}