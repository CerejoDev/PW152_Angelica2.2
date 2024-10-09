#include "StdAfx.h"
#include "MA3DVector4.h"
#include "MA3DVector3.h"
#include <A3DMatrix.h>

using namespace System;

namespace APhysXCommonDNet
{
	Vector4::Vector4( float value )
	{
		X = value;
		Y = value;
		Z = value;
		W = value;
	}

	Vector4::Vector4( Vector3 value, float w )
	{
		X = value.X;
		Y = value.Y;
		Z = value.Z;
		W = w;
	}

	Vector4::Vector4( float x, float y, float z, float w )
	{
		X = x;
		Y = y;
		Z = z;
		W = w;
	}

	float Vector4::default::get( int index )
	{
		switch( index )
		{
		case 0:
			return X;

		case 1:
			return Y;

		case 2:
			return Z;

		case 3:
			return W;

		default:
			throw gcnew System::ArgumentOutOfRangeException( "index", "Indices for Vector4 run from 0 to 3, inclusive." );
		}
	}

	void Vector4::default::set( int index, float value )
	{
		switch( index )
		{
		case 0:
			X = value;
			break;

		case 1:
			Y = value;
			break;

		case 2:
			Z = value;
			break;

		case 3:
			W = value;
			break;

		default:
			throw gcnew System::ArgumentOutOfRangeException( "index", "Indices for Vector4 run from 0 to 3, inclusive." );
		}
	}

	Vector4 Vector4::operator + ( Vector4 left, Vector4 right )
	{
		return Vector4( left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W );
	}

	Vector4 Vector4::operator - ( Vector4 left, Vector4 right )
	{
		return Vector4( left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W );
	}

	Vector4 Vector4::operator - ( Vector4 value )
	{
		return Vector4( -value.X, -value.Y, -value.Z, -value.W );
	}

	Vector4 Vector4::operator * ( Vector4 value, float scale )
	{
		return Vector4( value.X * scale, value.Y * scale, value.Z * scale, value.W * scale );
	}

	Vector4 Vector4::operator * ( float scale, Vector4 vec )
	{
		return vec * scale;
	}

	Vector4 Vector4::operator / ( Vector4 value, float scale )
	{
		return Vector4( value.X / scale, value.Y / scale, value.Z / scale, value.W / scale );
	}

	bool Vector4::operator == ( Vector4 left, Vector4 right )
	{
		return Vector4::Equals( left, right );
	}

	bool Vector4::operator != ( Vector4 left, Vector4 right )
	{
		return !Vector4::Equals( left, right );
	}

	String^ Vector4::ToString()
	{
		return String::Format( CultureInfo::CurrentCulture, "X:{0} Y:{1} Z:{2} W:{3}", X.ToString(CultureInfo::CurrentCulture), 
			Y.ToString(CultureInfo::CurrentCulture), Z.ToString(CultureInfo::CurrentCulture),
			W.ToString(CultureInfo::CurrentCulture) );
	}

	int Vector4::GetHashCode()
	{
		return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
	}

	bool Vector4::Equals( Object^ value )
	{
		if( value == nullptr )
			return false;

		if( value->GetType() != GetType() )
			return false;

		return Equals( safe_cast<Vector4>( value ) );
	}

	bool Vector4::Equals( Vector4 value )
	{
		return ( X == value.X && Y == value.Y && Z == value.Z && W == value.W );
	}

	bool Vector4::Equals( Vector4% value1, Vector4% value2 )
	{
		return ( value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z && value1.W == value2.W );
	}
}