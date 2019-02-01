// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Rendering\NativeType.h>
#include <Rendering\Color.h>
#include <Containers\Strings.h>
#include <Containers\MathContainers.h>
#include <Rendering\DataTypes.h>
#include <Containers\AnyDataType.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class Texture;

		class RENDERING_API Program : public NativeType
		{
			friend class DeviceInterface;

		public:
			typedef int32 ConstantHandle;

			struct ConstantData
			{
			public:
				ConstantHandle Handle;
				String Name;
				DataTypes Type;
				AnyDataType Value;
			};

			typedef Vector<ConstantData> ConstantDataList;

		private:
			Program(IDevice *Device, Handle Handle);

		public:
			bool SetFloat32(Program::ConstantHandle Handle, float32 Value);
			bool SetVector2(Program::ConstantHandle Handle, const Vector2F &Value);
			bool SetVector3(Program::ConstantHandle Handle, const Vector3F &Value);
			bool SetVector4(Program::ConstantHandle Handle, const Vector4F &Value);
			bool SetMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value);
			bool SetTexture(Program::ConstantHandle Handle, const Texture *Value);

			bool SetFloat32(const String &Name, float32 Value);
			bool SetVector2(const String &Name, const Vector2F &Value);
			bool SetVector3(const String &Name, const Vector3F &Value);
			bool SetVector4(const String &Name, const Vector4F &Value);
			bool SetMatrix4(const String &Name, const Matrix4F &Value);
			bool SetTexture(const String &Name, const Texture *Value);

			void ApplyConstantValue(const ConstantDataList &DataList);

			INLINE ConstantDataList &GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const ConstantDataList &GetConstants(void) const
			{
				return m_Constants;
			}

		private:
			ConstantData * GetConstantData(const String &Name);

		private:
			void QueryActiveConstants(void);

		private:
			ConstantDataList m_Constants;
		};
	}
}

#endif