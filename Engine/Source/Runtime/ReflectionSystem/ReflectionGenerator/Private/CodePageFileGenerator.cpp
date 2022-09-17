// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\CodePageFileGenerator.h>
#include <ReflectionGenerator\Private\MetaEnum.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaProperty.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace Platform;

	namespace ReflectionGenerator
	{
		const cstr CodePageFileGenerator::FILE_HEADER = "// Copyright 2016-2020 ?????????????. All Rights Reserved.\n// This file has generated by the ReflectionGenerator module";

		void CodePageFileGenerator::GenerateDataType(const DataType& Type, const String& VariableName, String& Content)
		{
			const String DATA_TYPE(STRINGIZE(Engine::Reflection::DataType));

#define ADD_NEW_LINE() Content += "\n"

			Content += "		" + DATA_TYPE + " " + VariableName + ";";
			ADD_NEW_LINE();

			Content += "		" + VariableName + ".SetValueType(" + GetValueType(Type.GetValueType()) + ");";
			ADD_NEW_LINE();

			Content += "		" + VariableName + ".SetPassType(" + GetPassType(Type.GetPassType()) + ");";
			ADD_NEW_LINE();

			Content += "		" + VariableName + ".SetExtraValueType(\"" + Type.GetExtraValueType() + "\");";
			ADD_NEW_LINE();

			Content += "		" + VariableName + ".SetIsConst(" + StringUtility::ToString<char8>(Type.GetIsConst()).ToLower() + ");";
			ADD_NEW_LINE();

			Content += "		" + VariableName + ".SetIsConstValue(" + StringUtility::ToString<char8>(Type.GetIsConstValue()).ToLower() + ");";
			ADD_NEW_LINE();

#undef ADD_NEW_LINE
		}

		String CodePageFileGenerator::GetPassType(DataType::PassesTypes Type)
		{
			switch (Type)
			{
			case DataType::PassesTypes::Pointer:
				return STRINGIZE(Engine::Reflection::DataType::PassesTypes::Pointer);

			case DataType::PassesTypes::Reference:
				return STRINGIZE(Engine::Reflection::DataType::PassesTypes::Reference);

			case DataType::PassesTypes::Value:
				return STRINGIZE(Engine::Reflection::DataType::PassesTypes::Value);

			default:
				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
			}
		}

		String CodePageFileGenerator::GetAccessSpecifier(AccessSpecifiers Access)
		{
			switch (Access)
			{
			case AccessSpecifiers::Private:
				return STRINGIZE(Engine::Reflection::AccessSpecifiers::Private);

			case AccessSpecifiers::Protected:
				return STRINGIZE(Engine::Reflection::AccessSpecifiers::Protected);

			case AccessSpecifiers::Public:
				return STRINGIZE(Engine::Reflection::AccessSpecifiers::Public);

			default:
				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
			}
		}

		String CodePageFileGenerator::GetValueType(ValueTypes Type)
		{
			switch (Type)
			{
			case ValueTypes::None:
				return STRINGIZE(Engine::Common::ValueTypes::None);

			case ValueTypes::Void:
				return STRINGIZE(Engine::Common::ValueTypes::Void);

			case ValueTypes::Bool:
				return STRINGIZE(Engine::Common::ValueTypes::Bool);

			case ValueTypes::UInt8:
				return STRINGIZE(Engine::Common::ValueTypes::UInt8);
			case ValueTypes::UInt16:
				return STRINGIZE(Engine::Common::ValueTypes::UInt16);
			case ValueTypes::UInt32:
				return STRINGIZE(Engine::Common::ValueTypes::UInt32);
			case ValueTypes::UInt64:
				return STRINGIZE(Engine::Common::ValueTypes::UInt64);

			case ValueTypes::Int8:
				return STRINGIZE(Engine::Common::ValueTypes::Int8);
			case ValueTypes::Int16:
				return STRINGIZE(Engine::Common::ValueTypes::Int16);
			case ValueTypes::Int32:
				return STRINGIZE(Engine::Common::ValueTypes::NoInt32ne);
			case ValueTypes::Int64:
				return STRINGIZE(Engine::Common::ValueTypes::Int64);

			case ValueTypes::Float32:
				return STRINGIZE(Engine::Common::ValueTypes::Float32);
			case ValueTypes::Float64:
				return STRINGIZE(Engine::Common::ValueTypes::Float64);

			case ValueTypes::String:
				return STRINGIZE(Engine::Common::ValueTypes::String);
			case ValueTypes::WString:
				return STRINGIZE(Engine::Common::ValueTypes::WString);

			case ValueTypes::Vector2F:
				return STRINGIZE(Engine::Common::ValueTypes::Vector2F);
			case ValueTypes::Vector3F:
				return STRINGIZE(Engine::Common::ValueTypes::Vector3F);
			case ValueTypes::Matrix4F:
				return STRINGIZE(Engine::Common::ValueTypes::Matrix4F);
			}

			THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
		}

		String CodePageFileGenerator::GetValueTypeName(ValueTypes Type)
		{
			switch (Type)
			{
			case ValueTypes::None:
				return "None";

			case ValueTypes::Void:
				return "Void";

			case ValueTypes::Bool:
				return "Bool";

			case ValueTypes::UInt8:
				return "UInt8";
			case ValueTypes::UInt16:
				return "UInt16";
			case ValueTypes::UInt32:
				return "UInt32";
			case ValueTypes::UInt64:
				return "UInt64";

			case ValueTypes::Int8:
				return "Int8";
			case ValueTypes::Int16:
				return "Int16";
			case ValueTypes::Int32:
				return "NoInt32ne";
			case ValueTypes::Int64:
				return "Int64";

			case ValueTypes::Float32:
				return "Float32";
			case ValueTypes::Float64:
				return "Float64";

			case ValueTypes::String:
				return "String";
			case ValueTypes::WString:
				return "WString";

			case ValueTypes::Vector2F:
				return "Vector2F";
			case ValueTypes::Vector3F:
				return "Vector3F";
			case ValueTypes::Matrix4F:
				return "Matrix4F";
			}

			THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection);
		}

		String CodePageFileGenerator::GetImplementType(Type* Type)
		{
			if (IsTypeOf(Type, MetaObject))
				return ReinterpretCast(MetaObject*, Type)->GetUniqueName() + "_" + STRINGIZE(ImplementObjectType);
			else if (IsTypeOf(Type, MetaEnum))
				return ReinterpretCast(MetaObject*, Type)->GetUniqueName() + "_" + STRINGIZE(ImplementEnumType);
			else if (IsTypeOf(Type, MetaFunction))
				return ReinterpretCast(MetaFunction*, Type)->GetUniqueName() + "_" + STRINGIZE(ImplementFunctionType);
			else if (IsTypeOf(Type, MetaProperty))
				return ReinterpretCast(MetaProperty*, Type)->GetUniqueName() + "_" + STRINGIZE(ImplementPropertyType);

			return "";
		}

		String CodePageFileGenerator::GetImplementTypePointerName(Type* Type)
		{
			if (Type == nullptr)
				return STRINGIZE(nullptr);

			return "g_" + GetImplementType(Type);
		}

		String CodePageFileGenerator::GetRegistryTypeName(ObjectType* Type)
		{
			return ReinterpretCast(MetaObject*, Type)->GetUniqueName() + "_Registry";
		}

		String CodePageFileGenerator::GetRegistryTypePointerName(ObjectType* Type)
		{
			if (Type == nullptr)
				return STRINGIZE(nullptr);

			return "g_" + GetRegistryTypeName(Type);
		}

		void CodePageFileGenerator::WriteToFile(const WString& FilePath, const String& Data)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);

			PlatformFile::Write(handle, Data.GetValue());

			PlatformFile::Close(handle);
		}
	}
}



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
//		THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::Reflection";
//	}
//}

//void CodePageFileGenerator::GetSignature(FunctionType* Function, String& Signature, String& SignatureName)
//{
//	GetSignature(Function->GetReturnType(), Signature";

//	SignatureName = Function->GetName() + "Signature";

//	Signature += "(" + Function->GetTopNest()->GetFullQualifiedName() + "::*" + SignatureName + ")";

//	Signature += "(";

//	bool isFirst = true;
//	for (auto& param : Function->GetParameters())
//	{
//		if (!isFirst)
//			Signature += ", ";
//		isFirst = false;

//		GetSignature(param, Signature";
//	}

//	Signature += ")";

//	if (Function->GetIsConst())
//		Signature += " const";
//}

//void CodePageFileGenerator::GetSignature(const ParameterType& Parameter, String& Signature)
//{
//	GetSignature(Parameter.GetDataType(), Signature";
//}

//String CodePageFileGenerator::GetPointerName(Type* Type)
//{
//	return (IsTypeOf(Type, MetaObject) ? ReinterpretCast(MetaObject*, Type)->GetUniqueName() : Type->GetName()) + "Ptr";
////}
//
//
//String GetArgumentsDataTypeText(const ParameterTypeList& Parameters)
//{
//	String ret;
//
//	if (Parameters.GetSize() != 0)
//	{
//		for (uint8 i = 0; i < Parameters.GetSize("; i++)
//		{
//			const Parameter& param = Parameters[i];
//
//			ret += GetArgumentDataTypeText(i, param.GetDataType()";
//
//			if (i < Parameters.GetSize() - 1)
//				ret += ",";
//		}
//	}
//
//	return ret;
//}
//
//String GetArgumentDataTypeText(int32 Index, const DataType& Type)
//{
//	String ret;
//
//	if (Type.GetValueType() == ValueTypes::None)
//		if (Type.GetPassType() == DataType::PassesTypes::Pointer)
//			ret += "(" + Type.GetExtraValueType() + "*)";
//
//	ret += "(*Arguments)[" + StringUtility::ToString<char8>(Index) + "].GetAs";
//
//	if (Type.GetPassType() == DataType::PassesTypes::Pointer)
//		ret += GetValueTypeText(ValueTypes::VoidPointer";
//	else
//		ret += GetValueTypeText(Type.GetValueType(), false";
//
//	ret += "()";
//
//	return ret;
//}