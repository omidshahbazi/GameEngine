// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef HEADER_FILE_GENERATOR_H
#define HEADER_FILE_GENERATOR_H
#include <ReflectionGenerator\Private\CodePageFileGenerator.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Containers;

	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API HeaderFileGenerator : protected CodePageFileGenerator
		{
		public:
			static void Generate(const TypeList& Types, const WString& FilePath, const WString& OutputFilePath);

		private:
			static void GenerateHeaderCode(const TypeList& Types, String& Content);
			static void GenerateTypesCode(const TypeList& Types, String& Content);
			static void GenerateTypeCode(Type* Type, String& Content);
			static void GenerateObjectCode(MetaObject* Type, String& Content);
		};
	}
}
#endif
