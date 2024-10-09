// BrushesObj.cpp: implementation of the CBrushesObj class.
//
//////////////////////////////////////////////////////////////////////

/*
 * FILE: BrushesObj.cpp
 *
 * DESCRIPTION: Brush objects corresponding to a file(.chf) 
 *
 * CREATED BY: YangXiao, 2010/10/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "aphysxlab.h"
#include "BrushesObj.h"
#include "HalfSpace.h"

#include "ChbEntity.h"
#include "ChbEntityManager.h"
#include "BrushesObjMgr.h"
#include "Collision.h"
#include "BrushObjAttr.h"

#include <AFI.h>
#include <ALog.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CChbEntityManager g_ChbMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrushesObj::CBrushesObj(): m_pPhysxObj(NULL), m_bLoad(false), m_strFile(""), m_pChbEntity(NULL)
{
	m_pDynAttr = NULL;
	m_isMoveing = false;

}

CBrushesObj::~CBrushesObj()
{
	Release();	
	delete m_pDynAttr;
}

bool CBrushesObj::Init(IPhysXObjBase* pPhysxObj)
{
	AString strFile = pPhysxObj->GetFilePathName();

	af_ChangeFileExt(strFile, ".chb");
	
	Release();
	
	m_strFile = "";
	m_pPhysxObj = pPhysxObj;

	if (!pPhysxObj->IsActive())//不加载
	{
		m_strFile = strFile;
		return true;
	}

	return (DynamicLoad(strFile));
}

#ifdef USING_BRUSH_MAN
void CBrushesObj::Release()
{
	if(!m_bLoad)
		return;
	g_brushMan.RemoveProvider(&m_building);
	m_bLoad = false;
	m_building.Release();
}
#else
void CBrushesObj::Release()
{
	if(!m_bLoad)
		return;

	m_bLoad = false;

	g_ChbMgr.ReleaseChb(m_strFile);
	m_pChbEntity = NULL;
}
#endif



#ifdef USING_BRUSH_MAN
bool CBrushesObj::DynamicLoad(const AString& strFile)
{
	g_brushMan.RemoveProvider(&m_building);
	m_bLoad = m_building.Load(strFile);
	m_strFile = strFile;
	g_brushMan.AddProvider(&m_building);

	A3DMATRIX4 trans = m_pPhysxObj->m_refFrame.GetRotate();
	trans.SetRow(3, m_pPhysxObj->m_refFrame.GetPos());
	A3DMATRIX4 scale;
	scale.Scale(m_pPhysxObj->m_fScale, m_pPhysxObj->m_fScale, m_pPhysxObj->m_fScale);

	m_building.SetTransMat(scale*trans);

	return m_bLoad;
}
#else
bool CBrushesObj::DynamicLoad(const AString& strFile)
{	
	m_trans = m_pPhysxObj->m_refFrame.GetRotate();
	m_trans.SetRow(3, m_pPhysxObj->m_refFrame.GetPos());
	m_strFile = strFile;
	m_bLoad = true;
	m_pChbEntity = g_ChbMgr.LoadChb(strFile);

	if (NULL == m_pChbEntity)
		return true;

	A3DMATRIX4 scale;
	scale.Scale(m_pPhysxObj->m_fScale, m_pPhysxObj->m_fScale, m_pPhysxObj->m_fScale);
	
	m_BrushAABBWorld = m_pChbEntity->GetTransformedAABB(scale * GetTrans());
	
	return true;
}
#endif



//
void CBrushesObj::Tick(int deltatime)
{
	if (m_pPhysxObj == NULL || m_strFile == "")
		return;

	if (m_pPhysxObj->IsActive() && !m_bLoad)
		DynamicLoad(m_strFile);
	else if (!m_pPhysxObj->IsActive() && m_bLoad)
		Release();
	if (m_pDynAttr && m_isMoveing && m_bLoad)
	{
		A3DVECTOR3 vPos = m_pPhysxObj->GetPos();
		A3DVECTOR3 vDir, vUp;
		m_pPhysxObj->GetDirAndUp(vDir, vUp);

		m_pDynAttr->TickMove(deltatime, vPos, vDir);
		m_pPhysxObj->UpdatePos(vPos);
		m_pPhysxObj->UpdateDirAndUp(vDir, vUp);
	}
}

void CBrushesObj::Render(A3DFlatCollector* pFC, DWORD dwFColor/* =0xa0ffff00 */)
{
	m_trans = m_pPhysxObj->m_refFrame.GetRotate();
	m_trans.SetRow(3, m_pPhysxObj->m_refFrame.GetPos());

	#ifdef USING_BRUSH_MAN
	m_building.DebugRender(pFC, m_pPhysxObj->m_fScale, dwFColor, A3DVECTOR3(1, 1, 1), &m_trans);
	#else
	if (m_pChbEntity)
    	m_pChbEntity->DebugRender(pFC, m_pPhysxObj->m_fScale, dwFColor, A3DVECTOR3(1, 1, 1), &m_trans );
	#endif   	
}

bool CBrushesObj::AABBTrace(AABBBrushTraceInfo2* pInfo)
{
	pInfo->fScale = m_pPhysxObj->m_fScale;
	
	if (m_pChbEntity == NULL)
		return false;

	if (m_pDynAttr == NULL)//静态物体
	{
		if (!AABBAABBCollision(pInfo->BoundAABB, m_BrushAABBWorld))
			return false;
	}
	else//动态物体
	{
		A3DAABB aabb = m_pChbEntity->GetAABB();
		aabb.Center = aabb.Center * pInfo->fScale;
		aabb.Extents = aabb.Extents * pInfo->fScale;
		aabb.CompleteMinsMaxs();
		
		if(!CLS_AABBOBBOverlap(aabb, pInfo->obb))
			return false;
	}
		
	return m_pChbEntity->AABBTrace(pInfo);
	
}

bool CBrushesObj::CapsuleTrace(CapusuleBrushTraceInfo* pInfo)
{
	
	pInfo->fScale = m_pPhysxObj->m_fScale;
	
	if (m_pChbEntity == NULL)
		return false;

	if (m_pDynAttr == NULL)//静态物体
	{
		if (!AABBAABBCollision(pInfo->BoundAABB, m_BrushAABBWorld))
		return false;
	}
	else//动态物体
	{
		A3DAABB aabb = m_pChbEntity->GetAABB();
		aabb.Center = aabb.Center * pInfo->fScale;
		aabb.Extents = aabb.Extents * pInfo->fScale;
		aabb.CompleteMinsMaxs();

		if(!CapsuleAABBCollision(pInfo, aabb))
			return false;
	}
	
	return m_pChbEntity->CapusuleTrace(pInfo);
	
}

bool CBrushesObj::RayTrace(RayBrushTraceInfo* pInfo)
{
	pInfo->fScale = m_pPhysxObj->m_fScale;
	
	if (m_pChbEntity == NULL)
		return false;
	
	A3DVECTOR3 vPoint, vNormal;
	float fraction;
	
	if (!CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, m_BrushAABBWorld.Mins, m_BrushAABBWorld.Maxs, vPoint, &fraction, vNormal))
		return false;
	
	return m_pChbEntity->RayTrace(pInfo);
}

void CBrushesObj::AddBrushAttr(BrushObjAttr* pBrushAttr)
{
	delete m_pDynAttr;
	m_pDynAttr = new BrushObjAttr(*pBrushAttr);
}
