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

		public:
			Pass(void);
			Pass(ProgramResource* Program);
			Pass(const Pass& Other);

			bool SetFloat32(ConstantHash Hash, float32 Value) override;
			bool SetColor(ConstantHash Hash, const ColorUI8& Value) override;
			bool SetVector2(ConstantHash Hash, const Vector2F& Value) override;
			bool SetVector3(ConstantHash Hash, const Vector3F& Value) override;
			bool SetVector4(ConstantHash Hash, const Vector4F& Value) override;
			bool SetMatrix4(ConstantHash Hash, const Matrix4F& Value) override;
			bool SetTexture(ConstantHash Hash, const TextureResource* Value) override;
			bool SetSprite(ConstantHash Hash, const SpriteResource* Value) override;

			bool SetFloat32(const String& Name, float32 Value) override;
			bool SetColor(const String& Name, const ColorUI8& Value) override;
			bool SetVector2(const String& Name, const Vector2F& Value) override;
			bool SetVector3(const String& Name, const Vector3F& Value) override;
			bool SetVector4(const String& Name, const Vector4F& Value) override;
			bool SetMatrix4(const String& Name, const Matrix4F& Value) override;
			bool SetTexture(const String& Name, const TextureResource* Value) override;
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

			ConstantHash GetConstantHash(const String& Name);

			INLINE Pass& operator=(const Pass& Other)
			{
				m_Program = Other.m_Program;
				m_Queue = Other.m_Queue;
				m_ConstantsInfo = Other.m_ConstantsInfo;
				m_RenderState = Other.m_RenderState;

				return *this;
			}

		private:
			INLINE Program::ConstantInfoMap& GetConstants(void)
			{
				return m_ConstantsInfo;
			}

			INLINE const Program::ConstantInfoMap& GetConstants(void) const
			{
				return m_ConstantsInfo;
			}

			bool SetConstantValue(Program::ConstantHash Hash, const AnyDataType& Value);

		private:
			ProgramResource* m_Program;
			RenderQueues m_Queue;
			Program::ConstantInfoMap m_ConstantsInfo;
			IDevice::State m_RenderState;
		};
	}
}

#endif