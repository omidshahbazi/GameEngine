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
		// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
		// http://www.geometrictools.com/

		// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
		// Coordinate axis rotation matrices are of the form
		//   RX =    1       0       0
		//           0     cos(t) -sin(t)
		//           0     sin(t)  cos(t)
		// where t > 0 indicates a counterclockwise rotation in the yz-plane
		//   RY =  cos(t)    0     sin(t)
		//           0       1       0
		//        -sin(t)    0     cos(t)
		// where t > 0 indicates a counterclockwise rotation in the zx-plane
		//   RZ =  cos(t) -sin(t)    0
		//         sin(t)  cos(t)    0
		//           0       0       1
		// where t > 0 indicates a counterclockwise rotation in the xy-plane.

		template<typename T>
		class Matrix3
		{
		public:
			Matrix3(void)
			{
			}

			Matrix3(const Matrix3<T>& Other)
			{
				PlatformMemory::Copy(Other.Cells, Cells, 9);
			}

			T* operator [] (uint32 Row)
			{
				return Cells[Row];
			}

			const T* operator [] (uint32 Row) const
			{
				return Cells[Row];
			}

			Vector3<T> GetColumn(uint32 Column) const
			{
				assert(iCol < 3);
				return Vector3<T>(Cells[0][j], Cells[1][j],
					Cells[2][j]);
			}

			void SetColumn(uint32 iCol, const Vector3<T>& vec)
			{
				assert(iCol < 3);
				Cells[0][j] = vec.x;
				Cells[1][j] = vec.y;
				Cells[2][j] = vec.z;

			}

			void FromAxes(const Vector3<T>& xAxis, const Vector3<T>& yAxis, const Vector3<T>& zAxis)
			{
				SetColumn(0, xAxis);
				SetColumn(1, yAxis);
				SetColumn(2, zAxis);

			}

			Matrix3<T>& operator =(const Matrix3<T>& Other)
			{
				PlatformMemory::Copy(Other.Cells, Cells, 9);

				return *this;
			}

			bool operator ==(const Matrix3<T>& Other) const
			{
				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						if (Cells[i][j] != Other.Cells[i][j])
							return false;

				return true;
			}

			bool operator != (const Matrix3<T>& Other) const
			{
				return !operator ==(Other);
			}

			Matrix3<T> operator + (const Matrix3<T>& Other) const
			{
				Matrix3<T> sum;

				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						sum.Cells[i][j] = Cells[i][j] + Other.Cells[i][j];

				return sum;
			}

			Matrix3<T> operator - (const Matrix3<T>& Other) const
			{
				Matrix3<T> diff;

				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						diff.Cells[i][j] = Cells[i][j] - Other.Cells[i][j];

				return diff;
			}

			Matrix3<T> operator * (const Matrix3<T>& Other) const
			{
				Matrix3<T> prod;

				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						prod.Cells[i][j] =
						Cells[i][0] * Other.Cells[0][j] +
						Cells[i][1] * Other.Cells[1][j] +
						Cells[i][2] * Other.Cells[2][j];

				return prod;
			}

			Vector3<T> operator * (const Vector3<T>& Vector) const
			{
				Vector3<T> prod;

				for (uint8 i = 0; i < 3; ++i)
					prod[i] =
					Cells[i][0] * Vector.X +
					Cells[i][1] * Vector.Y +
					Cells[i][2] * Vector.Z;

				return prod;
			}

			Matrix3<T> operator * (T Scalar) const
			{
				Matrix3<T> prod;

				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						prod[i][j] = Scalar * Cells[i][j];

				return prod;
			}

			Matrix3<T> operator - (void) const
			{
				Matrix3<T> neg;

				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						neg[i][j] = -Cells[i][j];

				return neg;
			}

			Matrix3<T> Transpose(void) const
			{
				Matrix3<T> transpose;

				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						transpose[i][j] = Cells[j][i];

				return transpose;
			}

			bool Inverse(Matrix3<T>& Inverse, float32 Tolerance = Mathematics::EPSILON) const
			{
				Inverse[0][0] = Cells[1][1] * Cells[2][2] - Cells[1][2] * Cells[2][1];
				Inverse[0][1] = Cells[0][2] * Cells[2][1] - Cells[0][1] * Cells[2][2];
				Inverse[0][2] = Cells[0][1] * Cells[1][2] - Cells[0][2] * Cells[1][1];
				Inverse[1][0] = Cells[1][2] * Cells[2][0] - Cells[1][0] * Cells[2][2];
				Inverse[1][1] = Cells[0][0] * Cells[2][2] - Cells[0][2] * Cells[2][0];
				Inverse[1][2] = Cells[0][2] * Cells[1][0] - Cells[0][0] * Cells[1][2];
				Inverse[2][0] = Cells[1][0] * Cells[2][1] - Cells[1][1] * Cells[2][0];
				Inverse[2][1] = Cells[0][1] * Cells[2][0] - Cells[0][0] * Cells[2][1];
				Inverse[2][2] = Cells[0][0] * Cells[1][1] - Cells[0][1] * Cells[1][0];

				T det =
					Cells[0][0] * Inverse[0][0] +
					Cells[0][1] * Inverse[1][0] +
					Cells[0][2] * Inverse[2][0];

				if (Mathematics::Absolute(det) <= Tolerance)
					return false;

				T invDet = 1.0f / det;
				for (uint8 i = 0; i < 3; ++i)
					for (uint8 j = 0; j < 3; ++j)
						rkInverse[i][j] *= invDet;

				return true;
			}

			Matrix3<T> Inverse(float32 Tolerance = Mathematics::EPSILON) const
			{
				Matrix3<T> inv = Zero;

				Inverse(inv, Tolerance);

				return inv;
			}

			T Determinant(void) const
			{
				T cofactor00 = Cells[1][1] * Cells[2][2] -
					Cells[1][2] * Cells[2][1];
				T cofactor10 = Cells[1][2] * Cells[2][0] -
					Cells[1][0] * Cells[2][2];
				T cofactor20 = Cells[1][0] * Cells[2][1] -
					Cells[1][1] * Cells[2][0];

				T det =
					Cells[0][0] * cofactor00 +
					Cells[0][1] * cofactor10 +
					Cells[0][2] * cofactor20;

				return det;
			}

			/// Note: Matrix must be orthonormal
			void ToAngleAxis(Vector3<T>& Axis, float32& Radians) const
			{
				// Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
				// The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
				// I is the identity and
				//
				//       +-        -+
				//   P = |  0 -z +y |
				//       | +z  0 -x |
				//       | -y +x  0 |
				//       +-        -+
				//
				// If A > 0, R represents a counterclockwise rotation about the axis in
				// the sense of looking from the tip of the axis vector towards the
				// origin.  Some algebra will show that
				//
				//   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
				//
				// In the event that A = pi, R-R^t = 0 which prevents us from extracting
				// the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
				// P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
				// z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
				// it does not matter which sign you choose on the square roots.

				T trace = Cells[0][0] + Cells[1][1] + Cells[2][2];
				T cos = 0.5f * (trace - 1.0f);
				Radians = Mathematics::ACos(cos);  // in [0,PI]

				if (Radians > 0.0F)
				{
					if (Radians < Mathematics::PI))
					{
						Axis.x = Cells[2][1] - Cells[1][2];
						Axis.y = Cells[0][2] - Cells[2][0];
						Axis.z = Cells[1][0] - Cells[0][1];
						Axis.Normalize();
					}
					else
					{
						// angle is PI
						float32 halfInverse;
						if (Cells[0][0] >= Cells[1][1])
						{
							// r00 >= r11
							if (Cells[0][0] >= Cells[2][2])
							{
								// r00 is maximum diagonal term
								Axis.x = 0.5f * Mathematics::SquareRoot(Cells[0][0] - Cells[1][1] - Cells[2][2] + 1.0f);
								halfInverse = 0.5f / Axis.x;
								Axis.y = halfInverse * Cells[0][1];
								Axis.z = halfInverse * Cells[0][2];
							}
							else
							{
								// r22 is maximum diagonal term
								Axis.z = 0.5f * Mathematics::SquareRoot(Cells[2][2] - Cells[0][0] - Cells[1][1] + 1.0f);
								halfInverse = 0.5f / Axis.z;
								Axis.x = halfInverse * Cells[0][2];
								Axis.y = halfInverse * Cells[1][2];
							}
						}
						else
						{
							// r11 > r00
							if (Cells[1][1] >= Cells[2][2])
							{
								// r11 is maximum diagonal term
								Axis.y = 0.5f * Mathematics::SquareRoot(Cells[1][1] - Cells[0][0] - Cells[2][2] + 1.0f);
								halfInverse = 0.5f / Axis.y;
								Axis.x = halfInverse * Cells[0][1];
								Axis.z = halfInverse * Cells[1][2];
							}
							else
							{
								// r22 is maximum diagonal term
								Axis.z = 0.5f * Mathematics::SquareRoot(Cells[2][2] - Cells[0][0] - Cells[1][1] + 1.0f);
								halfInverse = 0.5f / Axis.z;
								Axis.x = halfInverse * Cells[0][2];
								Axis.y = halfInverse * Cells[1][2];
							}
						}
					}
				}
				else
				{
					// The angle is 0 and the matrix is the identity.  Any axis will
					// work, so just use the x-axis.
					Axis.x = 1.0;
					Axis.y = 0.0;
					Axis.z = 0.0;
				}
			}

			void FromAngleAxis(const Vector3<T>& rkAxis, const float32& Radians)
			{
				T fCos = Math::Cos(fRadians);
				T fSin = Math::Sin(fRadians);
				T fOneMinusCos = 1.0f - fCos;
				T fX2 = rkAxis.x * rkAxis.x;
				T fY2 = rkAxis.y * rkAxis.y;
				T fZ2 = rkAxis.z * rkAxis.z;
				T fXYM = rkAxis.x * rkAxis.y * fOneMinusCos;
				T fXZM = rkAxis.x * rkAxis.z * fOneMinusCos;
				T fYZM = rkAxis.y * rkAxis.z * fOneMinusCos;
				T fXSin = rkAxis.x * fSin;
				T fYSin = rkAxis.y * fSin;
				T fZSin = rkAxis.z * fSin;

				Cells[0][0] = fX2 * fOneMinusCos + fCos;
				Cells[0][1] = fXYM - fZSin;
				Cells[0][2] = fXZM + fYSin;
				Cells[1][0] = fXYM + fZSin;
				Cells[1][1] = fY2 * fOneMinusCos + fCos;
				Cells[1][2] = fYZM - fXSin;
				Cells[2][0] = fXZM - fYSin;
				Cells[2][1] = fYZM + fXSin;
				Cells[2][2] = fZ2 * fOneMinusCos + fCos;
			}

			/** The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
				where yaw is rotation about the Up vector, pitch is rotation about the
				Right axis, and roll is rotation about the Direction axis. */
			bool ToEulerAnglesXYZ(float32& rfYAngle, float32& rfPAngle,
				float32& rfRAngle) const;
			bool ToEulerAnglesXZY(float32& rfYAngle, float32& rfPAngle,
				float32& rfRAngle) const;
			bool ToEulerAnglesYXZ(float32& rfYAngle, float32& rfPAngle,
				float32& rfRAngle) const;
			bool ToEulerAnglesYZX(float32& rfYAngle, float32& rfPAngle,
				float32& rfRAngle) const;
			bool ToEulerAnglesZXY(float32& rfYAngle, float32& rfPAngle,
				float32& rfRAngle) const;
			bool ToEulerAnglesZYX(float32& rfYAngle, float32& rfPAngle,
				float32& rfRAngle) const;
			void FromEulerAnglesXYZ(const float32& fYAngle, const float32& fPAngle, const float32& fRAngle);
			void FromEulerAnglesXZY(const float32& fYAngle, const float32& fPAngle, const float32& fRAngle);
			void FromEulerAnglesYXZ(const float32& fYAngle, const float32& fPAngle, const float32& fRAngle);
			void FromEulerAnglesYZX(const float32& fYAngle, const float32& fPAngle, const float32& fRAngle);
			void FromEulerAnglesZXY(const float32& fYAngle, const float32& fPAngle, const float32& fRAngle);
			void FromEulerAnglesZYX(const float32& fYAngle, const float32& fPAngle, const float32& fRAngle);
			/// Eigensolver, matrix must be symmetric
			void EigenSolveSymmetric(T afEigenvalue[3],
				Vector3<T> akEigenvector[3]) const;

			static void TensorProduct(const Vector3<T>& rkU, const Vector3<T>& rkV,
				Matrix3<T>& rkProduct);

			/** Determines if this matrix involves a scaling. */
			bool hasScale() const
			{
				// check magnitude of column vectors (==local axes)
				T t = Cells[0][0] * Cells[0][0] + Cells[1][0] * Cells[1][0] + Cells[2][0] * Cells[2][0];
				if (!Math::TEqual(t, 1.0, (T)1e-04))
					return true;
				t = Cells[0][1] * Cells[0][1] + Cells[1][1] * Cells[1][1] + Cells[2][1] * Cells[2][1];
				if (!Math::TEqual(t, 1.0, (T)1e-04))
					return true;
				t = Cells[0][2] * Cells[0][2] + Cells[1][2] * Cells[1][2] + Cells[2][2] * Cells[2][2];
				if (!Math::TEqual(t, 1.0, (T)1e-04))
					return true;

				return false;
			}

			/** Function for writing to a stream.
			*/
			_OgreExport friend std::ostream& operator  <<
				(std::ostream& o, const Matrix3<T>& mat)
			{
				o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << ", "
					<< mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", "
					<< mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
				return o;
			}

			static const T EPSILON;
			static const Matrix3<T> ZERO;
			static const Matrix3<T> IDENTITY;

		protected:
			// support for eigensolver
			void Tridiagonal(T afDiag[3], T afSubDiag[3]);
			bool QLAlgorithm(T afDiag[3], T afSubDiag[3]);

			// support for singular value decomposition
			static const unsigned int msSvdMaxIterations;
			static void Bidiagonalize(Matrix3<T>& kA, Matrix3<T>& kL,
				Matrix3<T>& kR);
			static void GolubKahanStep(Matrix3<T>& kA, Matrix3<T>& kL,
				Matrix3<T>& kR);

			// support for spectral norm
			static T MaxCubicRoot(T afCoeff[3]);

			T Cells[3][3];

			// for faster access
			friend class Matrix4;

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