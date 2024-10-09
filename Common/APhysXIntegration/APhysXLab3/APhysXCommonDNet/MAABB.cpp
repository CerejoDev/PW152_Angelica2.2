#include "Stdafx.h"
#include "MAABB.h"

using namespace System;

namespace APhysXCommonDNet
{

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
	A3DAABB AABB::ToA3DAABB(AABB aabb)
	{
		A3DAABB ret;
		ret.Center = Vector3::ToA3DVECTOR3(aabb.Center);
		ret.Extents = Vector3::ToA3DVECTOR3(aabb.Extents);
		ret.CompleteMinsMaxs();

		return ret;
	}

	AABB AABB::FromA3DAABB(const A3DAABB& aabb)
	{
		AABB ret;
		ret.Center = Vector3::FromA3DVECTOR3(aabb.Center);
		ret.Extents = Vector3::FromA3DVECTOR3(aabb.Extents);
		ret.Mins = Vector3::FromA3DVECTOR3(aabb.Mins);
		ret.Maxs = Vector3::FromA3DVECTOR3(aabb.Maxs);

		return ret;
	}


	void AABB::Merge(AABB subAABB, bool bComplete)
	{
		Mins.X = min(Mins.X, subAABB.Mins.X);
		Mins.Y = min(Mins.Y, subAABB.Mins.Y);
		Mins.Z = min(Mins.Z, subAABB.Mins.Z);

		Maxs.X = max(Maxs.X, subAABB.Maxs.X);
		Maxs.Y = max(Maxs.Y, subAABB.Maxs.Y);
		Maxs.Z = max(Maxs.Z, subAABB.Maxs.Z);

		CompleteCenterExts();
	}

	//	Check whether another aabb is in this aabb
	bool AABB::IsAABBIn(AABB aabb)
	{
		if ((Mins.X < aabb.Mins.X && Maxs.X > aabb.Maxs.X)
			&& (Mins.Y < aabb.Mins.Y && Maxs.Y > aabb.Maxs.Y)
			&& (Mins.Z < aabb.Mins.Z && Maxs.Z > aabb.Maxs.Z))
			return true;

		return false;
	}

	//	Build AABB from vertices
	void AABB::Build(array<Vector3>^ Verts)
	{
		if (Verts->Length == 0)
		{
			Clear();
			return;
		}
		Mins = Maxs = Verts[0];
		for (int i = 1; i < Verts->Length; i++)
		{
			if (Verts[i].X < Mins.X) Mins.X = Verts[i].X;
			if (Verts[i].Y < Mins.Y) Mins.Y = Verts[i].Y;
			if (Verts[i].Z < Mins.Z) Mins.Z = Verts[i].Z;

			if (Verts[i].X > Maxs.X) Maxs.X = Verts[i].X;
			if (Verts[i].Y > Maxs.Y) Maxs.Y = Verts[i].Y;
			if (Verts[i].Z > Maxs.Z) Maxs.Z = Verts[i].Z;
		}
		CompleteCenterExts();
	}
	//	Get corner vertices
	void AABB::GetCorners(array<Vector3>^% Verts)
	{
		//Verts->Clear();
	}

	bool AABB::Equals( System::Object^ obj )
	{
		if( obj == nullptr )
			return false;

		if( obj->GetType() != GetType() )
			return false;

		return Equals( safe_cast<AABB>( obj ) );
	}

	bool AABB::Equals( AABB other )
	{
		return (Mins == other.Mins && Maxs == other.Maxs);
	}

	bool AABB::Equals( AABB% value1, AABB% value2 )
	{
		return (value1.Mins == value2.Mins && value1.Maxs == value2.Maxs);
	}
}