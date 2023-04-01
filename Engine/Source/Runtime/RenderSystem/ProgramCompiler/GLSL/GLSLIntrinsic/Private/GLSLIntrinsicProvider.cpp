// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GLSLIntrinsic\Private\GLSLIntrinsicProvider.h>
#include <ProgramCompilerCommon\Common.h>
#include <ASTCompiler\ASTCompilerBase.h>

namespace Engine
{
	using namespace ASTCompiler;

	namespace GLSLIntrinsic
	{
		namespace Private
		{
			void GLSLIntrinsicProvider::Initialize(IInitializeHelper* Helper)
			{
				BEGIN_FUNCTION("GetDimentions", _countof(Constants::UNSIGNED_DATA_TYPES) * _countof(Constants::READABLE_TEXTURE_TYPES));
				{
					for (ProgramDataTypes returnType : Constants::UNSIGNED_DATA_TYPES)
						for (uint8 i = 0; i < _countof(Constants::READABLE_TEXTURE_TYPES); ++i)
						{
							BEGIN_OVERRIDE(returnType);
							{
								ADD_PARAMETER(Constants::READABLE_TEXTURE_TYPES[i]);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += "imageSize(";
								Shader += textureVarialeName;
								Shader += ")";
									});
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Sample", _countof(Constants::NUMERIC_AND_VECTOR_DATA_TYPES) * _countof(Constants::SAMPLABLE_TEXTURE_TYPES));
				{
					for (ProgramDataTypes returnType : Constants::NUMERIC_AND_VECTOR_DATA_TYPES)
						for (uint8 i = 0; i < _countof(Constants::SAMPLABLE_TEXTURE_TYPES); ++i)
						{
							BEGIN_OVERRIDE(returnType);
							{
								ADD_PARAMETER(Constants::SAMPLABLE_TEXTURE_TYPES[i]);
								ADD_PARAMETER(Constants::SAMPLABLE_TEXTURE_UV_TYPES[i]);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += "texture(";
								Shader += ASTCompiler::ASTCompilerBase::GetSamplerVariableName(textureVarialeName);
								Shader += ", ";
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
									});
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Load", _countof(Constants::NUMERIC_AND_VECTOR_DATA_TYPES) * _countof(Constants::READABLE_TEXTURE_TYPES));
				{
					for (ProgramDataTypes returnType : Constants::NUMERIC_AND_VECTOR_DATA_TYPES)
						for (uint8 i = 0; i < _countof(Constants::READABLE_TEXTURE_TYPES); ++i)
						{
							BEGIN_OVERRIDE(returnType);
							{
								ADD_PARAMETER(Constants::READABLE_TEXTURE_TYPES[i]);
								ADD_PARAMETER(Constants::READABLE_TEXTURE_UV_TYPES[i]);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += "imageLoad(";
								Shader += textureVarialeName;
								Shader += ", ";
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
									});
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Store", _countof(Constants::NUMERIC_AND_VECTOR_DATA_TYPES) * _countof(Constants::WRITEABLE_TEXTURE_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_AND_VECTOR_DATA_TYPES)
						for (uint8 i = 0; i < _countof(Constants::WRITEABLE_TEXTURE_TYPES); ++i)
						{
							BEGIN_OVERRIDE(ProgramDataTypes::Void);
							{
								ADD_PARAMETER(Constants::WRITEABLE_TEXTURE_TYPES[i]);
								ADD_PARAMETER(Constants::WRITEABLE_TEXTURE_UV_TYPES[i]);
								ADD_PARAMETER(dataType);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += "imageStore(";
								Shader += textureVarialeName;
								Shader += ", ";
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ", ";
								Compiler->BuildStatement(Arguments[2], Type, Stage, Shader);
								Shader += ')';
									});
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Normalize", _countof(Constants::FLOATING_POINT_VECTOR_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::FLOATING_POINT_VECTOR_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("normalize");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Min", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("min");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Max", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("max");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Multiply", _countof(Constants::MATRICES_DATA_TYPES) * _countof(Constants::NUMERIC_VECTOR_AND_MATRICES_DATA_TYPES));
				{
					for (ProgramDataTypes firstDataType : Constants::MATRICES_DATA_TYPES)
						for (ProgramDataTypes secondDataType : Constants::NUMERIC_VECTOR_AND_MATRICES_DATA_TYPES)
						{
							BEGIN_OVERRIDE(secondDataType);
							{
								ADD_PARAMETER(firstDataType);
								ADD_PARAMETER(secondDataType);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
									});
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Reminder", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("mod");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Dot", _countof(Constants::VECTOR_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::VECTOR_DATA_TYPES)
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("dot");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Reflect", _countof(Constants::VECTOR_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::VECTOR_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("reflect");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Refract", _countof(Constants::VECTOR_DATA_TYPES) * _countof(Constants::FLOATING_POINT_VECTOR_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::VECTOR_DATA_TYPES)
						for (ProgramDataTypes biasDataType : Constants::FLOATING_POINT_VECTOR_TYPES)
						{
							BEGIN_OVERRIDE(dataType);
							{
								ADD_PARAMETER(dataType);
								ADD_PARAMETER(dataType);
								ADD_PARAMETER(biasDataType);
								SET_NATIVE_DESCRIPTION("refract");
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Length", _countof(Constants::VECTOR_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::VECTOR_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("length");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Distance", _countof(Constants::VECTOR_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::VECTOR_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("distance");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Transpose", _countof(Constants::MATRICES_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::MATRICES_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("transpose");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Sin", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("sin");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Cos", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("cos");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Tan", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("tan");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("ASos", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("asin");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("ACos", _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("acos");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Pow", _countof(Constants::NUMERIC_AND_VECTOR_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_AND_VECTOR_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("pow");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Clamp", _countof(Constants::NUMERIC_AND_VECTOR_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_AND_VECTOR_DATA_TYPES)
					{
						BEGIN_OVERRIDE(dataType);
						{
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							ADD_PARAMETER(dataType);
							SET_NATIVE_DESCRIPTION("clamp");
						}
						END_OVERRIDE();
					}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("AppendToStream", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Void);
					{
						MARK_AS_TEMPLATE();
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += "EmitVertex();\n";
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("RestartStrip", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Void);
					{
						MARK_AS_TEMPLATE();
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += "EndPrimitive();\n";
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();
			}
		}
	}
}