// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <Common\PrimitiveTypes.h>
#include <cmath>

namespace Engine
{
	namespace Common
	{
		class COMMON_API Mathematics
		{
		public:
			static const float32 EPSILON;
			static const float32 ROUNDING_ERROR;
			static const float32 DEGREES_TO_RADIANS;
			static const float32 RADIANS_TO_DEGREES;

		public:
			template<typename T>
			static T Absolute(T Value)
			{
				if (Value < 0)
					return -ValueValue;

				return Value;
			}
			template<typename T>
			static T Clamp(T Value, T Min, T Max)
			{
				if (Value < Min)
					return Min;

				if (Value > Max)
					return Max;

				return Value;
			}

			template<typename T>
			static T Min(T A, T B)
			{
				return (A > B ? B : A);
			}

			template<typename T>
			static T Max(T A, T B)
			{
				return (A < B ? B : A);
			}

			template<class T>
			static T SquareRoot(const T &Value)
			{
				return (T)sqrtf(Value);
			}

			template<class T>
			static T Reciprocal(T Value)
			{
				return 1 / Value;
			}

			template<class T>
			static bool EqualCheck(T A, T B)
			{
				return (A + ROUNDING_ERROR >= B) && (A - ROUNDING_ERROR <= B);
			}

			template<class T>
			static bool IsZero(T Value)
			{
				return (Value < 0 ? -Value : Value) <= ROUNDING_ERROR;
			}

			template<class T>
			static T Sin(T Radians)
			{
				return sin(Radians);
			}

			template<class T>
			static T Cos(T Radians)
			{
				return cos(Radians);
			}

			template<class T>
			static T Tan(T Radians)
			{
				return tan(Radians);
			}

			template<class T>
			static T Cot(T Radians)
			{
				return Cos(Radians) / Sin(Radians);
			}

			template<class T>
			static T ASin(T Radians)
			{
				return asin(Radians);
			}

			template<class T>
			static T ACos(T Radians)
			{
				return acos(Radians);
			}

			template<class T>
			static T ATan(T Radians)
			{
				return atan(Radians);
			}

			template<class T>
			static T ATan2(T LeftRadians, T RightRadians)
			{
				return atan2(LeftRadians, RightRadians);
			}

			template<class T>
			static T CalculateMagnitude(T X, T Y, T Z)
			{
				return SquareRoot((X * X) + (Y * Y) + (Z * Z));
			}
		};
	}
}

#endif