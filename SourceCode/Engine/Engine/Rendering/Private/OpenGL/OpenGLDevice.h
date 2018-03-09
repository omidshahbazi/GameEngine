// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				class OpenGLDevice : public IDevice
				{
				public:
					OpenGLDevice(void);
					~OpenGLDevice(void);

					bool Initialize(void) override;

					void SetSampleCount(uint8 Count) override;

					void SetForwardCompatible(bool Value)override;

					void SetProfilingEnabled(bool Value) override;

					void SetClearColor(Color Color) override;

					bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Handle &Handle) override;
					bool DestroyTexture2D(Texture::Handle Handle) override;

					bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle) override;
					bool DestroyProgram(Program::Handle Handle) override;

					bool CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle) override;
					bool DestroyWindow(Window::Handle Handle) override;

					void Clear(ClearFlags Flags) override;

					INLINE cstr GetLastError(void) const override
					{
						return m_LastError;
					}

				private:
					str m_LastError;
				};
			}
		}
	}
}

#endif