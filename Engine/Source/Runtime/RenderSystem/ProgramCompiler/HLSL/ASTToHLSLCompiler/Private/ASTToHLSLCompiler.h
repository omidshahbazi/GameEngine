// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_TO_HLSL_COMPILER_H
#define AST_TO_HLSL_COMPILER_H

#include <ASTCompiler\ASTCompilerBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace ASTCompiler;
	using namespace MemoryManagement::Allocator;

	namespace ASTToHLSLCompiler
	{
		namespace Private
		{
			class ASTTOHLSLCOMPILER_API ASTToHLSLCompiler : public ASTCompilerBase
			{
			private:
				typedef Map<String, String> OutputMap;

			public:
				ASTToHLSLCompiler(void);

				virtual void Initialize(DeviceTypes DeviceType) override;

				virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, OutputInfo& Output) override;

			private:
				virtual void ResetPerStageValues(Stages Stage) override;

				virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override;

				virtual void BuildVariable(VariableType* Variable, Stages Stage, String& Shader) override;

				virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override;

				virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildType(ProgramDataTypes Type, String& Shader) override;

				virtual void InjectParameterIntoTopFunction(ProgramDataTypes Type, const String& Name, const String& Register) override;

				void BuildStruct(StructType* Struct, Stages Stage, bool IsOutputStruct, String& Shader);

				void BuildVariable(const String& Name, const String& Register, DataTypeStatement* DataType, String& Shader);

				String GetOutputStructName(void) const;

				static String GetStageResultFieldName(uint8 Index);

				static cstr GetStageResultVariableName(void);

				static cstr GetRootSignatureDefineName(void);

			public:
				static String GetSamplerVariableName(const String& TextureVariableName);

			private:
				StructType* m_InputAssemblerStruct;
				FunctionList m_Functions;
				uint8 m_ConstantBufferBindingCount;
				uint8 m_TextureBindingCount;

				String* m_EndOfFunctionParametersCode;
			};
		}
	}
}

#endif