/*
* FILE: ECEntity.h
*
* DESCRIPTION: collision entity
*
* CREATED BY: YangXiao
*
* HISTORY: 2011/2/18
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#ifndef _ECENTITY_H_
#define _ECENTITY_H_

#include <A3DGeometry.h>
#include <A3DVector.h>
#include "ConvexBrush.h"
#include "ChbEntity.h"
#include "HalfSpace.h"

class ECTraceInfo
{
public:
	enum TRACETYPE{ CAPSULE, AABB, RAY };
	TRACETYPE m_iType;
	//in parameter
	A3DVECTOR3 m_vStart;
	A3DVECTOR3 m_vDelta;
	A3DVECTOR3 m_vExtent;//for aabb
	float m_fCylinderHei;//for capsule
	float m_fRadius;//for capsule
	A3DVECTOR3 m_vCapsuleUp;//for capsule
	float m_fDistEpsilon;
	DWORD	m_dwUserData1;// 有时用于告知碰到的是什么 By:Newer
	DWORD	m_dwUserData2;// 有时用于告知碰到的是什么 By:Newer 2011-8-17

public:
	//out parameter
	bool		bStartSolid;	//	Collide something at start point
	bool		bAllSolid;		//	All in something
	CHBasedCD::CHalfSpace	ClipPlane;		//	Clip plane
	float		fFraction;		//	Fraction
	A3DAABB     BoundAABB;//bound aabb

	DWORD       dwUser1;

	void InitCapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float fCylinderHei, float fRadius, 
		const A3DVECTOR3& vUp = A3DVECTOR3(0, 1, 0), float fDistEpsilon = 0.03125f)
	{
		m_vStart = vStart;
		m_vDelta = vDelta;
		m_fCylinderHei = fCylinderHei;
		m_fRadius = fRadius;
		m_vCapsuleUp = vUp;
		m_fDistEpsilon = fDistEpsilon;

		A3DVECTOR3 g_vAxisX(1,0,0);
		A3DVECTOR3 g_vAxisZ(0,0,1);
		A3DVECTOR3 vRight = CrossProduct(vUp, g_vAxisX);
		if (vRight.SquaredMagnitude() < 1E-4f)
			vRight = CrossProduct(vUp, g_vAxisZ);
		A3DVECTOR3 vFront = CrossProduct(vRight, vUp);

		A3DVECTOR3 vExtents(fRadius, fRadius + fCylinderHei, fRadius);
		A3DOBB obb;
		obb.Clear();
		obb.Center = vStart;
		obb.Extents = vExtents;
		obb.XAxis = vRight;
		obb.ZAxis = vFront;
		obb.YAxis = vUp;
		obb.CompleteExtAxis();			
		BoundAABB.Build(obb);

		obb.Center = vStart + vDelta;
		obb.CompleteExtAxis();
		A3DAABB aabb;
		aabb.Build(obb);
		BoundAABB.Merge(aabb);
		BoundAABB.Extents += A3DVECTOR3(fDistEpsilon, fDistEpsilon, fDistEpsilon);
		BoundAABB.CompleteMinsMaxs();
		dwUser1 = 0;
		m_iType = CAPSULE;		
	}

	void InitAABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, const A3DVECTOR3& vExtents, float fDistEpsilon = 0.03125f)
	{
		m_vStart = vStart;
		m_vDelta = vDelta;
		m_vExtent = vExtents;
		m_fDistEpsilon = fDistEpsilon;

		BoundAABB.Clear();
		BoundAABB.AddVertex(vStart );
		BoundAABB.AddVertex((vStart + vExtents));
		BoundAABB.AddVertex((vStart - vExtents));
		BoundAABB.AddVertex((vStart+ vDelta));
		BoundAABB.AddVertex((vStart + vDelta + vExtents) );
		BoundAABB.AddVertex((vStart + vDelta - vExtents) );
		BoundAABB.CompleteCenterExts();
		BoundAABB.Extents += A3DVECTOR3(fDistEpsilon, fDistEpsilon, fDistEpsilon);
		BoundAABB.CompleteMinsMaxs();
		dwUser1 = 0;
		m_iType = AABB;
	}

	void InitRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float fDistEpsilon = 1E-5f)
	{
		m_vStart = vStart;
		m_vDelta = vDelta;
		m_fDistEpsilon = fDistEpsilon;
		dwUser1 = 0;
		m_iType = RAY;		
	}
};

typedef abase::vector<CHBasedCD::CConvexBrush*>							BrushVec;

/*
static object collision entity base CChbEntity
ECEntity包含静态物体世界坐标系下的aabb，aabb的overlap来加速Trace，aabb有相交由CChbEntity完成Trace 
*/
class ECEntity
{
public:
	ECEntity();	
	~ECEntity();

	//parameter description
	//brushes: brush used for collision detection
	//aabb: AABB of the brushes in world coordinate
	//AbsMatrix: matrix denotes the coordinate of entity
	//bAutoCleanUp: delete the brushes or not
	//minBrushInNode: min num of brushes in collision tree node(for setting up of collision tree)
	//minNodeSize: min size of node(for setting up of collision tree)
	void Init(const BrushVec& brushes, const A3DAABB& aabb, const A3DMATRIX4& AbsMatrix, bool bAutoCleanUp = false, int minBrushInNode = 16, float minNodeSize = 16);

	bool Trace(ECTraceInfo* pInfo, bool bCheckCHFlags = true);

	//release the collision tree
	void Release();

	//当brush没有变化，仅世界坐标系和世界坐标系下的AABB发生变化时，可以调用该函数
	void SetAbsMatrixAndAABB(const A3DMATRIX4& AbsMat, const A3DAABB& aabb); 

	//set & clear collision flag
	void SetCollisionFlag(DWORD flag) { m_dwCollisionFlag |= flag; }
	void ClearCollisionFlag(DWORD flag) { m_dwCollisionFlag &= ~flag; }
	bool IsCollisionFlagEnable(DWORD flag) const { return (m_dwCollisionFlag & flag) != 0; }

protected:
	//property
	A3DMATRIX4 m_RotTransMat;//rotate and translate matrix	

	float m_fScale;//scale

	A3DAABB m_aabb;//entity aabb in world

	CHBasedCD::CChbEntity* m_pChbEntity;//collision entity(.chb file)
	
	DWORD m_dwCollisionFlag;//collision flag;

	ECEntity(const ECEntity&);
	ECEntity& operator=(const ECEntity&);

};

/*
可以在此添加基于CChbEntity类的一个动态物体碰撞类 
*/

inline bool AABBAABBOverlap(const A3DAABB& aabb1, const A3DAABB& aabb2)
{
	if (aabb1.Mins.x > aabb2.Maxs.x || aabb2.Mins.x > aabb1.Maxs.x)
		return false;
	if (aabb1.Mins.y > aabb2.Maxs.y || aabb2.Mins.y > aabb1.Maxs.y)
		return false;
	if (aabb1.Mins.z > aabb2.Maxs.z || aabb2.Mins.z > aabb1.Maxs.z)
		return false;
	return true;
}

#endif

