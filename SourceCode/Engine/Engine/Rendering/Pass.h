// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <Rendering\IDevice.h>

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
			bool SetVector2(const String& Name, const Vector2I& Value);
			bool SetVector3(const String& Name, const Vector3F& Value);
			bool SetVector4(const String& Name, const Vector4F& Value);
			bool SetColor(const String& Name, Color Value);
			bool SetMatrix4(const String& Name, const Matrix4F& Value);
			bool SetTexture(const String& Name, const TextureHandle* Value);

			ProgramHandle* GetProgram(void);

			void SetProgram(ProgramHandle* Program);

			INLINE Program::ConstantDataList& GetShaderConstants(void)
			{
				return m_LastProgramPtr->GetConstants();
			}

			INLINE const Program::ConstantDataList& GetShaderConstants(void) const
			{
				return m_LastProgramPtr->GetConstants();
			}

			INLINE Program::ConstantInfoList& GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const Program::ConstantInfoList& GetConstants(void) const
			{
				return m_Constants;
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
			Program* m_LastProgramPtr;
			ProgramHandle* m_Program;
			Program::ConstantInfoList m_Constants;
			IDevice::State m_RenderState;
		};
	}
}

#endif