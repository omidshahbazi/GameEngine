// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Utility::Lexer;
	using namespace Containers;

	namespace WrapperTool
	{
		const String NAMESPACE_TEXT(STRINGIZE(namespace));
		const String WRAPPER_OBJECT_TEXT(STRINGIZE(WRAPPER_OBJECT));

		class WRAPPERTOOL_API HeaderParser : public Tokenizer
		{
		private:
			enum class AccessSpecifiers
			{
				None = 0,
				Public = 1,
				NonPublic = 2
			};

		public:
			HeaderParser(const String& Text) :
				Tokenizer(Text)
			{
			}

			virtual bool Parse(StringStream& Stream);

		protected:
			virtual bool CompileDeclaration(StringStream& Stream, Token& DelarationToken);

			bool CompileTypeDeclaration(StringStream& Stream, Token& DeclarationToke);

			AccessSpecifiers GetAccessSpecifier(Token& Token);

			bool ReadSpecifier(Token& DeclarationToken);
			bool ReadTypeName(Token& Token);

			bool RequiredToken(const String& Value);

			void AddNamespace(void);

			void RemoveNamespace(void);

			String GetNamespaces(void) const;

		private:
			StringList m_Namespaces;
		};
	}
}
#endif