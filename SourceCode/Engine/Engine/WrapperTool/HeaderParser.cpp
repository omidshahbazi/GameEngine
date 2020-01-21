// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool\HeaderParser.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Debugging;

	namespace WrapperTool
	{
		//void HeaderParser::Parse(TypesList& Types)
		//{
		//	Tokenizer::Parse();

		//	m_BlockLevel = 0;

		//	//String str;
		//	do
		//	{
		//		Token token;
		//		if (!GetToken(token))
		//			return;
		//		else if (!CompileDeclaration(Types, token))
		//			Debug::LogError((TEXT("'") + token.GetIdentifier() + "': Bad command or expression").GetValue());

		//		//str += token.GetIdentifier() + "\n";
		//	} while (true);
		//}

	/*	bool HeaderParser::CompileDeclaration(TypesList& Types, Token& DelarationToken)
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
			else if (DelarationToken.Matches(REFLECTION_STRUCT_TEXT, Token::SearchCases::CaseSensitive))
				CompileStructDeclaration(DelarationToken, Types);
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

				if (MatchSymbol(SEMICOLON))
				{
					if (m_CurrentDataStructure != nullptr && m_BlockLevel == m_CurrentDataStructure->GetBlockLevel())
						m_CurrentDataStructure = (MetaDataStructure*)m_CurrentDataStructure->GetTopNest();
				}
				else
				{
					if (m_Namespaces.GetSize() != 0)
						m_Namespaces.RemoveAt(0);
				}
			}
			else if (DelarationToken.Matches(NAMESPACE, Token::SearchCases::CaseSensitive))
				AddNamespace();

			return true;
		}*/

		void HeaderParser::AddNamespace(void)
		{
			Token nameToken;
			GetToken(nameToken);

			if (!MatchSymbol(OPEN_BRACKET))
				return;

			//m_Namespaces.Add(nameToken.GetIdentifier());
		}

		String HeaderParser::GetNamespaces(void) const
		{
			String str;

			//bool isFirst = true;
			//for each (auto & name in m_Namespaces)
			//{
			//	if (!isFirst)
			//		str += "::";

			//	isFirst = false;

			//	str += name;
			//}

			return str;
		}
	}
}
