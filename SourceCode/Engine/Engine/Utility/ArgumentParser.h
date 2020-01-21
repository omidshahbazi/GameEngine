// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Utility
	{
		class UTILITY_API ArgumentParser
		{
		public:
			ArgumentParser(uint8 Count, const char **Arguments, bool FirstIsAddress = true);

			INLINE const String &GetAddress(void) const
			{
				return m_Address;
			}

			INLINE const String &GetAsString(uint8 Index) const
			{
				Assert(Index < m_Arguments.GetSize(), "Index must be less than Arguments count");

				return m_Arguments[Index];
			}

			int8 GetAsInt8(uint8 Index) const;
			int16 GetAsInt16(uint8 Index) const;
			int32 GetAsInt32(uint8 Index) const;
			int64 GetAsInt64(uint8 Index) const;

			uint8 GetAsUInt8(uint8 Index) const;
			uint16 GetAsUInt16(uint8 Index) const;
			uint32 GetAsUInt32(uint8 Index) const;
			uint64 GetAsUInt64(uint8 Index) const;

			float32 GetAsFloat32(uint8 Index) const;

			const String &GetAsString(const String &Parameter) const;

			int8 GetAsInt8(const String &Parameter) const;
			int16 GetAsInt16(const String &Parameter) const;
			int32 GetAsInt32(const String &Parameter) const;
			int64 GetAsInt64(const String &Parameter) const;

			uint8 GetAsUInt8(const String &Parameter) const;
			uint16 GetAsUInt16(const String &Parameter) const;
			uint32 GetAsUInt32(const String &Parameter) const;
			uint64 GetAsUInt64(const String &Parameter) const;

			float32 GetAsFloat32(const String &Parameter) const;

			INLINE bool Has(const String &Parameter) const
			{
				return m_Arguments.Find(Parameter) != -1;
			}

			INLINE uint8 GetCount(void) const
			{
				return m_Arguments.GetSize();
			}

		private:
			String m_Address;
			StringList m_Arguments;
		};
	}
}
#endif
