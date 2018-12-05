// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Rendering\NativeType.h>
#include <Rendering\Color.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class RENDERING_API Program : public NativeType
		{
		public:
			typedef int32 ConstantHandle;

		public:
			Program(IDevice *Device, Handle Handle) :
				NativeType(Device, Handle)
			{
			}

			bool SetFloat32(Program::ConstantHandle Handle, float32 Value);
			bool SetColor(Program::ConstantHandle Handle, Color Value);
			//bool SetMatrix4(Program::ConstantHandle Handle, const Matrix4 &Value);

			bool SetFloat32(const String &Name, float32 Value);
			bool SetColor(const String &Name, Color Value);
			//bool SetMatrix4(const String &Name, const Matrix4 &Value);
		};
	}
}

#endif