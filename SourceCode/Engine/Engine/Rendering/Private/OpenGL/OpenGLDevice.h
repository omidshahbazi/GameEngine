// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Rendering\IDevice.h>

#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

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
					bool Initialize(void) override;

					bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, TextureHandle &Handle) override;

					bool CreateProgram(cstr VertexShader, cstr FragmentShader, ProgramHandle &Handle) override;
				};
			}
		}
	}
}

#endif