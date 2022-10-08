// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H
#include <ReflectionGenerator\Private\CodePageParser.h>
#include <ReflectionGenerator\Private\MetaObject.h>
#include <Allocators\AllocatorBase.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Containers;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API HeaderParser : public CodePageParser
			{
			private:
				typedef Pair<uint16, String> NamespaceInfo;
				typedef Stack<NamespaceInfo> NamespaceStack;

			public:
				HeaderParser(AllocatorBase* Allocator, const String& Text) :
					CodePageParser(Text),
					m_Allocator(Allocator),
					m_BlockLevel(0),
					m_CurrentObject(nullptr)
				{
				}

				virtual ~HeaderParser(void)
				{
				}

				virtual void Parse(TypeList& Types) override;

			protected:
				virtual void CompileDeclaration(TypeList& Types, Token& Token);
				virtual void CompileObjectDeclaration(const Token& Declaration, TypeList& Types);
				virtual void CompileEnumDeclaration(TypeList& Types);
				virtual void CompileConstructorDeclaration(void);
				virtual void CompileFunctionDeclaration(void);
				virtual void CompilePropertyDeclaration(void);

				virtual void PushNamespace(void);
				virtual void PopNamespace(void);
				String GetFullNamespace(void) const;

				virtual void IncreaseBlockLevel(void)
				{
					++m_BlockLevel;
				}
				virtual void DecreaseBlockLevel(void)
				{
					--m_BlockLevel;
				}

			private:
				AllocatorBase* m_Allocator;
				uint16 m_BlockLevel;
				MetaObject* m_CurrentObject;
				NamespaceStack m_Namespaces;
			};
		}
	}
}
#endif
