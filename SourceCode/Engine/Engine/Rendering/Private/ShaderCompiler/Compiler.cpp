// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\StructType.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				String GetOpenGLTypeByDataType(DataTypes Type)
				{
					switch (Type)
					{
					case DataTypes::Float:
						return "float";

					case DataTypes::Float2:
						return "vec2";

					case DataTypes::Float3:
						return "vec3";

					case DataTypes::Float4:
						return "vec4";

					case DataTypes::Float4X4:
						return "mat4";
					}
					
					return "";
				}

				void BuildOpenGLVertexShader(ShaderParser::StructTypeList StructList, ShaderParser::FunctionTypeList FunctionList, String &VertextShader)
				{
					Map<ConstString, uint16> layouts;
					layouts["POSITION"] = 0;
					layouts["NORMAL"] = 1;
					layouts["TEXCOORD"] = 2;

					VertextShader += "#version 330 core";

					for each (auto st in StructList)
					{
						uint16 index = 0;
						for each (auto var in st->GetVariables())
						{
							if (var->GetRegister().GetLength() == 0)
								VertextShader += "uniform ";
							else
							{
								VertextShader += "layout(location=";
								VertextShader += StringUtility::ToString(index++) + ")";
							}

							VertextShader += GetOpenGLTypeByDataType(var->GetDataType());

							VertextShader += var->GetName();

							VertextShader += ";";
						}
					}

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