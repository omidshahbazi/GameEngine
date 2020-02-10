// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_3_H
#define VECTOR_3_H

#include <Common\Mathematics.h>
#include <Common\Definitions.h>

namespace Engine
{
	namespace Containers
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

			Vector3<T> &Normalize(void)
			{
				T mag = Mathematics::CalculateMagnitude(X, Y, Z);

				X /= mag;
				Y /= mag;
				Z /= mag;

				return *this;
			}

			Vector3<T> GetNormalized(void) const
			{
				Vector3<T> result = *this;

				return result.Normalize();
			}

			Vector3<T> operator+(const Vector3& Other) const
			{
				return Vector3<T>(X + Other.X, Y + Other.Y, Z + Other.Z);
			}

			Vector3<T> operator*(T Scalar) const
			{
				return Vector3<T>(X * Scalar, Y * Scalar, Z * Scalar);
			}

			Vector3<T>& operator+=(const Vector3& Other)
			{
				X += Other.X;
				Y += Other.Y;
				Z += Other.Z;

				return *this;
			}

			Vector3<T>& operator-=(const Vector3& Other)
			{
				X -= Other.X;
				Y -= Other.Y;
				Z -= Other.Z;

				return *this;
			}

			bool operator==(const Vector3& Other) const
			{
				return (X == Other.X && Y == Other.Y && Z == Other.Z);
			}

		public:
			T X, Y, Z;
		};
	}
}

#endif