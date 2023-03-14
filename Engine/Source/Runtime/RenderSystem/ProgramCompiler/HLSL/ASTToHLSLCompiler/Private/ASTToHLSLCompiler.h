// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_TO_HLSL_COMPILER_H
#define AST_TO_HLSL_COMPILER_H

#include <ASTCompiler\ASTCompilerBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace ASTCompiler;
	using namespace Allocators;

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

				virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output) override;

			private:
				virtual void ResetPerStageValues(StageData& Data) override;

				virtual void BuildStageShader(StageData& Data) override;

				virtual void BuildStruct(const StructType* Struct, StageData& Data) override;

				virtual void BuildGlobalVariable(const GlobalVariableType* Variable, StageData& Data) override;

				virtual void BuildFunction(const FunctionType* Function, StageData& Data) override;

				virtual void BuildDomainAttributeType(const DomainAttributeType* Attribute, StageData& Data)  override;

				virtual void BuildPartitioningAttributeType(const PartitioningAttributeType* Attribute, StageData& Data)  override;

				virtual void BuildTopologyAttributeType(const TopologyAttributeType* Attribute, StageData& Data) override;

				virtual void BuildControlPointsAttributeType(const ControlPointsAttributeType* Attribute, StageData& Data) override;

				virtual void BuildMaxVertexCountAttributeType(const MaxVertexCountAttributeType* Attribute, StageData& Data) override;

				virtual void BuildConstantEntrypointAttributeType(const ConstantEntrypointAttributeType* Attribute, StageData& Data)  override;

				virtual void BuildThreadCountAttributeType(const ThreadCountAttributeType* Attribute, StageData& Data)  override;

				virtual void BuildReturnStatement(const ReturnStatement* Statement, StageData& Data) override;

				virtual void BuildArrayStatement(const ArrayStatement* Statement, StageData& Data) override;

				virtual void BuildPostTemplateDataTypeStatement(const DataTypeStatement* Statement, StageData& Data) override;

				virtual void BuildType(const ProgramDataTypes Type, StageData& Data) override;

				virtual void BuildRootSignature(StageData& Data);

				virtual void BuildStructVariable(const StructVariableType* Variable, StageData& Data);

				static cstr GetRootSignatureDefineName(void);

			public:
				static String GetGeometryOutputStreamParameterName(void);

			private:
				FunctionList m_Functions;
				uint8 m_ConstantBufferBindingCount;
				uint8 m_TextureBindingCount;
			};
		}
	}
}

#endif