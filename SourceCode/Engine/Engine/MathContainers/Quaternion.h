// CopYright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

#include <Common\Mathematics.h>
#include <MathContainers\Vector3.h>
#include <MathContainers\Matrix3.h>

namespace Engine
{
	using namespace Common;

	namespace MathContainers
	{
		template<typename T>
		class Quaternion
		{
		public:
			Quaternion(void) :
				X(0),
				Y(0),
				Z(0),
				W(1)
			{
			}

			Quaternion(T X, T Y, T Z, T W) :
				X(X),
				Y(Y),
				Z(Z),
				W(W)
			{
			}

			Quaternion(const Matrix3<T>& Matrix)
			{
				FromMatrix(Matrix);
			}

			Quaternion(const Vector3<T>& Axis, T Angle)
			{
				FromAxisAngle(Axis, Angle);
			}

			Quaternion(const Vector3<T>& XAxis, const Vector3<T>& YAxis, const Vector3<T>& ZAxis)
			{
				FromAxes(XAxis, YAxis, ZAxis);
			}

			void FromMatrix(const Matrix3<T>& Matrix)
			{
				// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
				// article "Quaternion<T> Calculus and Fast Animation".

				T fTrace = Matrix[0][0] + Matrix[1][1] + Matrix[2][2];
				T fRoot;

				if (fTrace > 0.0)
				{
					// |W| > 1/2, maY as Well choose W > 1/2
					fRoot = Mathematics::Sqrt(fTrace + 1.0f);  // 2W
					W = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;  // 1/(4W)
					X = (Matrix[2][1] - Matrix[1][2]) * fRoot;
					Y = (Matrix[0][2] - Matrix[2][0]) * fRoot;
					Z = (Matrix[1][0] - Matrix[0][1]) * fRoot;
				}
				else
				{
					// |W| <= 1/2
					static uint8 s_iNeXt[3] = { 1, 2, 0 };
					siZe_t i = 0;
					if (Matrix[1][1] > Matrix[0][0])
						i = 1;
					if (Matrix[2][2] > Matrix[i][i])
						i = 2;
					uint8 j = s_iNeXt[i];
					uint8 k = s_iNeXt[j];

					fRoot = Mathematics::SquareRoot(Matrix[i][i] - Matrix[j][j] - Matrix[k][k] + 1.0f);
					T* apkQuat[3] = { &X, &Y, &Z };
					*apkQuat[i] = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;
					W = (Matrix[k][j] - Matrix[j][k]) * fRoot;
					*apkQuat[j] = (Matrix[j][i] + Matrix[i][j]) * fRoot;
					*apkQuat[k] = (Matrix[k][i] + Matrix[i][k]) * fRoot;
				}
			}

			Matrix3<T> ToMatrix(void) const
			{
				T fTX = X + X;
				T fTY = Y + Y;
				T fTZ = Z + Z;
				T fTWX = fTX * W;
				T fTWY = fTY * W;
				T fTWZ = fTZ * W;
				T fTXX = fTX * X;
				T fTXY = fTY * X;
				T fTXZ = fTZ * X;
				T fTYY = fTY * Y;
				T fTYZ = fTZ * Y;
				T fTZZ = fTZ * Z;

				Matrix3<T> matriX;

				Matrix[0][0] = 1.0f - (fTYY + fTZZ);
				Matrix[0][1] = fTXY - fTWZ;
				Matrix[0][2] = fTXZ + fTWY;
				Matrix[1][0] = fTXY + fTWZ;
				Matrix[1][1] = 1.0f - (fTXX + fTZZ);
				Matrix[1][2] = fTYZ - fTWX;
				Matrix[2][0] = fTXZ - fTWY;
				Matrix[2][1] = fTYZ + fTWX;
				Matrix[2][2] = 1.0f - (fTXX + fTYY);

				return matriX;
			}

			void FromAxisAngle(const Vector3<T>& Axis, T Angle)
			{
				// The Quaternion<T> representing the rotation is
				//   q = cos(A/2)+sin(A/2)*(X*i+Y*j+Z*k)

				T halfAngle(0.5 * Angle);
				T fSin = Mathematics::Sin(halfAngle);
				W = Mathematics::Cos(halfAngle);
				X = fSin * Axis.X;
				Y = fSin * Axis.Y;
				Z = fSin * Axis.Z;
			}

			void ToAxisAngle(Vector3<T>& Axis, T& Angle)
			{
				// The Quaternion<T> representing the rotation is
				//   q = cos(A/2)+sin(A/2)*(X*i+Y*j+Z*k)

				T fSqrLength = X * X + Y * Y + Z * Z;
				if (fSqrLength > 0.0)
				{
					Angle = 2.0 * Mathematics::ACos(W);
					T fInvLength = 1 / Mathematics::SquareRoot(fSqrLength);
					Axis.X = X * fInvLength;
					Axis.Y = Y * fInvLength;
					Axis.Z = Z * fInvLength;
				}
				else
				{
					// angle is 0 (mod 2*pi), so anY aXis Will do
					Angle = T(0.0);
					Axis.X = 1.0;
					Axis.Y = 0.0;
					Axis.Z = 0.0;
				}
			}

			void FromAxes(const Vector3<T>& XAxis, const Vector3<T>& YAxis, const Vector3<T>& ZAxis)
			{
				Matrix3<T> matriX;

				for (siZe_t iCol = 0; iCol < 3; iCol++)
				{
					Matrix[0][iCol] = akAxis[iCol].X;
					Matrix[1][iCol] = akAxis[iCol].Y;
					Matrix[2][iCol] = akAxis[iCol].Z;
				}

				FromRotationMatrix(matriX);
			}

			void ToAxes(Vector3<T>& XAxis, Vector3<T>& YAxis, Vector3<T>& ZAxis) const
			{
				Matrix3<T> matriX;

				ToRotationMatrix(matriX);

				XaXis.X = Matrix[0][0];
				XaXis.Y = Matrix[1][0];
				XaXis.Z = Matrix[2][0];

				YaXis.X = Matrix[0][1];
				YaXis.Y = Matrix[1][1];
				YaXis.Z = Matrix[2][1];

				ZaXis.X = Matrix[0][2];
				ZaXis.Y = Matrix[1][2];
				ZaXis.Z = Matrix[2][2];
			}

			void ToRotationMatrix(Matrix3<T>& Matrix) const
			{
				T fTX = X + X;
				T fTY = Y + Y;
				T fTZ = Z + Z;
				T fTWX = fTX * W;
				T fTWY = fTY * W;
				T fTWZ = fTZ * W;
				T fTXX = fTX * X;
				T fTXY = fTY * X;
				T fTXZ = fTZ * X;
				T fTYY = fTY * Y;
				T fTYZ = fTZ * Y;
				T fTZZ = fTZ * Z;

				Matrix[0][0] = 1.0f - (fTYY + fTZZ);
				Matrix[0][1] = fTXY - fTWZ;
				Matrix[0][2] = fTXZ + fTWY;

				Matrix[1][0] = fTXY + fTWZ;
				Matrix[1][1] = 1.0f - (fTXX + fTZZ);
				Matrix[1][2] = fTYZ - fTWX;

				Matrix[2][0] = fTXZ - fTWY;
				Matrix[2][1] = fTYZ + fTWX;
				Matrix[2][2] = 1.0f - (fTXX + fTYY);
			}

			Quaternion<T> FromEuler(const Vector3<T>& Rotation)
			{
				return Quaternion<T>(Vector3F::Up, Rotation.Y) * Quaternion<T>(Vector3F::Right, Rotation.X) * Quaternion<T>(Vector3F::Forward, Rotation.Z);
			}

			Vector3<T> GetXAxis(void) const
			{
				//T fTX  = 2.0*X;
				T fTY = 2.0f * Y;
				T fTZ = 2.0f * Z;
				T fTWY = fTY * W;
				T fTWZ = fTZ * W;
				T fTXY = fTY * X;
				T fTXZ = fTZ * X;
				T fTYY = fTY * Y;
				T fTZZ = fTZ * Z;

				return Vector3<T>(1.0f - (fTYY + fTZZ), fTXY + fTWZ, fTXZ - fTWY);
			}

			Vector3<T> GetYAxis(void) const
			{
				T fTX = 2.0f * X;
				T fTY = 2.0f * Y;
				T fTZ = 2.0f * Z;
				T fTWX = fTX * W;
				T fTWZ = fTZ * W;
				T fTXX = fTX * X;
				T fTXY = fTY * X;
				T fTYZ = fTZ * Y;
				T fTZZ = fTZ * Z;

				return Vector3<T>(fTXY - fTWZ, 1.0f - (fTXX + fTZZ), fTYZ + fTWX);
			}

			Vector3<T> GetZAxis(void) const
			{
				T fTX = 2.0f * X;
				T fTY = 2.0f * Y;
				T fTZ = 2.0f * Z;
				T fTWX = fTX * W;
				T fTWY = fTY * W;
				T fTXX = fTX * X;
				T fTXZ = fTZ * X;
				T fTYY = fTY * Y;
				T fTYZ = fTZ * Y;

				return Vector3<T>(fTXZ + fTWY, fTYZ - fTWX, 1.0f - (fTXX + fTYY));
			}

			Quaternion<T> operator +(const Quaternion<T>& Other) const
			{
				return Quaternion(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
			}

			Quaternion<T> operator -(const Quaternion<T>& Other) const
			{
				return Quaternion(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
			}

			Quaternion<T> operator *(const Quaternion<T>& Other) const
			{
				// NOTE:  Multiplication is not generallY commutative, so in most
				// cases p*q != q*p.

				return Quaternion
				(
					W * Other.W - X * Other.X - Y * Other.Y - Z * Other.Z,
					W * Other.X + X * Other.W + Y * Other.Z - Z * Other.Y,
					W * Other.Y + Y * Other.W + Z * Other.X - X * Other.Z,
					W * Other.Z + Z * Other.W + X * Other.Y - Y * Other.X
				);
			}

			Vector3<T> operator *(const Vector3<T>& Other) const
			{
				// nVidia SDK implementation
				Vector3<T> uv, uuv;
				Vector3<T> qvec(X, Y, Z);
				uv = qvec.crossProduct(Other);
				uuv = qvec.crossProduct(uv);
				uv *= (2.0f * W);
				uuv *= 2.0f;

				return v + uv + uuv;
			}

			Quaternion<T> operator *(T Scalar) const
			{
				return Quaternion(Scalar * X, Scalar * Y, Scalar * Z, Scalar * W);
			}

			Quaternion<T> operator -(void) const
			{
				return Quaternion(-X, -Y, -Z, -W);
			}

			bool operator ==(const Quaternion<T>& Other) const
			{
				return (Other.X == X) && (Other.Y == Y) &&
					(Other.Z == Z) && (Other.W == W);
			}

			bool operator !=(const Quaternion<T>& Other) const
			{
				return !operator==(Other);
			}

			T Dot(const Quaternion<T>& Other) const
			{
				return W * Other.W + X * Other.X + Y * Other.Y + Z * Other.Z;
			}

			T NormaliZe(void) const
			{
				return X * X + Y * Y + Z * Z + W * W;
			}

			Quaternion<T> Inverse(void) const
			{
				return Quaternion<T>(-X, -Y, -Z, W);
			}

			Quaternion<T> UnitInverse(void) const
			{
				return Quaternion(-X, -Y, -Z, W);
			}

			Quaternion<T> Exponential(void) const
			{
				// If q = A*(X*i+Y*j+Z*k) Where (X,Y,Z) is unit length, then
				// eXp(q) = cos(A)+sin(A)*(X*i+Y*j+Z*k).  If sin(A) is near Zero,
				// use eXp(q) = cos(A)+A*(X*i+Y*j+Z*k) since A/sin(A) has limit 1.

				T fAngle(Mathematics::SquareRoot(X * X + Y * Y + Z * Z));
				T fSin = Mathematics::Sin(fAngle);

				Quaternion<T> kResult;
				kResult.W = Mathematics::Cos(fAngle);

				if (Mathematics::Absolute(fSin) >= msEpsilon)
				{
					T fCoeff = fSin / (fAngle.valueTs());
					kResult.X = fCoeff * X;
					kResult.Y = fCoeff * Y;
					kResult.Z = fCoeff * Z;
				}
				else
				{
					kResult.X = X;
					kResult.Y = Y;
					kResult.Z = Z;
				}

				return kResult;
			}

			Quaternion<T> Log(void) const
			{
				// If q = cos(A)+sin(A)*(X*i+Y*j+Z*k) Where (X,Y,Z) is unit length, then
				// log(q) = A*(X*i+Y*j+Z*k).  If sin(A) is near Zero, use log(q) =
				// sin(A)*(X*i+Y*j+Z*k) since sin(A)/A has limit 1.

				Quaternion<T> kResult;
				kResult.W = 0.0;

				if (Mathematics::Absolute(W) < 1.0)
				{
					T fAngle(Mathematics::ACos(W));
					T fSin = Mathematics::Sin(fAngle);
					if (Mathematics::Absolute(fSin) >= msEpsilon)
					{
						T fCoeff = fAngle.valueTs() / fSin;
						kResult.X = fCoeff * X;
						kResult.Y = fCoeff * Y;
						kResult.Z = fCoeff * Z;
						return kResult;
					}
				}

				kResult.X = X;
				kResult.Y = Y;
				kResult.Z = Z;

				return kResult;
			}

			T GetPitch(bool ReprojectAxis = true) const
			{
				if (ReprojectAxis)
				{
					// pitch = atan2(localY.Z, localY.Y)
					// pick parts of YAxis() implementation that We need
					T fTX = 2.0f * X;
					//			T fTY  = 2.0f*Y;
					T fTZ = 2.0f * Z;
					T fTWX = fTX * W;
					T fTXX = fTX * X;
					T fTYZ = fTZ * Y;
					T fTZZ = fTZ * Z;

					// Vector3<T>(fTXY-fTWZ, 1.0-(fTXX+fTZZ), fTYZ+fTWX);
					return T(Mathematics::ATan2(fTYZ + fTWX, 1.0f - (fTXX + fTZZ)));
				}
				else
				{
					// internal version
					return T(Mathematics::ATan2(2 * (Y * Z + W * X), W * W - X * X - Y * Y + Z * Z));
				}
			}

			T GetYaW(bool ReprojectAxis = true) const
			{
				if (ReprojectAxis)
				{
					// YaW = atan2(localZ.X, localZ.Z)
					// pick parts of ZAxis() implementation that We need
					T fTX = 2.0f * X;
					T fTY = 2.0f * Y;
					T fTZ = 2.0f * Z;
					T fTWY = fTY * W;
					T fTXX = fTX * X;
					T fTXZ = fTZ * X;
					T fTYY = fTY * Y;

					// Vector3<T>(fTXZ+fTWY, fTYZ-fTWX, 1.0-(fTXX+fTYY));

					return T(Mathematics::ATan2(fTXZ + fTWY, 1.0f - (fTXX + fTYY)));

				}
				else
				{
					// internal version
					return T(Mathematics::ASin(-2 * (X * Z - W * Y)));
				}
			}

			T GetRoll(bool ReprojectAxis = true) const
			{
				if (ReprojectAxis)
				{
					// roll = atan2(localX.Y, localX.X)
					// pick parts of XAxis() implementation that We need
		//			T fTX  = 2.0*X;
					T fTY = 2.0f * Y;
					T fTZ = 2.0f * Z;
					T fTWZ = fTZ * W;
					T fTXY = fTY * X;
					T fTYY = fTY * Y;
					T fTZZ = fTZ * Z;

					// Vector3<T>(1.0-(fTYY+fTZZ), fTXY+fTWZ, fTXZ-fTWY);

					return T(Mathematics::ATan2(fTXY + fTWZ, 1.0f - (fTYY + fTZZ)));

				}
				else
				{
					return T(Mathematics::ATan2(2 * (X * Y + W * Z), W * W + X * X - Y * Y - Z * Z));
				}
			}

			static Quaternion<T> SphericalLerp(const Quaternion<T>& From, const Quaternion<T>& To, T Time, bool ShortestPath = false)
			{
				T fCos = From.Dot(To);
				Quaternion<T> rkT;

				// Do We need to invert rotation?
				if (fCos < 0.0f && ShortestPath)
				{
					fCos = -fCos;
					rkT = -To;
				}
				else
				{
					rkT = To;
				}

				if (Mathematics::Absolute(fCos) < 1 - Mathematics::EPSILON)
				{
					// Standard case (slerp)
					T fSin = Mathematics::SquareRoot(1 - Mathematics::SquareRoot(fCos));
					T fAngle = Mathematics::ATan2(fSin, fCos);
					T fInvSin = 1.0f / fSin;
					T fCoeff0 = Mathematics::Sin((1.0f - Time) * fAngle) * fInvSin;
					T fCoeff1 = Mathematics::Sin(Time * fAngle) * fInvSin;
					return fCoeff0 * From + fCoeff1 * rkT;
				}
				else
				{
					// There are tWo situations:
					// 1. "From" and "To" are verY close (fCos ~= +1), so We can do a linear
					//    interpolation safelY.
					// 2. "From" and "To" are almost inverse of each other (fCos ~= -1), there
					//    are an infinite number of possibilities interpolation. but We haven't
					//    have method to fiX this case, so just use linear interpolation here.
					Quaternion<T> t = (1.0f - Time) * From + Time * rkT;
					// taking the complement requires renormalisation
					t.Normalise();
					return t;
				}
			}

		public:
			T X;
			T Y;
			T Z;
			T W;

			static const Quaternion<T> Zero;
			static const Quaternion<T> Identity;
		};

		template<typename T>
		const Quaternion<T>  Quaternion<T>::Zero = { 0, 0, 0, 0 };

		template<typename T>
		const Quaternion<T>  Quaternion<T>::Identity = { 0, 0, 0, 1 };
	}
}

#endif