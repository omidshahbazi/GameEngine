// Copyright 2012-2015 ?????????????. All Rights Reserved.
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
		class HeaderParser : public CodePageParser
		{
		public:
			HeaderParser(const String &Text) :
				CodePageParser(Text),
				m_CurrentDataStructure(nullptr)
			{
			}

			void Parse(Type::TypesList &Types);

		protected:
			virtual bool CompileDeclaration(Type::TypesList &Types, Token &Token);
			virtual void CompileTypeDeclaration(const Token &Declaration, Type::TypesList &Types);
			virtual void CompileStructDeclaration(const Token &Declaration, Type::TypesList &Types);
			virtual void CompileEnumDeclaration(Type::TypesList &Types);
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
			List<String> m_Namespaces;
		};
	}
}
#endif
