// ConvexBrushMgr.cpp: implementation of the CConvexBrushMgr class.
//
//////////////////////////////////////////////////////////////////////

/*
 * FILE: ConvexBrushMgr.h
 *
 * DESCRIPTION: Manager of CBrushesObj 
 *
 * CREATED BY: YangXiao, 2010/10/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "aphysxlab.h"
#include "BrushesObjMgr.h"
#include <AFI.h>

#include "BrushesObj.h"
#include "PhysXObjStatic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace CHBasedCD;

#ifdef USING_BRUSH_MAN
CECBrushMan g_brushMan;
#endif

CBrushesObjMgr::CBrushesObjMgr()
{
	m_CollidedBrush = NULL;
	m_pSinkedBrush = NULL;
	m_isMoving = false;
}

CBrushesObjMgr::~CBrushesObjMgr()
{
	Release();
}

bool CBrushesObjMgr::AddBrushesObj(IPhysXObjBase* pPhysxObj)
{
	assert(pPhysxObj);
	int i;
	for (i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (pPhysxObj == m_pBrushObjs[i]->GetPhysXObj())
			break;
	}
	if (i < m_pBrushObjs.size())
		return false;
	
	CBrushesObj* pBrushesObj = new CBrushesObj;
	if (pBrushesObj->Init(pPhysxObj))
	{
		m_pBrushObjs.push_back(pBrushesObj);
	}
	else
	{
		delete pBrushesObj;
		return false;
	}
	return true;
}

bool CBrushesObjMgr::ReleaseBrushesObj(IPhysXObjBase* pPhysxObj)
{
	int i;
	for (i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (m_pBrushObjs[i]->GetPhysXObj() == pPhysxObj)
		{
			delete m_pBrushObjs[i];
			m_pBrushObjs.erase(m_pBrushObjs.begin() + i);
			return true;
		}
	}
	return false;
}

void CBrushesObjMgr::Init(CPhysXObjMgr* physxMgr)
{
	Release();

	const APtrArray<IPhysXObjBase*>& objs = physxMgr->m_aObjects;

	for (int i = 0; i < objs.GetSize(); i++)
	{
		AString strFile = objs[i]->GetFilePathName();
		af_ChangeFileExt(strFile, ".chb");
		if (!af_IsFileExist(strFile))
			continue;

		CBrushesObj* pBrushesObj = new CBrushesObj;
		if (pBrushesObj->Init(objs[i]))
		{
			m_pBrushObjs.push_back(pBrushesObj);
		}
		else
			delete pBrushesObj;
	}
}

void CBrushesObjMgr::Release()
{
	for (int i = 0; i < m_pBrushObjs.size(); i++)
	{
		delete m_pBrushObjs[i];
	}
	m_pBrushObjs.clear();
}

void CBrushesObjMgr::Tick(int deltaTime)
{
	for (int i = 0; i < m_pBrushObjs.size(); i++)
		m_pBrushObjs[i]->Tick(deltaTime);
}

void CBrushesObjMgr::Render(A3DFlatCollector* pFC, DWORD dwFColor)
{
	for (int i = 0; i < m_pBrushObjs.size(); i++)
	{
		m_pBrushObjs[i]->Render(pFC, dwFColor);
	}
}

#ifdef USING_BRUSH_MAN
bool CBrushesObjMgr::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid)
{
	AABBBrushTraceInfo2 info;
	info.Init(vStart, vDelta, aabb.Extents);
	bool bCollide = g_brushMan.Trace(&info);
	if (bCollide)
	{
		fFraction = info.fFraction;
		if (pNormal)
			*pNormal = info.ClipPlane.GetNormal();
		bStartSolid = info.bStartSolid;
		
		return true;
	}
	return false;
}
bool CBrushesObjMgr::CapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const CCapsule& capsule, A3DVECTOR3 *pNormal, bool& bStartSolid)
{
	CapusuleBrushTraceInfo info;
	info.Init(vStart, vDelta, capsule.GetRadius(), capsule.GetHeight());
	bool bCollide = g_brushMan.Trace(&info);
	if (bCollide)
	{
		fFraction = info.fFraction;
		if (pNormal)
			*pNormal = info.ClipPlane.GetNormal();
		bStartSolid = info.bStartSolid;

		return true;
	}
	return false;
}
bool CBrushesObjMgr::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3 *pNormal, bool& bStartSolid)
{
	RayBrushTraceInfo info;
	info.Init(vStart, vDelta);
	bool bCollide = g_brushMan.Trace(&info);
	if (bCollide)
	{
		fFraction = info.fFraction;
		if (pNormal)
			*pNormal = info.ClipPlane.GetNormal();
		bStartSolid = info.bStartSolid;
		
		return true;
	}
	return false;
}
#else
bool CBrushesObjMgr::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid)
{
	AABBBrushTraceInfo2 info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;
	m_CollidedBrush = NULL;

	for (int i = 0; i < m_pBrushObjs.size(); i++)
	{
		info.Init(vStart, vDelta, aabb.Extents, m_pBrushObjs[i]->GetTrans(),  1, 0.01f);
		if (m_pBrushObjs[i]->AABBTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			fFraction = info.fFraction;
			m_CollidedBrush = m_pBrushObjs[i];
			if (fFraction == 0)
				break;
		}
	}
	if (fFraction < 1)
	{
		if (pNormal)
		{
			mat._41 = mat._42 = mat._43 = 0; 
			*pNormal = ClipPlane.GetNormal() * mat;
			pNormal->Normalize();
		}
		return true;
	}
	return false;
}

bool CBrushesObjMgr::CapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const CCapsule& capsule, A3DVECTOR3 *pNormal, bool& bStartSolid)
{
	CapusuleBrushTraceInfo info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;
	m_CollidedBrush = NULL;
	m_pSinkedBrush = NULL;

	for (int i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (!m_pBrushObjs[i]->IsLoad())
			continue;

		info.Init(vStart, vDelta, capsule.GetRadius(), capsule.GetHeight(), m_pBrushObjs[i]->GetTrans(), capsule.GetUp(), 1);
		if (m_pBrushObjs[i]->CapsuleTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			fFraction = info.fFraction;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			m_CollidedBrush = m_pBrushObjs[i];
			if (fFraction == 0)
			{
				if (bStartSolid)
					m_pSinkedBrush = (CConvexBrush*)info.dwUser2;
				break;
			}
		}
	}
	if (fFraction < 100)
	{
		if (pNormal)
		{
			mat._41 =0, mat._42 = 0, mat._43 = 0;
			*pNormal = ClipPlane.GetNormal() * mat;
			pNormal->Normalize();
		}
		return true;
	}
	return false;
}

bool CBrushesObjMgr::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3 *pNormal, bool& bStartSolid)
{
	RayBrushTraceInfo info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;
	m_CollidedBrush = NULL;
	
	for (int i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (!m_pBrushObjs[i]->IsLoad())
			continue;
		
		info.Init(vStart, vDelta, m_pBrushObjs[i]->GetTrans(), 1, 0.01f);
		if (m_pBrushObjs[i]->RayTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			fFraction = info.fFraction;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			m_CollidedBrush = m_pBrushObjs[i];
			if (fFraction == 0)
				break;
		}
	}
	if (fFraction < 100)
	{
		if (pNormal)
		{
			mat._41 =0, mat._42 = 0, mat._43 = 0;
			*pNormal = ClipPlane.GetNormal() * mat;
			pNormal->Normalize();
		}
		return true;
	}
	return false;
}
#endif

void CBrushesObjMgr::SkipMovingObjCollision(bool bSkip)
{
	for ( int i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (m_pBrushObjs[i]->IsDynObj() && m_pBrushObjs[i]->GetMoving())
			m_pBrushObjs[i]->SkipCollisionDetection(bSkip);
	}
}

void CBrushesObjMgr::SetMoving(bool bMove)
{
	m_isMoving = bMove;
	if (m_pBrushObjs.size() == 0)
		return;

	int i;
	for (i = 0; i < m_pBrushObjs.size(); i++)
		if (m_pBrushObjs[i]->IsDynObj())
			m_pBrushObjs[i]->SetMoving(bMove);
}

abase::vector<CBrushesObj*> CBrushesObjMgr::GetDynamicObjs()
{
	abase::vector<CBrushesObj*> re;
	for ( int i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (m_pBrushObjs[i]->IsDynObj())
			re.push_back(m_pBrushObjs[i]);
	}
	return re;
}

void CBrushesObjMgr::PrepareStart()
{
	for (int i = 0; i < m_pBrushObjs.size(); i++)
	{
		m_pBrushObjs[i]->UpdateTrans();
	}
}
