// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <Common\PrimitiveTypes.h>
#include <Common\Mathematics.h>
#include <Debugging\Debug.h>
#include <MathContainers\Vector3.h>
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
			Matrix4(void) :
				m_Cells{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			{
			}

			Matrix4(
				T m00, T m01, T m02, T m03,
				T m10, T m11, T m12, T m13,
				T m20, T m21, T m22, T m23,
				T m30, T m31, T m32, T m33)
			{
				m_Cells[0] = m00;
				m_Cells[1] = m01;
				m_Cells[2] = m02;
				m_Cells[3] = m03;
				m_Cells[4] = m10;
				m_Cells[5] = m11;
				m_Cells[6] = m12;
				m_Cells[7] = m13;
				m_Cells[8] = m20;
				m_Cells[9] = m21;
				m_Cells[10] = m22;
				m_Cells[11] = m23;
				m_Cells[12] = m30;
				m_Cells[13] = m31;
				m_Cells[14] = m32;
				m_Cells[15] = m33;
			}

			INLINE void SetTranslate(const Vector3<T>& Value)
			{
				m_Cells[12] = Value.X;
				m_Cells[13] = Value.Y;
				m_Cells[14] = Value.Z;
			}

			INLINE 	Vector3<T> GetTranslate(void) const
			{
				return Vector3<T>(m_Cells[12], m_Cells[13], m_Cells[14]);
			}

			INLINE void SetRotation(const Vector3<T>& Value)
			{
				const T cr = Mathematics::Cos(Value.X);
				const T sr = Mathematics::Sin(Value.X);
				const T cp = Mathematics::Cos(Value.Y);
				const T sp = Mathematics::Sin(Value.Y);
				const T cy = Mathematics::Cos(Value.Z);
				const T sy = Mathematics::Sin(Value.Z);

				m_Cells[0] = (cp * cy);
				m_Cells[1] = (cp * sy);
				m_Cells[2] = (-sp);

				const T srsp = sr * sp;
				const T crsp = cr * sp;

				m_Cells[4] = (srsp * cy - cr * sy);
				m_Cells[5] = (srsp * sy + cr * cy);
				m_Cells[6] = (sr * cp);

				m_Cells[8] = (crsp * cy + sr * sy);
				m_Cells[9] = (crsp * sy - sr * cy);
				m_Cells[10] = (cr * cp);
			}

			INLINE Vector3<T> GetRotation(void) const
			{
				Vector3<T> scale = GetScale();
				// we need to check for negative scale on to axes, which would bring up wrong results
				if (scale.Y < 0 && scale.Z < 0)
				{
					scale.Y = -scale.Y;
					scale.Z = -scale.Z;
				}
				else if (scale.X < 0 && scale.Z < 0)
				{
					scale.X = -scale.X;
					scale.Z = -scale.Z;
				}
				else if (scale.X < 0 && scale.Y < 0)
				{
					scale.X = -scale.X;
					scale.Y = -scale.Y;
				}

				const Vector3<T> invScale(Mathematics::Reciprocal(scale.X), Mathematics::Reciprocal(scale.Y), Mathematics::Reciprocal(scale.Z));

				T y = -Mathematics::ASin(Mathematics::Clamp(m_Cells[2] * invScale.X, -1.0F, 1.0F));
				const T C = Mathematics::Cos(y);

				T rotX, rotY, x, z;

				if (!Mathematics::IsZero(C))
				{
					const T invC = Mathematics::Reciprocal(C);

					rotX = m_Cells[10] * invC * invScale.Z;
					rotY = m_Cells[6] * invC * invScale.Y;
					x = Mathematics::ATan2(rotY, rotX);

					rotX = m_Cells[0] * invC * invScale.X;
					rotY = m_Cells[1] * invC * invScale.X;
					z = Mathematics::ATan2(rotY, rotX)
				}
				else
				{
					x = 0;

					rotX = m_Cells[5] * invScale.Y;
					rotY = -m_Cells[4] * invScale.Y;
					z = Mathematics::ATan2(rotY, rotX);
				}

				// fix values that get below zero
				if (x < 0) x += Mathematics::PI * 2;
				if (y < 0) y += Mathematics::PI * 2;
				if (z < 0) z += Mathematics::PI * 2;

				return Vector3<T>(x, y, z);
			}

			INLINE void SetScale(const Vector3<T>& Value)
			{
				m_Cells[0] = Value.X;
				m_Cells[5] = Value.Y;
				m_Cells[10] = Value.Z;
			}

			INLINE Vector3<T> GetScale(void) const
			{
				// See http://www.robertblum.com/articles/2005/02/14/decomposing-matrices

				if (Mathematics::IsZero(m_Cells[1]) && Mathematics::IsZero(m_Cells[2]) &&
					Mathematics::IsZero(m_Cells[4]) && Mathematics::IsZero(m_Cells[6]) &&
					Mathematics::IsZero(m_Cells[8]) && Mathematics::IsZero(m_Cells[9]))
					return Vector3<T>(m_Cells[0], m_Cells[5], m_Cells[10]);

				// We have to do the full calculation.
				return Vector3<T>(
					Mathematics::SquareRoot(m_Cells[0] * m_Cells[0] + m_Cells[1] * m_Cells[1] + m_Cells[2] * m_Cells[2]),
					Mathematics::SquareRoot(m_Cells[4] * m_Cells[4] + m_Cells[5] * m_Cells[5] + m_Cells[6] * m_Cells[6]),
					Mathematics::SquareRoot(m_Cells[8] * m_Cells[8] + m_Cells[9] * m_Cells[9] + m_Cells[10] * m_Cells[10]));
			}

			INLINE Vector3<T> GetRight(void) const
			{
				return Vector3<T>(m_Cells[0], m_Cells[4], m_Cells[8]).GetNormalized();
			}

			INLINE 	Vector3<T> GetUp(void) const
			{
				return Vector3<T>(m_Cells[1], m_Cells[5], m_Cells[9]).GetNormalized();
			}

			INLINE 	Vector3<T> GetForward(void) const
			{
				return Vector3<T>(m_Cells[2], m_Cells[6], m_Cells[10]).GetNormalized();
			}

			INLINE 	void SetTransform(const Vector3<T>& Translate, const Vector3<T>& Rotation, const Vector3<T>& Scale)
			{
				SetTranslate(Translate);
				SetRotation(Rotation);
				SetScale(Scale);
			}

			INLINE void SetPerspectiveProjection(T FieldOfView, T AspectRatio, T NearClipDistance, T FarClipDistance)
			{
				const T h = Mathematics::Reciprocal(Mathematics::Tan(FieldOfView * 0.5F));
				const T w = h / AspectRatio;

				m_Cells[0] = w;
				m_Cells[1] = 0.0F;
				m_Cells[2] = 0.0F;
				m_Cells[3] = 0.0F;

				m_Cells[4] = 0.0F;
				m_Cells[5] = h;
				m_Cells[6] = 0.0F;
				m_Cells[7] = 0.0F;

				m_Cells[8] = 0.0F;
				m_Cells[9] = 0.0F;
				m_Cells[10] = FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
				//m_Cells[10] = FarClipDistance + NearClipDistance / (NearClipDistance - FarClipDistance); // OpenGL version
				m_Cells[11] = -1.0F;

				m_Cells[12] = 0.0F;
				m_Cells[13] = 0.0F;
				m_Cells[14] = NearClipDistance * FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
				//m_Cells[14] = 2.0F * NearClipDistance * FarClipDistance/ (NearClipDistance - FarClipDistance); // OpenGL version
				m_Cells[15] = 0.0F;
			}

			INLINE void SetOrthographicProjection(T Width, T Height, T NearClipDistance, T FarClipDistance)
			{
				m_Cells[0] = 2.0F / Width;
				m_Cells[1] = 0.0F;
				m_Cells[2] = 0.0F;
				m_Cells[3] = 0.0F;

				m_Cells[4] = 0.0F;
				m_Cells[5] = 2.0F / Height;
				m_Cells[6] = 0.0F;
				m_Cells[7] = 0.0F;

				m_Cells[8] = 0.0F;
				m_Cells[9] = 0.0F;
				m_Cells[10] = 1.0F / (NearClipDistance - FarClipDistance);
				m_Cells[11] = 0.0F;

				m_Cells[12] = 0.0F;
				m_Cells[13] = 0.0F;
				m_Cells[14] = NearClipDistance / (NearClipDistance - FarClipDistance);
				m_Cells[15] = 1.0F;
			}

			INLINE 	T GetDeterminant(void) const
			{
				return
					(m_Cells[0] * m_Cells[5] - m_Cells[1] * m_Cells[4]) * (m_Cells[10] * m_Cells[15] - m_Cells[11] * m_Cells[14]) -
					(m_Cells[0] * m_Cells[6] - m_Cells[2] * m_Cells[4]) * (m_Cells[9] * m_Cells[15] - m_Cells[11] * m_Cells[13]) +
					(m_Cells[0] * m_Cells[7] - m_Cells[3] * m_Cells[4]) * (m_Cells[9] * m_Cells[14] - m_Cells[10] * m_Cells[13]) +
					(m_Cells[1] * m_Cells[6] - m_Cells[2] * m_Cells[5]) * (m_Cells[8] * m_Cells[15] - m_Cells[11] * m_Cells[12]) -
					(m_Cells[1] * m_Cells[7] - m_Cells[3] * m_Cells[5]) * (m_Cells[8] * m_Cells[14] - m_Cells[10] * m_Cells[12]) +
					(m_Cells[1] * m_Cells[7] - m_Cells[3] * m_Cells[6]) * (m_Cells[8] * m_Cells[13] - m_Cells[9] * m_Cells[12]);
			}

			INLINE 	void Inverse(void)
			{
				const Matrix4<T>& m = *this;

				T d = GetDeterminant();

				if (Mathematics::IsZero(d))
					return *this;

				d = Mathematics::Reciprocal(d);

				Matrix4<T> mat;
				mat.m_Cells[0] = d * (m_Cells[5] * (m_Cells[10] * m_Cells[15] - m_Cells[11] * m_Cells[14]) + m_Cells[6] * (m_Cells[11] * m_Cells[13] - m_Cells[9] * m_Cells[15]) + m_Cells[7] * (m_Cells[9] * m_Cells[14] - m_Cells[10] * m_Cells[13]));
				mat.m_Cells[1] = d * (m_Cells[9] * (m_Cells[2] * m_Cells[15] - m_Cells[3] * m_Cells[14]) + m_Cells[10] * (m_Cells[3] * m_Cells[13] - m_Cells[1] * m_Cells[15]) + m_Cells[11] * (m_Cells[1] * m_Cells[14] - m_Cells[2] * m_Cells[13]));
				mat.m_Cells[2] = d * (m_Cells[13] * (m_Cells[2] * m_Cells[7] - m_Cells[3] * m_Cells[6]) + m_Cells[14] * (m_Cells[3] * m_Cells[5] - m_Cells[1] * m_Cells[7]) + m_Cells[15] * (m_Cells[1] * m_Cells[6] - m_Cells[2] * m_Cells[5]));
				mat.m_Cells[3] = d * (m_Cells[1] * (m_Cells[7] * m_Cells[10] - m_Cells[6] * m_Cells[11]) + m_Cells[2] * (m_Cells[5] * m_Cells[11] - m_Cells[7] * m_Cells[9]) + m_Cells[3] * (m_Cells[6] * m_Cells[9] - m_Cells[5] * m_Cells[10]));
				mat.m_Cells[4] = d * (m_Cells[6] * (m_Cells[8] * m_Cells[15] - m_Cells[11] * m_Cells[12]) + m_Cells[7] * (m_Cells[10] * m_Cells[12] - m_Cells[8] * m_Cells[14]) + m_Cells[4] * (m_Cells[11] * m_Cells[14] - m_Cells[10] * m_Cells[15]));
				mat.m_Cells[5] = d * (m_Cells[10] * (m_Cells[0] * m_Cells[15] - m_Cells[3] * m_Cells[12]) + m_Cells[11] * (m_Cells[2] * m_Cells[12] - m_Cells[0] * m_Cells[14]) + m_Cells[8] * (m_Cells[3] * m_Cells[14] - m_Cells[2] * m_Cells[15]));
				mat.m_Cells[6] = d * (m_Cells[14] * (m_Cells[0] * m_Cells[7] - m_Cells[3] * m_Cells[4]) + m_Cells[15] * (m_Cells[2] * m_Cells[4] - m_Cells[0] * m_Cells[6]) + m_Cells[12] * (m_Cells[3] * m_Cells[6] - m_Cells[2] * m_Cells[7]));
				mat.m_Cells[7] = d * (m_Cells[2] * (m_Cells[7] * m_Cells[8] - m_Cells[4] * m_Cells[11]) + m_Cells[3] * (m_Cells[4] * m_Cells[10] - m_Cells[6] * m_Cells[8]) + m_Cells[0] * (m_Cells[6] * m_Cells[11] - m_Cells[7] * m_Cells[10]));
				mat.m_Cells[8] = d * (m_Cells[7] * (m_Cells[8] * m_Cells[13] - m_Cells[9] * m_Cells[12]) + m_Cells[4] * (m_Cells[9] * m_Cells[15] - m_Cells[11] * m_Cells[13]) + m_Cells[5] * (m_Cells[11] * m_Cells[12] - m_Cells[8] * m_Cells[15]));
				mat.m_Cells[9] = d * (m_Cells[11] * (m_Cells[0] * m_Cells[13] - m_Cells[1] * m_Cells[12]) + m_Cells[8] * (m_Cells[1] * m_Cells[15] - m_Cells[3] * m_Cells[13]) + m_Cells[9] * (m_Cells[3] * m_Cells[12] - m_Cells[0] * m_Cells[15]));
				mat.m_Cells[10] = d * (m_Cells[15] * (m_Cells[0] * m_Cells[5] - m_Cells[1] * m_Cells[4]) + m_Cells[12] * (m_Cells[1] * m_Cells[7] - m_Cells[3] * m_Cells[5]) + m_Cells[13] * (m_Cells[3] * m_Cells[4] - m_Cells[0] * m_Cells[7]));
				mat.m_Cells[11] = d * (m_Cells[3] * (m_Cells[5] * m_Cells[8] - m_Cells[4] * m_Cells[9]) + m_Cells[0] * (m_Cells[7] * m_Cells[9] - m_Cells[5] * m_Cells[11]) + m_Cells[1] * (m_Cells[4] * m_Cells[11] - m_Cells[7] * m_Cells[8]));
				mat.m_Cells[12] = d * (m_Cells[4] * (m_Cells[10] * m_Cells[13] - m_Cells[9] * m_Cells[14]) + m_Cells[5] * (m_Cells[8] * m_Cells[14] - m_Cells[10] * m_Cells[12]) + m_Cells[6] * (m_Cells[9] * m_Cells[12] - m_Cells[8] * m_Cells[13]));
				mat.m_Cells[13] = d * (m_Cells[8] * (m_Cells[2] * m_Cells[13] - m_Cells[1] * m_Cells[14]) + m_Cells[9] * (m_Cells[0] * m_Cells[14] - m_Cells[2] * m_Cells[12]) + m_Cells[10] * (m_Cells[1] * m_Cells[12] - m_Cells[0] * m_Cells[13]));
				mat.m_Cells[14] = d * (m_Cells[12] * (m_Cells[2] * m_Cells[5] - m_Cells[1] * m_Cells[6]) + m_Cells[13] * (m_Cells[0] * m_Cells[6] - m_Cells[2] * m_Cells[4]) + m_Cells[14] * (m_Cells[1] * m_Cells[4] - m_Cells[0] * m_Cells[5]));
				mat.m_Cells[15] = d * (m_Cells[0] * (m_Cells[5] * m_Cells[10] - m_Cells[6] * m_Cells[9]) + m_Cells[1] * (m_Cells[6] * m_Cells[8] - m_Cells[4] * m_Cells[10]) + m_Cells[2] * (m_Cells[4] * m_Cells[9] - m_Cells[5] * m_Cells[8]));

				*this = mat;
			}

			INLINE 	Matrix4<T> GetInverse(void) const
			{
				Matrix4<T> mat(*this);
				mat.Inverse();
				return mat;
			}

			INLINE 	bool GetIsIdentity(void) const
			{
				if (!Mathematics::EqualCheck(m_Cells[12], 0) || !Mathematics::EqualCheck(m_Cells[13], 0) || !Mathematics::EqualCheck(m_Cells[14], 0) || !Mathematics::EqualCheck(m_Cells[15], 1))
					return false;

				if (!Mathematics::EqualCheck(m_Cells[0], 1) || !Mathematics::EqualCheck(m_Cells[1], 0) || !Mathematics::EqualCheck(m_Cells[2], 0) || !Mathematics::EqualCheck(m_Cells[3], 0))
					return false;

				if (!Mathematics::EqualCheck(m_Cells[4], 0) || !Mathematics::EqualCheck(m_Cells[5], 1) || !Mathematics::EqualCheck(m_Cells[6], 0) || !Mathematics::EqualCheck(m_Cells[7], 0))
					return false;

				if (!Mathematics::EqualCheck(m_Cells[8], 0) || !Mathematics::EqualCheck(m_Cells[9], 0) || !Mathematics::EqualCheck(m_Cells[10], 1) || !Mathematics::EqualCheck(m_Cells[11], 0))
					return false;

				return true;
			}

			INLINE 	T& operator[](uint8 Index)
			{
				return m_Cells[Index];
			}

			INLINE T operator[](uint8 Index) const
			{
				return m_Cells[Index];
			}

			INLINE Matrix4<T>& operator=(T Scalar)
			{
				for (uint8 i = 0; i < 16; ++i)
					m_Cells[i] = Scalar;

				return *this;
			}

			INLINE bool operator==(const Matrix4<T>& Other) const
			{
				for (uint8 i = 0; i < 16; ++i)
					if (m_Cells[i] != Other.m_Cells[i])
						return false;

				return true;
			}

			INLINE bool operator!=(const Matrix4<T>& Other) const
			{
				return !(*this == Other);
			}

			INLINE Matrix4<T> operator+(const Matrix4<T>& Other) const
			{
				Matrix4<T> mat;

				mat.m_Cells[0] = m_Cells[0] + Other.m_Cells[0];
				mat.m_Cells[1] = m_Cells[1] + Other.m_Cells[1];
				mat.m_Cells[2] = m_Cells[2] + Other.m_Cells[2];
				mat.m_Cells[3] = m_Cells[3] + Other.m_Cells[3];
				mat.m_Cells[4] = m_Cells[4] + Other.m_Cells[4];
				mat.m_Cells[5] = m_Cells[5] + Other.m_Cells[5];
				mat.m_Cells[6] = m_Cells[6] + Other.m_Cells[6];
				mat.m_Cells[7] = m_Cells[7] + Other.m_Cells[7];
				mat.m_Cells[8] = m_Cells[8] + Other.m_Cells[8];
				mat.m_Cells[9] = m_Cells[9] + Other.m_Cells[9];
				mat.m_Cells[10] = m_Cells[10] + Other.m_Cells[10];
				mat.m_Cells[11] = m_Cells[11] + Other.m_Cells[11];
				mat.m_Cells[12] = m_Cells[12] + Other.m_Cells[12];
				mat.m_Cells[13] = m_Cells[13] + Other.m_Cells[13];
				mat.m_Cells[14] = m_Cells[14] + Other.m_Cells[14];
				mat.m_Cells[15] = m_Cells[15] + Other.m_Cells[15];

				return mat;
			}

			INLINE Matrix4<T> operator-(const Matrix4<T>& Other) const
			{
				Matrix4<T> mat;

				mat.m_Cells[0] = m_Cells[0] - Other.m_Cells[0];
				mat.m_Cells[1] = m_Cells[1] - Other.m_Cells[1];
				mat.m_Cells[2] = m_Cells[2] - Other.m_Cells[2];
				mat.m_Cells[3] = m_Cells[3] - Other.m_Cells[3];
				mat.m_Cells[4] = m_Cells[4] - Other.m_Cells[4];
				mat.m_Cells[5] = m_Cells[5] - Other.m_Cells[5];
				mat.m_Cells[6] = m_Cells[6] - Other.m_Cells[6];
				mat.m_Cells[7] = m_Cells[7] - Other.m_Cells[7];
				mat.m_Cells[8] = m_Cells[8] - Other.m_Cells[8];
				mat.m_Cells[9] = m_Cells[9] - Other.m_Cells[9];
				mat.m_Cells[10] = m_Cells[10] - Other.m_Cells[10];
				mat.m_Cells[11] = m_Cells[11] - Other.m_Cells[11];
				mat.m_Cells[12] = m_Cells[12] - Other.m_Cells[12];
				mat.m_Cells[13] = m_Cells[13] - Other.m_Cells[13];
				mat.m_Cells[14] = m_Cells[14] - Other.m_Cells[14];
				mat.m_Cells[15] = m_Cells[15] - Other.m_Cells[15];

				return mat;
			}

			INLINE Matrix4<T> operator*(const Matrix4<T>& Other) const
			{
				Matrix4<T> mat(*this);
				mat *= Other;
				return mat;
			}

			INLINE Matrix4<T> operator*(T Scalar) const
			{
				Matrix4<T> mat;

				mat.m_Cells[0] = m_Cells[0] * Scalar;
				mat.m_Cells[1] = m_Cells[1] * Scalar;
				mat.m_Cells[2] = m_Cells[2] * Scalar;
				mat.m_Cells[3] = m_Cells[3] * Scalar;
				mat.m_Cells[4] = m_Cells[4] * Scalar;
				mat.m_Cells[5] = m_Cells[5] * Scalar;
				mat.m_Cells[6] = m_Cells[6] * Scalar;
				mat.m_Cells[7] = m_Cells[7] * Scalar;
				mat.m_Cells[8] = m_Cells[8] * Scalar;
				mat.m_Cells[9] = m_Cells[9] * Scalar;
				mat.m_Cells[10] = m_Cells[10] * Scalar;
				mat.m_Cells[11] = m_Cells[11] * Scalar;
				mat.m_Cells[12] = m_Cells[12] * Scalar;
				mat.m_Cells[13] = m_Cells[13] * Scalar;
				mat.m_Cells[14] = m_Cells[14] * Scalar;
				mat.m_Cells[15] = m_Cells[15] * Scalar;

				return mat;
			}

			INLINE Matrix4<T>& operator+=(const Matrix4<T>& Other)
			{
				m_Cells[0] += Other[0];
				m_Cells[1] += Other[1];
				m_Cells[2] += Other[2];
				m_Cells[3] += Other[3];
				m_Cells[4] += Other[4];
				m_Cells[5] += Other[5];
				m_Cells[6] += Other[6];
				m_Cells[7] += Other[7];
				m_Cells[8] += Other[8];
				m_Cells[9] += Other[9];
				m_Cells[10] += Other[10];
				m_Cells[11] += Other[11];
				m_Cells[12] += Other[12];
				m_Cells[13] += Other[13];
				m_Cells[14] += Other[14];
				m_Cells[15] += Other[15];

				return *this;
			}

			INLINE Matrix4<T>& operator-=(const Matrix4<T>& Other)
			{
				m_Cells[0] -= Other[0];
				m_Cells[1] -= Other[1];
				m_Cells[2] -= Other[2];
				m_Cells[3] -= Other[3];
				m_Cells[4] -= Other[4];
				m_Cells[5] -= Other[5];
				m_Cells[6] -= Other[6];
				m_Cells[7] -= Other[7];
				m_Cells[8] -= Other[8];
				m_Cells[9] -= Other[9];
				m_Cells[10] -= Other[10];
				m_Cells[11] -= Other[11];
				m_Cells[12] -= Other[12];
				m_Cells[13] -= Other[13];
				m_Cells[14] -= Other[14];
				m_Cells[15] -= Other[15];

				return *this;
			}

			INLINE Matrix4<T>& operator*=(const Matrix4<T>& Other)
			{
				const Matrix4<T> tempMat(*this);

				m_Cells[0] = tempMat.m_Cells[0] * Other.m_Cells[0] + tempMat.m_Cells[4] * Other.m_Cells[1] + tempMat.m_Cells[8] * Other.m_Cells[2] + tempMat.m_Cells[12] * Other.m_Cells[3];
				m_Cells[1] = tempMat.m_Cells[1] * Other.m_Cells[0] + tempMat.m_Cells[5] * Other.m_Cells[1] + tempMat.m_Cells[9] * Other.m_Cells[2] + tempMat.m_Cells[13] * Other.m_Cells[3];
				m_Cells[2] = tempMat.m_Cells[2] * Other.m_Cells[0] + tempMat.m_Cells[6] * Other.m_Cells[1] + tempMat.m_Cells[10] * Other.m_Cells[2] + tempMat.m_Cells[14] * Other.m_Cells[3];
				m_Cells[3] = tempMat.m_Cells[3] * Other.m_Cells[0] + tempMat.m_Cells[7] * Other.m_Cells[1] + tempMat.m_Cells[11] * Other.m_Cells[2] + tempMat.m_Cells[15] * Other.m_Cells[3];

				m_Cells[4] = tempMat.m_Cells[0] * Other.m_Cells[4] + tempMat.m_Cells[4] * Other.m_Cells[5] + tempMat.m_Cells[8] * Other.m_Cells[6] + tempMat.m_Cells[12] * Other.m_Cells[7];
				m_Cells[5] = tempMat.m_Cells[1] * Other.m_Cells[4] + tempMat.m_Cells[5] * Other.m_Cells[5] + tempMat.m_Cells[9] * Other.m_Cells[6] + tempMat.m_Cells[13] * Other.m_Cells[7];
				m_Cells[6] = tempMat.m_Cells[2] * Other.m_Cells[4] + tempMat.m_Cells[6] * Other.m_Cells[5] + tempMat.m_Cells[10] * Other.m_Cells[6] + tempMat.m_Cells[14] * Other.m_Cells[7];
				m_Cells[7] = tempMat.m_Cells[3] * Other.m_Cells[4] + tempMat.m_Cells[7] * Other.m_Cells[5] + tempMat.m_Cells[11] * Other.m_Cells[6] + tempMat.m_Cells[15] * Other.m_Cells[7];

				m_Cells[8] = tempMat.m_Cells[0] * Other.m_Cells[8] + tempMat.m_Cells[4] * Other.m_Cells[9] + tempMat.m_Cells[8] * Other.m_Cells[10] + tempMat.m_Cells[12] * Other.m_Cells[11];
				m_Cells[9] = tempMat.m_Cells[1] * Other.m_Cells[8] + tempMat.m_Cells[5] * Other.m_Cells[9] + tempMat.m_Cells[9] * Other.m_Cells[10] + tempMat.m_Cells[13] * Other.m_Cells[11];
				m_Cells[10] = tempMat.m_Cells[2] * Other.m_Cells[8] + tempMat.m_Cells[6] * Other.m_Cells[9] + tempMat.m_Cells[10] * Other.m_Cells[10] + tempMat.m_Cells[14] * Other.m_Cells[11];
				m_Cells[11] = tempMat.m_Cells[3] * Other.m_Cells[8] + tempMat.m_Cells[7] * Other.m_Cells[9] + tempMat.m_Cells[11] * Other.m_Cells[10] + tempMat.m_Cells[15] * Other.m_Cells[11];

				m_Cells[12] = tempMat.m_Cells[0] * Other.m_Cells[12] + tempMat.m_Cells[4] * Other.m_Cells[13] + tempMat.m_Cells[8] * Other.m_Cells[14] + tempMat.m_Cells[12] * Other.m_Cells[15];
				m_Cells[13] = tempMat.m_Cells[1] * Other.m_Cells[12] + tempMat.m_Cells[5] * Other.m_Cells[13] + tempMat.m_Cells[9] * Other.m_Cells[14] + tempMat.m_Cells[13] * Other.m_Cells[15];
				m_Cells[14] = tempMat.m_Cells[2] * Other.m_Cells[12] + tempMat.m_Cells[6] * Other.m_Cells[13] + tempMat.m_Cells[10] * Other.m_Cells[14] + tempMat.m_Cells[14] * Other.m_Cells[15];
				m_Cells[15] = tempMat.m_Cells[3] * Other.m_Cells[12] + tempMat.m_Cells[7] * Other.m_Cells[13] + tempMat.m_Cells[11] * Other.m_Cells[14] + tempMat.m_Cells[15] * Other.m_Cells[15];

				return *this;
			}

			INLINE Matrix4<T>& operator*=(T Scalar)
			{
				Matrix4<T> mat;

				mat.m_Cells[0] = m_Cells[0] * Scalar;
				mat.m_Cells[1] = m_Cells[1] * Scalar;
				mat.m_Cells[2] = m_Cells[2] * Scalar;
				mat.m_Cells[3] = m_Cells[3] * Scalar;
				mat.m_Cells[4] = m_Cells[4] * Scalar;
				mat.m_Cells[5] = m_Cells[5] * Scalar;
				mat.m_Cells[6] = m_Cells[6] * Scalar;
				mat.m_Cells[7] = m_Cells[7] * Scalar;
				mat.m_Cells[8] = m_Cells[8] * Scalar;
				mat.m_Cells[9] = m_Cells[9] * Scalar;
				mat.m_Cells[10] = m_Cells[10] * Scalar;
				mat.m_Cells[11] = m_Cells[11] * Scalar;
				mat.m_Cells[12] = m_Cells[12] * Scalar;
				mat.m_Cells[13] = m_Cells[13] * Scalar;
				mat.m_Cells[14] = m_Cells[14] * Scalar;
				mat.m_Cells[15] = m_Cells[15] * Scalar;

				return temp;
			}

			INLINE const T* GetValue(void) const
			{
				return m_Cells;
			}

			INLINE 	static Matrix4<T> MakeTransformMatrix(const Vector3<T>& Translate, const Vector3<T>& Rotation, const Vector3<T>& Scale)
			{
				Matrix4<T> mat;
				mat.SetTransform(Translate, Rotation, Scale);
				return mat;
			}

			INLINE static Matrix4<T> MakePerspectiveProjectionMatrix(T FieldOfView, T AspectRatio, T NearClipDistance, T FarClipDistance)
			{
				Matrix4<T> mat;
				mat.SetPerspectiveProjection(FieldOfView, AspectRatio, NearClipDistance, FarClipDistance);
				return mat;
			}

			INLINE 	static Matrix4<T> MakeOrthographicProjectionMatrix(T Width, T Height, T NearClipDistance, T FarClipDistance)
			{
				Matrix4<T> mat;
				mat.SetOrthographicProjection(Width, Height, NearClipDistance, FarClipDistance);
				return mat;
			}

		private:
			T m_Cells[16];

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