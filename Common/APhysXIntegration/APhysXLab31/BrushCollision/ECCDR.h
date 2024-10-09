/*
 * FILE: ECCDR.h
 *
 * DESCRIPTION:  A set of interfaces define the moving strategy of the avatar.
 *							Which focus on collision detection and give an appropriate response.
 *
 * CREATED BY: He wenfeng, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once
#include <Windows.h>
#include "A3DVector.h"

//inspired  by half life 2
#define  CDR_HL2_MOVE

#define  CDR_BRUSH    0x1
#define  CDR_TERRAIN  0x2
#define  CDR_WATER    0x4


// for ground air water move case
//@note  :   By Niuyadong 2009-3-10
struct CDR_INFO
{
	//revised by yx, for capsule trace
	A3DVECTOR3	vCenter;//the capsule position
	A3DVECTOR3  vCapsuleUp;//capsule up direction
	float fCapsuleR;//capsule radius
	float fCapsuleHei;//capsule length(not include cap)
	float fCapsuleCenterHei;// capsule center height

	//added by yx,防止特别短的陡坡也上不去，例如稍微倾斜的楼梯
	float fSlopMaxH;//max length(in horizon) steep slop can stride over
	bool bShortSlop;//

	//@note : the caller should make sure ext(.x, .y, .z) > 0.  By Kuiwu[22/9/2005]
	A3DVECTOR3  vExtent;//aabb trace in the past, should be deleted latter
	float	fStepHeight;
	float	fMaxFallSpeed;	
	// Velocity Info
	A3DVECTOR3 vVelDir;
#ifdef CDR_HL2_MOVE
	A3DVECTOR3 vClipVel;  //velolcity after clipping
#else
	
#endif
	float fSpeed;
	// time span ( sec )
	float t;					
	// Gravity acceleration
	float fGravityAccel;
	// the Climb Slope Thresh
	float fSlopeThresh;
	// Tangent plane Info
	A3DVECTOR3 vTPNormal;
	// Absolute Velocity: output for forecast!
	A3DVECTOR3 vAbsVelocity;
	//the moving dist
	float     fMoveDist; 	
	//block by obstacle
	bool      bBlocked; 
	//on the surface
	bool      bOnSurface;
	//can stay
	bool      bCanStay;
	//	Collide something
	unsigned long     dwCollideFlag;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// After the move action is done, If the fHeightThresh 
	// still be satisfied, bMeetHeightThresh is set to true.
	bool bMeetHeightThresh;
	// Hold a height from the surface of terrain or building
	float	fHeightThresh;
	//	For air move
	float	fAccDir;
	//	For air move in the vertical direction
	float	fAccVertDir;
	// On air or water, true if on air, false for on water case.
	bool	bOnAir;

	//  Current roll for glide fly
	float	fCurRoll;
	//	Destination
	float	fDestRoll;
	//	The dirction of lift
	A3DVECTOR3 vUp;

	//	For water move
	// Distance Thresh under the water surface
	float fUnderWaterDistThresh;

	//param out
	void* pUser;//sinked in brush
};

//	VertAABBTrace/VertCapsuleTrace output value
struct CDR_TRACE_OUTPUT
{
	A3DVECTOR3	vHitPos;
	A3DVECTOR3	vHitNormal;
	float		fFaction;		//	< 0.0f: invalid value
	int			iUserData1;
	int			iUserData2;

	CDR_TRACE_OUTPUT::CDR_TRACE_OUTPUT() :
	vHitPos(0.0f),
	vHitNormal(0.0f)
	{
		fFaction	= -1.0f;
		iUserData1	= 0;
		iUserData2	= 0;
	}
};

struct BrushCollideParam
{
	A3DVECTOR3 vStart;  // brush start
	A3DVECTOR3 vExt;
	A3DVECTOR3 vDelta;
	
	A3DVECTOR3  vCapsuleUp;//capsule up direction
	float fCapsuleR;//capsule radius
	float fCapsuleHei;//capsule length(not include cap)
	float fCapsuleCenterHei;//capsule center to bottom
	
	float		fFraction;
	A3DVECTOR3	vHitNormal;
	bool		bStartSolid;     //start in solid
	
	int			iUserData1;		//	User data 1
	int			iUserData2;		//	User data 2
	void*       pSinkedBrush;   //pointer to sinked in brush 
	BrushCollideParam():vStart(0.f), vExt(0.f), vDelta(0.f), vCapsuleUp(0, 1, 0), fCapsuleR(0.f), 
		fCapsuleHei(0.f), fFraction(1.f), vHitNormal(0.f), bStartSolid(false), iUserData1(0), iUserData2(0), pSinkedBrush(NULL)
	{
	}
};


void OnGroundMove(CDR_INFO& CDRInfo);
void OnGlideFly(CDR_INFO& CDRInfo);
void OnAirMove(CDR_INFO& OnAirCDRInfo);

// Trace the building and the ground
bool VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY=1000.0f);
bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction);
bool RetrievePlane(BrushCollideParam *pParam, A3DVECTOR3& vHitPos);

bool VertTrace(BrushCollideParam *pParam, CDR_TRACE_OUTPUT& Out, float DeltaY=100.0f);

// only trace the building
bool RayTraceEnv(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction );
bool VertRayTraceEnv(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY=100.0f);

// test if the position lies in the valid submaps
bool IsPosInAvailableMap(const A3DVECTOR3& vPos);

