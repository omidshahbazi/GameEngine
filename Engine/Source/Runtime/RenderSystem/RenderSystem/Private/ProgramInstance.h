// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_INSTANCE_H
#define PROGRAM_INSTANCE_H

#include <RenderSystem\Sprite.h>
#include <RenderSystem\Private\ConstantData.h>

namespace Engine
{
	namespace RenderSystem
	{
		class CommandBuffer;
		class ConstantBuffer;
		class ProgramConstantSupplier;

		namespace Private
		{
			class RENDERSYSTEM_API ProgramInstance
			{
				friend class CommandBuffer;
				friend class ProgramConstantSupplier;

				typedef ProgramBufferMap<ConstantBuffer> ProgramConstantBufferMap;

			public:
				ProgramInstance(void);
				ProgramInstance(ProgramResource* Program);
				ProgramInstance(const ProgramInstance& Other);
				virtual ~ProgramInstance(void);

				const ProgramResource* GetProgram(void) const
				{
					return m_Program;
				}

				bool SetProgram(ProgramResource* Program);

				ConstantBuffer* GetConstantBuffer(ProgramConstantHash Hash, uint16 Size);
				ConstantBuffer* GetConstantBuffer(const String& Name, uint16 Size)
				{
					return GetConstantBuffer(GetHash(Name), Size);
				}

				bool SetBuffer(ProgramConstantHash Hash, const byte* Data, uint16 Size);
				bool SetBuffer(const String& Name, const byte* Data, uint16 Size)
				{
					return SetBuffer(GetHash(Name), Data, Size);
				}

				bool SetBuffer(ProgramConstantHash Hash, const ConstantBuffer* const Buffer);
				bool SetBuffer(const String& Name, const ConstantBuffer* const Buffer)
				{
					return SetBuffer(GetHash(Name), Buffer);
				}

				template<typename T, int Size = sizeof(T)>
				void SetBuffer(ProgramConstantHash Hash, const T* Data)
				{
					SetBuffer(Hash, ReinterpretCast(const byte*, Data), Size);
				}

				template<typename T, int Size = sizeof(T)>
				void SetBuffer(const String& Name, const T* Data)
				{
					SetBuffer(Name, ReinterpretCast(const byte*, Data), Size);
				}

				bool SetTexture(ProgramConstantHash Hash, const TextureResource* Value);
				bool SetTexture(const String& Name, const TextureResource* Value);

				bool SetSprite(ProgramConstantHash Hash, const SpriteResource* Value);
				bool SetSprite(const String& Name, const SpriteResource* Value);

				ProgramInstance& operator=(const ProgramInstance& Other);

			private:
				void OnProgramUpdated(ProgramResource* Resource);
				DECLARE_MEMBER_EVENT_LISTENER(ProgramInstance, OnProgramUpdated);

				INLINE const ProgramConstantHandleMap& GetHandles(void) const
				{
					return m_Handles;
				}

				INLINE const ProgramConstantBufferMap& GetBuffers(void) const
				{
					return m_Buffers;
				}

				INLINE const ProgramTextureMap& GetTextures(void) const
				{
					return m_Textures;
				}

			public:
				static ProgramConstantHash GetHash(const String& Name);

			private:
				ProgramResource* m_Program;
				ProgramConstantHandleMap m_Handles;
				ProgramConstantBufferMap m_Buffers;
				ProgramTextureMap m_Textures;
			};
		}
	}
}

#endif