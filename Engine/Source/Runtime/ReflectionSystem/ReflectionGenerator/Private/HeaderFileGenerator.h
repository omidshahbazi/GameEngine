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
			static void Generate(const TypeList& Types, const WString& OutputFilePath);

		private:
			static void GenerateHeaderCode(const TypeList& Types, String& Content);
		};
	}
}
#endif
