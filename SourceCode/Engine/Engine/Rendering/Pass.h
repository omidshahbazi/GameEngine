// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <Rendering\IDevice.h>
#include <Rendering\Sprite.h>
#include <Rendering\RenderingCommon.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		class Texture;

		class RENDERING_API Pass
		{
		public:
			Pass(void);
			Pass(ShaderResource* Shader);
			Pass(const Pass& Other);

			~Pass(void)
			{
				Destruct(&m_Constants);
			}

			bool SetFloat32(const String& Name, float32 Value);
			bool SetColor(const String& Name, const ColorUI8& Value);
			bool SetVector2(const String& Name, const Vector2F& Value);
			bool SetVector3(const String& Name, const Vector3F& Value);
			bool SetVector4(const String& Name, const Vector4F& Value);
			bool SetMatrix4(const String& Name, const Matrix4F& Value);
			bool SetTexture(const String& Name, const TextureResource* Value);
			bool SetSprite(const String& Name, const SpriteResource* Value);

			ShaderResource* GetShader(void)
			{
				return m_Shader;
			}

			void SetShader(ShaderResource* Shader);

			INLINE Shader::ConstantInfoList& GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const Shader::ConstantInfoList& GetConstants(void) const
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
				m_Shader = Other.m_Shader;
				m_Queue = Other.m_Queue;
				m_Constants = Other.m_Constants;
				m_RenderState = Other.m_RenderState;

				return *this;
			}

		private:
			bool SetConstantValue(const String& Name, const AnyDataType& Value);

		private:
			ShaderResource* m_Shader;
			RenderQueues m_Queue;
			Shader::ConstantInfoList m_Constants;
			IDevice::State m_RenderState;
		};
	}
}

#endif