// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATH_CONTAINERS_H
#define MATH_CONTAINERS_H

#include <Common\PrimitiveTypes.h>
#include <MathContainers\Vector2.h>
#include <MathContainers\Vector3.h>
#include <MathContainers\Vector4.h>
#include <MathContainers\Matrix4.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		WRAPPER_DATA_STRUCTURE()
		typedef Vector2<float32> Vector2F;
		WRAPPER_DATA_STRUCTURE()
		typedef Vector2<int32> Vector2I;

		WRAPPER_DATA_STRUCTURE()
		typedef Vector3<float32> Vector3F;

		typedef Vector4<float32> Vector4F;

		typedef Matrix4<float32> Matrix4F;
	}
}

#endif