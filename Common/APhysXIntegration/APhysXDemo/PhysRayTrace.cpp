
#include "physraytrace.h"
#include "assert.h"

inline static float PhysDot(const PhysVec3& v1, const PhysVec3& v2)
{
#if		USE_A3DVECTOR3
	
	return DotProduct(v1, v2);

#elif	USE_NXVEC3
	
	return v1.dot(v2);

#else

	return v1.Dot(v2);

#endif


}

inline static bool IsInRange(float fValue, float fMin, float fMax)
{
	return fValue >= fMin && fValue <= fMax;

}

bool RayTraceAABB(const PhysRay& ray, const PhysAABB& aabb, PhysRayTraceHit& hitInfo)
{
	assert(hitInfo.t <= 1.0f);
	
	// intersection test with perpendicular X plane...

	if(ray.vDelta.x != 0)
	{
		float fX = (ray.vDelta.x > 0) ? aabb.vMin.x : aabb.vMax.x;

		float t = (fX - ray.vStart.x) / ray.vDelta.x; 

		if(IsInRange(t, 0, hitInfo.t))
		{
			float fY = ray.vStart.y + t * ray.vDelta.y;
			
			if(IsInRange(fY, aabb.vMin.y, aabb.vMax.y))
			{
				float fZ = ray.vStart.z + t * ray.vDelta.z;

				if(IsInRange(fZ, aabb.vMin.z, aabb.vMax.z))
				{
					// intersection occurs...
					// fill the hitInfo

					hitInfo.t = t;

					hitInfo.vHit.x = fX;
					hitInfo.vHit.y = fY;
					hitInfo.vHit.z = fZ;

					hitInfo.vNormal.x = (ray.vDelta.x > 0) ? -1.0f : 1.0f;
					hitInfo.vNormal.y = 0;
					hitInfo.vNormal.z = 0;
					
					return true;

				}
			}

		}


	}

	// intersection test with perpendicular Y plane...

	if(ray.vDelta.y != 0)
	{
		float fY = (ray.vDelta.y > 0) ? aabb.vMin.y : aabb.vMax.y;

		float t = (fY - ray.vStart.y) / ray.vDelta.y; 

		if(IsInRange(t, 0, hitInfo.t))
		{
			float fX = ray.vStart.x + t * ray.vDelta.x;
			
			if(IsInRange(fX, aabb.vMin.x, aabb.vMax.x))
			{
				float fZ = ray.vStart.z + t * ray.vDelta.z;

				if(IsInRange(fZ, aabb.vMin.z, aabb.vMax.z))
				{
					// intersection occurs...
					// fill the hitInfo

					hitInfo.t = t;

					hitInfo.vHit.x = fX;
					hitInfo.vHit.y = fY;
					hitInfo.vHit.z = fZ;

					hitInfo.vNormal.x = 0;
					hitInfo.vNormal.y = (ray.vDelta.y > 0) ? -1.0f : 1.0f;
					hitInfo.vNormal.z = 0;
					
					return true;

				}
			}

		}


	}


	// intersection test with perpendicular Z plane...

	if(ray.vDelta.z != 0)
	{
		float fZ = (ray.vDelta.z > 0) ? aabb.vMin.z : aabb.vMax.z;

		float t = (fZ - ray.vStart.z) / ray.vDelta.z; 

		if(IsInRange(t, 0, hitInfo.t))
		{
			float fY = ray.vStart.y + t * ray.vDelta.y;
			
			if(IsInRange(fY, aabb.vMin.y, aabb.vMax.y))
			{
				float fX = ray.vStart.x + t * ray.vDelta.x;

				if(IsInRange(fX, aabb.vMin.x, aabb.vMax.x))
				{
					// intersection occurs...
					// fill the hitInfo

					hitInfo.t = t;

					hitInfo.vHit.x = fX;
					hitInfo.vHit.y = fY;
					hitInfo.vHit.z = fZ;

					hitInfo.vNormal.x = 0;
					hitInfo.vNormal.y = 0;
					hitInfo.vNormal.z = (ray.vDelta.x > 0) ? -1.0f : 1.0f;
					
					return true;

				}
			}

		}


	}

	return false;
}

bool RayTraceOBB(const PhysRay& ray, const PhysOBB& obb, PhysRayTraceHit& hitInfo)
{
	// transform the ray...
	PhysRay rayTrans;
	rayTrans.vStart.x = PhysDot(ray.vStart, obb.vX);
	rayTrans.vStart.y = PhysDot(ray.vStart, obb.vY);
	rayTrans.vStart.z = PhysDot(ray.vStart, obb.vZ);

	rayTrans.vDelta.x = PhysDot(ray.vDelta, obb.vX);
	rayTrans.vDelta.y = PhysDot(ray.vDelta, obb.vY);
	rayTrans.vDelta.z = PhysDot(ray.vDelta, obb.vZ);

	

	bool bRes = RayTraceAABB(rayTrans, (const PhysAABB& )obb, hitInfo);

	if(bRes)
	{
		// re-filling the hitInfo...

		// revise the hit normal...
		if(hitInfo.vNormal.x == 1.0f)
		{
			hitInfo.vNormal = obb.vX;
		}
		else if (hitInfo.vNormal.x == -1.0f)
		{
			hitInfo.vNormal = -obb.vX;
		}
		else if (hitInfo.vNormal.y == 1.0f)
		{
			hitInfo.vNormal = obb.vY;
		}
		else if (hitInfo.vNormal.y == -1.0f)
		{
			hitInfo.vNormal = -obb.vY;
		}
		else if (hitInfo.vNormal.z == 1.0f)
		{
			hitInfo.vNormal = obb.vZ;
		}
		else if (hitInfo.vNormal.z == -1.0f)
		{
			hitInfo.vNormal = -obb.vZ;
		}


		// recompute the hit pos...
		hitInfo.vHit = ray.vStart + hitInfo.t * ray.vDelta;
	}
	
	return bRes;
}
