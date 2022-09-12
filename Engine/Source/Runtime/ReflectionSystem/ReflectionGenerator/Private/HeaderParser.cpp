// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\HeaderParser.h>
#include <ReflectionGenerator\Private\MetaObject.h>
#include <ReflectionGenerator\Private\MetaEnum.h>
#include <ReflectionGenerator\Private\MetaConstructor.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaParameter.h>
#include <ReflectionGenerator\Private\MetaProperty.h>
#include <ReflectionGenerator\ReflectionGeneratorException.h>

namespace Engine
{
	namespace ReflectionGenerator
	{
		namespace Private
		{
			void HeaderParser::Parse(TypeList& Types)
			{
				CodePageParser::Parse(Types);

				m_CurrentObject = nullptr;
				m_BlockLevel = 0;

				do
				{
					Token token;
					if (!GetToken(token))
						return;

					CompileDeclaration(Types, token);

				} while (true);
			}

			void HeaderParser::CompileDeclaration(TypeList& Types, Token& DelarationToken)
			{
				AccessSpecifiers access = ParseAccessSpecifier(DelarationToken);

				if (access != AccessSpecifiers::None)
				{
					if (MatchSymbol(COLON))
					{
						if (m_CurrentObject != nullptr)
							m_CurrentObject->SetLastAccessSpecifier(access);
					}
				}
				else if (DelarationToken.Matches(REFLECTION_CLASS_TEXT, Token::SearchCases::CaseSensitive))
					CompileObjectDeclaration(DelarationToken, Types, false);
				else if (DelarationToken.Matches(REFLECTION_STRUCT_TEXT, Token::SearchCases::CaseSensitive))
					CompileObjectDeclaration(DelarationToken, Types, true);
				else if (m_CurrentObject != nullptr && DelarationToken.Matches(m_CurrentObject->GetName(), Token::SearchCases::CaseSensitive))
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
						if (m_CurrentObject != nullptr && m_BlockLevel == m_CurrentObject->GetBlockLevel())
							m_CurrentObject = ReinterpretCast(MetaObject*, m_CurrentObject->GetTopNest());
					}
					else
						PopNamespace();
				}
				else if (DelarationToken.Matches(NAMESPACE, Token::SearchCases::CaseSensitive))
					PushNamespace();
			}

			void HeaderParser::CompileObjectDeclaration(const Token& Declaration, TypeList& Types, bool IsStruct)
			{
				MetaObject* type = ReinterpretCast(MetaObject*, AllocateMemory(m_Allocator, sizeof(MetaObject)));
				Construct(type, m_Allocator, m_CurrentObject);

				type->SetNamespace(GetFullNamespace());
				type->SetIsStruct(IsStruct);

				ParseSpecifiers(type, "object");

				if (IsStruct)
				{
					RequireIdentifier(STRUCT, "object");

					type->SetLastAccessSpecifier(AccessSpecifiers::Public);
				}
				else
				{
					RequireIdentifier(CLASS, "object");

					type->SetLastAccessSpecifier(AccessSpecifiers::Private);
				}

				Token nameToken;
				RequireIdentifierToken(nameToken);

				type->SetName(nameToken.GetName());

				if (MatchSymbol(SEMICOLON))
				{
					DeallocateMemory(m_Allocator, type);
					return;
				}

				bool hasParent = false;
				if ((hasParent = MatchSymbol(COLON)))
				{
					uint8 parentCount = 0;

					while (!MatchSymbol(OPEN_BRACKET))
					{
						if (parentCount++ > 0)
							RequireSymbol(COMMA, "missing in parent list");

						Token token;
						RequireIdentifierToken(token);

						AccessSpecifiers access = ParseAccessSpecifier(token);

						if (access == AccessSpecifiers::None)
							type->AddParentName(token.GetName(), AccessSpecifiers::Private);
						else
						{
							Token token;
							RequireIdentifierToken(token);

							type->AddParentName(token.GetName(), access);
						}
					}
				}

				RequireIdentifier(type->GetDeclarationMacroName(), "object");

				if (m_CurrentObject == nullptr)
					Types.Add(type);
				else
					m_CurrentObject->AddNestedType(type);

				m_CurrentObject = type;
				m_CurrentObject->SetBlockLevel(m_BlockLevel);

				AddBlockLevel();
			}

			void HeaderParser::CompileEnumDeclaration(TypeList& Types)
			{
				MetaEnum* type = ReinterpretCast(MetaEnum*, AllocateMemory(m_Allocator, sizeof(MetaEnum)));
				Construct(type, m_CurrentObject);

				ParseSpecifiers(type, "enum");

				RequireIdentifier(ENUM, "enum");

				MatchIdentifier(CLASS);

				Token nameToken;
				RequireIdentifierToken(nameToken);
				type->SetName(nameToken.GetName());

				uint8 membersCount = 0;
				int32 prevItemValue = 0;
				while (true)
				{
					Token itemNameToken;
					RequireIdentifierToken(itemNameToken);

					if (MatchSymbol(EQUAL))
					{
						Token itemValueToken;
						RequireToken(itemValueToken);

						prevItemValue = itemValueToken.GetConstantInt32();
					}

					type->AddItem(itemNameToken.GetName(), prevItemValue);

					++prevItemValue;

					if (MatchSymbol(COMMA))
						continue;

					if (MatchSymbol(CLOSE_BRACKET))
						break;
				}

				if (m_CurrentObject == nullptr)
					Types.Add(type);
				else
					m_CurrentObject->AddNestedType(type);
			}

			void HeaderParser::CompileConstructorDeclaration(void)
			{
				MetaConstructor* ctor = ReinterpretCast(MetaConstructor*, AllocateMemory(m_Allocator, sizeof(MetaConstructor)));
				Construct(ctor, m_CurrentObject);

				ctor->SetName(m_CurrentObject->GetName());

				RequireSymbol(OPEN_BRACE, "ctor");

				ParseParameters(ctor);

				m_CurrentObject->AddConstructor(ctor);
			}

			void HeaderParser::CompileFunctionDeclaration(void)
			{
				MetaFunction* func = ReinterpretCast(MetaFunction*, AllocateMemory(m_Allocator, sizeof(MetaFunction)));
				Construct(func, m_CurrentObject);

				ParseSpecifiers(func, "function");

				MetaDataType returnType;
				if (!ParseDataType(returnType))
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

				ParseParameters(func);

				if (MatchIdentifier(CONST))
					func->SetIsConst(true);

				m_CurrentObject->AddFunction(func);
			}

			void HeaderParser::CompileVariableDeclaration(void)
			{
				MetaProperty* property = ReinterpretCast(MetaProperty*, AllocateMemory(m_Allocator, sizeof(MetaProperty)));
				Construct(property, m_CurrentObject);

				ParseSpecifiers(property, "property");

				MetaDataType dataType;
				ParseDataType(dataType);

				if (dataType.GetValueType() == ValueTypes::None && dataType.GetExtraValueType() == String::Empty)
					THROW_REFLECTION_GENERATOR_EXCEPTION("Unexpected token");

				Token nameToken;
				GetToken(nameToken);

				property->SetName(nameToken.GetName());
				property->SetDataType(dataType);

				m_CurrentObject->AddProperty(property);
			}

			void HeaderParser::PushNamespace(void)
			{
				Token nameToken;
				GetToken(nameToken);

				if (!MatchSymbol(OPEN_BRACKET))
					return;

				m_Namespaces.Push(nameToken.GetName());
			}

			void HeaderParser::PopNamespace(void)
			{
				m_Namespaces.Pop();
			}

			String HeaderParser::GetFullNamespace(void) const
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
		}
	}
}
