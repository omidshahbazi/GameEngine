// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\StructType.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				void BuildOpenGLVertexShader(ShaderParser::StructTypeList StructList, ShaderParser::FunctionTypeList FunctionList, String &VertextShader)
				{
					VertextShader += "#version 330 core";

					for each (auto st in StructList)
						VertextShader += "\n" + st->ToString();

					for each (auto fn in FunctionList)
					{
						if (fn->GetType() == FunctionType::Types::FragmentMain)
							continue;

						VertextShader += "\n" + fn->ToString();
					}

					VertextShader = VertextShader.Replace(VERTEX_MAIN, "main");
				}

				void BuildVertexShader(DeviceInterfarce::Type DeviceType, ShaderParser::StructTypeList StructList, ShaderParser::FunctionTypeList FunctionList, String &VertextShader)
				{
					switch (DeviceType)
					{
					case DeviceInterfarce::Type::OpenGL:
						BuildOpenGLVertexShader(StructList, FunctionList, VertextShader);
						break;
					}
				}

				bool Compiler::Compile(DeviceInterfarce::Type DeviceType, const String &Shader, String &VertexShader, String &FragmentShader)
				{
					ShaderParser parser(Shader);

					ShaderParser::StructTypeList structs;
					ShaderParser::FunctionTypeList functions;
					parser.Parse(structs, functions);

					BuildVertexShader(DeviceType, structs, functions, VertexShader);

					return true;
				}
			}
		}
	}
}