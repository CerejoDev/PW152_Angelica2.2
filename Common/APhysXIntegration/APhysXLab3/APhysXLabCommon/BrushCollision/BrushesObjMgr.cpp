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

template <class T>
inline int FindObj(abase::vector<T>& pObjs, T pFindObj)
{
	for (size_t i = 0; i < pObjs.size(); i++)
	{
		if (pObjs[i] == pFindObj)
			return (int)i;
	}
	return -1;
}

CBrushesObjMgr::CBrushesObjMgr()
{
}

CBrushesObjMgr::~CBrushesObjMgr()
{
	Release();
}

void CBrushesObjMgr::Release()
{
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
	{
		delete m_pBrushObjs[i];
	}
	m_pBrushObjs.clear();
	m_pDynObjs.clear();
}

void CBrushesObjMgr::Render(A3DFlatCollector* pFC, DWORD dwFColor)
{
	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
	{
		m_pBrushObjs[i]->Render(pFC, dwFColor);
	}
}

bool CBrushesObjMgr::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush)
{
	AABBBrushTraceInfo2 info;
	A3DMATRIX4 mat;
	fFraction = 100;
	CHalfSpace ClipPlane;

	for (size_t i = 0; i < m_pBrushObjs.size(); i++)
	{
		info.Init(vStart, vDelta, aabb.Extents, m_pBrushObjs[i]->GetTrans(),  m_pBrushObjs[i]->GetScale());
		if (m_pBrushObjs[i]->AABBTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			fFraction = info.fFraction;
			if (fFraction == 0)
			{
				if (bStartSolid && ppSinkedBrush)
				{
#ifdef _ANGELICA3
					*ppSinkedBrush = (CConvexBrush*)info.pUser2;
#else
					//assert(!"To update");
#endif
				}
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

		info.Init(vStart, vDelta, capsule.GetRadius(), capsule.GetHeight(), m_pBrushObjs[i]->GetTrans(), capsule.GetUp(), m_pBrushObjs[i]->GetScale());
		if (m_pBrushObjs[i]->CapsuleTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			fFraction = info.fFraction;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			if (fFraction == 0)
			{
				if (bStartSolid && ppSinkedBrush)
				{
#ifdef _ANGELICA3
					*ppSinkedBrush = (CConvexBrush*)info.pUser2;
#else
					//assert(!"To update");
#endif
				}
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
		
		info.Init(vStart, vDelta, m_pBrushObjs[i]->GetTrans(), m_pBrushObjs[i]->GetScale());
		if (m_pBrushObjs[i]->RayTrace(&info) && info.fFraction < fFraction)
		{
			mat = info.mat;
			fFraction = info.fFraction;
			ClipPlane = info.ClipPlane;
			bStartSolid = info.bStartSolid;
			if (fFraction == 0)
			{
				if (bStartSolid && ppSinkedBrush)
				{
#ifdef _ANGELICA3
					*ppSinkedBrush = (CConvexBrush*)info.pUser2;
#else
					//assert(!"To update");
#endif
				}
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

void CBrushesObjMgr::SkipMovingObjCollision(bool bSkip)
{
	for ( size_t i = 0; i < m_pBrushObjs.size(); i++)
	{
		if (m_pBrushObjs[i]->IsDynObj())
			m_pBrushObjs[i]->SkipCollisionDetection(bSkip);
	}
}


void CBrushesObjMgr::AddBrushesObj(CBrushesObj* pBrushObj)
{
	int index = FindObj(m_pBrushObjs, pBrushObj);
	if (index == -1)
		m_pBrushObjs.push_back(pBrushObj);

}
bool CBrushesObjMgr::RemoveBrushesObj(CBrushesObj* pBrushObj)
{
	int index = FindObj(m_pBrushObjs, pBrushObj);
	if (index != -1)
	{
		m_pBrushObjs.erase(m_pBrushObjs.begin() + index);

		UnRegisterDynBrushObj(pBrushObj);

		return true;
	}
	
	return false;
}

void CBrushesObjMgr::RegisterDynBrushObj(CBrushesObj* pObj)
{
	int index = FindObj(m_pDynObjs, pObj);
	if (index == -1)
	{
		m_pDynObjs.push_back(pObj);
		for(size_t i = 0; i < m_DynObjReceiver.size(); i++)
			m_DynObjReceiver[i]->OnRegisterDynBrushObj(pObj);
	}
}
bool CBrushesObjMgr::UnRegisterDynBrushObj(CBrushesObj* pObj)
{
	int index = FindObj(m_pDynObjs, pObj);
	if (index != -1)
	{
		m_pDynObjs.erase(m_pDynObjs.begin() + index);
		for(size_t i = 0; i < m_DynObjReceiver.size(); i++)
			m_DynObjReceiver[i]->OnUnRegisterDynBrushObj(pObj);

		return true;
	}
	return false;
}

void CBrushesObjMgr::RegisterDynObjReceiver(IDynObjStateReceiver* pDynObjReceiver)
{
	int index = FindObj(m_DynObjReceiver, pDynObjReceiver);
	if (index == -1)
		m_DynObjReceiver.push_back(pDynObjReceiver);
}
void CBrushesObjMgr::UnRegisterDynObjReceiver(IDynObjStateReceiver* pDynObjReceiver)
{
	int index = FindObj(m_DynObjReceiver, pDynObjReceiver);
	if (index != -1)
	{
		m_DynObjReceiver.erase(m_DynObjReceiver.begin() + index);
	}
}
