// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <JSON\JSONBasic.h>
#include <JSON\JSONObject.h>
#include <JSON\JSONArray.h>
#include <JSON\JSONBasic.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	namespace JSON
	{
		String JSONBasic::ToString(const JSONData* Data)
		{
			String value;

			switch (Data->GetType())
			{
			case JSONData::Types::Object: value += Data->GetObject().ToString(); break;
			case JSONData::Types::Array: value += Data->GetArray().ToString(); break;
			case JSONData::Types::Any:
			{
				const AnyDataType& any = Data->GetAny();

				if (any.GetValueType() == ValueTypes::Void)
					value = "null";
				if (any.GetValueType() == ValueTypes::Bool)
					value = (any.GetAsBool() ? "true" : "false");
				else if (any.GetValueType() == ValueTypes::Float32)
					value = StringUtility::ToString<char8>(any.GetAsFloat32());
				else if (any.GetValueType() == ValueTypes::Float64)
					value = StringUtility::ToString<char8>(any.GetAsFloat64());
				else if (any.GetValueType() == ValueTypes::Int8)
					value = StringUtility::ToString<char8>(any.GetAsInt8());
				else if (any.GetValueType() == ValueTypes::UInt8)
					value = StringUtility::ToString<char8>(any.GetAsUInt8());
				else if (any.GetValueType() == ValueTypes::Int16)
					value = StringUtility::ToString<char8>(any.GetAsInt16());
				else if (any.GetValueType() == ValueTypes::UInt16)
					value = StringUtility::ToString<char8>(any.GetAsUInt16());
				else if (any.GetValueType() == ValueTypes::Int32)
					value = StringUtility::ToString<char8>(any.GetAsInt32());
				else if (any.GetValueType() == ValueTypes::UInt32)
					value = StringUtility::ToString<char8>(any.GetAsUInt32());
				else if (any.GetValueType() == ValueTypes::Int64)
					value = StringUtility::ToString<char8>(any.GetAsInt64());
				else if (any.GetValueType() == ValueTypes::UInt64)
					value = StringUtility::ToString<char8>(any.GetAsUInt64());
				else if (any.GetValueType() == ValueTypes::Int32)
					value = StringUtility::ToString<char8>(any.GetAsInt64());
				else if (any.GetValueType() == ValueTypes::Int64)
					value = StringUtility::ToString<char8>(any.GetAsInt64());
				else if (any.GetValueType() == ValueTypes::String)
					value = '"' + any.GetAsString() + '"';
				else if (any.GetValueType() == ValueTypes::WString)
					value = '"' + any.GetAsWString().ChangeType<char8>() + '"';
			} break;
			}

			return value;
		}
	}
}