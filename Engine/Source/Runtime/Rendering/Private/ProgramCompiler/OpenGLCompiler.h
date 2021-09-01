// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPENGL_COMPILER_H
#define OPENGL_COMPILER_H

#include <Rendering\Private\ProgramCompiler\APICompiler.h>

namespace Engine
{
	using namespace Containers;
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				using namespace Syntax;

				class OpenGLCompiler : public APICompiler
				{
				public:
					OpenGLCompiler(AllocatorBase* Allocator);

					virtual void Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, CompileOutputInfo& Output) override;

				private:
					virtual void ResetPerStageValues(Stages Stage) override;

					virtual void BuildHeader(String& Shader) override;

					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override;

					virtual void BuildVariable(VariableType* Variable, Stages Stage, String& Shader) override;

					virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override;

					virtual void BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildType(ProgramDataTypes Type, String& Shader) override;

					void BuildVariable(String Name, const String& Register, DataTypeStatement* DataType, bool IsOutputMode, String& Shader);

					void BuildUniformBlock(const StructType* Struct, const String& Name, Stages Stage, String& Shader);

				private:
					uint8 m_AdditionalLayoutCount;
					uint8 m_BindingCount;
					OutputMap m_Outputs;
					ParameterList m_Parameters;
				};
			}
		}
	}
}

#endif