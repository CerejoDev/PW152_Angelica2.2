#pragma once

#using <System.dll>
#using <mscorlib.dll>
#include <A3DMatrix.h>


using System::Globalization::CultureInfo;
using System::Runtime::InteropServices::OutAttribute;

class A3DMATRIX4;

namespace APhysXCommonDNet
{

	value class Vector3;
	value class Vector4;

	[System::Serializable]
	[System::Runtime::InteropServices::StructLayout( System::Runtime::InteropServices::LayoutKind::Sequential, Pack = 4 )]
	public value class Matrix4 : System::IEquatable<Matrix4>
	{
	public:
		static A3DMATRIX4 ToA3DMATRIX(Matrix4 matrix);
		static Matrix4 FromA3DMATRIX(const A3DMATRIX4& matrix);

	public:

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the first row and first column. 
		/// </summary>
		float M11;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the first row and second column. 
		/// </summary>
		float M12;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the first row and third column. 
		/// </summary>
		float M13;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the first row and fourth column. 
		/// </summary>
		float M14;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the second row and first column. 
		/// </summary>
		float M21;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the second row and second column. 
		/// </summary>
		float M22;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the second row and third column. 
		/// </summary>
		float M23;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the second row and fourth column. 
		/// </summary>
		float M24;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the third row and first column. 
		/// </summary>
		float M31;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the third row and second column. 
		/// </summary>
		float M32;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the third row and third column. 
		/// </summary>
		float M33;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the third row and fourth column. 
		/// </summary>
		float M34;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the fourth row and first column. 
		/// </summary>
		float M41;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the fourth row and second column. 
		/// </summary>
		float M42;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the fourth row and third column. 
		/// </summary>
		float M43;

		/// <summary>
		/// Gets or sets the element of the matrix that exists in the fourth row and fourth column. 
		/// </summary>
		float M44;

		[System::ComponentModel::Browsable(false)]
		property float default[int, int]
		{
			float get( int row, int column );
			void set( int row, int column, float value );
		}

		[System::ComponentModel::Browsable(false)]
		property Vector4 Rows[int]
		{
			Vector4 get( int row );
			void set( int row, Vector4 value );
		}

		[System::ComponentModel::Browsable(false)]
		property Vector4 Columns[int]
		{
			Vector4 get( int column );
			void set( int column, Vector4 value );
		}

		/// <summary>
		/// Gets a <see cref="Matrix4"/> that represents an identity matrix.
		/// </summary>
		static property Matrix4 Identity { Matrix4 get(); }

		/// <summary>
		/// Gets a value indicating whether this instance is an identity matrix.
		/// </summary>
		[System::ComponentModel::Browsable(false)]
		property bool IsIdentity { bool get(); }

		array<float>^ ToArray();

		/// <summary>
		/// Converts the value of the object to its equivalent string representation.
		/// </summary>
		/// <returns>The string representation of the value of this instance.</returns>
		virtual System::String^ ToString() override;

		/// <summary>
		/// Returns the hash code for this instance.
		/// </summary>
		/// <returns>A 32-bit signed integer hash code.</returns>
		virtual int GetHashCode() override;

		/// <summary>
		/// Returns a value that indicates whether the current instance is equal to a specified object. 
		/// </summary>
		/// <param name="obj">Object to make the comparison with.</param>
		/// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
		virtual bool Equals( System::Object^ obj ) override;

		/// <summary>
		/// Returns a value that indicates whether the current instance is equal to the specified object. 
		/// </summary>
		/// <param name="other">Object to make the comparison with.</param>
		/// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
		virtual bool Equals( Matrix4 other );

		/// <summary>
		/// Determines whether the specified object instances are considered equal. 
		/// </summary>
		/// <param name="value1"></param>
		/// <param name="value2"></param>
		/// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
		/// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
		static bool Equals( Matrix4% value1, Matrix4% value2 );

		//	+ operator
		static Matrix4 operator + (Matrix4 mat1, Matrix4 mat2);
		//	- operator
		static Matrix4 operator - (Matrix4 mat1, Matrix4 mat2);

		//	* operator
		static Vector3 operator * (Vector3 v, Matrix4 mat);
		static Vector3 operator * (Matrix4 mat, Vector3 v);
		static Vector4 operator * (Vector4 v, Matrix4 mat);
		static Vector4 operator * (Matrix4 mat, Vector4 v);
		static Matrix4 operator * (Matrix4 mat1, Matrix4 mat2);

		//	Scale matrix (矩阵全部元素乘除因子)
		static Matrix4 operator * (Matrix4 mat, float s);
		static Matrix4 operator * (float s, Matrix4 mat) { return mat * s; }
		static Matrix4 operator / (Matrix4 mat, float s) { return mat * (1.0f / s); }

		//	Clear all elements to zero
		void Clear();
		//	Transpose matrix
		void Transpose();
		//	Get transpose matrix of this matrix
		Matrix4 GetTranspose();
		//	InverseTM and GetInverseTM can only be used on transform matrix. A transform matrix is a
		//	translation, rotation or scale matrix, it also can be a matrix combined by these
		//	three type matrices. For calcuating an arbitrary matrix's inverse, use Inverse and GetInverse.
		void InverseTM() { *this = GetInverseTM(); }
		Matrix4 GetInverseTM();


		void Translate(float x, float y, float z);
		void RotateX(float fRad);
		void RotateY(float fRad);
		void RotateZ(float fRad);
		void RotateAxis(Vector3 v, float fRad);
		void RotateAxis(Vector3 vPos, Vector3 vAxis, float fRad);
		void Scale(float sx, float sy, float sz);
	};

}