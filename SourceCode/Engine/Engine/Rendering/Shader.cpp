// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Shader.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		Shader::Shader(ThreadedDevice* Device, Handle Handle) :
			NativeType(Device, Handle)
		{
			QueryActiveConstants();
		}

		void Shader::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Shader, GetName().GetValue());
		}

		bool Shader::SetFloat32(ConstantHash Hash, float32 Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Shader::SetColor(ConstantHash Hash, const ColorUI8& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Shader::SetVector2(ConstantHash Hash, const Vector2F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Shader::SetVector3(ConstantHash Hash, const Vector3F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Shader::SetVector4(ConstantHash Hash, const Vector4F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Shader::SetMatrix4(ConstantHash Hash, const Matrix4F& Value)
		{
			return SetConstantValue(Hash, Value);
		}

		bool Shader::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			return SetConstantValue(Hash, ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Shader::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			return SetConstantValue(Hash, ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		bool Shader::SetFloat32(const String& Name, float32 Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Shader::SetColor(const String& Name, const ColorUI8& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Shader::SetVector2(const String& Name, const Vector2F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Shader::SetVector3(const String& Name, const Vector3F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Shader::SetVector4(const String& Name, const Vector4F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Shader::SetMatrix4(const String& Name, const Matrix4F& Value)
		{
			return SetConstantValue(GetHash(Name), Value);
		}

		bool Shader::SetTexture(const String& Name, const TextureResource* Value)
		{
			return SetConstantValue(GetHash(Name), ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Shader::SetSprite(const String& Name, const SpriteResource* Value)
		{
			return SetConstantValue(GetHash(Name), ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		Shader::ConstantHash Shader::GetConstantHash(const String& Name)
		{
			ConstantHash hash = GetHash(Name);

			if (m_ConstantsData.Contains(hash))
				return hash;

			return 0;
		}

		void Shader::SetConstantsValue(const ConstantInfoMap& Constants)
		{
			for (auto& info : Constants)
			{
				auto& constant = info.GetSecond();

				if (!m_ConstantsData.Contains(constant.Hash))
					continue;

				m_ConstantsData[constant.Hash].Value = constant.Value;
			}
		}

		void Shader::ApplyConstantsValue(IDevice* Device)
		{
			for (auto& item : m_ConstantsData)
			{
				auto& constant = item.GetSecond();

				SetConstantValueOnDevice(Device, constant.Handle, constant.Type, constant.Value);
			}
		}

		bool Shader::SetConstantValue(Shader::ConstantHash Hash, const AnyDataType& Value)
		{
			if (!m_ConstantsData.Contains(Hash))
				return false;

			m_ConstantsData[Hash].Value = Value;

			return true;
		}

		void Shader::QueryActiveConstants(void)
		{
			ConstantDataList list;
			GetDevice()->QueryShaderActiveConstants(GetHandle(), list).Wait();

			for (auto& constant : list)
				m_ConstantsData[constant.Hash] = constant;
		}

		bool Shader::SetConstantValueOnDevice(IDevice* Device, Shader::ConstantHandle Handle, ShaderDataType::Types Type, const AnyDataType& Value)
		{
			switch (Type)
			{
			case ShaderDataType::Types::Float:
			{
				Device->SetShaderFloat32(Handle, Value.Get<float32>());
			} break;

			case ShaderDataType::Types::Float2:
			{
				Device->SetShaderVector2(Handle, Value.Get<Vector2F>());
			} break;

			case ShaderDataType::Types::Float3:
			{
				Device->SetShaderVector3(Handle, Value.Get<Vector3F>());
			} break;

			case ShaderDataType::Types::Float4:
			{
				if (Value.GetValueType() == ValueTypes::ColorUI8)
					Device->SetShaderColor(Handle, Value.GetAsColorUI8());
				else
					Device->SetShaderVector4(Handle, Value.Get<Vector4F>());
			} break;

			case ShaderDataType::Types::Matrix4:
			{
				Device->SetShaderMatrix4(Handle, Value.Get<Matrix4F>());
			} break;

			case ShaderDataType::Types::Texture2D:
			{
				auto val = Value.Get<TextureResource*>();
				Texture::Types type = Texture::Types::TwoD;
				Texture::Handle texHandle = 0;
				if (val != nullptr && !val->IsNull())
				{
					Texture* tex = val->GetPointer();

					texHandle = tex->GetHandle();
					type = tex->GetType();
				}

				Device->SetShaderTexture(Handle, type, texHandle);
			} break;

			default:
				return false;
			}

			return true;
		}
	}
}