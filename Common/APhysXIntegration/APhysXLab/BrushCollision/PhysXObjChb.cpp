/*
 * FILE: PhysXObjChb.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY:  2011/01/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "PhysXObjChb.h"

CPhysXObjChb::CPhysXObjChb() : IPhysXObjBase(CPhysXObjMgr::OBJ_TYPEID_CHB)
{
	m_pObjModel = 0;
	m_pEngine   = 0;
}

bool CPhysXObjChb::OnLoadModel(CRender& render, const char* szFile)
{
	CConvexHullDataSet* pModel = new CConvexHullDataSet;
	if (0 == pModel)
	{
		a_LogOutput(1, "CPhysXObjChb::OnLoadModel: Not enough memory!");
		return false;
	}
	
	CHFSAVERESULT res;
	if (!pModel->Load(szFile, true, res))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjChb::OnLoadModel: Failed to load chb file! (Name: %s)", szFile);
		return false;
	}
	
	OnReleaseModel();
	m_pObjModel = pModel;

	m_strFilePhysX = szFile;
	af_ChangeFileExt(m_strFilePhysX, ".pxd");

	m_pEngine = render.GetA3DEngine();
	return true;
}

void CPhysXObjChb::OnReleaseModel()
{
	A3DRELEASE(m_pObjModel);
}

void CPhysXObjChb::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	assert(0 != m_pObjModel);
	A3DMATRIX4 mat(m_refFrame.GetRotate());
	mat.SetRow(3, m_refFrame.GetPos());
	if (m_pEngine)
		m_pObjModel->Render(m_pEngine->GetA3DFlatCollector(), 1, &mat);
}

void CPhysXObjChb::OnSetPos(const A3DVECTOR3& vPos)
{
}

void CPhysXObjChb::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
}

bool CPhysXObjChb::OnSetScale(const float& scale)
{
	return true;
}

bool CPhysXObjChb::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert(!m_strFilePhysX.IsEmpty() && m_pObjModel);

	abase::vector<APhysXObject*> physxobjs;

	APhysXObjectInstanceDesc desc;
	
	int DescNum = m_pObjModel->GetModelNum();

	for (int i = 0; i < DescNum; i++)
	{
		if (i == 0)
		desc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(m_strFilePhysX);
		else
		{
			CHBMODEL* pChbMode = m_pObjModel->GetCHBModelByIndex(i);
			desc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(m_strFilePhysX, pChbMode->m_sName);
		}
		
		if (0 == desc.mPhysXObjectDesc)
			continue;
		GetGlobalPose(desc.mGlobalPose);
		desc.mScale3D = NxVec3(m_fScale);
		
		APhysXObject* pNewObj = aPhysXScene.CreatePhysXObject(desc);
		if (0 == pNewObj)
			continue;
		physxobjs.push_back(pNewObj);
	}

	if (physxobjs.size() == 0)
	{
		return false;
	}
	
	m_pAllObjPhysX = physxobjs;
	return true;
}

void CPhysXObjChb::OnReleasePhysXObj(const bool RunTimeIsEnd)
{
	for (int i = 0; i < m_pAllObjPhysX.size(); i++)
	{
		m_pAllObjPhysX[i]->GetPhysXScene()->ReleasePhysXObject(m_pAllObjPhysX[i]);
	}
	m_pAllObjPhysX.clear();
}

bool CPhysXObjChb::GetPhysXRigidBodyAABB(NxBounds3& outAABB) const
{
	outAABB.setEmpty();
	for (int i = 0; i < m_pAllObjPhysX.size(); i++)
	{
		if (m_pAllObjPhysX[i]->IsRigidBody())
		{
			//
			NxBounds3 aabb;
			APhysXRigidBodyObject* pRB = static_cast<APhysXRigidBodyObject*>(m_pAllObjPhysX[i]);
			IPhysXObjBase::GetPhysXRigidBodyAABB(*pRB, aabb);
			outAABB.combine(aabb);
		}
	}
	if (!outAABB.isEmpty())
		return true;
		
	return false;
}

bool CPhysXObjChb::OnGetModelAABB(A3DAABB& outAABB) const
{
	if (0 == m_pObjModel)
		return false;
	
	outAABB = m_pObjModel->GetAABB();
	outAABB.CompleteMinsMaxs();
	return true;
}

int CPhysXObjChb::GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor) const
{
	return 0;
}

NxActor* CPhysXObjChb::GetFirstNxActor() const
{
	return 0;
}
