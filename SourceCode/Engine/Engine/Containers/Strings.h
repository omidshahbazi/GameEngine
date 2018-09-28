// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRINGS_H
#define STRINGS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\DynamicString.h>
#include <Containers\ConstantString.h>
#include <Containers\StringStream.h>

namespace Engine
{
	namespace Containers
	{
		typedef DynamicString<char8> String;
		typedef DynamicString<char16> WString;

		typedef ConstantString<char8> ConstString;
		typedef ConstantString<char16> ConstWString;

		namespace Private
		{
			typedef SharedBlock<char8> _SharedBlock;
			typedef SharedBlock<char16> _WSharedBlock;
		}

		typedef Vector<String> StringList;
		typedef Vector<WString> WStringList;

#define TEXT(Value) String(Value)
#define STRINGIZE(Value) #Value
	}
}

#endif