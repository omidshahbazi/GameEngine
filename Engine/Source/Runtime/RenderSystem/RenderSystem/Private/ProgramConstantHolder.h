// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_HOLDER_H
#define PROGRAM_CONSTANT_HOLDER_H

#include <RenderCommon\RenderCommon.h>
#include <Containers\Strings.h>
#include <Containers\AnyDataType.h>
#include <Containers\Map.h>
#include <MathContainers\MathContainers.h>
#include <ResourceCommon\Resource.h>

namespace Engine
{
	using namespace RenderCommon;
	using namespace ResourceCommon;
	using namespace Containers;
	using namespace MathContainers;

	namespace RenderSystem
	{
		class ConstantBuffer;

		namespace Private
		{
			class RENDERSYSTEM_API ProgramConstantHolder
			{
				friend class ProgramConstantSupplier;
				friend class CommandBuffer;

			public:
				typedef int32 ConstantHash;

			protected:
				template<typename T>
				struct ConstantDataBase
				{
				public:
					ConstantDataBase(void) :
						Handle(0)
					{
					}

					ConstantDataBase(ProgramConstantHandle Handle, const T& Value) :
						Handle(Handle),
						Value(Value)
					{
					}

					INLINE ConstantDataBase& operator =(const ConstantDataBase& Other)
					{
						Handle = Other.Handle;
						Value = Other.Value;

						return *this;
					}

				public:
					ProgramConstantHandle Handle;
					T Value;
				};

				template<typename T>
				struct ConstantData : public ConstantDataBase<T>
				{
				public:
					ConstantData(void) :
						Hash(0)
					{
					}

					ConstantData(ProgramConstantHandle Handle, const String& Name, const T& Value) :
						ConstantDataBase<T>(Handle, Value),
						Name(Name),
						Hash(GetHash(Name))
					{
					}

					ConstantData(ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, const T& Value) :
						ConstantDataBase<T>(Handle, Value),
						Name(Name),
						Hash(GetHash(Name)),
						UserDefinedType(UserDefinedType)
					{
					}

					INLINE ConstantData& operator =(const ConstantData& Other)
					{
						ConstantDataBase<T>::operator=(Other);

						Name = Other.Name;
						Hash = Other.Hash;
						UserDefinedType = Other.UserDefinedType;

						return *this;
					}

				public:
					String Name;
					ConstantHash Hash;
					String UserDefinedType;
				};

				typedef ConstantDataBase<ConstantBuffer*> BufferConstantDataBase;
				typedef ConstantDataBase<TextureResource*> TextureConstantDataBase;

				typedef Map<ConstantHash, BufferConstantDataBase> BufferDataBaseMap;
				typedef Map<ConstantHash, TextureConstantDataBase> TextureDataBaseMap;

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

				template<typename T, int Size = sizeof(T)>
				void SetBuffer(ConstantHash Hash, const T* Data)
				{
					SetBuffer(Hash, ReinterpretCast(const byte*, Data), Size);
				}

				template<typename T, int Size = sizeof(T)>
				void SetBuffer(const String& Name, const T* Data)
				{
					SetBuffer(Name, ReinterpretCast(const byte*, Data), Size);
				}

				bool SetTexture(ConstantHash Hash, const TextureResource* Value);
				bool SetTexture(const String& Name, const TextureResource* Value);

				bool SetSprite(ConstantHash Hash, const SpriteResource* Value);
				bool SetSprite(const String& Name, const SpriteResource* Value);

				static ConstantHash GetHash(const String& Name);

			protected:
				void CreateBufferData(ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size);
				void CreateTextureData(ProgramConstantHandle Handle, const String& Name);
				void SyncData(const ProgramConstantHolder& Other, bool IncludingValues);

				INLINE const BufferDataMap& GetBuffers(void) const
				{
					return m_Buffers;
				}

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