// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_4_H
#define VECTOR_4_H

namespace Engine
{
	namespace MathContainers
	{
		template<typename T>
		class Vector4
		{
		public:
			typedef T ElementType;

		public:
			Vector4(void) :
				X(0),
				Y(0),
				Z(0),
				W(0)
			{
			}

			Vector4(T Value) :
				X(Value),
				Y(Value),
				Z(Value),
				W(Value)
			{
			}

			Vector4(T X, T Y, T Z, T W) :
				X(X),
				Y(Y),
				Z(Z),
				W(W)
			{
			}

			INLINE void Normalize(void)
			{
			}

			INLINE Vector4<T> GetNormalized(void) const
			{
				Vector3<T> result;

				result.Normalize();

				return result;
			}

			INLINE Vector4<T> operator*(T Scalar) const
			{
				return Vector4<T>(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
			}

			INLINE Vector4<T>& operator+=(const Vector4& Other)
			{
				X += Other.X;
				Y += Other.Y;
				Z += Other.Z;
				W += Other.W;

				return *this;
			}

			INLINE Vector4<T>& operator-=(const Vector4& Other)
			{
				X -= Other.X;
				Y -= Other.Y;
				Z -= Other.Z;
				W -= Other.W;

				return *this;
			}

			INLINE bool operator==(const Vector4& Other) const
			{
				return (X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W);
			}

		public:
			T X, Y, Z, W;
		};
	}
}

#endif