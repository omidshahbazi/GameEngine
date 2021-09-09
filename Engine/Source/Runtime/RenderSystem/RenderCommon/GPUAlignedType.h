// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_ALIGNED_TYPE_H
#define GPU_ALIGNED_TYPE_H

#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace MathContainers;

	namespace RenderCommon
	{
		template<typename T, uint8 Alignment>
		struct alignas(Alignment) GPUAlignedType
		{
		public:
			static const uint8 Size = sizeof(T);
			static const uint8 Alignment = Alignment;

		private:
			T Value;

		public:
			GPUAlignedType<T, Alignment>& operator =(const T& Other)
			{
				Value = Other;
				return *this;
			}

			operator T(void)
			{
				return Value;
			}

			T& operator *(void)
			{
				return Value;
			}

			const T& operator *(void) const
			{
				return Value;
			}

			T& operator ->(void)
			{
				return Value;
			}

			const T& operator ->(void) const
			{
				return Value;
			}
		};

		typedef GPUAlignedType<bool, 16> GPUAlignedBool;
		typedef GPUAlignedType<float32, 16> GPUAlignedFloat32;
		typedef GPUAlignedType<float64, 16> GPUAlignedFloat64;
		typedef GPUAlignedType<Vector2F, 16> GPUAlignedVector2F;
		typedef GPUAlignedType<Vector2D, 16> GPUAlignedVector2D;
		typedef GPUAlignedType<Vector3F, 16> GPUAlignedVector3F;
		typedef GPUAlignedType<Vector3D, 32> GPUAlignedVector3D;
		typedef GPUAlignedType<Vector4F, 16> GPUAlignedVector4F;
		typedef GPUAlignedType<Vector4D, 32> GPUAlignedVector4D;
		typedef GPUAlignedType<ColorF, 16> GPUAlignedColorF;
		typedef GPUAlignedType<Matrix4F, 16> GPUAlignedMatrix4F;
	}
}

#endif