// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef MATH_H
#define MATH_H

#include <Common\PrimitiveTypes.h>
#include <cmath>

namespace Engine
{
	using namespace Common;

	namespace Mathematics
	{
		class MATHEMATICS_API Math
		{
		public:
			static const float32 EPSILON;
			static const float32 ROUNDING_ERROR;
			static const float64 PI;
			static const float32 DEGREES_TO_RADIANS;
			static const float32 RADIANS_TO_DEGREES;

		public:
			template<typename T>
			static T Absolute(T Value)
			{
				if (Value < 0)
					return -Value;

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

			template<typename T>
			static T FLoor(T A)
			{
				return (int64)A;
			}

			template<typename T>
			static T Ceil(T A)
			{
				if (A == (int64)A)
					return A;

				return (int64)A + 1;
			}

			template<class T>
			static T SquareRoot(const T& Value)
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
				return EqualCheck(A, B, ROUNDING_ERROR);
			}

			template<class T>
			static bool EqualCheck(T A, T B, T RoundingError)
			{
				return (A + RoundingError >= B) && (A - RoundingError <= B);
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