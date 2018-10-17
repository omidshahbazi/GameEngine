#pragma once

#include <functional>
#include <vector>
#include <glm\matrix.hpp>
#include "Color.h"

class GameObject;
class Component;

enum class Keys
{
	None = 0,
	Back,
	Tab,
	Return,
	Enter,
	CapsLock,
	Escape,
	Space,
	Prior,
	PageUp,
	Next,
	PageDown,
	End,
	Home,
	Left,
	Up,
	Right,
	Down,
	Insert,
	Delete,
	D0,
	D1,
	D2,
	D3,
	D4,
	D5,
	D6,
	D7,
	D8,
	D9,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Multiply,
	Add,
	Subtract,
	Decimal,
	Divide,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,
	LShiftKey,
	RShiftKey,
	LControlKey,
	RControlKey,
	LMenu,
	RMenu,
	OemSemicolon,
	Oemcomma,
	OemMinus,
	OemPeriod,
	Oem3,
	OemOpenBrackets,
	Oem4,
	Oem5,
	OemCloseBrackets,
	Oem6,
	OemQuotes,
	Oem7,
	OemBackslash,
	Oem102,
	COUNT,
};

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
	inline Color GetClearColor(void) const
	{
		return m_ClearColor;
	}

	void SetWireframeEnabled(bool Enabled);
	inline bool GetWireframeEnabled(void) const
	{
		return m_WireframeEnabled;
	}

	void Run(void);

	void Shutdown(void);

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

	inline void SetOnInitialize(InitializeCallback Callback)
	{
		m_InitializeCallback = Callback;
	}

	inline void SetOnDeinitialize(InitializeCallback Callback)
	{
		m_DeinitializeCallback = Callback;
	}

	inline void SetOnUpdate(UpdateCallback Callback)
	{
		m_UpdateCallback = Callback;
	}

	inline void SetOnRender(RenderCallback Callback)
	{
		m_RenderCallback = Callback;
	}

	inline void SetOnDeviceResized(DeviceResizedCallback Callback)
	{
		m_DeviceResizedCallback = Callback;
	}

	inline bool IsKeyDown(Keys Key)
	{
		return m_IsKeyDown[(int)Key];
	}

	inline bool IsKeyPressed(Keys Key)
	{
		return (m_WasKeyDown[(int)Key] && !m_IsKeyDown[(int)Key]);
	}

private:
	void OnComponentAdded(Component *Component);

	void HandleDeviceResize(int Width, int Height);

	void ProcessInput(void);

	void ResetKeyStates(bool *KeyArray);

private:
	float m_Time;
	float m_DeltaTime;
	float m_FPS;

	void *m_Window;

	bool m_WasKeyDown[(int)Keys::COUNT];
	bool m_IsKeyDown[(int)Keys::COUNT];

	Color m_ClearColor;
	bool m_WireframeEnabled;

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