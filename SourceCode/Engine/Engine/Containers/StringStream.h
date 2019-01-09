// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include <Containers\Buffer.h>
#include <Containers\Strings.h>

namespace Engine
{
	namespace Containers
	{
		typedef Buffer<char8, false> StringStream;
		typedef Buffer<char16, false> WStringStream;

		template<typename T, bool Binary>
		INLINE Buffer<T, Binary> &operator << (Buffer<T, Binary> &Buffer, char8 Value)
		{
			T val = Value;
			Buffer.AppendBuffer(&val, 0, 1);
			return Buffer;
		}

		template<typename T, bool Binary>
		INLINE Buffer<T, Binary> &operator << (Buffer<T, Binary> &Buffer, char16 Value)
		{
			T val = Value;
			Buffer.AppendBuffer(&val, 0, 1);
			return Buffer;
		}

		template<typename T, bool Binary>
		INLINE Buffer<T, Binary> &operator << (Buffer<T, Binary> &Buffer, cstr Value)
		{
			String str(Value);

			Buffer.AppendBuffer(str.ChangeType<T>().GetValue(), 0, str.GetLength());

			return Buffer;
		}

		template<typename T, bool Binary>
		INLINE Buffer<T, Binary> &operator << (Buffer<T, Binary> &Buffer, cwstr Value)
		{
			WString str(Value);

			Buffer.AppendBuffer(str.ChangeType<T>().GetValue(), 0, str.GetLength());

			return Buffer;
		}

		template<typename T, bool Binary>
		INLINE Buffer<T, Binary> &operator << (Buffer<T, Binary> &Buffer, const DynamicString<T> &Value)
		{
			Buffer.AppendBuffer(Value.GetValue(), 0, Value.GetLength());
			return Buffer;
		}

		template<typename T, bool Binary>
		INLINE Buffer<T, Binary> &operator << (Buffer<T, Binary> &Buffer, const ConstantString<T> &Value)
		{
			Buffer.AppendBuffer(Value.GetValue(), 0, Value.GetLength());
			return Buffer;
		}
	}
}

#endif