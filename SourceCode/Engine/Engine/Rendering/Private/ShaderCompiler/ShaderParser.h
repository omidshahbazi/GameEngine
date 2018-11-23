// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Rendering\Private\ShaderCompiler\DataTypes.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
//#include <Containers\Stack.h>
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
				class Statement;

				//typedef Stack<Token> TokenStack;

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

				public:
					ShaderParser(const String &Text);

					void Parse(VariableTypeList &Variables, FunctionTypeList &Functions);

				private:
					ParseResults ParseVariable(Token &DeclarationToken, VariableTypeList &Variables);

					ParseResults ParseFunction(Token &DeclarationToken, FunctionTypeList &Functions);

					ParseResults ParseVariable(Token &DeclarationToken, VariableType *Variable);

					ParseResults ParseParameter(Token &DeclarationToken, ParameterType *Parameter);

					ParseResults ParseFunction(FunctionType *Function);

					Statement *ParseIfStatement(Token &DeclarationToken);
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
					Statement *ParseFunctionCallStatement(Token &DeclarationToken);
					Statement *ParseMemberAccessStatement(Token &DeclarationToken);
					Statement *ParseConstantStatement(Token &DeclarationToken);
					Statement *ParseAssignmentStatement(Token &DeclarationToken);
					Statement *ParseVariableStatement(Token &DeclarationToken);

					Statement *ParseStatement(Token &DeclarationToken);

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