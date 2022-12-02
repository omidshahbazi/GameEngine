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

				virtual void Initialize(DeviceTypes DeviceType) override;

				virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output) override;

			private:
				virtual void ResetPerStageValues(const StageData& Data) override;

				virtual void BuildStageShader(const StageData& Data) override;

				virtual void BuildStruct(StructType* Struct, const StageData& Data) override;

				virtual void BuildGlobalVariable(GlobalVariableType* Variable, const StageData& Data) override;

				virtual void BuildFunction(FunctionType* Function, const StageData& Data) override;

				virtual void BuildDomainAttributeType(DomainAttributeType* Attribute, const StageData& Data)  override;

				virtual void BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, const StageData& Data)  override;

				virtual void BuildTopologyAttributeType(TopologyAttributeType* Attribute, const StageData& Data) override;

				virtual void BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, const StageData& Data) override;

				virtual void BuildMaxVertexCountAttributeType(MaxVertexCountAttributeType* Attribute, const StageData& Data) override;

				virtual void BuildPrimitiveTypeAttributeType(PrimitiveTypeAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildOutputStreamTypeAttributeType(OutputStreamTypeAttributeType* Attribute, const StageData& Data) override
				{
				}

				virtual void BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, const StageData& Data)  override;

				virtual void BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, const StageData& Data)  override;

				virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, const StageData& Data) override;

				virtual void BuildReturnStatement(ReturnStatement* Statement, const StageData& Data) override;

				virtual void BuildArrayStatement(ArrayStatement* Statement, const StageData& Data) override;

				virtual void BuildType(ProgramDataTypes Type, const StageData& Data) override;

				virtual void BuildRootSignature(const StageData& Data);

				virtual void BuildStructVariable(StructVariableType* Variable, const StageData& Data);

				static cstr GetRootSignatureDefineName(void);

			public:
				static String GetSamplerVariableName(const String& TextureVariableName);
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