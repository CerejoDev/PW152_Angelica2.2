 /*
 * FILE: EC_CDR.cpp
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


//#define  CDR_DEBUG

#include "ECCDR.h"

#include "Collision.h"

#include "A3DFuncs.h"
#include "A3DTerrain2.h"
#include <A3DCollision.h>

#include "Game.h"

#ifdef CDR_DEBUG
#include <A3DEngine.h>
#include <A3DFont.h>

char    msg[200];
#define OUTPUT_DEBUG_INFO(s)   OutputDebugStringA(s)
#endif


using namespace CHBasedCD;

#define SLOPE_Y_THRESH_30DEGREE 0.866f
#define SLOPE_THRESH	0.5f		// 60 degree

#define FLY_MAX_HEIGHT	800.0f		// 飞行的最大高度！




#define	 VEL_EPSILON     1E-4f	
#define  NORMAL_EPSILON	  1e-2f
#define  DIST_EPSILON    1e-4f
#define  VEL_MAX_SPEED		50.0f
#define  VEL_MAXY_AIRSWIM   3.0f

//#define  CDR_ACCUM_DIST

struct move_t
{
	A3DVECTOR3 start;
	A3DVECTOR3 ext;

	//capsule info(yx)
	A3DVECTOR3  vCapsuleUp;//capsule up direction
	float fCapsuleR;//capsule radius
	float fCapsuleHei;//capsule length(not include cap)
	float fCapsuleCenterHei;//capsule center to bottom

	//added by yx,防止特别短的陡坡也上不去，例如稍微倾斜的楼梯
	float fSlopMaxH;//max length(in horizon) steep slop can stride over
	bool bShortSlop;//which can be stride over
	bool bTryStepUp;//try to step up

	A3DVECTOR3 velocity;
	float      t;
	float      slope;
	A3DVECTOR3  tpnormal;
	A3DVECTOR3  wishdir;
	float       wishspd;
	A3DVECTOR3  absvelocity;
	float		maxfallspeed;
	float		underwaterdistthresh;
	DWORD		collideflag;	
	float       accel;
	
	A3DVECTOR3 end;

	DWORD dwUser1;
	DWORD dwUser2;
};

//ref hl2
void FullGroundMove(move_t& mv, float gravity, float step);
void WalkMove(move_t& mv, float step);
void JumpFallMove(move_t& mv);
void ClipVelocity(const A3DVECTOR3& in, const A3DVECTOR3& normal, A3DVECTOR3& out, float overbounce, float fWishSpeedH);
int  TrySlideMove(move_t& mv);
void StepUp(move_t& mv, float step);
void Accelerate(const A3DVECTOR3& wishdir, float wishspeed, float accel, float t,  A3DVECTOR3& vel);
void AccelerateAir(const A3DVECTOR3& wishdir, float wishspeed, float accel, float vaccel, float t,  A3DVECTOR3& vel,A3DVECTOR3& newdir);


//////////////////////////////////////////////////////////////////////////
// Note by Niuyadong, 2010-05-07
//		This function is only for the Big world but not applicable for the 
//		Instance world!
//
//////////////////////////////////////////////////////////////////////////



bool RayTraceEnv(
						  const A3DVECTOR3& vStart, 
						  const A3DVECTOR3& vDelta,
						  A3DVECTOR3& vHitPos,
						  A3DVECTOR3& vNormal,
						  float& fraction
						  )
{
	return GetCollisionSupplier()->RayTraceEnv(vStart, vDelta, vHitPos, vNormal, fraction);
}

// 向下 Trace 地形和建筑，并返回第一个碰撞点的情况
bool VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY )
{
	return GetCollisionSupplier()->VertRayTrace(vPos, vHitPos, vHitNormal, DeltaY);		
}

// 仅仅向下 Trace 建筑
bool VertRayTraceEnv(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY)
{
	float fraction;
	return GetCollisionSupplier()->RayTraceEnv(vPos,A3DVECTOR3(0, -DeltaY, 0),vHitPos,vHitNormal,fraction);
}

inline bool CollideWithBrush(BrushCollideParam *pParam)
{
	return GetCollisionSupplier()->CollideWithBrush(pParam);
}

/*
 *
 * @desc :
 * @param  bWaterSolid:   true, treat water and water plane as a solid; 
						  otherwise treat air and water plane as a solid.
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/10/2005]
 * @ref:
 */
inline bool CollideWithWater(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, bool bWaterSolid, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart )
{
	return GetCollisionSupplier()->CollideWithWater(vStart, vDelta, bWaterSolid, fFraction, vHitNormal, bStart);
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [9/10/2005]
 * @ref:
 */ 
inline bool CollideWithTerrain(const A3DVECTOR3& vStart,const A3DVECTOR3& vCenter, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart)
{
	return GetCollisionSupplier()->CollideWithTerrain(vStart, vDelta, fFraction, vHitNormal, bStart);
}
/*
 * collision detect with environment, including brush, terrain and water
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [8/10/2005]
 * @ref:
 */
inline bool CollideWithEnv(env_trace_t * pEnvTrc)
{
	return GetCollisionSupplier()->CollideWithEnv(pEnvTrc);
}


/*
 *
 * @desc :
 * @param :     
 * @return : false if some error occurs during the trace, e.g. start in solid...
 * @note:  ASSUME that ext is positive!!!
 * @todo:   
 * @author: kuiwu [12/9/2005]
 * @ref:
 */
inline bool  RetrieveSupportPlane(ground_trace_t * pTrc)
{
	return GetCollisionSupplier()->RetrieveSupportPlane(pTrc);	
}

/*
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author:  [3/26/2009 Niuyadong]
 * @ref: RetrievePlane
 */
//should convert to capsule trace(yx)
bool RetrievePlane(BrushCollideParam *pParam, A3DVECTOR3& vHitPos)
{
	return GetCollisionSupplier()->RetrievePlane(pParam, vHitPos);
}

bool VertTrace(BrushCollideParam *pParam, CDR_TRACE_OUTPUT& Out, float DeltaY)
{
	return GetCollisionSupplier()->VertTrace(pParam, &Out, DeltaY);
}

inline bool IsPosInAvailableMap(const A3DVECTOR3& vPos)
{
	return GetCollisionSupplier()->IsPosInAvailableMap(vPos);
}

/*
 * add terrain-collision detection 
 * @desc :
 * @param :     
 * @return :
 * @note: 
 * @todo:  
 * @author: NiuYaDong[2/13/2009]
 * @history : Add moving dist. By NiuYaDong[2/13/2009]
 * @ref: 
 */
void OnGroundMove(CDR_INFO& CDRInfo)
{
   move_t  mv;
   mv.start = CDRInfo.vCenter;
   mv.ext   = CDRInfo.vExtent;
   mv.tpnormal = CDRInfo.vTPNormal;
   mv.slope    = CDRInfo.fSlopeThresh;
   mv.t        = CDRInfo.t;
   mv.wishdir  = CDRInfo.vVelDir;
   mv.wishspd  = CDRInfo.fSpeed;
   //mv.accel    = 1.5f ;  //tune it. 
   //mv.accel    = 2.0f ;  //tune it. 
   //mv.accel    = 3.0f ;  //tune it. 
   mv.accel    = 5.0f ;  //tune it. 
   //还需要取上一次移动的速度
   mv.velocity = CDRInfo.vClipVel;
   mv.maxfallspeed = CDRInfo.fMaxFallSpeed;

   //capsule info
   mv.fCapsuleHei = CDRInfo.fCapsuleHei;
   mv.fCapsuleR = CDRInfo.fCapsuleR;
   mv.fCapsuleCenterHei = CDRInfo.fCapsuleCenterHei;
   mv.vCapsuleUp = CDRInfo.vCapsuleUp;

   mv.fSlopMaxH = CDRInfo.fSlopMaxH;
   mv.bShortSlop = CDRInfo.bShortSlop;//可以跨过的陡坡
   mv.bTryStepUp = false;

   mv.dwUser1 = mv.dwUser2 = 0;

   FullGroundMove(mv,  CDRInfo.fGravityAccel, CDRInfo.fStepHeight);
	CDRInfo.vClipVel = mv.velocity;
    CDRInfo.fMoveDist = Magnitude(mv.end - CDRInfo.vCenter);
	CDRInfo.bBlocked =  (CDRInfo.fMoveDist < 1e-4f);
	if (CDRInfo.bBlocked)
	{
		//CDRInfo.vClipVel.Clear();
	}
	
	CDRInfo.vCenter  = mv.end;
	CDRInfo.vTPNormal = mv.tpnormal;
	CDRInfo.vAbsVelocity = mv.absvelocity;

	CDRInfo.bShortSlop = mv.bShortSlop;

	CDRInfo.bCanStay = CDRInfo.vTPNormal.y >= CDRInfo.fSlopeThresh;

	CDRInfo.bOnSurface = !(CDRInfo.vTPNormal.MaxMember() < NORMAL_EPSILON 
						 && CDRInfo.vTPNormal.MinMember() > -NORMAL_EPSILON);

	CDRInfo.dwUser = mv.dwUser2;
		
}


//void OnGroundMove(CDR_INFO& CDRInfo)
//{
//
//	
//	//OnGroundMoveBigAABB(CDRInfo);
//	OnGroundMoveBigAABB2(CDRInfo);
//}



/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: NiuYaDong [2/23/2009]
 * @ref:
 */
void AirMove(CDR_INFO& awmInfo)
{
#define  MAX_TRY         4
#define  VEL_REFLECT     0.0f
	if (awmInfo.fSpeed * awmInfo.t < DIST_EPSILON)
	{
		return;
	}
	move_t  mv;
	mv.start = awmInfo.vCenter;
	mv.ext   = awmInfo.vExtent;
	mv.tpnormal = awmInfo.vTPNormal;
	mv.slope    = awmInfo.fSlopeThresh;
	mv.t        = awmInfo.t;
	mv.wishdir  = awmInfo.vVelDir;
	mv.wishspd  = awmInfo.fSpeed; 
	mv.accel    = awmInfo.fAccDir ;  //tune it. 
	mv.velocity = awmInfo.vClipVel;

	//capsule info
	mv.fCapsuleCenterHei = awmInfo.fCapsuleCenterHei;
	mv.fCapsuleHei = awmInfo.fCapsuleHei;
	mv.fCapsuleR = awmInfo.fCapsuleR;
	mv.vCapsuleUp = awmInfo.vCapsuleUp;

	A3DVECTOR3 origin(mv.start), originvel(mv.velocity);
	A3DVECTOR3 wishdir;	
	float  wishspeed = mv.wishspd;
	// Set pmove velocity
 	AccelerateAir(mv.wishdir, wishspeed, mv.accel, awmInfo.fAccVertDir, mv.t, mv.velocity, wishdir);
	if (mv.velocity.y>VEL_MAXY_AIRSWIM)
	{
		mv.velocity.y = VEL_MAXY_AIRSWIM;
	}
	
	mv.wishdir = wishdir;
	mv.absvelocity = mv.velocity;
	A3DVECTOR3 vDelta(mv.velocity * mv.t);
	
	/*if (mv.velocity.SquaredMagnitude() < 1E-4f)
	{
		mv.velocity.Clear();
		mv.end = mv.start;
	}
	else
	{
		// first try just moving to the destination	
		env_trace_t tmpInfo;
		tmpInfo.vExt = mv.ext;
		tmpInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
		tmpInfo.vStart = mv.start;
		tmpInfo.vDelta = mv.velocity * mv.t;
		tmpInfo.vTerStart = tmpInfo.vStart;
		tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
		bool bClear = !CollideWithEnv(&tmpInfo);

		if (tmpInfo.bStartSolid)
		{
			mv.velocity.Clear();
			mv.end = origin;
		}
		else
		{
			// If we made it all the way, then copy trace end
			//  as new player position.
			if (bClear)
			{
				mv.end = tmpInfo.vStart + tmpInfo.vDelta;
			}
			else
			{
				TrySlideMove(mv);  //down slide move
			}
		}
	}*/
	
	int nTry = 0;
	bool bClear = true;
	env_trace_t  trcInfo;
	trcInfo.bWaterSolid = true;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	trcInfo.vExt = mv.ext;
	trcInfo.fCapsuleHei = mv.fCapsuleHei;
	trcInfo.fCapsuleR = mv.fCapsuleR;
	trcInfo.vCapsuleUp = mv.vCapsuleUp;

	float fTime = mv.t;
	float dtp;
	
	if (vDelta.Magnitude() < 1E-4f)
	{
		//mv.velocity.Clear();
		mv.end = mv.start;
	}
	else
	{
		// first try just moving to the destination	
		//env_trace_t tmpInfo;
		trcInfo.vExt = mv.ext;
		trcInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
		trcInfo.vStart = mv.start;
		trcInfo.vDelta = vDelta;
		trcInfo.vTerStart = trcInfo.vStart;
		//trcInfo.vTerStart.y -= trcInfo.vExt.y;
		trcInfo.vTerStart.y -= mv.fCapsuleCenterHei;
		bool bClear = !CollideWithEnv(&trcInfo);
		
		if (trcInfo.bStartSolid)
		{
			mv.velocity.Clear();
			mv.end = origin;
		}
		else
		{
			// If we made it all the way, then copy trace end
			//  as new player position.
			if (bClear)
			{
				mv.end = trcInfo.vStart + trcInfo.vDelta;
			}
			else
			{		
				A3DVECTOR3 vInitVelocity = mv.velocity;
				while (nTry < MAX_TRY) 
				{
					if (vDelta.Magnitude() < DIST_EPSILON ) 
					{			
						break;
					}

					if (DotProduct(vDelta, vInitVelocity) <= 0)
						break;
					
					trcInfo.vStart = mv.start;
					trcInfo.vDelta = vDelta;
					trcInfo.vTerStart = mv.start;
					//trcInfo.vTerStart.y -= mv.ext.y;
					trcInfo.vTerStart.y -= mv.fCapsuleCenterHei;
					trcInfo.vWatStart = mv.start;
					//trcInfo.vWatStart.y -= mv.ext.y;
					trcInfo.vWatStart.y -= mv.fCapsuleCenterHei;
					bClear = !CollideWithEnv(&trcInfo);	
					
					++nTry;
					if (bClear || trcInfo.bStartSolid) 
					{
						mv.end = mv.start + vDelta;
						break;
					}
					mv.start  += vDelta*trcInfo.fFraction;
					mv.end = mv.start;
					fTime -= fTime*trcInfo.fFraction;
					mv.tpnormal = trcInfo.vHitNormal;
					mv.wishspd = Normalize(mv.velocity, mv.wishdir);
					mv.wishspd *= (1- nTry * 0.1f);
					dtp = DotProduct(mv.tpnormal, mv.wishdir);
					mv.velocity = (mv.wishdir - mv.tpnormal * dtp - mv.tpnormal*dtp * VEL_REFLECT) * mv.wishspd; 
					vDelta = mv.velocity * fTime;
					
				}
				mv.velocity = mv.absvelocity;
			}
		}
	}

	if (!IsPosInAvailableMap(mv.end))
	{
		mv.end = origin;
		mv.velocity = originvel;
		return;
	}
	
	//see if meet height thresh
	ground_trace_t groundTrc;
	groundTrc.vStart = mv.end;
	groundTrc.vExt = mv.ext;
	groundTrc.fCapsuleCenterHei = mv.fCapsuleCenterHei;
	groundTrc.fCapsuleHei = mv.fCapsuleHei;
	groundTrc.fCapsuleR = mv.fCapsuleR;
	groundTrc.vCapsuleUp = mv.vCapsuleUp;

	groundTrc.fDeltaY = awmInfo.fHeightThresh + 0.1f;
	awmInfo.bMeetHeightThresh = false;

	if (!RetrieveSupportPlane(&groundTrc))
	{
		awmInfo.vClipVel = originvel;
		awmInfo.vCenter  = origin;
		return;
		
	}
	else
	{
		A3DVECTOR3 vTpNormal(0.0f);
		A3DVECTOR3 vOverTp(mv.end);
		bool bAdjust = false;
		float fHWater = GetCollisionSupplier()->GetWaterHeight(mv.end);
		if (groundTrc.bSupport)
		{
			bAdjust = true;
			vOverTp = groundTrc.vEnd;
			vTpNormal = groundTrc.vHitNormal;
			if (fHWater > groundTrc.vEnd.y)
			{
				vOverTp.y = fHWater;
				vTpNormal = A3DVECTOR3(0, 1, 0);
			}
		}
		else if (mv.end.y < fHWater + awmInfo.fHeightThresh )
		{
			bAdjust = true;
			vOverTp = mv.end;
			vOverTp.y = fHWater;
			vTpNormal = A3DVECTOR3(0, 1, 0);
		}
		
		if (bAdjust &&  (vOverTp.y + awmInfo.fHeightThresh > mv.end.y) )
		{
			//BrushTraceInfo  brushTrc;
			vDelta.Clear();
			vDelta.y = vOverTp.y + awmInfo.fHeightThresh - mv.end.y;
			float fAllow = (float)fabs(awmInfo.vCenter.y - mv.end.y)+ 0.001f;
			fAllow = a_Max(fAllow, mv.t);
			a_ClampRoof(vDelta.y, fAllow);

			BrushCollideParam para;
			para.vStart = mv.end;
			para.vDelta = vDelta;
			para.fCapsuleCenterHei = mv.fCapsuleCenterHei;
			para.fCapsuleHei = mv.fCapsuleHei;
			para.fCapsuleR = mv.fCapsuleR;
			para.vCapsuleUp = mv.vCapsuleUp;
			if (CollideWithBrush(&para))
			{
				mv.end += (vDelta * para.fFraction);
			}
			else
			{
				mv.end += vDelta;
			}
			//awmInfo.bMeetHeightThresh = (mv.end.y - vOverTp.y > awmInfo.fHeightThresh);
			awmInfo.bMeetHeightThresh = true;
		}
	}
	awmInfo.fMoveDist = Magnitude(mv.end - awmInfo.vCenter);
	awmInfo.bBlocked =  (awmInfo.fMoveDist < DIST_EPSILON);
	if (awmInfo.bBlocked)
	{
		awmInfo.vClipVel.Clear();
	}
	awmInfo.vClipVel = mv.velocity;
	awmInfo.vCenter  = mv.end;
	awmInfo.vTPNormal = mv.tpnormal;	
	awmInfo.vAbsVelocity = mv.absvelocity;
#undef  MAX_TRY
#undef  VEL_REFLECT

}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: NiuYaDong [3/2/2009]
 * @ref:
 */
void WaterMove(CDR_INFO& awmInfo)
{
#define  MAX_TRY         4
#define  VEL_REFLECT     0.0f
	if (awmInfo.fSpeed * awmInfo.t < DIST_EPSILON)
	{
		return;
	}
	move_t  mv;
	mv.start = awmInfo.vCenter;
	mv.end	 = mv.start;
	mv.ext   = awmInfo.vExtent;
	mv.tpnormal = awmInfo.vTPNormal;
	mv.slope    = awmInfo.fSlopeThresh;
	mv.t        = awmInfo.t;
	mv.wishdir  = awmInfo.vVelDir;
	mv.wishspd  = awmInfo.fSpeed; 
	mv.accel    = awmInfo.fAccDir ;  //tune it. 
	mv.velocity = awmInfo.vClipVel;
	mv.underwaterdistthresh = awmInfo.fUnderWaterDistThresh;
	//mv.checkflag= CDR_BRUSH | CDR_TERRAIN | CDR_WATER;

	//capsule info
	mv.fCapsuleR = awmInfo.fCapsuleR;
	mv.fCapsuleHei = awmInfo.fCapsuleHei;
	mv.fCapsuleCenterHei = awmInfo.fCapsuleCenterHei;
	mv.vCapsuleUp = awmInfo.vCapsuleUp;

	A3DVECTOR3 origin(mv.start), originvel(mv.velocity);
	A3DVECTOR3 wishdir;
	float  wishspeed = mv.wishspd;
	// Set pmove velocity
	AccelerateAir(mv.wishdir, wishspeed, mv.accel, awmInfo.fAccVertDir, mv.t, mv.velocity, wishdir);
	if (mv.velocity.y>VEL_MAXY_AIRSWIM)
	{
		mv.velocity.y = VEL_MAXY_AIRSWIM;
	}
	mv.absvelocity = mv.velocity;	
		
	A3DVECTOR3 vVelDir, vVelocity(mv.velocity), vDelta(mv.velocity * mv.t), vNormal;
	float fSpeed,time_left = mv.t;
	int nTry = 0;
	bool bClear = true;
	env_trace_t  trcInfo;
	trcInfo.bWaterSolid = false;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH | CDR_WATER ;
	trcInfo.vExt = mv.ext;

	trcInfo.fCapsuleHei = mv.fCapsuleHei;
	trcInfo.fCapsuleR = mv.fCapsuleR;
	trcInfo.vCapsuleUp = mv.vCapsuleUp;
	
	A3DVECTOR3 vInitVelocity = mv.velocity;
	while (nTry < MAX_TRY) 
	{
		if (vDelta.Magnitude() < DIST_EPSILON ) 
		{
			break;
		}

		if (DotProduct(vDelta, vInitVelocity) <= 0)
			break;

		trcInfo.vStart = mv.start;
		trcInfo.vDelta = vDelta;
		trcInfo.vTerStart = mv.start;
		//trcInfo.vTerStart.y -= mv.ext.y;
		trcInfo.vTerStart.y -= mv.fCapsuleCenterHei;
		trcInfo.vWatStart = mv.start;
		trcInfo.vWatStart.y += mv.underwaterdistthresh; //shoulder


		bClear = !CollideWithEnv(&trcInfo);	


		++nTry;
		if (bClear || (trcInfo.bStartSolid && ((trcInfo.dwClsFlag & CDR_WATER)!= CDR_WATER)) )
		{
			
			mv.end = mv.start + vDelta;
			break;
		}
		

		fSpeed = Normalize(vVelocity, vVelDir);
		
		if ((trcInfo.dwClsFlag & CDR_WATER) == CDR_WATER)
		{
			vVelDir.y = 0.0f;
			trcInfo.fFraction = 0.0f;// 目的是 改变方向 再Try一次
			
		}
		else
		{
			fSpeed *= (1- nTry * 0.1f);
			float dtp = DotProduct(trcInfo.vHitNormal, vVelDir);
			vVelDir = (vVelDir - trcInfo.vHitNormal * dtp * (1.0f + VEL_REFLECT)) ; 

		}

		//	对水面只碰撞一次 专门防止浅滩水岸和水面夹角的地方造成Try四次也不动的问题
		trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
		vVelocity = vVelDir * fSpeed;
		vDelta = vVelocity * time_left;
		mv.start += vDelta*trcInfo.fFraction;
		time_left -= time_left*trcInfo.fFraction;
		mv.end = mv.start;

	}
	mv.velocity = mv.absvelocity;

	if (!IsPosInAvailableMap(mv.end))
	{
		awmInfo.vClipVel = originvel;
		awmInfo.vCenter  = origin;
		return;
	}
	
	//see if meet height thresh
	float fHWater = GetCollisionSupplier()->GetWaterHeight(mv.end);
	float fAllow = fHWater - awmInfo.fUnderWaterDistThresh - mv.end.y-0.005f;// For change to GroundEnv
	ground_trace_t groundTrc;
	groundTrc.vStart = mv.end;
	groundTrc.vExt = mv.ext;
	float fHeightThresh;
	if (fAllow > awmInfo.fUnderWaterDistThresh)	
		fHeightThresh = awmInfo.fHeightThresh;	//	深水就用水中的距离方式
	else
		fHeightThresh = 0.0f;//	浅滩要和陆地的方法一致
	

	groundTrc.fDeltaY = fHeightThresh + 0.08f;
	groundTrc.fCapsuleCenterHei = mv.fCapsuleCenterHei;
	groundTrc.fCapsuleHei = mv.fCapsuleHei;
	groundTrc.fCapsuleR = mv.fCapsuleR;
	groundTrc.vCapsuleUp = mv.vCapsuleUp;

   	if (!RetrieveSupportPlane(&groundTrc))
	{
		awmInfo.vClipVel = originvel;
		awmInfo.vCenter  = origin;
		return;
	}
	else
	{
		A3DVECTOR3 vTpNormal(0.0f);
		A3DVECTOR3 vOverTp(mv.end);
		//A3DVECTOR3 vDelta(0.0f);
		vDelta.Clear();
		awmInfo.bMeetHeightThresh = false;
		if (groundTrc.bSupport)
		{
			mv.tpnormal = groundTrc.vHitNormal;
			if(groundTrc.vEnd.y + fHeightThresh + 0.08f > mv.end.y)
			{	
				if (fHeightThresh<1E-4f)// 浅滩和地面处理一致
				{
					if (mv.tpnormal.y > mv.slope)
					{
						mv.end = groundTrc.vEnd;// Leave water in here
					}
					else
					{
						// Can not leave water ,in a huge slope
					}
					
				}
				else
				{
					vDelta.y = groundTrc.vEnd.y + fHeightThresh+ 0.08f - mv.end.y;
					//fAllow = (float)fabs(awmInfo.vCenter.y - mv.end.y)+ 0.001f;
					fAllow = a_Min(fAllow, 0.0008f);
					a_ClampRoof(vDelta.y, fAllow);
											
					BrushCollideParam para;
					para.vStart = mv.end;
					para.vDelta = vDelta;
					para.fCapsuleCenterHei = mv.fCapsuleCenterHei;
					para.fCapsuleHei = mv.fCapsuleHei;
					para.fCapsuleR = mv.fCapsuleR;
					para.vCapsuleUp = mv.vCapsuleUp;
					
					if (CollideWithBrush(&para))
					{
						mv.end += (vDelta * para.fFraction);
					}
					else
					{
						mv.end += vDelta;
					}
				}
					
				awmInfo.bMeetHeightThresh = (mv.end.y - groundTrc.vEnd.y > fHeightThresh);
			}
			
			
		}
	}
	
//	mv.tpnormal = A3DVECTOR3(0, 1, 0);
	

	awmInfo.fMoveDist = Magnitude(mv.end - awmInfo.vCenter);
	awmInfo.bBlocked =  (awmInfo.fMoveDist < DIST_EPSILON);
	if (awmInfo.bBlocked)
	{
		awmInfo.vClipVel.Clear();
	}

	awmInfo.vClipVel = mv.velocity;
	awmInfo.vCenter  = mv.end;
	awmInfo.vTPNormal = mv.tpnormal;	
	awmInfo.vAbsVelocity = mv.absvelocity;


#undef  MAX_TRY
#undef  VEL_REFLECT

}
/*
 * 
 * @desc : refactor version of OnAirMoveBigAABB, add collision detect with water & terrain
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [30/9/2005]
 * @ref:
 */
void OnAirMove(CDR_INFO& awmInfo)
{
	//assert(0 && "Not ready yet");

	if (awmInfo.bOnAir)
	{
		AirMove(awmInfo);
	}
	else
	{
		WaterMove(awmInfo);
	}
}


bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction)
{
	return GetCollisionSupplier()->RayTrace(vStart, vDelta, vHitPos, vNormal, fraction);
}


void Accelerate(const A3DVECTOR3& wishdir, float wishspeed, float accel, float t,  A3DVECTOR3& vel)
{
	float addspeed, accelspeed, currentspeed;

	// See if we are changing direction a bit
	currentspeed = DotProduct(vel, wishdir);

	// Reduce wishspeed by the amount of veer.
	addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
	{
		vel = wishdir * wishspeed;  //不要超过wishspeed
		return;
	}

	// Determine amount of acceleration.
	accelspeed = accel * t * wishspeed;

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	vel = wishdir * (currentspeed + accelspeed);
	//vel +=  wishdir * accelspeed;
}

void AccelerateAir(const A3DVECTOR3& wishdir, float wishspeed, float accel, float vaccel, float t,  A3DVECTOR3& vel,A3DVECTOR3& newdir)
{
	A3DVECTOR3 vdelta = wishdir*accel+A3DVECTOR3(0, 1, 0) * vaccel;
	float acc = Normalize( vdelta, newdir);
	float addspeed, accelspeed, currentspeed;
	
	// See if we are changing direction a bit
	currentspeed = DotProduct(vel, newdir);
	
	// Reduce wishspeed by the amount of veer.
	addspeed = wishspeed - currentspeed;
	
	// If not going to add any speed, done.
	if (addspeed <= 0)
	{
		vel = newdir * wishspeed;  //不要超过wishspeed
		return;
	}
	
	// Determine amount of acceleration.
	accelspeed = acc * t * wishspeed;
	
	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;
	
	// Adjust velocity.
	vel = newdir * (currentspeed + accelspeed);
	//vel +=  wishdir * accelspeed;
}

//跨短陡坡状态
bool IsOnShortSlop(const move_t& mv, float step)
{
	A3DVECTOR3 vDirH(mv.velocity);
	vDirH.y = 0;
	if (DotProduct(vDirH, mv.tpnormal) >= 0 || vDirH.SquaredMagnitude() < 1E-4f)
	{
		return false;
	}

	env_trace_t tmpInfo;
	A3DVECTOR3 StepPos(mv.end);

	//step up
	tmpInfo.vExt = mv.ext;
	tmpInfo.fCapsuleR = mv.fCapsuleR;
	tmpInfo.fCapsuleHei = mv.fCapsuleHei;
	tmpInfo.vCapsuleUp = mv.vCapsuleUp;

	tmpInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
	tmpInfo.vStart = mv.end;
	tmpInfo.vDelta = A3DVECTOR3(0, step, 0);
	tmpInfo.vTerStart = tmpInfo.vStart;
	//tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
	tmpInfo.vTerStart.y -= mv.fCapsuleCenterHei;
	bool bClear = !CollideWithEnv(&tmpInfo);

	if (bClear)
		StepPos += tmpInfo.vDelta;
	else
		StepPos += tmpInfo.fFraction * tmpInfo.vDelta;

	//step forward
	
	vDirH.Normalize();

	tmpInfo.vStart = StepPos;
	tmpInfo.vDelta = vDirH * mv.fSlopMaxH;
	tmpInfo.vTerStart = tmpInfo.vStart;
	//tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
	tmpInfo.vTerStart.y -= mv.fCapsuleCenterHei;
	bClear = !CollideWithEnv(&tmpInfo);
	if (bClear)
		StepPos += tmpInfo.vDelta;
	else
		StepPos += tmpInfo.vDelta * tmpInfo.fFraction;

	//step down
	tmpInfo.vStart = StepPos;
	tmpInfo.vDelta = A3DVECTOR3(0, -step, 0);
	tmpInfo.vTerStart = tmpInfo.vStart;
	//tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
	tmpInfo.vTerStart.y -= mv.fCapsuleCenterHei;
	bClear = !CollideWithEnv(&tmpInfo);
	if (!bClear && tmpInfo.vHitNormal.y >= mv.slope)
		return true;
	return false;
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author NiuYaDong
 * \date 13/2/2009
 * \see 
 */
void FullGroundMove(move_t& mv, float gravity, float step)
{
	A3DVECTOR3 origin(mv.start), originvel(mv.velocity);
	mv.velocity.y -= gravity * mv.t* 0.5f;  // start 0.5 and end 0.5?

	//	Add by dyx
	if (mv.velocity.y < mv.maxfallspeed)
		mv.velocity.y = mv.maxfallspeed;
	
	// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
	//  we don't slow when standing still, relative to the conveyor.
	//if (player->GetGroundEntity() != NULL)
	if (mv.tpnormal.y > mv.slope)
	{
		//清除竖直方向速度
		mv.velocity.y = 0.0f;
	}


	// Make sure velocity is valid.
	//CheckVelocity(mv.velocity);
	
	if (mv.tpnormal.y > mv.slope)
	{
		mv.bShortSlop = false;
		WalkMove(mv,  step);
	}
	else if (mv.bShortSlop && !mv.wishdir.IsZero())
	{
		//在短陡短斜坡处，继续上斜坡（台阶）
		mv.velocity.y = 0.0f;
		A3DVECTOR3 wishdir(mv.wishdir);
		// Zero out y part of velocity
		wishdir.y = 0.0f;
		Accelerate(wishdir, mv.wishspd, mv.accel, mv.t, mv.velocity);
		mv.velocity.y = 0.0f;
		mv.absvelocity = mv.velocity;

		if (mv.velocity.SquaredMagnitude() < 1E-4f)
		{
			//mv.velocity.Clear();
			mv.end = mv.start;
			return;
		}
		StepUp(mv, step);
	}
	else
	{
		mv.bShortSlop = false;
		move_t OldMv = mv;
		JumpFallMove(mv);
		//added by yx, 存在支持面，又不能向下滑动时可以走动，防止卡在角落里既不能下滑又不能走动，只能跳
		if ((mv.end - origin).IsZero() && mv.tpnormal.y > 0)
		{
			mv = OldMv;
			WalkMove(mv, step);
		}
	}

	if (!IsPosInAvailableMap(mv.end))
	{
		mv.end = origin;
		mv.velocity = originvel;
		mv.bShortSlop = false;
		return;
	}

	//TODO: tune this value
	if (originvel.y  > 1.0f)
	{
		//jump 
		mv.tpnormal.Clear();
	}
	else
	{
		ground_trace_t groundTrc;
		groundTrc.vStart = mv.end;
		groundTrc.vExt = mv.ext;
		groundTrc.fDeltaY = 0.08f;
		groundTrc.fCapsuleCenterHei = mv.fCapsuleCenterHei;
		groundTrc.fCapsuleHei = mv.fCapsuleHei;
		groundTrc.fCapsuleR = mv.fCapsuleR;
		groundTrc.vCapsuleUp = mv.vCapsuleUp;

		//groundTrc.fBump = 0.001f;
		if (!RetrieveSupportPlane(&groundTrc))
		{
			mv.end = origin;
			mv.velocity = originvel;
			if (mv.tpnormal.y < mv.slope)
			{
				mv.tpnormal = A3DVECTOR3(0.0f, 1.0f, 0.0f);
			}
			mv.bShortSlop = false;
			mv.dwUser2 = (DWORD)groundTrc.iUserData2;
			return;  
		}		
		if (groundTrc.bSupport)
		{
			mv.end = groundTrc.vEnd;
			mv.tpnormal = groundTrc.vHitNormal;
			if (mv.tpnormal.y < mv.slope && !mv.bTryStepUp && IsOnShortSlop(mv, step))
			{
				mv.velocity = originvel;
				mv.bShortSlop = true;
			}
		}
		else
		{
			mv.tpnormal.Clear();
		}
		
	}

	mv.velocity.y -= gravity * mv.t* 0.5f;  // start 0.5 and end 0.5?
	if (mv.tpnormal.y > mv.slope)
	{
		mv.velocity.y = 0.0f;
	}
}

/**
 * \brief jump, fall and slide in steep slope.
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 8/6/2006
 * \see 
 */
void JumpFallMove(move_t& mv)
{
	A3DVECTOR3		wishdir;
	float		wishspeed;

	wishdir = mv.wishdir;
	wishspeed = mv.wishspd;
	assert(wishdir.y == 0.0f);
	mv.absvelocity = mv.velocity;

	TrySlideMove(mv);
}

/**
 * \brief walk move, tpnormal > slope
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 8/6/2006
 * \see 
 */
void WalkMove(move_t& mv, float step)
{

	A3DVECTOR3 wishdir(mv.wishdir);
	// Zero out y part of velocity
	wishdir.y = 0.0f;
	float  wishspeed = mv.wishspd;
	
// 	float max_wishspeed   = 5.0f;
// 	//
// 	// Clamp to server defined max speed
// 	//
// 	if (wishspeed > max_wishspeed)
// 	{
// 		wishspeed = max_wishspeed;
// 	}
	// Set pmove velocity

	mv.velocity.y = 0.0f; // Zero out y part of velocity
	Accelerate(wishdir, wishspeed, mv.accel, mv.t, mv.velocity);
	mv.velocity.y = 0.0f;
	mv.absvelocity = mv.velocity;

	if (mv.velocity.SquaredMagnitude() < 1E-4f)
	{
		//mv.velocity.Clear();
		mv.end = mv.start;
		return;
	}
	

	A3DVECTOR3 origin(mv.start);   //save original
	A3DVECTOR3 originvel(mv.velocity);
	

	// first try just moving to the destination	
	//note: 这里与hl2稍有不同，对velocity按当前tpnormal clip，避免额外的碰撞
	ClipVelocity(originvel, mv.tpnormal, mv.velocity, 1.01f, mv.wishspd);

	A3DVECTOR3 vDelta(mv.velocity * mv.t);
	if (vDelta.Magnitude() < 1E-4f)
	{
		//mv.velocity.Clear();
		mv.end = mv.start;
		return;
	}

	
	env_trace_t tmpInfo;
	tmpInfo.vExt = mv.ext;
	tmpInfo.fCapsuleR = mv.fCapsuleR;
	tmpInfo.fCapsuleHei = mv.fCapsuleHei;
	tmpInfo.vCapsuleUp = mv.vCapsuleUp;

	tmpInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
	tmpInfo.vStart = mv.start;
	tmpInfo.vDelta = mv.velocity * mv.t;
	tmpInfo.vTerStart = tmpInfo.vStart;
	//tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
	tmpInfo.vTerStart.y -= mv.fCapsuleCenterHei;
	bool bClear = !CollideWithEnv(&tmpInfo);

	if (tmpInfo.bStartSolid)
	{
		mv.velocity.Clear();
		mv.end = origin;
		return;
	}
	// If we made it all the way, then copy trace end
	//  as new player position.
	if (bClear)
	{
		mv.velocity = originvel;
		mv.end = tmpInfo.vStart + tmpInfo.vDelta;
		return;
	}


	// Try sliding forward both on ground and up,
	//  take the move that goes farthest
	A3DVECTOR3 down(origin), downvel(originvel);
	A3DVECTOR3 up(origin);
	//mv.velocity = originvel;  //需要恢复原速度吗？
	TrySlideMove(mv);  //down slide move
	
	// Copy the results out
	down = mv.end;
	downvel = mv.velocity;
    
	//ECHostPlayer* pHost = glb_GetHostPlayer();
	//PosPair PairBack = pHost->GetEndPosPair();
	
	// Reset original values.
	mv.start = origin;
	mv.velocity = originvel;
	StepUp(mv, step);   //up move
	
	up = mv.end;

	// decide which one went farther
	float downdist = (down.x - origin.x) * (down.x - origin.x)
				   + (down.z - origin.z) * (down.z - origin.z);
	float updist   = (up.x - origin.x) * (up.x - origin.x)
				   + (up.z - origin.z) * (up.z - origin.z);

	if (downdist > updist)
	{
		mv.end = down;
		mv.velocity = downvel;
		//pHost->SetEndPosPair(PairBack);
	}
	else
	{
		mv.end = up;
		mv.velocity.y = downvel.y;  // copy z value from slide move???
	}
	
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note 与hl2的做法不同，需要考虑dot(in, normal) > 0的情况
 * \warning
 * \todo   
 * \author NiuYaDong
 * \date 13/2/2009
 * \see 
 */
void ClipVelocity(const A3DVECTOR3& in, const A3DVECTOR3& normal, A3DVECTOR3& out, float overbounce, float fWishSpeedH)
{

	float	backoff;

#define DTP_EPSILON   0.001f

	a_Clamp(overbounce, 1.0f, 1.5f); 
	A3DVECTOR3 indir;
	float inspd;

	inspd = Normalize(in, indir);

	float dtp = DotProduct (indir, normal);

	if (dtp > 0 && dtp < DTP_EPSILON)
	{
		out = in;
		return;
	}
	
	out = in - normal * dtp * inspd;

	backoff = (float)(fabs(dtp) * (overbounce - 1.0f)*inspd);
	a_ClampFloor(backoff, DTP_EPSILON);
	out += normal*backoff;
	
	//	Added by dyx: clamp speed base on wished horizonal speed
	A3DVECTOR3 vVelH = out;
	vVelH.y = 0.0f;
	float fSpeedH = vVelH.Normalize();
	if (fSpeedH > 0.0f && fSpeedH > fWishSpeedH)
	{
		float f = fWishSpeedH / fSpeedH;

		vVelH *= fWishSpeedH;
		out.x = vVelH.x;
		out.z = vVelH.z;
		out.y = out.y * f;
	}


}


/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 6/6/2006
 * \see 
 */
int TrySlideMove(move_t& mv)
{
#define  MAX_TRY  4
	A3DVECTOR3 planes[MAX_TRY +1];
	int        numplanes;
	A3DVECTOR3 primal_velocity, original_velocity, new_velocity, original_pos;
	float      time_left, allFraction;
	int        nTry;
	env_trace_t  trcInfo;
	A3DVECTOR3   delta;
	bool         bClear ;

	trcInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;
	trcInfo.vExt = mv.ext;
	trcInfo.fCapsuleHei = mv.fCapsuleHei;
	trcInfo.fCapsuleR = mv.fCapsuleR;
	trcInfo.vCapsuleUp = mv.vCapsuleUp;

	primal_velocity = mv.velocity;
	original_velocity = mv.velocity;
	original_pos = mv.start;
	new_velocity.Clear();

	time_left =         mv.t;
	allFraction    = 0.0f;
	nTry   = 0;
	numplanes = 0;

	A3DVECTOR3 vInitVelocity = mv.velocity;
	for (nTry = 0; nTry < MAX_TRY; nTry++)
	{
		mv.end = mv.start;
		delta =  mv.velocity * time_left;
		
		float fDeltaDist = delta.Magnitude();
		if (fDeltaDist < 1E-4f ) 
		{
			//_OutputDebugInfo("no delta %f", fDeltaDist);
			break;
		}

		if (DotProduct(delta, vInitVelocity) <= 0)
			break;
		
		trcInfo.vStart = mv.start;
		trcInfo.vDelta = delta;
		trcInfo.vTerStart = mv.start;
		//trcInfo.vTerStart.y -= mv.ext.y; //foot
		trcInfo.vTerStart.y -= mv.fCapsuleCenterHei; //foot
		trcInfo.vWatStart = mv.start;
		trcInfo.vWatStart.y += mv.underwaterdistthresh; //shoulder

		bClear = !CollideWithEnv(&trcInfo);	
	
		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity 
		if (trcInfo.bStartSolid) 
		{
			// entity is trapped in another solid
			mv.end = original_pos;
			mv.velocity.Clear();
			return 0;
		}

		allFraction += trcInfo.fFraction;
		time_left -= time_left*trcInfo.fFraction;
		mv.start +=  delta * trcInfo.fFraction;
		mv.end    = mv.start;

		//moved some portion of the total distance
		if (trcInfo.fFraction > 1E-3)
		{
			original_velocity = mv.velocity;
			numplanes = 0;
		}

		
		// If we covered the entire distance, we are done
		//  and can return.
		if (trcInfo.fFraction == 1)
		{
			 break;		// moved the entire distance
		}

		planes[numplanes] = trcInfo.vHitNormal;
		++numplanes;

		// modify original_velocity so it parallels all of the clip planes
		//

		// relfect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		//if (numplanes == 1 && mv.tpnormal.IsZero())
		if (numplanes == 1 && mv.tpnormal.y < mv.slope)
		{
			if (planes[0].y > mv.slope)
			{
				//floor or slope
				//	ClipVelocity(original_velocity, planes[0], new_velocity, 1.0f);
					ClipVelocity(original_velocity, planes[0], new_velocity, 1.05f, mv.wishspd);
			}
			else
			{
				//陡坡
				ClipVelocity(original_velocity, planes[0], new_velocity, 1.08f, mv.wishspd);
				
			}
			mv.velocity = new_velocity;
			original_velocity = new_velocity;
		}
		else
		{
			int i;
			for (i=0 ; i < numplanes ; ++i)
			{
				//ClipVelocity (original_velocity, planes[i],	mv.velocity,	1.0f);
				ClipVelocity (original_velocity, planes[i],	mv.velocity, 1.05f, mv.wishspd);
				int j;
				for (j=0 ; j<numplanes ; ++j)
				{
					if (j != i)
					{
						// Are we now moving against this plane?
						if (DotProduct(planes[j], mv.velocity) < 0)
							break;	// not ok
					}
				}
				if (j == numplanes)  // Didn't have to clip, so we're ok
					break;
			}
			
			// Did we go all the way through plane set
			if (i != numplanes)
			{	// go along this plane
				// velocity is set in clipping call, no need to set again.
				;  
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
					//对于复杂的场景，crease有三个或三个以上的plane, 清除velocity,
					//会造成不能移动???
					mv.velocity.Clear();
					break;
				}
				//只有两个面，计算一个新的速度方向
				A3DVECTOR3 dir;
				dir =	CrossProduct(planes[0], planes[1]);
				//将原velocity投影到新方向
				float dtp = DotProduct(dir, mv.velocity);
				mv.velocity = dir * dtp;
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			//这样做有效防止爬到陡坡上吗？
			float dtp =  DotProduct(mv.velocity, primal_velocity);
			if (dtp <= 0)
			{
				mv.velocity.Clear();
				break;
			}
		}
		
	}
	

	

	if ( allFraction == 0 && nTry != 0 )
	{
		mv.velocity.Clear(); // useless ?!	
	}
	
	return 0;

#undef   MAX_TRY
}

/**
 * \brief try to  step up to stairs
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 8/6/2006
 * \see 
 */
void StepUp(move_t& mv, float step)
{

	A3DVECTOR3 origin = mv.start;  // save the input pos [6/8/2006 kuiwu]

	mv.bTryStepUp = true;

	env_trace_t tmpInfo;
	tmpInfo.vExt = mv.ext;
	tmpInfo.fCapsuleHei = mv.fCapsuleHei;
	tmpInfo.fCapsuleR = mv.fCapsuleR;
	tmpInfo.vCapsuleUp = mv.vCapsuleUp;
	//@note : need check terrain?? By Kuiwu[8/10/2005]
	tmpInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  

	//up
	tmpInfo.vStart = mv.start;
	tmpInfo.vDelta = A3DVECTOR3(0.0f, step, 0.0f);
	tmpInfo.vTerStart = tmpInfo.vStart;
	//tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
	tmpInfo.vTerStart.y -= mv.fCapsuleCenterHei;
	bool bClear = !CollideWithEnv(&tmpInfo);
	if (tmpInfo.bStartSolid)
	{
		mv.end = origin;
		return;
	}
	if (bClear)
	{
		mv.start += tmpInfo.vDelta;
	}
	else
	{
		mv.start +=  tmpInfo.vDelta*tmpInfo.fFraction;
	}

	A3DVECTOR3 StepPos = mv.start;//记录抬腿位置

	//forward
	TrySlideMove(mv);

	//down
	tmpInfo.vStart = mv.end;
	tmpInfo.vDelta = A3DVECTOR3(0.0f, -step, 0.0f);
	tmpInfo.vTerStart = tmpInfo.vStart;
	//tmpInfo.vTerStart.y -= tmpInfo.vExt.y;
	tmpInfo.vTerStart.y -= mv.fCapsuleCenterHei;
	bClear = !CollideWithEnv(&tmpInfo);
	if (tmpInfo.bStartSolid)
	{
		mv.end = origin;
		return;
	}

	if (bClear)
	{
		mv.end += tmpInfo.vDelta;
		return;
	}

	if (tmpInfo.vHitNormal.y < mv.slope)
	{
		//steep
		//向前试探是否是短陡坡（yx）
		A3DVECTOR3 velocityH(mv.velocity);
		velocityH.y = 0;
		if (velocityH.Normalize() > 1E-4f)
		{
			//forward
			env_trace_t SteepTraceInfo;
			SteepTraceInfo.vExt = mv.ext;
			SteepTraceInfo.fCapsuleHei = mv.fCapsuleHei;
			SteepTraceInfo.fCapsuleR = mv.fCapsuleR;
			SteepTraceInfo.vCapsuleUp = mv.vCapsuleUp;
			//@note : need check terrain?? By Kuiwu[8/10/2005]
			SteepTraceInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
			SteepTraceInfo.vStart = StepPos;
			SteepTraceInfo.vDelta = mv.fSlopMaxH * velocityH;
			SteepTraceInfo.vTerStart = SteepTraceInfo.vStart;
			SteepTraceInfo.vTerStart.y -= mv.fCapsuleCenterHei;

			bClear = !CollideWithEnv(&SteepTraceInfo);
			if (bClear)
				StepPos += SteepTraceInfo.vDelta;
			else
				StepPos += SteepTraceInfo.vDelta * SteepTraceInfo.fFraction;

			//down
			SteepTraceInfo.vStart = StepPos;
			SteepTraceInfo.vDelta = A3DVECTOR3(0.0f, -step, 0.0f);
			SteepTraceInfo.vTerStart = SteepTraceInfo.vStart;
			SteepTraceInfo.vTerStart.y -= mv.fCapsuleCenterHei;

			bClear = !CollideWithEnv(&SteepTraceInfo);
			if (!bClear && SteepTraceInfo.vHitNormal.y >= mv.slope)
			{
				mv.bShortSlop = true;
			}
			else
			{
				mv.end = origin;
				return;
			}

		}
		else
		{
			mv.end = origin;
			return;
		}		
	}
	
	mv.end += tmpInfo.vDelta * tmpInfo.fFraction;
}

/*
 * add fly-collision detection 
 * @desc : 
 * @param :     
 * @return :
 * @note: 
 * @todo:  
 * @author: NiuYaDong[3/19/2009]
 * @history : 
 * @ref: 
 */
void OnGlideFly(CDR_INFO& awmInfo)
{
#define  MAX_TRY         4
#define  VEL_REFLECT     0.0f
	move_t  mv;
	mv.start = awmInfo.vCenter;
	mv.ext   = awmInfo.vExtent;
	mv.tpnormal = awmInfo.vTPNormal;
	mv.slope    = awmInfo.fSlopeThresh;
	mv.t        = awmInfo.t;
	mv.wishdir  = awmInfo.vVelDir;
	mv.wishspd  = awmInfo.fSpeed; 
	mv.accel    = awmInfo.fAccDir ;  //tune it. 
	mv.collideflag = 0;

	//capsule info
	mv.fCapsuleCenterHei = awmInfo.fCapsuleCenterHei;
	mv.fCapsuleHei = awmInfo.fCapsuleHei;
	mv.fCapsuleR = awmInfo.fCapsuleR;
	mv.vCapsuleUp = awmInfo.vCapsuleUp;

	A3DVECTOR3 origin(mv.start), originvel(mv.velocity);
	A3DVECTOR3 wishdir(mv.wishdir);	
	float  wishspeed = mv.wishspd;
	// Set pmove velocity
	float fLK ;	//	升力系数
	float fRK = 0.3f;	//	阻力系数 F1赛车的系数是 0.3 以此为标准调整， 可以根据装备 头盔 等来调整
	A3DVECTOR3 vLift, vResistance, vGravity1, vGravity2, vGravity(0.0f,-9.8f,0.0f);	
	vGravity1 = mv.wishdir*DotProduct(vGravity,mv.wishdir);			// 重力在当前方向上的分量
	vGravity2 = awmInfo.vUp*DotProduct(vGravity,awmInfo.vUp);		// 重力在升力方向上的分量
		
 	if (awmInfo.fCurRoll<awmInfo.fDestRoll)
	{
		fLK = 1.5f;// 为1表示，没有空气阻力的时候可以爬升到起始点 (注：0.8，1.0，1.5，2.0 当值越大说明越慢的速度就能保持平飞了，能飞很远的距离，这个值最好能由装备属性来定)
	}
	else
		fLK = -1.0f;
	a_Clamp(fLK, -1.0f, 2.0f);
	vLift = awmInfo.vUp*mv.wishspd*fLK;// 升力总是垂直于当前方向
	if (vLift.Magnitude()>vGravity2.Magnitude()&&((awmInfo.fCurRoll-awmInfo.fDestRoll)<0.01f&&(awmInfo.fCurRoll-awmInfo.fDestRoll)>-0.01f))
	{
		awmInfo.fCurRoll = awmInfo.fDestRoll;
		vLift = -vGravity2;	
	}
	if (awmInfo.bMeetHeightThresh)
	{
		vGravity.Clear();
		vLift.Clear();
		fRK = 1.0f;
	}
	vResistance = -fRK*mv.wishspd*mv.wishdir;						// 阻力在当前方向上，反向
	mv.velocity = mv.wishdir*mv.wishspd+(vLift+vGravity+vResistance)*mv.t;
	mv.absvelocity = mv.velocity;
	mv.end =mv.start + mv.velocity*mv.t;
	mv.absvelocity = mv.velocity;
	A3DVECTOR3 vDelta(mv.velocity * mv.t);
	
	int nTry = 0;
	bool bClear = true;
	env_trace_t  trcInfo;
	trcInfo.bWaterSolid = true;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	trcInfo.vExt = mv.ext;
	trcInfo.fCapsuleR = mv.fCapsuleR;
	trcInfo.fCapsuleHei = mv.fCapsuleHei;
	trcInfo.vCapsuleUp = mv.vCapsuleUp;

	float fTime = mv.t;
	float dtp;

	if (vDelta.Magnitude() < 1E-4f)
	{
		mv.velocity.Clear();
		mv.end = mv.start;
	}
	else
	{
		// first try just moving to the destination	
		//env_trace_t tmpInfo;
		trcInfo.vExt = mv.ext;
		trcInfo.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;  
		trcInfo.vStart = mv.start;
		trcInfo.vDelta = vDelta;
		trcInfo.vTerStart = trcInfo.vStart;
		//trcInfo.vTerStart.y -= trcInfo.vExt.y;
		trcInfo.vTerStart.y -= mv.fCapsuleCenterHei;
		bool bClear = !CollideWithEnv(&trcInfo);
		
		if (trcInfo.bStartSolid)
		{
			mv.velocity.Clear();
			mv.end = origin;
		}
		else
		{
			// If we made it all the way, then copy trace end
			//  as new player position.
			if (bClear)
			{
				mv.end = trcInfo.vStart + trcInfo.vDelta;
			}
			else
			{	
				A3DVECTOR3 vInitVelocity = mv.velocity;
				while (nTry < MAX_TRY) 
				{
					if (vDelta.SquaredMagnitude() < DIST_EPSILON ) 
					{
						break;
					}

					if (DotProduct(vDelta, vInitVelocity) <= 0)
						break;
					
					trcInfo.vStart = mv.start;
					trcInfo.vDelta = vDelta;
					trcInfo.vTerStart = mv.start;
					//trcInfo.vTerStart.y -= mv.ext.y;
					trcInfo.vTerStart.y -= mv.fCapsuleCenterHei;
					trcInfo.vWatStart = mv.start;
					//trcInfo.vWatStart.y -= mv.ext.y;
					trcInfo.vWatStart.y -= mv.fCapsuleCenterHei;
					bClear = !CollideWithEnv(&trcInfo);	
					
					++nTry;
					if (bClear || trcInfo.bStartSolid) 
					{
						mv.end = mv.start + vDelta;
						break;
					}
					mv.collideflag |= trcInfo.dwClsFlag;
					mv.start  += vDelta*trcInfo.fFraction;
					mv.end = mv.start;
					fTime -= fTime*trcInfo.fFraction;
					mv.tpnormal = trcInfo.vHitNormal;
					mv.wishspd = Normalize(mv.velocity, mv.wishdir);
					mv.wishspd *= (1- nTry * 0.1f);
					dtp = DotProduct(mv.tpnormal, mv.wishdir);
					mv.velocity = (mv.wishdir - mv.tpnormal * dtp - mv.tpnormal*dtp * VEL_REFLECT) * mv.wishspd; 
					vDelta = mv.velocity * fTime;
					
				}
				mv.velocity = mv.absvelocity;
			}
		}
	}

	if (!IsPosInAvailableMap(mv.end))
	{
		awmInfo.vClipVel = originvel;
		awmInfo.vCenter  = origin;
		return;
	}
	
	//see if meet height thresh
	ground_trace_t groundTrc;
	groundTrc.vStart = mv.end;
	groundTrc.vExt = mv.ext;
	groundTrc.fCapsuleCenterHei = mv.fCapsuleCenterHei;
	groundTrc.fCapsuleHei = mv.fCapsuleHei;
	groundTrc.fCapsuleR = mv.fCapsuleR;
	groundTrc.vCapsuleUp = mv.vCapsuleUp;

	groundTrc.fDeltaY = awmInfo.fHeightThresh + 0.1f;
	awmInfo.bMeetHeightThresh = false;

	if (!RetrieveSupportPlane(&groundTrc))
	{
		awmInfo.vClipVel = originvel;
		awmInfo.vCenter  = origin;
		return;
	}
	else
	{
		A3DVECTOR3 vTpNormal(0.0f);
		A3DVECTOR3 vOverTp(mv.end);
		bool bAdjust = false;
		
		float fHWater = GetCollisionSupplier()->GetWaterHeight(mv.end);
		if (groundTrc.bSupport)
		{
			bAdjust = true;
			vOverTp = groundTrc.vEnd;
			vTpNormal = groundTrc.vHitNormal;
			if (fHWater > groundTrc.vEnd.y)
			{
				vOverTp.y = fHWater;
				vTpNormal = A3DVECTOR3(0, 1, 0);
			}
		}
		else if (mv.end.y < fHWater + awmInfo.fHeightThresh )
		{
			bAdjust = true;
			vOverTp = mv.end;
			vOverTp.y = fHWater;
			vTpNormal = A3DVECTOR3(0, 1, 0);
		}
		
		if (bAdjust &&  (vOverTp.y + awmInfo.fHeightThresh > mv.end.y) )
		{
			//BrushTraceInfo  brushTrc;
			vDelta.Clear();
			vDelta.y = vOverTp.y + awmInfo.fHeightThresh - mv.end.y;
			float fAllow = (float)fabs(awmInfo.vCenter.y - mv.end.y)+ 0.001f;
			fAllow = a_Max(fAllow, mv.t);
			a_ClampRoof(vDelta.y, fAllow);

			BrushCollideParam para;
			para.vStart = mv.end;
			para.vDelta = vDelta;
			para.fCapsuleCenterHei = mv.fCapsuleCenterHei;
			para.fCapsuleHei = mv.fCapsuleHei;
			para.fCapsuleR = mv.fCapsuleR;
			para.vCapsuleUp = mv.vCapsuleUp;

			if (CollideWithBrush(&para))
			{
				mv.end += (vDelta * para.fFraction);
				mv.collideflag |= CDR_BRUSH;
			}
			else
			{
				mv.end += vDelta;
			}
			awmInfo.bMeetHeightThresh = true;
			//awmInfo.bMeetHeightThresh = (mv.end.y - vOverTp.y > awmInfo.fHeightThresh);
		}
	}
	awmInfo.fMoveDist = Magnitude(mv.end - awmInfo.vCenter);
	awmInfo.bBlocked =  (awmInfo.fMoveDist < DIST_EPSILON);
	if (awmInfo.bBlocked)
	{
		awmInfo.vClipVel.Clear();
	}

	awmInfo.vClipVel	= mv.velocity;
	awmInfo.vCenter		= mv.end;
	awmInfo.vTPNormal	= mv.tpnormal;	
	awmInfo.vAbsVelocity = mv.absvelocity;
	awmInfo.fSpeed		= Normalize(mv.velocity,awmInfo.vVelDir);
	awmInfo.fCurRoll	= awmInfo.vVelDir.y;
	awmInfo.dwCollideFlag	= mv.collideflag;
	
#undef  MAX_TRY
#undef  VEL_REFLECT
}


#undef  CDR_BRUSH
#undef  CDR_TERRAIN
#undef  CDR_WATER

