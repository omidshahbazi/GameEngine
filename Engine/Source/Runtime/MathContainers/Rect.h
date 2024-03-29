// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RECT_H
#define RECT_H

#include <MathContainers\Vector2.h>

namespace Engine
{
	namespace MathContainers
	{
		template<typename T>
		class Rect
		{
		public:
			typedef T ElementType;

		public:
			Rect(void)
			{
			}

			Rect(T Value) :
				Position(Value),
				Size(Value)
			{
			}

			Rect(const Vector2<T>& Position, const Vector2<T>& Size) :
				Position(Position),
				Size(Size)
			{
			}

			Rect(T X, T Y, T Width, T Height) :
				Position(X, Y),
				Size(Width, Height)
			{
			}

			INLINE bool Contains(const Vector2<T>& Value) const
			{
				if (Value.X < Position.X || Position.X + Size.X < Value.X)
					return false;

				if (Value.Y < Position.Y || Position.Y + Size.Y < Value.Y)
					return false;

				return true;
			}

			INLINE Rect<T> operator+(Rect Other) const
			{
				return Rect<T>(Position + Other.Position, Size + Other.Size);
			}

			INLINE Rect<T> operator-(Rect Other) const
			{
				return Vector2<T>(Position - Other.Position, Size - Other.Size);
			}

			INLINE Rect<T>& operator+=(const Rect& Other)
			{
				Position += Other.Position;
				Size += Other.Size;

				return *this;
			}

			INLINE Rect<T>& operator-=(const Rect& Other)
			{
				Position -= Other.Position;
				Size -= Other.Size;

				return *this;
			}

			INLINE bool operator==(const Rect& Other) const
			{
				return (Position == Other.Position && Size == Other.Size);
			}

		public:
			Vector2<T> Position, Size;
		};
	}
}

#endif