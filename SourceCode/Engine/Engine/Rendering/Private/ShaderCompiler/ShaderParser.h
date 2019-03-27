// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Rendering\DataType.h>
#include <Rendering\Private\ShaderCompiler\StatementsHolder.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <functional>
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
			namespace ShaderCompiler
			{
				class FunctionType;
				class VariableType;
				class ParameterType;
				class Statement;

				static const String VERTEX_ENTRY_POINT_NAME("vertexmain");
				static const String FRAGMENT_ENTRY_POINT_NAME("fragmentmain");

				class ShaderParser : private Tokenizer
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
						Semicolon = 1 << 0,
						Brace = 1 << 1,
						Comma = 1 << 2,
						Bracket = 1 << 4
					};

				public:
					typedef Vector<VariableType*> VariableTypeList;
					typedef Vector<FunctionType*> FunctionTypeList;
					typedef std::function<Statement*(Token &DeclarationToken)> KeywordParseFunction;
					typedef std::shared_ptr<KeywordParseFunction> KeywordParseFunctionPtr;
					typedef Map<String, KeywordParseFunctionPtr> KeywordParseMap;

				public:
					ShaderParser(AllocatorBase *Allocator, const String &Text);

					void Parse(VariableTypeList &Variables, FunctionTypeList &Functions);

				private:
					ParseResults ParseVariable(Token &DeclarationToken, VariableTypeList &Variables);
					ParseResults ParseFunction(Token &DeclarationToken, FunctionTypeList &Functions);
					ParseResults ParseFunctionParameter(Token &DeclarationToken, ParameterType *Parameter);

					Statement *ParseIfStatement(Token &DeclarationToken);
					Statement *ParseElseStatement(Token &DeclarationToken);
					Statement *ParseSwitchStatement(Token &DeclarationToken);
					Statement *ParseCaseStatement(Token &DeclarationToken);
					Statement *ParseForStatement(Token &DeclarationToken);
					Statement *ParseDoStatement(Token &DeclarationToken);
					Statement *ParseWhileStatement(Token &DeclarationToken);
					Statement *ParseContinueStatement(Token &DeclarationToken);
					Statement *ParseBreakStatement(Token &DeclarationToken);
					Statement *ParseReturnStatement(Token &DeclarationToken);
					Statement *ParseDiscardStatement(Token &DeclarationToken);
					Statement *ParseSemicolonStatement(Token &DeclarationToken);

					ParseResults ParseScopedStatements(StatementsHolder *StatementHolder);

					Statement *ParseVariableStatement(Token &DeclarationToken, EndConditions ConditionMask);

					Statement *ParseExpression(Token &DeclarationToken, EndConditions ConditionMask);
					Statement *ParseUnaryExpression(Token &DeclarationToken, EndConditions ConditionMask);
					Statement *ParseUnaryExpressionPrefix(Token &DeclarationToken, EndConditions ConditionMask);
					Statement *ParseUnaryOperatorExpression(Token &DeclarationToken, EndConditions ConditionMask);
					Statement *ParseArrayExpression(Token &DeclarationToken, EndConditions ConditionMask);
					Statement *ParseBinary(int8 LeftHandPrecedence, Statement *LeftHandStatement, EndConditions ConditionMask);

					Statement *ParseConstantStatement(Token &DeclarationToken);
					Statement *ParseVariableAccessStatement(Token &DeclarationToken);
					Statement *ParseMemberAccessStatement(Token &DeclarationToken, Statement *LeftStatement);
					Statement *ParseFunctionCallStatement(Token &DeclarationToken);

					bool IsEndCondition(Token Token, ShaderParser::EndConditions ConditionMask);

					template<typename T>
					INLINE T *Allocate(void)
					{
						T *value = ReinterpretCast(T*, AllocateMemory(m_Allocator, sizeof(T)));
						Construct(value);
						return value;
					}

					template<typename T>
					INLINE void Deallocate(T *Address)
					{
						DeallocateMemory(m_Allocator, Address);
					}

				public:
					static DataType::Types GetDataType(const String &Name);

				private:
					AllocatorBase *m_Allocator;
					KeywordParseMap m_KwywordParsers;
				};
			}
		}
	}
}

#endif