/*
 * FILE: Collision.cpp
 *
 * DESCRIPTION: userinterface used by ECCDR
 *
 * 
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

#include "Collision.h"
#include "BrushesObjMgr.h"
#include "ECCDR.h"
#include <A3DTrace.h>
#include <A3DTerrain2.h>
#include "EngineInterface.h"
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

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
bool ICollisionSupplier::CollideWithWater(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, bool bWaterSolid, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart )
{
#define LOCAL_EPSILON  1E-5f
	if (bWaterSolid && (vDelta.y > LOCAL_EPSILON ))
	{
		return false;
	}
	
	if ( !bWaterSolid && (vDelta.y < -LOCAL_EPSILON))
	{
		return false;
	}
	
	if (vDelta.y < LOCAL_EPSILON && vDelta.y > -LOCAL_EPSILON)
	{ //parallel  the water plane
		return false;
	} 	
	
	float h0 = GetWaterHeight(vStart);
	float h1 = GetWaterHeight(vStart+vDelta);
	float hWater = a_Max(h0, h1);
	fFraction = 100.0f;
	
	vHitNormal = A3DVECTOR3(0, 1, 0);
	bStart = false;
	
	float t=  (hWater - vStart.y)/vDelta.y;
	if(t >= 0.0f && t<= 1.0f)
	{
		fFraction = a_Max(0.0f , t - 1E-2f);
		if (bWaterSolid && h0 > vStart.y)
		{
			fFraction = 0.0f;
			bStart = true;
		}
		
		if (!bWaterSolid && h0 < vStart.y)
		{
			fFraction = 0.0f;
			bStart = true;
		}
		
		return true;
	}
	
#undef  LOCAL_EPSILON
	return false;
}

class CollisionSupplierImp: public ICollisionSupplier
{
public:
	CollisionSupplierImp();
	virtual ~CollisionSupplierImp() {};

	virtual bool Init(A3DTerrain2* pTerr, CBrushesObjMgr* pBrushesObjMgr);
	virtual float GetWaterHeight(const A3DVECTOR3& vPos) const;
	virtual A3DTerrain2* GetTerrain() const;
	virtual CBrushesObjMgr* GetBrushObjMgr() const;

protected:
	virtual void GetTerrainInfo(const A3DVECTOR3& vPos, A3DVECTOR3& vPosOnSurface, A3DVECTOR3& vNormal);
	virtual bool IsPosInAvailableMap(const A3DVECTOR3& vPos);
	virtual bool RayTraceEnv(const A3DVECTOR3& vStart,  const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction, bool* pStartSolid = NULL);
	virtual bool CollideWithBrush(BrushCollideParam *pParam);
	virtual bool IsTerrEntry(const A3DVECTOR3& vPos);

	A3DTerrain2* m_pTerr;
	CBrushesObjMgr* m_pBrushesObjMgr;

	//friend function
	FRIENDFUN
};


/*
* ray: origin + t* dir,  triangle:  p(u,v) = (1-u -v)vert[0] + u*vert[1] + v*vert[2]
* @desc :
* @param vDir: normalized direction     
* @param bCull: cull back face if true
* @return :
* @note:
* @todo:  
* @author: kuiwu [8/10/2005]
* @ref:  Tomas Moller's JGT code
*/
bool   RayTriangleIntersect(const  A3DVECTOR3& vOrigin, const A3DVECTOR3& vDir,   A3DVECTOR3 *  vert[3], float& t, float& u, float& v, bool bCull)
{
	// find vectors for two edges sharing vert0 
	A3DVECTOR3 edge1( (*vert[1]) - (*vert[0]));
	A3DVECTOR3 edge2( (*vert[2]) - (*vert[0]));
	// begin calculating determinant - also used to calculate U parameter 
	A3DVECTOR3 pvec = CrossProduct(vDir, edge2);
	// if determinant is near zero, ray lies in plane of triangle 
	float det = DotProduct(edge1, pvec);
#define   LOCAL_EPSILON   1e-5f
	if (bCull)
	{
		if(det<LOCAL_EPSILON)
			return false;
		// From here, det is > 0. 
		// Calculate distance from vert0 to ray origin
		A3DVECTOR3 tvec(vOrigin - (*vert[0]));
		// Calculate U parameter and test bounds
		u = DotProduct(tvec, pvec);
		if ((u  < 0.0f) || (u > det))
			return false;
		
		// prepare to test V parameter 
		A3DVECTOR3 qvec = CrossProduct(tvec, edge1);
		// calculate V parameter and test bounds 
		v = DotProduct(vDir, qvec);
		if ((v < 0.0f) || (u + v > det))
			return false;
		
		// calculate t, ray intersects triangle 
		t = DotProduct(edge2, qvec) ;
		// Det > 0 so we can early exit here
		// Intersection point is valid if distance is positive 
		// (else it can just be a face behind the orig point)
		if (t < 0.0f)
		{
			return false;
		}
		
		float OneOverDet = 1.0f / det;
		
		t *= OneOverDet;
		u *= OneOverDet;
		v *= OneOverDet;
	}
	else
	{
		if(det>-LOCAL_EPSILON && det<LOCAL_EPSILON)
			return false;
		float OneOverDet = 1.0f / det;
		// Calculate distance from vert0 to ray origin
		A3DVECTOR3 tvec(vOrigin - (*vert[0]));
		// calculate U parameter and test bounds 
		u = DotProduct(tvec, pvec)* OneOverDet;
		if ((u < 0.0f) || (u > 1.0f))
			return false;
		// prepare to test V parameter 
		A3DVECTOR3 qvec = CrossProduct(tvec, edge1);
		// calculate V parameter and test bounds 
		v = DotProduct(vDir, qvec) * OneOverDet;
		if ((v < 0.0f) || (u + v > 1.0f))
			return false;
		// calculate t, ray intersects triangle 
		t = DotProduct(edge2, qvec) * OneOverDet;
	}
	
#undef    LOCAL_EPSILON
	
	return true;
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
bool SegmentTriangleIntersect( const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta,  A3DVECTOR3 * vert[3], float& fFraction, bool bCull)
{
	float dist;
	A3DVECTOR3 vDir(vDelta);
	
	dist = vDir.Normalize();
	if (dist < 1E-5f)
	{
		assert(0 && "too small dist!");
		fFraction = 0.0f;
		return true;
	}
	
	float t, u, v;
	
	if (RayTriangleIntersect(vStart, vDir, vert, t, u, v, bCull) && (t>=0.0f) && (t <= dist))
	{
		//fFraction = t/dist;
		//fFraction = a_Max( 0.0f, fFraction -1E-4f); //put back
		fFraction  = (t- 5E-4f)/dist;
		a_ClampFloor(fFraction, 0.0f);
		return true;
	}
	
	return false;
}

bool ICollisionSupplier::VertTrace(BrushCollideParam *pParam, CDR_TRACE_OUTPUT* pOut, float DeltaY)
{
	A3DVECTOR3 vTerrainPos, vTerrainNormal;
	GetTerrainInfo(pParam->vStart, vTerrainPos,vTerrainNormal);
	ground_trace_t  groundTrc;
	
	groundTrc.vStart = pParam->vStart;
	groundTrc.fDeltaY = a_Min(pParam->vStart.y - pParam->fCapsuleCenterHei - vTerrainPos.y+ 0.5f, DeltaY);
	groundTrc.fCapsuleR = pParam->fCapsuleR;
	groundTrc.fCapsuleHei = pParam->fCapsuleHei;
	groundTrc.fCapsuleCenterHei = pParam->fCapsuleCenterHei;
	groundTrc.vExt = pParam->vExt;
	
	//groundTrc.fBump = 0.001f;
	if (!RetrieveSupportPlane(&groundTrc) || (!groundTrc.bSupport))
	{
		pOut->vHitPos = vTerrainPos;
		pOut->vHitPos.y += (pParam->fCapsuleCenterHei + 1E-4f);
		pOut->vHitNormal = vTerrainNormal;
		pOut->fFaction = -1.0f;	//	Set to invalid value
		pParam->pSinkedBrush = groundTrc.pUser1;
		return false;
	}
	
	assert(groundTrc.bSupport);
	
	pOut->vHitPos = groundTrc.vEnd;
	pOut->vHitNormal = groundTrc.vHitNormal;
	pOut->fFaction = -1.0f;	//	Set to invalid value
	
	pParam->vHitNormal = groundTrc.vHitNormal;
	pParam->fFraction = -1;
	pParam->pSinkedBrush = groundTrc.pUser1;
	return true;
} 

bool ICollisionSupplier::VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY , bool* pStartSolid)
{
	A3DVECTOR3 vTerrainPos, vTerrainNormal, vBuildingPos, vBuildingNormal;
	float fraction;

	if (*pStartSolid)
		*pStartSolid = false;
	
	GetTerrainInfo(vPos, vTerrainPos, vTerrainNormal);
	if(RayTraceEnv(vPos, -DeltaY*A3DVECTOR3(0, 1, 0), vBuildingPos, vBuildingNormal, fraction, pStartSolid))
	{
		if(vBuildingPos.y>vTerrainPos.y)
		{
			// 与建筑发生了碰撞
			vHitPos=vBuildingPos;
			vHitNormal=vBuildingNormal;
			return true;
		}
	}
	
	//@note: kuiwu  fix the bug 2005/8/10
	
	//if(vTerrainPos.y<=vPos.y && vTerrainPos.y>=vPos.y-DeltaY)
	if (vTerrainPos.y > vPos.y && pStartSolid)
		*pStartSolid = true;

	
	if (vTerrainPos.y > vPos.y || (vTerrainPos.y<=vPos.y && vTerrainPos.y>=vPos.y-DeltaY) ) 
	{
		
		vHitPos=vTerrainPos;
		vHitNormal=vTerrainNormal;
		return true;
	}
	return false;
}

bool ICollisionSupplier::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction)
{
	RAYTRACERT rt;
	fraction = 1.0f;
	rt.fFraction = 1.0f;
	bool bTraceEnv = RayTraceEnv(vStart,vDelta,vHitPos,vNormal,fraction);
	bool bTraceTerrain = GetTerrain()->RayTrace(vStart, vDelta, 1.0f, &rt);
	
	if(bTraceEnv)
	{
		if(bTraceTerrain && rt.fFraction<fraction)
		{
			vHitPos=rt.vHitPos;
			vNormal=rt.vNormal;
			fraction=rt.fFraction;
		}
		return true;
	}
	else
		if(bTraceTerrain)
		{
			vHitPos=rt.vHitPos;
			vNormal=rt.vNormal;
			fraction=rt.fFraction;
			return true;	
		}
		else
			return false;
		
}

bool ICollisionSupplier::RetrieveSupportPlane(ground_trace_t * pTrc)
{
	env_trace_t  trcInfo;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	trcInfo.vExt = pTrc->vExt;
	trcInfo.fCapsuleR = pTrc->fCapsuleR;
	trcInfo.fCapsuleHei = pTrc->fCapsuleHei;
	trcInfo.vCapsuleUp = pTrc->vCapsuleUp;
	
	pTrc->bSupport = false;
	trcInfo.vStart = pTrc->vStart;
	//trcInfo.vStart.y += pTrc->fBump;  //bump up, similar to hl2
	trcInfo.vDelta = A3DVECTOR3(0.0f, -(pTrc->fDeltaY), 0.0f);
	trcInfo.vTerStart=pTrc->vStart;
	trcInfo.vTerStart.y -= pTrc->fCapsuleCenterHei;//foot
	
	bool bClear = !CollideWithEnv(&trcInfo);	
	pTrc->pUser1 = trcInfo.pUser1;
	
	if (trcInfo.bStartSolid)
	{
		return false;
	}
	
	if (bClear)
	{
		pTrc->bSupport = false;
		return true;
	}
	
	pTrc->vEnd = trcInfo.vStart + trcInfo.vDelta* trcInfo.fFraction;
	pTrc->bSupport = true;
	pTrc->vHitNormal = trcInfo.vHitNormal;
		
	return true;
}

bool ICollisionSupplier::CollideWithEnv(env_trace_t * pEnvTrc)
{
	pEnvTrc->fFraction = 100.0f;
	pEnvTrc->bStartSolid = false;
	pEnvTrc->dwClsFlag = 0;

	if ((pEnvTrc->dwCheckFlag & CDR_BRUSH) == CDR_BRUSH)
	{
		BrushCollideParam param;
		param.vStart = pEnvTrc->vStart;
		param.vDelta = pEnvTrc->vDelta;
		param.fCapsuleHei = pEnvTrc->fCapsuleHei;
		param.fCapsuleR = pEnvTrc->fCapsuleR;
		param.vCapsuleUp = pEnvTrc->vCapsuleUp;
		param.vExt = pEnvTrc->vExt;
		
		if (CollideWithBrush(&param))
		{
			pEnvTrc->fFraction = param.fFraction;
			pEnvTrc->vHitNormal = param.vHitNormal;
			pEnvTrc->bStartSolid = param.bStartSolid;
			pEnvTrc->pUser1 = param.pSinkedBrush;
			pEnvTrc->dwClsFlag = CDR_BRUSH;
		}
	}
	if ((pEnvTrc->dwCheckFlag & CDR_TERRAIN) == CDR_TERRAIN )
	{
		float fFraction;
		A3DVECTOR3 vTerNormal;
		bool bStart;
		if(CollideWithTerrain(pEnvTrc->vTerStart, pEnvTrc->vStart, pEnvTrc->vDelta, fFraction,  vTerNormal, bStart) 
			&& ((fFraction < pEnvTrc->fFraction )||bStart))// StartSolid must conside
		{
			assert(fFraction >= 0.0f);
			pEnvTrc->fFraction = fFraction ;
			pEnvTrc->vHitNormal = vTerNormal;
			pEnvTrc->bStartSolid = bStart;
			pEnvTrc->dwClsFlag = CDR_TERRAIN;
		}
		
	}
	if ((pEnvTrc->dwCheckFlag & CDR_WATER) == CDR_WATER)
	{
		float fFraction;
		A3DVECTOR3 vWatNormal;
		bool bStart;
		//@todo : TBD: use center or foot? By Kuiwu[10/10/2005]
		if (CollideWithWater(pEnvTrc->vWatStart, pEnvTrc->vDelta, pEnvTrc->bWaterSolid, fFraction, vWatNormal, bStart)
			&& fFraction < pEnvTrc->fFraction)
		{
			pEnvTrc->fFraction = fFraction;
			pEnvTrc->vHitNormal = vWatNormal;
			pEnvTrc->bStartSolid = bStart;
			pEnvTrc->dwClsFlag = CDR_WATER;			
		}
	}

	bool bCollide = (pEnvTrc->fFraction < 1.0f+ 1E-4f);
	pEnvTrc->fFraction = a_Min(pEnvTrc->fFraction,  1.0f);
	return bCollide;
}

bool ICollisionSupplier::CollideWithTerrain(const A3DVECTOR3& vStart, const A3DVECTOR3& vCenter, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3& vHitNormal, bool& bStart)
{
	A3DTerrain2* pTerrain = GetTerrain();
	bStart = false;
	float h1 = 0.0f;
	GetTerrainHeight(pTerrain, vStart, h1, &vHitNormal);
	////if (h1 > vStart.y + 1E-4f )
	////{//start under terrain
	////	bStart = true;
	////	fFraction = 0.0f;
	////	return true;
	////}
	bool bAllowInside = true;


	if (IsTerrEntry(vStart))// This is in hole flag!
		bAllowInside = true;
	else if (h1 < vStart.y + 1E-4f )
		bAllowInside = false; //一旦出来按照正常地形检测 ，除非在入口处（IsTerrEntry）才能进入地形
	else if (h1 < vCenter.y + 1E-4f )
	{
		bStart = true;
		fFraction = 0.0f;
		return true;
	}

	if(bAllowInside)
	{	
		bStart = false;
		fFraction = 1.0f;
		return false;
	}

	
	int nWid, nHei; // in grid, 2 meters
	float fMag = vDelta.Magnitude();
	nWid = (int)ceil(fMag / 2.0f);
	nWid = a_Max(3, nWid);
	nHei = nWid;
	int nTriangles = nWid * nHei*2;
	// make the buffer a little larger
	A3DVECTOR3 * pVerts = new A3DVECTOR3[(nWid +2) * (nHei +2)];
	assert(pVerts != NULL);
	memset(pVerts, 0, sizeof(A3DVECTOR3) * (nWid + 2) * (nHei + 2));
	WORD * pIndices = new WORD[nTriangles*3];
	assert(pIndices != NULL);
	memset(pIndices, 0, sizeof(WORD) * nTriangles * 3);
#ifdef _ANGELICA3
	int iNumVert,iNumIndex;
	if (!pTerrain->GetFacesOfArea(vStart, nWid, nHei, pVerts, pIndices, iNumVert, iNumIndex))
#else
	if (!pTerrain->GetFacesOfArea(vStart, nWid, nHei, pVerts, pIndices))
#endif
	{
		delete[] pVerts;
		delete[] pIndices;
		return false;
	}
	int i;
	A3DVECTOR3 * vert[3];
	//@note : Here init the fraction.  By Kuiwu[9/10/2005]
	fFraction = 100.0f;
	float tmpFraction = fFraction;
	
	for (i = 0; i < nTriangles; i++)
	{
		vert[0] = pVerts +pIndices[i*3];
		vert[1] = pVerts +pIndices[i*3+1];
		vert[2] = pVerts +pIndices[i*3+2];
		A3DVECTOR3 vPt;
		
		//@note: Tomas Moller's JGT code  : By Kuiwu[9/10/2005]
		//@note: discard the engine version because it put back the hit point too much. By Kuiwu[13/10/2005]
		//		if(CLS_RayToTriangle(vStart, vDelta, *vert[0], *vert[1], *vert[2], vPt, true, &tmpFraction)
		//			&& (tmpFraction <= 1.0f)  && (tmpFraction < fFraction))
		if (SegmentTriangleIntersect(vStart, vDelta, vert, tmpFraction, true) && (tmpFraction < fFraction))
		{
			//get the triangle normal
			A3DVECTOR3 vEdge1((*vert[1]) - (*vert[0]) );
			A3DVECTOR3 vEdge2((*vert[2]) - (*vert[0]) );
			vHitNormal = CrossProduct(vEdge1, vEdge2);
			vHitNormal.Normalize();
			
			//@note : may be redundant, but to assure.  By Kuiwu[17/10/2005]
			A3DVECTOR3 vDir;
			Normalize(vDelta, vDir);
			if (DotProduct(vHitNormal, vDir) > 0.01f)
			{//leave the hit plane
				assert(0 && "hit a plane with same direction!");
				continue;
			}
			
			fFraction = a_Max(0.0f, tmpFraction);
		}
		
	}
	delete[] pVerts;
	delete[] pIndices;

	// fFraction其实会有极的小误差, 卡住的问题 By Niuyadong 2009-3-12
	//						|
	//					<---|			(Start point 近似在平面上)
	//						|<----		(End point 近似在平面上。这种情况没试出来)
	//						|
	A3DVECTOR3 vTempEnd = vStart+vDelta, vTempDelta = vDelta,vTemStart = vStart;
	GetTerrainHeight(pTerrain, vTempEnd, h1);

	if ((h1 > vTempEnd.y + 1E-4f)&&(fFraction == 100.0f) )
	{	
		float fTryMax = 3.0f;
		fFraction = 1.0f;
		vTempDelta.Normalize();
		vTempDelta *= a_Min(0.01f, vDelta.Magnitude()*0.2f);
		float fD = vTempDelta.Magnitude()/vDelta.Magnitude();
		float fTry = 0.0f;
		do
		{
			vTemStart += vTempDelta;
			GetTerrainHeight(pTerrain, vTemStart, h1);
			if (h1 > vTemStart.y + 1E-4f )
			{
				fFraction = fD*fTry;	//					<---|			(Start point 近似在平面上)
				break;
			}
			
			vTempEnd -= vTempDelta;
			GetTerrainHeight(pTerrain, vTempEnd, h1);
			if (h1 < vTempEnd.y + 1E-4f )
			{
				fFraction = 1.0f-(fTry+1)*fD;//					|<----		(End point 近似在平面上。这种情况没试出来)
				break;
			}
		}while(++fTry<fTryMax);
		if(fTry>=fTryMax)
			fFraction = 0.0f;
		fFraction = a_Max(0.0f, fFraction);
	}
	
	return fFraction <= 1.0f;
}

bool ICollisionSupplier::RetrievePlane(BrushCollideParam *pParam, A3DVECTOR3& vHitPos)
{	
	env_trace_t  trcInfo;
	trcInfo.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	trcInfo.vExt	= pParam->vExt;
	trcInfo.vStart	= pParam->vStart;
	trcInfo.vDelta	= pParam->vDelta;
	trcInfo.vTerStart= pParam->vStart;
	trcInfo.fCapsuleR = pParam->fCapsuleR;
	trcInfo.fCapsuleHei = pParam->fCapsuleHei;
	trcInfo.vTerStart.y -= pParam->fCapsuleCenterHei; //foot
	pParam->bStartSolid = false;
	pParam->iUserData1 = pParam->iUserData2 = 0;
	
	bool bClear = !CollideWithEnv(&trcInfo);
	pParam->pSinkedBrush = trcInfo.pUser1;

	if (trcInfo.bStartSolid)
	{
		pParam->bStartSolid = true;
		return false;
	}
	
	if (bClear)
	{
		return false;
	}
	else
	{
		vHitPos = trcInfo.vStart + trcInfo.vDelta* trcInfo.fFraction;
		pParam->vHitNormal = trcInfo.vHitNormal;
		pParam->fFraction = trcInfo.fFraction;
		
		return true;
	}
}


////////////////////////////////////ConcreteCollision//////////////////////////////////////
CollisionSupplierImp::CollisionSupplierImp()
{
	m_pTerr = NULL;
	m_pBrushesObjMgr = NULL;
}

bool CollisionSupplierImp::Init(A3DTerrain2* pTerr, CBrushesObjMgr* pBrushesObjMgr)
{
	m_pTerr = pTerr;
	m_pBrushesObjMgr = pBrushesObjMgr;
	return true;
}

float CollisionSupplierImp::GetWaterHeight(const A3DVECTOR3& vPos) const
{
	return vPos.y;
}

A3DTerrain2* CollisionSupplierImp::GetTerrain() const
{
	return m_pTerr;
}

CBrushesObjMgr* CollisionSupplierImp::GetBrushObjMgr() const
{
	return m_pBrushesObjMgr;
}

bool CollisionSupplierImp::CollideWithBrush(BrushCollideParam *pParam)
{
	//here we use capsule
	if (m_pBrushesObjMgr)
	{
		CHBasedCD::CCapsule capsule;
		capsule.SetRadius(pParam->fCapsuleR);
		capsule.SetHeight(pParam->fCapsuleHei);
		capsule.SetUp(pParam->vCapsuleUp);
		pParam->pSinkedBrush = NULL;

		A3DVECTOR3 normal;
		if (m_pBrushesObjMgr->CapsuleTrace(pParam->vStart, pParam->vDelta, pParam->fFraction, capsule, &normal, pParam->bStartSolid, (CHBasedCD::CConvexBrush**)&pParam->pSinkedBrush))
		{
			pParam->vHitNormal = normal;
			return true;
		}
		return false;
	}
	
	return false;
}

bool CollisionSupplierImp::RayTraceEnv(const A3DVECTOR3& vStart,  const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction, bool* pStartSolid)
{
	if (pStartSolid)
		*pStartSolid = false;

	if (m_pBrushesObjMgr)
	{		
		bool bStart = false;
		if (m_pBrushesObjMgr->RayTrace(vStart, vDelta, fraction, &vNormal, bStart))
		{
			vHitPos = vStart + fraction*vDelta;
			*pStartSolid = bStart;
			return true;
		}
		return false;
	}
	
	return false;
}

void CollisionSupplierImp::GetTerrainInfo(const A3DVECTOR3& vPos, A3DVECTOR3& vPosOnSurface, A3DVECTOR3& vNormal)
{
	A3DTerrain2* pTerr = GetTerrain();
	vPosOnSurface=vPos;
	GetTerrainHeight(pTerr, vPos, vPosOnSurface.y, &vNormal);
}

bool CollisionSupplierImp::IsPosInAvailableMap(const A3DVECTOR3& vPos)
{
	return true;
}

bool CollisionSupplierImp::IsTerrEntry(const A3DVECTOR3& vPos)
{	
	if(!m_pTerr)
		return false;

	A3DVECTOR3 vHitNormal;
	float h;
	GetTerrainHeight(m_pTerr, vPos, h, &vHitNormal);
	/*ARectF& r = ECGame::GetGame()->m_RectHole;*/
	const float DIST_EPSILON = 1e-4f;
	if (h<DIST_EPSILON&&h>-DIST_EPSILON)
	{
		return true;
	}
	else
		return false;
}

ICollisionSupplier* GetCollisionSupplier()
{
	static CollisionSupplierImp collision;
	return &collision;
}
