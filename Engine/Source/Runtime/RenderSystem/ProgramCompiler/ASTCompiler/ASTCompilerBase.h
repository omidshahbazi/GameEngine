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

			virtual void BuildStruct(StructType* Struct, StageData& Data) = 0;

			virtual void BuildGlobalVariables(StageData& Data);

			virtual void BuildGlobalVariable(GlobalVariableType* Variable, StageData& Data) = 0;

			virtual void BuildFunctions(StageData& Data);

			virtual void BuildFunction(FunctionType* Function, StageData& Data) = 0;

			virtual void ValidateEntrypointFunction(FunctionType* Function, StageData& Data);

			virtual void BuildAttributes(const AttributeList& Attributes, StageData& Data);

			virtual void BuildAttribute(BaseAttributeType* Attribute, StageData& Data);

			virtual void BuildDomainAttributeType(DomainAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildTopologyAttributeType(TopologyAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildMaxVertexCountAttributeType(MaxVertexCountAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildPrimitiveTypeAttributeType(PrimitiveTypeAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildOutputStreamTypeAttributeType(OutputStreamTypeAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, StageData& Data) = 0;

			virtual void BuildParameters(const ParameterList& Parameters, StageData& Data);

			virtual void BuildParameter(ParameterType* Parameter, StageData& Data);

			virtual void BuildStatementHolder(StatementItemHolder* Holder, bool IncreamentBlock, StageData& Data);

			virtual void BuildStatement(Statement* Statement, StageData& Data);

			virtual void BuildOperatorStatement(OperatorStatement* Statement, StageData& Data);

			virtual void BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, StageData& Data);

			virtual void BuildConstantStatement(ConstantStatement* Statement, StageData& Data);

			virtual void BuildFunctionCallStatement(FunctionCallStatement* Statement, StageData& Data);

			virtual void BuildArguments(const Vector<Statement*>& Statements, StageData& Data);

			virtual void BuildArguments(StatementItemHolder* Statements, StageData& Data);

			virtual void BuildVariableStatement(VariableStatement* Statement, StageData& Data);

			virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, StageData& Data) = 0;

			virtual void BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, StageData& Data);

			virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, StageData& Data);

			virtual void BuildIfStatement(IfStatement* Statement, StageData& Data);

			virtual void BuildElseStatement(ElseStatement* Statement, StageData& Data);

			virtual void BuildSwitchStatement(SwitchStatement* Statement, StageData& Data);

			virtual void BuildCaseStatement(CaseStatement* Statement, StageData& Data);

			virtual void BuildDefaultStatement(DefaultStatement* Statement, StageData& Data);

			virtual void BuildForStatement(ForStatement* Statement, StageData& Data);

			virtual void BuildDoStatement(DoStatement* Statement, StageData& Data);

			virtual void BuildWhileStatement(WhileStatement* Statement, StageData& Data);

			virtual void BuildContinueStatement(ContinueStatement* Statement, StageData& Data);

			virtual void BuildBreakStatement(BreakStatement* Statement, StageData& Data);

			virtual void BuildReturnStatement(ReturnStatement* Statement, StageData& Data) = 0;

			virtual void BuildArrayStatement(ArrayStatement* Statement, StageData& Data) = 0;

			virtual void BuildDiscardStatement(DiscardStatement* Statement, StageData& Data);

			virtual uint8 BuildReturnValue(Statement* Statement, StageData& Data);

			virtual void BuildDataTypeStatement(const DataTypeStatement* Statement, StageData& Data);

			virtual void BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, StageData& Data);
			virtual void BuildExplicitCast(Statement* Statement, ProgramDataTypes DataType, StageData& Data);

			virtual void BuildType(ProgramDataTypes Type, StageData& Data) = 0;

			virtual uint8 EvaluateDataTypeElementCount(DataTypeStatement* Statement);
			DataTypeStatement EvaluateDataType(Statement* CurrentStatement, Statement* TopStatement = nullptr) const;
			ProgramDataTypes EvaluateProgramDataType(Statement* Statement) const override;

			bool CompareDataTypes(const DataTypeStatement& Left, const DataTypeStatement& Right) const;
			void CheckForImplicitCast(const DataTypeStatement& Source, const DataTypeStatement& Destination) const;

			const VariableType* FindVariableType(const String& Name, bool LatestBlockOnly = false) const;
			void IncreaseBlockIndex(void);
			void DecreaseBlockIndex(void);
			void PushVariable(VariableType* Variable);

			const FunctionType* FindFunctionType(const String& Name) const;
			const FunctionType* FindMatchingFunction(const String& Name, const StatementItemHolder* Arguments) const;
			const FunctionType* GetFunctionType(const String& Name) const;

			const StructType* FindStructType(const String& Name) const;
			const StructType* GetStructType(const String& Name) const;

			const StructVariableType* FindVariableType(const StructType* StructType, const String& Name) const;
			const StructVariableType* GetVariableType(const StructType* StructType, const String& Name) const;

			const StructVariableType* FindVariableType(const StructType* StructType, std::function<bool(const StructVariableType*)> Condition) const;
			const StructVariableType* GetVariableType(const StructType* StructType, std::function<bool(const StructVariableType*)> Condition) const;

			void AddCode(const String& Value, StageData& Data);
			void AddNewLine(StageData& Data);

			void BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
			{
				StageData data = { Type, Stage, {}, {}, {}, Shader, 0 };
				BuildStatement(Statement, data);
			}

			void BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader) override
			{
				StageData data = { Type, Stage, {}, {}, {}, Shader, 0 };
				BuildArguments(Statements, data);
			}

			AllocatorBase* GetAllocator(void) const
			{
				return m_Allocator;
			}

			const FunctionType* GetLastFunction(void) const
			{
				return m_LastFunction;
			}

			static cstr GetStageResultArrayVariableName(void);

		private:
			AllocatorBase* m_Allocator;

			StructList m_Structs;
			FunctionList m_Functions;

			BlockVariablesList m_BlockVariables;
			int8 m_BlockIndex;

			FunctionType* m_LastFunction;

			bool m_ReturnValueAlreadyBuilt;
		};
	}
}

#endif