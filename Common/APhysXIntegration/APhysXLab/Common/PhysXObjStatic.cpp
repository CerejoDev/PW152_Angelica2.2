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

#define APX_CREATE_STATIC_AS_KINEMATIC

CPhysXObjStatic::CPhysXObjStatic() : IPhysXObjBase(CPhysXObjMgr::OBJ_TYPEID_STATIC)
{
	m_pObjModel = 0;
	m_pObjPhysX	= 0;
}

bool CPhysXObjStatic::OnLoadModel(CRender& render, const char* szFile)
{
	A3DLitModel* pModel = new A3DLitModel;
	if (0 == pModel)
	{
		a_LogOutput(1, "CPhysXObjStatic::OnLoadModel: Not enough memory!");
		return false;
	}
	
	if (!pModel->LoadFromMox(render.GetA3DDevice(), szFile))
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

void CPhysXObjStatic::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	assert(0 != m_pObjModel);
	m_pObjModel->Render(&viewport);	
}

void CPhysXObjStatic::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(0 != m_pObjModel);
	m_pObjModel->SetPos(vPos);
}

void CPhysXObjStatic::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(0 != m_pObjModel);
	m_pObjModel->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjStatic::OnSetScale(const float& scale)
{
	assert(0 != m_pObjModel);
	return m_pObjModel->SetScale(scale, scale, scale);
}

bool CPhysXObjStatic::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert(!m_strFilePhysX.IsEmpty());

	APhysXObjectInstanceDesc desc;
	desc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(m_strFilePhysX);
	if (0 == desc.mPhysXObjectDesc)
		return false;

#ifdef APX_CREATE_STATIC_AS_KINEMATIC

	// create a kinematic RB object desc instead...
	APHYSX_ASSERT(desc.mPhysXObjectDesc->IsStaticRB());
	APhysXRigidBodyObjectDesc* pStaticRBObjDesc = (APhysXRigidBodyObjectDesc*)desc.mPhysXObjectDesc;
	APhysXRigidBodyObjectDesc* pKinematicRBObjDesc = pStaticRBObjDesc->CreateRBObjectDesc(APX_OBJTYPE_DYNAMIC);
	desc.mPhysXObjectDesc = pKinematicRBObjDesc;

#endif

	GetGlobalPose(desc.mGlobalPose);
	desc.mScale3D = NxVec3(m_fScale);

	APhysXObject* pNewObj = aPhysXScene.CreatePhysXObject(desc);
	if (0 == pNewObj)
		return false;
	
	ReleasePhysXObj();
	m_pObjPhysX = pNewObj;

#ifdef APX_CREATE_STATIC_AS_KINEMATIC

	APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)pNewObj;
	pRBObj->EnableKinematic();
	delete pKinematicRBObjDesc;

#endif

	return true;
}

void CPhysXObjStatic::OnReleasePhysXObj(const bool RunTimeIsEnd)
{
	if(0 != m_pObjPhysX)
	{
		m_pObjPhysX->GetPhysXScene()->ReleasePhysXObject(m_pObjPhysX);
		m_pObjPhysX = 0;
	}
}

bool CPhysXObjStatic::GetPhysXRigidBodyAABB(NxBounds3& outAABB) const
{
	if ((0 != m_pObjPhysX) && m_pObjPhysX->IsRigidBody())
	{
		APhysXRigidBodyObject* pRB = static_cast<APhysXRigidBodyObject*>(m_pObjPhysX);
		IPhysXObjBase::GetPhysXRigidBodyAABB(*pRB, outAABB);
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

int CPhysXObjStatic::GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor) const
{
	int rtn = 0;
	if (0 == m_pObjPhysX)
		return rtn;
	
	// ? Currently, there is no way to get NxActor from APhysXObject
	return rtn;
}

NxActor* CPhysXObjStatic::GetFirstNxActor() const
{
	if (0 == m_pObjPhysX)
		return 0;
	
	// ? Currently, there is no way to get NxActor from APhysXObject
	return 0;
}

void CPhysXObjStatic::UpdatePos(const A3DVECTOR3& vPos)
{
	IPhysXObjBase::UpdatePos(vPos);

	if(m_pObjPhysX)
	{
		NxMat34 mtPose;
		APhysXConverter::A2N_Matrix44(m_pObjModel->GetAbsoluteTM(), mtPose);
		
		m_pObjPhysX->SetPose(mtPose);

	}
}


void CPhysXObjStatic::UpdateDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	IPhysXObjBase::UpdateDirAndUp(vDir, vUp);

	if(m_pObjPhysX)
	{
		NxMat34 mtPose;
		APhysXConverter::A2N_Matrix44(m_pObjModel->GetAbsoluteTM(), mtPose);
		
		m_pObjPhysX->SetPose(mtPose);

	}

}

bool CPhysXObjStatic::OnTickMove(float deltaTimeSec) 
{ 

#if 0

	if(m_pObjPhysX)
		UpdatePos(GetPos() + A3DVECTOR3(5.0f * deltaTimeSec, 0.0f, 0.0f));
	
#endif

	return true; 

}