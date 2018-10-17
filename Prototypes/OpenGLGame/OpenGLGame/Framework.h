#pragma once

#include "Color.h"

class Framework
{
public:
	Framework(void)
	{
	}

public:
	void Initialize(void);
	void Deinitialize(void);

	bool CreateWindow(const char *Title, unsigned int Width, unsigned int Height);

	void SetClearColor(Color Color);

	void Run(void);

private:
	void *m_Window;
};