// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CODE_PAGE_FILE_GENERATOR_H
#define CODE_PAGE_FILE_GENERATOR_H
#include <ReflectionGenerator\ReflectionGeneratorException.h>
#include <ReflectionGenerator\Private\MetaObject.h>
#include <ReflectionGenerator\Private\MetaEnum.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaProperty.h>
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

			static void GenerateSignature(const DataType& Type, String& Content);
			static void GenerateSignature(FunctionType* Type, String& Content);

			static uint32 GetSignatureID(FunctionType* Type);

			static String GetPassType(DataType::PassesTypes Type);
			static String GetAccessSpecifier(AccessSpecifiers Access);

			static String GetValueType(ValueTypes Type);
			static String GetValueTypeType(ValueTypes Type);

			static String GetUniqueName(ObjectType* Type);
			static String GetUniqueName(EnumType* Type);
			static String GetUniqueName(FunctionType* Type);
			static String GetUniqueName(PropertyType* Type);
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
