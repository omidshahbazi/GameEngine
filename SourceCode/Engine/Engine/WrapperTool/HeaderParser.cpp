// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool\HeaderParser.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Debugging;

	namespace WrapperTool
	{
		String GetUniqueFunctionName(const String& FullQualified, const String& Name)
		{
			return FullQualified.Replace("::", "_") + "_" + Name;
		}

		String GetNativePointerName(String TypeName)
		{
			return "native" + TypeName;
		}

		bool HeaderParser::Parse(StringStream& HeaderStream, StringStream& CSStream)
		{
			Tokenizer::Parse();

			do
			{
				Token token;
				if (!GetToken(token))
					break;

				if (!CompileDeclaration(HeaderStream, token))
				{
					Debug::LogError((TEXT("'") + token.GetIdentifier() + "': Bad command or expression").GetValue());
					return false;
				}
			} while (true);

			CSStream << m_CSNameUsingNamespaces.GetBuffer();
			CSStream << m_CSTypeDeclaration.GetBuffer();

			return true;
		}

		bool HeaderParser::CompileDeclaration(StringStream& HeaderStream, Token& DeclarationToken)
		{
			AccessSpecifiers access = GetAccessSpecifier(DeclarationToken);

			if (DeclarationToken.Matches(WRAPPER_OBJECT_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileTypeDeclaration(HeaderStream, DeclarationToken))
					return false;
			}
			//else if (DeclarationToken.Matches(CLASS_TEXT, Token::SearchCases::CaseSensitive) || DeclarationToken.Matches(STRUCT_TEXT, Token::SearchCases::CaseSensitive))
			//{
			//	if (!CompileForwardDeclaration(HeaderStream, CSStream, DeclarationToken))
			//		return false;
			//}
			else if (DeclarationToken.Matches(USING_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileUsingNamespaceDeclaration(HeaderStream, DeclarationToken))
					return false;
			}
			else if (DeclarationToken.Matches(NAMESPACE_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileNamespace(HeaderStream, DeclarationToken))
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

					HeaderStream << CLOSE_BRACKET << NEWLINE;
					m_CSTypeDeclaration << CLOSE_BRACKET << NEWLINE;
				}
			}

			return true;
		}

		bool HeaderParser::CompileTypeDeclaration(StringStream& HeaderStream, Token& DeclarationToke)
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

			m_CSTypeDeclaration << PUBLIC_TEXT << SPACE << CLASS_TEXT << SPACE << typeName << NEWLINE;
			m_CSTypeDeclaration << OPEN_BRACKET << NEWLINE;

			AccessSpecifiers lastAccessSpecifier = (isStruct ? AccessSpecifiers::Public : AccessSpecifiers::NonPublic);

			int scoreCount = 1;

			const String fullQualifiedTypeName = GetQualifiers() + "::" + typeName;

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
					StringList returnTypeIdentifiers;
					returnTypeIdentifiers.Add(typeName);
					returnTypeIdentifiers.Add(STAR);

					ParameterInfoList parameters;

					const String getInstanceFunctionName = "GetInstance";

					AddExportFunction(HeaderStream, fullQualifiedTypeName, typeName, getInstanceFunctionName, returnTypeIdentifiers, parameters, false);

					AddImportFunction(m_CSTypeDeclaration, getInstanceFunctionName, GetUniqueFunctionName(fullQualifiedTypeName, getInstanceFunctionName), returnTypeIdentifiers, parameters, false);

					m_CSTypeDeclaration << PRIVATE_TEXT << SPACE << CS_POINTER_TEXT << SPACE << GetNativePointerName(typeName) << EQUAL << "0" << SEMICOLON << NEWLINE;
					m_CSTypeDeclaration << PRIVATE_TEXT << SPACE << STATIC_TEXT << SPACE << typeName << " instance" << EQUAL << "new " << typeName << OPEN_BRACE << getInstanceFunctionName << OPEN_BRACE << CLOSE_BRACE << CLOSE_BRACE << SEMICOLON << NEWLINE;
					m_CSTypeDeclaration << PUBLIC_TEXT << SPACE << STATIC_TEXT << SPACE << typeName << " Instance" << OPEN_BRACKET << "get" << OPEN_BRACKET << "return instance" << SEMICOLON << CLOSE_BRACKET << CLOSE_BRACKET << NEWLINE;
					m_CSTypeDeclaration << PRIVATE_TEXT << SPACE << typeName << OPEN_BRACE << CS_POINTER_TEXT << SPACE << GetNativePointerName(typeName) << CLOSE_BRACE << OPEN_BRACKET << THIS_TEXT << DOT << GetNativePointerName(typeName) << EQUAL << GetNativePointerName(typeName) << SEMICOLON << CLOSE_BRACKET << NEWLINE;

					if (!RequiredToken(OPEN_BRACE))
						return false;

					Token nameToken;
					if (!GetToken(nameToken))
						return false;

					if (!RequiredToken(CLOSE_BRACE))
						return false;

					continue;
				}
				else if (lastAccessSpecifier == AccessSpecifiers::Public)
				{
					if (!CompileFunctionDeclaration(HeaderStream, fullQualifiedTypeName, typeName, token))
						return false;
				}
			}

			m_CSTypeDeclaration << CLOSE_BRACKET << NEWLINE;
		}

		bool HeaderParser::CompileFunctionDeclaration(StringStream& HeaderStream, const String& FullQualifiedTypeName, const String& TypeName, Token& DeclarationToken)
		{
			StringList returnTypeIdentifiers;
			String name;
			ParameterInfoList parameters;

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

					AddExportFunction(HeaderStream, FullQualifiedTypeName, TypeName, name, returnTypeIdentifiers, parameters, true);

					AddImportFunction(m_CSTypeDeclaration, name, GetUniqueFunctionName(FullQualifiedTypeName, name), returnTypeIdentifiers, parameters, true);

					isFunction = true;
				}
				else if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					return true;
				else if (token.Matches(TILDE, Token::SearchCases::IgnoreCase))
					return true;
				else if (token.Matches(COLON, Token::SearchCases::IgnoreCase))
					return true;
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

			return true;
		}

		bool HeaderParser::CompileUsingNamespaceDeclaration(StringStream& HeaderStream, Token& DeclarationToken)
		{
			if (!RequiredToken(NAMESPACE_TEXT))
				return false;

			HeaderStream << USING_TEXT << SPACE << NAMESPACE_TEXT << SPACE;
			m_CSNameUsingNamespaces << USING_TEXT << SPACE;

			const String qualifiers = GetQualifiers();
			if (qualifiers.GetLength() != 0)
				m_CSNameUsingNamespaces << qualifiers << DOT;

			while (true)
			{
				Token token;
				if (!GetToken(token))
					return false;

				if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					break;

				if (token.Matches(DOUBLE_COLON, Token::SearchCases::IgnoreCase))
				{
					HeaderStream << DOUBLE_COLON;
					m_CSNameUsingNamespaces << DOT;

					continue;
				}

				HeaderStream << token.GetIdentifier();
				m_CSNameUsingNamespaces << token.GetIdentifier();
			}

			HeaderStream << SEMICOLON << NEWLINE;
			m_CSNameUsingNamespaces << SEMICOLON << NEWLINE;

			return true;
		}

		bool HeaderParser::CompileNamespace(StringStream& HeaderStream, Token& DeclarationToken)
		{
			Token nameToken;
			if (!GetToken(nameToken))
				return false;

			if (!MatchSymbol(OPEN_BRACKET))
				return false;

			AddQualifier(nameToken.GetIdentifier());

			HeaderStream << NAMESPACE_TEXT << SPACE << nameToken.GetIdentifier() << NEWLINE;
			HeaderStream << OPEN_BRACKET << NEWLINE;

			m_CSTypeDeclaration << NAMESPACE_TEXT << SPACE << nameToken.GetIdentifier() << NEWLINE;
			m_CSTypeDeclaration << OPEN_BRACKET << NEWLINE;

			return true;
		}

		//bool HeaderParser::CompileForwardDeclaration(StringStream& HeaderStream, Token& DeclarationToken)
		//{
		//	return true;
		//}

		void HeaderParser::AddExportFunction(StringStream& Stream, const String& FullQualifiedTypeName, const String& TypeName, const String& Name, const StringList& ReturnTypeIdentifiers, const ParameterInfoList& Parameters, bool AddInstanceParameter)
		{
			Stream << m_ModuleAPI << SPACE;

			for each (auto t in ReturnTypeIdentifiers)
				Stream << t << SPACE;

			Stream << GetUniqueFunctionName(FullQualifiedTypeName, Name) << OPEN_BRACE;

			if (AddInstanceParameter)
				Stream << TypeName << STAR << "Instance";

			for each (const auto & parameter in Parameters)
			{
				Stream << COMMA;

				Stream << parameter.Type;

				if (parameter.IsPointer)
					Stream << STAR;

				Stream << SPACE << parameter.Name;
			}

			Stream << CLOSE_BRACE << NEWLINE;
			Stream << OPEN_BRACKET << NEWLINE;

			Stream << TAB;

			if (ReturnTypeIdentifiers.GetSize() != 1 || ReturnTypeIdentifiers[0] != VOID_TEXT)
				Stream << "return ";

			if (AddInstanceParameter)
				Stream << "Instance->";
			else
				Stream << TypeName << DOUBLE_COLON;

			Stream << Name << OPEN_BRACE;

			for (int i = 0; i < Parameters.GetSize(); ++i)
			{
				if (i != 0)
					Stream << COMMA;

				Stream << Parameters[i].Name;
			}

			Stream << CLOSE_BRACE << SEMICOLON << NEWLINE;
			Stream << CLOSE_BRACKET << NEWLINE;
		}

		void HeaderParser::AddImportFunction(StringStream& Stream, const String& FunctionName, const String& ExportFunctionName, const StringList& ReturnTypeIdentifiers, const ParameterInfoList& Parameters, bool AddInstanceParameter)
		{
			Stream << "[System.Runtime.InteropServices.DllImport(\"" << m_BinaryFileName << "\", EntryPoint = \"" << ExportFunctionName << "\")]" << NEWLINE;
			Stream << PUBLIC_TEXT << " static extern ";

			bool isPointer = false;
			String returnTypeName = "void";
			for each (const auto & name in ReturnTypeIdentifiers)
			{
				if (name == STAR)
				{
					isPointer = true;
					break;
				}
				
				if (name == AND)
					continue;

				returnTypeName = name;
			}

			if (isPointer)
				Stream << CS_POINTER_TEXT;
			else
				Stream << returnTypeName;

			Stream << SPACE << FunctionName << OPEN_BRACE;
			
			if (AddInstanceParameter)
				Stream << CS_POINTER_TEXT << SPACE << "Instance";

			for each (const auto & parameter in Parameters)
			{
				Stream << COMMA;

				if (parameter.IsPointer)
					Stream << CS_POINTER_TEXT << SPACE;
				else
					Stream << parameter.Type;

				Stream << SPACE << parameter.Name;
			}			
			
			Stream << CLOSE_BRACE << SEMICOLON << NEWLINE;
		}

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
