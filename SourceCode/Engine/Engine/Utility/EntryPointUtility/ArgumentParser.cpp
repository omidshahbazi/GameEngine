// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EntryPointUtility\ArgumentParser.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace EntryPointUtility
	{
		ArgumentParser::ArgumentParser(uint8 Count, const char** Arguments, bool FirstIsAddress)
		{
			if (FirstIsAddress && Count != 0)
				m_Address = Arguments[0];

			for (uint8 i = FirstIsAddress ? 1 : 0; i < Count; i++)
				m_Arguments.Add(String(Arguments[i]).TrimAll());
		}

		int8 ArgumentParser::GetAsInt8(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToInt8(m_Arguments[Index]);
		}

		int16 ArgumentParser::GetAsInt16(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToInt16(m_Arguments[Index]);
		}

		int32 ArgumentParser::GetAsInt32(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToInt32(m_Arguments[Index]);
		}

		int64 ArgumentParser::GetAsInt64(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToInt64(m_Arguments[Index]);
		}

		uint8 ArgumentParser::GetAsUInt8(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToUInt8(m_Arguments[Index]);
		}

		uint16 ArgumentParser::GetAsUInt16(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToUInt16(m_Arguments[Index]);
		}

		uint32 ArgumentParser::GetAsUInt32(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToUInt32(m_Arguments[Index]);
		}

		uint64 ArgumentParser::GetAsUInt64(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToUInt64(m_Arguments[Index]);
		}

		float32 ArgumentParser::GetAsFloat32(uint8 Index) const
		{
			THROW_IF_EXCEPTION(Categories::Utilities, Index >= m_Arguments.GetSize(), "Index must be less than Arguments count");

			return StringUtility::ToFloat32(m_Arguments[Index]);
		}

		const String& ArgumentParser::GetAsString(const String& Parameter) const
		{
			const int8 index = m_Arguments.Find(Parameter);

			if (index != -1 && index + 1 < m_Arguments.GetSize())
				return m_Arguments[index + 1];

			return "";
		}

		int8 ArgumentParser::GetAsInt8(const String& Parameter) const
		{
			return StringUtility::ToInt8(GetAsString(Parameter));
		}

		int16 ArgumentParser::GetAsInt16(const String& Parameter) const
		{
			return StringUtility::ToInt16(GetAsString(Parameter));
		}

		int32 ArgumentParser::GetAsInt32(const String& Parameter) const
		{
			return StringUtility::ToInt32(GetAsString(Parameter));
		}

		int64 ArgumentParser::GetAsInt64(const String& Parameter) const
		{
			return StringUtility::ToInt64(GetAsString(Parameter));
		}

		uint8 ArgumentParser::GetAsUInt8(const String& Parameter) const
		{
			return StringUtility::ToUInt8(GetAsString(Parameter));
		}

		uint16 ArgumentParser::GetAsUInt16(const String& Parameter) const
		{
			return StringUtility::ToUInt16(GetAsString(Parameter));
		}

		uint32 ArgumentParser::GetAsUInt32(const String& Parameter) const
		{
			return StringUtility::ToUInt32(GetAsString(Parameter));
		}

		uint64 ArgumentParser::GetAsUInt64(const String& Parameter) const
		{
			return StringUtility::ToUInt64(GetAsString(Parameter));
		}

		float32 ArgumentParser::GetAsFloat32(const String& Parameter) const
		{
			return StringUtility::ToFloat32(GetAsString(Parameter));
		}
	}
}