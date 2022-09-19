// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\CodePageFileGenerator.h>
#include <ReflectionGenerator\Private\MetaEnum.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaProperty.h>
#include <Reflection\Private\ImplementDataType.h>
#include <Debugging\CoreDebug.h>
#include <Platform\PlatformFile.h>
#include <DataUtility\Hash.h>

namespace Engine
{
	using namespace Platform;
	using namespace DataUtility;

	namespace ReflectionGenerator
	{
		const cstr CodePageFileGenerator::FILE_HEADER = "// Copyright 2016-2020 ?????????????. All Rights Reserved.\n// This file has generated by the ReflectionGenerator module";

		void CodePageFileGenerator::GenerateDataType(const DataType& Type, const String& VariableName, String& Content)
		{
			const String DATA_TYPE(STRINGIZE(ImplementDataType));

#define ADD_NEW_LINE() Content += "\n"

			Content += "			" + DATA_TYPE + " " + VariableName + ";";
			ADD_NEW_LINE();

			Content += "			" + VariableName + ".SetValueType(" + GetValueType(Type.GetValueType()) + ");";
			ADD_NEW_LINE();

			Content += "			" + VariableName + ".SetPassType(" + GetPassType(Type.GetPassType()) + ");";
			ADD_NEW_LINE();

			Content += "			" + VariableName + ".SetExtraValueType(\"" + Type.GetExtraValueType() + "\");";
			ADD_NEW_LINE();

			Content += "			" + VariableName + ".SetIsConst(" + StringUtility::ToString<char8>(Type.GetIsConst()).ToLower() + ");";
			ADD_NEW_LINE();

			Content += "			" + VariableName + ".SetIsConstValue(" + StringUtility::ToString<char8>(Type.GetIsConstValue()).ToLower() + ");";
			ADD_NEW_LINE();

			Content += "			" + VariableName + ".SetIsTemplate(" + StringUtility::ToString<char8>(Type.GetIsTemplate()).ToLower() + ");";
			ADD_NEW_LINE();

			uint32 index = 0;
			for (auto templateParam : Type.GetTemplateParameters())
			{
				const String varName = VariableName + "_Template_" + StringUtility::ToString<char8>(index++);

				GenerateDataType(templateParam, varName, Content);

				Content += "			" + VariableName + ".AddTemplateParameter(" + varName + ");";
				ADD_NEW_LINE();
			}

#undef ADD_NEW_LINE
		}

		void CodePageFileGenerator::GenerateSignature(const DataType& Type, String& Content)
		{
			if (Type.GetIsConst())
				Content += "const ";

			if (Type.GetValueType() == ValueTypes::None)
				Content += Type.GetExtraValueType();
			else
				Content += GetValueTypeType(Type.GetValueType());

			if (Type.GetPassType() == DataType::PassesTypes::Pointer)
				Content += "*";
			else if (Type.GetPassType() == DataType::PassesTypes::Reference)
				Content += "&";

			if (Type.GetIsConstValue())
				Content += " const";
		}

		void CodePageFileGenerator::GenerateSignature(MetaFunction* Type, String& Content)
		{
			GenerateSignature(Type->GetReturnType(), Content);

			Content += "(";

			bool isFirstOne = true;
			for (auto& param : Type->GetParameters())
			{
				if (!isFirstOne)
					Content += ", ";
				isFirstOne = false;

				GenerateSignature(param.GetDataType(), Content);
			}

			Content += ")";
		}

		uint32 CodePageFileGenerator::GetSignatureID(MetaFunction* Type)
		{
			String content;
			GenerateSignature(Type, content);

			return Hash::CRC32(content.GetValue(), content.GetLength());
		}

		String CodePageFileGenerator::GetPassType(DataType::PassesTypes Type)
		{
			switch (Type)
			{
			case DataType::PassesTypes::Pointer:
				return STRINGIZE(DataType::PassesTypes::Pointer);

			case DataType::PassesTypes::Reference:
				return STRINGIZE(DataType::PassesTypes::Reference);

			case DataType::PassesTypes::Value:
				return STRINGIZE(DataType::PassesTypes::Value);
			}

			CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
		}

		String CodePageFileGenerator::GetAccessSpecifier(AccessSpecifiers Access)
		{
			switch (Access)
			{
			case AccessSpecifiers::Private:
				return STRINGIZE(AccessSpecifiers::Private);

			case AccessSpecifiers::Protected:
				return STRINGIZE(AccessSpecifiers::Protected);

			case AccessSpecifiers::Public:
				return STRINGIZE(AccessSpecifiers::Public);
			}

			CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
		}

		String CodePageFileGenerator::GetValueType(ValueTypes Type)
		{
			switch (Type)
			{
			case ValueTypes::None:
				return STRINGIZE(ValueTypes::None);

			case ValueTypes::Void:
				return STRINGIZE(ValueTypes::Void);

			case ValueTypes::Bool:
				return STRINGIZE(ValueTypes::Bool);

			case ValueTypes::UInt8:
				return STRINGIZE(ValueTypes::UInt8);
			case ValueTypes::UInt16:
				return STRINGIZE(ValueTypes::UInt16);
			case ValueTypes::UInt32:
				return STRINGIZE(ValueTypes::UInt32);
			case ValueTypes::UInt64:
				return STRINGIZE(ValueTypes::UInt64);

			case ValueTypes::Int8:
				return STRINGIZE(ValueTypes::Int8);
			case ValueTypes::Int16:
				return STRINGIZE(ValueTypes::Int16);
			case ValueTypes::Int32:
				return STRINGIZE(ValueTypes::Int32);
			case ValueTypes::Int64:
				return STRINGIZE(ValueTypes::Int64);

			case ValueTypes::Float32:
				return STRINGIZE(ValueTypes::Float32);
			case ValueTypes::Float64:
				return STRINGIZE(ValueTypes::Float64);

			case ValueTypes::String:
				return STRINGIZE(ValueTypes::String);
			case ValueTypes::WString:
				return STRINGIZE(ValueTypes::WString);

			case ValueTypes::Vector2F:
				return STRINGIZE(ValueTypes::Vector2F);
			case ValueTypes::Vector3F:
				return STRINGIZE(ValueTypes::Vector3F);
			case ValueTypes::Matrix4F:
				return STRINGIZE(ValueTypes::Matrix4F);
			}

			CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
		}

		String CodePageFileGenerator::GetValueTypeType(ValueTypes Type)
		{
			switch (Type)
			{
			case ValueTypes::None:
			case ValueTypes::Void:
				return STRINGIZE(void);

			case ValueTypes::Bool:
				return STRINGIZE(bool);

			case ValueTypes::UInt8:
				return STRINGIZE(uint8);
			case ValueTypes::UInt16:
				return STRINGIZE(uint16);
			case ValueTypes::UInt32:
				return STRINGIZE(uint32);
			case ValueTypes::UInt64:
				return STRINGIZE(uint64);

			case ValueTypes::Int8:
				return STRINGIZE(int8);
			case ValueTypes::Int16:
				return STRINGIZE(int16);
			case ValueTypes::Int32:
				return STRINGIZE(int32);
			case ValueTypes::Int64:
				return STRINGIZE(int64);

			case ValueTypes::Float32:
				return STRINGIZE(float32);
			case ValueTypes::Float64:
				return STRINGIZE(float64);

			case ValueTypes::String:
				return STRINGIZE(String);
			case ValueTypes::WString:
				return STRINGIZE(WString);

			case ValueTypes::Vector2F:
				return STRINGIZE(Vector2F);
			case ValueTypes::Vector3F:
				return STRINGIZE(Vector3F);
			case ValueTypes::Matrix4F:
				return STRINGIZE(Matrix4F);
			}

			CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
		}

		String CodePageFileGenerator::GetUniqueName(ObjectType* Type)
		{
			return (Type->GetTopNest() == nullptr ? String::Empty : GetUniqueName(Type->GetTopNest()) + "_") + Type->GetName();
		}

		String CodePageFileGenerator::GetUniqueName(EnumType* Type)
		{
			return (Type->GetTopNest() == nullptr ? String::Empty : GetUniqueName(Type->GetTopNest()) + "_") + Type->GetName();
		}

		String CodePageFileGenerator::GetUniqueName(FunctionType* Type)
		{
			return GetUniqueName(Type->GetTopNest()) + "_" + Type->GetName() + "_" + StringUtility::ToString<char8>(GetSignatureID(ReinterpretCast(MetaFunction*, Type)));
		}

		String CodePageFileGenerator::GetUniqueName(PropertyType* Type)
		{
			return GetUniqueName(Type->GetTopNest()) + "_" + Type->GetName();
		}

		String CodePageFileGenerator::GetUniqueName(Type* Type)
		{
			if (IsTypeOf(Type, MetaObject))
				return GetUniqueName(ReinterpretCast(MetaObject*, Type));
			else if (IsTypeOf(Type, MetaEnum))
				return  GetUniqueName(ReinterpretCast(MetaEnum*, Type));
			else if (IsTypeOf(Type, MetaFunction))
				return GetUniqueName(ReinterpretCast(MetaFunction*, Type));
			else if (IsTypeOf(Type, MetaProperty))
				return  GetUniqueName(ReinterpretCast(MetaProperty*, Type));

			CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
		}

		String CodePageFileGenerator::GetImplementType(Type* Type)
		{
			String uniqueName = GetUniqueName(Type);

			if (IsTypeOf(Type, MetaObject))
				return uniqueName + "_" + STRINGIZE(ImplementObjectType);
			else if (IsTypeOf(Type, MetaEnum))
				return uniqueName + "_" + STRINGIZE(ImplementEnumType);
			else if (IsTypeOf(Type, MetaFunction))
				return uniqueName + "_" + STRINGIZE(ImplementFunctionType);
			else if (IsTypeOf(Type, MetaProperty))
				return uniqueName + "_" + STRINGIZE(ImplementPropertyType);

			return "";
		}

		String CodePageFileGenerator::GetImplementTypePointerName(Type* Type)
		{
			if (Type == nullptr)
				return STRINGIZE(nullptr);

			return "g_" + GetImplementType(Type);
		}

		String CodePageFileGenerator::GetRegistryTypeName(Type* Type)
		{
			return GetUniqueName(Type) + "_Registry";
		}

		String CodePageFileGenerator::GetRegistryTypePointerName(Type* Type)
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