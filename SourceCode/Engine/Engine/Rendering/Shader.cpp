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

		Shader::~Shader(void)
		{
		}

		bool Shader::SetFloat32(ConstantHandle Handle, float32 Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetColor(ConstantHandle Handle, const ColorUI8& Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetVector2(ConstantHandle Handle, const Vector2F& Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetVector3(ConstantHandle Handle, const Vector3F& Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetVector4(ConstantHandle Handle, const Vector4F& Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetMatrix4(ConstantHandle Handle, const Matrix4F& Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetTexture(ConstantHandle Handle, const Texture* Value)
		{
			return SetConstantValue(Handle, Value);
		}

		bool Shader::SetFloat32(const String& Name, float32 Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetFloat32(data->Handle, Value);
		}

		bool Shader::SetColor(const String& Name, const ColorUI8& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetColor(data->Handle, Value);
		}

		bool Shader::SetVector2(const String& Name, const Vector2F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector2(data->Handle, Value);
		}

		bool Shader::SetVector3(const String& Name, const Vector3F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector3(data->Handle, Value);
		}

		bool Shader::SetVector4(const String& Name, const Vector4F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector4(data->Handle, Value);
		}

		bool Shader::SetMatrix4(const String& Name, const Matrix4F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetMatrix4(data->Handle, Value);
		}

		bool Shader::SetTexture(const String& Name, const Texture* Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetTexture(data->Handle, Value);
		}

		void Shader::SetConstantsValue(const ConstantInfoList& Constants)
		{
			for each (auto & info in Constants)
			{
				auto data = GetConstantData(info.Name);

				if (data == nullptr)
					continue;

				data->Value = info.Value;
			}
		}

		void Shader::ApplyConstantsValue(IDevice* Device)
		{
			for each (auto & info in m_ConstantsData)
				SetConstantValue(Device, info.Handle, info.Type, info.Value);
		}

		Shader::ConstantHandle Shader::GetConstantHandle(const String& Name)
		{
			auto constant = GetConstantData(Name);
			if (constant == nullptr)
				return 0;

			return constant->Handle;
		}

		Shader::ConstantData* Shader::GetConstantData(const String& Name)
		{
			int32 index = m_ConstantsData.Find([&](auto& Item) { return (Item.Name == Name); });
			if (index == -1)
				return nullptr;

			return ConstCast(ConstantData*, &m_ConstantsData[index]);
		}

		bool Shader::SetConstantValue(Shader::ConstantHandle Handle, const AnyDataType& Value)
		{
			int32 index = m_ConstantsData.Find([&](auto& Item) { return (Item.Handle == Handle); });
			if (index == -1)
				return false;

			m_ConstantsData[index].Value = Value;

			return true;
		}

		bool Shader::SetConstantValue(IDevice* Device, Shader::ConstantHandle Handle, ShaderDataType::Types Type, const AnyDataType& Value)
		{
			switch (Type)
			{
			case ShaderDataType::Types::Float: Device->SetShaderFloat32(Handle, Value.Get<float32>()); break;
			case ShaderDataType::Types::Float2: Device->SetShaderVector2(Handle, Value.Get<Vector2F>()); break;
			case ShaderDataType::Types::Float3: Device->SetShaderVector3(Handle, Value.Get<Vector3F>()); break;
			case ShaderDataType::Types::Float4:
			{
				if (Value.GetValueType() == ValueTypes::ColorUI8)
					Device->SetShaderColor(Handle, Value.GetAsColorUI8());
				else
					Device->SetShaderVector4(Handle, Value.Get<Vector4F>()); break;
			}
			case ShaderDataType::Types::Matrix4: Device->SetShaderMatrix4(Handle, Value.Get<Matrix4F>()); break;
			case ShaderDataType::Types::Texture2D:
			{
				auto val = Value.Get<TextureResource*>();
				Texture::Types type = Texture::Types::TwoD;
				Texture::Handle texHandle = 0;
				if (val != nullptr)
				{
					Texture* tex = val->GetPointer();
					if (tex != nullptr)
					{
						texHandle = tex->GetHandle();
						type = tex->GetType();
					}
				}

				Device->SetShaderTexture(Handle, type, texHandle);
			} break;

			default:
				return false;
			}

			return true;
		}

		void Shader::QueryActiveConstants(void)
		{
			GetDevice()->Lock();
			Promise<bool> promise = GetDevice()->QueryShaderActiveConstants(GetHandle(), m_ConstantsData);
			GetDevice()->Release();

			promise.Wait();
		}
	}
}