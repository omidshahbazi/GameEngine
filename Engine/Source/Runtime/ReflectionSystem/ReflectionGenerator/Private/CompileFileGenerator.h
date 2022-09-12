//// Copyright 2016-2020 ?????????????. All Rights Reserved.
//#ifndef COMPILE_FILE_GENERATOR_H
//#define COMPILE_FILE_GENERATOR_H
//#include <ReflectionGenerator\Private\CodePageFileGenerator.h>
//
//namespace Engine
//{
//	using namespace Allocators;
//	using namespace Reflection;
//	using namespace Containers;
//
//	namespace ReflectionGenerator
//	{
//		class REFLECTIONGENERATOR_API CompileFileGenerator : protected CodePageFileGenerator
//		{
//		public:
//			static void Generate(const TypeList& Types, const WString& OutputFilePath);
//
//		private:
//			static void GenerateCompileCode(const TypeList& Types, String& Content);
//			static void GenerateDataStructuresDefinition(String& RootContent, String& ConstructorContents, String& DestructorContents, String& FunctionsDefinition, const TypeList& Types, AccessSpecifiers Access);
//			static void GenerateConstructorsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access);
//			static void GenerateParentsNameDefinition(String& Content, Type* Type, const StringList& ParentsName, AccessSpecifiers Access);
//			static void GenerateFunctionsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access);
//			static void GenerateVariablesDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access);
//		};
//
//	}
//}
//#endif
