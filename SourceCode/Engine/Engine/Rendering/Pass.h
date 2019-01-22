// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
			Pass(Program *Program);

			bool SetFloat32(const String &Name, float32 Value);
			bool SetVector2(const String &Name, const Vector2F &Value);
			bool SetVector3(const String &Name, const Vector3F &Value);
			bool SetVector4(const String &Name, const Vector4F &Value);
			bool SetColor(const String &Name, Color Value);
			bool SetMatrix4(const String &Name, const Matrix4F &Value);
			bool SetTexture(const String &Name, const Texture *Value);

			INLINE Program *GetProgram(void) const
			{
				return m_Program;
			}

			void SetProgram(Program *Program);

			INLINE Program::ConstantDataList &GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const Program::ConstantDataList &GetConstants(void) const
			{
				return m_Constants;
			}

			const IDevice::State &GetRenderState(void) const
			{
				return m_RenderState;
			}
			void SetRenderState(const IDevice::State &State);

		private:
			Program::ConstantData *GetConstantData(const String &Name);

			bool SetConstantValue(const String &Name, const AnyDataType &Value);

		private:
			Program * m_Program;
			Program::ConstantDataList m_Constants;
			IDevice::State m_RenderState;
		};
	}
}

#endif