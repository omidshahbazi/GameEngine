// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\ProgramCompiler\IntrinsicFunctions.h>
#include <Rendering\Private\ProgramCompiler\ProgramParser.h>
#include <Rendering\Private\ProgramCompiler\ProgramParserPreprocess.h>
#include <Rendering\Private\ProgramCompiler\OpenGLCompiler.h>
#include <Rendering\Private\ProgramCompiler\DirectXCompiler.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				SINGLETON_DEFINITION(Compiler);

				cstr Compiler::ENTRY_POINT_NAME = "main";

				Compiler::Compiler(void)
				{
				}

				bool Compiler::Compile(const ProgramInfo* Info, DeviceTypes DeviceType, CompileOutputInfo& Output, ErrorFunction OnError)
				{
					return Compile(Info, &DeviceType, 1, &Output, OnError);
				}

				bool Compiler::Compile(const ProgramInfo* Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompileOutputInfo* Outputs, ErrorFunction OnError)
				{
					if (Info->Source.GetLength() == 0)
						return false;

					ProgramParserPreprocess parserPreprocessor(Info->Source);
					ProgramParserPreprocess::Parameters preprocessParameters;
					preprocessParameters.IncludeFunction = [&](const String& Name, String& Source)
					{
						for (auto listener : m_IListener_List)
						{
							if (listener->FetchShaderSource(Name, Source))
								return true;
						}

						return false;
					};
					preprocessParameters.Defines = Info->Defines;
					if (!parserPreprocessor.Process(preprocessParameters))
						return false;

					FrameAllocator alloc("Program Statements Allocator", RenderingAllocators::ProgramCompilerAllocator);
					ProgramParser parser(&alloc, preprocessParameters.Result, OnError);
					ProgramParser::Parameters parameters;
					if (!parser.Parse(parameters))
						return false;

					for (uint8 i = 0; i < DeviceTypeCount; ++i)
					{
						CompileOutputInfo& output = Outputs[i];

						switch (DeviceTypes[i])
						{
						case DeviceTypes::OpenGL:
						case DeviceTypes::Vulkan:
						{
							static OpenGLCompiler openGL;
							output.Result = openGL.Compile(parameters.Structs, parameters.Variables, parameters.Functions, output);
						} break;

						case DeviceTypes::DirectX12:
						{
							static DirectXCompiler directX;
							output.Result = directX.Compile(parameters.Structs, parameters.Variables, parameters.Functions, output);
						} break;
						}

						if (output.Result)
						{
							for (auto& structType : parameters.Structs)
							{
								auto variables = structType->GetItems();
								variables.RemoveIf([](auto item) { return item->GetRegister().GetLength() != 0; });

								if (variables.GetSize() == 0)
									continue;

								output.MetaInfo.Structs.Add({});
								StructMetaInfo& structMeta = output.MetaInfo.Structs[output.MetaInfo.Structs.GetSize() - 1];

								structMeta.Name = structType->GetName();

								for (auto& variableType : variables)
								{
									ProgramDataTypes dataType = variableType->GetDataType().GetType();

									structMeta.Variables.Add({ dataType, variableType->GetName() });
								}

								structMeta.Size = APICompiler::GetStructSize(structType);
							}

							uint8 bindingCount = 0;
							for (auto& variableType : parameters.Variables)
							{
								output.MetaInfo.Variables.Add({});
								VariableMetaInfo& variableMeta = output.MetaInfo.Variables[output.MetaInfo.Variables.GetSize() - 1];

								const DataType& type = variableType->GetDataType();

								variableMeta.Handle = bindingCount++;
								variableMeta.Name = variableType->GetName();
								variableMeta.DataType = type.GetType();
								variableMeta.UserDefinedType = type.GetUserDefined();
							}
						}
					}

					for (auto structType : parameters.Structs)
						Destruct(structType);

					for (auto variableType : parameters.Variables)
						Destruct(variableType);

					for (auto function : parameters.Functions)
						Destruct(function);

					return true;
				}
			}
		}
	}
}