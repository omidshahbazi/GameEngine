// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <RenderDevice\IDevice.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Private\ConstantData.h>
#include <ResourceCommon\Resource.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace ResourceCommon;
	using namespace RenderDevice;
	using namespace Containers;

	namespace RenderSystem
	{
		using namespace Private;

		class CommandBuffer;
		class ConstantBuffer;

		class RENDERSYSTEM_API Pass
		{
			friend class CommandBuffer;

		public:
			typedef MetaConstantData<ConstantBuffer*> BufferMetaConstantData;
			typedef MetaConstantData<TextureResource*> TextureMetaConstantData;
			typedef Map<ProgramConstantHash, BufferMetaConstantData> BufferMetaDataMap;
			typedef Map<ProgramConstantHash, TextureMetaConstantData> TextureMetaDataMap;

		public:
			Pass(void);
			Pass(ProgramResource* Program);
			Pass(const Pass& Other);
			Pass(Pass&& Other);
			~Pass(void);

			const ProgramResource* GetProgram(void) const
			{
				return m_Program;
			}

			bool SetProgram(ProgramResource* Program);

			ConstantBuffer* GetConstantBuffer(ProgramConstantHash Hash);
			ConstantBuffer* GetConstantBuffer(const String& Name);

			bool SetBuffer(ProgramConstantHash Hash, const byte* Data, uint16 Size);
			bool SetBuffer(const String& Name, const byte* Data, uint16 Size)
			{
				return SetBuffer(GetHash(Name), Data, Size);
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

			INLINE RenderQueues GetQueue(void) const
			{
				return m_Queue;
			}

			INLINE void SetQueue(RenderQueues Queue)
			{
				m_Queue = Queue;
			}

			INLINE RenderState& GetRenderState(void)
			{
				return m_RenderState;
			}

			INLINE const RenderState& GetRenderState(void) const
			{
				return m_RenderState;
			}

			void SetRenderState(const RenderState& State);

			INLINE Pass& operator=(const Pass& Other);

		private:
			void CreateBufferData(ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size);
			void CreateTextureData(ProgramConstantHandle Handle, const String& Name);
			void SyncData(const Pass& Other);

			INLINE const BufferMetaDataMap& GetBuffers(void) const
			{
				return m_Buffers;
			}

			INLINE const TextureMetaDataMap& GetTextures(void) const
			{
				return m_Textures;
			}

		public:
			static ProgramConstantHash GetHash(const String& Name);

		private:
			ProgramResource* m_Program;
			BufferMetaDataMap m_Buffers;
			TextureMetaDataMap m_Textures;
			RenderQueues m_Queue;
			RenderState m_RenderState;
		};
	}
}

#endif