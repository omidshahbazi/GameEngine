// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <Common\PrimitiveTypes.h>
#include <type_traits>

#ifndef BITWISE_UTILS_H
#define BITWISE_UTILS_H

namespace Engine
{
	namespace Common
	{
		class BitwiseUtils
		{
		public:
			template<typename T> static bool IsEnabled(T Mask, T Value)
			{
				return (((uint32)Mask & (uint32)Value) == (uint32)Value);
			}
		};

		template<typename Enum> Enum operator & (Enum Left, Enum Right)
		{
			static_assert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return static_cast<Enum>(static_cast<underlying>(Left) & static_cast<underlying>(Right));
		}

		template<typename Enum> Enum operator | (Enum Left, Enum Right)
		{
			static_assert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return static_cast<Enum>(static_cast<underlying>(Left) | static_cast<underlying>(Right));
		}

		template<typename Enum> Enum operator ^ (Enum Left, Enum Right)
		{
			static_assert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return static_cast<Enum>(static_cast<underlying>(Left) ^ static_cast<underlying>(Right));
		}

		template<typename Enum> Enum operator ~ (Enum Value)
		{
			static_assert(std::is_enum<Enum>::value, "Template parameter must be an enum type");

			using underlying = typename std::underlying_type<Enum>::type;

			return static_cast<Enum>(~static_cast<underlying>(Value));
		}

		template<typename Enum> Enum operator &= (Enum &LeftValue, Enum RightValue)
		{
			LeftValue = LeftValue & RightValue;
			return LeftValue;
		}

		template<typename Enum> Enum operator |= (Enum &LeftValue, Enum RightValue)
		{
			LeftValue = LeftValue & RightValue;
			return LeftValue;
		}

		template<typename Enum> Enum operator ^= (Enum &LeftValue, Enum RightValue)
		{
			LeftValue = LeftValue & RightValue;
			return LeftValue;
		}
	}
}

#endif