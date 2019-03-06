// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Containers\Strings.h>
#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;

	namespace Utility
	{
		class UTILITY_API Window
		{
		public:
			Window(const String &Name);

		public:
			bool Initialize(void);

			void SetTitle(const String &Title);
			void SetSize(uint16 Width, uint16 Height);

			bool ShouldClose(void) const;

			INLINE PlatformWindow::Handle GetHandle(void) const
			{
				return m_Handle;
			}





			//bool OpenGLDevice::CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle)
			//{
			//	GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

			//	if (window == nullptr)
			//	{
			//		PlatformMemory::Copy("Window creation failed", m_LastError, 22);

			//		return false;
			//	}

			//	glfwMakeContextCurrent(window);
			//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

			//	if (SecondInitialize())
			//		Handle = ReinterpretCast(Window::Handle, window);

			//	return true;
			//}

			//bool OpenGLDevice::DestroyWindow(Window::Handle Handle)
			//{
			//	//glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(Handle));

			//	return true;
			//}

			//Window *DeviceInterface::CreateWindowInternal(uint16 Width, uint16 Height, cstr Title)
			//{
			//	CHECK_DEVICE();

			//	//Window::Handle handle;
			//	//CHECK_CALL(m_Device->CreateWindow(Width, Height, Title, handle));

			//	Window *window = ALLOCATE(Window);
			//	//new (window) Window(m_Device, handle);

			//	return window;
			//}

			//void DeviceInterface::DestroyWindowInternal(Window *Window)
			//{
			//	CHECK_DEVICE();

			//	CHECK_CALL(m_Device->DestroyWindow(Window->GetHandle()));
			//	Window->~Window();
			//	DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Window);
			//}

		private:
			PlatformWindow::Handle m_Handle;
			String m_Name;
			String m_Title;
			uint16 m_Width;
			uint16 m_Height;
		};
	}
}

#endif