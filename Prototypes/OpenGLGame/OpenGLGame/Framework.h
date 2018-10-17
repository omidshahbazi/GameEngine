#pragma once

#include "Color.h"
#include <functional>

class Framework
{
public:
	typedef std::function<void(void)> InitializeCallback;
	typedef std::function<void(void)> DeinitializeCallback;
	typedef std::function<void(float Time)> UpdateCallback;
	typedef std::function<void(float Time)> RenderCallback;
	typedef std::function<void(int Width, int Height)> DeviceResizedCallback;

public:
	Framework(void);

public:
	void SetOnInitialize(InitializeCallback Callback)
	{
		m_InitializeCallback = Callback;
	}

	void SetOnDeinitialize(InitializeCallback Callback)
	{
		m_DeinitializeCallback = Callback;
	}

	void SetOnUpdate(UpdateCallback Callback)
	{
		m_UpdateCallback = Callback;
	}

	void SetOnRender(RenderCallback Callback)
	{
		m_RenderCallback = Callback;
	}

	void SetOnDeviceResized(DeviceResizedCallback Callback)
	{
		m_DeviceResizedCallback = Callback;
	}

	void Initialize(void);
	void Deinitialize(void);

	bool CreateWindow(const char *Title, unsigned int Width, unsigned int Height);

	void SetClearColor(Color Color);

	void Run(void);

private:
	void HandleDeviceResize(int Width, int Height);

	void ProcessInput(void);

private:
	void *m_Window;

	InitializeCallback m_InitializeCallback;
	DeinitializeCallback m_DeinitializeCallback;
	UpdateCallback m_UpdateCallback;
	RenderCallback m_RenderCallback;
	DeviceResizedCallback m_DeviceResizedCallback;
};