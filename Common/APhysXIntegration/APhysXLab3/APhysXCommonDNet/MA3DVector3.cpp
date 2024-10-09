#include "Stdafx.h"
#include "MA3DVector3.h"
#include <A3DVector.h>

using namespace System;

namespace APhysXCommonDNet
{

	A3DVECTOR3 Vector3::ToA3DVECTOR3(Vector3 v)
	{
		return A3DVECTOR3(v.X, v.Y, v.Z);
	}

	Vector3 Vector3::FromA3DVECTOR3(const A3DVECTOR3& v)
	{
		return Vector3(v.x, v.y, v.z);
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="Vector3"/> class.
	/// </summary>
	/// <param name="value">The value that will be assigned to all components.</param>
	Vector3::Vector3( float value )
	{
		X = Y = Z = value;
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="Vector3"/> class.
	/// </summary>
	/// <param name="x">Initial value for the X component of the vector.</param>
	/// <param name="y">Initial value for the Y component of the vector.</param>
	/// <param name="z">Initial value for the Z component of the vector.</param>
	Vector3::Vector3( float x, float y, float z )
	{
		X = x;
		Y = y;
		Z = z;
	}

	/// <summary>
	/// Adds two vectors.
	/// </summary>
	/// <param name="left">The first vector to add.</param>
	/// <param name="right">The second vector to add.</param>
	/// <returns>The sum of the two vectors.</returns>
	Vector3 Vector3::operator + ( Vector3 left, Vector3 right )
	{
		return Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
	}

	/// <summary>
	/// Subtracts two vectors.
	/// </summary>
	/// <param name="left">The first vector to subtract.</param>
	/// <param name="right">The second vector to subtract.</param>
	/// <returns>The difference of the two vectors.</returns>
	Vector3 Vector3::operator - ( Vector3 left, Vector3 right )
	{
		return Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
	}

	/// <summary>
	/// Reverses the direction of a given vector.
	/// </summary>
	/// <param name="value">The vector to negate.</param>
	/// <returns>A vector facing in the opposite direction.</returns>
	Vector3 Vector3::operator - ( Vector3 value )
	{
		return Vector3(-value.X, -value.Y, -value.Z);
	}

	/// <summary>
	/// Scales a vector by the given value.
	/// </summary>
	/// <param name="vector">The vector to scale.</param>
	/// <param name="scale">The amount by which to scale the vector.</param>
	/// <returns>The scaled vector.</returns>
	Vector3 Vector3::operator * ( Vector3 vector, float scale )
	{
		return Vector3(vector.X * scale, vector.Y * scale, vector.Z * scale);
	}

	/// <summary>
	/// Scales a vector by the given value.
	/// </summary>
	/// <param name="vector">The vector to scale.</param>
	/// <param name="scale">The amount by which to scale the vector.</param>
	/// <returns>The scaled vector.</returns>
	Vector3 Vector3::operator * ( float scale, Vector3 vector )
	{
		return operator * (vector, scale);
	}

	/// <summary>
	/// Scales a vector by the given value.
	/// </summary>
	/// <param name="vector">The vector to scale.</param>
	/// <param name="scale">The amount by which to scale the vector.</param>
	/// <returns>The scaled vector.</returns>
	Vector3 Vector3::operator / ( Vector3 vector, float scale )
	{
		float invScale = 1.0f / scale;
		return Vector3(vector.X * invScale, vector.Y * invScale, vector.Z * invScale);
	}

	/// <summary>
	/// Tests for equality between two objects.
	/// </summary>
	/// <param name="left">The first value to compare.</param>
	/// <param name="right">The second value to compare.</param>
	/// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
	bool Vector3::operator == ( Vector3 left, Vector3 right )
	{
		return Vector3::Equals( left, right );
	}

	/// <summary>
	/// Tests for inequality between two objects.
	/// </summary>
	/// <param name="left">The first value to compare.</param>
	/// <param name="right">The second value to compare.</param>
	/// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
	bool Vector3::operator != ( Vector3 left, Vector3 right )
	{
		return !Vector3::Equals( left, right );
	}

	float Vector3::DotProduct(Vector3 v1, Vector3 v2) 
	{ 
		return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z; 
	}

	Vector3 Vector3::CrossProduct(Vector3 v1, Vector3 v2)
	{
		return Vector3(v1.Y * v2.Z - v1.Z * v2.Y, 
				v1.Z * v2.X - v1.X * v2.Z,
				v1.X * v2.Y - v1.Y * v2.X);
	}

	float Vector3::Magnitude()
	{ 
		return (float)(sqrt(X * X + Y * Y + Z * Z)); 
	}

	float Vector3::MagnitudeH() 
	{ 
		return (float)(sqrt(X * X + Z * Z)); 
	}

	float Vector3::SquaredMagnitude() 
	{ 
		return X * X + Y * Y + Z * Z; 
	}

	float Vector3::Normalize()
	{
		float fMag = Magnitude();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			Clear();
			fMag = 0.0f;
		}
		else
		{
			float f = 1.0f / fMag;
			X = X * f;
			Y = Y * f;
			Z = Z * f;
		}

		return fMag;
	}
	
	float Vector3::Normalize(Vector3% vIn, [Out] Vector3% vOut)
	{
		float fMag = vIn.Magnitude();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			vOut.Clear();
			fMag = 0.0f;
		}
		else
		{
			float f = 1.0f / fMag;
			vOut = vIn * f;
		}

		return fMag;
	}

	/// <summary>
	/// Converts the value of the object to its equivalent string representation.
	/// </summary>
	/// <returns>The string representation of the value of this instance.</returns>
	System::String^ Vector3::ToString()
	{
		return String::Format( CultureInfo::CurrentCulture, "X:{0} Y:{1} Z:{2}", X.ToString(CultureInfo::CurrentCulture), Y.ToString(CultureInfo::CurrentCulture), Z.ToString(CultureInfo::CurrentCulture) );
	}

	int Vector3::GetHashCode()
	{
		return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
	}

	bool Vector3::Equals( Object^ value )
	{
		if( value == nullptr )
			return false;

		if( value->GetType() != GetType() )
			return false;

		return Equals( safe_cast<Vector3>( value ) );
	}

	bool Vector3::Equals( Vector3 value )
	{
		return ( X == value.X && Y == value.Y && Z == value.Z );
	}

	bool Vector3::Equals( Vector3% value1, Vector3% value2 )
	{
		return ( value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z );
	}
}