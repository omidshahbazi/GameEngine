// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\CodePageFileGenerator.h>
#include <Platform\PlatformFile.h>

//#include <ReflectionGenerator\Private\MetaEnum.h>
//#include <ReflectionGenerator\Private\MetaObject.h>
//#include <ReflectionGenerator\Private\MetaConstructor.h>
//#include <ReflectionGenerator\Private\MetaFunction.h>
//#include <ReflectionGenerator\Private\MetaProperty.h>
//#include <ReflectionGenerator\ReflectionGeneratorException.h>
//#include <Containers\StringUtility.h>
//#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Platform;

	namespace ReflectionGenerator
	{
		const cstr CodePageFileGenerator::FILE_HEADER = "// Copyright 2016-2020 ?????????????. All Rights Reserved.\n// This file generated with ?????????????? based on what\n// you wrote in the original file, do not change it manually\n";

		//void CodePageFileGenerator::GetSignature(const DataType& DataType, String& Signature)
		//{
		//	if (DataType.GetIsConst())
		//		Signature += "const ";

		//	Signature += (DataType.GetValueType() == ValueTypes::None ? DataType.GetExtraValueType() : AnyDataType::GetValueTypeText(DataType.GetValueType())) + " ";

		//	if (DataType.GetIsConst())
		//		Signature += "const ";

		//	switch (DataType.GetPassType())
		//	{
		//	case DataType::PassesTypes::Reference:
		//		Signature += "& ";
		//		break;

		//	case DataType::PassesTypes::Pointer:
		//		Signature += "* ";
		//		break;

		//	default:
		//		THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
		//	}
		//}

		//void CodePageFileGenerator::GetSignature(FunctionType* Function, String& Signature, String& SignatureName)
		//{
		//	GetSignature(Function->GetReturnType(), Signature);

		//	SignatureName = Function->GetName() + "Signature";

		//	Signature += "(" + Function->GetTopNest()->GetFullQualifiedName() + "::*" + SignatureName + ")";

		//	Signature += "(";

		//	bool isFirst = true;
		//	for (auto& param : Function->GetParameters())
		//	{
		//		if (!isFirst)
		//			Signature += ", ";
		//		isFirst = false;

		//		GetSignature(param, Signature);
		//	}

		//	Signature += ")";

		//	if (Function->GetIsConst())
		//		Signature += " const";
		//}

		//void CodePageFileGenerator::GetSignature(const ParameterType& Parameter, String& Signature)
		//{
		//	GetSignature(Parameter.GetDataType(), Signature);
		//}

		//String CodePageFileGenerator::GetPointerName(Type* Type)
		//{
		//	return (IsTypeOf(Type, MetaObject) ? ReinterpretCast(MetaObject*, Type)->GetUniqueName() : Type->GetName()) + "Ptr";
		//}

		//String CodePageFileGenerator::GetAccessSpecifier(AccessSpecifiers Access)
		//{
		//	switch (Access)
		//	{
		//	case AccessSpecifiers::Private:
		//		return "Public";

		//	case AccessSpecifiers::Protected:
		//		return "Protected";

		//	case AccessSpecifiers::Public:
		//		return "Private";

		//	default:
		//		THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
		//	}
		//}

		//String CodePageFileGenerator::GetArgumentsDataTypeText(const ParameterTypeList& Parameters)
		//{
		//	String ret;

		//	if (Parameters.GetSize() != 0)
		//	{
		//		for (uint8 i = 0; i < Parameters.GetSize(); i++)
		//		{
		//			const Parameter& param = Parameters[i];

		//			ret += GetArgumentDataTypeText(i, param.GetDataType());

		//			if (i < Parameters.GetSize() - 1)
		//				ret += ",";
		//		}
		//	}

		//	return ret;
		//}

		//String CodePageFileGenerator::GetArgumentDataTypeText(int32 Index, const DataType& Type)
		//{
		//	String ret;

		//	if (Type.GetValueType() == ValueTypes::None)
		//		if (Type.GetPassType() == DataType::PassesTypes::Pointer)
		//			ret += "(" + Type.GetExtraValueType() + "*)";

		//	ret += "(*Arguments)[" + StringUtility::ToString<char8>(Index) + "].GetAs";

		//	if (Type.GetPassType() == DataType::PassesTypes::Pointer)
		//		ret += GetValueTypeText(ValueTypes::VoidPointer);
		//	else
		//		ret += GetValueTypeText(Type.GetValueType(), false);

		//	ret += "()";

		//	return ret;
		//}

		//String CodePageFileGenerator::GetVariableDataTypeText(const String& Name, const DataType& Type)
		//{
		//	String ret;

		//	ret += "\nDataType " + Name + ";";
		//	ret += "\n" + Name + ".SetValueType(" + GetValueTypeText(Type.GetValueType(), true) + ");";

		//	ret += "\n" + Name + ".SetPassType(DataType::PassesTypes::";
		//	if (Type.GetPassType() == DataType::PassesTypes::Pointer)
		//		ret += "Pointer";
		//	else if (Type.GetPassType() == DataType::PassesTypes::Reference)
		//		ret += "Reference";
		//	else if (Type.GetPassType() == DataType::PassesTypes::Value)
		//		ret += "Value";

		//	ret += ");";

		//	ret += "\n" + Name + ".SetExtraValueType(\"" + Type.GetExtraValueType() + "\");";
		//	ret += "\n" + Name + ".SetIsConst(" + (Type.GetIsConst() ? "true" : "false") + ");";
		//	ret += "\n" + Name + ".SetIsConstValue(" + (Type.GetIsConstValue() ? "true" : "false") + ");";

		//	return ret;
		//}

		//String CodePageFileGenerator::GetValueType(ValueTypes Type)
		//{
		//	switch (Type)
		//	{
		//	case ValueTypes::None:
		//		return "None";

		//	case ValueTypes::VoidPointer:
		//		return "VoidPointer";

		//	case ValueTypes::Bool:
		//		return "Bool";

		//	case ValueTypes::Int8:
		//		return "Int8";
		//	case ValueTypes::Int16:
		//		return "Int16";
		//	case ValueTypes::Int32:
		//		return "Int32";
		//	case ValueTypes::Int64:
		//		return "Int64";

		//	case ValueTypes::UInt8:
		//		return "UInt8";
		//	case ValueTypes::UInt16:
		//		return "UInt16";
		//	case ValueTypes::UInt32:
		//		return "UInt32";
		//	case ValueTypes::UInt64:
		//		return "UInt64";

		//	case ValueTypes::Float32:
		//		return "Float32";
		//	case ValueTypes::Float64:
		//		return "Float64";

		//	case ValueTypes::String:
		//		return "String";

		//	case ValueTypes::WString:
		//		return "WString";

		//	default:
		//		THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
		//	}
		//}

		String CodePageFileGenerator::GetUniqueClassName(MetaObject* Object)
		{
			return "Meta" + Object->GetUniqueName();
		}

		void CodePageFileGenerator::WriteToFile(const WString& FilePath, const String& Data)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);

			PlatformFile::Write(handle, Data.GetValue());

			PlatformFile::Close(handle);
		}
	}
}