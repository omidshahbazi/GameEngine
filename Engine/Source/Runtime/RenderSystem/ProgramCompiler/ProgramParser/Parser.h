// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <RenderCommon\Enums.h>
#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <ProgramCompilerCommon\Common.h>
#include <Allocators\AllocatorBase.h>
#include <Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Containers\Stack.h>
#include <memory>

namespace Engine
{
	using namespace MemoryManagement;
	using namespace Lexer;
	using namespace Containers;
	using namespace RenderCommon;
	using namespace ProgramCompilerCommon;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class VariableType;
			class GlobalVariableType;
			class StructType;
			class StructVariableType;
			class BaseAttributeType;
			class FunctionType;
			class ParameterType;
			class DataTypeStatement;

			typedef Vector<StructType*> StructList;
			typedef Stack<StructType*> StructStack;
			typedef Vector<VariableType*> VariableList;
			typedef Vector<GlobalVariableType*> GlobalVariableList;
			typedef Vector<BaseAttributeType*> AttributeList;
			typedef Vector<FunctionType*> FunctionList;
		}

		using namespace AbstractSyntaxTree;

		class PROGRAMPARSER_API Parser : private Tokenizer
		{
		private:
			enum class EndConditions
			{
				None = 1 << 0,
				Colon = 1 << 1,
				Semicolon = 1 << 2,
				Brace = 1 << 3,
				Comma = 1 << 4,
				Bracket = 1 << 5,
				SquareBracket = 1 << 6,
				Break = 1 << 7,
			};

			typedef std::function<Statement* (const Token& DeclarationToken)> KeywordParseFunction;
			typedef std::shared_ptr<KeywordParseFunction> KeywordParseFunctionPtr;
			typedef Map<String, KeywordParseFunctionPtr> KeywordParseMap;
			typedef std::function<BaseAttributeType* (const Token& DeclarationToken)> AttributeParseFunction;
			typedef std::shared_ptr<AttributeParseFunction> AttributeParseFunctionPtr;
			typedef Map<String, AttributeParseFunctionPtr> AttributeParseMap;
			typedef Stack<VariableList> VariablesStack;

		public:
			struct Parameters
			{
			public:
				StructList Structs;
				GlobalVariableList Variables;
				FunctionList Functions;
			};

		public:
			Parser(AllocatorBase* Allocator, const String& Text);

			void Parse(Parameters& Parameters);

		private:
			bool ParseStruct(const Token& DeclarationToken);
			bool ParseStructVariable(const Token& DeclarationToken, StructVariableType* VariableType);
			bool ParseGlobalVariable(const Token& DeclarationToken);
			bool ParseAttribute(const Token& DeclarationToken);
			bool ParseFunction(const Token& DeclarationToken);
			bool ParseParameter(const Token& DeclarationToken, ParameterType* ParameterType);

			BaseAttributeType* ParseDomainAttributeType(const Token& DeclarationToken);
			BaseAttributeType* ParsePartitioningAttributeType(const Token& DeclarationToken);
			BaseAttributeType* ParseTopologyAttributeType(const Token& DeclarationToken);
			BaseAttributeType* ParseControlPointsAttributeType(const Token& DeclarationToken);
			BaseAttributeType* ParseConstantEntrypointAttributeType(const Token& DeclarationToken);
			BaseAttributeType* ParseThreadCountAttributeType(const Token& DeclarationToken);

			DataTypeStatement* ParseDataType(const Token& DeclarationToken);
			void ParsePostArrayDataType(DataTypeStatement* DataType);
			Statement* ParseArrayElementCountStatement(void);

			Statement* ParseIfStatement(const Token& DeclarationToken);
			Statement* ParseElseStatement(const Token& DeclarationToken);
			Statement* ParseSwitchStatement(const Token& DeclarationToken);
			Statement* ParseDefaultStatement(const Token& DeclarationToken);
			Statement* ParseCaseStatement(const Token& DeclarationToken);
			Statement* ParseForStatement(const Token& DeclarationToken);
			Statement* ParseDoStatement(const Token& DeclarationToken);
			Statement* ParseWhileStatement(const Token& DeclarationToken);
			Statement* ParseContinueStatement(const Token& DeclarationToken);
			Statement* ParseBreakStatement(const Token& DeclarationToken);
			Statement* ParseReturnStatement(const Token& DeclarationToken);
			Statement* ParseDiscardStatement(const Token& DeclarationToken);

			void ParseScopedStatements(StatementItemHolder* StatementItemHolder, bool MustHaveBrackets, EndConditions ConditionMask);

			Statement* ParseVariableStatement(const Token& DeclarationToken, EndConditions ConditionMask);

			Statement* ParseExpression(const Token& DeclarationToken, EndConditions ConditionMask);
			Statement* ParseUnaryExpression(const Token& DeclarationToken, EndConditions ConditionMask);
			Statement* ParseUnaryExpressionPrefix(const Token& DeclarationToken, EndConditions ConditionMask);
			Statement* ParseUnaryOperatorExpression(const Token& DeclarationToken, EndConditions ConditionMask);
			Statement* ParseBinaryExpression(int8 LeftHandPrecedence, Statement* LeftHandStatement, EndConditions ConditionMask);
			Statement* ParseArrayExpression(const Token& DeclarationToken, EndConditions ConditionMask);

			Statement* ParseConstantStatement(const Token& DeclarationToken);
			Statement* ParseVariableAccessStatement(const Token& DeclarationToken);
			Statement* ParseVariableAccessStatementBase(const Token& DeclarationToken);
			Statement* ParseArrayElementAccessStatement(const Token& DeclarationToken, Statement* ArrayStatement);
			Statement* ParseMemberAccessStatement(const Token& DeclarationToken, Statement* LeftStatement, StructType* ParentType);
			Statement* ParseFunctionCallStatement(const Token& DeclarationToken);

			bool IsEndCondition(const Token& DeclarationToken, EndConditions ConditionMask);

			void PushAVariableList(void);
			void PopAVariableList(void);
			void AddVariableToStack(VariableType* Variable);
			void RequiredVarialbe(const Token& Name);
			void RequiredVarialbe(const VariableList& List, const Token& Token);

			StructType* FindStructType(const String& Name);
			VariableType* FindVariableType(const String& Name);
			FunctionType* FindFunctionType(const String& Name);

			template<typename T>
			INLINE T* Allocate(void)
			{
				T* value = ReinterpretCast(T*, AllocateMemory(m_Allocator, sizeof(T)));
				Construct(value);
				return value;
			}

			template<typename T, typename... Parameters>
			INLINE T* Allocate(Parameters&&... Args)
			{
				T* value = ReinterpretCast(T*, AllocateMemory(m_Allocator, sizeof(T)));
				Construct(value, std::forward<Parameters>(Args)...);
				return value;
			}

			template<typename T>
			INLINE void Deallocate(T* Address)
			{
				Destruct(Address);
				DeallocateMemory(m_Allocator, Address);
			}

		public:
			static ProgramDataTypes GetPrimitiveDataType(const String& Name);

		private:
			AllocatorBase* m_Allocator;
			KeywordParseMap m_KeywordParsers;
			AttributeParseMap m_AttributeParsers;
			Parameters* m_Parameters;
			StructStack m_StructsStack;
			AttributeList m_AttributesList;
			VariablesStack m_VariablesStack;
		};
	}
}

#endif