// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MATRIX_3_H
#define MATRIX_3_H

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
		class Matrix3
		{
		public:
			Matrix3(bool InitialIdentity = false)
			{
				if (InitialIdentity)
					MakeIdentity();
				else
					PlatformMemory::Set<T>(Cells, 0, 9);
			}

			Matrix3(const Matrix3<T>& Other)
			{
				*this = Other;
			}

			Matrix3<T>& Inverse(void)
			{
				Matrix3<T> temp;

				const Matrix3<T>& m = *this;

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

			Matrix3<T> GetInverse(void) const
			{
				Matrix3<T> mat(*this);
				mat.MakeInverse();
				return mat;
			}

			Matrix3<T>& Set(const Vector3<T>& Position, const Vector3<T>& Rotation, const Vector3<T>& Scale)
			{
				SetPosition(Position);
				SetRotation(Rotation);
				SetScale(Scale);

				return *this;
			}

			Matrix3<T>& SetPosition(const Vector3<T>& Position)
			{
				SetPosition(Position.X, Position.Y, Position.Z);

				return *this;
			}

			Matrix3<T>& SetPosition(T X, T Y, T Z)
			{
				Cells[12] = X;
				Cells[13] = Y;
				Cells[14] = Z;

				return *this;
			}

			Vector3<T> GetPosition(void) const
			{
				return Vector3<T>(Cells[12], Cells[13], Cells[14]);
			}

			Matrix3<T>& SetRotation(const Vector3<T>& Rotation)
			{
				return SetRotation(Rotation.X, Rotation.Y, Rotation.Z);
			}

			Matrix3<T>& SetRotation(T X, T Y, T Z)
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

				Cells[0] = (cp * cy);
				Cells[1] = (cp * sy);
				Cells[2] = (-sp);

				const T srsp = sr * sp;
				const T crsp = cr * sp;

				Cells[4] = (srsp * cy - cr * sy);
				Cells[5] = (srsp * sy + cr * cy);
				Cells[6] = (sr * cp);

				Cells[8] = (crsp * cy + sr * sy);
				Cells[9] = (crsp * sy - sr * cy);
				Cells[10] = (cr * cp);

				return *this;
			}

			Vector3<T> GetRotation(void) const
			{
				const Matrix3<T>& mat = *this;
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

			Matrix3<T>& SetScale(const Vector3<T>& Scale)
			{
				SetScale(Scale.X, Scale.Y, Scale.Z);

				return *this;
			}

			Matrix3<T>& SetScale(T X, T Y, T Z)
			{
				Cells[0] = X;
				Cells[5] = Y;
				Cells[10] = Z;

				return *this;
			}

			Matrix3<T>& SetScale(T Scalar)
			{
				Cells[0] = Cells[5] = Cells[10] = Scalar;

				return *this;
			}

			Matrix3<T> GetScale(void) const
			{
				// See http://www.robertblum.com/articles/2005/02/14/decomposing-matrices

				if (Mathematics::IsZero(Cells[1]) && Mathematics::IsZero(Cells[2]) &&
					Mathematics::IsZero(Cells[4]) && Mathematics::IsZero(Cells[6]) &&
					Mathematics::IsZero(Cells[8]) && Mathematics::IsZero(Cells[9]))
					return Vector3<T>(Cells[0], Cells[5], Cells[10]);

				// We have to do the full calculation.
				return Vector3<T>(
					Mathematics::SquareRoot(Cells[0] * Cells[0] + Cells[1] * Cells[1] + Cells[2] * Cells[2]),
					Mathematics::SquareRoot(Cells[4] * Cells[4] + Cells[5] * Cells[5] + Cells[6] * Cells[6]),
					Mathematics::SquareRoot(Cells[8] * Cells[8] + Cells[9] * Cells[9] + Cells[10] * Cells[10]));
			}

			Matrix3<T> GetRight(void) const
			{
				return Vector3<T>(Cells[0], Cells[4], Cells[8]).GetNormalized();
			}

			Matrix3<T> GetUp(void) const
			{
				return Vector3<T>(Cells[1], Cells[5], Cells[9]).GetNormalized();
			}

			Matrix3<T> GetForward(void) const
			{
				return Vector3<T>(Cells[2], Cells[6], Cells[10]).GetNormalized()
#ifdef LEFT_HAND_MATRIX
					* -1
#endif
					;
			}

			Matrix3<T>& SetRotationCenter(const Vector3<T>& Center, const Vector3<T>& Translate)
			{
				Cells[12] = -Cells[0] * Center.X - Cells[4] * Center.Y - Cells[8] * Center.Z + (Center.X - Translate.X);
				Cells[13] = -Cells[1] * Center.X - Cells[5] * Center.Y - Cells[9] * Center.Z + (Center.Y - Translate.Y);
				Cells[14] = -Cells[2] * Center.X - Cells[6] * Center.Y - Cells[10] * Center.Z + (Center.Z - Translate.Z);
				Cells[15] = 1.0F;
			}

			Matrix3<T>& MakeIdentity(void)
			{
				PlatformMemory::Set<T>(Cells, 0, 16);

				Cells[0] = Cells[5] = Cells[10] = Cells[15] = 1;

				return *this;
			}

			Matrix3<T>& MakePerspectiveProjectionMatrix(T FieldOfView, T AspectRatio, T NearClipDistance, T FarClipDistance)
			{
				Assert(AspectRatio != 0.0F, "AspectRatio must be non-zero, devide by zero will happen");
				Assert(NearClipDistance != FarClipDistance, "NearClipDistance and FarClipDistance cannot equals, devide by zero will happen");

				const T h = Mathematics::Reciprocal(tan((FieldOfView * Mathematics::DEGREES_TO_RADIANS) * 0.5F));
				const T w = h / AspectRatio;

#ifdef LEFT_HAND_MATRIX
				Cells[0] = w;
				Cells[1] = 0.0F;
				Cells[2] = 0.0F;
				Cells[3] = 0.0F;

				Cells[4] = 0.0F;
				Cells[5] = h;
				Cells[6] = 0.0F;
				Cells[7] = 0.0F;

				Cells[8] = 0.0F;
				Cells[9] = 0.0F;
				Cells[10] = FarClipDistance / (FarClipDistance - NearClipDistance);
				Cells[11] = 1.0F;

				Cells[12] = 0.0F;
				Cells[13] = 0.0F;
				Cells[14] = -NearClipDistance * FarClipDistance / (FarClipDistance - NearClipDistance);
				Cells[15] = 0.0F;
#else
				Cells[0] = w;
				Cells[1] = 0.0F;
				Cells[2] = 0.0F;
				Cells[3] = 0.0F;

				Cells[4] = 0.0F;
				Cells[5] = h;
				Cells[6] = 0.0F;
				Cells[7] = 0.0F;

				Cells[8] = 0.0F;
				Cells[9] = 0.0F;
				Cells[10] = FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
				//Cells[10] = FarClipDistance + NearClipDistance / (NearClipDistance - FarClipDistance); // OpenGL version
				Cells[11] = -1.0F;

				Cells[12] = 0.0F;
				Cells[13] = 0.0F;
				Cells[14] = NearClipDistance * FarClipDistance / (NearClipDistance - FarClipDistance); // DirectX version
				//Cells[14] = 2.0F * NearClipDistance * FarClipDistance/ (NearClipDistance - FarClipDistance); // OpenGL version
				Cells[15] = 0.0F;
#endif

				return *this;
			}

			Matrix3<T>& MakeOrthographicProjectionMatrix(T Width, T Height, T NearClipDistance, T FarClipDistance)
			{
				Assert(Width != 0, "Width must be non-zero, devide by zero will happen");
				Assert(Height != 0, "Height must be non-zero, devide by zero will happen");
				Assert(NearClipDistance != FarClipDistance, "NearClipDistance and FarClipDistance cannot equals, devide by zero will happen");

#ifdef LEFT_HAND_MATRIX
				Cells[0] = 2.0F / Width;
				Cells[1] = 0.0F;
				Cells[2] = 0.0F;
				Cells[3] = 0.0F;

				Cells[4] = 0.0F;
				Cells[5] = 2.0F / Height;
				Cells[6] = 0.0F;
				Cells[7] = 0.0F;

				Cells[8] = 0.0F;
				Cells[9] = 0.0F;
				Cells[10] = 1.0F / (FarClipDistance - NearClipDistance);
				Cells[11] = 0.0F;

				Cells[12] = 0.0F;
				Cells[13] = 0.0F;
				Cells[14] = NearClipDistance / (NearClipDistance - FarClipDistance);
				Cells[15] = 1.0F;
#else
				Cells[0] = 2.0F / Width;
				Cells[1] = 0.0F;
				Cells[2] = 0.0F;
				Cells[3] = 0.0F;

				Cells[4] = 0.0F;
				Cells[5] = 2.0F / Height;
				Cells[6] = 0.0F;
				Cells[7] = 0.0F;

				Cells[8] = 0.0F;
				Cells[9] = 0.0F;
				Cells[10] = 1.0F / (NearClipDistance - FarClipDistance);
				Cells[11] = 0.0F;

				Cells[12] = 0.0F;
				Cells[13] = 0.0F;
				Cells[14] = NearClipDistance / (NearClipDistance - FarClipDistance);
				Cells[15] = 1.0F;
#endif

				return *this;
			}

			void TransformVector(Vector3<T>& Vector) const
			{
				Vector.X = Vector.X * Cells[0] + Vector.Y * Cells[4] + Vector.Z * Cells[8] + Cells[12];
				Vector.Y = Vector.X * Cells[1] + Vector.Y * Cells[5] + Vector.Z * Cells[9] + Cells[13];
				Vector.Z = Vector.X * Cells[2] + Vector.Y * Cells[6] + Vector.Z * Cells[10] + Cells[14];
			}

			void TranslateVector(Vector3<T>& Vector) const
			{
				Vector.X += Cells[12];
				Vector.Y += Cells[13];
				Vector.Z += Cells[14];
			}

			void RotateVector(Vector3<T>& Vector) const
			{
				Vector3<T> tmp(Vector);
				Vector.X = tmp.X * Cells[0] + tmp.Y * Cells[4] + tmp.Z * Cells[8];
				Vector.Y = tmp.X * Cells[1] + tmp.Y * Cells[5] + tmp.Z * Cells[9];
				Vector.Z = tmp.X * Cells[2] + tmp.Y * Cells[6] + tmp.Z * Cells[10];
			}

			bool IsIdentity(void) const
			{
				if (!Mathematics::EqualCheck<T>(Cells[12], 0) || !Mathematics::EqualCheck<T>(Cells[13], 0) || !Mathematics::EqualCheck<T>(Cells[14], 0) || !Mathematics::EqualCheck<T>(Cells[15], 1))
					return false;

				if (!Mathematics::EqualCheck<T>(Cells[0], 1) || !Mathematics::EqualCheck<T>(Cells[1], 0) || !Mathematics::EqualCheck<T>(Cells[2], 0) || !Mathematics::EqualCheck<T>(Cells[3], 0))
					return false;

				if (!Mathematics::EqualCheck<T>(Cells[4], 0) || !Mathematics::EqualCheck<T>(Cells[5], 1) || !Mathematics::EqualCheck<T>(Cells[6], 0) || !Mathematics::EqualCheck<T>(Cells[7], 0))
					return false;

				if (!Mathematics::EqualCheck<T>(Cells[8], 0) || !Mathematics::EqualCheck<T>(Cells[9], 0) || !Mathematics::EqualCheck<T>(Cells[10], 1) || !Mathematics::EqualCheck<T>(Cells[11], 0))
					return false;

				return true;
			}

			T& operator()(uint8 RowIndex, uint8 ColumnIndex)
			{
				return Cells[RowIndex * 4 + ColumnIndex];
			}

			T operator()(uint8 RowIndex, uint8 ColumnIndex) const
			{
				return Cells[RowIndex * 4 + ColumnIndex];
			}

			T& operator[](uint8 Index)
			{
				return Cells[Index];
			}

			T operator[](uint8 Index) const
			{
				return Cells[Index];
			}

			Matrix3<T>& operator=(const Matrix3<T>& Other)
			{
				if (this == &Other)
					return *this;

				PlatformMemory::Copy<T>(Other.Cells, Cells, 16);

				return *this;
			}

			Matrix3<T>& operator=(T Scalar)
			{
				for (uint8 i = 0; i < 16; ++i)
					Cells[i] = Scalar;

				return *this;
			}

			bool operator==(const Matrix3<T>& Other) const
			{
				for (uint8 i = 0; i < 16; ++i)
					if (Cells[i] != Other.Cells[i])
						return false;

				return true;
			}

			bool operator!=(const Matrix3<T>& Other) const
			{
				return !(*this == Other);
			}

			Matrix3<T> operator+(const Matrix3<T>& Other) const
			{
				Matrix3<T> temp;

				temp[0] = Cells[0] + Other[0];
				temp[1] = Cells[1] + Other[1];
				temp[2] = Cells[2] + Other[2];
				temp[3] = Cells[3] + Other[3];
				temp[4] = Cells[4] + Other[4];
				temp[5] = Cells[5] + Other[5];
				temp[6] = Cells[6] + Other[6];
				temp[7] = Cells[7] + Other[7];
				temp[8] = Cells[8] + Other[8];
				temp[9] = Cells[9] + Other[9];
				temp[10] = Cells[10] + Other[10];
				temp[11] = Cells[11] + Other[11];
				temp[12] = Cells[12] + Other[12];
				temp[13] = Cells[13] + Other[13];
				temp[14] = Cells[14] + Other[14];
				temp[15] = Cells[15] + Other[15];

				return temp;
			}

			Matrix3<T> operator-(const Matrix3<T>& Other) const
			{
				Matrix3<T> temp;

				temp[0] = Cells[0] - Other[0];
				temp[1] = Cells[1] - Other[1];
				temp[2] = Cells[2] - Other[2];
				temp[3] = Cells[3] - Other[3];
				temp[4] = Cells[4] - Other[4];
				temp[5] = Cells[5] - Other[5];
				temp[6] = Cells[6] - Other[6];
				temp[7] = Cells[7] - Other[7];
				temp[8] = Cells[8] - Other[8];
				temp[9] = Cells[9] - Other[9];
				temp[10] = Cells[10] - Other[10];
				temp[11] = Cells[11] - Other[11];
				temp[12] = Cells[12] - Other[12];
				temp[13] = Cells[13] - Other[13];
				temp[14] = Cells[14] - Other[14];
				temp[15] = Cells[15] - Other[15];

				return temp;
			}

			Matrix3<T> operator*(const Matrix3<T>& Other) const
			{
				Matrix3<T> temp;

				const T* m1 = Cells;
				const T* m2 = Other.Cells;
				T* m3 = temp.Cells;

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

			Matrix3<T> operator*(T Scalar) const
			{
				Matrix3<T> temp;

				temp[0] = Cells[0] * Scalar;
				temp[1] = Cells[1] * Scalar;
				temp[2] = Cells[2] * Scalar;
				temp[3] = Cells[3] * Scalar;
				temp[4] = Cells[4] * Scalar;
				temp[5] = Cells[5] * Scalar;
				temp[6] = Cells[6] * Scalar;
				temp[7] = Cells[7] * Scalar;
				temp[8] = Cells[8] * Scalar;
				temp[9] = Cells[9] * Scalar;
				temp[10] = Cells[10] * Scalar;
				temp[11] = Cells[11] * Scalar;
				temp[12] = Cells[12] * Scalar;
				temp[13] = Cells[13] * Scalar;
				temp[14] = Cells[14] * Scalar;
				temp[15] = Cells[15] * Scalar;

				return temp;
			}

			Matrix3<T>& operator+=(const Matrix3<T>& Other)
			{
				Cells[0] += Other[0];
				Cells[1] += Other[1];
				Cells[2] += Other[2];
				Cells[3] += Other[3];
				Cells[4] += Other[4];
				Cells[5] += Other[5];
				Cells[6] += Other[6];
				Cells[7] += Other[7];
				Cells[8] += Other[8];
				Cells[9] += Other[9];
				Cells[10] += Other[10];
				Cells[11] += Other[11];
				Cells[12] += Other[12];
				Cells[13] += Other[13];
				Cells[14] += Other[14];
				Cells[15] += Other[15];

				return *this;
			}

			Matrix3<T>& operator-=(const Matrix3<T>& Other)
			{
				Cells[0] -= Other[0];
				Cells[1] -= Other[1];
				Cells[2] -= Other[2];
				Cells[3] -= Other[3];
				Cells[4] -= Other[4];
				Cells[5] -= Other[5];
				Cells[6] -= Other[6];
				Cells[7] -= Other[7];
				Cells[8] -= Other[8];
				Cells[9] -= Other[9];
				Cells[10] -= Other[10];
				Cells[11] -= Other[11];
				Cells[12] -= Other[12];
				Cells[13] -= Other[13];
				Cells[14] -= Other[14];
				Cells[15] -= Other[15];

				return *this;
			}

			Matrix3<T>& operator*=(const Matrix3<T>& Other)
			{
				const Matrix3<T> temp(*this);

				const T* m1 = temp.Cells;
				const T* m2 = Other.Cells;

				Cells[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
				Cells[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
				Cells[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
				Cells[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

				Cells[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
				Cells[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
				Cells[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
				Cells[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

				Cells[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
				Cells[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
				Cells[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
				Cells[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

				Cells[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
				Cells[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
				Cells[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
				Cells[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

				return *this;
			}

			Matrix3<T>& operator*=(T Scalar)
			{
				Matrix3<T> temp;

				temp[0] = Cells[0] * Scalar;
				temp[1] = Cells[1] * Scalar;
				temp[2] = Cells[2] * Scalar;
				temp[3] = Cells[3] * Scalar;
				temp[4] = Cells[4] * Scalar;
				temp[5] = Cells[5] * Scalar;
				temp[6] = Cells[6] * Scalar;
				temp[7] = Cells[7] * Scalar;
				temp[8] = Cells[8] * Scalar;
				temp[9] = Cells[9] * Scalar;
				temp[10] = Cells[10] * Scalar;
				temp[11] = Cells[11] * Scalar;
				temp[12] = Cells[12] * Scalar;
				temp[13] = Cells[13] * Scalar;
				temp[14] = Cells[14] * Scalar;
				temp[15] = Cells[15] * Scalar;

				return temp;
			}

			const T* GetValue(void) const
			{
				return Cells;
			}

		public:
			T Cells[9];

		public:
			static const Matrix3<T> Zero;
			static const Matrix3<T> Identity;
		};

		template<typename T>
		const Matrix3<T> Matrix3<T>::Zero(false);

		template<typename T>
		const Matrix3<T> Matrix3<T>::Identity(true);
	}
}

#endif