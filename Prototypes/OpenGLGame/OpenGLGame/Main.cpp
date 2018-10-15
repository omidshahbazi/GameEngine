
#include <GL\glew.h>
#include <glfw\glfw3.h>
#include <iostream>

struct Color
{
public:
	float R, G, B, A;
};

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const Color CLEAR_COLOR = { 0.2F, 0.2F, 0.2F, 1.0F };

const char *VERTEX_SHADER = "#version 330 core\nlayout(location = 0) in vec3 aPos;void main(){gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}";
const char *FRAGMENT_SHADER = "#version 330 core\nout vec4 FragColor;void main(){FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}";

void OnWindowResized(GLFWwindow *Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

void ProcessInput(GLFWwindow *Window)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(Window, true);
}

GLFWwindow *CreateWindow(int Width, int Height, const char *Title)
{
	GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

	if (window == nullptr)
		return nullptr;

	glfwSetFramebufferSizeCallback(window, OnWindowResized);

	OnWindowResized(window, Width, Height);

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
	float vertices[] =
	{
		-0.5F, -0.5F, 0.0F,
		-0.5F, 0.5F, 0.0F,
		0.5F, 0.5F, 0.0F,
		0.5F, -0.5F, 0.0F
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

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	return vao;
}

unsigned int CompileShader(const char *VertexShader, const char *FragmentShader)
{
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &VertexShader, nullptr);
	glCompileShader(vertShader);

	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &FragmentShader, nullptr);
	glCompileShader(fragShader);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	return program;
}

void main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Game");
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		return;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	unsigned int mesh = CreateQuadEBO();
	unsigned int shader = CompileShader(VERTEX_SHADER, FRAGMENT_SHADER);

	glClearColor(CLEAR_COLOR.R, CLEAR_COLOR.G, CLEAR_COLOR.B, CLEAR_COLOR.A);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(mesh);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

Finalize:
	glfwTerminate();

	return;
}