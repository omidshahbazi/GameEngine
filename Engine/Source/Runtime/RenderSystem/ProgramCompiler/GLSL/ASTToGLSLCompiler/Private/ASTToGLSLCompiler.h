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
				virtual void ResetPerStageValues(const StageData& Data) override;

				virtual void BuildStageShader(const StageData& Data) override;

				virtual void BuildStruct(StructType* Struct, const StageData& Data) override;

				virtual void BuildGlobalVariable(GlobalVariableType* Variable, const StageData& Data) override;

				virtual void BuildFunction(FunctionType* Function, const StageData& Data) override;

				virtual void BuildDomainAttributeType(DomainAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildTopologyAttributeType(TopologyAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, const StageData& Data) override;

				virtual void BuildMaxVertexCountAttributeType(MaxVertexCountAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildPrimitiveTypeAttributeType(PrimitiveTypeAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildOutputStreamTypeAttributeType(OutputStreamTypeAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, const StageData& Data) override;

				virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, const StageData& Data) override;

				virtual void BuildSwitchStatement(SwitchStatement* Statement, const StageData& Data) override;

				virtual void BuildReturnStatement(ReturnStatement* Statement, const StageData& Data) override;

				virtual void BuildArrayStatement(ArrayStatement* Statement, const StageData& Data) override;

				virtual void BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, const StageData& Data) override;

				virtual void BuildType(ProgramDataTypes Type, const StageData& Data) override;

				virtual void BuildPostDataType(const DataTypeStatement* Type, const StageData& Data);

				virtual void BuildInOutStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, const StageData& Data);

				virtual void BuildUniformBlock(const StructType* Struct, const String& Name, const StageData& Data);

				virtual void BuildFlattenStructMemberVariableName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, const StageData& Data);

			private:
				uint8 m_AdditionalLayoutCount;
				uint8 m_BindingCount;
			};
		}
	}
}

#endif