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
#include "CConvexHullDataSetManager.h"
#include "BrushesObj.h"
#include "Collision.h"
#include "BrushObjAttr.h"

CHBasedCD::CConvexHullDataSetManager g_ChbMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrushesObj::CBrushesObj(): m_pPhysxObj(NULL), m_bLoad(false), m_strFile(""), m_pChbEntity(NULL),m_pConvexDataSet(NULL)
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
	IPropPhysXObjBase* pProp = pPhysxObj->GetProperties();
	AString strFile = pProp->GetFilePathName();

	af_ChangeFileExt(strFile, ".chb");
	
	Release();
	
	m_strFile = "";
	m_pPhysxObj = pPhysxObj;

	if (!pProp->IsActive())//不加载
	{
		m_strFile = strFile;
		return true;
	}

	return (DynamicLoad(strFile));
}

void CBrushesObj::Release()
{
	if(!m_bLoad)
		return;
	m_bLoad = false;

#ifdef USING_BRUSH_MAN
	g_brushMan.RemoveProvider(&m_building);
	m_building.Release();
#else
	if (m_pConvexDataSet)
	{
		g_ChbMgr.ReleaseChb(m_strFile);     
		m_pConvexDataSet = NULL;
	}
	if (m_pChbEntity)
	{
		delete m_pChbEntity;
		m_pChbEntity = NULL;
	}	
#endif
}



#ifdef USING_BRUSH_MAN
bool CBrushesObj::DynamicLoad(const AString& strFile)
{
	g_brushMan.RemoveProvider(&m_building);
	m_bLoad = m_building.Load(strFile);
	m_strFile = strFile;
	g_brushMan.AddProvider(&m_building);

	const float fscale = m_pPhysxObj->GetProperties()->GetScale();
	A3DMATRIX4 scale;
	scale.Scale(fscale, fscale, fscale);
	A3DMATRIX4 matABS = scale * GetTrans();
	m_building.SetTransMat(matABS);

	return m_bLoad;
}
#else
bool CBrushesObj::DynamicLoad(const AString& strFile)
{	
	CHBasedCD::CConvexHullDataSet* pCHDS = g_ChbMgr.LoadChb(strFile, true);
	if (0 == pCHDS)
		return false;

	m_strFile = strFile;
	m_bLoad = true;
	m_pConvexDataSet = pCHDS;
	delete m_pChbEntity;
	m_pChbEntity = 0;

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

	const float fscale = m_pPhysxObj->GetProperties()->GetScale();
	A3DMATRIX4 scale;
	scale.Scale(fscale, fscale, fscale);
	A3DMATRIX4 matABS = scale * GetTrans();
	m_BrushAABBWorld = m_pConvexDataSet->GetTransformedAABB(matABS);
	return true;
}
#endif

void CBrushesObj::Tick(int deltatime)
{
	if (m_pPhysxObj == NULL || m_strFile == "")
		return;

	IPropPhysXObjBase* pProp = m_pPhysxObj->GetProperties();
	if (pProp->IsActive() && !m_bLoad)
	{
		DynamicLoad(m_strFile);
		return;
	}
	
	if (!pProp->IsActive() && m_bLoad)
	{
		Release();
		return;
	}

	if (m_pDynAttr && m_isMoveing && m_bLoad)
	{
		A3DVECTOR3 vPos = pProp->GetPos();
		A3DVECTOR3 vDir = pProp->GetDir();
		m_pDynAttr->TickMove(deltatime, vPos, vDir);

		A3DMATRIX4 mat;
		A3DVECTOR3 vUp = pProp->GetUp();
		A3DVECTOR3 vRight = CrossProduct(vUp, vDir);
		mat.SetRow(0, vRight);
		mat.SetRow(1, vUp);
		mat.SetRow(2, vDir);
		mat.SetRow(3, vPos);
		pProp->SetPose(mat);
	}
	GetTrans();
}

void CBrushesObj::Render(A3DFlatCollector* pFC, DWORD dwFColor/* =0xa0ffff00 */)
{
	float scale = m_pPhysxObj->GetProperties()->GetScale();
	#ifdef USING_BRUSH_MAN
		m_building.DebugRender(pFC, scale, dwFColor, A3DVECTOR3(1, 1, 1), &m_trans);
	#else
		if (m_pChbEntity)
	  		m_pChbEntity->DebugRender(pFC, scale, dwFColor, A3DVECTOR3(1, 1, 1), &m_trans);
	#endif
}

bool CBrushesObj::AABBTrace(CHBasedCD::AABBBrushTraceInfo2* pInfo)
{
	pInfo->fScale = m_pPhysxObj->GetProperties()->GetScale();
	if (m_pChbEntity == NULL)
		return false;

	if (m_pDynAttr == NULL)//静态物体
	{
		if (!AABBAABBCollision(pInfo->BoundAABB, m_BrushAABBWorld))
			return false;
	}
	return m_pChbEntity->Trace(pInfo);
}

bool CBrushesObj::CapsuleTrace(CHBasedCD::CapusuleBrushTraceInfo* pInfo)
{
	pInfo->fScale = m_pPhysxObj->GetProperties()->GetScale();
	if (m_pChbEntity == NULL)
		return false;

	if (m_pDynAttr == NULL)//静态物体
	{
		if (!AABBAABBCollision(pInfo->BoundAABB, m_BrushAABBWorld))
			return false;
	}
	return m_pChbEntity->Trace(pInfo);
}

bool CBrushesObj::RayTrace(CHBasedCD::RayBrushTraceInfo* pInfo)
{
	pInfo->fScale = m_pPhysxObj->GetProperties()->GetScale();
	if (m_pChbEntity == NULL)
		return false;
	
	A3DVECTOR3 vPoint, vNormal;
	float fraction;
	
	if (!CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, m_BrushAABBWorld.Mins, m_BrushAABBWorld.Maxs, vPoint, &fraction, vNormal))
		return false;
	
	return m_pChbEntity->Trace(pInfo);
}

const A3DMATRIX4& CBrushesObj::GetTrans()
{
	if (m_pPhysxObj && m_bLoad)
	{
		m_trans = m_pPhysxObj->GetProperties()->GetPose();
		return m_trans;
	}

	m_trans.Identity();
	return m_trans;
}

void CBrushesObj::SkipCollisionDetection(bool bSkip)
{
	if (bSkip)
		m_pChbEntity->SetFlag(CHDATA_SKIP_COLLISION);
	else
		m_pChbEntity->ClearFlag(CHDATA_SKIP_COLLISION);
}

void CBrushesObj::SetPos(const A3DVECTOR3& vPos)
{
	m_pPhysxObj->GetProperties()->SetPos(vPos); 
}

A3DVECTOR3 CBrushesObj::GetPos() const
{
	return m_pPhysxObj->GetProperties()->GetPos(); 
}

void CBrushesObj::AddBrushAttr(BrushObjAttr* pBrushAttr)
{
	delete m_pDynAttr;
	m_pDynAttr = new BrushObjAttr(*pBrushAttr);
}
