// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		const Program::ConstantInfo* FindConstantData(const Program::ConstantInfoList& InfoList, const String& Name)
		{
			for each (auto & constant in InfoList)
				if (constant.Name == Name)
					return &constant;

			return nullptr;
		}

		Program::Program(IDevice* Device, Handle Handle) :
			NativeType(Device, Handle)
		{
			QueryActiveConstants();
		}

		bool Program::SetFloat32(Program::ConstantHandle Handle, float32 Value)
		{
			return GetDevice()->SetProgramFloat32(Handle, Value);
		}

		bool Program::SetVector2(Program::ConstantHandle Handle, const Vector2F& Value)
		{
			return GetDevice()->SetProgramVector2(Handle, Value);
		}

		bool Program::SetVector3(Program::ConstantHandle Handle, const Vector3F& Value)
		{
			return GetDevice()->SetProgramVector3(Handle, Value);
		}

		bool Program::SetVector4(Program::ConstantHandle Handle, const Vector4F& Value)
		{
			return GetDevice()->SetProgramVector4(Handle, Value);
		}

		bool Program::SetMatrix4(Program::ConstantHandle Handle, const Matrix4F& Value)
		{
			return GetDevice()->SetProgramMatrix4(Handle, Value);
		}

		bool Program::SetTexture(Program::ConstantHandle Handle, const Texture* Value)
		{
			uint32 handle = (Value == nullptr ? 0 : Value->GetHandle());
			Texture::Types type = (Value == nullptr ? Texture::Types::TwoD : Value->GetType());

			return GetDevice()->SetProgramTexture(Handle, type, handle);
		}

		bool Program::SetFloat32(const String& Name, float32 Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetFloat32(data->Handle, Value);
		}

		bool Program::SetVector2(const String& Name, const Vector2F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector2(data->Handle, Value);
		}

		bool Program::SetVector3(const String& Name, const Vector3F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector3(data->Handle, Value);
		}

		bool Program::SetVector4(const String& Name, const Vector4F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector4(data->Handle, Value);
		}

		bool Program::SetMatrix4(const String& Name, const Matrix4F& Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetMatrix4(data->Handle, Value);
		}

		bool Program::SetTexture(const String& Name, const Texture* Value)
		{
			ConstantData* data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetTexture(data->Handle, Value);
		}

		void Program::ApplyConstantValue(const ConstantInfoList& Constants)
		{
			for each (auto & info in Constants)
			{
				auto data = GetConstantData(info.Name);

				if (data == nullptr)
					continue;

				switch (data->Type)
				{
				case DataType::Types::Float: SetFloat32(info.Name, info.Value.Get<float32>()); break;
				case DataType::Types::Float2: SetVector2(info.Name, info.Value.Get<Vector2F>()); break;
				case DataType::Types::Float3: SetVector3(info.Name, info.Value.Get<Vector3F>()); break;
				case DataType::Types::Float4: SetVector4(info.Name, info.Value.Get<Vector4F>()); break;
				case DataType::Types::Matrix4: SetMatrix4(info.Name, info.Value.Get<Matrix4F>()); break;
				case DataType::Types::Texture2D:
				{
					auto val = info.Value.Get<TextureHandle*>();
					SetTexture(info.Name, (val == nullptr ? nullptr : val->GetData()));
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
				case DataType::Types::Texture2D:
				{
					auto val = data.Value.Get<TextureHandle*>();
					SetTexture(data.Name, (val == nullptr ? nullptr : val->GetData()));
				} break;
				}
			}
		}

		Program::ConstantData* Program::GetConstantData(const String& Name)
		{
			for each (auto & constant in m_Constants)
				if (constant.Name == Name)
					return ConstCast(Program::ConstantData*, &constant);

			return nullptr;
		}

		void Program::QueryActiveConstants(void)
		{
			GetDevice()->QueryProgramActiveConstants(GetHandle(), m_Constants);
		}
	}
}