// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATH_CONTAINERS_H
#define MATH_CONTAINERS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Vector2.h>
#include <Containers\Vector3.h>
#include <Containers\Vector4.h>
#include <Containers\Matrix4.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		typedef Vector2<float32> Vector2F;

		typedef Vector3<float32> Vector3F;

		typedef Vector4<float32> Vector4F;

		typedef Matrix4<float32> Matrix4F;
	}
}

#endif