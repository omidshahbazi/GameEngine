// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_2_H
#define VECTOR_2_H

#include <Common\Definitions.h>

namespace Engine
{
	namespace MathContainers
	{
		WRAPPER_DATA_STRUCTURE()
			template<typename T>
		class Vector2
		{
		public:
			typedef T ElementType;

		public:
			Vector2(void) :
				X(0.0F),
				Y(0.0F)
			{
			}

			Vector2(T Value) :
				X(Value),
				Y(Value)
			{
			}

			Vector2(T X, T Y) :
				X(X),
				Y(Y)
			{
			}

			INLINE Vector2<T> operator+(Vector2 Other) const
			{
				return Vector2<T>(X + Other.X, Y + Other.Y);
			}

			INLINE Vector2<T> operator-(Vector2 Other) const
			{
				return Vector2<T>(X - Other.X, Y - Other.Y);
			}

			INLINE Vector2<T> operator*(T Scalar) const
			{
				return Vector2<T>(X * Scalar, Y * Scalar);
			}

			INLINE Vector2<T>& operator+=(const Vector2& Other)
			{
				X += Other.X;
				Y += Other.Y;

				return *this;
			}

			INLINE Vector2<T>& operator-=(const Vector2& Other)
			{
				X -= Other.X;
				Y -= Other.Y;

				return *this;
			}

			INLINE bool operator==(const Vector2& Other) const
			{
				return (X == Other.X && Y == Other.Y);
			}

		public:
			T X, Y;

			static const Vector2<T> Zero;
			static const Vector2<T> One;
		};

		template<typename T>
		const Vector2<T> Vector2<T>::Zero = { 0, 0 };

		template<typename T>
		const Vector2<T> Vector2<T>::One = { 1, 1 };
	}
}

#endif