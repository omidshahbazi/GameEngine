// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_3_H
#define VECTOR_3_H

namespace Engine
{
	namespace Containers
	{
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

		public:
			T X, Y, Z;
		};
	}
}

#endif