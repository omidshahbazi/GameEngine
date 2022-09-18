// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef COMPILE_FILE_GENERATOR_H
#define COMPILE_FILE_GENERATOR_H
#include <ReflectionGenerator\Private\CodePageFileGenerator.h>
#include <ReflectionGenerator\Private\MetaEnum.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaProperty.h>

namespace Engine
{
	using namespace Allocators;
	using namespace Reflection;
	using namespace Containers;

	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API CompileFileGenerator : protected CodePageFileGenerator
		{
		public:
			static void Generate(const TypeList& Types, const WString& FilePath, const WString& OutputFilePath);

		private:
			static void GenerateHeaderCode(const TypeList& Types, const String& FilePath, const String& HeaderFilePath, String& Content);
			static void GenerateTypesCode(const TypeList& Types, String& Content);
			static void GenerateForwardDeclarationsCode(const TypeList& Types, String& Content);
			static void GenerateTypeCode(Type* Type, String& Content);
			static void GenerateObjectCode(MetaObject* Type, String& Content);
			static void GenerateEnumCode(MetaEnum* Type, String& Content);
			static void GenerateFunctionCode(MetaFunction* Type, String& Content);
			static void GenerateArgumentListCode(const ParameterTypeList& Parameters, String& Content);
			static void GeneratePropertyCode(MetaProperty* Type, String& Content);
			static void GenerateNamespaceHeader(Type* Type, String& Content);
			static void GenerateNamespaceFooter(Type* Type, String& Content);
		};
	}
}
#endif
