// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		Program::Program(IDevice *Device, Handle Handle) :
			NativeType(Device, Handle)
		{
			QueryActiveConstants();
		}

		bool Program::SetFloat32(Program::ConstantHandle Handle, float32 Value)
		{
			return GetDevice()->SetProgramFloat32(Handle, Value);
		}

		bool Program::SetVector2(Program::ConstantHandle Handle, const Vector2F & Value)
		{
			return GetDevice()->SetProgramVector2(Handle, Value);
		}

		bool Program::SetVector3(Program::ConstantHandle Handle, const Vector3F & Value)
		{
			return GetDevice()->SetProgramVector3(Handle, Value);
		}

		bool Program::SetVector4(Program::ConstantHandle Handle, const Vector4F & Value)
		{
			return GetDevice()->SetProgramVector4(Handle, Value);
		}

		bool Program::SetMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value)
		{
			return GetDevice()->SetProgramMatrix4(Handle, Value);
		}

		bool Program::SetTexture(Program::ConstantHandle Handle, const Texture * Value)
		{
			if (Value == nullptr)
				return false;

			return GetDevice()->SetProgramTexture(Handle, Value->GetHandle());
		}

		bool Program::SetFloat32(const String &Name, float32 Value)
		{
			ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetFloat32(data->Handle, Value);
		}

		bool Program::SetVector2(const String & Name, const Vector2F & Value)
		{
			ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector2(data->Handle, Value);
		}

		bool Program::SetVector3(const String & Name, const Vector3F & Value)
		{
			ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector3(data->Handle, Value);
		}

		bool Program::SetVector4(const String & Name, const Vector4F & Value)
		{
			ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetVector4(data->Handle, Value);
		}

		bool Program::SetMatrix4(const String &Name, const Matrix4F &Value)
		{
			ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetMatrix4(data->Handle, Value);
		}

		bool Program::SetTexture(const String & Name, const Texture * Value)
		{
			ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			return SetTexture(data->Handle, Value);
		}

		void Program::ApplyConstantValue(const ConstantDataList & DataList)
		{
			for each (auto &constant in DataList)
			{
				switch (constant.Type)
				{
				case DataType::Types::Float: SetFloat32(constant.Handle, constant.Value.Get<float32>()); break;
				case DataType::Types::Float2: SetVector2(constant.Handle, constant.Value.Get<Vector2F>()); break;
				case DataType::Types::Float3: SetVector3(constant.Handle, constant.Value.Get<Vector3F>()); break;
				case DataType::Types::Float4: SetVector4(constant.Handle, constant.Value.Get<Vector4F>()); break;
				case DataType::Types::Matrix4: SetMatrix4(constant.Handle, constant.Value.Get<Matrix4F>()); break;
				case DataType::Types::Texture2D: SetTexture(constant.Handle, constant.Value.Get<TextureHandle*>()->GetData()); break;
				}
			}
		}

		Program::ConstantData *Program::GetConstantData(const String & Name)
		{
			for each (auto &constant in m_Constants)
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