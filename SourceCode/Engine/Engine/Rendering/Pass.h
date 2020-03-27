// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <Rendering\IDevice.h>
#include <Rendering\RenderingCommon.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;

		class RENDERING_API Pass
		{
		public:
			Pass(ProgramHandle* Program);

			bool SetFloat32(const String& Name, float32 Value);
			bool SetVector2(const String& Name, const Vector2F& Value);
			bool SetVector3(const String& Name, const Vector3F& Value);
			bool SetVector4(const String& Name, const Vector4F& Value);
			bool SetColor(const String& Name, Color Value);
			bool SetMatrix4(const String& Name, const Matrix4F& Value);
			bool SetTexture(const String& Name, const TextureHandle* Value);

			ProgramHandle* GetProgram(void);

			void SetProgram(ProgramHandle* Program);

			INLINE Program::ConstantInfoList& GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const Program::ConstantInfoList& GetConstants(void) const
			{
				return m_Constants;
			}

			INLINE void SetQueue(RenderQueues Queue)
			{
				m_Queue = Queue;
			}

			INLINE RenderQueues GetQueue(void) const
			{
				return m_Queue;
			}

			IDevice::State& GetRenderState(void)
			{
				return m_RenderState;
			}

			const IDevice::State& GetRenderState(void) const
			{
				return m_RenderState;
			}
			void SetRenderState(const IDevice::State& State);

		private:
			bool SetConstantValue(const String& Name, const AnyDataType& Value);

		private:
			ProgramHandle* m_Program;
			RenderQueues m_Queue;
			Program::ConstantInfoList m_Constants;
			IDevice::State m_RenderState;
		};
	}
}

#endif