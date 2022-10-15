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
		typedef Vector2<int32> Vector2I;
		typedef Vector2<uint32> Vector2U;
		typedef Vector2<float32> Vector2F;
		typedef Vector2<float64> Vector2D;

		typedef Vector3<int32> Vector3I;
		typedef Vector3<uint32> Vector3U;
		typedef Vector3<float32> Vector3F;
		typedef Vector3<float64> Vector3D;

		typedef Vector4<int32> Vector4I;
		typedef Vector4<uint32> Vector4U;
		typedef Vector4<float32> Vector4F;
		typedef Vector4<float64> Vector4D;

		typedef Quaternion<float32> QuaternionF;

		typedef Matrix4<float32> Matrix4F;
		typedef Matrix4<float64> Matrix4D;

		typedef Rect<int32> RectI;
	}
}

#endif