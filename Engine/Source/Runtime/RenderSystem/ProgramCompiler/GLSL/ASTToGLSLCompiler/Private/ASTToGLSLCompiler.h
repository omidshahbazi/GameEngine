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

				virtual void BuildStruct(StructType* Struct, StageData& Data) override;

				virtual void BuildGlobalVariable(GlobalVariableType* Variable, StageData& Data) override;

				virtual void BuildFunction(FunctionType* Function, StageData& Data) override;

				virtual void BuildDomainAttributeType(DomainAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildTopologyAttributeType(TopologyAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, StageData& Data) override;

				virtual void BuildMaxVertexCountAttributeType(MaxVertexCountAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildPrimitiveTypeAttributeType(PrimitiveTypeAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildOutputStreamTypeAttributeType(OutputStreamTypeAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, StageData& Data) override
				{
				}

				virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, StageData& Data) override;

				virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, StageData& Data) override;

				virtual void BuildReturnStatement(ReturnStatement* Statement, StageData& Data) override;

				virtual void BuildArrayStatement(ArrayStatement* Statement, StageData& Data) override;

				virtual void BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, StageData& Data) override;

				virtual void BuildType(ProgramDataTypes Type, StageData& Data) override;

				virtual void BuildPostDataType(const DataTypeStatement* Type, StageData& Data);

				virtual void BuildInOutStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, StageData& Data);

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