// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_COMPILER_H
#define AST_COMPILER_H

#include <ProgramCompilerCommon\OutputInfo.h>
#include <APIIntrinsic\IntrinsicsBuilder.h>
#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>
#include <ProgramParser\AbstractSyntaxTree\StructType.h>
#include <ProgramParser\AbstractSyntaxTree\GlobalVariableType.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ProgramParser\AbstractSyntaxTree\DomainAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\PartitioningAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\TopologyAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\ControlPointsAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\ConstantEntrypointAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\MaxVertexCountAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\PrimitiveTypeAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\OutputStreamTypeAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\ThreadCountAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\IfStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ElseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\SwitchStatement.h>
#include <ProgramParser\AbstractSyntaxTree\CaseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\DefaultStatement.h>
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
#include <ProgramParser\AbstractSyntaxTree\ArrayStatement.h>
#include <Containers\Stack.h>
#include <Allocators\AllocatorBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace Allocators;
	using namespace ProgramCompilerCommon;
	using namespace ProgramParser::AbstractSyntaxTree;

	namespace ASTCompiler
	{
		class ASTCOMPILER_API ASTCompilerBase : protected IntrinsicsBuilder
		{
		protected:
			struct StageData
			{
				const FunctionType::Types FunctionType;
				const Stages Stage;
				const StructList& Structs;
				const GlobalVariableList& Variables;
				const FunctionList& Functions;
				String& Shader;
				int8 IndentOffset;
			};

			typedef Map<String, String> OutputMap;
			typedef Map<String, DataTypeStatement*> VariableTypeMap;
			typedef Vector<VariableList> BlockVariablesList;
			typedef Vector<const Statement*> StatementStack;

		public:
			ASTCompilerBase(void);
			~ASTCompilerBase(void)
			{
			}

			virtual void Initialize(DeviceTypes DeviceType);

			virtual void Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output);

		protected:
			virtual void BuildStageShader(StageData& Data);

			virtual void ResetPerStageValues(StageData& Data);

			virtual void BuildStructs(StageData& Data);

			virtual void BuildStruct(const StructType* Struct, StageData& Data) = 0;

			virtual void BuildGlobalVariables(StageData& Data);

			virtual void BuildGlobalVariable(const GlobalVariableType* Variable, StageData& Data) = 0;

			virtual void BuildFunctions(StageData& Data);

			virtual void BuildFunction(const FunctionType* Function, StageData& Data) = 0;

			virtual void ValidateEntrypointFunction(const FunctionType* Function, StageData& Data);

			virtual void BuildAttributes(const AttributeList& Attributes, StageData& Data);

			virtual void BuildAttribute(const BaseAttributeType* Attribute, StageData& Data);

			virtual void BuildDomainAttributeType(const DomainAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildPartitioningAttributeType(const PartitioningAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildTopologyAttributeType(const TopologyAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildControlPointsAttributeType(const ControlPointsAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildConstantEntrypointAttributeType(const ConstantEntrypointAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildMaxVertexCountAttributeType(const MaxVertexCountAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildPrimitiveTypeAttributeType(const PrimitiveTypeAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildOutputStreamTypeAttributeType(const OutputStreamTypeAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildThreadCountAttributeType(const ThreadCountAttributeType* Attribute, StageData& Data)
			{
			}

			virtual void BuildParameters(const ParameterList& Parameters, StageData& Data);

			virtual void BuildParameter(const ParameterType* Parameter, StageData& Data);

			virtual void BuildStatementHolder(const StatementItemHolder* Holder, bool IncreamentBlock, StageData& Data);

			virtual void BuildStatement(const Statement* Statement, StageData& Data);

			virtual void BuildOperatorStatement(const OperatorStatement* Statement, StageData& Data);

			virtual void BuildUnaryOperatorStatement(const UnaryOperatorStatement* Statement, StageData& Data);

			virtual void BuildConstantStatement(const ConstantStatement* Statement, StageData& Data);

			virtual void BuildFunctionCallStatement(const FunctionCallStatement* Statement, StageData& Data);

			virtual void BuildArguments(const Vector<Statement*>& Statements, StageData& Data);

			virtual void BuildArguments(const StatementItemHolder* Statements, StageData& Data);

			virtual void BuildVariableStatement(const VariableStatement* Statement, StageData& Data);

			virtual void BuildVariableAccessStatement(const VariableAccessStatement* Statement, StageData& Data);

			virtual void BuildArrayElementAccessStatement(const ArrayElementAccessStatement* Statement, StageData& Data);

			virtual void BuildMemberAccessStatement(const MemberAccessStatement* Statement, StageData& Data);

			virtual void BuildIfStatement(const IfStatement* Statement, StageData& Data);

			virtual void BuildElseStatement(const ElseStatement* Statement, StageData& Data);

			virtual void BuildSwitchStatement(const SwitchStatement* Statement, StageData& Data);

			virtual void BuildCaseStatement(const CaseStatement* Statement, StageData& Data);

			virtual void BuildDefaultStatement(const DefaultStatement* Statement, StageData& Data);

			virtual void BuildForStatement(const ForStatement* Statement, StageData& Data);

			virtual void BuildDoStatement(const DoStatement* Statement, StageData& Data);

			virtual void BuildWhileStatement(const WhileStatement* Statement, StageData& Data);

			virtual void BuildContinueStatement(const ContinueStatement* Statement, StageData& Data);

			virtual void BuildBreakStatement(const BreakStatement* Statement, StageData& Data);

			virtual void BuildReturnStatement(const ReturnStatement* Statement, StageData& Data) = 0;

			virtual void BuildArrayStatement(const ArrayStatement* Statement, StageData& Data) = 0;

			virtual void BuildDiscardStatement(const DiscardStatement* Statement, StageData& Data);

			virtual void BuildDataTypeStatement(const DataTypeStatement* Statement, StageData& Data);

			virtual void BuildPreTemplateDataTypeStatement(const DataTypeStatement* Statement, StageData& Data);
			virtual void BuildPostTemplateDataTypeStatement(const DataTypeStatement* Statement, StageData& Data);

			virtual void BuildExplicitCast(const Statement* Statement, const DataTypeStatement* DataType, StageData& Data);
			virtual void BuildExplicitCast(const Statement* Statement, ProgramDataTypes DataType, StageData& Data);

			virtual void BuildDataTypeInitializer(const DataTypeStatement* DataType, StageData& Data);

			virtual void BuildType(ProgramDataTypes Type, StageData& Data) = 0;

			virtual uint8 EvaluateDataTypeElementCount(const DataTypeStatement* Statement);
			DataTypeStatement EvaluateDataType(const Statement* CurrentStatement, const Statement* TopStatement = nullptr) const;
			ProgramDataTypes EvaluateProgramDataType(const Statement* Statement) const override;

			bool CompareDataTypes(const DataTypeStatement& Left, const DataTypeStatement& Right) const;
			void CheckForImplicitCast(const DataTypeStatement& Source, const DataTypeStatement& Destination) const;

			void CheckVariableExsitence(const String& Name) const;

			const VariableType* FindVariableType(const String& Name, bool LatestBlockOnly = false) const;
			void IncreaseBlockIndex(void);
			void DecreaseBlockIndex(void);
			void PushVariable(const VariableType* Variable);
			void IncreaseBlockIndexAndPushStructVariables(const StructType* Struct);

			const FunctionType* FindFunctionType(const String& Name) const;
			const FunctionType* FindMatchingFunction(const String& Name, const StatementItemHolder* Arguments) const;
			const FunctionType* GetFunctionType(const String& Name) const;
			const FunctionType* GetEntrypointFunctionType(FunctionType::Types Type, StageData& Data) const;

			const StructType* FindStructType(const String& Name) const;
			const StructType* GetStructType(const String& Name) const;

			const StructVariableType* FindVariableType(const StructType* StructType, const String& Name) const;
			const StructVariableType* GetVariableType(const StructType* StructType, const String& Name) const;

			const StructVariableType* FindVariableType(const StructType* StructType, std::function<bool(const StructVariableType*)> Condition) const;
			const StructVariableType* GetVariableType(const StructType* StructType, std::function<bool(const StructVariableType*)> Condition) const;

			String StatementToString(const Statement* Statement, const StageData& Data);

			void AddCode(const String& Value, StageData& Data);
			void AddNewLine(StageData& Data);

			String BuildSequentialVariable(const Statement* IntializerStatement, StageData& Data);
			String BuildSequentialVariable(const DataTypeStatement* DataType, const Statement* IntializerStatement, StageData& Data);
			String GetSequentialVariableName(void);

			void BuildStatement(const Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
			{
				StageData data = { Type, Stage, {}, {}, {}, Shader, 0 };
				BuildStatement(Statement, data);
			}

			void BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader) override
			{
				StageData data = { Type, Stage, {}, {}, {}, Shader, 0 };
				BuildArguments(Statements, data);
			}

			bool IsHullConstant(const FunctionType* Function, StageData& Data) const
			{
				return (Data.Stage == Stages::Hull && Function == m_HullConstantFunction);
			}

			bool IsEntrypointOrHullConstant(const FunctionType* Function, StageData& Data) const
			{
				return (Function->IsEntrypoint() || IsHullConstant(Function, Data));
			}

			const FunctionType* GetHullConstantFunction(void) const
			{
				return m_HullConstantFunction;
			}

			AllocatorBase* GetAllocator(void) const
			{
				return m_Allocator;
			}

			const FunctionType* GetLastFunction(void) const
			{
				return m_LastFunction;
			}

			void PushDataAccessStatement(const Statement* Statement)
			{
				m_DataAccessStatements.Insert(0, Statement);
			}
			void PopDataAceessStatement(void)
			{
				CoreDebugAssert(Categories::ProgramCompiler, m_DataAccessStatements.GetSize() != 0, "A mismatch has happend in data-access stack");

				m_DataAccessStatements.RemoveAt(0);
			}

		public:
			static String GetSamplerVariableName(const String& TextureVariableName);

		private:
			AllocatorBase* m_Allocator;

			StructList m_Structs;
			FunctionList m_Functions;

			BlockVariablesList m_BlockVariables;
			int8 m_BlockIndex;

			const FunctionType* m_HullConstantFunction;
			FunctionType* m_LastFunction;

			StatementStack m_DataAccessStatements;

			uint32 m_SequentialVariableNumber = 0;

			bool m_ReturnValueAlreadyBuilt;
		};
	}
}

#endif