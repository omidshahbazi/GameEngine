// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <Rendering\IDevice.h>
#include <Rendering\Sprite.h>
#include <Rendering\RenderingCommon.h>
#include <Rendering\Private\ProgramConstantHolder.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}
		}

		class Texture;

		using namespace Private::Commands;

		class RENDERING_API Pass : public ProgramConstantHolder
		{
			friend class DrawCommand;

		private:
			typedef Vector<ConstantBuffer*> ConstantBufferList;

		public:
			Pass(void);
			Pass(ProgramResource* Program);
			Pass(const Pass& Other);
			~Pass(void);

			ConstantBuffer* GetConstantBuffer(ConstantHash Hash) override;
			ConstantBuffer* GetConstantBuffer(const String& Name) override;

			bool SetTexture(ConstantHash Hash, const TextureResource* Value) override;
			bool SetTexture(const String& Name, const TextureResource* Value) override;

			bool SetSprite(ConstantHash Hash, const SpriteResource* Value) override;
			bool SetSprite(const String& Name, const SpriteResource* Value) override;

			ProgramResource* GetProgram(void)
			{
				return m_Program;
			}

			void SetProgram(ProgramResource* Program);

			INLINE void SetQueue(RenderQueues Queue)
			{
				m_Queue = Queue;
			}

			INLINE RenderQueues GetQueue(void) const
			{
				return m_Queue;
			}

			INLINE IDevice::State& GetRenderState(void)
			{
				return m_RenderState;
			}

			INLINE const IDevice::State& GetRenderState(void) const
			{
				return m_RenderState;
			}
			void SetRenderState(const IDevice::State& State);

			INLINE Pass& operator=(const Pass& Other)
			{
				m_Program = Other.m_Program;
				m_Queue = Other.m_Queue;
				m_ConstantInfos = Other.m_ConstantInfos;
				m_RenderState = Other.m_RenderState;

				return *this;
			}

		private:
			INLINE ConstantInfoMap& GetConstants(void)
			{
				return m_ConstantInfos;
			}

			INLINE const ConstantInfoMap& GetConstants(void) const
			{
				return m_ConstantInfos;
			}

		private:
			ProgramResource* m_Program;
			RenderQueues m_Queue;
			ConstantInfoMap m_ConstantInfos;
			ConstantBufferList m_ConstantBuffers;
			IDevice::State m_RenderState;
		};
	}
}

#endif