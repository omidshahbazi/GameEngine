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
		const String TEMPLATE_TEXT(STRINGIZE(template));
		const String TYPEDEF_TEXT(STRINGIZE(typedef));
		const String PUBLIC_TEXT(STRINGIZE(public));
		const String PRIVATE_TEXT(STRINGIZE(private));
		const String STATIC_TEXT(STRINGIZE(static));
		const String VOID_TEXT(STRINGIZE(void));
		const String CONST_TEXT(STRINGIZE(const));
		const String THIS_TEXT(STRINGIZE(this));
		const String INLINE_TEXT(STRINGIZE(INLINE));
		const String WRAPPER_OBJECT_TEXT(STRINGIZE(WRAPPER_OBJECT));
		const String WRAPPER_DATA_STRUCTURE_TEXT(STRINGIZE(WRAPPER_DATA_STRUCTURE));
		const String SINGLETON_DECLARATION_TEXT(STRINGIZE(SINGLETON_DECLARATION));
		const String LESS_THAN('<');
		const String GREATER_THAN('>');

		const String CS_POINTER_TEXT = "System.IntPtr";

		class WRAPPERTOOL_API HeaderParser : public Tokenizer
		{
		private:
			struct DataTypeInfo
			{
			public:
				bool IsPointer = false;
				bool IsReference = false;
				String Type;
				StringList TemplateArguments;
			};

			struct ParamaterInfo
			{
			public:
				DataTypeInfo DataType;
				String Name;
			};

			typedef Vector<ParamaterInfo> ParameterInfoList;

			enum class CompileResults
			{
				Done = 0,
				Reject = 1,
				Failed = 2
			};

			enum class AccessSpecifiers
			{
				None = 0,
				Public = 1,
				NonPublic = 2
			};

		public:
			HeaderParser(const String& Text, const String& BinaryFileName, const String& ModuleAPI) :
				Tokenizer(Text),
				m_BinaryFileName(BinaryFileName),
				m_ModuleAPI(ModuleAPI)
			{
			}

			virtual bool Parse(StringStream& HeaderStream, StringStream& CSStream);

		protected:
			virtual bool CompileDeclaration(StringStream& HeaderStream, Token& DelarationToken);

			bool CompileTypeDeclaration(StringStream& HeaderStream, Token& DeclarationToke);

			bool CompileDataStructureDeclaration(StringStream& HeaderStream, Token& DeclarationToke);

			bool CompileTyoeDefDataStructureDeclaration(StringStream& HeaderStream, Token& DeclarationToke);

			bool CompileWrapperFunctionDeclaration(StringStream& HeaderStream, const String& FullQualifiedTypeName, const String& TypeName, Token& DeclarationToken);

			CompileResults CompileDataStructureFunctionDeclaration(StringStream& HeaderStream, Token& DeclarationToken);

			CompileResults CompileDataStructureVariableDeclaration(StringStream& HeaderStream, AccessSpecifiers AccessSpecifier, Token& DeclarationToken);

			bool CompileUsingNamespaceDeclaration(StringStream& HeaderStream, Token& DeclarationToken);

			bool CompileNamespace(StringStream& HeaderStream, Token& DeclarationToken);

			//bool CompileForwardDeclaration(StringStream& HeaderStream, Token& DeclarationToken);

			bool CompileDataType(DataTypeInfo& DataType, Token& DeclarationToken);

			void AddExportFunction(StringStream& Stream, const String& FullQualifiedTypeName, const String& TypeName, const String& Name, const DataTypeInfo& ReturnType, const ParameterInfoList& Parameters, bool AddInstanceParameter, const String& Additional = "");

			void AddImportFunction(StringStream& Stream, const String& TypeName, const String& FunctionName, const String& ExportFunctionName, const DataTypeInfo& ReturnType, const ParameterInfoList& Parameters, bool AddInstanceParameter);

			bool SkipScope(void);

			AccessSpecifiers GetAccessSpecifier(Token& Token);

			bool ReadSpecifier(Token& DeclarationToken);
			bool ReadTypeName(Token& Token);

			bool RequiredToken(const String& Value);

			void AddQualifier(const String& Name);

			void RemoveLastQualifier(void);

			String GetQualifiers(void) const;

			String GetCPPType(DataTypeInfo DataType);

			String GetCSType(DataTypeInfo DataType);

		private:
			String m_BinaryFileName;
			String m_ModuleAPI;
			StringList m_Qualifiers;

			StringStream m_CSNameUsingNamespaces;
			StringStream m_CSTypeDeclaration;
		};
	}
}
#endif