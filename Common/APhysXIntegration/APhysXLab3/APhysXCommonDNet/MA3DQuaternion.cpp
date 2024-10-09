#include "Stdafx.h"
#include "MA3DQuaternion.h"
#include "MA3DVector3.h"
#include "MA3DMatrix4.h"
#include "d3dx10math.h"

using namespace System;
namespace APhysXCommonDNet
{
	NxQuat Quaternion::ToNxQuat(const Quaternion& q)
	{
		NxQuat quat;
		quat.setXYZW(q.X, q.Y, q.Z, q.W);
		return quat;
	}
	Quaternion Quaternion::FromNxQuat(const NxQuat& q)
	{
		return Quaternion(q.x, q.y, q.z, q.w);
	}
	A3DQUATERNION Quaternion::ToA3DQuat(const Quaternion& q)
	{
		return A3DQUATERNION(q.W, q.X, q.Y, q.Z);
	}
	Quaternion Quaternion::FromA3DQuat(const A3DQUATERNION& q)
	{
		return Quaternion(q.x, q.y, q.z, q.w);
	}

	Quaternion::Quaternion(float x, float y, float z, float w)
	{
		X = x;
		Y = y;
		Z = z;
		W = w;
	}

	Quaternion::Quaternion( Vector3 value, float w )
	{
		X = value.X;
		Y = value.Y;
		Z = value.Z;
		W = w;
	}

	Quaternion Quaternion::Identity::get()
	{
		Quaternion result;
		result.X = 0.0f;
		result.Y = 0.0f;
		result.Z = 0.0f;
		result.W = 1.0f;
		return result;
	}

	bool Quaternion::IsIdentity::get()
	{
		if( X != 0.0f || Y != 0.0f || Z != 0.0f )
			return false;

		return (W == 1.0f);
	}

	Vector3 Quaternion::Axis::get()
	{
		pin_ptr<Quaternion> pinThis = this;
		float angle;
		Vector3 axis;

		D3DXQuaternionToAxisAngle( (D3DXQUATERNION*) pinThis, (D3DXVECTOR3*) &axis, &angle );
		return axis;
	}

	float Quaternion::Angle::get()
	{
		pin_ptr<Quaternion> pinThis = this;
		float angle;
		Vector3 axis;

		D3DXQuaternionToAxisAngle( (D3DXQUATERNION*) pinThis, (D3DXVECTOR3*) &axis, &angle );
		return angle;
	}

	float Quaternion::Length()
	{
		return static_cast<float>( Math::Sqrt( (X * X) + (Y * Y) + (Z * Z) + (W * W) ) );
	}

	float Quaternion::LengthSquared()
	{
		return (X * X) + (Y * Y) + (Z * Z) + (W * W);
	}

	void Quaternion::Normalize()
	{
		float length = 1.0f / Length();
		X *= length;
		Y *= length;
		Z *= length;
		W *= length;
	}

	void Quaternion::Conjugate()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
	}

	Quaternion Quaternion::Lerp( Quaternion left, Quaternion right, float amount )
	{
		Quaternion result;
		float inverse = 1.0f - amount;
		float dot = (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z) + (left.W * right.W); 

		if( dot >= 0.0f )
		{
			result.X = (inverse * left.X) + (amount * right.X);
			result.Y = (inverse * left.Y) + (amount * right.Y);
			result.Z = (inverse * left.Z) + (amount * right.Z);
			result.W = (inverse * left.W) + (amount * right.W);
		}
		else
		{
			result.X = (inverse * left.X) - (amount * right.X);
			result.Y = (inverse * left.Y) - (amount * right.Y);
			result.Z = (inverse * left.Z) - (amount * right.Z);
			result.W = (inverse * left.W) - (amount * right.W);
		}

		float invLength = 1.0f / result.Length();

		result.X *= invLength;
		result.Y *= invLength;
		result.Z *= invLength;
		result.W *= invLength;

		return result;
	}

	void Quaternion::Lerp( Quaternion% left, Quaternion% right, float amount, [Out] Quaternion% result )
	{
		float inverse = 1.0f - amount;
		float dot = (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z) + (left.W * right.W); 

		if( dot >= 0.0f )
		{
			result.X = (inverse * left.X) + (amount * right.X);
			result.Y = (inverse * left.Y) + (amount * right.Y);
			result.Z = (inverse * left.Z) + (amount * right.Z);
			result.W = (inverse * left.W) + (amount * right.W);
		}
		else
		{
			result.X = (inverse * left.X) - (amount * right.X);
			result.Y = (inverse * left.Y) - (amount * right.Y);
			result.Z = (inverse * left.Z) - (amount * right.Z);
			result.W = (inverse * left.W) - (amount * right.W);
		}

		float invLength = 1.0f / result.Length();

		result.X *= invLength;
		result.Y *= invLength;
		result.Z *= invLength;
		result.W *= invLength;
	}

	Quaternion Quaternion::RotationAxis( Vector3 axis, float angle )
	{
		Quaternion result;

		Vector3::Normalize( axis, axis );

		float half = angle * 0.5f;
		float sin = static_cast<float>( Math::Sin( static_cast<double>( half ) ) );
		float cos = static_cast<float>( Math::Cos( static_cast<double>( half ) ) );

		result.X = axis.X * sin;
		result.Y = axis.Y * sin;
		result.Z = axis.Z * sin;
		result.W = cos;

		return result;
	}

	void Quaternion::RotationAxis( Vector3% axis, float angle, [Out] Quaternion% result )
	{
		Vector3::Normalize( axis, axis );

		float half = angle * 0.5f;
		float sin = static_cast<float>( Math::Sin( static_cast<double>( half ) ) );
		float cos = static_cast<float>( Math::Cos( static_cast<double>( half ) ) );

		result.X = axis.X * sin;
		result.Y = axis.Y * sin;
		result.Z = axis.Z * sin;
		result.W = cos;
	}

	Quaternion Quaternion::RotationMatrix( Matrix4 matrix )
	{
		Quaternion result;
		float scale = matrix.M11 + matrix.M22 + matrix.M33;

		if( scale > 0.0f )
		{
			float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(scale + 1.0f) ) );

			result.W = sqrt * 0.5f;
			sqrt = 0.5f / sqrt;

			result.X = (matrix.M23 - matrix.M32) * sqrt;
			result.Y = (matrix.M31 - matrix.M13) * sqrt;
			result.Z = (matrix.M12 - matrix.M21) * sqrt;

			return result;
		}

		if( (matrix.M11 >= matrix.M22) && (matrix.M11 >= matrix.M33) )
		{
			float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(1.0f + matrix.M11 - matrix.M22 - matrix.M33) ) );
			float half = 0.5f / sqrt;

			result.X = 0.5f * sqrt;
			result.Y = (matrix.M12 + matrix.M21) * half;
			result.Z = (matrix.M13 + matrix.M31) * half;
			result.W = (matrix.M23 - matrix.M32) * half;

			return result;
		}

		if( matrix.M22 > matrix.M33 )
		{
			float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(1.0f + matrix.M22 - matrix.M11 - matrix.M33) ) );
			float half = 0.5f / sqrt;

			result.X = (matrix.M21 + matrix.M12) * half;
			result.Y = 0.5f * sqrt;
			result.Z = (matrix.M32 + matrix.M23) * half;
			result.W = (matrix.M31 - matrix.M13) * half;

			return result;
		}

		float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(1.0f + matrix.M33 - matrix.M11 - matrix.M22) ) );
		float half = 0.5f / sqrt;

		result.X = (matrix.M31 + matrix.M13) * half;
		result.Y = (matrix.M32 + matrix.M23) * half;
		result.Z = 0.5f * sqrt;
		result.W = (matrix.M12 - matrix.M21) * half;

		return result;
	}

	void Quaternion::RotationMatrix( Matrix4% matrix, [Out] Quaternion% result )
	{
		float scale = matrix.M11 + matrix.M22 + matrix.M33;

		if( scale > 0.0f )
		{
			float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(scale + 1.0f) ) );

			result.W = sqrt * 0.5f;
			sqrt = 0.5f / sqrt;

			result.X = (matrix.M23 - matrix.M32) * sqrt;
			result.Y = (matrix.M31 - matrix.M13) * sqrt;
			result.Z = (matrix.M12 - matrix.M21) * sqrt;
			return;
		}

		if( (matrix.M11 >= matrix.M22) && (matrix.M11 >= matrix.M33) )
		{
			float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(1.0f + matrix.M11 - matrix.M22 - matrix.M33) ) );
			float half = 0.5f / sqrt;

			result.X = 0.5f * sqrt;
			result.Y = (matrix.M12 + matrix.M21) * half;
			result.Z = (matrix.M13 + matrix.M31) * half;
			result.W = (matrix.M23 - matrix.M32) * half;
			return;
		}

		if( matrix.M22 > matrix.M33 )
		{
			float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(1.0f + matrix.M22 - matrix.M11 - matrix.M33) ) );
			float half = 0.5f / sqrt;

			result.X = (matrix.M21 + matrix.M12) * half;
			result.Y = 0.5f * sqrt;
			result.Z = (matrix.M32 + matrix.M23) * half;
			result.W = (matrix.M31 - matrix.M13) * half;
			return;
		}

		float sqrt = static_cast<float>( Math::Sqrt( static_cast<double>(1.0f + matrix.M33 - matrix.M11 - matrix.M22) ) );
		float half = 0.5f / sqrt;

		result.X = (matrix.M31 + matrix.M13) * half;
		result.Y = (matrix.M32 + matrix.M23) * half;
		result.Z = 0.5f * sqrt;
		result.W = (matrix.M12 - matrix.M21) * half;
	}

	Quaternion Quaternion::RotationYawPitchRoll( float yaw, float pitch, float roll )
	{
		Quaternion result;

		float halfRoll = roll * 0.5f;
		float sinRoll = static_cast<float>( Math::Sin( static_cast<double>( halfRoll ) ) );
		float cosRoll = static_cast<float>( Math::Cos( static_cast<double>( halfRoll ) ) );
		float halfPitch = pitch * 0.5f;
		float sinPitch = static_cast<float>( Math::Sin( static_cast<double>( halfPitch ) ) );
		float cosPitch = static_cast<float>( Math::Cos( static_cast<double>( halfPitch ) ) );
		float halfYaw = yaw * 0.5f;
		float sinYaw = static_cast<float>( Math::Sin( static_cast<double>( halfYaw ) ) );
		float cosYaw = static_cast<float>( Math::Cos( static_cast<double>( halfYaw ) ) );

		result.X = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
		result.Y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
		result.Z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
		result.W = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);

		return result;
	}

	void Quaternion::RotationYawPitchRoll( float yaw, float pitch, float roll, [Out] Quaternion% result )
	{
		float halfRoll = roll * 0.5f;
		float sinRoll = static_cast<float>( Math::Sin( static_cast<double>( halfRoll ) ) );
		float cosRoll = static_cast<float>( Math::Cos( static_cast<double>( halfRoll ) ) );
		float halfPitch = pitch * 0.5f;
		float sinPitch = static_cast<float>( Math::Sin( static_cast<double>( halfPitch ) ) );
		float cosPitch = static_cast<float>( Math::Cos( static_cast<double>( halfPitch ) ) );
		float halfYaw = yaw * 0.5f;
		float sinYaw = static_cast<float>( Math::Sin( static_cast<double>( halfYaw ) ) );
		float cosYaw = static_cast<float>( Math::Cos( static_cast<double>( halfYaw ) ) );

		result.X = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
		result.Y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
		result.Z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
		result.W = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
	}

	Quaternion Quaternion::Slerp( Quaternion q1, Quaternion q2, float t )
	{
		Quaternion result;

		float opposite;
		float inverse;
		float dot = (q1.X * q2.X) + (q1.Y * q2.Y) + (q1.Z * q2.Z) + (q1.W * q2.W);
		bool flag = false;

		if( dot < 0.0f )
		{
			flag = true;
			dot = -dot;
		}

		if( dot > 0.999999f )
		{
			inverse = 1.0f - t;
			opposite = flag ? -t : t;
		}
		else
		{
			float acos = static_cast<float>( Math::Acos( static_cast<double>( dot ) ) );
			float invSin = static_cast<float>( ( 1.0f / Math::Sin( static_cast<double>( acos ) ) ) );

			inverse = ( static_cast<float>( Math::Sin( static_cast<double>( (1.0f - t) * acos ) ) ) ) * invSin;
			opposite = flag ? ( ( static_cast<float>( -Math::Sin( static_cast<double>( t * acos ) ) ) ) * invSin ) : ( ( static_cast<float>( Math::Sin( static_cast<double>( t * acos ) ) ) ) * invSin );
		}

		result.X = (inverse * q1.X) + (opposite * q2.X);
		result.Y = (inverse * q1.Y) + (opposite * q2.Y);
		result.Z = (inverse * q1.Z) + (opposite * q2.Z);
		result.W = (inverse * q1.W) + (opposite * q2.W);

		return result;
	}

	void Quaternion::Slerp( Quaternion% q1, Quaternion% q2, float t, [Out] Quaternion% result )
	{
		float opposite;
		float inverse;
		float dot = (q1.X * q2.X) + (q1.Y * q2.Y) + (q1.Z * q2.Z) + (q1.W * q2.W);
		bool flag = false;

		if( dot < 0.0f )
		{
			flag = true;
			dot = -dot;
		}

		if( dot > 0.999999f )
		{
			inverse = 1.0f - t;
			opposite = flag ? -t : t;
		}
		else
		{
			float acos = static_cast<float>( Math::Acos( static_cast<double>( dot ) ) );
			float invSin = static_cast<float>( ( 1.0f / Math::Sin( static_cast<double>( acos ) ) ) );

			inverse = ( static_cast<float>( Math::Sin( static_cast<double>( (1.0f - t) * acos ) ) ) ) * invSin;
			opposite = flag ? ( ( static_cast<float>( -Math::Sin( static_cast<double>( t * acos ) ) ) ) * invSin ) : ( ( static_cast<float>( Math::Sin( static_cast<double>( t * acos ) ) ) ) * invSin );
		}

		result.X = (inverse * q1.X) + (opposite * q2.X);
		result.Y = (inverse * q1.Y) + (opposite * q2.Y);
		result.Z = (inverse * q1.Z) + (opposite * q2.Z);
		result.W = (inverse * q1.W) + (opposite * q2.W);
	}

	Quaternion Quaternion::operator * (Quaternion left, Quaternion right)
	{
		Quaternion quaternion;
		float lx = left.X;
		float ly = left.Y;
		float lz = left.Z;
		float lw = left.W;
		float rx = right.X;
		float ry = right.Y;
		float rz = right.Z;
		float rw = right.W;

		quaternion.X = (rx * lw + lx * rw + ry * lz) - (rz * ly);
		quaternion.Y = (ry * lw + ly * rw + rz * lx) - (rx * lz);
		quaternion.Z = (rz * lw + lz * rw + rx * ly) - (ry * lx);
		quaternion.W = (rw * lw) - (rx * lx + ry * ly + rz * lz);

		return quaternion;
	}

	Quaternion Quaternion::operator * (Quaternion quaternion, float scale)
	{
		Quaternion result;
		result.X = quaternion.X * scale;
		result.Y = quaternion.Y * scale;
		result.Z = quaternion.Z * scale;
		result.W = quaternion.W * scale;
		return result;
	}

	Quaternion Quaternion::operator * (float scale, Quaternion quaternion)
	{
		Quaternion result;
		result.X = quaternion.X * scale;
		result.Y = quaternion.Y * scale;
		result.Z = quaternion.Z * scale;
		result.W = quaternion.W * scale;
		return result;
	}

	Quaternion Quaternion::operator / (Quaternion lhs, float rhs)
	{
		Quaternion result;
		result.X = lhs.X / rhs;
		result.Y = lhs.Y / rhs;
		result.Z = lhs.Z / rhs;
		result.W = lhs.W / rhs;
		return result;
	}

	Quaternion Quaternion::operator + (Quaternion lhs, Quaternion rhs)
	{
		Quaternion result;
		result.X = lhs.X + rhs.X;
		result.Y = lhs.Y + rhs.Y;
		result.Z = lhs.Z + rhs.Z;
		result.W = lhs.W + rhs.W;
		return result;
	}

	Quaternion Quaternion::operator - (Quaternion lhs, Quaternion rhs)
	{
		Quaternion result;
		result.X = lhs.X - rhs.X;
		result.Y = lhs.Y - rhs.Y;
		result.Z = lhs.Z - rhs.Z;
		result.W = lhs.W - rhs.W;
		return result;
	}

	Quaternion Quaternion::operator - (Quaternion quaternion)
	{
		Quaternion result;
		result.X = -quaternion.X;
		result.Y = -quaternion.Y;
		result.Z = -quaternion.Z;
		result.W = -quaternion.W;
		return result;
	}

	bool Quaternion::operator == ( Quaternion left, Quaternion right )
	{
		return Quaternion::Equals( left, right );
	}

	bool Quaternion::operator != ( Quaternion left, Quaternion right )
	{
		return !Quaternion::Equals( left, right );
	}

	String^ Quaternion::ToString()
	{
		return String::Format( CultureInfo::CurrentCulture, "X:{0} Y:{1} Z:{2} W:{3}", X.ToString(CultureInfo::CurrentCulture), 
			Y.ToString(CultureInfo::CurrentCulture), Z.ToString(CultureInfo::CurrentCulture),
			W.ToString(CultureInfo::CurrentCulture) );
	}

	int Quaternion::GetHashCode()
	{
		return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
	}

	bool Quaternion::Equals( Object^ value )
	{
		if( value == nullptr )
			return false;

		if( value->GetType() != GetType() )
			return false;

		return Equals( safe_cast<Quaternion>( value ) );
	}

	bool Quaternion::Equals( Quaternion value )
	{
		return ( X == value.X && Y == value.Y && Z == value.Z && W == value.W );
	}

	bool Quaternion::Equals( Quaternion% value1, Quaternion% value2 )
	{
		return ( value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z && value1.W == value2.W );
	}

	Matrix4 Quaternion::ToMatrix4(Quaternion quat)
	{
		quat.Normalize();
		Matrix4 mat = Matrix4::Identity;
		mat.M11 = 1 - 2*(quat.Y * quat.Y + quat.Z * quat.Z);    mat.M21 = 2*(quat.X * quat.Y - quat.W * quat.Z);    mat.M31 = 2*(quat.X * quat.Z + quat.W * quat.Y);
		mat.M12 = 2*(quat.X * quat.Y + quat.W * quat.Z);        mat.M22 = 1 - 2*(quat.X*quat.X + quat.Z * quat.Z);  mat.M32 = 2*(quat.Y * quat.Z - quat.W * quat.X);
		mat.M13 = 2*(quat.X * quat.Z - quat.W * quat.Y);        mat.M23 = 2*(quat.Y * quat.Z + quat.W * quat.X);    mat.M33 = 1 - 2*(quat.X * quat.X + quat.Y * quat.Y); 
		return mat;
	}
	Matrix4 Quaternion::ToMatrix4()
	{
		return ToMatrix4(*this);
	}
}