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
			class StructType;
			class VariableType;
			class FunctionType;
			class ParameterType;
			class DataTypeStatement;

			typedef Vector<StructType*> StructList;
			typedef Vector<VariableType*> VariableList;
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

		public:
			struct Parameters
			{
			public:
				StructList Structs;
				VariableList Variables;
				FunctionList Functions;
			};

		public:
			Parser(AllocatorBase* Allocator, const String& Text);

			void Parse(Parameters& Parameters);

		private:
			bool ParseStruct(const Token& DeclarationToken);
			bool ParseVariable(const Token& DeclarationToken);
			bool ParseFunction(const Token& DeclarationToken);
			bool ParseFunctionParameter(const Token& DeclarationToken, ParameterType* Parameter);

			DataTypeStatement* ParseDataType(const Token& DeclarationToken);

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
			Statement* ParseArrayElementAccessStatement(const Token& DeclarationToken, Statement* ArrayStatement);
			Statement* ParseMemberAccessStatement(const Token& DeclarationToken, Statement* LeftStatement);
			Statement* ParseFunctionCallStatement(const Token& DeclarationToken);

			bool IsEndCondition(const Token& DeclarationToken, EndConditions ConditionMask);

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
			Parameters* m_Parameters;
			Stack<StructType*> m_Structs;
		};
	}
}

#endif