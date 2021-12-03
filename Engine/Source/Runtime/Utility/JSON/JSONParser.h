// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <Containers\Strings.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class AllocatorBase;
		}
	}

	using namespace Containers;
	using namespace Allocators;

	namespace JSON
	{
		class JSONBasic;

		class JSON_API JSONParser
		{
		public:
			static void Parse(AllocatorBase* Allocator, const String& Value, JSONBasic* Basic, bool ForceWString = false);
		};
	}
}

#endif