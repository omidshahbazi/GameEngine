// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Pass.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		using namespace Private;

		Pass::Pass(ShaderHandle* Shader) :
			m_Queue(RenderQueues::Default)
		{
			SetShader(Shader);
		}

		Pass::Pass(const Pass& Other)
		{
			*this = Other;
		}

		void Pass::SetRenderState(const IDevice::State& State)
		{
			PlatformMemory::Copy(&State, &m_RenderState, 1);
		}

		bool Pass::SetFloat32(const String& Name, float32 Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetColor(const String& Name, const ColorUI8& Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetVector2(const String& Name, const Vector2F& Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetVector3(const String& Name, const Vector3F& Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetVector4(const String& Name, const Vector4F& Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetMatrix4(const String& Name, const Matrix4F& Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetTexture(const String& Name, const TextureHandle* Value)
		{
			return SetConstantValue(Name, ReinterpretCast(void*, ConstCast(TextureHandle*, Value)));
		}

		bool Pass::SetSprite(const String& Name, const SpriteHandle* Value)
		{
			return SetConstantValue(Name, ReinterpretCast(void*, ConstCast(SpriteHandle*, Value)));
		}

		void Pass::SetShader(ShaderHandle* Shader)
		{
			m_Shader = Shader;
		}

		bool Pass::SetConstantValue(const String& Name, const AnyDataType& Value)
		{
			for (uint32 i = 0; i < m_Constants.GetSize(); ++i)
			{
				auto& constant = m_Constants[i];

				if (constant.Name != Name)
					continue;

				constant.Value = Value;
				return true;
			}

			m_Constants.Add({ Name, Value });

			return true;
		}
	}
}