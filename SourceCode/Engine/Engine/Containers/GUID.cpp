// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Containers\GUID.h>
#include <Containers\StringUtility.h>
#include <Containers\Exception.h>
#include <Containers\HexUtility.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Containers
	{
		const GUID GUID::Invalid;

		GUID::GUID(void) :
			m_Data({})
		{
		}
		GUID::GUID(const PlatformOS::GUIDData& Data)
		{
			PlatformMemory::Copy(&Data, &m_Data, 1);
		}

		GUID::GUID(const String& Value) :
			m_Data({})
		{
			*this = Value;
		}

		GUID& GUID::operator=(const String& Value)
		{
			THROW_IF_EXCEPTION(Categories::Containers, Value.GetLength() < 36, "Invalid GUID format");
			THROW_IF_EXCEPTION(Categories::Containers, Value[8] != '-' || Value[13] != '-' || Value[18] != '-' || Value[23] != '-', "Invalid GUID format");

			m_Data.Part1[0] = HexUtility::ToByte(Value, 0);
			m_Data.Part1[1] = HexUtility::ToByte(Value, 2);
			m_Data.Part1[2] = HexUtility::ToByte(Value, 4);
			m_Data.Part1[3] = HexUtility::ToByte(Value, 6);

			m_Data.Part2[0] = HexUtility::ToByte(Value, 9);
			m_Data.Part2[1] = HexUtility::ToByte(Value, 11);

			m_Data.Part3[0] = HexUtility::ToByte(Value, 14);
			m_Data.Part3[1] = HexUtility::ToByte(Value, 16);

			m_Data.Part4[0] = HexUtility::ToByte(Value, 19);
			m_Data.Part4[1] = HexUtility::ToByte(Value, 21);

			m_Data.Part5[0] = HexUtility::ToByte(Value, 24);
			m_Data.Part5[1] = HexUtility::ToByte(Value, 26);
			m_Data.Part5[2] = HexUtility::ToByte(Value, 28);
			m_Data.Part5[3] = HexUtility::ToByte(Value, 30);
			m_Data.Part5[4] = HexUtility::ToByte(Value, 32);
			m_Data.Part5[5] = HexUtility::ToByte(Value, 34);
		}

		bool GUID::operator==(const GUID& Other) const
		{
			for (uint8 i = 0; i < sizeof(m_Data); ++i)
				if (ReinterpretCast(const byte*, &m_Data)[i] != ReinterpretCast(const byte*, &Other.m_Data)[i])
					return false;

			return true;
		}

		bool GUID::operator!=(const GUID& Other) const
		{
			return !(*this == Other);
		}

		String GUID::ToString(void) const
		{
			String result;

			StringUtility::Format(result, String("%02lX%02lX%02lX%02lX-%02lX%02lX-%02lX%02lX-%02lX%02lX-%02lX%02lX%02lX%02lX%02lX%02lX"),
				m_Data.Part1[0], m_Data.Part1[1], m_Data.Part1[2], m_Data.Part1[3],
				m_Data.Part2[0], m_Data.Part2[1],
				m_Data.Part3[0], m_Data.Part3[1],
				m_Data.Part4[0], m_Data.Part4[1],
				m_Data.Part5[0], m_Data.Part5[1], m_Data.Part5[2], m_Data.Part5[3], m_Data.Part5[4], m_Data.Part5[5]);

			return result;
		}

		GUID GUID::Create(void)
		{
			GUID guid;

			PlatformOS::GenerateGUID(&guid.m_Data);

			return guid;
		}
	}
}