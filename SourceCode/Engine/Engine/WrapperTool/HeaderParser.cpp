// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool\HeaderParser.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Debugging;

	namespace WrapperTool
	{
		bool HeaderParser::Parse(StringStream& Stream)
		{
			Tokenizer::Parse();

			do
			{
				Token token;
				if (!GetToken(token))
					break;
				else if (!CompileDeclaration(Stream, token))
				{
					Debug::LogError((TEXT("'") + token.GetIdentifier() + "': Bad command or expression").GetValue());
					return false;
				}
			} while (true);

			return true;
		}

		bool HeaderParser::CompileDeclaration(StringStream& Stream, Token& DelarationToken)
		{
			AccessSpecifiers access = GetAccessSpecifier(DelarationToken);

			if (DelarationToken.Matches(WRAPPER_OBJECT_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileTypeDeclaration(Stream, DelarationToken))
					return false;
			}
			else if (DelarationToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
			{
				if (MatchSymbol(SEMICOLON))
				{
					//if (m_CurrentDataStructure != nullptr && m_BlockLevel == m_CurrentDataStructure->GetBlockLevel())
					//	m_CurrentDataStructure = (MetaDataStructure*)m_CurrentDataStructure->GetTopNest();
				}
				else
				{
					RemoveNamespace();
				}
			}
			else if (DelarationToken.Matches(NAMESPACE_TEXT, Token::SearchCases::CaseSensitive))
				AddNamespace();

			return true;
		}

		bool HeaderParser::CompileTypeDeclaration(StringStream& Stream, Token& DeclarationToke)
		{
			if (!ReadSpecifier(DeclarationToke))
				return false;

			if (!RequiredToken("class") && !RequiredToken("struct"))
				return false;

			Token typeNameToken;
			if (!ReadTypeName(typeNameToken))
				return false;
		}

		HeaderParser::AccessSpecifiers HeaderParser::GetAccessSpecifier(Token& Token)
		{
			if (Token.Matches("public", Token::SearchCases::IgnoreCase))
				if (RequiredToken(COLON))
					return AccessSpecifiers::Public;

			if (RequiredToken(COLON))
				return AccessSpecifiers::NonPublic;

			return AccessSpecifiers::None;
		}

		bool HeaderParser::ReadSpecifier(Token& DeclarationToken)
		{
			if (!RequiredToken(OPEN_BRACE))
				return false;

			if (!RequiredToken(CLOSE_BRACE))
				return false;

			return true;
		}

		bool HeaderParser::ReadTypeName(Token &ReadToken)
		{
			Token name;
			if (!GetToken(name))
				return false;

			Token openBracketToken;
			if (!GetToken(openBracketToken))
				return false;

			if (openBracketToken.Matches(OPEN_BRACKET, Token::SearchCases::IgnoreCase))
			{
				ReadToken = name;
				return true;
			}

			ReadToken = openBracketToken;

			return RequiredToken(OPEN_BRACKET);
		}

		bool HeaderParser::RequiredToken(const String& Value)
		{
			Token token;
			if (!GetToken(token))
				return false;

			if (token.Matches(Value, Token::SearchCases::IgnoreCase))
				return true;

			UngetToken(token);

			return false;
		}

		void HeaderParser::AddNamespace(void)
		{
			Token nameToken;
			if (!GetToken(nameToken))
				return;

			if (!MatchSymbol(OPEN_BRACKET))
				return;

			m_Namespaces.Add(nameToken.GetIdentifier());
		}

		void HeaderParser::RemoveNamespace(void)
		{
			if (m_Namespaces.GetSize() != 0)
				m_Namespaces.RemoveAt(0);
		}

		String HeaderParser::GetNamespaces(void) const
		{
			String str;

			bool isFirst = true;
			for each (auto & name in m_Namespaces)
			{
				if (!isFirst)
					str += "::";

				isFirst = false;

				str += name;
			}

			return str;
		}
	}
}
