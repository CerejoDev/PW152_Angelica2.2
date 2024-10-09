
/************************************************************************

 PhysRayTrace.h

	this file tries to support some RayTrace functions for picking objects

	Created by:		He Wenfeng 
	Date:			Oct. 20, 2008

************************************************************************/



#ifndef _PHYSRAYTRACE_H_
#define _PHYSRAYTRACE_H_

#ifndef NULL
	#define NULL 0
#endif

#define USE_A3DVECTOR3	1
#define USE_NXVEC3		0

#if		USE_A3DVECTOR3

	#include "A3DVECTOR.h"	
	typedef A3DVECTOR3 PhysVec3;

#elif	USE_NXVEC3
	
	#include "NxVec3.h"
	typedef NxVec3 PhysVec3;

#else
	
	// to do
	// #error PhysVec3 has not defined!

	struct PhysVec3
	{
		float x, y, z;
		
		PhysVec3() {}
		
		PhysVec3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}

		void Set(float _x, float _y, float _z){ x =_x; y = _y; z = _z;}
		
		// some basic functions...
		PhysVec3 operator- () const
		{
			return PhysVec3(-x, -y, -z);
		}

		PhysVec3 operator+ (const PhysVec3& other) const
		{
			return PhysVec3(x + other.x, y + other.y, z + other.z);
		}

		PhysVec3 operator- (const PhysVec3& other) const
		{
			return PhysVec3(x - other.x, y - other.y, z - other.z);
		}
		
		PhysVec3 operator* (float t) const
		{
			return PhysVec3(t * x, t * y, t * z);
		}

		const PhysVec3& operator=(const PhysVec3& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;

			return *this;
		}

		float Dot(const PhysVec3& vOther) const
		{
			return x * vOther.x + y * vOther.y + z* vOther.z;
		}
	
	};


	inline PhysVec3 operator* (float t, const PhysVec3& v)
	{
		return PhysVec3(t * v.x, t * v.y, t * v.z);
	}

#endif

//-----------------------------------------------------------------------
// ray function is vStart + t * vDelta  (0 =< t <= 1)
//
struct PhysRay
{
	PhysVec3 vStart;
	PhysVec3 vDelta;
};

//-----------------------------------------------------------------------
struct PhysRayTraceHit
{

	PhysVec3 vHit;
	PhysVec3 vNormal;

	float t;

	void* UserData;

	PhysRayTraceHit()
	{
		t = 1.0f;
		UserData = NULL;
	}
};

//-----------------------------------------------------------------------
struct PhysAABB
{

	PhysVec3 vMin;
	PhysVec3 vMax;
};

//-----------------------------------------------------------------------
// Note:
//		OBB's vMin and vMax is defined in the local frame's coordinates...
struct PhysOBB: public PhysAABB
{
	PhysVec3 vX;
	PhysVec3 vY;
	PhysVec3 vZ;
};


//-----------------------------------------------------------------------
// Note: we will ignore the case if the ray's start position is inside the aabb...
bool RayTraceAABB(const PhysRay& ray, const PhysAABB& aabb, PhysRayTraceHit& hitInfo);

bool RayTraceOBB(const PhysRay& ray, const PhysOBB& obb, PhysRayTraceHit& hitInfo);


#endif