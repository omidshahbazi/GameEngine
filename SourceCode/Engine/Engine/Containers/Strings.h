// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <Containers\DynamicString.h>
#include <Containers\ConstantString.h>

#ifndef STRINGS_H
#define STRINGS_H

namespace Engine
{
	namespace Containers
	{
		typedef DynamicString<char8> String;
		typedef DynamicString<char16> WString;

		typedef ConstantString<char8> ConstString;
		typedef ConstantString<char16> ConstWString;
	}
}

#endif