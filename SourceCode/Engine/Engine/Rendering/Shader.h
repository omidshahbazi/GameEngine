// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <Rendering\NativeType.h>
#include <Rendering\ShaderDataType.h>
#include <Containers\Color.h>
#include <Containers\Strings.h>
#include <Containers\AnyDataType.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class Texture;

		class RENDERING_API Shader : public NativeType
		{
			friend class DeviceInterface;

		public:
			typedef int32 ConstantHandle;

			struct ConstantInfo
			{
			public:
				ConstantInfo(void)
				{
				}

				ConstantInfo(const String& Name, const AnyDataType& Value) :
					Name(Name),
					Value(Value)
				{
				}

				ConstantInfo(const ConstantInfo& Other)
				{
					*this = Other;
				}

				INLINE ConstantInfo& operator =(const ConstantInfo& Other)
				{
					Name = Other.Name;
					Value = Other.Value;

					return *this;
				}

			public:
				String Name;
				AnyDataType Value;
			};

			struct ConstantData : public ConstantInfo
			{
			public:
				ShaderDataType::Types Type;
				ConstantHandle Handle;
			};

			typedef Vector<ConstantInfo> ConstantInfoList;
			typedef Vector<ConstantData> ConstantDataList;

		private:
			Shader(IDevice* Device, Handle Handle);

		public:
			bool SetFloat32(ConstantHandle Handle, float32 Value);
			bool SetColor(ConstantHandle Handle, const ColorUI8& Value);
			bool SetVector2(ConstantHandle Handle, const Vector2F& Value);
			bool SetVector3(ConstantHandle Handle, const Vector3F& Value);
			bool SetVector4(ConstantHandle Handle, const Vector4F& Value);
			bool SetMatrix4(ConstantHandle Handle, const Matrix4F& Value);
			bool SetTexture(ConstantHandle Handle, const Texture* Value);

			bool SetFloat32(const String& Name, float32 Value);
			bool SetColor(const String& Name, const ColorUI8& Value);
			bool SetVector2(const String& Name, const Vector2F& Value);
			bool SetVector3(const String& Name, const Vector3F& Value);
			bool SetVector4(const String& Name, const Vector4F& Value);
			bool SetMatrix4(const String& Name, const Matrix4F& Value);
			bool SetTexture(const String& Name, const Texture* Value);

			void ApplyConstantValue(const ConstantInfoList& Constants);

			INLINE ConstantDataList& GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const ConstantDataList& GetConstants(void) const
			{
				return m_Constants;
			}

		private:
			ConstantData* GetConstantData(const String& Name);

		private:
			void QueryActiveConstants(void);

		private:
			ConstantDataList m_Constants;
		};

		typedef ResourceSystem::ResourceHandle<Shader> ShaderHandle;
	}
}

#endif