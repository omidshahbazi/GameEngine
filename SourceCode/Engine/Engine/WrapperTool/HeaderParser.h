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
		const String PRIVATE_TEXT(STRINGIZE(private));
		const String STATIC_TEXT(STRINGIZE(static));
		const String VOID_TEXT(STRINGIZE(void));
		const String CONST_TEXT(STRINGIZE(const));
		const String THIS_TEXT(STRINGIZE(this));
		const String INLINE_TEXT(STRINGIZE(INLINE));
		const String WRAPPER_OBJECT_TEXT(STRINGIZE(WRAPPER_OBJECT));
		const String SINGLETON_DECLARATION_TEXT(STRINGIZE(SINGLETON_DECLARATION));

		const String CS_POINTER_TEXT = "System.IntPtr";

		class WRAPPERTOOL_API HeaderParser : public Tokenizer
		{
		private:
			struct ParamaterInfo
			{
			public:
				bool IsPointer;
				String Type;
				String Name;
			};

			typedef Vector<ParamaterInfo> ParamaterInfoList;

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

			virtual bool Parse(StringStream& HeaderStream, StringStream& CSStream);

		protected:
			virtual bool CompileDeclaration(StringStream& HeaderStream, Token& DelarationToken);

			bool CompileTypeDeclaration(StringStream& HeaderStream, Token& DeclarationToke);

			bool CompileFunctionDeclaration(StringStream& HeaderStream, const String& FullQualifiedTypeName, const String& TypeName, Token& DeclarationToken);

			bool CompileUsingNamespaceDeclaration(StringStream& HeaderStream, Token& DeclarationToken);

			bool CompileNamespace(StringStream& HeaderStream, Token& DeclarationToken);

			//bool CompileForwardDeclaration(StringStream& HeaderStream, Token& DeclarationToken);

			void AddExportFunction(StringStream& Stream, const String& FullQualifiedTypeName, const String& TypeName, const String& Name, const StringList& ReturnTypeIdentifiers, const ParamaterInfoList& Parameters, bool AddInstanceParameter);

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

			StringStream m_CSNameUsingNamespaces;
			StringStream m_CSTypeDeclaration;
		};
	}
}
#endif