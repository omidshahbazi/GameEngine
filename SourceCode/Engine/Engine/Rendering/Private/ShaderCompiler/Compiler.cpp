// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				bool Compiler::Compile(DeviceInterfarce::Type DeviceType, const String &Shader, String &Result)
				{
					ShaderParser parser(Shader);

					parser.Parse();

					return true;
				}
			}
		}
	}
}