#pragma once

#include <glm\matrix.hpp>
#include "Color.h"

class Shader
{
public:
	Shader(const char *VertexShader, const char *FragmentShader);

public:
	void Use(void);

	void SetFloat(const char *Name, float Value);

	void SetColor(const char *Name, const Color &Value);

	void SetMatrix(const char *Name, const glm::mat4 &Value);

	static Shader *CreateDefaultShader(void);

private:
	unsigned int Compile(unsigned int Type, const char *Shader);

private:
	unsigned int m_ProgramID;
};
