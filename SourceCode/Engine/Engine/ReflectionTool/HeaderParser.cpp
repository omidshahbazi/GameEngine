// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <ReflectionTool\HeaderParser.h>
#include <ReflectionTool\MetaDataStructure.h>
#include <ReflectionTool\MetaEnum.h>
#include <ReflectionTool\MetaConstructor.h>
#include <ReflectionTool\MetaFunction.h>
#include <ReflectionTool\MetaProperty.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Debugging;

	namespace ReflectionTool
	{
		void HeaderParser::Parse(Type::TypesList &Types)
		{
			CodePageParser::Parse(Types);

			m_CurrentDataStructure = nullptr;
			m_BlockLevel = 0;

			//String str;
			do
			{
				Token token;
				if (!GetToken(token))
					return;
				else if (!CompileDeclaration(Types, token))
					Debug::LogError((TEXT("'") + token.GetIdentifier() + "': Bad command or expression").GetValue());

				//str += token.GetIdentifier() + "\n";
			} while (true);
		}


		bool HeaderParser::CompileDeclaration(Type::TypesList &Types, Token &DelarationToken)
		{
			AccessSpecifiers access = GetAccessSpecifier(DelarationToken);

			if (access != AccessSpecifiers::None)
			{
				if (m_CurrentDataStructure == nullptr)
				{
					Debug::LogError("Access specifier detected out of nest");
					return false;
				}

				m_CurrentDataStructure->SetLastAccessSpecifier(access);
			}
			else if (DelarationToken.Matches(REFLECTION_OBJECT_TEXT, Token::SearchCases::CaseSensitive))
				CompileTypeDeclaration(DelarationToken, Types);
			else if (m_CurrentDataStructure != nullptr && DelarationToken.Matches(m_CurrentDataStructure->GetName(), Token::SearchCases::CaseSensitive))
				CompileConstructorDeclaration();
			else if (DelarationToken.Matches(REFLECTION_ENUM_TEXT, Token::SearchCases::CaseSensitive))
				CompileEnumDeclaration(Types);
			else if (DelarationToken.Matches(REFLECTION_FUNCTION_TEXT, Token::SearchCases::CaseSensitive))
				CompileFunctionDeclaration();
			else if (DelarationToken.Matches(REFLECTION_PROPERTY_TEXT, Token::SearchCases::CaseSensitive))
				CompileVariableDeclaration();
			else if (DelarationToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive))
				AddBlockLevel();
			else if (DelarationToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
			{
				m_BlockLevel--;

				if (MatchSymbol(SEMI_COLON) && m_CurrentDataStructure != nullptr && m_BlockLevel == m_CurrentDataStructure->GetBlockLevel())
					m_CurrentDataStructure = (MetaDataStructure*)m_CurrentDataStructure->GetTopNest();
			}

			return true;
		}


		void HeaderParser::CompileTypeDeclaration(const Token &Declaration, Type::TypesList &Types)
		{
			MetaDataStructure *type = new MetaDataStructure(m_CurrentDataStructure);
			ReadSpecifiers(type, "Class");

			bool hasParent = false;
			while (true)
			{
				Token token;
				if (!GetToken(token, true))
					break;

				if (MatchSymbol(SEMI_COLON))
				{
					delete type;
					return;
				}
				else if ((hasParent = MatchSymbol(COLON)) || MatchSymbol(OPEN_BRACKET))
				{
					type->SetName(token.GetIdentifier());
					break;
				}

				if (token.Matches(STRUCT, Token::SearchCases::CaseSensitive))
					type->SetLastAccessSpecifier(AccessSpecifiers::Public);
			}

			if (hasParent)
			{
				uint8 parentCount = 0;

				while (!MatchSymbol(OPEN_BRACKET))
				{
					if (parentCount++ > 0)
						RequireSymbol(COMMA, "missing in parent list");

					Token token;
					GetToken(token);

					AccessSpecifiers access = MetaDataStructure::ParseAccessSpecifier(token);

					if (access == AccessSpecifiers::None)
						type->AddParentName(token.GetIdentifier());
					else
					{
						Token token;
						GetToken(token);
						type->AddParentName(token.GetIdentifier());
					}
				}
			}

			Token objectDeclMacroToken;
			GetToken(objectDeclMacroToken);
			if (!objectDeclMacroToken.Matches(type->GetDeclarationMacroName(), Token::SearchCases::CaseSensitive))
			{
				AddBlockLevel();
				delete type;
				return;
			}

			if (m_CurrentDataStructure == nullptr)
				Types.Add(type);
			else
				m_CurrentDataStructure->AddSubType(type);
			m_CurrentDataStructure = type;
			m_CurrentDataStructure->SetBlockLevel(m_BlockLevel);

			AddBlockLevel();
		}


		void HeaderParser::CompileEnumDeclaration(Type::TypesList &Types)
		{
			MetaEnum *type = new MetaEnum();
			ReadSpecifiers(type, "enum");

			Token nameToken;
			while (GetToken(nameToken, true))
			{
				if (MatchSymbol(SEMI_COLON))
					return;
				if (MatchSymbol(OPEN_BRACKET))
					break;

				nameToken.SetName("");
				nameToken.SetIdentifier("");
			}

			type->SetName(nameToken.GetIdentifier());

			uint8 membersCount = 0;
			do
			{
				if (MatchSymbol(COMMA))
					continue;

				bool bracketPresent = false;
				Token member;
				for (int i = 0; true; i++)
				{
					if (MatchSymbol(COMMA))
						break;
					else if (bracketPresent = MatchSymbol(CLOSE_BRACKET))
					{
						if (i == 0)
							goto Finalize;
						break;
					}

					GetToken(member);
				}

				type->AddItem(member.GetIdentifier());
				membersCount++;

				if (bracketPresent)
					break;

			} while (true);

			//if (m_CurrentDataStructure == nullptr)
			Types.Add(type);
			//else
			//	m_CurrentDataStructure->AddSubType(type);

			if (membersCount == 0)
				Finalize:
			delete type;
		}


		void HeaderParser::CompileConstructorDeclaration(void)
		{
			MetaConstructor *ctor = new MetaConstructor(m_CurrentDataStructure);
			ctor->SetName(m_CurrentDataStructure->GetName());

			if (!MatchSymbol(OPEN_BRACE))
			{
				delete ctor;
				return;
			}

			while (true)
			{
				DataType paramDataType;
				if (!GetDataType(paramDataType))
					break;

				Token paramName;
				GetToken(paramName);

				ctor->AddParameter(paramDataType, paramName.GetIdentifier());
			}

			m_CurrentDataStructure->AddConstructor(ctor);
		}


		void HeaderParser::CompileFunctionDeclaration(void)
		{
			MetaFunction *func = new MetaFunction(m_CurrentDataStructure);

			ReadSpecifiers(func, "function");

			DataType returnType;
			if (!GetDataType(returnType))
			{
				delete func;
				return;
			}
			func->SetReturnType(returnType);

			Token nameToken;
			GetToken(nameToken);
			func->SetName(nameToken.GetIdentifier());

			if (!MatchSymbol(OPEN_BRACE))
			{
				delete func;
				return;
			}

			while (true)
			{
				DataType paramDataType;
				if (!GetDataType(paramDataType))
					break;

				Token paramName;
				GetToken(paramName);

				func->AddParameter(paramDataType, paramName.GetIdentifier());
			}

			if (MatchIdentifier(CONST))
				func->SetIsConst(true);

			m_CurrentDataStructure->AddFunction(func);
		}


		void HeaderParser::CompileVariableDeclaration(void)
		{
			MetaProperty *property = new MetaProperty(m_CurrentDataStructure);

			ReadSpecifiers(property, "property");

			DataType dataType;
			GetDataType(dataType);
			if (dataType.GetValueType() == ValueTypes::None)
			{
				delete property;
				return;
			}

			Token nameToken;
			GetToken(nameToken);

			property->SetName(nameToken.GetIdentifier());
			property->SetDataType(dataType);

			m_CurrentDataStructure->AddProperty(property);
		}


		AccessSpecifiers HeaderParser::GetAccessSpecifier(Token &Token)
		{
			AccessSpecifiers access = MetaDataStructure::ParseAccessSpecifier(Token);

			if (access != AccessSpecifiers::None)
				RequireSymbol(COLON, "after " + Token.GetIdentifier(), SymbolParseOptions::Normal);

			return access;
		}
	}
}
