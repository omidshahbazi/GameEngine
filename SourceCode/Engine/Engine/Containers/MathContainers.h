// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATH_CONTAINERS_H
#define MATH_CONTAINERS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Vector2.h>
#include <Containers\Vector3.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		typedef Vector2<float32> Vector2F;
		typedef Vector2<int32> Vector2I;

		typedef Vector3<float32> Vector3F;
		typedef Vector3<int32> Vector3I;
	}
}

#endif