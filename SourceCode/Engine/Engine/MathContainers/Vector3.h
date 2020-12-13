// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_3_H
#define VECTOR_3_H

#include <Common\Mathematics.h>
#include <Common\Definitions.h>

namespace Engine
{
	using namespace Common;

	namespace MathContainers
	{
		WRAPPER_DATA_STRUCTURE()
			template<typename T>
		class Vector3
		{
		public:
			typedef T ElementType;

		public:
			Vector3(void) :
				X(0),
				Y(0),
				Z(0)
			{
			}

			Vector3(T Value) :
				X(Value),
				Y(Value),
				Z(Value)
			{
			}

			Vector3(T X, T Y, T Z) :
				X(X),
				Y(Y),
				Z(Z)
			{
			}

			INLINE void Normalize(void)
			{
				T mag = Mathematics::CalculateMagnitude(X, Y, Z);

				X /= mag;
				Y /= mag;
				Z /= mag;
			}

			INLINE Vector3<T> GetNormalized(void) const
			{
				Vector3<T> result = *this;

				result.Normalize();

				return result;
			}

			INLINE Vector3<T> operator+(const Vector3& Other) const
			{
				return Vector3<T>(X + Other.X, Y + Other.Y, Z + Other.Z);
			}

			INLINE Vector3<T> operator-(const Vector3& Other) const
			{
				return Vector3<T>(X - Other.X, Y - Other.Y, Z - Other.Z);
			}

			INLINE Vector3<T> operator*(T Scalar) const
			{
				return Vector3<T>(X * Scalar, Y * Scalar, Z * Scalar);
			}

			INLINE Vector3<T> operator/(T Scalar) const
			{
				return Vector3<T>(X / Scalar, Y / Scalar, Z / Scalar);
			}

			INLINE Vector3<T>& operator+=(const Vector3& Other)
			{
				X += Other.X;
				Y += Other.Y;
				Z += Other.Z;

				return *this;
			}

			INLINE Vector3<T>& operator-=(const Vector3& Other)
			{
				X -= Other.X;
				Y -= Other.Y;
				Z -= Other.Z;

				return *this;
			}

			INLINE Vector3<T>& operator*=(T Scalar)
			{
				X *= Scalar;
				Y *= Scalar;
				Z *= Scalar;

				return *this;
			}

			INLINE Vector3<T>& operator/=(T Scalar)
			{
				X /= Scalar;
				Y /= Scalar;
				Z /= Scalar;

				return *this;
			}

			INLINE bool operator==(const Vector3& Other) const
			{
				return (X == Other.X && Y == Other.Y && Z == Other.Z);
			}

		public:
			T X, Y, Z;

		public:
			static const Vector3<T> Zero;
			static const Vector3<T> One;
			static const Vector3<T> Up;
			static const Vector3<T> Right;
			static const Vector3<T> Forward;
		};

		template<typename T>
		const Vector3<T> Vector3<T>::Zero(0, 0, 0);

		template<typename T>
		const Vector3<T> Vector3<T>::One(1, 1, 1);

		template<typename T>
		const Vector3<T> Vector3<T>::Up(0, 1, 0);

		template<typename T>
		const Vector3<T> Vector3<T>::Right(1, 0, 0);

		template<typename T>
		const Vector3<T> Vector3<T>::Forward(0, 0, 1);
	}
}

#endif