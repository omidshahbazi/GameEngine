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
		class ProgramConstantSupplier;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommandBase;
				class ProgramDrawCommand;
				class PassDrawCommand;
			}

			using namespace Private::Commands;

			class RENDERING_API ProgramConstantHolder
			{
				friend class ProgramConstantSupplier;
				friend class DrawCommandBase;
				friend class ProgramDrawCommand;
				friend class PassDrawCommand;

			public:
				typedef int32 ConstantHash;
				typedef int32 ConstantHandle;

			protected:
				//template<typename T>
				//struct ConstantInfo
				//{
				//public:
				//	ConstantInfo(void) :
				//		Hash(0)
				//	{
				//	}

				//	ConstantInfo(ConstantHash Hash, const T& Value) :
				//		Hash(Hash),
				//		Value(Value)
				//	{
				//	}

				//	ConstantInfo(const ConstantInfo& Other)
				//	{
				//		*this = Other;
				//	}

				//	INLINE ConstantInfo& operator =(const ConstantInfo& Other)
				//	{
				//		Hash = Other.Hash;
				//		Value = Other.Value;

				//		return *this;
				//	}

				//public:
				//	ConstantHash Hash;
				//	T Value;
				//};

				//template<typename T>
				//struct ConstantData : public ConstantInfo<T>
				//{
				//public:
				//	ConstantData(void) :
				//		Handle(0),
				//		Type(ProgramDataTypes::Unknown)
				//	{
				//	}

				//	ConstantData(ConstantHandle Handle, const String& Name, ProgramDataTypes Type, const T& Value) :
				//		ConstantInfo<T>(GetHash(Name), Value),
				//		Handle(Handle),
				//		Name(Name),
				//		Type(Type)
				//	{
				//	}

				//	ConstantData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, const T& Value = nullptr) :
				//		ConstantInfo<T>(GetHash(Name), Value),
				//		Handle(Handle),
				//		Name(Name),
				//		Type(ProgramDataTypes::Unknown),
				//		UserDefinedType(UserDefinedType)
				//	{
				//	}

				//	INLINE ConstantData& operator =(const ConstantData& Other)
				//	{
				//		ConstantInfo<T>::operator=(Other);

				//		Handle = Other.Handle;
				//		Name = Other.Name;
				//		Type = Other.Type;
				//		UserDefinedType = Other.UserDefinedType;

				//		return *this;
				//	}

				//public:
				//	ConstantHandle Handle;
				//	String Name;
				//	ProgramDataTypes Type;
				//	String UserDefinedType;
				//};

				template<typename T>
				struct ConstantData
				{
				public:
					ConstantData(void) :
						Handle(0),
						Hash(0)
					{
					}

					ConstantData(ConstantHandle Handle, const String& Name, const T& Value) :
						Handle(Handle),
						Name(Name),
						Hash(GetHash(Name)),
						Value(Value)
					{
					}

					ConstantData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, const T& Value = nullptr) :
						Handle(Handle),
						Name(Name),
						Hash(GetHash(Name)),
						Value(Value),
						UserDefinedType(UserDefinedType)
					{
					}

					INLINE ConstantData& operator =(const ConstantData& Other)
					{
						Handle = Other.Handle;
						Name = Other.Name;
						Hash = Other.Hash;
						UserDefinedType = Other.UserDefinedType;
						Value = Other.Value;

						return *this;
					}

				public:
					ConstantHandle Handle;
					String Name;
					ConstantHash Hash;
					String UserDefinedType;
					T Value;
				};

				typedef ConstantData<ConstantBuffer*> BufferConstantData;
				typedef ConstantData<TextureResource*> TextureConstantData;

				typedef Map<ConstantHash, BufferConstantData> BufferDataMap;
				typedef Map<ConstantHash, TextureConstantData> TextureDataMap;

			public:
				virtual ~ProgramConstantHolder(void);

				ConstantBuffer* GetConstantBuffer(ConstantHash Hash);
				ConstantBuffer* GetConstantBuffer(const String& Name);

				bool SetBuffer(ConstantHash Hash, const byte* Data, uint16 Size);
				bool SetBuffer(const String& Name, const byte* Data, uint16 Size)
				{
					return SetBuffer(GetHash(Name), Data, Size);
				}

				bool SetTexture(ConstantHash Hash, const TextureResource* Value);
				bool SetTexture(const String& Name, const TextureResource* Value);

				bool SetSprite(ConstantHash Hash, const SpriteResource* Value);
				bool SetSprite(const String& Name, const SpriteResource* Value);

				static ConstantHash GetHash(const String& Name);

			protected:
				void CreateBufferData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size);
				void CreateTextureData(ConstantHandle Handle, const String& Name);
				void CloneData(const ProgramConstantHolder& Other);
				void CleanupData(void);

				//INLINE BufferDataMap& GetBuffers(void)
				//{
				//	return m_Buffers;
				//}

				INLINE const BufferDataMap& GetBuffers(void) const
				{
					return m_Buffers;
				}

				//INLINE TextureDataMap& GetTextures(void)
				//{
				//	return m_Textures;
				//}

				INLINE const TextureDataMap& GetTextures(void) const
				{
					return m_Textures;
				}

			private:
				BufferDataMap m_Buffers;
				TextureDataMap m_Textures;
			};
		}
	}
}

#endif