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

		bool Program::SetVector3(Program::ConstantHandle Handle, const Vector3F & Value)
		{
			return GetDevice()->SetProgramVector3(Handle, Value);
		}

		bool Program::SetColor(Program::ConstantHandle Handle, Color Value)
		{
			return GetDevice()->SetProgramColor(Handle, Value);
		}

		bool Program::SetMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value)
		{
			return GetDevice()->SetProgramMatrix4(Handle, Value);
		}

		bool Program::SetTexture(Program::ConstantHandle Handle, const Texture * Value)
		{
			return GetDevice()->SetProgramTexture(Handle, Value->GetHandle());
		}

		bool Program::SetFloat32(const String &Name, float32 Value)
		{
			return GetDevice()->SetProgramFloat32(GetHandle(), Name, Value);
		}

		bool Program::SetVector3(const String & Name, const Vector3F & Value)
		{
			return GetDevice()->SetProgramVector3(GetHandle(), Name, Value);
		}

		bool Program::SetColor(const String &Name, Color Value)
		{
			return GetDevice()->SetProgramColor(GetHandle(), Name, Value);
		}

		bool Program::SetMatrix4(const String &Name, const Matrix4F &Value)
		{
			return GetDevice()->SetProgramMatrix4(GetHandle(), Name, Value);
		}

		bool Program::SetTexture(const String & Name, const Texture * Value)
		{
			return GetDevice()->SetProgramTexture(GetHandle(), Name, Value->GetHandle());
		}

		void Program::QueryActiveConstants(void)
		{
			GetDevice()->QueryProgramActiveConstants(GetHandle(), m_Constants);
		}
	}
}