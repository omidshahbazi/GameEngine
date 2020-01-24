// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool\HeaderParser.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Debugging;

	namespace WrapperTool
	{
		struct ParamaterInfo
		{
		public:
			bool IsPointer;
			String Type;
			String Name;
		};

		typedef Vector<ParamaterInfo> ParamaterInfoList;

		String GetUniqueFunctionName(const String& FullQualified, const String& Name)
		{
			return FullQualified.Replace("::", "_") + "_" + Name;
		}

		bool HeaderParser::Parse(StringStream& Stream)
		{
			Tokenizer::Parse();

			do
			{
				Token token;
				if (!GetToken(token))
					break;

				if (!CompileDeclaration(Stream, token))
				{
					Debug::LogError((TEXT("'") + token.GetIdentifier() + "': Bad command or expression").GetValue());
					return false;
				}
			} while (true);

			return true;
		}

		bool HeaderParser::CompileDeclaration(StringStream& Stream, Token& DeclarationToken)
		{
			AccessSpecifiers access = GetAccessSpecifier(DeclarationToken);

			if (DeclarationToken.Matches(WRAPPER_OBJECT_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileTypeDeclaration(Stream, DeclarationToken))
					return false;
			}
			//else if (DeclarationToken.Matches(CLASS_TEXT, Token::SearchCases::CaseSensitive) || DeclarationToken.Matches(STRUCT_TEXT, Token::SearchCases::CaseSensitive))
			//{
			//	if (!CompileForwardDeclaration(Stream, DeclarationToken))
			//		return false;
			//}
			else if (DeclarationToken.Matches(USING_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileUsingNamespaceDeclaration(Stream, DeclarationToken))
					return false;
			}
			else if (DeclarationToken.Matches(NAMESPACE_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileNamespace(Stream, DeclarationToken))
					return false;
			}
			else if (DeclarationToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
			{
				if (MatchSymbol(SEMICOLON))
				{
					//if (m_CurrentDataStructure != nullptr && m_BlockLevel == m_CurrentDataStructure->GetBlockLevel())
					//	m_CurrentDataStructure = (MetaDataStructure*)m_CurrentDataStructure->GetTopNest();
				}
				else
				{
					RemoveLastQualifier();
					Stream << CLOSE_BRACKET << "\n";
				}
			}

			return true;
		}

		bool HeaderParser::CompileTypeDeclaration(StringStream& Stream, Token& DeclarationToke)
		{
			if (!ReadSpecifier(DeclarationToke))
				return false;

			if (!RequiredToken(CLASS_TEXT) && !RequiredToken(STRUCT_TEXT))
				return false;

			bool isStruct = DeclarationToke.Matches(STRUCT_TEXT, Token::SearchCases::IgnoreCase);

			Token typeNameToken;
			if (!ReadTypeName(typeNameToken))
				return false;

			const String& typeName = typeNameToken.GetIdentifier();

			AccessSpecifiers lastAccessSpecifier = (isStruct ? AccessSpecifiers::Public : AccessSpecifiers::NonPublic);

			int scoreCount = 1;

			const String fullQualified = GetQualifiers() + "::" + typeName;

			while (scoreCount != 0)
			{
				Token token;
				if (!GetToken(token))
					break;

				AccessSpecifiers access = GetAccessSpecifier(token);
				if (access != AccessSpecifiers::None)
				{
					lastAccessSpecifier = access;
					continue;
				}

				if (token.Matches(OPEN_BRACKET, Token::SearchCases::IgnoreCase))
				{
					++scoreCount;
				}
				else if (token.Matches(CLOSE_BRACKET, Token::SearchCases::IgnoreCase))
				{
					--scoreCount;
				}
				else if (token.Matches(SINGLETON_DECLARATION_TEXT, Token::SearchCases::IgnoreCase))
				{
					Stream << m_ModuleAPI << " ";
					Stream << typeName << "* " << GetUniqueFunctionName(fullQualified, "GetInstance") << "(void)\n";
					Stream << OPEN_BRACKET << "\n";
					Stream << "return " << typeName << "::GetInstance();\n";
					Stream << CLOSE_BRACKET << "\n";

					if (!RequiredToken(OPEN_BRACE))
						return false;

					Token nameToken;
					GetToken(nameToken);

					if (!RequiredToken(CLOSE_BRACE))
						return false;

					continue;
				}
				else if (lastAccessSpecifier == AccessSpecifiers::Public && CompileFunctionDeclaration(Stream, fullQualified, typeName, token))
				{

				}
			}
		}

		bool HeaderParser::CompileFunctionDeclaration(StringStream& Stream, const String& FullQualifiedTypeName, const String& TypeName, Token& DeclarationToken)
		{
			StringList returnTypeIdentifiers;
			String name;
			ParamaterInfoList parameters;

			returnTypeIdentifiers.Add(DeclarationToken.GetIdentifier());

			bool isFunction = false;

			while (true)
			{
				Token token;
				if (!GetToken(token))
					return false;

				if (RequiredToken(SEMICOLON))
					break;
				else if (RequiredToken(OPEN_BRACE))
				{
					if (returnTypeIdentifiers.GetSize() == 0)
						break;

					name = token.GetIdentifier();

					Stream << m_ModuleAPI << " ";

					for each (auto t in returnTypeIdentifiers)
						Stream << t << " ";

					Stream << GetUniqueFunctionName(FullQualifiedTypeName, name) << OPEN_BRACE;
					Stream << TypeName << STAR << "Instance";

					while (true)
					{
						Token tempParamToken;
						if (!GetToken(tempParamToken))
							return false;

						if (tempParamToken.Matches(CLOSE_BRACE, Token::SearchCases::IgnoreCase))
							break;

						if (MatchSymbol(CLOSE_BRACE))
							break;

						if (tempParamToken.Matches(COMMA, Token::SearchCases::IgnoreCase))
							continue;

						ParamaterInfo parameter;
						parameter.IsPointer = false;
						UngetToken(tempParamToken);
						while (true)
						{
							Token paramToken;
							if (!GetToken(paramToken))
								return false;

							if (paramToken.Matches(CONST_TEXT, Token::SearchCases::IgnoreCase))
								continue;

							if (paramToken.Matches(STAR, Token::SearchCases::IgnoreCase))
							{
								parameter.IsPointer = true;
								continue;
							}

							if (paramToken.Matches(COMMA, Token::SearchCases::IgnoreCase))
								break;

							if (paramToken.Matches(CLOSE_BRACE, Token::SearchCases::IgnoreCase))
							{
								UngetToken(paramToken);
								break;
							}

							if (parameter.Type.GetLength() == 0)
								parameter.Type = paramToken.GetIdentifier();
							else
								parameter.Name = paramToken.GetIdentifier();
						}

						parameters.Add(parameter);
					}

					for each (const auto & parameter in parameters)
					{
						Stream << COMMA;

						Stream << parameter.Type;

						if (parameter.IsPointer)
							Stream << STAR;

						Stream << ' ' << parameter.Name;
					}

					Stream << CLOSE_BRACE << "\n";
					Stream << OPEN_BRACKET << "\n";

					Stream << "\t";

					if (returnTypeIdentifiers.GetSize() != 1 || returnTypeIdentifiers[0] != VOID_TEXT)
						Stream << "return ";

					Stream << "Instance->" << name << OPEN_BRACE;

					for (int i = 0; i < parameters.GetSize(); ++i)
					{
						if (i != 0)
							Stream << COMMA;

						Stream << parameters[i].Name;
					}

					Stream << CLOSE_BRACE << SEMICOLON << "\n";
					Stream << CLOSE_BRACKET << "\n";

					isFunction = true;
				}
				else if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					return false;
				else if (token.Matches(TILDE, Token::SearchCases::IgnoreCase))
					return false;
				else if (token.Matches(COLON, Token::SearchCases::IgnoreCase))
					return false;
				else
					returnTypeIdentifiers.Add(token.GetIdentifier());
			}

			if (!isFunction)
			{
				UngetToken(DeclarationToken);
				Token temp;
				GetToken(temp);
			}
			else
				(RequiredToken(CLOSE_BRACKET) || RequiredToken(SEMICOLON));

			return isFunction;
		}

		bool HeaderParser::CompileUsingNamespaceDeclaration(StringStream& Stream, Token& DeclarationToken)
		{
			if (!RequiredToken(NAMESPACE_TEXT))
				return false;

			Stream << USING_TEXT << " " << NAMESPACE_TEXT << " ";

			while (true)
			{
				Token token;
				if (!GetToken(token))
					return false;

				if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					break;

				if (token.Matches(DOUBLE_COLON, Token::SearchCases::IgnoreCase))
				{
					Stream << DOUBLE_COLON;

					continue;
				}

				Stream << token.GetIdentifier();
			}

			Stream << SEMICOLON << "\n";

			return true;
		}

		bool HeaderParser::CompileNamespace(StringStream& Stream, Token& DeclarationToken)
		{
			Token nameToken;
			if (!GetToken(nameToken))
				return false;

			if (!MatchSymbol(OPEN_BRACKET))
				return false;

			AddQualifier(nameToken.GetIdentifier());

			Stream << NAMESPACE_TEXT << " " << nameToken.GetIdentifier() << "\n";
			Stream << OPEN_BRACKET;

			return true;
		}

		//bool HeaderParser::CompileForwardDeclaration(StringStream& Stream, Token& DeclarationToken)
		//{
		//	return true;
		//}

		HeaderParser::AccessSpecifiers HeaderParser::GetAccessSpecifier(Token& Token)
		{
			if (Token.Matches(PUBLIC_TEXT, Token::SearchCases::IgnoreCase))
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

		bool HeaderParser::ReadTypeName(Token& ReadToken)
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

		void HeaderParser::AddQualifier(const String& Name)
		{
			m_Qualifiers.Add(Name);
		}

		void HeaderParser::RemoveLastQualifier(void)
		{
			if (m_Qualifiers.GetSize() != 0)
				m_Qualifiers.RemoveAt(m_Qualifiers.GetSize() - 1);
		}

		String HeaderParser::GetQualifiers(void) const
		{
			String str;

			bool isFirst = true;
			for each (auto & name in m_Qualifiers)
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
