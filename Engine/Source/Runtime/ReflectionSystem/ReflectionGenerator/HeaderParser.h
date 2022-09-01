// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H
#include <ReflectionGenerator\CodePageParser.h>
#include <Allocators\AllocatorBase.h>
#include <ReflectionGenerator\MetaDataStructure.h>
#include <ReflectionGenerator\MetaProperty.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Containers;

	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API HeaderParser : public CodePageParser
		{
		public:
			HeaderParser(AllocatorBase* Allocator, const String& Text) :
				CodePageParser(Text),
				m_Allocator(Allocator),
				m_BlockLevel(0),
				m_CurrentDataStructure(nullptr)
			{
			}
			virtual ~HeaderParser(void)
			{
			}

			virtual void Parse(TypeList& Types) override;

		protected:
			virtual bool CompileDeclaration(TypeList& Types, Token& Token);
			virtual void CompileTypeDeclaration(const Token& Declaration, TypeList& Types);
			virtual void CompileStructDeclaration(const Token& Declaration, TypeList& Types);
			virtual void CompileEnumDeclaration(TypeList& Types);
			virtual void CompileConstructorDeclaration(void);
			virtual void CompileFunctionDeclaration(void);
			virtual void CompileVariableDeclaration(void);

			virtual AccessSpecifiers ParseAccessSpecifier(Token& Token);
			virtual AccessSpecifiers GetAccessSpecifier(Token& Token);

			virtual void AddBlockLevel(void)
			{
				m_BlockLevel++;
			}

			virtual void AddNamespace(void);

			String GetNamespaces(void) const;

		private:
			AllocatorBase* m_Allocator;
			uint16 m_BlockLevel;
			MetaDataStructure* m_CurrentDataStructure;
			StringList m_Namespaces;
		};
	}
}
#endif
