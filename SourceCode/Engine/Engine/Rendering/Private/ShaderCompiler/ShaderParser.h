// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Rendering\Private\ShaderCompiler\DataTypes.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Stack.h>
#include <Containers\Map.h>
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
				class AssignmentStatement;
				class VariableStatement;

				typedef Stack<Token> TokenStack;

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
					typedef std::function<ParseResults(Token &DeclarationToken)> KeywordParseFunction;
					typedef std::shared_ptr<KeywordParseFunction> KeywordParseFunctionPtr;
					typedef Map<String, DataTypes> DataTypesNameMap;
					typedef Map<String, KeywordParseFunctionPtr> KeywordParseMap;

				public:
					ShaderParser(const String &Text);

					void Parse(VariableTypeList &Variables, FunctionTypeList &Functions);

				private:
					ParseResults ParseVariable(Token &DeclarationToken, VariableTypeList &Variables);

					ParseResults ParseFunction(Token &DeclarationToken, FunctionTypeList &Functions);

					ParseResults ParseVariable(Token &DeclarationToken, VariableType *Variable);

					ParseResults ParseParameter(Token &DeclarationToken, ParameterType *Parameter);

					ParseResults ParseFunction(FunctionType *Function);

					ParseResults ParseIfStatement(Token &DeclarationToken);
					ParseResults ParseSwitchStatement(Token &DeclarationToken);
					ParseResults ParseCaseStatement(Token &DeclarationToken);
					ParseResults ParseForStatement(Token &DeclarationToken);
					ParseResults ParseDoStatement(Token &DeclarationToken);
					ParseResults ParseWhileStatement(Token &DeclarationToken);
					ParseResults ParseContinueStatement(Token &DeclarationToken);
					ParseResults ParseBreakStatement(Token &DeclarationToken);
					ParseResults ParseReturnStatement(Token &DeclarationToken);
					ParseResults ParseDiscardStatement(Token &DeclarationToken);
					ParseResults ParseSemicolonStatement(Token &DeclarationToken);

					AssignmentStatement *ParseAssignment(TokenStack &Stack);
					VariableStatement *ParseVariable(TokenStack &Stack);

					DataTypes GetDataType(Token &DeclarationToken);

				private:
					DataTypesNameMap m_DataTypesName;
					KeywordParseMap m_KwywordParsers;
				};
			}
		}
	}
}

#endif