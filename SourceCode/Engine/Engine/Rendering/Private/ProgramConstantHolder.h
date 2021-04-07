// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_HOLDER_H
#define PROGRAM_CONSTANT_HOLDER_H

#include <Rendering\ProgramDataTypes.h>
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
		class ConstantBuffer;

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

			class RENDERING_API ProgramConstantHolder
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

				template<typename T>
				struct ConstantInfo
				{
				public:
					ConstantInfo(void) :
						Hash(0)
					{
					}

					ConstantInfo(ConstantHash Hash, const T& Value) :
						Hash(Hash),
						Value(Value)
					{
					}

					ConstantInfo(const ConstantInfo& Other)
					{
						*this = Other;
					}

					INLINE ConstantInfo& operator =(const ConstantInfo& Other)
					{
						Hash = Other.Hash;
						Value = Other.Value;

						return *this;
					}

				public:
					ConstantHash Hash;
					T Value;
				};

				template<typename T>
				struct ConstantData : public ConstantInfo<T>
				{
				public:
					ConstantData(void) :
						Handle(0),
						Type(ProgramDataTypes::Unknown)
					{
					}

					ConstantData(ConstantHandle Handle, const String& Name, ProgramDataTypes Type, const T& Value) :
						ConstantInfo<T>(GetHash(Name), Value),
						Handle(Handle),
						Name(Name),
						Type(Type)
					{
					}

					ConstantData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, const T& Value = nullptr) :
						ConstantInfo<T>(GetHash(Name), {}),
						Handle(Handle),
						Name(Name),
						Type(ProgramDataTypes::Unknown),
						UserDefinedType(UserDefinedType)
					{
					}

					INLINE ConstantData& operator =(const ConstantData& Other)
					{
						ConstantInfo<T>::operator=(Other);

						Handle = Other.Handle;
						Name = Other.Name;
						Type = Other.Type;
						UserDefinedType = Other.UserDefinedType;

						return *this;
					}

				public:
					ConstantHandle Handle;
					String Name;
					ProgramDataTypes Type;
					String UserDefinedType;
				};

				typedef ConstantData<AnyDataType> AnyConstantData;
				typedef ConstantInfo<ConstantBuffer*> BufferConstantInfo;
				typedef ConstantInfo<TextureResource*> TextureConstantInfo;
				typedef ConstantData<ConstantBuffer*> BufferConstantData;
				typedef ConstantData<TextureResource*> TextureConstantData;

				typedef Vector<AnyConstantData> ConstantDataList;
				typedef Map<ConstantHash, BufferConstantInfo> BufferInfoMap;
				typedef Map<ConstantHash, TextureConstantInfo> TextureInfoMap;
				typedef Map<ConstantHash, BufferConstantData> BufferDataMap;
				typedef Map<ConstantHash, TextureConstantData> TextureDataMap;

			public:
				virtual ConstantBuffer* GetConstantBuffer(ConstantHash Hash) = 0;
				virtual ConstantBuffer* GetConstantBuffer(const String& Name) = 0;

				virtual bool SetBuffer(ConstantHash Hash, const byte* Data, uint16 Size);
				virtual bool SetBuffer(const String& Name, const byte* Data, uint16 Size)
				{
					return SetBuffer(GetHash(Name), Data, Size);
				}

				virtual bool SetTexture(ConstantHash Hash, const TextureResource* Value) = 0;
				virtual bool SetTexture(const String& Name, const TextureResource* Value) = 0;

				virtual bool SetSprite(ConstantHash Hash, const SpriteResource* Value) = 0;
				virtual bool SetSprite(const String& Name, const SpriteResource* Value) = 0;

				static ConstantHash GetHash(const String& Name);
			};
		}
	}
}

#endif