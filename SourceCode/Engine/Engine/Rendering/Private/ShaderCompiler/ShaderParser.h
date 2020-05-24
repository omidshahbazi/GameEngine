// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Rendering\ShaderDataType.h>
#include <Rendering\ShaderInfo.h>
#include <Rendering\Private\ShaderCompiler\Syntax\StatementsHolder.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
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
				namespace Syntax
				{
					class FunctionType;
					class VariableType;
					class ParameterType;
					class Statement;
				}

				using namespace Syntax;

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
						None = 1 << 0,
						PreprocessorElse = 1 << 1,
						PreprocessorEndIf = 1 << 2,
						Semicolon = 1 << 3,
						Brace = 1 << 4,
						Comma = 1 << 5,
						Bracket = 1 << 6,
						SquareBracket = 1 << 7,
					};

					typedef std::function<Statement* (Token& DeclarationToken)> KeywordParseFunction;
					typedef std::shared_ptr<KeywordParseFunction> KeywordParseFunctionPtr;
					typedef Map<String, KeywordParseFunctionPtr> KeywordParseMap;

				public:
					typedef Vector<VariableType*> VariableTypeList;
					typedef Vector<FunctionType*> FunctionTypeList;
					typedef std::function<bool(const String&, String& Source)> IncludeFunction;

					struct Parameters
					{
					public:
						IncludeFunction IncludeFunction;
						ShaderInfo::DefineList Defines;
						VariableTypeList Variables;
						FunctionTypeList Functions;
					};

				public:
					ShaderParser(AllocatorBase* Allocator, const String& Text);

					void Parse(Parameters& Parameters);

				private:
					void Parse(Parameters& Parameters, EndConditions ConditionMask);

					ParseResults ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters);
					ParseResults ParseVariable(Token& DeclarationToken, Parameters& Parameters);
					ParseResults ParseFunction(Token& DeclarationToken, Parameters& Parameters);
					ParseResults ParseFunctionParameter(Token& DeclarationToken, ParameterType* Parameter);

					ParseResults ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove);

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

					ParseResults ParseScopedStatements(StatementsHolder* StatementHolder);

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

					bool IsEndCondition(Token Token, ShaderParser::EndConditions ConditionMask);

					template<typename T>
					INLINE T* Allocate(void)
					{
						T* value = ReinterpretCast(T*, AllocateMemory(m_Allocator, sizeof(T)));
						Construct(value);
						return value;
					}

					template<typename T>
					INLINE void Deallocate(T* Address)
					{
						DeallocateMemory(m_Allocator, Address);
					}

				public:
					static ShaderDataType::Types GetDataType(const String& Name);

				private:
					AllocatorBase* m_Allocator;
					KeywordParseMap m_KeywordParsers;
				};
			}
		}
	}
}

#endif