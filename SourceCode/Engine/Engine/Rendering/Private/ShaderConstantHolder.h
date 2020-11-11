// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_CONSTANT_HOLDER_H
#define SHADER_CONSTANT_HOLDER_H

#include <Rendering\ShaderDataType.h>
#include <Containers\Color.h>
#include <Containers\Strings.h>
#include <Containers\AnyDataType.h>
#include <Containers\Map.h>
#include <MathContainers\MathContainers.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace Containers;
	using namespace MathContainers;

	namespace Rendering
	{
		class IDevice;

		namespace Private
		{
			class ThreadedDevice;

			namespace OpenGL
			{
				class OpenGLDevice;
			}

			namespace DirectX12
			{
				class DirectX12Device;
			}

			namespace Vulkan
			{
				class VulkanDevice;
			}

			using namespace OpenGL;
			using namespace DirectX12;
			using namespace Vulkan;

			class RENDERING_API ShaderConstantHolder
			{
				friend class IDevice;
				friend class ThreadedDevice;
				friend class OpenGLDevice;
				friend class DirectX12Device;
				friend class VulkanDevice;

			public:
				typedef int32 ConstantHash;

			protected:
				typedef int32 ConstantHandle;

				struct ConstantInfo
				{
				public:
					ConstantInfo(void) :
						Hash(0)
					{
					}

					ConstantInfo(const String& Name, const AnyDataType& Value);

					ConstantInfo(const ConstantInfo& Other)
					{
						*this = Other;
					}

					INLINE ConstantInfo& operator =(const ConstantInfo& Other)
					{
						Hash = Other.Hash;
						Name = Other.Name;
						Value = Other.Value;

						return *this;
					}

				public:
					ConstantHash Hash;
					String Name;
					AnyDataType Value;
				};

				struct ConstantData : public ConstantInfo
				{
				public:
					ConstantData(void) :
						Handle(0),
						Type(ShaderDataType::Types::Unknown)
					{
					}

					ConstantData(ConstantHandle Handle, const String& Name, ShaderDataType::Types Type, const AnyDataType& Value) :
						ConstantInfo(Name, Value),
						Handle(Handle),
						Type(Type)
					{
					}

				public:
					ConstantHandle Handle;
					ShaderDataType::Types Type;
				};

				typedef Vector<ConstantInfo> ConstantInfoList;
				typedef Vector<ConstantData> ConstantDataList;
				typedef Map<ConstantHash, ConstantInfo> ConstantInfoMap;
				typedef Map<ConstantHash, ConstantData> ConstantDataMap;

			public:
				virtual bool SetFloat32(ConstantHash Hash, float32 Value) = 0;
				virtual bool SetColor(ConstantHash Hash, const ColorUI8& Value) = 0;
				virtual bool SetVector2(ConstantHash Hash, const Vector2F& Value) = 0;
				virtual bool SetVector3(ConstantHash Hash, const Vector3F& Value) = 0;
				virtual bool SetVector4(ConstantHash Hash, const Vector4F& Value) = 0;
				virtual bool SetMatrix4(ConstantHash Hash, const Matrix4F& Value) = 0;
				virtual bool SetTexture(ConstantHash Hash, const TextureResource* Value) = 0;
				virtual bool SetSprite(ConstantHash Hash, const SpriteResource* Value) = 0;

				virtual bool SetFloat32(const String& Name, float32 Value) = 0;
				virtual bool SetColor(const String& Name, const ColorUI8& Value) = 0;
				virtual bool SetVector2(const String& Name, const Vector2F& Value) = 0;
				virtual bool SetVector3(const String& Name, const Vector3F& Value) = 0;
				virtual bool SetVector4(const String& Name, const Vector4F& Value) = 0;
				virtual bool SetMatrix4(const String& Name, const Matrix4F& Value) = 0;
				virtual bool SetTexture(const String& Name, const TextureResource* Value) = 0;
				virtual bool SetSprite(const String& Name, const SpriteResource* Value) = 0;

				static ConstantHash GetHash(const String& Name);
			};
		}
	}
}

#endif