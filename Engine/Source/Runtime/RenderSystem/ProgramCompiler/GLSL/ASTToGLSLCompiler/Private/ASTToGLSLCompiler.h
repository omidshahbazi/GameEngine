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

				virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output) override;

			private:
				virtual void ResetPerStageValues(Stages Stage) override;

				virtual void BuildHeader(String& Shader) override;

				virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override;

				virtual void BuildStructVariable(StructVariableType* Variable, Stages Stage, String& Shader) override;

				virtual void BuildGlobalVariable(GlobalVariableType* Variable, Stages Stage, String& Shader) override;

				virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override;

				virtual void BuildDomainAttributeType(DomainAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)  override;

				virtual void BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)  override;

				virtual void BuildTopologyAttributeType(TopologyAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)  override;
				
				virtual void BuildMaxVertexCountAttributeType(MaxVertexCountAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)  override;

				virtual void BuildPrimitiveTypeAttributeType(PrimitiveTypeAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildOutputStreamTypeAttributeType(OutputStreamTypeAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildSwitchStatement(SwitchStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

				virtual void BuildType(ProgramDataTypes Type, String& Shader) override;

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

#endif