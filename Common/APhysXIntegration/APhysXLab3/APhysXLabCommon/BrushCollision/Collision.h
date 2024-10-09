/*
 * FILE: Collision.h
 *
 * DESCRIPTION: collision interface used by ECCDR
 *
 * 
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#pragma once
#include <Windows.h>
#include <A3DVector.h>
#include <A3DGeometry.h>

struct CDR_TRACE_OUTPUT;
struct BrushCollideParam;
class A3DTerrain2;
class CBrushesObjMgr;

//@desc : used to retrieve support plane (ground or brush),  By Kuiwu[12/9/2005]
struct ground_trace_t 
{
	A3DVECTOR3 vStart;
	A3DVECTOR3 vExt;
	float      fDeltaY;   //down (-y)
	
	//capsule info(yx)
	A3DVECTOR3  vCapsuleUp;//capsule up direction
	float fCapsuleR;//capsule radius
	float fCapsuleHei;//capsule length(not include cap)
	float fCapsuleCenterHei;//capsule center to bottom
	
	A3DVECTOR3 vEnd;
	A3DVECTOR3 vHitNormal;
	bool       bSupport;  //false if ground missed
	
	
	int			iUserData1;		//	User data 1
	int			iUserData2;		//	User data 2
	void*       pUser1;         //user pointer 1 
	
	ground_trace_t::ground_trace_t() :
	vStart(0.0f),
		vExt(0.0f),
		vEnd(0.0f),
		vHitNormal(0.0f),
		fCapsuleHei(0.0f),
		fCapsuleR(0.0f),
		vCapsuleUp(0,1,0),
		fCapsuleCenterHei(0)
	{
		fDeltaY		= 0.0f;
		bSupport	= false;
		iUserData1	= 0;
		iUserData2	= 0;
		pUser1 = NULL;
	}
};


//@desc :used to trace the environment, brush&terrain&water  By Kuiwu[8/10/2005]
struct env_trace_t
{
	A3DVECTOR3 vStart;  // brush start
	A3DVECTOR3 vExt;
	A3DVECTOR3 vDelta;
	A3DVECTOR3 vTerStart;
	A3DVECTOR3 vWatStart;
	DWORD      dwCheckFlag;
	bool       bWaterSolid;
	
	//capsule info(yx)
	A3DVECTOR3  vCapsuleUp;//capsule up direction
	float fCapsuleR;//capsule radius
	float fCapsuleHei;//capsule length(not include cap)
	
	float		fFraction;
	A3DVECTOR3	vHitNormal;
	bool		bStartSolid;     //start in solid
	DWORD		dwClsFlag;       //collision flag
	int			iUserData1;		//	User data 1
	int			iUserData2;		//	User data 2
	void*       pUser1;         //  user defined pointer(solid brush)
	
	env_trace_t::env_trace_t() :
	vStart(0.0f),
		vExt(0.0f),
		vDelta(0.0f),
		vTerStart(0.0f),
		vWatStart(0.0f),
		vHitNormal(0.0f),
		fCapsuleHei(0.0f),
		fCapsuleR(0.0f),
		vCapsuleUp(0,1,0)
	{
		dwCheckFlag	= 0;
		bWaterSolid	= false;
		fFraction	= 1.0f;
		bStartSolid	= false;
		dwClsFlag	= 0;
		iUserData1	= 0;
		iUserData2	= 0;
		pUser1      = NULL;
	}
};

#define FRIENDFUN friend bool VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY, bool* pStartSolid);\
friend bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction);\
friend bool RetrievePlane(BrushCollideParam *pParam, A3DVECTOR3& vHitPos);\
friend bool VertTrace(BrushCollideParam *pParam, CDR_TRACE_OUTPUT& Out, float DeltaY);\
friend bool RayTraceEnv(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction , bool* pStartSolid );\
friend bool VertRayTraceEnv(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY );\
friend bool IsPosInAvailableMap(const A3DVECTOR3& vPos);\
friend bool CollideWithBrush(BrushCollideParam *pParam);\
friend bool CollideWithWater(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, bool bWaterSolid, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart );\
friend bool CollideWithTerrain(const A3DVECTOR3& vStart,const A3DVECTOR3& vCenter, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart);\
friend bool CollideWithEnv(env_trace_t * pEnvTrc);\
friend bool  RetrieveSupportPlane(ground_trace_t * pTrc);

					

class ICollisionSupplier
{
public:
	virtual ~ICollisionSupplier() {}

	virtual bool CollideWithEnv(env_trace_t * pEnvTrc);

	virtual bool Init(A3DTerrain2* pTerr, CBrushesObjMgr* pBrushesObjMgr) = 0;
	virtual float GetWaterHeight(const A3DVECTOR3& vPos) const = 0;

	virtual A3DTerrain2* GetTerrain() const = 0;
	virtual CBrushesObjMgr* GetBrushObjMgr() const = 0;

protected:
	virtual bool VertTrace(BrushCollideParam *pParam, CDR_TRACE_OUTPUT* pOut, float DeltaY=100.0f);
		

	//called by CollideWithEnv
	virtual bool CollideWithTerrain(const A3DVECTOR3& vStart, const A3DVECTOR3& vCenter, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart);

	//called by CollideWithEnv
	virtual bool CollideWithWater(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, bool bWaterSolid, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart);

	
	virtual bool RetrieveSupportPlane(ground_trace_t * pTrc);

	virtual bool VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY, bool* pStartSolid = NULL);

	
	virtual bool RetrievePlane(BrushCollideParam *pParam, A3DVECTOR3& vHitPos);

	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction);

	////////////////////////////////////pure/////////////////////////////////////	
	virtual void GetTerrainInfo(const A3DVECTOR3& vPos, A3DVECTOR3& vPosOnSurface, A3DVECTOR3& vNormal) = 0;
	virtual bool IsPosInAvailableMap(const A3DVECTOR3& vPos) = 0;
	
	//called by RayTrace and VertRayTrace
	virtual bool RayTraceEnv(const A3DVECTOR3& vStart,  const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction, bool* pStartSolid = NULL) = 0;
	//called by CollideWithEnv and cdr
	virtual bool CollideWithBrush(BrushCollideParam *pParam) = 0;

	virtual bool IsTerrEntry(const A3DVECTOR3& vPos) = 0;
	
	//friend function
	FRIENDFUN
};

ICollisionSupplier* GetCollisionSupplier();

inline bool AABBAABBCollision(const A3DAABB& aabb1, const A3DAABB& aabb2)
{
	if (aabb1.Mins.x > aabb2.Maxs.x || aabb2.Mins.x > aabb1.Maxs.x)
		return false;
	if (aabb1.Mins.y > aabb2.Maxs.y || aabb2.Mins.y > aabb1.Maxs.y)
		return false;
	if (aabb1.Mins.z > aabb2.Maxs.z || aabb2.Mins.z > aabb1.Maxs.z)
		return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

