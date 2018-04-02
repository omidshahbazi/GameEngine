// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRINGS_H
#define STRINGS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\DynamicString.h>
#include <Containers\ConstantString.h>

namespace Engine
{
	namespace Containers
	{
		template CONTAINERS_API class DynamicString<char8>;
		//template CONTAINERS_API class DynamicString<char16>;

		template CONTAINERS_API class ConstantString<char8>;
		//template CONTAINERS_API class ConstantString<char16>;

		typedef DynamicString<char8> String;
		typedef DynamicString<char16> WString;

		typedef ConstantString<char8> ConstString;
		typedef ConstantString<char16> ConstWString;

		typedef String Path;
	}
}

#endif