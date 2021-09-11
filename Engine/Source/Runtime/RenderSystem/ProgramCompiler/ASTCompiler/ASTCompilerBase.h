// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_COMPILER_H
#define AST_COMPILER_H

#include <ProgramCompilerCommon\OutputInfo.h>
#include <APIIntrinsic\IntrinsicsBuilder.h>
#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>
#include <ProgramParser\AbstractSyntaxTree\StructType.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ProgramParser\AbstractSyntaxTree\IfStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ElseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\SwitchStatement.h>
#include <ProgramParser\AbstractSyntaxTree\CaseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ForStatement.h>
#include <ProgramParser\AbstractSyntaxTree\DoStatement.h>
#include <ProgramParser\AbstractSyntaxTree\WhileStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ContinueStatement.h>
#include <ProgramParser\AbstractSyntaxTree\BreakStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ReturnStatement.h>
#include <ProgramParser\AbstractSyntaxTree\DiscardStatement.h>
#include <ProgramParser\AbstractSyntaxTree\OperatorStatement.h>
#include <ProgramParser\AbstractSyntaxTree\UnaryOperatorStatement.h>
#include <ProgramParser\AbstractSyntaxTree\VariableStatement.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionCallStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ConstantStatement.h>
#include <ProgramParser\AbstractSyntaxTree\VariableAccessStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ArrayElementAccessStatement.h>
#include <ProgramParser\AbstractSyntaxTree\MemberAccessStatement.h>
#include <ProgramParser\AbstractSyntaxTree\SemicolonStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ArrayStatement.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace MemoryManagement::Allocator;
	using namespace ProgramCompilerCommon;
	using namespace ProgramParser::AbstractSyntaxTree;

	namespace ASTCompiler
	{
		class ASTCOMPILER_API ASTCompilerBase : protected IntrinsicsBuilder
		{
		protected:
			typedef Map<String, String> OutputMap;
			typedef Map<String, DataTypeStatement*> VariableTypeMap;

		public:
			ASTCompilerBase(void);
			~ASTCompilerBase(void)
			{
			}

			virtual void Initialize(DeviceTypes DeviceType);

			virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, OutputInfo& Output);

		protected:
			virtual void BuildStageShader(Stages Stage, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader);

			virtual void BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader);

			virtual void BuildTessellationShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader);

			virtual void BuildGeometryShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader);

			virtual void BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader);

			virtual void BuildComputeShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader);

			virtual void ResetPerStageValues(Stages Stage);

			virtual void BuildHeader(String& Shader);

			virtual void BuildStructs(const StructList& Structs, Stages Stage, String& Shader);

			virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) = 0;

			virtual void BuildVariables(const VariableList& Variables, Stages Stage, String& Shader);

			virtual void BuildVariable(VariableType* VariableType, Stages Stage, String& Shader) = 0;

			virtual void BuildFunctions(const FunctionList& Functions, Stages Stage, String& Shader);

			virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) = 0;

			virtual void BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildConstantStatement(ConstantStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildArguments(StatementItemHolder* Statements, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

			virtual void BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildSemicolonStatement(SemicolonStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildElseStatement(ElseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

			virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

			virtual void BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual uint8 BuildReturnValue(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

			virtual void BuildDataTypeStatement(DataTypeStatement* Statement, String& Shader);

			virtual void BuildType(ProgramDataTypes Type, String& Shader) = 0;

			bool ContainsReturnStatement(StatementItemHolder* Statement);

			virtual uint8 EvaluateDataTypeElementCount(DataTypeStatement* Statement);
			DataTypeStatement EvaluateDataType(Statement* CurrentStatement, Statement* TopStatement = nullptr) const;

			const StructType* FindStructType(const String& Name) const;
			const VariableType* FindVariableType(const StructType* StructType, const String& Name) const;

			static cstr GetStageResultArrayVariableName(void);

			ProgramDataTypes EvaluateProgramDataType(Statement* Statement) const override;

			void BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader) override;
			virtual void InjectParameterIntoTopFunction(ProgramDataTypes Type, const String& Name, const String& Register) override;

			AllocatorBase* GetAllocator(void) const
			{
				return m_Allocator;
			}

			void IncreamentOpenScopeCount(void)
			{
				++m_OpenScopeCount;
			}

			const StructList& GetStructs(void) const
			{
				return m_Structs;
			}

		private:
			AllocatorBase* m_Allocator;

			int8 m_OpenScopeCount;

			StructList m_Structs;
			FunctionList m_Functions;
			VariableTypeMap m_Variables;

			FunctionType* m_LastFunction;
		};
	}
}

#endif