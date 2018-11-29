// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Rendering\Private\ShaderCompiler\DataTypes.h>
#include <Rendering\Private\ShaderCompiler\StatementsHolder.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Containers\Stack.h>
#include <functional>
#include <memory>

namespace Engine
{
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

				public:
					typedef Vector<VariableType*> VariableTypeList;
					typedef Vector<FunctionType*> FunctionTypeList;
					typedef std::function<Statement*(Token &DeclarationToken)> KeywordParseFunction;
					typedef std::shared_ptr<KeywordParseFunction> KeywordParseFunctionPtr;
					typedef Map<String, KeywordParseFunctionPtr> KeywordParseMap;
					typedef Stack<Token> TokenStack;

				public:
					ShaderParser(const String &Text);

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

					Statement *ParseExpression(Token &DeclarationToken);
					Statement *ParseExpressionStack(TokenStack &Stack);

					Statement *ParseOperatorStatement(Token &DeclarationToken, TokenStack &Stack);
					Statement *ParseConstantStatement(Token &DeclarationToken, TokenStack &Stack);
					Statement *ParseMemberAccessStatement(Token &DeclarationToken, TokenStack &Stack);
					Statement *ParseFunctionCallStatement(Token &DeclarationToken, TokenStack &Stack);
					Statement *ParseVariableStatement(Token &DeclarationToken, TokenStack &Stack);

					Statement *ReverseMemberAccessStatement(Token &DeclarationToken, TokenStack &Stack);

				public:
					static DataTypes GetDataType(const String &Name);

				private:
					KeywordParseMap m_KwywordParsers;
				};
			}
		}
	}
}

#endif