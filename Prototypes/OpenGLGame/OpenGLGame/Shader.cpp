
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>
#include "Shader.h"

Shader::Shader(const char *VertexShader, const char *FragmentShader)
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

void Shader::Use(void)
{
	glUseProgram(m_ProgramID);
}

void Shader::SetFloat(const char *Name, float Value)
{
	glUniform1f(glGetUniformLocation(m_ProgramID, Name), Value);
}

void Shader::SetColor(const char *Name, const Color &Value)
{
	glUniform4f(glGetUniformLocation(m_ProgramID, Name), Value.R, Value.G, Value.B, Value.A);
}

void Shader::SetMatrix(const char *Name, const glm::mat4 &Value)
{
	glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, Name), 1, false, glm::value_ptr(Value));
}

unsigned int Shader::Compile(unsigned int Type, const char *Shader)
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

			std::cout << errorInfo << std::endl;

			return -1;
		}
	}

	return shader;
}

Shader *Shader::CreateDefaultShader(void)
{
	static const char *VERTEX_SHADER = "#version 330 core\nlayout(location = 0) in vec3 inPosition;uniform mat4 MVP;void main(){gl_Position = MVP * vec4(inPosition, 1.0);}";
	static const char *FRAGMENT_SHADER = "#version 330 core\nout vec4 FragColor;void main(){FragColor = vec4(1, 1, 1, 1);}";

	static Shader shader(VERTEX_SHADER, FRAGMENT_SHADER);

	return &shader;
}