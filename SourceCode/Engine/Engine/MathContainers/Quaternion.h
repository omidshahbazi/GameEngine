// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

			Quaternion(const Vector3<T>& Axis, const T& Angle)
			{
				FromAngleAxis(Angle, Axis);
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
					// |w| > 1/2, maY as well choose W > 1/2
					fRoot = Mathematics::Sqrt(fTrace + 1.0f);  // 2w
					W = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;  // 1/(4w)
					X = (Matrix[2][1] - Matrix[1][2]) * fRoot;
					Y = (Matrix[0][2] - Matrix[2][0]) * fRoot;
					Z = (Matrix[1][0] - Matrix[0][1]) * fRoot;
				}
				else
				{
					// |w| <= 1/2
					static uint8 s_iNext[3] = { 1, 2, 0 };
					size_t i = 0;
					if (Matrix[1][1] > Matrix[0][0])
						i = 1;
					if (Matrix[2][2] > Matrix[i][i])
						i = 2;
					uint8 j = s_iNext[i];
					uint8 k = s_iNext[j];

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
				T fTwX = fTX * W;
				T fTwY = fTY * W;
				T fTwZ = fTZ * W;
				T fTxX = fTX * X;
				T fTxY = fTY * X;
				T fTxZ = fTZ * X;
				T fTyY = fTY * Y;
				T fTyZ = fTZ * Y;
				T fTzZ = fTZ * Z;

				Matrix3<T> matrix;

				Matrix[0][0] = 1.0f - (fTyY + fTzZ);
				Matrix[0][1] = fTxY - fTwz;
				Matrix[0][2] = fTxZ + fTwy;
				Matrix[1][0] = fTxY + fTwz;
				Matrix[1][1] = 1.0f - (fTxX + fTzZ);
				Matrix[1][2] = fTyZ - fTwx;
				Matrix[2][0] = fTxZ - fTwy;
				Matrix[2][1] = fTyZ + fTwx;
				Matrix[2][2] = 1.0f - (fTxX + fTyy);

				return matrix;
			}

			void FromAngleAxis(const Vector3<T>& Axis, const T& Angle)
			{
				// The Quaternion<T> representing the rotation is
				//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

				T fHalfAngle(0.5 * rfAngle);
				T fSin = Mathematics::Sin(fHalfAngle);
				W = Mathematics::Cos(fHalfAngle);
				X = fSin * rkAxis.x;
				Y = fSin * rkAxis.y;
				Z = fSin * rkAxis.z;
			}

			void ToAngleAxis(Vector3<T>& Axis, T& Angle)
			{
				// The Quaternion<T> representing the rotation is
				//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

				T fSqrLength = X * X + Y * Y + Z * Z;
				if (fSqrLength > 0.0)
				{
					rfAngle = 2.0 * Mathematics::ACos(W);
					T fInvLength = 1 / Mathematics::SquareRoot(fSqrLength);
					rkAxis.X = X * fInvLength;
					rkAxis.Y = Y * fInvLength;
					rkAxis.Z = Z * fInvLength;
				}
				else
				{
					// angle is 0 (mod 2*pi), so anY axis will do
					rfAngle = T(0.0);
					rkAxis.X = 1.0;
					rkAxis.Y = 0.0;
					rkAxis.Z = 0.0;
				}
			}

			void FromAxes(const Vector3<T>& XAxis, const Vector3<T>& YAxis, const Vector3<T>& ZAxis)
			{
				Matrix3<T> matrix;

				for (size_t iCol = 0; iCol < 3; iCol++)
				{
					Matrix[0][iCol] = akAxis[iCol].x;
					Matrix[1][iCol] = akAxis[iCol].y;
					Matrix[2][iCol] = akAxis[iCol].z;
				}

				FromRotationMatrix(matrix);
			}

			void ToAxes(Vector3<T>& XAxis, Vector3<T>& YAxis, Vector3<T>& ZAxis) const
			{
				Matrix3<T> matrix;

				ToRotationMatrix(matrix);

				xaxis.X = Matrix[0][0];
				xaxis.Y = Matrix[1][0];
				xaxis.Z = Matrix[2][0];

				yaxis.X = Matrix[0][1];
				yaxis.Y = Matrix[1][1];
				yaxis.Z = Matrix[2][1];

				zaxis.X = Matrix[0][2];
				zaxis.Y = Matrix[1][2];
				zaxis.Z = Matrix[2][2];
			}

			Vector3<T> GetXAxis(void) const
			{
				//T fTX  = 2.0*x;
				T fTY = 2.0f * Y;
				T fTZ = 2.0f * Z;
				T fTwY = fTY * W;
				T fTwZ = fTZ * W;
				T fTxY = fTY * X;
				T fTxZ = fTZ * X;
				T fTyY = fTY * Y;
				T fTzZ = fTZ * Z;

				return Vector3<T>(1.0f - (fTyY + fTzZ), fTxY + fTwZ, fTxZ - fTwy);
			}

			Vector3<T> GetYAxis(void) const
			{
				T fTX = 2.0f * X;
				T fTY = 2.0f * Y;
				T fTZ = 2.0f * Z;
				T fTwX = fTX * W;
				T fTwZ = fTZ * W;
				T fTxX = fTX * X;
				T fTxY = fTY * X;
				T fTyZ = fTZ * Y;
				T fTzZ = fTZ * Z;

				return Vector3<T>(fTxY - fTwZ, 1.0f - (fTxX + fTzZ), fTyZ + fTwx);
			}

			Vector3<T> GetZAxis(void) const
			{
				T fTX = 2.0f * X;
				T fTY = 2.0f * Y;
				T fTZ = 2.0f * Z;
				T fTwX = fTX * W;
				T fTwY = fTY * W;
				T fTxX = fTX * X;
				T fTxZ = fTZ * X;
				T fTyY = fTY * Y;
				T fTyZ = fTZ * Y;

				return Vector3<T>(fTxZ + fTwY, fTyZ - fTwX, 1.0f - (fTxX + fTyy));
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
					W * Other.Z + Z * Other.W + X * Other.Y - Y * Other.x
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
				return (Other.X == x) && (Other.Y == y) &&
					(Other.Z == Z) && (Other.W == W);
			}

			bool operator !=(const Quaternion<T>& Other) const
			{
				return !operator==(Other);
			}

			T Dot(const Quaternion<T>& Other) const
			{
				return W * Other.W + X * Other.X + Y * Other.Y + Z * Other.z;
			}

			T Normalize(void) const
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
				// If q = A*(x*i+y*j+z*k) where (X,Y,Z) is unit length, then
				// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
				// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

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
				// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (X,Y,Z) is unit length, then
				// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
				// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

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
					// pitch = atan2(localy.Z, localy.y)
					// pick parts of yAxis() implementation that we need
					T fTX = 2.0f * X;
					//			T fTY  = 2.0f*y;
					T fTZ = 2.0f * Z;
					T fTwX = fTX * W;
					T fTxX = fTX * X;
					T fTyZ = fTZ * Y;
					T fTzZ = fTZ * Z;

					// Vector3<T>(fTxy-fTwZ, 1.0-(fTxx+fTzZ), fTyz+fTwx);
					return T(Mathematics::ATan2(fTyZ + fTwX, 1.0f - (fTxX + fTzZ)));
				}
				else
				{
					// internal version
					return T(Mathematics::ATan2(2 * (Y * Z + W * x), W * W - X * X - Y * Y + Z * Z));
				}
			}

			T GetYaw(bool ReprojectAxis = true) const
			{
				if (ReprojectAxis)
				{
					// yaW = atan2(localz.X, localz.Z)
					// pick parts of zAxis() implementation that we need
					T fTX = 2.0f * X;
					T fTY = 2.0f * Y;
					T fTZ = 2.0f * Z;
					T fTwY = fTY * W;
					T fTxX = fTX * X;
					T fTxZ = fTZ * X;
					T fTyY = fTY * Y;

					// Vector3<T>(fTxz+fTwY, fTyz-fTwX, 1.0-(fTxx+fTyy));

					return T(Mathematics::ATan2(fTxZ + fTwY, 1.0f - (fTxX + fTyy)));

				}
				else
				{
					// internal version
					return T(Mathematics::ASin(-2 * (X * Z - W * y)));
				}
			}

			T GetRoll(bool ReprojectAxis = true) const
			{
				if (ReprojectAxis)
				{
					// roll = atan2(localx.Y, localx.x)
					// pick parts of xAxis() implementation that we need
		//			T fTX  = 2.0*x;
					T fTY = 2.0f * Y;
					T fTZ = 2.0f * Z;
					T fTwZ = fTZ * W;
					T fTxY = fTY * X;
					T fTyY = fTY * Y;
					T fTzZ = fTZ * Z;

					// Vector3<T>(1.0-(fTyy+fTzZ), fTxy+fTwZ, fTxz-fTwy);

					return T(Mathematics::ATan2(fTxY + fTwZ, 1.0f - (fTyY + fTzZ)));

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

				// Do we need to invert rotation?
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
					// There are two situations:
					// 1. "From" and "To" are verY close (fCos ~= +1), so we can do a linear
					//    interpolation safely.
					// 2. "From" and "To" are almost inverse of each other (fCos ~= -1), there
					//    are an infinite number of possibilities interpolation. but we haven't
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