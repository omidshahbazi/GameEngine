// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef PRIMITIVE_TYPES_H
#define PRIMITIVE_TYPES_H

namespace Engine
{
	namespace Common
	{
#if defined(_MSC_VER)

		typedef unsigned __int8 byte;

		typedef char char8;
		typedef wchar_t char16;

		typedef signed __int8 int8;
		typedef signed __int16 int16;
		typedef signed __int32 int32;
		typedef signed __int64 int64;
		//typedef signed __int128 int128;

		typedef unsigned __int8 uint8;
		typedef unsigned __int16 uint16;
		typedef unsigned __int32 uint32;
		typedef unsigned __int64 uint64;
		//typedef unsigned __int128 uint128;

		typedef float float32;
		typedef double float64;
		typedef long double float128;

#define INLINE __forceinline

#elif defined(__GNUC__)

		typedef unsigned char byte;

		typedef unsigned char char8;
		typedef unsigned short char16;

		typedef signed char int8;
		typedef signed short int16;
		typedef signed int int32;
		typedef signed long int64;

		typedef unsigned char uint8;
		typedef unsigned short uint16;
		typedef unsigned int uint32;
		typedef unsigned long uint64;

		typedef float float32;
		typedef double float64;
		typedef long double float128;

#define INLINE inline

#endif

		typedef char8* str;
		typedef char16* wstr;

		typedef const char8* cstr;
		typedef const char16* cwstr;
	}
}

#endif