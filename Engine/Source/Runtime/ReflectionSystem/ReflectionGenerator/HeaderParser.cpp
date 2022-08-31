// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\HeaderParser.h>
#include <ReflectionGenerator\MetaDataStructure.h>
#include <ReflectionGenerator\MetaEnum.h>
#include <ReflectionGenerator\MetaConstructor.h>
#include <ReflectionGenerator\MetaFunction.h>
#include <ReflectionGenerator\MetaProperty.h>
#include <ReflectionGenerator\ReflectionGeneratorException.h>

namespace Engine
{
	namespace ReflectionGenerator
	{
		void HeaderParser::Parse(TypeList& Types)
		{
			CodePageParser::Parse(Types);

			m_CurrentDataStructure = nullptr;
			m_BlockLevel = 0;

			do
			{
				Token token;
				if (!GetToken(token))
					return;

				else if (!CompileDeclaration(Types, token))
					THROW_REFLECTION_TOOL_EXCEPTION("'" + token.GetName() + "': Bad command or expression");

			} while (true);
		}

		bool HeaderParser::CompileDeclaration(TypeList& Types, Token& DelarationToken)
		{
			AccessSpecifiers access = GetAccessSpecifier(DelarationToken);

			if (access != AccessSpecifiers::None)
			{
				//if (m_CurrentDataStructure == nullptr)
				//{
				//	Debug::LogError("Access specifier detected out of nest");
				//	return false;
				//}

				//m_CurrentDataStructure->SetLastAccessSpecifier(access);

				if (m_CurrentDataStructure != nullptr)
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

				if (MatchSemiColon())
				{
					if (m_CurrentDataStructure != nullptr && m_BlockLevel == m_CurrentDataStructure->GetBlockLevel())
						m_CurrentDataStructure = (MetaDataStructure*)m_CurrentDataStructure->GetTopNest();
				}
				else
				{
					if (m_Namespaces.GetSize() != 0)
						m_Namespaces.RemoveAt(m_Namespaces.GetSize() - 1);
				}
			}
			else if (DelarationToken.Matches(NAMESPACE, Token::SearchCases::CaseSensitive))
				AddNamespace();

			return true;
		}

		void HeaderParser::CompileTypeDeclaration(const Token& Declaration, TypeList& Types)
		{
			MetaDataStructure* type = ReinterpretCast(MetaDataStructure*, AllocateMemory(m_Allocator, sizeof(MetaDataStructure)));
			Construct(type, m_Allocator, m_CurrentDataStructure);

			type->SetNamespace(GetNamespaces());

			ReadSpecifiers(type, "Class");

			bool hasParent = false;
			while (true)
			{
				Token token;
				if (!GetToken(token, true))
					break;

				if (MatchSemiColon())
				{
					DeallocateMemory(m_Allocator, type);
					return;
				}
				else if ((hasParent = MatchSymbol(COLON)) || MatchSymbol(OPEN_BRACKET))
				{
					type->SetName(token.GetName());
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

					AccessSpecifiers access = ParseAccessSpecifier(token);

					if (access == AccessSpecifiers::None)
						type->AddParentName(token.GetName(), AccessSpecifiers::Private);
					else
					{
						Token token;
						GetToken(token);
						type->AddParentName(token.GetName(), access);
					}
				}
			}

			Token objectDeclMacroToken;
			GetToken(objectDeclMacroToken);
			if (!objectDeclMacroToken.Matches(type->GetDeclarationMacroName(), Token::SearchCases::CaseSensitive))
			{
				AddBlockLevel();
				DeallocateMemory(m_Allocator, type);
				return;
			}

			if (m_CurrentDataStructure == nullptr)
				Types.Add(type);
			else
				m_CurrentDataStructure->AddNestedType(type);
			m_CurrentDataStructure = type;
			m_CurrentDataStructure->SetBlockLevel(m_BlockLevel);

			AddBlockLevel();
		}

		void HeaderParser::CompileStructDeclaration(const Token& Declaration, TypeList& Types)
		{
			CompileTypeDeclaration(Declaration, Types);
		}

		void HeaderParser::CompileEnumDeclaration(TypeList& Types)
		{
			MetaEnum* type = ReinterpretCast(MetaEnum*, AllocateMemory(m_Allocator, sizeof(MetaEnum)));
			Construct(type);

			ReadSpecifiers(type, "enum");

			Token nameToken;
			while (GetToken(nameToken, true))
			{
				if (MatchSemiColon())
					return;

				if (MatchSymbol(OPEN_BRACKET))
					break;

				nameToken.SetIdentifier("");
			}

			type->SetName(nameToken.GetName());

			uint8 membersCount = 0;
			do
			{
				if (MatchSymbol(COMMA))
					continue;

				bool bracketPresent = false;
				Token member;
				for (int32 i = 0; true; i++)
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

				type->AddItem(member.GetName());
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
			DeallocateMemory(m_Allocator, type);
		}

		void HeaderParser::CompileConstructorDeclaration(void)
		{
			MetaConstructor* ctor = ReinterpretCast(MetaConstructor*, AllocateMemory(m_Allocator, sizeof(MetaConstructor)));
			Construct(ctor, m_CurrentDataStructure);

			ctor->SetName(m_CurrentDataStructure->GetName());

			if (!MatchSymbol(OPEN_BRACE))
			{
				DeallocateMemory(m_Allocator, ctor);
				return;
			}

			while (true)
			{
				DataType paramDataType;
				if (!GetDataType(paramDataType))
					break;

				Token paramName;
				GetToken(paramName);

				ctor->AddParameter(paramDataType, paramName.GetName());
			}

			m_CurrentDataStructure->AddConstructor(ctor);

			while (true)
			{
				if (MatchSemiColon() || MatchSymbol(CLOSE_BRACKET))
					break;

				Token token;
				GetToken(token);
			}
		}

		void HeaderParser::CompileFunctionDeclaration(void)
		{
			MetaFunction* func = ReinterpretCast(MetaFunction*, AllocateMemory(m_Allocator, sizeof(MetaFunction)));
			Construct(func, m_CurrentDataStructure);

			ReadSpecifiers(func, "function");

			DataType returnType;
			if (!GetDataType(returnType))
			{
				DeallocateMemory(m_Allocator, func);
				return;
			}
			func->SetReturnType(returnType);

			Token nameToken;
			GetToken(nameToken);
			func->SetName(nameToken.GetName());

			if (!MatchSymbol(OPEN_BRACE))
			{
				DeallocateMemory(m_Allocator, func);
				return;
			}

			while (true)
			{
				DataType paramDataType;
				if (!GetDataType(paramDataType))
					break;

				Token paramName;
				GetToken(paramName);

				func->AddParameter(paramDataType, paramName.GetName());
			}

			if (MatchIdentifier(CONST))
				func->SetIsConst(true);

			m_CurrentDataStructure->AddFunction(func);
		}

		void HeaderParser::CompileVariableDeclaration(void)
		{
			MetaProperty* property = ReinterpretCast(MetaProperty*, AllocateMemory(m_Allocator, sizeof(MetaProperty)));
			Construct(property, m_CurrentDataStructure);

			ReadSpecifiers(property, "property");

			DataType dataType;
			GetDataType(dataType);
			if (dataType.GetValueType() == ValueTypes::None && dataType.GetExtraValueType() == String::Empty)
			{
				DeallocateMemory(m_Allocator, property);
				return;
			}

			Token nameToken;
			GetToken(nameToken);

			property->SetName(nameToken.GetName());
			property->SetDataType(dataType);

			m_CurrentDataStructure->AddProperty(property);
		}

		AccessSpecifiers HeaderParser::ParseAccessSpecifier(Token& Token)
		{
			if (Token.GetName() == "private")
				return AccessSpecifiers::Private;
			else if (Token.GetName() == "protected")
				return AccessSpecifiers::Protected;
			else if (Token.GetName() == "public")
				return AccessSpecifiers::Public;

			return AccessSpecifiers::None;
		}

		AccessSpecifiers HeaderParser::GetAccessSpecifier(Token& Token)
		{
			AccessSpecifiers access = ParseAccessSpecifier(Token);

			if (access != AccessSpecifiers::None)
				RequireSymbol(COLON, "after " + Token.GetName(), SymbolParseOptions::Normal);

			return access;
		}

		void HeaderParser::AddNamespace(void)
		{
			Token nameToken;
			GetToken(nameToken);

			if (!MatchSymbol(OPEN_BRACKET))
				return;

			m_Namespaces.Add(nameToken.GetName());
		}

		String HeaderParser::GetNamespaces(void) const
		{
			String str;

			bool isFirst = true;
			for (auto& name : m_Namespaces)
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
