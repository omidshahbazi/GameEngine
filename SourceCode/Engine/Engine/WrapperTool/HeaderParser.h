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
		const String USING_TEXT(STRINGIZE(using));
		const String NAMESPACE_TEXT(STRINGIZE(namespace));
		const String CLASS_TEXT(STRINGIZE(class));
		const String STRUCT_TEXT(STRINGIZE(struct));
		const String PUBLIC_TEXT(STRINGIZE(public));
		const String VOID_TEXT(STRINGIZE(void));
		const String INLINE_TEXT(STRINGIZE(INLINE));
		const String WRAPPER_OBJECT_TEXT(STRINGIZE(WRAPPER_OBJECT));
		const String SINGLETON_DECLARATION_TEXT(STRINGIZE(SINGLETON_DECLARATION));

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
			HeaderParser(const String& Text, const String& ModuleAPI) :
				Tokenizer(Text),
				m_ModuleAPI(ModuleAPI)
			{
			}

			virtual bool Parse(StringStream& Stream);

		protected:
			virtual bool CompileDeclaration(StringStream& Stream, Token& DelarationToken);

			bool CompileTypeDeclaration(StringStream& Stream, Token& DeclarationToke);

			bool CompileFunctionDeclaration(StringStream& Stream, const String& FullQualifiedTypeName, const String& TypeName, Token& DeclarationToken);

			bool CompileUsingNamespaceDeclaration(StringStream& Stream, Token& DeclarationToken);

			bool CompileNamespace(StringStream& Stream, Token& DeclarationToken);

			//bool CompileForwardDeclaration(StringStream& Stream, Token& DeclarationToken);

			AccessSpecifiers GetAccessSpecifier(Token& Token);

			bool ReadSpecifier(Token& DeclarationToken);
			bool ReadTypeName(Token& Token);

			bool RequiredToken(const String& Value);

			void AddQualifier(const String& Name);

			void RemoveLastQualifier(void);

			String GetQualifiers(void) const;

		private:
			String m_ModuleAPI;
			StringList m_Qualifiers;
		};
	}
}
#endif