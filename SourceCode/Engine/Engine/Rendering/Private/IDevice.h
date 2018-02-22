// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef I_DEVICE_H
#define I_DEVICE_H

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			class IDevice
			{
			public:
				enum class Type
				{
					OpenGL
				};

			public:
				typedef uint32 ProgramHandle;
				typedef uint32 TextureHandle;

				virtual bool Initialize(void) = 0;

				virtual bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, TextureHandle &Handle) = 0;

				virtual bool CreateProgram(cstr VertexShader, cstr FragmentShader, ProgramHandle &Handle) = 0;

				virtual Type GetType(void) const = 0;
			};
		}
	}
}

#endif