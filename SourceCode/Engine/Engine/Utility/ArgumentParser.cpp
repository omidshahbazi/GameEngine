// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <Utility\ArgumentParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Utility
	{
		ArgumentParser::ArgumentParser(uint8 Count, const char **Arguments, bool FirstIsAddress)
		{
			if (FirstIsAddress && Count != 0)
				m_Address = Arguments[0];

			for (uint8 i = FirstIsAddress ? 1 : 0; i < Count; i++)
				m_Arguments.Add(String(Arguments[i]).TrimAll());
		}

		int8 ArgumentParser::GetAsInt8(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseInt8();
		}

		int16 ArgumentParser::GetAsInt16(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseInt16();
		}

		int32 ArgumentParser::GetAsInt32(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseInt32();
		}

		int64 ArgumentParser::GetAsInt64(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseInt64();
		}

		uint8 ArgumentParser::GetAsUInt8(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseUInt8();
		}

		uint16 ArgumentParser::GetAsUInt16(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseUInt16();
		}

		uint32 ArgumentParser::GetAsUInt32(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseUInt32();
		}

		uint64 ArgumentParser::GetAsUInt64(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseUInt64();
		}

		float32 ArgumentParser::GetAsFloat32(uint8 Index) const
		{
			Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

			return m_Arguments[Index].ParseFloat32();
		}

		const String &ArgumentParser::GetAsString(const String &Parameter) const
		{
			const int8 index = m_Arguments.Find(Parameter);

			if (index != -1 && index + 1 < m_Arguments.GetSize())
				return m_Arguments[index + 1];

			return "";
		}

		int8 ArgumentParser::GetAsInt8(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseInt8();
		}

		int16 ArgumentParser::GetAsInt16(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseInt16();
		}

		int32 ArgumentParser::GetAsInt32(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseInt32();
		}

		int64 ArgumentParser::GetAsInt64(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseInt64();
		}

		uint8 ArgumentParser::GetAsUInt8(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseUInt8();
		}

		uint16 ArgumentParser::GetAsUInt16(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseUInt16();
		}

		uint32 ArgumentParser::GetAsUInt32(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseUInt32();
		}

		uint64 ArgumentParser::GetAsUInt64(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseUInt64();
		}

		float32 ArgumentParser::GetAsFloat32(const String &Parameter) const
		{
			return GetAsString(Parameter).ParseFloat32();
		}
	}
}