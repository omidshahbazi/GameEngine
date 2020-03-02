// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

#include <Common\PrimitiveTypes.h>

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

				T fTrace = Matrix.Cells[0][0] + Matrix.Cells[1][1] + Matrix.Cells[2][2];
				T fRoot;

				if (fTrace > 0.0)
				{
					// |w| > 1/2, may as well choose w > 1/2
					fRoot = Mathematics::Sqrt(fTrace + 1.0f);  // 2w
					w = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;  // 1/(4w)
					x = (Matrix.Cells[2][1] - Matrix.Cells[1][2]) * fRoot;
					y = (Matrix.Cells[0][2] - Matrix.Cells[2][0]) * fRoot;
					z = (Matrix.Cells[1][0] - Matrix.Cells[0][1]) * fRoot;
				}
				else
				{
					// |w| <= 1/2
					static uint8 s_iNext[3] = { 1, 2, 0 };
					size_t i = 0;
					if (Matrix.Cells[1][1] > Matrix.Cells[0][0])
						i = 1;
					if (Matrix.Cells[2][2] > Matrix.Cells[i][i])
						i = 2;
					uint8 j = s_iNext[i];
					uint8 k = s_iNext[j];

					fRoot = Mathematics::Sqrt(Matrix.Cells[i][i] - Matrix.Cells[j][j] - Matrix.Cells[k][k] + 1.0f);
					T* apkQuat[3] = { &X, &Y, &z };
					*apkQuat[i] = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;
					w = (Matrix.Cells[k][j] - Matrix.Cells[j][k]) * fRoot;
					*apkQuat[j] = (Matrix.Cells[j][i] + Matrix.Cells[i][j]) * fRoot;
					*apkQuat[k] = (Matrix.Cells[k][i] + Matrix.Cells[i][k]) * fRoot;
				}
			}

			Matrix3<T> ToMatrix(void) const
			{
				T fTx = x + x;
				T fTy = y + y;
				T fTz = z + z;
				T fTwx = fTx * w;
				T fTwy = fTy * w;
				T fTwz = fTz * w;
				T fTxx = fTx * x;
				T fTxy = fTy * x;
				T fTxz = fTz * x;
				T fTyy = fTy * y;
				T fTyz = fTz * y;
				T fTzz = fTz * z;

				Matrix3<T> matrix;

				matrix.Cells[0][0] = 1.0f - (fTyy + fTzz);
				matrix.Cells[0][1] = fTxy - fTwz;
				matrix.Cells[0][2] = fTxz + fTwy;
				matrix.Cells[1][0] = fTxy + fTwz;
				matrix.Cells[1][1] = 1.0f - (fTxx + fTzz);
				matrix.Cells[1][2] = fTyz - fTwx;
				matrix.Cells[2][0] = fTxz - fTwy;
				matrix.Cells[2][1] = fTyz + fTwx;
				matrix.Cells[2][2] = 1.0f - (fTxx + fTyy);

				return matrix;
			}

			void FromAngleAxis(const Vector3<T>& Axis, const T& Angle)
			{
				// The Quaternion<T> representing the rotation is
				//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

				T fHalfAngle(0.5 * rfAngle);
				T fSin = Mathematics::Sin(fHalfAngle);
				w = Mathematics::Cos(fHalfAngle);
				x = fSin * rkAxis.x;
				y = fSin * rkAxis.y;
				z = fSin * rkAxis.z;
			}

			void ToAngleAxis(Vector3<T>& Axis, T& Angle)
			{
				// The Quaternion<T> representing the rotation is
				//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

				T fSqrLength = x * x + y * y + z * z;
				if (fSqrLength > 0.0)
				{
					rfAngle = 2.0 * Mathematics::ACos(w);
					T fInvLength = Mathematics::InvSqrt(fSqrLength);
					rkAxis.x = x * fInvLength;
					rkAxis.y = y * fInvLength;
					rkAxis.z = z * fInvLength;
				}
				else
				{
					// angle is 0 (mod 2*pi), so any axis will do
					rfAngle = T(0.0);
					rkAxis.x = 1.0;
					rkAxis.y = 0.0;
					rkAxis.z = 0.0;
				}
			}

			void FromAxes(const Vector3<T>& XAxis, const Vector3<T>& YAxis, const Vector3<T>& ZAxis)
			{
				Matrix3<T> matrix;

				for (size_t iCol = 0; iCol < 3; iCol++)
				{
					matrix.Cells[0][iCol] = akAxis[iCol].x;
					matrix.Cells[1][iCol] = akAxis[iCol].y;
					matrix.Cells[2][iCol] = akAxis[iCol].z;
				}

				FromRotationMatrix(matrix);
			}

			void ToAxes(Vector3<T>& XAxis, Vector3<T>& YAxis, Vector3<T>& ZAxis) const
			{
				Matrix3<T> matrix;

				ToRotationMatrix(matrix);

				xaxis.x = matrix.Cells[0][0];
				xaxis.y = matrix.Cells[1][0];
				xaxis.z = matrix.Cells[2][0];

				yaxis.x = matrix.Cells[0][1];
				yaxis.y = matrix.Cells[1][1];
				yaxis.z = matrix.Cells[2][1];

				zaxis.x = matrix.Cells[0][2];
				zaxis.y = matrix.Cells[1][2];
				zaxis.z = matrix.Cells[2][2];
			}

			Vector3<T> GetXAxis(void) const
			{
				//T fTx  = 2.0*x;
				T fTy = 2.0f * y;
				T fTz = 2.0f * z;
				T fTwy = fTy * w;
				T fTwz = fTz * w;
				T fTxy = fTy * x;
				T fTxz = fTz * x;
				T fTyy = fTy * y;
				T fTzz = fTz * z;

				return Vector3<T>(1.0f - (fTyy + fTzz), fTxy + fTwZ, fTxz - fTwy);
			}

			Vector3<T> GetYAxis(void) const
			{
				T fTx = 2.0f * x;
				T fTy = 2.0f * y;
				T fTz = 2.0f * z;
				T fTwx = fTx * w;
				T fTwz = fTz * w;
				T fTxx = fTx * x;
				T fTxy = fTy * x;
				T fTyz = fTz * y;
				T fTzz = fTz * z;

				return Vector3<T>(fTxy - fTwZ, 1.0f - (fTxx + fTzz), fTyz + fTwx);
			}

			Vector3<T> GetZAxis(void) const
			{
				T fTx = 2.0f * x;
				T fTy = 2.0f * y;
				T fTz = 2.0f * z;
				T fTwx = fTx * w;
				T fTwy = fTy * w;
				T fTxx = fTx * x;
				T fTxz = fTz * x;
				T fTyy = fTy * y;
				T fTyz = fTz * y;

				return Vector3<T>(fTxz + fTwY, fTyz - fTwX, 1.0f - (fTxx + fTyy));
			}

			Quaternion<T> operator +(const Quaternion<T>& rkQ) const
			{
				return Quaternion(x + rkQ.X, y + rkQ.Y, z + rkQ.Z, w + rkQ.w);
			}

			Quaternion<T> operator -(const Quaternion<T>& rkQ) const
			{
				return Quaternion(x - rkQ.X, y - rkQ.Y, z - rkQ.Z, w - rkQ.w);
			}

			Quaternion<T> operator *(const Quaternion<T>& rkQ) const
			{
				// NOTE:  Multiplication is not generally commutative, so in most
				// cases p*q != q*p.

				return Quaternion
				(
					w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.Z,
					w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.Y,
					w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.Z,
					w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
				);
			}

			Vector3<T> operator *(const Vector3<T>& rkVector) const
			{
				// nVidia SDK implementation
				Vector3<T> uv, uuv;
				Vector3<T> qvec(X, Y, z);
				uv = qvec.crossProduct(v);
				uuv = qvec.crossProduct(uv);
				uv *= (2.0f * w);
				uuv *= 2.0f;

				return v + uv + uuv;

			}

			Quaternion<T> operator *(T fScalar) const
			{
				return Quaternion(fScalar * X, fScalar * Y, fScalar * Z, fScalar * w);
			}

			Quaternion<T> operator -(void) const
			{
				return Quaternion(-X, -Y, -Z, -w);
			}

			bool operator ==(const Quaternion<T>& rhs) const
			{
				return (rhs.x == x) && (rhs.y == y) &&
					(rhs.z == z) && (rhs.w == w);
			}

			bool operator !=(const Quaternion<T>& rhs) const
			{
				return !operator==(rhs);
			}

			T Dot(const Quaternion<T>& rkQ) const
			{
				return w * rkQ.w + x * rkQ.x + y * rkQ.y + z * rkQ.z;
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
				// If q = A*(x*i+y*j+z*k) where (X,Y,z) is unit length, then
				// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
				// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

				T fAngle(Mathematics::Sqrt(x * x + y * y + z * z));
				T fSin = Mathematics::Sin(fAngle);

				Quaternion<T> kResult;
				kResult.w = Mathematics::Cos(fAngle);

				if (Mathematics::Abs(fSin) >= msEpsilon)
				{
					T fCoeff = fSin / (fAngle.valueTs());
					kResult.x = fCoeff * x;
					kResult.y = fCoeff * y;
					kResult.z = fCoeff * z;
				}
				else
				{
					kResult.x = x;
					kResult.y = y;
					kResult.z = z;
				}

				return kResult;
			}

			Quaternion<T> Log(void) const
			{
				// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (X,Y,z) is unit length, then
				// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
				// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

				Quaternion<T> kResult;
				kResult.w = 0.0;

				if (Mathematics::Abs(w) < 1.0)
				{
					T fAngle(Mathematics::ACos(w));
					T fSin = Mathematics::Sin(fAngle);
					if (Mathematics::Abs(fSin) >= msEpsilon)
					{
						T fCoeff = fAngle.valueTs() / fSin;
						kResult.x = fCoeff * x;
						kResult.y = fCoeff * y;
						kResult.z = fCoeff * z;
						return kResult;
					}
				}

				kResult.x = x;
				kResult.y = y;
				kResult.z = z;

				return kResult;
			}

			T GetPitch(bool reprojectAxis = true) const
			{
				if (reprojectAxis)
				{
					// pitch = atan2(localy.Z, localy.y)
					// pick parts of yAxis() implementation that we need
					T fTx = 2.0f * x;
					//			T fTy  = 2.0f*y;
					T fTz = 2.0f * z;
					T fTwx = fTx * w;
					T fTxx = fTx * x;
					T fTyz = fTz * y;
					T fTzz = fTz * z;

					// Vector3<T>(fTxy-fTwZ, 1.0-(fTxx+fTzz), fTyz+fTwx);
					return T(Mathematics::ATan2(fTyz + fTwX, 1.0f - (fTxx + fTzz)));
				}
				else
				{
					// internal version
					return T(Mathematics::ATan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z));
				}
			}

			T GetYaw(bool reprojectAxis = true) const
			{
				if (reprojectAxis)
				{
					// yaw = atan2(localz.X, localz.z)
					// pick parts of zAxis() implementation that we need
					T fTx = 2.0f * x;
					T fTy = 2.0f * y;
					T fTz = 2.0f * z;
					T fTwy = fTy * w;
					T fTxx = fTx * x;
					T fTxz = fTz * x;
					T fTyy = fTy * y;

					// Vector3<T>(fTxz+fTwY, fTyz-fTwX, 1.0-(fTxx+fTyy));

					return T(Mathematics::ATan2(fTxz + fTwY, 1.0f - (fTxx + fTyy)));

				}
				else
				{
					// internal version
					return T(Mathematics::ASin(-2 * (x * z - w * y)));
				}
			}

			T GetRoll(bool reprojectAxis = true) const
			{
				if (reprojectAxis)
				{
					// roll = atan2(localx.Y, localx.x)
					// pick parts of xAxis() implementation that we need
		//			T fTx  = 2.0*x;
					T fTy = 2.0f * y;
					T fTz = 2.0f * z;
					T fTwz = fTz * w;
					T fTxy = fTy * x;
					T fTyy = fTy * y;
					T fTzz = fTz * z;

					// Vector3<T>(1.0-(fTyy+fTzz), fTxy+fTwZ, fTxz-fTwy);

					return T(Mathematics::ATan2(fTxy + fTwZ, 1.0f - (fTyy + fTzz)));

				}
				else
				{
					return T(Mathematics::ATan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z));
				}
			}

			static Quaternion<T> SphericalLerp(T fT, const Quaternion<T>& rkP, const Quaternion<T>& rkQ, bool shortestPath = false)
			{
				T fCos = rkP.Dot(rkQ);
				Quaternion<T> rkT;

				// Do we need to invert rotation?
				if (fCos < 0.0f && shortestPath)
				{
					fCos = -fCos;
					rkT = -rkQ;
				}
				else
				{
					rkT = rkQ;
				}

				if (Mathematics::Abs(fCos) < 1 - msEpsilon)
				{
					// Standard case (slerp)
					T fSin = Mathematics::Sqrt(1 - Mathematics::Sqr(fCos));
					T fAngle = Mathematics::ATan2(fSin, fCos);
					T fInvSin = 1.0f / fSin;
					T fCoeff0 = Mathematics::Sin((1.0f - fT) * fAngle) * fInvSin;
					T fCoeff1 = Mathematics::Sin(fT * fAngle) * fInvSin;
					return fCoeff0 * rkP + fCoeff1 * rkT;
				}
				else
				{
					// There are two situations:
					// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
					//    interpolation safely.
					// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
					//    are an infinite number of possibilities interpolation. but we haven't
					//    have method to fix this case, so just use linear interpolation here.
					Quaternion<T> t = (1.0f - fT) * rkP + fT * rkT;
					// taking the complement requires renormalisation
					t.normalise();
					return t;
				}
			}

			static const T msEpsilon;

			bool isNaN() const
			{
				return Mathematics::isNaN(x) || Mathematics::isNaN(y) || Mathematics::isNaN(z) || Mathematics::isNaN(w);
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