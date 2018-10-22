// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_2_H
#define VECTOR_2_H

namespace Engine
{
	namespace Containers
	{
		template<typename T>
		class Vector2
		{
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

		public:
			T X, Y;
		};
	}
}

#endif