#include "Stdafx.h"
#include "MA3DMatrix4.h"
#include "MA3DVector3.h"
#include "MA3DVector4.h"
#include <A3DMatrix.h>

using namespace System;

namespace APhysXCommonDNet
{

	A3DMATRIX4 Matrix4::ToA3DMATRIX( Matrix4 matrix )
	{
		A3DMATRIX4 result;
		result._11 = matrix.M11;
		result._12 = matrix.M12;
		result._13 = matrix.M13;
		result._14 = matrix.M14;
		result._21 = matrix.M21;
		result._22 = matrix.M22;
		result._23 = matrix.M23;
		result._24 = matrix.M24;
		result._31 = matrix.M31;
		result._32 = matrix.M32;
		result._33 = matrix.M33;
		result._34 = matrix.M34;
		result._41 = matrix.M41;
		result._42 = matrix.M42;
		result._43 = matrix.M43;
		result._44 = matrix.M44;

		return result;
	}

	Matrix4 Matrix4::FromA3DMATRIX( const A3DMATRIX4 &matrix )
	{
		Matrix4 result;
		result.M11 = matrix._11;
		result.M12 = matrix._12;
		result.M13 = matrix._13;
		result.M14 = matrix._14;
		result.M21 = matrix._21;
		result.M22 = matrix._22;
		result.M23 = matrix._23;
		result.M24 = matrix._24;
		result.M31 = matrix._31;
		result.M32 = matrix._32;
		result.M33 = matrix._33;
		result.M34 = matrix._34;
		result.M41 = matrix._41;
		result.M42 = matrix._42;
		result.M43 = matrix._43;
		result.M44 = matrix._44;

		return result;
	}

	float Matrix4::default::get( int row, int column )
	{
		if( row < 0 || row > 3 )
			throw gcnew ArgumentOutOfRangeException( "row", "Rows and columns for matrices run from 0 to 3, inclusive." );

		if( column < 0 || column > 3 )
			throw gcnew ArgumentOutOfRangeException( "column", "Rows and columns for matrices run from 0 to 3, inclusive." );

		int index = row * 4 + column;
		switch( index )
		{
		case 0:  return M11;
		case 1:  return M12;
		case 2:  return M13;
		case 3:  return M14;
		case 4:  return M21;
		case 5:  return M22;
		case 6:  return M23;
		case 7:  return M24;
		case 8:  return M31;
		case 9:  return M32;
		case 10: return M33;
		case 11: return M34;
		case 12: return M41;
		case 13: return M42;
		case 14: return M43;
		case 15: return M44;
		}

		return 0.0f;
	}

	void Matrix4::default::set( int row, int column, float value ) 
	{
		if( row < 0 || row > 3 )
			throw gcnew ArgumentOutOfRangeException( "row", "Rows and columns for matrices run from 0 to 3, inclusive." );

		if( column < 0 || column > 3 )
			throw gcnew ArgumentOutOfRangeException( "column", "Rows and columns for matrices run from 0 to 3, inclusive." );

		int index = row * 4 + column;
		switch( index )
		{
		case 0:  M11 = value; break;
		case 1:  M12 = value; break;
		case 2:  M13 = value; break;
		case 3:  M14 = value; break;
		case 4:  M21 = value; break;
		case 5:  M22 = value; break;
		case 6:  M23 = value; break;
		case 7:  M24 = value; break;
		case 8:  M31 = value; break;
		case 9:  M32 = value; break;
		case 10: M33 = value; break;
		case 11: M34 = value; break;
		case 12: M41 = value; break;
		case 13: M42 = value; break;
		case 14: M43 = value; break;
		case 15: M44 = value; break;
		}
	}	

	Vector4 Matrix4::Rows::get( int row )
	{
		return Vector4( default[row, 0], default[row, 1], default[row, 2], default[row, 3] );
	}

	void Matrix4::Rows::set( int row, Vector4 value )
	{
		default[row, 0] = value.X;
		default[row, 1] = value.Y;
		default[row, 2] = value.Z;
		default[row, 3] = value.W;
	}

	Vector4 Matrix4::Columns::get( int column )
	{
		return Vector4( default[0, column], default[1, column], default[2, column], default[3, column] );
	}

	void Matrix4::Columns::set( int column, Vector4 value )
	{
		default[0, column] = value.X;
		default[1, column] = value.Y;
		default[2, column] = value.Z;
		default[3, column] = value.W;
	}

	Matrix4 Matrix4::Identity::get()
	{
		Matrix4 result;
		result.M11 = 1.0f;
		result.M22 = 1.0f;
		result.M33 = 1.0f;
		result.M44 = 1.0f;

		return result;
	}

	bool Matrix4::IsIdentity::get()
	{
		if( M11 != 1.0f|| M22 != 1.0f || M33 != 1.0f || M44 != 1.0f )
			return false;

		if( M12 != 0.0f || M13 != 0.0f || M14 != 0.0f ||
			M21 != 0.0f || M23 != 0.0f || M24 != 0.0f ||
			M31 != 0.0f || M32 != 0.0f || M34 != 0.0f ||
			M41 != 0.0f || M42 != 0.0f || M43 != 0.0f )
			return false;

		return true;
	}

	array<float>^ Matrix4::ToArray()
	{
		array<float>^ result = gcnew array<float>( 16 );
		result[0] = M11;
		result[1] = M12;
		result[2] = M13;
		result[3] = M14;
		result[4] = M21;
		result[5] = M22;
		result[6] = M23;
		result[7] = M24;
		result[8] = M31;
		result[9] = M32;
		result[10] = M33;
		result[11] = M34;
		result[12] = M41;
		result[13] = M42;
		result[14] = M43;
		result[15] = M44;

		return result;
	}

	String^ Matrix4::ToString()
	{
		return String::Format( CultureInfo::CurrentCulture, "[[M11:{0} M12:{1} M13:{2} M14:{3}] [M21:{4} M22:{5} M23:{6} M24:{7}] [M31:{8} M32:{9} M33:{10} M34:{11}] [M41:{12} M42:{13} M43:{14} M44:{15}]]",
			M11.ToString(CultureInfo::CurrentCulture), M12.ToString(CultureInfo::CurrentCulture), M13.ToString(CultureInfo::CurrentCulture), M14.ToString(CultureInfo::CurrentCulture),
			M21.ToString(CultureInfo::CurrentCulture), M22.ToString(CultureInfo::CurrentCulture), M23.ToString(CultureInfo::CurrentCulture), M24.ToString(CultureInfo::CurrentCulture),
			M31.ToString(CultureInfo::CurrentCulture), M32.ToString(CultureInfo::CurrentCulture), M33.ToString(CultureInfo::CurrentCulture), M34.ToString(CultureInfo::CurrentCulture),
			M41.ToString(CultureInfo::CurrentCulture), M42.ToString(CultureInfo::CurrentCulture), M43.ToString(CultureInfo::CurrentCulture), M44.ToString(CultureInfo::CurrentCulture) );
	}

	int Matrix4::GetHashCode()
	{
		return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() + M14.GetHashCode() +
			M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() + M24.GetHashCode() +
			M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode() + M34.GetHashCode() +
			M41.GetHashCode() + M42.GetHashCode() + M43.GetHashCode() + M44.GetHashCode();
	}

	bool Matrix4::Equals( Object^ value )
	{
		if( value == nullptr )
			return false;

		if( value->GetType() != GetType() )
			return false;

		return Equals( safe_cast<Matrix4>( value ) );
	}

	bool Matrix4::Equals( Matrix4 value )
	{
		return ( M11 == value.M11 && M12 == value.M12 && M13 == value.M13 && M14 == value.M14 &&
			M21 == value.M21 && M22 == value.M22 && M23 == value.M23 && M24 == value.M24 &&
			M31 == value.M31 && M32 == value.M32 && M33 == value.M33 && M34 == value.M34 &&
			M41 == value.M41 && M42 == value.M42 && M43 == value.M43 && M44 == value.M44 );
	}

	bool Matrix4::Equals( Matrix4% value1, Matrix4% value2 )
	{
		return ( value1.M11 == value2.M11 && value1.M12 == value2.M12 && value1.M13 == value2.M13 && value1.M14 == value2.M14 &&
			value1.M21 == value2.M21 && value1.M22 == value2.M22 && value1.M23 == value2.M23 && value1.M24 == value2.M24 &&
			value1.M31 == value2.M31 && value1.M32 == value2.M32 && value1.M33 == value2.M33 && value1.M34 == value2.M34 &&
			value1.M41 == value2.M41 && value1.M42 == value2.M42 && value1.M43 == value2.M43 && value1.M44 == value2.M44 );
	}

	Vector3 Matrix4::operator * (Vector3 v, Matrix4 mat)
	{
		Vector4 vec4(v, 1);
		Vector4 result = (vec4 * mat);
		return Vector3(result.X, result.Y, result.Z);
	}
	Vector3 Matrix4::operator * (Matrix4 mat, Vector3 v)
	{
		return v * mat;
	}
	Vector4 Matrix4::operator * (Vector4 v, Matrix4 mat)
	{
		Vector4 result;
		result.X = v.X * mat[0, 0 ] + v.Y * mat[1, 0] + v.Z * mat[2, 0] + v.W * mat[3, 0];
		result.Y = v.X * mat[0, 1 ] + v.Y * mat[1, 1] + v.Z * mat[2, 1] + v.W * mat[3, 1];
		result.Z = v.X * mat[0, 2 ] + v.Y * mat[1, 2] + v.Z * mat[2, 2] + v.W * mat[3, 2];
		result.W = v.X * mat[0, 3 ] + v.Y * mat[1, 3] + v.Z * mat[2, 3] + v.W * mat[3, 3];
		return result;
	}
	Vector4 Matrix4::operator * (Matrix4 mat, Vector4 v)
	{
		return v * mat;
	}
	Matrix4 Matrix4::operator * (Matrix4 mat1, Matrix4 mat2)
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float tmp = 0;
				for (int k = 0; k < 4; k++)
				{
					tmp += mat1[i, k] * mat2[k, j];
				}
				mat[i, j] = tmp;
			}
		}
		return mat;
	}

	//	Scale matrix
	Matrix4 Matrix4::operator * (Matrix4 mat, float s)
	{
		Matrix4 result = mat;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++ )
			{
				result[i, j] = mat[i, j] * s; 
			}
		}
		return result;
	}

	void Matrix4::Clear()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j <4; j++)
			{
				this->default[i, j] = 0;
			}
		}
	}

	void Matrix4::Transpose()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < i; j++)
			{
				float tmp = this->default[i, j];
				this->default[i, j] = this->default[j, i];
				this->default[j, i] = tmp;
			}
		}
	}

	Matrix4 Matrix4::GetTranspose()
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				mat[j, i] = this->default[i, j];
			}
		}
		return mat;
	}

	Matrix4 Matrix4::GetInverseTM()
	{
		Matrix4 mat;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				mat[i, j] = this->default[j, i];
			}
		}
		mat[0, 3] = 0, mat[1, 3] = 0, mat[2, 3] = 0;
		mat[3, 3] = 1;
		Vector3 p(M41, M42, M43);
		Vector3 tmp(M11, M12, M13);
		mat[3, 0] = -Vector3::DotProduct(p, tmp);
		tmp.X = M21, tmp.Y = M22, tmp.Z = M23;
		mat[3, 1] = -Vector3::DotProduct(p, tmp);
		tmp.X = M31, tmp.Y = M32, tmp.Z = M33;
		mat[3, 2] = -Vector3::DotProduct(p, tmp);
		return mat;
	}

	//	+ operator
	Matrix4 Matrix4::operator + (Matrix4 mat1, Matrix4 mat2)
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				mat[i, j] = mat1[i, j] + mat2[i, j];
			}
		}
		return mat;
	}
	//	- operator
	Matrix4 Matrix4::operator - (Matrix4 mat1, Matrix4 mat2)
	{
		Matrix4 mat;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				mat[i, j] = mat1[i, j] - mat2[i, j];
			}
		}
		return mat;
	}

	void Matrix4::Translate(float x, float y, float z)
	{
		(*this) = Identity;
		M41 = x;
		M42 = y;
		M43 = z;
	}
	void Matrix4::RotateX(float fRad)
	{
		(*this) = Identity;
		float costh = cosf(fRad);
		float sinth = sinf(fRad);
		M22 = costh;
		M23 =sinth;
		M32 = -sinth;
		M33 = costh;
	}
	void Matrix4::RotateY(float fRad)
	{
		(*this) = Identity;
		float costh = cosf(fRad);
		float sinth = sinf(fRad);
		M11 = costh;
		M13 = -sinth;
		M31 = sinth;
		M33 = costh;
	}
	void Matrix4::RotateZ(float fRad)
	{
		(*this) = Identity;
		float costh = cosf(fRad);
		float sinth = sinf(fRad);
		M11 = costh;
		M12 = sinth;
		M21 = -sinth;
		M22 = costh;
	}
	void Matrix4::RotateAxis(Vector3 v, float fRad)
	{
		v.Normalize();
		(*this) = Identity;
		float costh = cosf(fRad);
		float sinth = sinf(fRad);
		float versth = 1 - costh;
		M11 = v.X * v.X * versth + costh;          M12 = v.X * v.Y * versth + v.Z * sinth;  M13 = v.X * v.Z * versth - v.Y * sinth;
		M21 = v.Y * v.X * versth - v.Z * sinth;    M22 = v.Y * v.Y * versth + costh;        M23 = v.Y * v.Z * versth + v.X * sinth;
		M31 = v.Z * v.X * versth + v.Y * sinth;    M32 = v.Z * v.Y * versth - v.X * sinth;  M33 = v.Z * v.Z * versth + costh;
	}
	void Matrix4::RotateAxis(Vector3 vPos, Vector3 vAxis, float fRad)
	{
		(*this) = Identity;
		Matrix4 mat;
		mat.Translate(-vPos.X, -vPos.Y, -vPos.Z);
		RotateAxis(vAxis, fRad);
		(*this) = mat * (*this);	
		mat.Translate(vPos.X, vPos.Y, vPos.Z);
		(*this) = (*this) * mat;
	}
	void Matrix4::Scale(float sx, float sy, float sz)
	{
		(*this) = Identity;
		M11 = sx;
		M22 = sy;
		M33 = sz;
	}
}