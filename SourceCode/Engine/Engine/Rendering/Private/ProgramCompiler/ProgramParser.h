// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_PARSER_H
#define PROGRAM_PARSER_H

#include <Rendering\ProgramInfo.h>
#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Containers\Stack.h>
#include <memory>

namespace Engine
{
	using namespace MemoryManagement;
	using namespace Utility::Lexer;
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
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

				using namespace Syntax;

				static cstr VERTEX_ENTRY_POINT_NAME("vertexmain");
				static cstr FRAGMENT_ENTRY_POINT_NAME("fragmentmain");

				const ProgramDataTypes ALLOWED_CONTEXT_FREE_DATA_TYPES[]{ ProgramDataTypes::Texture2D };

				class ProgramParser : private Tokenizer
				{
				private:
					enum class ParseResults
					{
						Approved = 0,
						Rejected,
						Failed
					};

					enum class EndConditions
					{
						None = 1 << 0,
						Semicolon = 1 << 1,
						Brace = 1 << 2,
						Comma = 1 << 3,
						Bracket = 1 << 4,
						SquareBracket = 1 << 5,
					};

					typedef std::function<Statement* (Token& DeclarationToken)> KeywordParseFunction;
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
					ProgramParser(AllocatorBase* Allocator, const String& Text, ErrorFunction OnError = nullptr);

					bool Parse(Parameters& Parameters);

				private:
					bool Parse(Parameters& Parameters, EndConditions ConditionMask);

					ParseResults ParseStruct(Token& DeclarationToken);
					ParseResults ParseVariable(Token& DeclarationToken);
					ParseResults ParseFunction(Token& DeclarationToken);
					ParseResults ParseFunctionParameter(Token& DeclarationToken, ParameterType* Parameter);

					DataTypeStatement* ParseDataType(Token& DeclarationToken);

					Statement* ParseIfStatement(Token& DeclarationToken);
					Statement* ParseElseStatement(Token& DeclarationToken);
					Statement* ParseSwitchStatement(Token& DeclarationToken);
					Statement* ParseCaseStatement(Token& DeclarationToken);
					Statement* ParseForStatement(Token& DeclarationToken);
					Statement* ParseDoStatement(Token& DeclarationToken);
					Statement* ParseWhileStatement(Token& DeclarationToken);
					Statement* ParseContinueStatement(Token& DeclarationToken);
					Statement* ParseBreakStatement(Token& DeclarationToken);
					Statement* ParseReturnStatement(Token& DeclarationToken);
					Statement* ParseDiscardStatement(Token& DeclarationToken);
					Statement* ParseSemicolonStatement(Token& DeclarationToken);

					ParseResults ParseScopedStatements(StatementItemHolder* StatementItemHolder);

					Statement* ParseVariableStatement(Token& DeclarationToken, EndConditions ConditionMask);

					Statement* ParseExpression(Token& DeclarationToken, EndConditions ConditionMask);
					Statement* ParseUnaryExpression(Token& DeclarationToken, EndConditions ConditionMask);
					Statement* ParseUnaryExpressionPrefix(Token& DeclarationToken, EndConditions ConditionMask);
					Statement* ParseUnaryOperatorExpression(Token& DeclarationToken, EndConditions ConditionMask);
					Statement* ParseArrayExpression(Token& DeclarationToken, EndConditions ConditionMask);
					Statement* ParseBinary(int8 LeftHandPrecedence, Statement* LeftHandStatement, EndConditions ConditionMask);

					Statement* ParseConstantStatement(Token& DeclarationToken);
					Statement* ParseVariableAccessStatement(Token& DeclarationToken);
					Statement* ParseArrayElementAccessStatement(Token& DeclarationToken, Statement* ArrayStatement);
					Statement* ParseMemberAccessStatement(Token& DeclarationToken, Statement* LeftStatement);
					Statement* ParseFunctionCallStatement(Token& DeclarationToken);

					bool IsEndCondition(Token& DeclarationToken, EndConditions ConditionMask);

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
	}
}

#endif