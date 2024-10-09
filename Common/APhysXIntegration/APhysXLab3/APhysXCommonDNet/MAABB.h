#pragma once

#using <System.dll>
#using <mscorlib.dll>


using System::Globalization::CultureInfo;
using System::Runtime::InteropServices::OutAttribute;

namespace APhysXCommonDNet
{

	[System::Serializable]
	[System::Runtime::InteropServices::StructLayout( System::Runtime::InteropServices::LayoutKind::Sequential, Pack = 4 )]
	public value class AABB : System::IEquatable<AABB>
	{
	public:

		static A3DAABB ToA3DAABB(AABB aabb);
		static AABB FromA3DAABB(const A3DAABB& aabb);

	public:
		Vector3	Center;
		Vector3	Extents;
		Vector3	Mins;
		Vector3	Maxs;

	public:


		AABB(Vector3 vMins, Vector3 vMaxs) :
		Mins(vMins),
			Maxs(vMaxs),
			Center((vMins + vMaxs) * 0.5f)
		{
			Extents = vMaxs - Center;
		}

	public:		//	Operations

		//	Clear aabb
		void Clear()
		{
			Mins.Set(FLT_MAX, FLT_MAX, FLT_MAX);
			Maxs.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			Extents.Set(FLT_MAX, FLT_MAX, FLT_MAX);
		}

		//	Check if aabb's content is valid ? A just cleared aabb is treated as invalid
		bool IsValid() { return Mins.X != FLT_MAX && Extents.X != FLT_MAX; }

		//	Merge two aabb
		//	bComplete: true, complete all AABB data after merge
		void Merge(AABB subAABB, bool bComplete);

		//	Compute Mins and Maxs
		void CompleteMinsMaxs()
		{
			if (Extents.X != FLT_MAX)
			{
				Mins = Center - Extents;
				Maxs = Center + Extents;
			}
		}

		//	Compute Center and Extents
		void CompleteCenterExts()
		{
			if (Mins.X != FLT_MAX && Maxs.X != FLT_MAX)
			{
				Center  = (Mins + Maxs) * 0.5f;
				Extents = Maxs - Center;
			}
		}

		//	Check whether a point is in this aabb
		bool IsPointIn(Vector3 v)
		{
			if (v.X > Maxs.X || v.X < Mins.X ||
				v.Y > Maxs.Y || v.Y < Mins.Y ||
				v.Z > Maxs.Z || v.Z < Mins.Z)
				return false;

			return true;
		}

		//	Check whether another aabb is in this aabb
		bool IsAABBIn(AABB aabb);

		//	Build AABB from vertices
		void Build(array<Vector3>^ Verts);
		//	Get corner vertices
		void GetCorners(array<Vector3>^% Verts);

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
		virtual bool Equals( AABB other );

		/// <summary>
		/// Determines whether the specified object instances are considered equal. 
		/// </summary>
		/// <param name="value1">The first value to compare.</param>
		/// <param name="value2">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
		/// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
		static bool Equals( AABB% value1, AABB% value2 );
	};


}