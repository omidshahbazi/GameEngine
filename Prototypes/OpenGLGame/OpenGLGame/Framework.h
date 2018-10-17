#pragma once

#include <functional>
#include <vector>
#include <glm\matrix.hpp>
#include "Color.h"

class GameObject;
class Component;

class Framework
{
	friend class GameObject;

public:
	typedef std::vector<GameObject*> GameObjectList;
	typedef std::vector<Component*> ComponentList;

	typedef std::function<void(void)> InitializeCallback;
	typedef std::function<void(void)> DeinitializeCallback;
	typedef std::function<void(void)> UpdateCallback;
	typedef std::function<void(void)> RenderCallback;
	typedef std::function<void(int Width, int Height)> DeviceResizedCallback;

public:
	Framework(void);

public:
	void Initialize(void);
	void Deinitialize(void);

	bool CreateWindow(const char *Title, unsigned int Width, unsigned int Height);

	void SetClearColor(Color Color);

	void Run(void);

	GameObject *CreateGameObject(void);

	inline float GetTime(void) const
	{
		return m_Time;
	}

	inline float GetDeltaTime(void) const
	{
		return m_DeltaTime;
	}

	inline float GetFPS(void) const
	{
		return m_FPS;
	}

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

private:
	void OnComponentAdded(Component *Component);

	void HandleDeviceResize(int Width, int Height);

	void ProcessInput(void);

private:
	float m_Time;
	float m_DeltaTime;
	float m_FPS;

	void *m_Window;

	glm::mat4 m_ProjectionMatrix;

	GameObjectList m_GameObjects;
	ComponentList m_Components;

	GameObject *m_CameraGameObject;

	InitializeCallback m_InitializeCallback;
	DeinitializeCallback m_DeinitializeCallback;
	UpdateCallback m_UpdateCallback;
	RenderCallback m_RenderCallback;
	DeviceResizedCallback m_DeviceResizedCallback;
};