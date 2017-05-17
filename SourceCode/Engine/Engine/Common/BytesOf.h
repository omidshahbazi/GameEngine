// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <Common\PrimitiveTypes.h>

#ifndef BYTES_OF_H
#define BYTES_OF_H

namespace Engine
{
	namespace Common
	{
		template<typename T> union BytesOf
		{
		public:
			T Value;
			byte Bytes[sizeof(T)];
		};
	}
}

#endif