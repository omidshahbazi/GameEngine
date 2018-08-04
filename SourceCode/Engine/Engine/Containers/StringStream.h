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
		typedef Buffer<char8> StringStream;
		typedef Buffer<char16> WStringStream;

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, char8 Value)
		{
			T val = Value;
			Buffer.AppendBuffer(&val, 0, 1);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, char16 Value)
		{
			T val = Value;
			Buffer.AppendBuffer(&val, 0, 1);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, cstr Value)
		{
			String str(Value);

			Buffer.AppendBuffer(str.ChangeType<T>().GetValue(), 0, str.GetLength());

			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, cwstr Value)
		{
			WString str(Value);

			Buffer.AppendBuffer(str.ChangeType<T>().GetValue(), 0, str.GetLength());

			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, const DynamicString<T> &Value)
		{
			Buffer.AppendBuffer(Value.GetValue(), 0, Value.GetLength());
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, const ConstantString<T> &Value)
		{
			Buffer.AppendBuffer(Value.GetValue(), 0, Value.GetLength());
			return Buffer;
		}
	}
}

#endif