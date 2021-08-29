// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef HEX_UTILITY_H
#define HEX_UTILITY_H

#include <Containers\StringStream.h>

namespace Engine
{
	namespace Containers
	{
		class HexUtility
		{
		public:
			INLINE static byte ToByte(const DynamicString<char8>& Value, uint32 Index)
			{
				return (CharToInt(Value.GetValue()[Index]) * 16) + CharToInt(Value.GetValue()[Index + 1]);
			}

		private:
			static int32 CharToInt(char8 input)
			{
				if (input >= '0' && input <= '9')
					return input - '0';
				if (input >= 'A' && input <= 'F')
					return input - 'A' + 10;
				if (input >= 'a' && input <= 'f')
					return input - 'a' + 10;

				return -1;
			}
		};
	}
}

#endif