/*
 * FILE: PhysXObjStatic.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

CPhysXObjStatic::CPhysXObjStatic(int UID)
{
	m_pObjModel = 0;
	m_pObjPhysX	= 0;
	m_propStatic.InitTypeIDAndHostObject(RawObjMgr::OBJ_TYPEID_STATIC, *this, UID);
	SetProperties(m_propStatic);
}

bool CPhysXObjStatic::OnLoadModel(IEngineUtility& eu, const char* szFile)
{
	A3DLitModel* pModel = new A3DLitModel;
	if (0 == pModel)
	{
		a_LogOutput(1, "CPhysXObjStatic::OnLoadModel: Not enough memory!");
		return false;
	}
	
	if (!pModel->LoadFromUMD(eu.GetA3DDevice(), szFile))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjStatic::OnLoadModel: Failed to load mox file! (Name: %s)", szFile);
		return false;
	}
	
	OnReleaseModel();
	m_pObjModel = pModel;

	m_strFilePhysX = szFile;
	af_ChangeFileExt(m_strFilePhysX, ".pxd");
	return true;
}

void CPhysXObjStatic::OnReleaseModel()
{
	A3DRELEASE(m_pObjModel);
}

void CPhysXObjStatic::OnRender(A3DViewport& viewport, bool bDoVisCheck) const
{
	assert(0 != m_pObjModel);
	if(bDoVisCheck)
		m_pObjModel->Render(&viewport);	
	else
		m_pObjModel->Render(&viewport, A3DLitModel::VIS_NOT_CHECK);	
}

bool CPhysXObjStatic::OnGetPos(A3DVECTOR3& vOutPos) const
{
	if (0 == m_pObjModel)
		return false;

	vOutPos = m_pObjModel->GetPos();
	return true; 
}

void CPhysXObjStatic::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(0 != m_pObjModel);
	m_pObjModel->SetPos(vPos);
}

bool CPhysXObjStatic::OnGetDir(A3DVECTOR3& vOutDir) const
{
	if (0 == m_pObjModel)
		return false;

	vOutDir = m_pObjModel->GetDir();
	return true; 
}

bool CPhysXObjStatic::OnGetUp(A3DVECTOR3& vOutUp) const 
{
	if (0 == m_pObjModel)
		return false;

	vOutUp = m_pObjModel->GetUp();
	return true;
}

void CPhysXObjStatic::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(0 != m_pObjModel);
	m_pObjModel->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjStatic::OnGetPose(A3DMATRIX4& matOutPose) const
{
	if (0 == m_pObjModel)
		return false;

	matOutPose = m_pObjModel->GetAbsoluteTM();
	return true;
}

void CPhysXObjStatic::OnSetPose(const A3DMATRIX4& matPose)
{
	assert(0 != m_pObjModel);
	OnSetPos(matPose.GetRow(3));
	OnSetDirAndUp(matPose.GetRow(2), matPose.GetRow(1));
}

bool CPhysXObjStatic::OnSetScale(const float& scale)
{
	assert(0 != m_pObjModel);
	return m_pObjModel->SetScale(scale, scale, scale);
}

bool CPhysXObjStatic::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert(!m_strFilePhysX.IsEmpty());
	if (!m_propStatic.GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY))
		return true;

	APhysXObjectInstanceDesc desc;
	desc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(m_strFilePhysX);
	if (0 == desc.mPhysXObjectDesc)
		return false;

	IPropObjBase* pObjProps = GetProperties();
	A3DMATRIX4 mat = pObjProps->GetPose(true);
	APhysXConverter::A2N_Matrix44(mat, desc.mGlobalPose);
	desc.mScale3D = NxVec3(pObjProps->GetScale());

	APhysXObject* pNewObj = aPhysXScene.CreatePhysXObject(desc);
	if (0 == pNewObj)
		return false;
	
	ReleasePhysXObj();
	m_pObjPhysX = pNewObj;
	return true;
}

void CPhysXObjStatic::OnReleasePhysXObj()
{
	if(0 != m_pObjPhysX)
	{
		m_pObjPhysX->GetPhysXScene()->ReleasePhysXObject(m_pObjPhysX);
		m_pObjPhysX = 0;
	}
}

bool CPhysXObjStatic::GetAPhysXInstanceAABB(NxBounds3& outAABB) const
{
	if ((0 != m_pObjPhysX) && m_pObjPhysX->IsRigidBody())
	{
		APhysXRigidBodyObject* pRB = static_cast<APhysXRigidBodyObject*>(m_pObjPhysX);
		pRB->GetAABB(outAABB);
		int nCount = pRB->GetNxActorNum();
		for (int i = 0; i < nCount; ++i)
			outAABB.include(pRB->GetNxActor(i)->getGlobalPosition());
		return true;
	}
	
	return false;
}

bool CPhysXObjStatic::OnGetModelAABB(A3DAABB& outAABB) const
{
	if (0 == m_pObjModel)
		return false;
	
	outAABB = m_pObjModel->GetModelAABB();
	outAABB.CompleteMinsMaxs();
	return true;
}

int CPhysXObjStatic::GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor) const
{
	if (0 == m_pObjPhysX)
		return 0;
	
	if (m_pObjPhysX->IsRigidBody())
	{
		APhysXRigidBodyObject* pRB = static_cast<APhysXRigidBodyObject*>(m_pObjPhysX);
		if (0 != pRB)
		{
			int nbActor = pRB->GetNxActorNum();
			for (int i = 0; i < nbActor; ++i)
				outVec.Add(pRB->GetNxActor(i));
			return nbActor;
		}
	}

	return 0;
}
/*
NxActor* CPhysXObjStatic::GetFirstNxActor() const
{
	if (0 == m_pObjPhysX)
		return 0;
	
	// ? Currently, there is no way to get NxActor from APhysXObject
	return 0;
}
*/