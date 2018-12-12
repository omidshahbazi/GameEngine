// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <Common\PrimitiveTypes.h>
#include <Common\Mathematics.h>
#include <Containers\Vector3.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Containers
	{
		template<typename T>
		class Matrix4
		{
		public:
			Matrix4(void)
			{
				PlatformMemory::Set<T>(m_Cells, 0, 16);
			}

			Matrix4(const Matrix4<T> &Other)
			{
				*this = Other;
			}

			Matrix4<T> &Inverse(void)
			{
				Matrix4<T> temp;

				const Matrix4<T> &m = *this;

				T d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
					(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
					(m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
					(m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
					(m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
					(m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

				if (Mathematics::IsZero(d))
					return *this;

				d = Mathematics::Reciprocal(d);

				temp(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
					m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
					m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
				temp(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
					m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
					m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
				temp(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
					m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
					m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
				temp(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
					m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
					m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
				temp(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
					m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
					m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
				temp(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
					m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
					m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
				temp(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
					m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
					m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
				temp(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
					m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
					m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
				temp(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
					m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
					m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
				temp(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
					m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
					m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
				temp(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
					m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
					m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
				temp(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
					m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
					m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
				temp(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
					m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
					m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
				temp(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
					m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
					m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
				temp(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
					m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
					m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
				temp(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
					m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
					m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));

				*this = temp;

				return *this;
			}

			Matrix4<T> GetInverse(void) const
			{
				Matrix4<T> mat(*this);
				mat.MakeInverse();
				return mat;
			}

			Matrix4<T> &Set(const Vector3<T> &Position, const Vector3<T> &Rotation, const Vector3<T> &Scale)
			{
				SetPosition(Position);
				SetRotation(Rotation);
				SetScale(Scale);

				return *this;
			}

			Matrix4<T> &SetPosition(const Vector3<T> &Position)
			{
				SetPosition(Position.X, Position.Y, Position.Z);

				return *this;
			}

			Matrix4<T> &SetPosition(T X, T Y, T Z)
			{
				m_Cells[12] = X;
				m_Cells[13] = Y;
				m_Cells[14] = Z;

				return *this;
			}

			Vector3<T> GetPosition(void) const
			{
				return Vector3<T>(m_Cells[12], m_Cells[13], m_Cells[14]);
			}

			Matrix4<T> &SetRotation(const Vector3<T> &Rotation)
			{
				return SetRotation(Rotation.X, Rotation.Y, Rotation.Z);
			}

			Matrix4<T> &SetRotation(T X, T Y, T Z)
			{
				const T radiansX = X * Mathematics::DEGREES_TO_RADIANS,
					radiansY = Y * Mathematics::DEGREES_TO_RADIANS,
					radiansZ = Z * Mathematics::DEGREES_TO_RADIANS;

				const T cr = Mathematics::Cos(radiansX);
				const T sr = Mathematics::Sin(radiansX);
				const T cp = Mathematics::Cos(radiansY);
				const T sp = Mathematics::Sin(radiansY);
				const T cy = Mathematics::Cos(radiansZ);
				const T sy = Mathematics::Sin(radiansZ);

				m_Cells[0] = (cp*cy);
				m_Cells[1] = (cp*sy);
				m_Cells[2] = (-sp);

				const T srsp = sr * sp;
				const T crsp = cr * sp;

				m_Cells[4] = (srsp*cy - cr * sy);
				m_Cells[5] = (srsp*sy + cr * cy);
				m_Cells[6] = (sr*cp);

				m_Cells[8] = (crsp*cy + sr * sy);
				m_Cells[9] = (crsp*sy - sr * cy);
				m_Cells[10] = (cr*cp);

				return *this;
			}

			Vector3<T> GetRotation(void) const
			{
				const Matrix4<T> &mat = *this;
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

				T Y = -Mathematics::ASin(Mathematics::Clamp(mat[2] * invScale.X, -1.0F, 1.0F));
				const T C = cos(Y);
				Y *= Mathematics::RADIANS_TO_DEGREES;

				T rotx, roty, X, Z;

				if (!Mathematics::IsZero(C))
				{
					const T invC = Mathematics::Reciprocal(C);
					rotx = mat[10] * invC * invScale.Z;
					roty = mat[6] * invC * invScale.Y;
					X = atan2(roty, rotx) * Mathematics::RADIANS_TO_DEGREES;
					rotx = mat[0] * invC * invScale.X;
					roty = mat[1] * invC * invScale.X;
					Z = atan2(roty, rotx) * Mathematics::RADIANS_TO_DEGREES;
				}
				else
				{
					X = 0.0;
					rotx = mat[5] * invScale.Y;
					roty = -mat[4] * invScale.Y;
					Z = atan2(roty, rotx) * Mathematics::RADIANS_TO_DEGREES;
				}

				// fix values that get below zero
				if (X < 0.0) X += 360.0;
				if (Y < 0.0) Y += 360.0;
				if (Z < 0.0) Z += 360.0;

				return Vector3<T>(X, Y, Z);
			}

			Matrix4<T> &SetScale(const Vector3<T> &Scale)
			{
				SetScale(Scale.X, Scale.Y, Scale.Z);

				return *this;
			}

			Matrix4<T> &SetScale(T X, T Y, T Z)
			{
				m_Cells[0] = X;
				m_Cells[5] = Y;
				m_Cells[10] = Z;

				return *this;
			}

			Matrix4<T> &SetScale(T Scalar)
			{
				m_Cells[0] = m_Cells[5] = m_Cells[10] = Scalar;

				return *this;
			}

			Vector3<T> GetScale(void) const
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

			Vector3<T> GetRight(void) const
			{
				return Vector3<T>(m_Cells[0], m_Cells[4], m_Cells[8]).GetNormalized();
			}

			Vector3<T> GetUp(void) const
			{
				return Vector3<T>(m_Cells[1], m_Cells[5], m_Cells[9]).GetNormalized();
			}

			Vector3<T> GetForward(void) const
			{
				return Vector3<T>(m_Cells[2], m_Cells[6], m_Cells[10]).GetNormalized()
#ifndef LEFT_HAND_MATRIX
					* -1
#endif
					;
			}

			Matrix4<T> &SetRotationCenter(const Vector3<T> &Center, const Vector3<T> &Translate)
			{
				m_Cells[12] = -m_Cells[0] * Center.X - m_Cells[4] * Center.Y - m_Cells[8] * Center.Z + (Center.X - Translate.X);
				m_Cells[13] = -m_Cells[1] * Center.X - m_Cells[5] * Center.Y - m_Cells[9] * Center.Z + (Center.Y - Translate.Y);
				m_Cells[14] = -m_Cells[2] * Center.X - m_Cells[6] * Center.Y - m_Cells[10] * Center.Z + (Center.Z - Translate.Z);
				m_Cells[15] = 1.0F;
			}

			Matrix4<T> &MakeIdentity(void)
			{
				PlatformMemory::Set<T>(m_Cells, 0, 16);

				m_Cells[0] = m_Cells[5] = m_Cells[10] = m_Cells[15] = 1;

				return *this;
			}

			Matrix4<T> &MakePerspectiveProjectionMatrix(T FieldOfView, T AspectRatio, T NearClipDistance, T FarClipDistance)
			{
				Assert(AspectRatio != 0.0F, "AspectRatio must be non-zero, devide by zero will happen");
				Assert(NearClipDistance != FarClipDistance, "NearClipDistance and FarClipDistance cannot equals, devide by zero will happen");

				const T h = Mathematics::Reciprocal(tan((FieldOfView * Mathematics::DEGREES_TO_RADIANS) * 0.5F));
				const T w = h / AspectRatio;

#ifdef LEFT_HAND_MATRIX
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
				m_Cells[10] = FarClipDistance / (FarClipDistance - NearClipDistance);
				m_Cells[11] = 1.0F;

				m_Cells[12] = 0.0F;
				m_Cells[13] = 0.0F;
				m_Cells[14] = -NearClipDistance * FarClipDistance / (FarClipDistance - NearClipDistance);
				m_Cells[15] = 0.0F;
#else
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
																					  //m_Cells[10] = FarClipDistance + NearClipDistance / (NearClipDistance - FarClipDistance)); // OpenGL version
				m_Cells[11] = -1.0F;

				m_Cells[12] = 0.0F;
				m_Cells[13] = 0.0F;
				m_Cells[14] = NearClipDistance * FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
																										 //m_Cells[14] = 2.0F * NearClipDistance * FarClipDistance/ (NearClipDistance - FarClipDistance); // OpenGL version
				m_Cells[15] = 0.0F;
#endif

				return *this;
			}

			Matrix4<T> &MakeOrthographicProjectionMatrix(T Width, T Height, T NearClipDistance, T FarClipDistance)
			{
				Assert(Width != 0, "Width must be non-zero, devide by zero will happen");
				Assert(Height != 0, "Width must be non-zero, devide by zero will happen");
				Assert(NearClipDistance != FarClipDistance, "NearClipDistance and FarClipDistance cannot equals, devide by zero will happen");

#ifdef LEFT_HAND_MATRIX
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
				m_Cells[10] = 1.0F / (FarClipDistance - NearClipDistance);
				m_Cells[11] = 0.0F;

				m_Cells[12] = 0.0F;
				m_Cells[13] = 0.0F;
				m_Cells[14] = NearClipDistance / (NearClipDistance - FarClipDistance);
				m_Cells[15] = 1.0F;
#else
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
#endif

				return *this;
			}

			void TransformVector(Vector3<T> &Vector) const
			{
				Vector.X = Vector.X*m_Cells[0] + Vector.Y*m_Cells[4] + Vector.Z*m_Cells[8] + m_Cells[12];
				Vector.Y = Vector.X*m_Cells[1] + Vector.Y*m_Cells[5] + Vector.Z*m_Cells[9] + m_Cells[13];
				Vector.Z = Vector.X*m_Cells[2] + Vector.Y*m_Cells[6] + Vector.Z*m_Cells[10] + m_Cells[14];
			}

			void TranslateVector(Vector3<T> &Vector) const
			{
				Vector.X += m_Cells[12];
				Vector.Y += m_Cells[13];
				Vector.Z += m_Cells[14];
			}

			void RotateVector(Vector3<T> &Vector) const
			{
				Vector3<T> tmp(Vector);
				Vector.X = tmp.X*m_Cells[0] + tmp.Y*m_Cells[4] + tmp.Z*m_Cells[8];
				Vector.Y = tmp.X*m_Cells[1] + tmp.Y*m_Cells[5] + tmp.Z*m_Cells[9];
				Vector.Z = tmp.X*m_Cells[2] + tmp.Y*m_Cells[6] + tmp.Z*m_Cells[10];
			}

			bool IsIdentity(void) const
			{
				if (!Mathematics::EqualCheck<T>(m_Cells[12], 0) || !Mathematics::EqualCheck<T>(m_Cells[13], 0) || !Mathematics::EqualCheck<T>(m_Cells[14], 0) || !Mathematics::EqualCheck<T>(m_Cells[15], 1))
					return false;

				if (!Mathematics::EqualCheck<T>(m_Cells[0], 1) || !Mathematics::EqualCheck<T>(m_Cells[1], 0) || !Mathematics::EqualCheck<T>(m_Cells[2], 0) || !Mathematics::EqualCheck<T>(m_Cells[3], 0))
					return false;

				if (!Mathematics::EqualCheck<T>(m_Cells[4], 0) || !Mathematics::EqualCheck<T>(m_Cells[5], 1) || !Mathematics::EqualCheck<T>(m_Cells[6], 0) || !Mathematics::EqualCheck<T>(m_Cells[7], 0))
					return false;

				if (!Mathematics::EqualCheck<T>(m_Cells[8], 0) || !Mathematics::EqualCheck<T>(m_Cells[9], 0) || !Mathematics::EqualCheck<T>(m_Cells[10], 1) || !Mathematics::EqualCheck<T>(m_Cells[11], 0))
					return false;

				return true;
			}

			T &operator()(uint8 RowIndex, uint8 ColumnIndex)
			{
				return m_Cells[RowIndex * 4 + ColumnIndex];
			}

			T operator()(uint8 RowIndex, uint8 ColumnIndex) const
			{
				return m_Cells[RowIndex * 4 + ColumnIndex];
			}

			T &operator[](uint8 Index)
			{
				return m_Cells[Index];
			}

			T operator[](uint8 Index) const
			{
				return m_Cells[Index];
			}

			Matrix4<T> &operator=(const Matrix4<T> &Other)
			{
				if (this == &Other)
					return *this;

				PlatformMemory::Copy<T>(Other.m_Cells, m_Cells, 16);

				return *this;
			}

			Matrix4<T> &operator=(T Scalar)
			{
				for (uint8 i = 0; i < 16; ++i)
					m_Cells[i] = Scalar;

				return *this;
			}

			bool operator==(const Matrix4<T> &Other) const
			{
				for (uint8 i = 0; i < 16; ++i)
					if (m_Cells[i] != Other.m_Cells[i])
						return false;

				return true;
			}

			bool operator!=(const Matrix4<T> &Other) const
			{
				return !(*this == Other);
			}

			Matrix4<T> operator+(const Matrix4<T> &Other) const
			{
				Matrix4<T> temp;

				temp[0] = m_Cells[0] + Other[0];
				temp[1] = m_Cells[1] + Other[1];
				temp[2] = m_Cells[2] + Other[2];
				temp[3] = m_Cells[3] + Other[3];
				temp[4] = m_Cells[4] + Other[4];
				temp[5] = m_Cells[5] + Other[5];
				temp[6] = m_Cells[6] + Other[6];
				temp[7] = m_Cells[7] + Other[7];
				temp[8] = m_Cells[8] + Other[8];
				temp[9] = m_Cells[9] + Other[9];
				temp[10] = m_Cells[10] + Other[10];
				temp[11] = m_Cells[11] + Other[11];
				temp[12] = m_Cells[12] + Other[12];
				temp[13] = m_Cells[13] + Other[13];
				temp[14] = m_Cells[14] + Other[14];
				temp[15] = m_Cells[15] + Other[15];

				return temp;
			}

			Matrix4<T> operator-(const Matrix4<T> &Other) const
			{
				Matrix4<T> temp;

				temp[0] = m_Cells[0] - Other[0];
				temp[1] = m_Cells[1] - Other[1];
				temp[2] = m_Cells[2] - Other[2];
				temp[3] = m_Cells[3] - Other[3];
				temp[4] = m_Cells[4] - Other[4];
				temp[5] = m_Cells[5] - Other[5];
				temp[6] = m_Cells[6] - Other[6];
				temp[7] = m_Cells[7] - Other[7];
				temp[8] = m_Cells[8] - Other[8];
				temp[9] = m_Cells[9] - Other[9];
				temp[10] = m_Cells[10] - Other[10];
				temp[11] = m_Cells[11] - Other[11];
				temp[12] = m_Cells[12] - Other[12];
				temp[13] = m_Cells[13] - Other[13];
				temp[14] = m_Cells[14] - Other[14];
				temp[15] = m_Cells[15] - Other[15];

				return temp;
			}

			Matrix4<T> operator*(const Matrix4<T> &Other) const
			{
				Matrix4<T> temp;

				const T *m1 = m_Cells;
				const T *m2 = Matrix.m_Cells;
				T *m3 = temp.m_Cells;

				m3[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
				m3[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
				m3[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
				m3[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

				m3[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
				m3[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
				m3[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
				m3[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

				m3[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
				m3[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
				m3[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
				m3[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

				m3[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
				m3[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
				m3[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
				m3[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

				return temp;
			}

			Matrix4<T> operator*(T Scalar) const
			{
				Matrix4<T> temp;

				temp[0] = m_Cells[0] * Scalar;
				temp[1] = m_Cells[1] * Scalar;
				temp[2] = m_Cells[2] * Scalar;
				temp[3] = m_Cells[3] * Scalar;
				temp[4] = m_Cells[4] * Scalar;
				temp[5] = m_Cells[5] * Scalar;
				temp[6] = m_Cells[6] * Scalar;
				temp[7] = m_Cells[7] * Scalar;
				temp[8] = m_Cells[8] * Scalar;
				temp[9] = m_Cells[9] * Scalar;
				temp[10] = m_Cells[10] * Scalar;
				temp[11] = m_Cells[11] * Scalar;
				temp[12] = m_Cells[12] * Scalar;
				temp[13] = m_Cells[13] * Scalar;
				temp[14] = m_Cells[14] * Scalar;
				temp[15] = m_Cells[15] * Scalar;

				return temp;
			}

			Matrix4<T> &operator+=(const Matrix4<T> &Other)
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

			Matrix4<T> &operator-=(const Matrix4<T> &Other)
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

			Matrix4<T> &operator*=(const Matrix4<T> &Other)
			{
				const Matrix4<T> temp(*this);

				const T *m1 = temp.m_Cells;
				const T *m2 = Other.m_Cells;

				m_Cells[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
				m_Cells[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
				m_Cells[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
				m_Cells[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

				m_Cells[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
				m_Cells[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
				m_Cells[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
				m_Cells[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

				m_Cells[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
				m_Cells[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
				m_Cells[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
				m_Cells[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

				m_Cells[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
				m_Cells[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
				m_Cells[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
				m_Cells[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

				return *this;
			}

			Matrix4<T> &operator*=(T Scalar)
			{
				Matrix4<T> temp;

				temp[0] = m_Cells[0] * Scalar;
				temp[1] = m_Cells[1] * Scalar;
				temp[2] = m_Cells[2] * Scalar;
				temp[3] = m_Cells[3] * Scalar;
				temp[4] = m_Cells[4] * Scalar;
				temp[5] = m_Cells[5] * Scalar;
				temp[6] = m_Cells[6] * Scalar;
				temp[7] = m_Cells[7] * Scalar;
				temp[8] = m_Cells[8] * Scalar;
				temp[9] = m_Cells[9] * Scalar;
				temp[10] = m_Cells[10] * Scalar;
				temp[11] = m_Cells[11] * Scalar;
				temp[12] = m_Cells[12] * Scalar;
				temp[13] = m_Cells[13] * Scalar;
				temp[14] = m_Cells[14] * Scalar;
				temp[15] = m_Cells[15] * Scalar;

				return temp;
			}

			const T *GetValue(void) const
			{
				return m_Cells;
			}

		private:
			T m_Cells[16];
		};
	}
}

#endif