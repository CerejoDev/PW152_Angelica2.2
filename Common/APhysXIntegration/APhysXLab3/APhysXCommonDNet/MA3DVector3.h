#pragma once

#using <System.dll>
#using <mscorlib.dll>


using System::Globalization::CultureInfo;
using System::Runtime::InteropServices::OutAttribute;

namespace APhysXCommonDNet
{

	[System::Serializable]
	[System::Runtime::InteropServices::StructLayout( System::Runtime::InteropServices::LayoutKind::Sequential, Pack = 4 )]
	public value class Vector3 : System::IEquatable<Vector3>
	{
	public:

		static A3DVECTOR3 ToA3DVECTOR3(Vector3 v);
		static Vector3 FromA3DVECTOR3(const A3DVECTOR3& v);

	public:
		/// <summary>
		/// Gets or sets the X component of the vector.
		/// </summary>
		/// <value>The X component of the vector.</value>
		float X;

		/// <summary>
		/// Gets or sets the Y component of the vector.
		/// </summary>
		/// <value>The Y component of the vector.</value>
		float Y;

		/// <summary>
		/// Gets or sets the Z component of the vector.
		/// </summary>
		/// <value>The Z component of the vector.</value>
		float Z;

		/// <summary>
		/// Gets a <see cref="Vector3"/> with all of its components set to zero.
		/// </summary>
		/// <value>A <see cref="Vector3"/> that has all of its components set to zero.</value>
		static property Vector3 vOrigin { Vector3 get() { return Vector3(0, 0, 0); } }

		/// <summary>
		/// Gets the X unit <see cref="Vector3"/> (1, 0, 0).
		/// </summary>
		/// <value>A <see cref="Vector3"/> that has a value of (1, 0, 0).</value>
		static property Vector3 vAxisX { Vector3 get() { return Vector3(1, 0, 0); } }

		/// <summary>
		/// Gets the Y unit <see cref="Vector3"/> (0, 1, 0).
		/// </summary>
		/// <value>A <see cref="Vector3"/> that has a value of (0, 1, 0).</value>
		static property Vector3 vAxisY { Vector3 get() { return Vector3(0, 1, 0); } }

		/// <summary>
		/// Gets the Z unit <see cref="Vector3"/> (0, 0, 1).
		/// </summary>
		/// <value>A <see cref="Vector3"/> that has a value of (0, 0, 1).</value>
		static property Vector3 vAxisZ { Vector3 get() { return Vector3(0, 0, 1); } }

		/// <summary>
		/// Initializes a new instance of the <see cref="Vector3"/> class.
		/// </summary>
		/// <param name="value">The value that will be assigned to all components.</param>
		Vector3( float value );		
		/// <summary>
		/// Initializes a new instance of the <see cref="Vector3"/> class.
		/// </summary>
		/// <param name="x">Initial value for the X component of the vector.</param>
		/// <param name="y">Initial value for the Y component of the vector.</param>
		/// <param name="z">Initial value for the Z component of the vector.</param>
		Vector3( float x, float y, float z );	

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="left">The first vector to add.</param>
		/// <param name="right">The second vector to add.</param>
		/// <returns>The sum of the two vectors.</returns>
		static Vector3 operator + ( Vector3 left, Vector3 right );

		/// <summary>
		/// Subtracts two vectors.
		/// </summary>
		/// <param name="left">The first vector to subtract.</param>
		/// <param name="right">The second vector to subtract.</param>
		/// <returns>The difference of the two vectors.</returns>
		static Vector3 operator - ( Vector3 left, Vector3 right );

		/// <summary>
		/// Reverses the direction of a given vector.
		/// </summary>
		/// <param name="value">The vector to negate.</param>
		/// <returns>A vector facing in the opposite direction.</returns>
		static Vector3 operator - ( Vector3 value );

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="vector">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		static Vector3 operator * ( Vector3 vector, float scale );

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="vector">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		static Vector3 operator * ( float scale, Vector3 vector );

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="vector">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		static Vector3 operator / ( Vector3 vector, float scale );

		/// <summary>
		/// Tests for equality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		static bool operator == ( Vector3 left, Vector3 right );

		/// <summary>
		/// Tests for inequality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		static bool operator != ( Vector3 left, Vector3 right );

		static float DotProduct(Vector3 v1, Vector3 v2);
		static Vector3 CrossProduct(Vector3 v1, Vector3 v2);

		void Set(float _x, float _y, float _z) { X = _x; Y = _y; Z = _z; }
		void Clear() { X = Y = Z = 0.0f; }

		float Magnitude();
		float MagnitudeH();
		float SquaredMagnitude();

		bool IsZero() { return (X == 0.f && Y == 0.f && Z == 0.f); }
		float Normalize();
		static float Normalize(Vector3% vIn, [Out] Vector3% vOut);

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
		virtual bool Equals( Vector3 other );

		/// <summary>
		/// Determines whether the specified object instances are considered equal. 
		/// </summary>
		/// <param name="value1">The first value to compare.</param>
		/// <param name="value2">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
		/// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
		static bool Equals( Vector3% value1, Vector3% value2 );
	};


}