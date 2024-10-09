#pragma once


namespace APhysXCommonDNet
{

	value class Vector3;

	[System::Serializable]
	[System::Runtime::InteropServices::StructLayout( System::Runtime::InteropServices::LayoutKind::Sequential, Pack = 4 )]
	public value class Vector4
	{
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
		/// Gets or sets the W component of the vector.
		/// </summary>
		/// <value>The W component of the vector.</value>
		float W;

		property float default[int]
		{
			float get( int index );
			void set( int index, float value );
		}

		/// <summary>
		/// Gets a <see cref="Vector4"/> with all of its components set to zero.
		/// </summary>
		/// <value>A <see cref="Vector4"/> that has all of its components set to zero.</value>
		static property Vector4 Zero { Vector4 get() { return Vector4(0, 0, 0, 0); } }

		/// <summary>
		/// Gets the X unit <see cref="Vector4"/> (1, 0, 0, 0).
		/// </summary>
		/// <value>A <see cref="Vector4"/> that has a value of (1, 0, 0, 0).</value>
		static property Vector4 UnitX { Vector4 get() { return Vector4(1, 0, 0, 0); } }

		/// <summary>
		/// Gets the Y unit <see cref="Vector4"/> (0, 1, 0, 0).
		/// </summary>
		/// <value>A <see cref="Vector4"/> that has a value of (0, 1, 0, 0).</value>
		static property Vector4 UnitY { Vector4 get() { return Vector4(0, 1, 0, 0); } }

		/// <summary>
		/// Gets the Z unit <see cref="Vector4"/> (0, 0, 1, 0).
		/// </summary>
		/// <value>A <see cref="Vector4"/> that has a value of (0, 0, 1, 0).</value>
		static property Vector4 UnitZ { Vector4 get() { return Vector4(0, 0, 1, 0); } }

		/// <summary>
		/// Gets the W unit <see cref="Vector4"/> (0, 0, 0, 1).
		/// </summary>
		/// <value>A <see cref="Vector4"/> that has a value of (0, 0, 0, 1).</value>
		static property Vector4 UnitW { Vector4 get() { return Vector4(0, 0, 0, 1); } }

		/// <summary>
		/// Gets the size of the <see cref="Vector4"/> type, in bytes.
		/// </summary>
		static property int SizeInBytes { int get() { return System::Runtime::InteropServices::Marshal::SizeOf(Vector4::typeid); } }

		/// <summary>
		/// Initializes a new instance of the <see cref="Vector4"/> class.
		/// </summary>
		/// <param name="value">The value that will be assigned to all components.</param>
		Vector4( float value );	

		/// <summary>
		/// Initializes a new instance of the <see cref="Vector4"/> class.
		/// </summary>
		/// <param name="value">A vector containing the values with which to initialize the X, Y, and Z components</param>
		/// <param name="w">Initial value for the W component of the vector.</param>
		Vector4( Vector3 value, float w );

		/// <summary>
		/// Initializes a new instance of the <see cref="Vector4"/> class.
		/// </summary>
		/// <param name="x">Initial value for the X component of the vector.</param>
		/// <param name="y">Initial value for the Y component of the vector.</param>
		/// <param name="z">Initial value for the Z component of the vector.</param>
		/// <param name="w">Initial value for the W component of the vector.</param>
		Vector4( float x, float y, float z, float w );	

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="left">The first vector to add.</param>
		/// <param name="right">The second vector to add.</param>
		/// <returns>The sum of the two vectors.</returns>
		static Vector4 operator + ( Vector4 left, Vector4 right );

		/// <summary>
		/// Subtracts two vectors.
		/// </summary>
		/// <param name="left">The first vector to subtract.</param>
		/// <param name="right">The second vector to subtract.</param>
		/// <returns>The difference of the two vectors.</returns>
		static Vector4 operator - ( Vector4 left, Vector4 right );

		/// <summary>
		/// Reverses the direction of a given vector.
		/// </summary>
		/// <param name="value">The vector to negate.</param>
		/// <returns>A vector facing in the opposite direction.</returns>
		static Vector4 operator - ( Vector4 value );

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="vector">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		static Vector4 operator * ( Vector4 vector, float scale );

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="vector">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		static Vector4 operator * ( float scale, Vector4 vector );

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="vector">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		static Vector4 operator / ( Vector4 vector, float scale );

		/// <summary>
		/// Tests for equality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		static bool operator == ( Vector4 left, Vector4 right );

		/// <summary>
		/// Tests for inequality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		static bool operator != ( Vector4 left, Vector4 right );

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
		virtual bool Equals( Vector4 other );

		/// <summary>
		/// Determines whether the specified object instances are considered equal. 
		/// </summary>
		/// <param name="value1">The first value to compare.</param>
		/// <param name="value2">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
		/// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
		static bool Equals( Vector4% value1, Vector4% value2 );
	};

}