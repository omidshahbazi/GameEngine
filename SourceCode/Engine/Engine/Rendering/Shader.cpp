// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Shader.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		const Shader::ConstantInfo* FindConstantData(const Shader::ConstantInfoList& InfoList, const String& Name)
		{
			for each (auto & constant in InfoList)
				if (constant.Name == Name)
					return &constant;

			return nullptr;
		}

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
			GetDevice()->SetShaderFloat32(Handle, Value);

			return true;
		}

		bool Shader::SetColor(ConstantHandle Handle, const ColorUI8& Value)
		{
			GetDevice()->SetShaderColor(Handle, Value);

			return true;
		}

		bool Shader::SetVector2(ConstantHandle Handle, const Vector2F& Value)
		{
			GetDevice()->SetShaderVector2(Handle, Value);

			return true;
		}

		bool Shader::SetVector3(ConstantHandle Handle, const Vector3F& Value)
		{
			GetDevice()->SetShaderVector3(Handle, Value);

			return true;
		}

		bool Shader::SetVector4(ConstantHandle Handle, const Vector4F& Value)
		{
			GetDevice()->SetShaderVector4(Handle, Value);

			return true;
		}

		bool Shader::SetMatrix4(ConstantHandle Handle, const Matrix4F& Value)
		{
			GetDevice()->SetShaderMatrix4(Handle, Value);

			return true;
		}

		bool Shader::SetTexture(ConstantHandle Handle, const Texture* Value)
		{
			uint32 handle = (Value == nullptr ? 0 : Value->GetHandle());
			Texture::Types type = (Value == nullptr ? Texture::Types::TwoD : Value->GetType());

			GetDevice()->SetShaderTexture(Handle, type, handle);

			return true;
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

		void Shader::ApplyConstantValue(const ConstantInfoList& Constants)
		{
			for each (auto & info in Constants)
			{
				auto data = GetConstantData(info.Name);

				if (data == nullptr)
					continue;

				switch (data->Type)
				{
				case ShaderDataType::Types::Float: SetFloat32(data->Handle, info.Value.Get<float32>()); break;
				case ShaderDataType::Types::Float2: SetVector2(data->Handle, info.Value.Get<Vector2F>()); break;
				case ShaderDataType::Types::Float3: SetVector3(data->Handle, info.Value.Get<Vector3F>()); break;
				case ShaderDataType::Types::Float4:
				{
					const auto& value = info.Value;

					if (value.GetValueType() == ValueTypes::ColorUI8)
						SetColor(data->Handle, value.GetAsColorUI8());
					else
						SetVector4(data->Handle, info.Value.Get<Vector4F>()); break;
				}
				case ShaderDataType::Types::Matrix4: SetMatrix4(info.Name, info.Value.Get<Matrix4F>()); break;
				case ShaderDataType::Types::Texture2D:
				{
					auto val = info.Value.Get<TextureResource*>();
					SetTexture(data->Handle, (val == nullptr ? nullptr : val->GetPointer()));
				} break;
				}
			}

			for each (auto & data in m_Constants)
			{
				const ConstantInfo* info = FindConstantData(Constants, data.Name);

				if (info != nullptr)
					continue;

				switch (data.Type)
				{
				case ShaderDataType::Types::Texture2D:
				{
					auto val = data.Value.Get<TextureResource*>();
					SetTexture(data.Handle, (val == nullptr ? nullptr : val->GetPointer()));
				} break;
				}
			}
		}

		Shader::ConstantData* Shader::GetConstantData(const String& Name)
		{
			for each (auto & constant in m_Constants)
				if (constant.Name == Name)
					return ConstCast(ConstantData*, &constant);

			return nullptr;
		}

		void Shader::QueryActiveConstants(void)
		{
			GetDevice()->Lock();
			Promise<bool> promise = GetDevice()->QueryShaderActiveConstants(GetHandle(), m_Constants);
			GetDevice()->Release();

			promise.Wait();
		}
	}
}