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
				class DrawCommand;
				class PassDrawCommand;
			}

			using namespace Private::Commands;

			class RENDERING_API ProgramConstantHolder
			{
				friend class ProgramConstantSupplier;
				friend class DrawCommand;
				friend class PassDrawCommand;

			public:
				typedef int32 ConstantHash;
				typedef int32 ConstantHandle;

			protected:
				template<typename T>
				struct ConstantDataBase
				{
				public:
					ConstantDataBase(void) :
						Handle(0)
					{
					}

					ConstantDataBase(ConstantHandle Handle, const T& Value) :
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
					ConstantHandle Handle;
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

					ConstantData(ConstantHandle Handle, const String& Name, const T& Value) :
						ConstantDataBase<T>(Handle, Value),
						Name(Name),
						Hash(GetHash(Name))
					{
					}

					ConstantData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, const T& Value) :
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

				bool SetTexture(ConstantHash Hash, const TextureResource* Value);
				bool SetTexture(const String& Name, const TextureResource* Value);

				bool SetSprite(ConstantHash Hash, const SpriteResource* Value);
				bool SetSprite(const String& Name, const SpriteResource* Value);

				static ConstantHash GetHash(const String& Name);

			protected:
				void CreateBufferData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size);
				void CreateTextureData(ConstantHandle Handle, const String& Name);
				void CloneData(const ProgramConstantHolder& Other);
				void MoveData(ProgramConstantHolder&& Other);
				void CleanupData(void);

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