// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPENGL_COMPILER_H
#define OPENGL_COMPILER_H

#include <ASTCompiler\ASTCompilerBase.h>

namespace Engine
{
	using namespace ASTCompiler;
	using namespace Containers;
	using namespace Allocators;

	namespace ASTToGLSLCompiler
	{
		namespace Private
		{
			class ASTToGLSLCompiler : public ASTCompilerBase
			{
			public:
				ASTToGLSLCompiler(void);

				virtual void Initialize(DeviceTypes DeviceType) override;

				virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, OutputInfo& Output) override;

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

				virtual void BuildSwitchStatement(SwitchStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

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
				uint8 m_MemberAccessLevel;
			};
		}
	}
}

#endif