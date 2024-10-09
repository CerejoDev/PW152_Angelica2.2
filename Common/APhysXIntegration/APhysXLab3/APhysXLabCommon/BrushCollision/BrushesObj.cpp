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

#include "ChbEntity.h"
#include "BrushesObj.h"
#include "Collision.h"
#include "BrushesObjMgr.h"
#include "EngineInterface.h"

#ifdef _ANGELICA3
	#include "CConvexHullDataSetManager.h"
	CHBasedCD::CConvexHullDataSetManager g_ChbMgr;
#else
	#include "ConvexHullArchive.h"
#endif

// extern A3DAABB g_aabb;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrushesObj::CBrushesObj(): m_bLoad(false), m_pChbEntity(NULL)
{
	m_UserData = 0;
	m_strFile.Empty();

#ifdef _ANGELICA3
	m_pConvexDataSet = NULL; 
#endif

	RaiseFlag(Dirty_AABB);
	ClearFlag(Dynamic_Obj);
}

CBrushesObj::~CBrushesObj()
{
	Release();	
}

void CBrushesObj::Release()
{
	if(!m_bLoad)
		return;
	m_bLoad = false;

#ifdef _ANGELICA3
	if (m_pConvexDataSet)
	{
		g_ChbMgr.ReleaseChb(m_strFile);     
		m_pConvexDataSet = NULL;
	}
#endif

	if (m_pChbEntity)
	{
		delete m_pChbEntity;
		m_pChbEntity = NULL;
	}
	RaiseFlag(Dirty_AABB);

	//notify brush object manager
	CBrushesObjMgr::GetInstance()->RemoveBrushesObj(this);
}


bool CBrushesObj::Load(const A3DMATRIX4& rotTrans, float fScale)//load brush file(.chb)
{
	if (m_bLoad)
		return true;

	
#if defined(_ANGELICA3)
	m_pConvexDataSet = NULL;

	CHBasedCD::CConvexHullDataSet* pCHDS = g_ChbMgr.LoadChb(m_strFile, true);
	if (0 == pCHDS)
		return false;

	m_pConvexDataSet = pCHDS;
	m_fScale = fScale;
	m_trans = rotTrans;

	abase::vector<CHBasedCD::CConvexBrush*> brushes;
	for (int i=0; i<m_pConvexDataSet->GetModelNum(); i++)
	{
		CHBasedCD::CHBMODEL* pCHBModel = m_pConvexDataSet->GetCHBModelByIndex(i);
		if (pCHBModel)
		{
			for (int j=0; j<(int)pCHBModel->m_pCHBrushes.size(); j++)
			{
				CHBasedCD::CConvexBrush* pBrush = pCHBModel->m_pCHBrushes.at(j);				
				brushes.push_back(pBrush);
			}
		}
	}

	m_pChbEntity = new CHBasedCD::CChbEntity;
	m_pChbEntity->Init(brushes, true, false);//use collision tree, not clean up brush
#elif defined(_ANGELICA21)
	AFile af;
	if (!af.Open(m_strFile, AFILE_OPENEXIST | AFILE_BINARY))
		return true;

	CHBasedCD::ConvexHullArchive ar;
	if (!ar.LoadHeader(af))
		return true;
	abase::vector<CHBasedCD::CConvexHullData*> chdatas;
	for (int i = 0; i < ar.GetNumHull(); i++)
	{
		CHBasedCD::CConvexHullData* pConvex = new CHBasedCD::CConvexHullData;
		ar.LoadHull(af, pConvex);
		chdatas.push_back(pConvex);
	}
	af.Close();
	m_pChbEntity = new CHBasedCD::CChbEntity;
	m_pChbEntity->Import(chdatas);
	
	m_fScale = fScale;
	m_trans = rotTrans;

	for (int i = 0; i < ar.GetNumHull(); i++)
	{
		delete chdatas[i];
	}
#endif

	UpdateBrushAABBWorld();

	//notify brush object manager
	CBrushesObjMgr::GetInstance()->AddBrushesObj(this);
	CBrushesObjMgr::GetInstance()->UnRegisterDynBrushObj(this);//static object by default

	ClearFlag(Dynamic_Obj);

	m_bLoad = true;

	return true;
}

void CBrushesObj::Render(A3DFlatCollector* pFC, int dwFColor/* =0xa0ffff00 */)
{
	if (m_pChbEntity)
		m_pChbEntity->DebugRender(pFC, m_fScale, dwFColor, A3DVECTOR3(1, 1, 1), &m_trans);
}

bool CBrushesObj::AABBTrace(CHBasedCD::AABBBrushTraceInfo2* pInfo)
{
	if (m_pChbEntity == NULL)
		return false;

	if (!ReadFlag(Dirty_AABB))//aabb validate
	{
		if (!AABBAABBCollision(pInfo->BoundAABB, m_BrushAABBWorld))
			return false;
	}

#if defined(_ANGELICA3)
	return m_pChbEntity->Trace(pInfo);
#elif defined(_ANGELICA21)
	return m_pChbEntity->AABBTrace(pInfo);
#endif

	return false;
}

bool CBrushesObj::CapsuleTrace(CHBasedCD::CapusuleBrushTraceInfo* pInfo)
{
	if (m_pChbEntity == NULL)
		return false;

	if (!ReadFlag(Dirty_AABB))//aabb validate
	{
#if defined(_ANGELICA3)
		if (!AABBAABBCollision(pInfo->BoundAABB, m_BrushAABBWorld))
#elif defined(_ANGELICA21)
		if (!CapsuleAABBCollision(pInfo, m_pChbEntity->GetAABB()))
#endif
			return false;
	}
#if defined(_ANGELICA3)
	return m_pChbEntity->Trace(pInfo);
#elif defined(_ANGELICA21)
	return m_pChbEntity->CapusuleTrace(pInfo);
#endif

	return false;
}

bool CBrushesObj::RayTrace(CHBasedCD::RayBrushTraceInfo* pInfo)
{
	if (m_pChbEntity == NULL)
		return false;
	
	if (!ReadFlag(Dirty_AABB))//aabb validate
	{
		A3DVECTOR3 vPoint, vNormal;
		float fraction;

		if (!CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, m_BrushAABBWorld.Mins, m_BrushAABBWorld.Maxs, vPoint, &fraction, vNormal))
			return false;
	}
#if defined(_ANGELICA3)
	return m_pChbEntity->Trace(pInfo);
#elif defined(_ANGELICA21)
	return m_pChbEntity->RayTrace(pInfo);
#endif
	return false;
}

const A3DMATRIX4& CBrushesObj::GetTrans()
{
	return m_trans;
}

void CBrushesObj::SkipCollisionDetection(bool bSkip)
{
	if (bSkip)
		m_pChbEntity->SetFlag(CHDATA_SKIP_COLLISION);
	else
		m_pChbEntity->ClearFlag(CHDATA_SKIP_COLLISION);
}

void CBrushesObj::UpdateBrushAABBWorld()
{
	if (ReadFlag(Dirty_AABB))
	{
#ifdef _ANGELICA3
		if (m_pConvexDataSet == NULL)
			return;
#endif

		A3DMATRIX4 scale;
		scale.Scale(m_fScale, m_fScale, m_fScale);
		A3DMATRIX4 matABS = scale * m_trans;
#ifdef _ANGELICA3
		m_BrushAABBWorld = m_pConvexDataSet->GetTransformedAABB(matABS);
#else
		m_BrushAABBWorld = m_pChbEntity->GetTransformedAABB(matABS);
		// g_aabb = m_BrushAABBWorld;
#endif
		ClearFlag(Dirty_AABB);
	}
}

void CBrushesObj::SetStrFile(const char* strFile)
{
	if (strFile == NULL)
	{
		if (!m_strFile.IsEmpty())
		{
			Release();
			m_strFile.Empty();
		}		
	}
	else
	{
		if (m_strFile != strFile)
		{
			Release();
			m_strFile = strFile;
		}		
	}
}

bool CBrushesObj::IsDynObj() const
{
	return ReadFlag(Dynamic_Obj);
}

void CBrushesObj::SetRotTrans(const A3DMATRIX4& rotTrans)
{ 	
	m_trans = rotTrans; 
	
	if (!ReadFlag(Dynamic_Obj))
		CBrushesObjMgr::GetInstance()->RegisterDynBrushObj(this);

	RaiseFlag(Dirty_AABB);
	RaiseFlag(Dynamic_Obj);
	
}

void CBrushesObj::SetPosition(const A3DVECTOR3& vPos) 
{
	Mat44_SetTransPart(vPos, m_trans);
	if (!ReadFlag(Dynamic_Obj))
		CBrushesObjMgr::GetInstance()->RegisterDynBrushObj(this);

	RaiseFlag(Dirty_AABB); 
	RaiseFlag(Dynamic_Obj);
}

void CBrushesObj::SetScale(float fScale) 
{ 
	m_fScale = fScale; 
	RaiseFlag(Dirty_AABB); 
}