// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H
#include <ReflectionTool\CodePageParser.h>
#include <ReflectionTool\MetaDataStructure.h>
#include <ReflectionTool\MetaProperty.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Containers;

	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API HeaderParser : public CodePageParser
		{
		public:
			HeaderParser(const String &Text) :
				m_BlockLevel(0),
				CodePageParser(Text),
				m_CurrentDataStructure(nullptr)
			{
			}
			virtual ~HeaderParser(void)
			{
				//m_Namespaces.Clear();
			}

			virtual void Parse(TypeList &Types) override;

		protected:
			virtual bool CompileDeclaration(TypeList &Types, Token &Token);
			virtual void CompileTypeDeclaration(const Token &Declaration, TypeList &Types);
			virtual void CompileStructDeclaration(const Token &Declaration, TypeList &Types);
			virtual void CompileEnumDeclaration(TypeList &Types);
			virtual void CompileConstructorDeclaration(void);
			virtual void CompileFunctionDeclaration(void);
			virtual void CompileVariableDeclaration(void);

			virtual AccessSpecifiers GetAccessSpecifier(Token &Token);

			virtual void AddBlockLevel(void)
			{
				m_BlockLevel++;
			}

			virtual void AddNamespace(void);

			String GetNamespaces(void) const;

		private:
			uint16 m_BlockLevel;
			MetaDataStructure *m_CurrentDataStructure;
			StringList m_Namespaces;
		};
	}
}
#endif
