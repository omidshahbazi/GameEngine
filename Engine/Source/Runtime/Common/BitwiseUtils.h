// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BITWISE_UTILS_H
#define BITWISE_UTILS_H

#include <Common\PrimitiveTypes.h>
#include <type_traits>

namespace Engine
{
	namespace Common
	{
		class BitwiseUtils
		{
		public:
			template<typename T>
			static void Enable(T &Mask, T Value)
			{
				Mask |= (T)(1 << (long)Value);
			}

			template<typename T>
			static void Disable(T &Mask, T Value)
			{
				Mask |= (T)(~(1 << (long)Value));
			}

			template<typename T>
			static bool IsEnabled(T Mask, T Value)
			{
				return ((T)((long)Mask & (long)Value) == Value);
			}
		};

		template<typename Enum>
		Enum operator & (Enum Left, Enum Right)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return StaticCast(Enum, StaticCast(underlying, Left) & StaticCast(underlying, Right));
		}

		template<typename Enum>
		Enum operator | (Enum Left, Enum Right)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return StaticCast(Enum, StaticCast(underlying, Left) | StaticCast(underlying, Right));
		}

		template<typename Enum>
		Enum operator ^ (Enum Left, Enum Right)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return StaticCast(Enum, StaticCast(underlying, Left) ^ StaticCast(underlying, Right));
		}

		template<typename Enum>
		Enum operator ~ (Enum Value)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return StaticCast(Enum, ~StaticCast(underlying, Value));
		}

		template<typename Enum>
		Enum operator &= (Enum &LeftValue, Enum RightValue)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			LeftValue = LeftValue & RightValue;

			return LeftValue;
		}

		template<typename Enum>
		Enum operator |= (Enum &LeftValue, Enum RightValue)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			LeftValue = LeftValue | RightValue;

			return LeftValue;
		}

		template<typename Enum>
		Enum operator ^= (Enum &LeftValue, Enum RightValue)
		{
			StaticAssert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			LeftValue = LeftValue & RightValue;

			return LeftValue;
		}
	}
}

#endif