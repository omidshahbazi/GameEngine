// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
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

					case DataTypes::Matrix4:
						return "mat4";
					}

					return "";
				}

				void BuildOpenGLVertexShader(ShaderParser::VariableTypeList Variables, ShaderParser::FunctionTypeList Functions, String &VertextShader)
				{
					Map<String, uint16> layouts;
					layouts["POSITION"] = 0;
					layouts["NORMAL"] = 1;
					layouts["TEXCOORD"] = 2;

					VertextShader += "#version 330 core\n";

					for each (auto var in Variables)
					{
						if (var->GetRegister().GetLength() == 0)
							VertextShader += "uniform ";
						else
						{
							VertextShader += "layout(location=";
							VertextShader += StringUtility::ToString<char8>(layouts[var->GetRegister()]);
							VertextShader += ") ";
						}

						VertextShader += GetOpenGLTypeByDataType(var->GetDataType());
						VertextShader += " ";
						VertextShader += var->GetName();
						VertextShader += ";";
					}

					for each (auto fn in Functions)
					{
						if (fn->GetType() == FunctionType::Types::FragmentMain)
							continue;

						VertextShader += "\n" + fn->ToString();
					}

					VertextShader = VertextShader.Replace(VERTEX_MAIN, "main");
				}

				void BuildVertexShader(DeviceInterfarce::Type DeviceType, ShaderParser::VariableTypeList Variables, ShaderParser::FunctionTypeList Functions, String &VertextShader)
				{
					switch (DeviceType)
					{
					case DeviceInterfarce::Type::OpenGL:
						BuildOpenGLVertexShader(Variables, Functions, VertextShader);
						break;
					}
				}

				bool Compiler::Compile(DeviceInterfarce::Type DeviceType, const String &Shader, String &VertexShader, String &FragmentShader)
				{
					ShaderParser parser(Shader);

					ShaderParser::VariableTypeList variables;
					ShaderParser::FunctionTypeList functions;
					parser.Parse(variables, functions);

					BuildVertexShader(DeviceType, variables, functions, VertexShader);

					return true;
				}
			}
		}
	}
}