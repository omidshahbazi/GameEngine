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

			private:
				virtual void ResetPerStageValues(StageData& Data) override;

				virtual void BuildStageShader(StageData& Data) override;

				virtual void BuildStruct(const StructType* Struct, StageData& Data) override;

				virtual void BuildGlobalVariable(const GlobalVariableType* Variable, StageData& Data) override;

				virtual void BuildFunction(const FunctionType* Function, StageData& Data) override;

				virtual void BuildFunctionCallStatement(const FunctionCallStatement* Statement, StageData& Data) override;

				virtual void BuildReturnStatement(const ReturnStatement* Statement, StageData& Data) override;

				virtual void BuildArrayStatement(const ArrayStatement* Statement, StageData& Data) override;

				virtual void BuildExplicitCast(const Statement* Statement, const DataTypeStatement* DataType, StageData& Data) override;

				virtual void BuildType(ProgramDataTypes Type, StageData& Data) override;

				virtual void BuildPostDataType(const DataTypeStatement* Type, StageData& Data);

				virtual void BuildInputOutputStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, bool ConvertToArray, bool ExplicitArrayLength, StageData& Data);

				virtual void BuildUniformBlock(const StructType* Struct, const String& Name, StageData& Data);

				virtual void BuildFlattenStructMemberVariableName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, StageData& Data);

			private:
				uint8 m_AdditionalLayoutCount;
				uint8 m_BindingCount;
			};
		}
	}
}

#endif