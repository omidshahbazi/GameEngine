// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATH_CONTAINERS_H
#define MATH_CONTAINERS_H

#include <Common\PrimitiveTypes.h>
#include <MathContainers\Vector2.h>
#include <MathContainers\Vector3.h>
#include <MathContainers\Vector4.h>
#include <MathContainers\Quaternion.h>
#include <MathContainers\Matrix4.h>
#include <MathContainers\Rect.h>

namespace Engine
{
	using namespace Common;

	namespace MathContainers
	{
		//TODO: Add required mathematic functionalities into Vectors

		WRAPPER_DATA_STRUCTURE()
		typedef Vector2<float32> Vector2F;
		WRAPPER_DATA_STRUCTURE()
		typedef Vector2<int32> Vector2I;

		WRAPPER_DATA_STRUCTURE()
		typedef Vector3<float32> Vector3F;

		typedef Quaternion<float32> QuaternionF;

		typedef Vector4<float32> Vector4F;
		typedef Vector4<int32> Vector4I;

		typedef Matrix4<float32> Matrix4F;

		typedef Rect<int32> RectI;
	}
}

#endif