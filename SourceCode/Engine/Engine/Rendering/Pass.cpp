// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Pass.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		using namespace Private;

		Pass::Pass(void) :
			m_Shader(nullptr),
			m_Queue(RenderQueues::Default)
		{
		}

		Pass::Pass(ShaderResource* Shader) :
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

		Pass::ConstantHash Pass::GetConstantHash(const String& Name)
		{
			ConstantHash hash = GetHash(Name);

			if (m_ConstantsInfo.Contains(hash))
				return hash;

			return 0;
		}

		bool Pass::SetFloat32(ConstantHash Hash, float32 Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Pass::SetColor(ConstantHash Hash, const ColorUI8& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Pass::SetVector2(ConstantHash Hash, const Vector2F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Pass::SetVector3(ConstantHash Hash, const Vector3F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Pass::SetVector4(ConstantHash Hash, const Vector4F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Pass::SetMatrix4(ConstantHash Hash, const Matrix4F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Pass::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			return SetConstantValue(Hash, ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Pass::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			return SetConstantValue(Hash, ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		bool Pass::SetFloat32(const String& Name, float32 Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Pass::SetColor(const String& Name, const ColorUI8& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Pass::SetVector2(const String& Name, const Vector2F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Pass::SetVector3(const String& Name, const Vector3F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Pass::SetVector4(const String& Name, const Vector4F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Pass::SetMatrix4(const String& Name, const Matrix4F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Pass::SetTexture(const String& Name, const TextureResource* Value)
		{
			return SetConstantValue(GetHash(Name), ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Pass::SetSprite(const String& Name, const SpriteResource* Value)
		{
			return SetConstantValue(GetHash(Name), ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		void Pass::SetShader(ShaderResource* Shader)
		{
			if (m_Shader == Shader)
				return;

			m_Shader = Shader;

			m_ConstantsInfo.Clear();
		}

		bool Pass::SetConstantValue(Shader::ConstantHash Hash, const AnyDataType& Value)
		{
			ConstantInfo info;
			info.Hash = Hash;
			info.Value = Value;

			m_ConstantsInfo[Hash] = info;

			return true;
		}
	}
}