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

		String GetCSTypeName(const String& DataType)
		{
			if (DataType == VOID_TEXT)
				return VOID_TEXT;

			if (DataType == "bool")
				return DataType;

			if (DataType == "char")
				return DataType;

			if (DataType == "int8")
				return "char";

			if (DataType == "int16")
				return "short";

			if (DataType == "int32")
				return "int";

			if (DataType == "int64")
				return "long";

			if (DataType == "uchar")
				return DataType;

			if (DataType == "uint8")
				return "uchar";

			if (DataType == "uint16")
				return "ushort";

			if (DataType == "uint32")
				return "uint";

			if (DataType == "uint64")
				return "ulong";

			if (DataType == "float32")
				return "float";

			if (DataType == "float64")
				return "double";

			if (DataType == "String")
				return "string";

			return DataType;
		}

		bool HeaderParser::Parse(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, StringStream& CSStream)
		{
			Tokenizer::Parse();

			do
			{
				Token token;
				if (!GetToken(token))
					break;

				if (!CompileDeclaration(HeaderIncludeStream, HeaderDeclarationStream, token))
				{
					Debug::LogError((TEXT("'") + token.GetIdentifier() + "': Bad command or expression").GetValue());
					return false;
				}
			} while (true);

			CSStream << m_CSNameUsingNamespaces.GetBuffer();
			CSStream << m_CSTypeDeclaration.GetBuffer();

			return true;
		}

		bool HeaderParser::CompileDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToken)
		{
			AccessSpecifiers access = GetAccessSpecifier(DeclarationToken);

			if (DeclarationToken.Matches(WRAPPER_OBJECT_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileTypeDeclaration(HeaderIncludeStream, HeaderDeclarationStream, DeclarationToken))
					return false;
			}
			else if (DeclarationToken.Matches(WRAPPER_DATA_STRUCTURE_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (CompileDataStructureDeclaration(HeaderIncludeStream, HeaderDeclarationStream, DeclarationToken))
					return true;
				else if (CompileTypeDefDataStructureDeclaration(HeaderIncludeStream, HeaderDeclarationStream, DeclarationToken))
					return true;

				return false;
			}
			else if (DeclarationToken.Matches(USING_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileUsingNamespaceDeclaration(HeaderIncludeStream, HeaderDeclarationStream, DeclarationToken))
					return false;
			}
			else if (DeclarationToken.Matches(NAMESPACE_TEXT, Token::SearchCases::CaseSensitive))
			{
				if (!CompileNamespace(HeaderIncludeStream, HeaderDeclarationStream, DeclarationToken))
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
					if (m_Qualifiers.GetSize() != 0)
					{
						RemoveLastQualifier();

						HeaderDeclarationStream << CLOSE_BRACKET << NEWLINE;
						m_CSTypeDeclaration << CLOSE_BRACKET << NEWLINE;
					}
				}
			}

			return true;
		}

		bool HeaderParser::CompileTypeDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToke)
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

			m_CSTypeDeclaration << PUBLIC_TEXT << SPACE << CLASS_TEXT << SPACE << typeName << NEWLINE << OPEN_BRACKET << NEWLINE;

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
					DataTypeInfo returnType;
					returnType.Type = typeName;
					returnType.IsPointer = true;

					HeaderIncludeStream << "#include <MemoryManagement\\Allocator\\RootAllocator.h>\n";

					ParameterInfoList parameters;

					const String getInstanceFunctionName = "GetInstance";

					StringStream createInstanceStream;
					createInstanceStream << typeName << DOUBLE_COLON << "Create" << OPEN_BRACE << "RootAllocator" << DOUBLE_COLON << getInstanceFunctionName << OPEN_BRACE << CLOSE_BRACE << CLOSE_BRACE << SEMICOLON << NEWLINE;

					AddExportFunction(HeaderDeclarationStream, fullQualifiedTypeName, typeName, getInstanceFunctionName, returnType, parameters, false, createInstanceStream.GetBuffer());

					AddImportFunction(m_CSTypeDeclaration, typeName, getInstanceFunctionName, GetUniqueFunctionName(fullQualifiedTypeName, getInstanceFunctionName), returnType, parameters, false);

					m_CSTypeDeclaration << PRIVATE_TEXT << SPACE << CS_POINTER_TEXT << SPACE << GetNativePointerName(typeName) << EQUAL << CS_POINTER_TEXT << ".Zero" << SEMICOLON << NEWLINE;
					m_CSTypeDeclaration << PRIVATE_TEXT << SPACE << STATIC_TEXT << " readonly " << typeName << " instance" << EQUAL << "new " << typeName << OPEN_BRACE << getInstanceFunctionName << OPEN_BRACE << CLOSE_BRACE << CLOSE_BRACE << SEMICOLON << NEWLINE;
					m_CSTypeDeclaration << PUBLIC_TEXT << SPACE << STATIC_TEXT << SPACE << typeName << " Instance" << OPEN_BRACKET << "get" << OPEN_BRACKET << "return instance" << SEMICOLON << CLOSE_BRACKET << CLOSE_BRACKET << NEWLINE;
					m_CSTypeDeclaration << PRIVATE_TEXT << SPACE << typeName << OPEN_BRACE << CLOSE_BRACE << OPEN_BRACKET << CLOSE_BRACKET << NEWLINE;
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
					if (!CompileWrapperFunctionDeclaration(HeaderIncludeStream, HeaderDeclarationStream, fullQualifiedTypeName, typeName, token))
						return false;
				}
			}

			m_CSTypeDeclaration << CLOSE_BRACKET << NEWLINE;
		}

		bool HeaderParser::CompileDataStructureDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToke)
		{
			if (!ReadSpecifier(DeclarationToke))
				return false;

			bool isTemplate = false;
			StringList templateParams;

			if (MatchIdentifier(TEMPLATE_TEXT))
			{
				if (!RequiredToken(LESS_THAN))
					return false;

				while (true)
				{
					Token typeToken;
					if (!GetToken(typeToken))
						return false;

					Token nameToken;
					if (!GetToken(nameToken))
						return false;

					templateParams.Add(nameToken.GetIdentifier());

					if (RequiredToken(COMMA))
						continue;

					if (!RequiredToken(GREATER_THAN))
						return false;

					break;
				}

				isTemplate = true;
			}

			if (!RequiredToken(CLASS_TEXT) && !RequiredToken(STRUCT_TEXT))
				return false;

			bool isStruct = DeclarationToke.Matches(STRUCT_TEXT, Token::SearchCases::IgnoreCase);

			Token typeNameToken;
			if (!ReadTypeName(typeNameToken))
				return false;

			const String& typeName = typeNameToken.GetIdentifier();

			if (isTemplate)
				m_CSTypeDeclaration << FORWARD_SLASH << FORWARD_SLASH << TEMPLATE_TEXT << SPACE << typeName << NEWLINE;

			m_CSTypeDeclaration << PUBLIC_TEXT << SPACE;

			if (isStruct)
				m_CSTypeDeclaration << STRUCT_TEXT;
			else
				m_CSTypeDeclaration << CLASS_TEXT;

			m_CSTypeDeclaration << SPACE << typeName;

			if (isTemplate)
			{
				m_CSTypeDeclaration << LESS_THAN;

				for (int i = 0; i < templateParams.GetSize(); ++i)
				{
					if (i != 0)
						m_CSTypeDeclaration << COMMA;

					m_CSTypeDeclaration << templateParams[i];
				}

				m_CSTypeDeclaration << GREATER_THAN;
			}

			m_CSTypeDeclaration << NEWLINE;
			m_CSTypeDeclaration << OPEN_BRACKET << NEWLINE;

			AccessSpecifiers lastAccessSpecifier = (isStruct ? AccessSpecifiers::Public : AccessSpecifiers::NonPublic);

			int scoreCount = 1;

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
				else
				{
					if (CompileDataStructureFunctionDeclaration(HeaderIncludeStream, HeaderDeclarationStream, token) != CompileResults::Reject)
					{
					}
					else if (CompileDataStructureVariableDeclaration(HeaderIncludeStream, HeaderDeclarationStream, lastAccessSpecifier, token) != CompileResults::Reject)
					{
					}
				}
			}

			m_CSTypeDeclaration << CLOSE_BRACKET << NEWLINE;
		}

		bool HeaderParser::CompileTypeDefDataStructureDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToke)
		{
			bool isTemplate = false;
			StringList templateParams;

			if (MatchIdentifier(TEMPLATE_TEXT))
			{
				if (!RequiredToken(LESS_THAN))
					return false;

				while (true)
				{
					Token typeToken;
					if (!GetToken(typeToken))
						return false;

					Token nameToken;
					if (!GetToken(nameToken))
						return false;

					templateParams.Add(nameToken.GetIdentifier());

					if (RequiredToken(COMMA))
						continue;

					if (!RequiredToken(GREATER_THAN))
						return false;

					break;
				}

				isTemplate = true;
			}

			if (!MatchIdentifier(TYPEDEF_TEXT))
				return false;

			Token token;
			if (!GetToken(token))
				return false;

			DataTypeInfo type;
			if (!CompileDataType(type, token))
				return false;

			Token nameToken;
			if (!GetToken(nameToken))
				return false;

			const String& typeName = nameToken.GetIdentifier();

			m_CSTypeDeclaration << FORWARD_SLASH << FORWARD_SLASH << TYPEDEF_TEXT << SPACE << typeName << SPACE << type.Type << NEWLINE;
			for each (const auto arg in type.TemplateArguments)
				m_CSTypeDeclaration << FORWARD_SLASH << FORWARD_SLASH << GetCSTypeName(arg) << NEWLINE;
			m_CSTypeDeclaration << PUBLIC_TEXT << SPACE << CLASS_TEXT << SPACE << typeName << COLON << GetCSType(type) << OPEN_BRACKET << CLOSE_BRACKET << NEWLINE;
		}

		bool HeaderParser::CompileWrapperFunctionDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, const String& FullQualifiedTypeName, const String& TypeName, Token& DeclarationToken)
		{
			DataTypeInfo returnType;
			if (!CompileDataType(returnType, DeclarationToken))
				return false;

			String name;
			ParameterInfoList parameters;

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
						parameter.DataType.IsPointer = false;
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
								parameter.DataType.IsPointer = true;
								continue;
							}

							if (paramToken.Matches(COMMA, Token::SearchCases::IgnoreCase))
								break;

							if (paramToken.Matches(CLOSE_BRACE, Token::SearchCases::IgnoreCase))
							{
								UngetToken(paramToken);
								break;
							}

							if (parameter.DataType.Type.GetLength() == 0)
								parameter.DataType.Type = paramToken.GetIdentifier();
							else
								parameter.Name = paramToken.GetIdentifier();
						}

						parameters.Add(parameter);
					}

					AddExportFunction(HeaderDeclarationStream, FullQualifiedTypeName, TypeName, name, returnType, parameters, true);

					AddImportFunction(m_CSTypeDeclaration, TypeName, name, GetUniqueFunctionName(FullQualifiedTypeName, name), returnType, parameters, true);

					isFunction = true;
				}
				else if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					return true;
				else if (token.Matches(TILDE, Token::SearchCases::IgnoreCase))
					return true;
				else if (token.Matches(COLON, Token::SearchCases::IgnoreCase))
					return true;
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

		HeaderParser::CompileResults HeaderParser::CompileDataStructureFunctionDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToken)
		{
			DataTypeInfo returnType;
			if (!CompileDataType(returnType, DeclarationToken))
				return CompileResults::Failed;

			if (returnType.Type.GetLength() == 0)
				UngetToken(DeclarationToken);

			String name;
			ParameterInfoList parameters;

			bool isFunction = false;

			while (true)
			{
				Token token;
				if (!GetToken(token))
					return CompileResults::Failed;

				if (RequiredToken(SEMICOLON))
					break;
				else if (RequiredToken(OPEN_BRACE))
				{
					//name = token.GetIdentifier();

					while (true)
					{
						Token tempParamToken;
						if (!GetToken(tempParamToken))
							return CompileResults::Failed;

						if (tempParamToken.Matches(CLOSE_BRACE, Token::SearchCases::IgnoreCase))
							break;

						if (MatchSymbol(CLOSE_BRACE))
							break;

						if (tempParamToken.Matches(COMMA, Token::SearchCases::IgnoreCase))
							continue;

						ParamaterInfo parameter;
						parameter.DataType.IsPointer = false;
						UngetToken(tempParamToken);
						while (true)
						{
							Token paramToken;
							if (!GetToken(paramToken))
								return CompileResults::Failed;

							if (paramToken.Matches(CONST_TEXT, Token::SearchCases::IgnoreCase))
								continue;

							if (paramToken.Matches(STAR, Token::SearchCases::IgnoreCase))
							{
								parameter.DataType.IsPointer = true;
								continue;
							}

							if (paramToken.Matches(COMMA, Token::SearchCases::IgnoreCase))
								break;

							if (paramToken.Matches(CLOSE_BRACE, Token::SearchCases::IgnoreCase))
							{
								UngetToken(paramToken);
								break;
							}

							if (parameter.DataType.Type.GetLength() == 0)
								parameter.DataType.Type = paramToken.GetIdentifier();
							else
								parameter.Name = paramToken.GetIdentifier();
						}

						parameters.Add(parameter);
					}

					if (!MatchSymbol(SEMICOLON))
						if (!SkipScope())
							return CompileResults::Failed;

					//AddImportFunction(m_CSTypeDeclaration, TypeName, name, GetUniqueFunctionName(FullQualifiedTypeName, name), returnType, parameters, true);

					isFunction = true;

					break;
				}
				else if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					return CompileResults::Done;
				else if (token.Matches(TILDE, Token::SearchCases::IgnoreCase))
					return CompileResults::Done;
				//else if (token.Matches(COLON, Token::SearchCases::IgnoreCase))
				//return CompileResults::Done;
			}

			if (!isFunction)
			{
				UngetToken(DeclarationToken);
				Token temp;
				GetToken(temp);

				return CompileResults::Reject;
			}
			else
				(RequiredToken(CLOSE_BRACKET) || RequiredToken(SEMICOLON));

			return CompileResults::Done;
		}

		HeaderParser::CompileResults HeaderParser::CompileDataStructureVariableDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, AccessSpecifiers AccessSpecifier, Token& DeclarationToken)
		{
			DataTypeInfo returnType;
			if (!CompileDataType(returnType, DeclarationToken))
				return CompileResults::Failed;

			if (returnType.Type.GetLength() != 0)
				while (true)
				{
					Token nameToken;
					if (!GetToken(nameToken))
						return CompileResults::Failed;

					if (AccessSpecifier == AccessSpecifiers::Public)
						m_CSTypeDeclaration << "public";
					else
						m_CSTypeDeclaration << "private";

					m_CSTypeDeclaration << SPACE << GetCSType(returnType) << SPACE << nameToken.GetIdentifier() << SEMICOLON << NEWLINE;

					if (MatchSymbol(SEMICOLON))
						break;

					if (MatchSymbol(COMMA))
						continue;

					return CompileResults::Done;
				}

			return CompileResults::Reject;
		}

		bool HeaderParser::CompileUsingNamespaceDeclaration(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToken)
		{
			if (!RequiredToken(NAMESPACE_TEXT))
				return false;

			HeaderDeclarationStream << USING_TEXT << SPACE << NAMESPACE_TEXT << SPACE;
			m_CSNameUsingNamespaces << USING_TEXT << SPACE;

			const String qualifiers = GetQualifiers();
			if (qualifiers.GetLength() != 0)
				m_CSNameUsingNamespaces << qualifiers << DOT;

			m_CSTypeDeclaration << NAMESPACE_TEXT << SPACE;

			while (true)
			{
				Token token;
				if (!GetToken(token))
					return false;

				if (token.Matches(SEMICOLON, Token::SearchCases::IgnoreCase))
					break;

				if (token.Matches(DOUBLE_COLON, Token::SearchCases::IgnoreCase))
				{
					HeaderDeclarationStream << DOUBLE_COLON;
					m_CSNameUsingNamespaces << DOT;
					m_CSTypeDeclaration << DOT;

					continue;
				}

				HeaderDeclarationStream << token.GetIdentifier();
				m_CSNameUsingNamespaces << token.GetIdentifier();
				m_CSTypeDeclaration << token.GetIdentifier();
			}

			HeaderDeclarationStream << SEMICOLON << NEWLINE;
			m_CSNameUsingNamespaces << SEMICOLON << NEWLINE;
			m_CSTypeDeclaration << OPEN_BRACKET << CLOSE_BRACKET << NEWLINE;

			return true;
		}

		bool HeaderParser::CompileNamespace(StringStream& HeaderIncludeStream, StringStream& HeaderDeclarationStream, Token& DeclarationToken)
		{
			Token nameToken;
			if (!GetToken(nameToken))
				return false;

			if (!MatchSymbol(OPEN_BRACKET))
				return false;

			AddQualifier(nameToken.GetIdentifier());

			HeaderDeclarationStream << NAMESPACE_TEXT << SPACE << nameToken.GetIdentifier() << NEWLINE;
			HeaderDeclarationStream << OPEN_BRACKET << NEWLINE;

			m_CSTypeDeclaration << NAMESPACE_TEXT << SPACE << nameToken.GetIdentifier() << NEWLINE;
			m_CSTypeDeclaration << OPEN_BRACKET << NEWLINE;

			return true;
		}

		bool HeaderParser::CompileDataType(DataTypeInfo& DataType, Token& DeclarationToken)
		{
			UngetToken(DeclarationToken);

			bool result = true;
			bool isFirst = true;
			bool isTemplate = false;
			while (true)
			{
				Token token;
				if (!GetToken(token))
					return false;

				if (token.Matches(STAR, Token::SearchCases::IgnoreCase))
				{
					DataType.IsPointer = true;
				}
				else if (token.Matches(AND, Token::SearchCases::IgnoreCase))
				{
					DataType.IsReference = true;
				}
				else if (token.Matches(CONST_TEXT, Token::SearchCases::IgnoreCase))
				{
				}
				else if (token.Matches(TYPEDEF_TEXT, Token::SearchCases::IgnoreCase))
				{
					result = false;
				}
				else if (MatchSymbol(OPEN_BRACE) || MatchSymbol(COMMA) || MatchSymbol(CLOSE_BRACE) || MatchSymbol(SEMICOLON))
				{
					if (!isFirst)
						UngetToken(token);

					break;
				}
				else if (token.Matches(LESS_THAN, Token::SearchCases::IgnoreCase))
				{
					isTemplate = true;
				}
				else if (token.Matches(GREATER_THAN, Token::SearchCases::IgnoreCase))
				{
					isTemplate = false;
				}
				else if (token.GetTokenType() != Token::Types::Identifier)
					break;
				else
				{
					if (isTemplate)
						DataType.TemplateArguments.Add(token.GetIdentifier());
					else
						DataType.Type = token.GetIdentifier();
				}

				isFirst = false;
			}

			return result;
		}

		void HeaderParser::AddExportFunction(StringStream& Stream, const String& FullQualifiedTypeName, const String& TypeName, const String& Name, const DataTypeInfo& ReturnType, const ParameterInfoList& Parameters, bool AddInstanceParameter, const String& Additional)
		{
			Stream << m_ModuleAPI << SPACE;

			Stream << GetCPPType(ReturnType) << SPACE;

			Stream << GetUniqueFunctionName(FullQualifiedTypeName, Name) << OPEN_BRACE;

			if (AddInstanceParameter)
				Stream << TypeName << STAR << "Instance";

			for each (const auto & parameter in Parameters)
				Stream << COMMA << GetCPPType(parameter.DataType) << SPACE << parameter.Name;

			Stream << CLOSE_BRACE << NEWLINE;
			Stream << OPEN_BRACKET << NEWLINE;

			Stream << TAB << Additional;

			Stream << TAB;

			if (ReturnType.Type != VOID_TEXT)
				Stream << "return ";

			if (ReturnType.IsReference)
				Stream << "const_cast<" << ReturnType.Type << STAR << GREATER_THAN << OPEN_BRACE << AND;

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

			if (ReturnType.IsReference)
				Stream << CLOSE_BRACE;

			Stream << CLOSE_BRACE << SEMICOLON << NEWLINE;
			Stream << CLOSE_BRACKET << NEWLINE;
		}

		void HeaderParser::AddImportFunction(StringStream& Stream, const String& TypeName, const String& FunctionName, const String& ExportFunctionName, const DataTypeInfo& ReturnType, const ParameterInfoList& Parameters, bool AddInstanceParameter)
		{
			Stream << "[System.Runtime.InteropServices.DllImport(\"" << m_BinaryFileName << "\", EntryPoint = \"" << ExportFunctionName << "\")]" << NEWLINE;
			Stream << PUBLIC_TEXT << " static extern " << GetCSType(ReturnType) << SPACE << FunctionName << OPEN_BRACE;

			if (AddInstanceParameter)
				Stream << CS_POINTER_TEXT << SPACE << "Instance";

			for each (const auto & parameter in Parameters)
				Stream << COMMA << GetCSType(parameter.DataType) << SPACE << parameter.Name;

			Stream << CLOSE_BRACE << SEMICOLON << NEWLINE;

			if (AddInstanceParameter)
			{
				Stream << PUBLIC_TEXT << SPACE << GetCSType(ReturnType) << SPACE << FunctionName << OPEN_BRACE;

				for (int i = 0; i < Parameters.GetSize(); ++i)
				{
					const auto& parameter = Parameters[i];

					if (i != 0)
						Stream << COMMA;

					Stream << GetCSType(parameter.DataType) << SPACE << parameter.Name;
				}

				Stream << CLOSE_BRACE << NEWLINE << OPEN_BRACKET << NEWLINE;

				if (ReturnType.Type != VOID_TEXT)
					Stream << "return ";

				Stream << FunctionName << OPEN_BRACE;

				Stream << GetNativePointerName(TypeName);

				for (int i = 0; i < Parameters.GetSize(); ++i)
				{
					Stream << COMMA;

					Stream << Parameters[i].Name;
				}

				Stream << CLOSE_BRACE << SEMICOLON << NEWLINE << CLOSE_BRACKET << NEWLINE;
			}
		}

		bool HeaderParser::SkipScope(void)
		{
			int scope = 0;

			while (true)
			{
				Token token;
				if (!GetToken(token))
					return false;

				if (token.Matches(OPEN_BRACKET, Token::SearchCases::IgnoreCase))
				{
					++scope;
					continue;
				}

				if (token.Matches(CLOSE_BRACKET, Token::SearchCases::IgnoreCase))
				{
					if (--scope == 0)
						break;

					continue;
				}
			}

			return true;
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

		String HeaderParser::GetCPPType(DataTypeInfo DataType)
		{
			if (DataType.Type == "String")
				return "const char*";

			if (DataType.IsPointer || DataType.IsReference)
				return DataType.Type + STAR;

			return DataType.Type;
		}

		String HeaderParser::GetCSType(DataTypeInfo DataType)
		{
			if (DataType.IsPointer || DataType.IsReference)
				return CS_POINTER_TEXT;

			String typeName = GetCSTypeName(DataType.Type);

			if (DataType.TemplateArguments.GetSize() != 0)
			{
				typeName += LESS_THAN;

				for (int i = 0; i < DataType.TemplateArguments.GetSize(); ++i)
				{
					if (i != 0)
						typeName += COMMA;

					typeName += GetCSTypeName(DataType.TemplateArguments[i]);
				}

				typeName += GREATER_THAN;
			}

			return typeName;
		}
	}
}