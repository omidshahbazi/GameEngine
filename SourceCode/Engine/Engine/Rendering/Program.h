// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Rendering\NativeType.h>
#include <Rendering\Color.h>
#include <Containers\Strings.h>
#include <Containers\MathContainers.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class Texture;

		class RENDERING_API Program : public NativeType
		{
		public:
			typedef int32 ConstantHandle;

		public:
			Program(IDevice *Device, Handle Handle);

			bool SetFloat32(Program::ConstantHandle Handle, float32 Value);
			bool SetVector3(Program::ConstantHandle Handle, const Vector3F &Value);
			bool SetColor(Program::ConstantHandle Handle, Color Value);
			bool SetMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value);
			bool SetTexture(Program::ConstantHandle Handle, const Texture *Value);

			bool SetFloat32(const String &Name, float32 Value);
			bool SetVector3(const String &Name, const Vector3F &Value);
			bool SetColor(const String &Name, Color Value);
			bool SetMatrix4(const String &Name, const Matrix4F &Value);
			bool SetTexture(const String &Name, const Texture *Value);

			INLINE const StringList &GetConstants(void) const
			{
				return m_Constants;
			}

		private:
			void QueryActiveConstants(void);

		private:
			StringList m_Constants;
		};
	}
}

#endif