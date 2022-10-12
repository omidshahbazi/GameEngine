// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef PRIMITIVE_TYPES_H
#define PRIMITIVE_TYPES_H

#include <atomic>

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

#ifdef WINDOWS

#define API_ENTRY __stdcall

#endif

		typedef char8* str;
		typedef char16* wstr;

		typedef const char8* cstr;
		typedef const char16* cwstr;

		typedef std::atomic<bool> AtomicBool;
		typedef std::atomic<uint16> AtomicUInt16;
		typedef std::atomic<int32> AtomicInt32;
		typedef std::atomic<int32> AtomicUInt32;

#define ReinterpretCast(Type, Value) reinterpret_cast<Type>(Value)
#define StaticCast(Type, Value) static_cast<Type>(Value)
#define DynamicCast(Type, Value) dynamic_cast<Type>(Value)
#define ConstCast(Type, Value) const_cast<Type>(Value)
#define IsAssignableFrom(Value, BaseType) (DynamicCast(BaseType*, Value) != nullptr)

#define HardAssert(Condition, Message) _STL_ASSERT(Condition, Message)

#define StaticAssert(Condition, Message) \
	static_assert(Condition, Message)

		const cstr EngineName = "Engine";

		//https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
		// __FUNCDNAME__		??0CustomAllocator@Allocator@MemoryManagement@Engine@@IEAA@PEBDPEAVAllocatorBase@123@_K@Z
		// __FUNCSIG__			__cdecl Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator(const char *,class Engine::MemoryManagement::Allocator::AllocatorBase *,unsigned __int64)
		// __FUNCTION__			Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator
		// __func__				CustomAllocator
		// __PRETTY_FUNCTION__	Doesn't declared
#define DEBUG_ARGUMENTS __FILE__, __LINE__, __FUNCSIG__
	}
}

#endif