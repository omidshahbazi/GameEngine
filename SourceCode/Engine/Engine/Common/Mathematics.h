// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
			static const float32 ROUNDING_ERROR;
			static const float32 DEGREES_TO_RADIANS;
			static const float32 RADIANS_TO_DEGREES;

		public:
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
			static T Sin(T Degree)
			{
				return sin(Degree * DEGREES_TO_RADIANS);
			}

			template<class T>
			static T Cos(T Degree)
			{
				return cos(Degree * DEGREES_TO_RADIANS);
			}

			template<class T>
			static T Tan(T Degree)
			{
				return tan(Degree * DEGREES_TO_RADIANS);
			}

			template<class T>
			static T Cot(T Degree)
			{
				return Cos(Degree) / Sin(Degree);
			}

			template<class T>
			static T ASin(T Degree)
			{
				return asin(Degree * DEGREES_TO_RADIANS);
			}

			template<class T>
			static T ACos(T Degree)
			{
				return acos(Degree * DEGREES_TO_RADIANS);
			}

			template<class T>
			static T ATan(T Degree)
			{
				return atan(Degree * DEGREES_TO_RADIANS);
			}
		};
	}
}

#endif