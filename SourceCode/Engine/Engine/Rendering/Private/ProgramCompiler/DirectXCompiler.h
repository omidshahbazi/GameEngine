// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_COMPILER_H
#define DIRECTX_COMPILER_H

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

				class DirectXCompiler : public APICompiler
				{
				private:
					typedef Map<String, String> OutputMap;

				public:
					DirectXCompiler(void);

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, CompileOutputInfo& Output) override;

				private:
					virtual void ResetPerStageValues(Stages Stage) override;

					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override;

					virtual void BuildVariable(VariableType* Variable, Stages Stage, String& Shader) override;

					virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override;

					virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override;

					virtual void BuildDataType(const DataType& Type, String& Shader) override;

					virtual void BuildType(ProgramDataTypes Type, String& Shader) override;

					void BuildStruct(StructType* Struct, Stages Stage, bool IsOutputStruct, String& Shader);

					void BuildVariable(const String& Name, const String& Register, const DataType& DataType, String& Shader);

					String GetOutputStructName(void) const;

					static String GetStageResultFieldName(uint8 Index);

					static cstr GetStageResultVariableName(void);

					static cstr GetRootSignatureDefineName(void);
				
				public:
					static String GetSamplerVariableName(const String& TextureVariableName);

				private:
					StructType* m_InputAssemblerStruct;
					FunctionType* m_LastFunction;
					FunctionList m_Functions;
					bool m_Add_SV_Position;
					uint8 m_BindingCount;
					String m_LatestAccessedTextureName;
				};
			}
		}
	}
}

#endif