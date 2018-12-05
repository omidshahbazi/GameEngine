// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		bool Program::SetFloat32(Program::ConstantHandle Handle, float32 Value)
		{
			return GetDevice()->SetProgramFloat32(Handle, Value);
		}

		bool Program::SetColor(Program::ConstantHandle Handle, Color Value)
		{
			return GetDevice()->SetProgramColor(Handle, Value);
		}

		//bool Program::SetMatrix4(Program::ConstantHandle Handle, const Matrix4 &Value) 
		//{
		//	return GetDevice()->SetProgramMatrix4(Handle, Value);
		//}

		bool Program::SetFloat32(const String &Name, float32 Value)
		{
			return GetDevice()->SetProgramFloat32(GetHandle(), Name, Value);
		}

		bool Program::SetColor(const String &Name, Color Value)
		{
			return GetDevice()->SetProgramColor(GetHandle(), Name, Value);
		}

		//bool Program::SetMatrix4(const String &Name, const Matrix4 &Value)
		//{
		//	return GetDevice()->SetProgramMatrix4(GetHandle(), Name, Value);
		//}
	}
}