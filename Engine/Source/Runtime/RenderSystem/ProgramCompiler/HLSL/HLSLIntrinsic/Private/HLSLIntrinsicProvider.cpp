// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <HLSLIntrinsic\Private\HLSLIntrinsicProvider.h>
#include <ProgramCompilerCommon\Common.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>
#include <ASTToHLSLCompiler\Private\ASTToHLSLCompiler.h>
#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>

namespace Engine
{
	using namespace ASTCompiler;
	using namespace ProgramParser::AbstractSyntaxTree;

	namespace HLSLIntrinsic
	{
		namespace Private
		{
			void HLSLIntrinsicProvider::Initialize(IInitializeHelper* Helper)
			{
				BEGIN_FUNCTION("GetDimentions", _countof(Constants::UNSIGNED_DATA_TYPES) * _countof(Constants::TEXTURE_DATA_TYPES) * _countof(Constants::READABLE_TEXTURE_TYPES));
				{
					for (ProgramDataTypes returnType : Constants::UNSIGNED_DATA_TYPES)
						for (ProgramDataTypes textureDataType : Constants::TEXTURE_DATA_TYPES)
							for (uint8 i = 0; i < _countof(Constants::READABLE_TEXTURE_TYPES); ++i)
							{
								BEGIN_OVERRIDE(returnType);
								{
									ADD_TEMPLATE_PARAMETER(Constants::READABLE_TEXTURE_TYPES[i], textureDataType);
									SET_NATIVE_DESCRIPTION("GetDimentions");
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
								ADD_TEMPLATE_PARAMETER(Constants::SAMPLABLE_TEXTURE_TYPES[i], returnType);
								ADD_PARAMETER(Constants::SAMPLABLE_TEXTURE_UV_TYPES[i]);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += textureVarialeName;
								Shader += ".Sample(";
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
								ADD_TEMPLATE_PARAMETER(Constants::READABLE_TEXTURE_TYPES[i], returnType);
								ADD_PARAMETER(Constants::READABLE_TEXTURE_UV_TYPES[i]);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += textureVarialeName;
								Shader += ".Load(";
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
								ADD_TEMPLATE_PARAMETER(Constants::WRITEABLE_TEXTURE_TYPES[i], dataType);
								ADD_PARAMETER(Constants::WRITEABLE_TEXTURE_UV_TYPES[i]);
								ADD_PARAMETER(dataType);
								SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
									{
										String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += textureVarialeName;
								Shader += '[';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += "] = ";
								Compiler->BuildStatement(Arguments[2], Type, Stage, Shader);
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

				BEGIN_FUNCTION("Min", _countof(Constants::NUMERIC_DATA_TYPES) * _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType1 : Constants::NUMERIC_DATA_TYPES)
						for (ProgramDataTypes dataType2 : Constants::NUMERIC_DATA_TYPES)
						{
							BEGIN_OVERRIDE(dataType1);
							{
								ADD_PARAMETER(dataType1);
								ADD_PARAMETER(dataType2);
								SET_NATIVE_DESCRIPTION("min");
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Max", _countof(Constants::NUMERIC_DATA_TYPES) * _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType1 : Constants::NUMERIC_DATA_TYPES)
						for (ProgramDataTypes dataType2 : Constants::NUMERIC_DATA_TYPES)
						{
							BEGIN_OVERRIDE(dataType1);
							{
								ADD_PARAMETER(dataType1);
								ADD_PARAMETER(dataType2);
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
								SET_NATIVE_DESCRIPTION("mul");
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Remainder", _countof(Constants::NUMERIC_DATA_TYPES) * _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType1 : Constants::NUMERIC_DATA_TYPES)
						for (ProgramDataTypes dataType2 : Constants::NUMERIC_DATA_TYPES)
						{
							BEGIN_OVERRIDE(ProgramDataTypes::Float);
							{
								ADD_PARAMETER(dataType1);
								ADD_PARAMETER(dataType2);
								SET_NATIVE_DESCRIPTION("fmod");
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
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
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
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
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

				BEGIN_FUNCTION("Pow", _countof(Constants::NUMERIC_AND_VECTOR_DATA_TYPES) * _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType : Constants::NUMERIC_AND_VECTOR_DATA_TYPES)
						for (ProgramDataTypes powDataType : Constants::NUMERIC_DATA_TYPES)
						{
							BEGIN_OVERRIDE(dataType);
							{
								ADD_PARAMETER(dataType);
								ADD_PARAMETER(powDataType);
								SET_NATIVE_DESCRIPTION("pow");
							}
							END_OVERRIDE();
						}
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Clamp", _countof(Constants::NUMERIC_DATA_TYPES) * _countof(Constants::NUMERIC_DATA_TYPES) * _countof(Constants::NUMERIC_DATA_TYPES));
				{
					for (ProgramDataTypes dataType1 : Constants::NUMERIC_DATA_TYPES)
						for (ProgramDataTypes dataType2 : Constants::NUMERIC_DATA_TYPES)
							for (ProgramDataTypes dataType3 : Constants::NUMERIC_DATA_TYPES)
							{
								BEGIN_OVERRIDE(dataType1);
								{
									ADD_PARAMETER(dataType1);
									ADD_PARAMETER(dataType2);
									ADD_PARAMETER(dataType3);
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
								String argument;
						Compiler->BuildStatement(Arguments[0], Type, Stage, argument);

						Shader += ASTToHLSLCompiler::Private::ASTToHLSLCompiler::GetGeometryOutputStreamParameterName();
						Shader += ".Append(";
						Shader += argument;
						Shader += ");\n";
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
								Shader += ASTToHLSLCompiler::Private::ASTToHLSLCompiler::GetGeometryOutputStreamParameterName();
						Shader += ".RestartStrip();\n";
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();
			}

			void HLSLIntrinsicProvider::BuildBuiltIns(ICompiler* Compiler, Stages Stage, String& Shader)
			{
				for (ProgramDataTypes textureType : Constants::READABLE_TEXTURE_TYPES)
					for (ProgramDataTypes textureDataType : Constants::TEXTURE_DATA_TYPES)
					{
						ProgramDataTypes returnType;
						switch (textureType)
						{
						case Engine::ProgramCompilerCommon::ProgramDataTypes::Texture1D:
						case Engine::ProgramCompilerCommon::ProgramDataTypes::Texture1DRW:
							returnType = ProgramDataTypes::UnsignedInteger;
							break;
						case Engine::ProgramCompilerCommon::ProgramDataTypes::Texture2D:
						case Engine::ProgramCompilerCommon::ProgramDataTypes::Texture2DRW:
							returnType = ProgramDataTypes::UnsignedInteger2;
							break;
						case Engine::ProgramCompilerCommon::ProgramDataTypes::Texture3D:
						case Engine::ProgramCompilerCommon::ProgramDataTypes::Texture3DRW:
							returnType = ProgramDataTypes::UnsignedInteger3;
							break;
						case Engine::ProgramCompilerCommon::ProgramDataTypes::TextureCube:
							returnType = ProgramDataTypes::UnsignedInteger4;
							break;

						default:
							THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
						}

						Compiler->BuildType(returnType, Shader);
						Shader += " GetDimensions(";
						Compiler->BuildType(textureType, Shader);
						Shader += "<";
						Compiler->BuildType(textureDataType, Shader);
						Shader += "> Texture){";

						uint8 componentCount = DataTypeStatement::GetComponentCount(returnType);
						for (uint8 i = 0; i < componentCount; ++i)
						{
							Compiler->BuildType(ProgramDataTypes::UnsignedInteger, Shader);
							Shader += " result";
							Shader += StringUtility::ToString<char8>(i);
							Shader += ";";
						}

						Shader += "Texture.GetDimensions(";

						for (uint8 i = 0; i < componentCount; ++i)
						{
							if (i != 0)
								Shader += ", ";

							Shader += "result";
							Shader += StringUtility::ToString<char8>(i);
						}

						Shader += ");";

						Shader += "return ";

						if (componentCount == 1)
							Shader += "result0";
						else
						{
							Compiler->BuildType(ProgramDataTypes::UnsignedInteger, Shader);
							Shader += StringUtility::ToString<char8>(componentCount);
							Shader += '(';
							for (uint8 i = 0; i < componentCount; ++i)
							{
								if (i != 0)
									Shader += ", ";

								Shader += "result";
								Shader += StringUtility::ToString<char8>(i);
							}

							Shader += ')';
						}

						Shader += ";}";
					}

				Shader += '\n';
			}
		}
	}
}