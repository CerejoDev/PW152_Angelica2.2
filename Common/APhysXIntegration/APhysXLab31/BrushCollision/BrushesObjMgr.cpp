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

#include "BrushesObjMgr.h"
#include "BrushesObj.h"

using namespace CHBasedCD;

#ifdef USING_BRUSH_MAN
	CECBrushMan g_brushMan;
#endif

CBrushesObjMgr::CBrushesObjMgr()
{
	m_isMoving = false;
}

CBrushesObjMgr::~CBrushesObjMgr()
{
	Release();
}

void CBrushesObjMgr::OnNotifyBorn(const IPhysXObjBase& obj)
{
	IPhysXObjBase* pObj = const_cast<IPhysXObjBase*>(&obj);
	AddBrushesObj(pObj);
}
void CBrushesObjMgr::OnNotifyDie(const IPhysXObjBase& obj)
{
	IPhysXObjBase* pObj = const_cast<IPhysXObjBase*>(&obj);
	ReleaseBrushesObj(pObj);
}
void CBrushesObjMgr::OnNotifyWakeUp(const IPhysXObjBase& obj)
{
	IPhysXObjBase* pObj = const_cast<IPhysXObjBase*>(&obj);
	AddBrushesObj(pObj);
}
void CBrushesObjMgr::OnNotifySleep(const IPhysXObjBase& obj)
{
	IPhysXObjBase* pObj = const_cast<IPhysXObjBase*>(&obj);
	ReleaseBrushesObj(pObj);
}

bool CBrushesObjMgr::AddBrushesObj(IPhysXObjBase* pPhysxObj)
{
	assert(pPhysxObj);
	size_t i = 0;
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
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
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

void CBrushesObjMgr::Release()
{
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
	{
		delete m_pBrushObjs[i];
	}
	m_pBrushObjs.clear();
}

void CBrushesObjMgr::Tick(int deltaTime)
{
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
		m_pBrushObjs[i]->Tick(deltaTime);
}

void CBrushesObjMgr::Render(A3DFlatCollector* pFC, DWORD dwFColor)
{
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
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
bool CBrushesObjMgr::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush)
{
	AABBBrushTraceInfo2 info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;

	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
	{
		info.Init(vStart, vDelta, aabb.Extents, m_pBrushObjs[i]->GetTrans(),  1, 0.01f);
		if (m_pBrushObjs[i]->AABBTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			fFraction = info.fFraction;
			if (fFraction == 0)
			{
				if (bStartSolid && ppSinkedBrush)
					*ppSinkedBrush = (CConvexBrush*)info.pUser2;
				break;
			}
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

bool CBrushesObjMgr::CapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const CCapsule& capsule, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush)
{
	CapusuleBrushTraceInfo info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;

	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
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
			if (fFraction == 0)
			{
				if (bStartSolid && ppSinkedBrush)
					*ppSinkedBrush = (CConvexBrush*)info.pUser2;
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

bool CBrushesObjMgr::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush)
{
	RayBrushTraceInfo info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;
	
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
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
			if (fFraction == 0)
			{
				if (bStartSolid && ppSinkedBrush)
					*ppSinkedBrush = (CConvexBrush*)info.pUser2;
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
#endif

void CBrushesObjMgr::SkipMovingObjCollision(bool bSkip)
{
	for ( size_t i = 0; i < m_pBrushObjs.size(); i++)
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

	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
		if (m_pBrushObjs[i]->IsDynObj())
			m_pBrushObjs[i]->SetMoving(bMove);
}

bool CBrushesObjMgr::HasDynamicObjs() const
{
	for (size_t i = 0; i < m_pBrushObjs.size(); ++i)
	{
		if (m_pBrushObjs[i]->IsDynObj())
			return true;
	}
	return false;
}

void CBrushesObjMgr::GetAllDynamicObjs(abase::vector<CBrushesObj*>& outObjs) const
{
	for (size_t i = 0; i < m_pBrushObjs.size(); ++i)
	{
		if (m_pBrushObjs[i]->IsDynObj())
			outObjs.push_back(m_pBrushObjs[i]);
	}
}

void CBrushesObjMgr::PrepareStart()
{
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
		m_pBrushObjs[i]->GetTrans();
}
