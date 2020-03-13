// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <Common\PrimitiveTypes.h>
#include <Common\Mathematics.h>
#include <Debugging\Debug.h>
#include <MathContainers\Vector3.h>
#include <MathContainers\Vector4.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Common;
	using namespace Debugging;
	using namespace Platform;

	namespace MathContainers
	{
		template<typename T>
		class Matrix4
		{
		public:
			Matrix4(void)
			{
			}

			Matrix4(const Matrix3<T>& Other)
			{
				operator=(Identity);
				operator=(Other);
			}

			Matrix4(
				T m00, T m01, T m02, T m03,
				T m10, T m11, T m12, T m13,
				T m20, T m21, T m22, T m23,
				T m30, T m31, T m32, T m33)
			{
				Cells[0][0] = m00;
				Cells[0][1] = m01;
				Cells[0][2] = m02;
				Cells[0][3] = m03;
				Cells[1][0] = m10;
				Cells[1][1] = m11;
				Cells[1][2] = m12;
				Cells[1][3] = m13;
				Cells[2][0] = m20;
				Cells[2][1] = m21;
				Cells[2][2] = m22;
				Cells[2][3] = m23;
				Cells[3][0] = m30;
				Cells[3][1] = m31;
				Cells[3][2] = m32;
				Cells[3][3] = m33;
			}

			T* operator [] (uint32 Row)
			{
				return Cells[Row];
			}

			const T* operator [] (uint32 Row) const
			{
				return Cells[Row];
			}

			Matrix4<T> operator + (const Matrix4<T>& Other) const
			{
				Matrix4<T> r;

				r.Cells[0][0] = Cells[0][0] + Other.Cells[0][0];
				r.Cells[0][1] = Cells[0][1] + Other.Cells[0][1];
				r.Cells[0][2] = Cells[0][2] + Other.Cells[0][2];
				r.Cells[0][3] = Cells[0][3] + Other.Cells[0][3];

				r.Cells[1][0] = Cells[1][0] + Other.Cells[1][0];
				r.Cells[1][1] = Cells[1][1] + Other.Cells[1][1];
				r.Cells[1][2] = Cells[1][2] + Other.Cells[1][2];
				r.Cells[1][3] = Cells[1][3] + Other.Cells[1][3];

				r.Cells[2][0] = Cells[2][0] + Other.Cells[2][0];
				r.Cells[2][1] = Cells[2][1] + Other.Cells[2][1];
				r.Cells[2][2] = Cells[2][2] + Other.Cells[2][2];
				r.Cells[2][3] = Cells[2][3] + Other.Cells[2][3];

				r.Cells[3][0] = Cells[3][0] + Other.Cells[3][0];
				r.Cells[3][1] = Cells[3][1] + Other.Cells[3][1];
				r.Cells[3][2] = Cells[3][2] + Other.Cells[3][2];
				r.Cells[3][3] = Cells[3][3] + Other.Cells[3][3];

				return r;
			}

			Matrix4<T> operator - (const Matrix4<T>& Other) const
			{
				Matrix4<T> r;

				r.Cells[0][0] = Cells[0][0] - Other.Cells[0][0];
				r.Cells[0][1] = Cells[0][1] - Other.Cells[0][1];
				r.Cells[0][2] = Cells[0][2] - Other.Cells[0][2];
				r.Cells[0][3] = Cells[0][3] - Other.Cells[0][3];

				r.Cells[1][0] = Cells[1][0] - Other.Cells[1][0];
				r.Cells[1][1] = Cells[1][1] - Other.Cells[1][1];
				r.Cells[1][2] = Cells[1][2] - Other.Cells[1][2];
				r.Cells[1][3] = Cells[1][3] - Other.Cells[1][3];

				r.Cells[2][0] = Cells[2][0] - Other.Cells[2][0];
				r.Cells[2][1] = Cells[2][1] - Other.Cells[2][1];
				r.Cells[2][2] = Cells[2][2] - Other.Cells[2][2];
				r.Cells[2][3] = Cells[2][3] - Other.Cells[2][3];

				r.Cells[3][0] = Cells[3][0] - Other.Cells[3][0];
				r.Cells[3][1] = Cells[3][1] - Other.Cells[3][1];
				r.Cells[3][2] = Cells[3][2] - Other.Cells[3][2];
				r.Cells[3][3] = Cells[3][3] - Other.Cells[3][3];

				return r;
			}

			Matrix4<T> operator * (const Matrix4<T>& Other) const
			{
				Matrix4<T> r;

				//New one
				r.Cells[0][0] = Cells[0][0] * Other.Cells[0][0] + Cells[0][1] * Other.Cells[1][0] + Cells[0][2] * Other.Cells[2][0] + Cells[0][3] * Other.Cells[3][0];
				r.Cells[0][1] = Cells[0][0] * Other.Cells[0][1] + Cells[0][1] * Other.Cells[1][1] + Cells[0][2] * Other.Cells[2][1] + Cells[0][3] * Other.Cells[3][1];
				r.Cells[0][2] = Cells[0][0] * Other.Cells[0][2] + Cells[0][1] * Other.Cells[1][2] + Cells[0][2] * Other.Cells[2][2] + Cells[0][3] * Other.Cells[3][2];
				r.Cells[0][3] = Cells[0][0] * Other.Cells[0][3] + Cells[0][1] * Other.Cells[1][3] + Cells[0][2] * Other.Cells[2][3] + Cells[0][3] * Other.Cells[3][3];

				r.Cells[1][0] = Cells[1][0] * Other.Cells[0][0] + Cells[1][1] * Other.Cells[1][0] + Cells[1][2] * Other.Cells[2][0] + Cells[1][3] * Other.Cells[3][0];
				r.Cells[1][1] = Cells[1][0] * Other.Cells[0][1] + Cells[1][1] * Other.Cells[1][1] + Cells[1][2] * Other.Cells[2][1] + Cells[1][3] * Other.Cells[3][1];
				r.Cells[1][2] = Cells[1][0] * Other.Cells[0][2] + Cells[1][1] * Other.Cells[1][2] + Cells[1][2] * Other.Cells[2][2] + Cells[1][3] * Other.Cells[3][2];
				r.Cells[1][3] = Cells[1][0] * Other.Cells[0][3] + Cells[1][1] * Other.Cells[1][3] + Cells[1][2] * Other.Cells[2][3] + Cells[1][3] * Other.Cells[3][3];

				r.Cells[2][0] = Cells[2][0] * Other.Cells[0][0] + Cells[2][1] * Other.Cells[1][0] + Cells[2][2] * Other.Cells[2][0] + Cells[2][3] * Other.Cells[3][0];
				r.Cells[2][1] = Cells[2][0] * Other.Cells[0][1] + Cells[2][1] * Other.Cells[1][1] + Cells[2][2] * Other.Cells[2][1] + Cells[2][3] * Other.Cells[3][1];
				r.Cells[2][2] = Cells[2][0] * Other.Cells[0][2] + Cells[2][1] * Other.Cells[1][2] + Cells[2][2] * Other.Cells[2][2] + Cells[2][3] * Other.Cells[3][2];
				r.Cells[2][3] = Cells[2][0] * Other.Cells[0][3] + Cells[2][1] * Other.Cells[1][3] + Cells[2][2] * Other.Cells[2][3] + Cells[2][3] * Other.Cells[3][3];

				r.Cells[3][0] = Cells[3][0] * Other.Cells[0][0] + Cells[3][1] * Other.Cells[1][0] + Cells[3][2] * Other.Cells[2][0] + Cells[3][3] * Other.Cells[3][0];
				r.Cells[3][1] = Cells[3][0] * Other.Cells[0][1] + Cells[3][1] * Other.Cells[1][1] + Cells[3][2] * Other.Cells[2][1] + Cells[3][3] * Other.Cells[3][1];
				r.Cells[3][2] = Cells[3][0] * Other.Cells[0][2] + Cells[3][1] * Other.Cells[1][2] + Cells[3][2] * Other.Cells[2][2] + Cells[3][3] * Other.Cells[3][2];
				r.Cells[3][3] = Cells[3][0] * Other.Cells[0][3] + Cells[3][1] * Other.Cells[1][3] + Cells[3][2] * Other.Cells[2][3] + Cells[3][3] * Other.Cells[3][3];


				//Old one
				//r.Cells[0][0] = Other.Cells[0][0] * Cells[0][0] + Other.Cells[0][1] * Cells[1][0] + Other.Cells[0][2] * Cells[2][0] + Other.Cells[0][3] * Cells[3][0];
				//r.Cells[0][1] = Other.Cells[0][0] * Cells[0][1] + Other.Cells[0][1] * Cells[1][1] + Other.Cells[0][2] * Cells[2][1] + Other.Cells[0][3] * Cells[3][1];
				//r.Cells[0][2] = Other.Cells[0][0] * Cells[0][2] + Other.Cells[0][1] * Cells[1][2] + Other.Cells[0][2] * Cells[2][2] + Other.Cells[0][3] * Cells[3][2];
				//r.Cells[0][3] = Other.Cells[0][0] * Cells[0][3] + Other.Cells[0][1] * Cells[1][3] + Other.Cells[0][2] * Cells[2][3] + Other.Cells[0][3] * Cells[3][3];

				//r.Cells[1][0] = Other.Cells[1][0] * Cells[0][0] + Other.Cells[1][1] * Cells[1][0] + Other.Cells[1][2] * Cells[2][0] + Other.Cells[1][3] * Cells[3][0];
				//r.Cells[1][1] = Other.Cells[1][0] * Cells[0][1] + Other.Cells[1][1] * Cells[1][1] + Other.Cells[1][2] * Cells[2][1] + Other.Cells[1][3] * Cells[3][1];
				//r.Cells[1][2] = Other.Cells[1][0] * Cells[0][2] + Other.Cells[1][1] * Cells[1][2] + Other.Cells[1][2] * Cells[2][2] + Other.Cells[1][3] * Cells[3][2];
				//r.Cells[1][3] = Other.Cells[1][0] * Cells[0][3] + Other.Cells[1][1] * Cells[1][3] + Other.Cells[1][2] * Cells[2][3] + Other.Cells[1][3] * Cells[3][3];

				//r.Cells[2][0] = Other.Cells[2][0] * Cells[0][0] + Other.Cells[2][1] * Cells[1][0] + Other.Cells[2][2] * Cells[2][0] + Other.Cells[2][3] * Cells[3][0];
				//r.Cells[2][1] = Other.Cells[2][0] * Cells[0][1] + Other.Cells[2][1] * Cells[1][1] + Other.Cells[2][2] * Cells[2][1] + Other.Cells[2][3] * Cells[3][1];
				//r.Cells[2][2] = Other.Cells[2][0] * Cells[0][2] + Other.Cells[2][1] * Cells[1][2] + Other.Cells[2][2] * Cells[2][2] + Other.Cells[2][3] * Cells[3][2];
				//r.Cells[2][3] = Other.Cells[2][0] * Cells[0][3] + Other.Cells[2][1] * Cells[1][3] + Other.Cells[2][2] * Cells[2][3] + Other.Cells[2][3] * Cells[3][3];

				//r.Cells[3][0] = Other.Cells[3][0] * Cells[0][0] + Other.Cells[3][1] * Cells[1][0] + Other.Cells[3][2] * Cells[2][0] + Other.Cells[3][3] * Cells[3][0];
				//r.Cells[3][1] = Other.Cells[3][0] * Cells[0][1] + Other.Cells[3][1] * Cells[1][1] + Other.Cells[3][2] * Cells[2][1] + Other.Cells[3][3] * Cells[3][1];
				//r.Cells[3][2] = Other.Cells[3][0] * Cells[0][2] + Other.Cells[3][1] * Cells[1][2] + Other.Cells[3][2] * Cells[2][2] + Other.Cells[3][3] * Cells[3][2];
				//r.Cells[3][3] = Other.Cells[3][0] * Cells[0][3] + Other.Cells[3][1] * Cells[1][3] + Other.Cells[3][2] * Cells[2][3] + Other.Cells[3][3] * Cells[3][3];

				return r;
			}

			Vector3<T> operator * (const Vector3<T>& Other) const
			{
				Vector3<T> r;

				T fInvW = 1.0f / (Cells[3][0] * Other.X + Cells[3][1] * Other.Y + Cells[3][2] * Other.Z + Cells[3][3]);

				r.X = (Cells[0][0] * Other.X + Cells[0][1] * Other.Y + Cells[0][2] * Other.Z + Cells[0][3]) * fInvW;
				r.Y = (Cells[1][0] * Other.X + Cells[1][1] * Other.Y + Cells[1][2] * Other.Z + Cells[1][3]) * fInvW;
				r.Z = (Cells[2][0] * Other.X + Cells[2][1] * Other.Y + Cells[2][2] * Other.Z + Cells[2][3]) * fInvW;

				return r;
			}

			Vector4<T> operator * (const Vector4<T>& Other) const
			{
				return Vector4<T>(
					Cells[0][0] * Other.X + Cells[0][1] * Other.Y + Cells[0][2] * Other.Z + Cells[0][3] * Other.w,
					Cells[1][0] * Other.X + Cells[1][1] * Other.Y + Cells[1][2] * Other.Z + Cells[1][3] * Other.w,
					Cells[2][0] * Other.X + Cells[2][1] * Other.Y + Cells[2][2] * Other.Z + Cells[2][3] * Other.w,
					Cells[3][0] * Other.X + Cells[3][1] * Other.Y + Cells[3][2] * Other.Z + Cells[3][3] * Other.w
					);
			}

			Matrix4<T> operator*(T Scalar) const
			{
				return Matrix4(
					Scalar * Cells[0][0], Scalar * Cells[0][1], Scalar * Cells[0][2], Scalar * Cells[0][3],
					Scalar * Cells[1][0], Scalar * Cells[1][1], Scalar * Cells[1][2], Scalar * Cells[1][3],
					Scalar * Cells[2][0], Scalar * Cells[2][1], Scalar * Cells[2][2], Scalar * Cells[2][3],
					Scalar * Cells[3][0], Scalar * Cells[3][1], Scalar * Cells[3][2], Scalar * Cells[3][3]);
			}

			void operator = (const Matrix3<T>& Other)
			{
				Cells[0][0] = Other.Cells[0][0]; Cells[0][1] = Other.Cells[0][1]; Cells[0][2] = Other.Cells[0][2];
				Cells[1][0] = Other.Cells[1][0]; Cells[1][1] = Other.Cells[1][1]; Cells[1][2] = Other.Cells[1][2];
				Cells[2][0] = Other.Cells[2][0]; Cells[2][1] = Other.Cells[2][1]; Cells[2][2] = Other.Cells[2][2];
			}

			bool operator == (const Matrix4<T>& Other) const
			{
				if (
					Cells[0][0] != Other.Cells[0][0] || Cells[0][1] != Other.Cells[0][1] || Cells[0][2] != Other.Cells[0][2] || Cells[0][3] != Other.Cells[0][3] ||
					Cells[1][0] != Other.Cells[1][0] || Cells[1][1] != Other.Cells[1][1] || Cells[1][2] != Other.Cells[1][2] || Cells[1][3] != Other.Cells[1][3] ||
					Cells[2][0] != Other.Cells[2][0] || Cells[2][1] != Other.Cells[2][1] || Cells[2][2] != Other.Cells[2][2] || Cells[2][3] != Other.Cells[2][3] ||
					Cells[3][0] != Other.Cells[3][0] || Cells[3][1] != Other.Cells[3][1] || Cells[3][2] != Other.Cells[3][2] || Cells[3][3] != Other.Cells[3][3])
					return false;
				return true;
			}

			bool operator != (const Matrix4<T>& Other) const
			{
				if (
					Cells[0][0] != Other.Cells[0][0] || Cells[0][1] != Other.Cells[0][1] || Cells[0][2] != Other.Cells[0][2] || Cells[0][3] != Other.Cells[0][3] ||
					Cells[1][0] != Other.Cells[1][0] || Cells[1][1] != Other.Cells[1][1] || Cells[1][2] != Other.Cells[1][2] || Cells[1][3] != Other.Cells[1][3] ||
					Cells[2][0] != Other.Cells[2][0] || Cells[2][1] != Other.Cells[2][1] || Cells[2][2] != Other.Cells[2][2] || Cells[2][3] != Other.Cells[2][3] ||
					Cells[3][0] != Other.Cells[3][0] || Cells[3][1] != Other.Cells[3][1] || Cells[3][2] != Other.Cells[3][2] || Cells[3][3] != Other.Cells[3][3])
					return true;
				return false;
			}

			void operator *= (const Matrix4<T>& Other)
			{
				*this = operator*(Other);
			}

			Matrix4<T> Transpose(void) const
			{
				return Matrix4(Cells[0][0], Cells[1][0], Cells[2][0], Cells[3][0],
					Cells[0][1], Cells[1][1], Cells[2][1], Cells[3][1],
					Cells[0][2], Cells[1][2], Cells[2][2], Cells[3][2],
					Cells[0][3], Cells[1][3], Cells[2][3], Cells[3][3]);
			}

			void SetPosition(const Vector3<T>& Value)
			{
				Cells[0][3] = Value.X;
				Cells[1][3] = Value.Y;
				Cells[2][3] = Value.Z;
			}

			Vector3<T> GetPosition(void) const
			{
				return Vector3<T>(Cells[0][3], Cells[1][3], Cells[2][3]);
			}

			void Translate(const Vector3<T>& Value)
			{
				Cells[0][0] = 1.0; Cells[0][1] = 0.0; Cells[0][2] = 0.0; Cells[0][3] = Value.X;
				Cells[1][0] = 0.0; Cells[1][1] = 1.0; Cells[1][2] = 0.0; Cells[1][3] = Value.Y;
				Cells[2][0] = 0.0; Cells[2][1] = 0.0; Cells[2][2] = 1.0; Cells[2][3] = Value.Z;
				Cells[3][0] = 0.0; Cells[3][1] = 0.0; Cells[3][2] = 0.0; Cells[3][3] = 1.0;
			}

			void SetScale(const Vector3<T>& Value)
			{
				Cells[0][0] = Value.X;
				Cells[1][1] = Value.Y;
				Cells[2][2] = Value.Z;
			}

			void ExtractMatrix3(Matrix3<T>& Other) const
			{
				Other.Cells[0][0] = Cells[0][0];
				Other.Cells[0][1] = Cells[0][1];
				Other.Cells[0][2] = Cells[0][2];
				Other.Cells[1][0] = Cells[1][0];
				Other.Cells[1][1] = Cells[1][1];
				Other.Cells[1][2] = Cells[1][2];
				Other.Cells[2][0] = Cells[2][0];
				Other.Cells[2][1] = Cells[2][1];
				Other.Cells[2][2] = Cells[2][2];
			}

			void ExtractQuaternion(Quaternion<T>& Other) const
			{
				Matrix3<T> mat;
				ExtractMatrix3(mat);
				Other = mat;
			}

			bool HasScale(void) const
			{
				T t = Cells[0][0] * Cells[0][0] + Cells[1][0] * Cells[1][0] + Cells[2][0] * Cells[2][0];
				if (!Mathematics::EqualCheck(t, 1.0))
					return true;

				t = Cells[0][1] * Cells[0][1] + Cells[1][1] * Cells[1][1] + Cells[2][1] * Cells[2][1];
				if (!Mathematics::EqualCheck(t, 1.0))
					return true;

				t = Cells[0][2] * Cells[0][2] + Cells[1][2] * Cells[1][2] + Cells[2][2] * Cells[2][2];
				if (!Mathematics::EqualCheck(t, 1.0))
					return true;

				return false;
			}

			bool HasNegativeScale(void) const
			{
				return Determinant() < 0;
			}

			T Determinant(void) const
			{
				return
					Cells[0][0] * Minor(*this, 1, 2, 3, 1, 2, 3) -
					Cells[0][1] * Minor(*this, 1, 2, 3, 0, 2, 3) +
					Cells[0][2] * Minor(*this, 1, 2, 3, 0, 1, 3) -
					Cells[0][3] * Minor(*this, 1, 2, 3, 0, 1, 2);
			}

			Matrix4<T> Inverse(void) const
			{
				T m00 = Cells[0][0]; T m01 = Cells[0][1]; T m02 = Cells[0][2]; T m03 = Cells[0][3];
				T m10 = Cells[1][0]; T m11 = Cells[1][1]; T m12 = Cells[1][2]; T m13 = Cells[1][3];
				T m20 = Cells[2][0]; T m21 = Cells[2][1]; T m22 = Cells[2][2]; T m23 = Cells[2][3];
				T m30 = Cells[3][0]; T m31 = Cells[3][1]; T m32 = Cells[3][2]; T m33 = Cells[3][3];

				T v0 = m20 * m31 - m21 * m30;
				T v1 = m20 * m32 - m22 * m30;
				T v2 = m20 * m33 - m23 * m30;
				T v3 = m21 * m32 - m22 * m31;
				T v4 = m21 * m33 - m23 * m31;
				T v5 = m22 * m33 - m23 * m32;

				T t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
				T t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
				T t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
				T t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

				T invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

				T d00 = t00 * invDet;
				T d10 = t10 * invDet;
				T d20 = t20 * invDet;
				T d30 = t30 * invDet;

				T d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
				T d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
				T d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
				T d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

				v0 = m10 * m31 - m11 * m30;
				v1 = m10 * m32 - m12 * m30;
				v2 = m10 * m33 - m13 * m30;
				v3 = m11 * m32 - m12 * m31;
				v4 = m11 * m33 - m13 * m31;
				v5 = m12 * m33 - m13 * m32;

				T d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
				T d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
				T d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
				T d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

				v0 = m21 * m10 - m20 * m11;
				v1 = m22 * m10 - m20 * m12;
				v2 = m23 * m10 - m20 * m13;
				v3 = m22 * m11 - m21 * m12;
				v4 = m23 * m11 - m21 * m13;
				v5 = m23 * m12 - m22 * m13;

				T d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
				T d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
				T d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
				T d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

				return Matrix4<T>(
					d00, d01, d02, d03,
					d10, d11, d12, d13,
					d20, d21, d22, d23,
					d30, d31, d32, d33);
			}

			void MakeTransform(const Vector3<T>& Position, const Vector3<T>& Scale, const Quaternion<T>& Orientation)
			{
				// Ordering:
				//    1. Scale
				//    2. Rotate
				//    3. Translate

				Matrix3<T> rotMat;
				Orientation.ToRotationMatrix(rotMat);

				// Set up final matrix with scale, rotation and translation
				Cells[0][0] = Scale.X * rotMat[0][0]; Cells[0][1] = Scale.Y * rotMat[0][1]; Cells[0][2] = Scale.Z * rotMat[0][2]; Cells[0][3] = Position.X;
				Cells[1][0] = Scale.X * rotMat[1][0]; Cells[1][1] = Scale.Y * rotMat[1][1]; Cells[1][2] = Scale.Z * rotMat[1][2]; Cells[1][3] = Position.Y;
				Cells[2][0] = Scale.X * rotMat[2][0]; Cells[2][1] = Scale.Y * rotMat[2][1]; Cells[2][2] = Scale.Z * rotMat[2][2]; Cells[2][3] = Position.Z;

				// No projection term
				Cells[3][0] = 0; Cells[3][1] = 0; Cells[3][2] = 0; Cells[3][3] = 1;
			}

			void MakePerspectiveProjectionMatrix(T FieldOfView, T AspectRatio, T NearClipDistance, T FarClipDistance)
			{
				Assert(AspectRatio != 0.0F, "AspectRatio must be non-zero, devide by zero will happen");
				Assert(NearClipDistance != FarClipDistance, "NearClipDistance and FarClipDistance cannot equals, devide by zero will happen");

				const T h = Mathematics::Reciprocal(Mathematics::Tan(FieldOfView * 0.5F));
				const T w = h / AspectRatio;

				Cells[0][0] = w;
				Cells[0][1] = 0.0F;
				Cells[0][2] = 0.0F;
				Cells[0][3] = 0.0F;

				Cells[1][0] = 0.0F;
				Cells[1][1] = h;
				Cells[1][2] = 0.0F;
				Cells[1][3] = 0.0F;

				Cells[2][0] = 0.0F;
				Cells[2][1] = 0.0F;
				Cells[2][2] = FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
				//Cells[2][2] = FarClipDistance + NearClipDistance / (NearClipDistance - FarClipDistance); // OpenGL version
				Cells[2][3] = -1.0F;

				Cells[3][0] = 0.0F;
				Cells[3][1] = 0.0F;
				Cells[3][2] = NearClipDistance * FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
				//Cells[3][2] = 2.0F * NearClipDistance * FarClipDistance/ (NearClipDistance - FarClipDistance); // OpenGL version
				Cells[3][3] = 0.0F;
			}

			void MakeOrthographicProjectionMatrix(T Width, T Height, T NearClipDistance, T FarClipDistance)
			{
				Assert(Width != 0, "Width must be non-zero, devide by zero will happen");
				Assert(Height != 0, "Height must be non-zero, devide by zero will happen");
				Assert(NearClipDistance != FarClipDistance, "NearClipDistance and FarClipDistance cannot equals, devide by zero will happen");

				Cells[0][0] = 2.0F / Width;
				Cells[0][1] = 0.0F;
				Cells[0][2] = 0.0F;
				Cells[0][3] = 0.0F;

				Cells[1][0] = 0.0F;
				Cells[1][1] = 2.0F / Height;
				Cells[1][2] = 0.0F;
				Cells[1][3] = 0.0F;

				Cells[2][0] = 0.0F;
				Cells[2][1] = 0.0F;
				Cells[2][2] = 1.0F / (NearClipDistance - FarClipDistance);
				Cells[2][3] = 0.0F;

				Cells[3][0] = 0.0F;
				Cells[3][1] = 0.0F;
				Cells[3][2] = NearClipDistance / (NearClipDistance - FarClipDistance);
				Cells[3][3] = 1.0F;
			}

			Vector3<T> GetRight(void) const
			{
				return Vector3<T>(Cells[0][0], Cells[0][3], Cells[2][0]).GetNormalized();
			}

			Vector3<T> GetUp(void) const
			{
				return Vector3<T>(Cells[0][1], Cells[1][1], Cells[2][1]).GetNormalized();
			}

			Vector3<T> GetForward(void) const
			{
				return Vector3<T>(Cells[0][2], Cells[1][2], Cells[2][2]).GetNormalized();
			}

			const T* GetValue(void) const
			{
				return Cells[0];
			}

		private:
			static T Minor(const Matrix4& Matrix, const size_t r0, const size_t r1, const size_t r2, const size_t c0, const size_t c1, const size_t c2)
			{
				return
					Matrix[r0][c0] * (Matrix[r1][c1] * Matrix[r2][c2] - Matrix[r2][c1] * Matrix[r1][c2]) -
					Matrix[r0][c1] * (Matrix[r1][c0] * Matrix[r2][c2] - Matrix[r2][c0] * Matrix[r1][c2]) +
					Matrix[r0][c2] * (Matrix[r1][c0] * Matrix[r2][c1] - Matrix[r2][c0] * Matrix[r1][c1]);
			}

		public:
			T Cells[4][4];

		public:
			static const Matrix4<T> Zero;
			static const Matrix4<T> Identity;
		};

		template<typename T>
		const Matrix4<T> Matrix4<T>::Zero(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0);

		template<typename T>
		const Matrix4<T> Matrix4<T>::Identity(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
}

#endif