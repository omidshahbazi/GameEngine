// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CODE_PAGE_FILE_GENERATOR_H
#define CODE_PAGE_FILE_GENERATOR_H
#include <ReflectionGenerator\ReflectionGeneratorException.h>
#include <ReflectionGenerator\Private\MetaObject.h>
#include <Reflection\FunctionType.h>
#include <Reflection\Definitions.h>
#include <Containers\Strings.h>

//#include <Allocators\AllocatorBase.h>
//#include <Reflection\Private\ImplementObjectType.h>
//#include <Reflection\Private\ImplementEnumType.h>
//#include <Reflection\Private\ImplementPropertyType.h>
//#include <ReflectionGenerator\Private\MetaObject.h>
//#include <Containers\StringUtility.h>

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
			static String GetUniqueClassName(MetaObject* Object);

			//static void GetSignature(const DataType& DataType, String& Signature);
			//static void GetSignature(FunctionType* Function, String& Signature, String& SignatureName);
			//static void GetSignature(const ParameterType& Parameter, String& Signature);
			//
			//static String GetPointerName(Type* Type);
			//	   
			//static String GetAccessSpecifier(AccessSpecifiers Access);
			//	   
			//static String GetArgumentsDataTypeText(const ParameterTypeList& Parameters);
			//static String GetArgumentDataTypeText(int32 Index, const DataType& Type);
			//	   
			//static String GetVariableDataTypeText(const String& Name, const DataType& Type);
			//	   
			//static String GetValueType(ValueTypes Type);

			static void WriteToFile(const WString& FilePath, const String& Data);
		};
	}
}
#endif
