// CopYright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef Quaternion_H
#define Quaternion_H

#include <Common\Mathematics.h>
#include <MathContainers\Vector3.h>
#include <MathContainers\Matrix4.h>

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

			INLINE void SetFromEuler(const Vector3<T>& Euler)
			{
				T angle;

				angle = Euler.X * 0.5F;
				const T sr = Mathematics::Sin(angle);
				const T cr = Mathematics::Cos(angle);

				angle = Euler.Y * 0.5F;
				const T sp = Mathematics::Sin(angle);
				const T cp = Mathematics::Cos(angle);

				angle = Euler.Z * 0.5F;
				const T sy = Mathematics::Sin(angle);
				const T cy = Mathematics::Cos(angle);

				const T cpcy = cp * cy;
				const T spcy = sp * cy;
				const T cpsy = cp * sy;
				const T spsy = sp * sy;

				X = (T)(sr * cpcy - cr * spsy);
				Y = (T)(cr * spcy + sr * cpsy);
				Z = (T)(cr * cpsy - sr * spcy);
				W = (T)(cr * cpcy + sr * spsy);

				Normalize();
			}

			INLINE void ToEuler(Vector3<T>& Euler) const
			{
				const T sqw = W * W;
				const T sqx = X * X;
				const T sqy = Y * Y;
				const T sqz = Z * Z;
				const T test = 2 * (Y * w - X * Z);

				if (Mathematics::EqualCheck(test, 1, 0.000001F))
				{
					// heading = rotation about Z-axis
					Euler.Z = -2 * Mathematics::ATan2(X, W);

					// bank = rotation about x-axis
					Euler.X = 0;

					// attitude = rotation about y-axis
					Euler.Y = Mathematics::PI / 2.0F;
				}
				else if (Mathematics::EqualCheck(test, -1, 0.000001F))
				{
					// heading = rotation about Z-axis
					Euler.Z = 2 * Mathematics::ATan2(X, W);

					// bank = rotation about x-axis
					Euler.X = 0;

					// attitude = rotation about y-axis
					Euler.Y = Mathematics::PI / -2.0F;
				}
				else
				{
					// heading = rotation about Z-axis
					Euler.Z = Mathematics::ATan2(2 * (X * Y + Z * W), (sqx - sqy - sqz + sqw));

					// bank = rotation about x-axis
					Euler.X = Mathematics::ATan2(2 * (Y * Z + X * W), (-sqx - sqy + sqz + sqw));

					// attitude = rotation about y-axis
					Euler.Y = Mathematics::ASin(Mathematics::Clamp(test, -1, 1));
				}
			}

			INLINE void SetFromAngleAxis(T Angle, const Vector3<T>& Axis)
			{
				Quaternion<T> quat;

				const T halfAngle = 0.5F * Angle;
				const T halfAngleSin = Mathematics::Sin(halfAngle);

				W = Mathematics::Cos(halfAngle);
				X = halfAngleSin * Axis.X;
				Y = halfAngleSin * Axis.Y;
				Z = halfAngleSin * Axis.Z;

				return quat;
			}

			INLINE void ToAngleAxis(T& Angle, Vector3<T>& Axis) const
			{
				const T scale = Mathematics::SquareRoot(X * X + Y * Y + Z * Z);

				if (Mathematics::IsZero(scale) || W > 1 || W < -1)
				{
					Angle = 0;

					axis.X = 0;
					axis.Y = 1;
					axis.Z = 0;
				}
				else
				{
					const T invscale = Mathematics::Reciprocal(scale);

					Angle = 2 * Mathematics::ACos(W);

					axis.X = X * invscale;
					axis.Y = Y * invscale;
					axis.Z = Z * invscale;
				}
			}

			INLINE void ToMatrix(Matrix4<T>& Matrix) const
			{
				Quaternion<T> quat = GetNormalized();

				Matrix[0] = 1 - 2 * quat.Y * quat.Y - 2 * quat.Z * quat.Z;
				Matrix[1] = 2 * quat.X * quat.Y + 2 * quat.Z * quat.W;
				Matrix[2] = 2 * quat.X * quat.Z - 2 * quat.Y * quat.W;
				Matrix[3] = 0;

				Matrix[4] = 2 * quat.X * quat.Y - 2 * quat.Z * quat.W;
				Matrix[5] = 1 - 2 * quat.X * quat.X - 2 * quat.Z * quat.Z;
				Matrix[6] = 2 * quat.Z * quat.Y + 2 * quat.X * quat.W;
				Matrix[7] = 0;

				Matrix[8] = 2 * quat.X * quat.Z + 2 * quat.Y * quat.W;
				Matrix[9] = 2 * quat.Z * quat.Y - 2 * quat.X * quat.W;
				Matrix[10] = 1 - 2 * quat.X * quat.X - 2 * quat.Y * quat.Y;
				Matrix[11] = 0;

				Matrix[12] = 0;
				Matrix[13] = 0;
				Matrix[14] = 0;
				Matrix[15] = 1;
			}

			INLINE void Normalize(void)
			{
				*this *= Mathematics::Reciprocal(Mathematics::SquareRoot(X * X + Y * Y + Z * Z + W * W));
			}

			INLINE Quaternion<T> GetNormalized(void) const
			{
				Quaternion<T> quat = *this;
				quat.Normalize();
				return quat;
			}

			INLINE void Inverse(void)
			{
				X = -X;
				Y = -Y;
				Z = -Z;
			}

			INLINE Quaternion<T> GetInversed(void) const
			{
				Quaternion<T> quat = *this;
				quat.Inverse();
				return quat;
			}

			INLINE T DotProduct(const Quaternion<T>& Other) const
			{
				return (X * Other.X) + (Y * Other.Y) + (Z * Other.Z) + (W * Other.W);
			}

			INLINE bool operator==(const Quaternion<T>& Other) const
			{
				return ((X == Other.X) && (Y == Other.Y) && (Z == Other.Z) && (W == Other.W));
			}

			INLINE bool operator!=(const Quaternion<T>& Other) const
			{
				return !(*this == Other);
			}

			INLINE Quaternion<T>& operator=(const Quaternion<T>& Other)
			{
				X = Other.X;
				Y = Other.Y;
				Z = Other.Z;
				W = Other.W;
				return *this;
			}

			INLINE Quaternion<T> operator+(const Quaternion<T>& Other) const
			{
				return Quaternion(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
			}

			// The operatororder here is opposite of that in Matrix4<T>
			INLINE Quaternion<T> operator*(const Quaternion<T>& Other) const
			{
				Quaternion<T> quat;

				quat.W = (Other.W * W) - (Other.X * X) - (Other.Y * Y) - (Other.Z * Z);
				quat.X = (Other.W * X) + (Other.X * W) + (Other.Y * Z) - (Other.Z * Y);
				quat.Y = (Other.W * Y) + (Other.Y * W) + (Other.Z * X) - (Other.X * Z);
				quat.Z = (Other.W * Z) + (Other.Z * W) + (Other.X * Y) - (Other.Y * X);

				return quat;
			}

			INLINE Quaternion<T> operator*(T Scalar) const
			{
				return Quaternion(Scalar * X, Scalar * Y, Scalar * Z, Scalar * W);
			}

			INLINE Vector3<T> operator*(const Vector3<T>& Vector) const
			{
				Quaternion<T> quat;

				quat.X = (Vector.W * X) + (Vector.X * W) + (Vector.Y * Z) - (Vector.Z * Y);
				quat.Y = (Vector.W * Y) + (Vector.Y * W) + (Vector.Z * X) - (Vector.X * Z);
				quat.Z = (Vector.W * Z) + (Vector.Z * W) + (Vector.X * Y) - (Vector.Y * X);
				quat.W = (Vector.W * W) - (Vector.X * X) - (Vector.Y * Y) - (Vector.Z * Z);

				return tmp;
			}

			INLINE Quaternion<T>& operator*=(T Scalar)
			{
				return Quaternion(Scalar * X, Scalar * Y, Scalar * Z, Scalar * W);
			}

			INLINE Quaternion<T>& operator*=(const Quaternion<T>& Other)
			{
				return (*this = Other * (*this));
			}

			INLINE static Quaternion<T> Lerp(const Quaternion<T>& A, const Quaternion<T>& B, T Time)
			{
				const T scale = 1.0F - Time;

				return (A * scale) + (B * time);
			}

			INLINE 	static Quaternion<T> Quaternion::SphericalLerp(const Quaternion<T>& A, const Quaternion<T>& B, T Time, T Threshold = Mathematics::EPSILON)
			{
				Quaternion<T> a = A;

				T angle = a.DotProduct(B);

				if (angle < 0.0F)
				{
					a *= -1.0F;
					angle *= -1.0F;
				}

				if (angle <= (1 - Threshold))
				{
					const T theta = Mathematics::ACos(angle);
					const T invSinTheta = Mathematics::Reciprocal(Mathematics::Sin(theta));
					const T scale = Mathematics::Sin(theta * (1.0F - Time)) * invSinTheta;
					const T invScale = Mathematics::Sin(theta * Time) * invsintheta;

					return (a * scale) + (q2 * invScale);
				}
				else
					return Lerp(A, B, Time);
			}

			INLINE static Quaternion<T> FromEuler(const Vector3<T>& Euler)
			{
				Quaternion<T> quat;
				quat.SetFromEuler(Euler);
				return quat;
			}

			INLINE static Quaternion<T> FromAngleAxis(T Angle, const Vector3<T>& Axis)
			{
				Quaternion<T> quat;
				quat.SetFromAngleAxis(Angle, Axis);
				return quat;
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