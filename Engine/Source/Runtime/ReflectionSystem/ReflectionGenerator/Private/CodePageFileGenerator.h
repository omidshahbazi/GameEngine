// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CODE_PAGE_FILE_GENERATOR_H
#define CODE_PAGE_FILE_GENERATOR_H
#include <ReflectionGenerator\ReflectionGeneratorException.h>
#include <ReflectionGenerator\Private\MetaObject.h>
#include <Reflection\FunctionType.h>
#include <Reflection\Definitions.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Allocators;
	using namespace Reflection;
	using namespace Containers;

	namespace ReflectionGenerator
	{
		using namespace Private;

		class REFLECTIONGENERATOR_API CodePageFileGenerator
		{
		protected:
			static const cstr FILE_HEADER;

		protected:
			static void GenerateDataType(const DataType& Type, const String& VariableName, String& Content);

			static String GetPassType(DataType::PassesTypes Type);
			static String GetAccessSpecifier(AccessSpecifiers Access);

			static String GetValueType(ValueTypes Type);
			static String GetValueTypeType(ValueTypes Type);

			static String GetUniqueName(Type* Type);
			static String GetImplementType(Type* Type);
			static String GetImplementTypePointerName(Type* Type);
			static String GetRegistryTypeName(Type* Type);
			static String GetRegistryTypePointerName(Type* Type);

			static void WriteToFile(const WString& FilePath, const String& Data);
		};
	}
}
#endif
