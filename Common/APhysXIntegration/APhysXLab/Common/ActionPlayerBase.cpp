/*
 * FILE: ActionPlayerBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/07/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */
#include "stdafx.h"
extern bool gEnableTestCCPerformance;

void CCHitReport::onOverlapBegin(const APhysXCCOverlap& ol)
{
//	OutputDebugString(_T("---onOverlapBegin---\n"));
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(ol.controller->userData);
	if (0 != pDynObj)
	{
		A3DModelPhysics* p3DMP = pDynObj->GetA3DModelPhysics();
		if (0 != p3DMP)
		{
			int nSS = p3DMP->GetSkinSlotNum();
			for (int i = 0; i < nSS; ++i)
			{
				A3DSkinPhysSync* p3DSPS = p3DMP->GetSkinPhysSync(i);
				if (0 != p3DSPS)
				{
					APhysXClothAttacher* pCA = p3DSPS->GetClothAttacher();
					if (0 != pCA)
						pCA->EnableCollision(false);
				}
			}
		}
	}
	
/*	for (APhysXU32 i = 0; i < nbEntities; ++i)
	{
		A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)entities[i]->mUserData;
		A3DModelPhysics* p3DMP = pSkinModelPhysSync->GetA3DModelPhysics();
		p3DMP->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_INDEPENDENT, 0);
		m_APhysXObjs.push_back(entities[i]);
	}
*/
}

void CCHitReport::onOverlapContinue(const APhysXCCOverlap& ol)
{
//	OutputDebugString(_T("---onOverlapContinue---\n"));
}

void CCHitReport::onOverlapEnd(APhysXCharacterController& APhysXCC)
{
//	OutputDebugString(_T("---onOverlapEnd---\n"));
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(APhysXCC.userData);
	if (0 != pDynObj)
	{
		A3DModelPhysics* p3DMP = pDynObj->GetA3DModelPhysics();
		if (0 != p3DMP)
		{
			int nSS = p3DMP->GetSkinSlotNum();
			for (int i = 0; i < nSS; ++i)
			{
				A3DSkinPhysSync* p3DSPS = p3DMP->GetSkinPhysSync(i);
				if (0 != p3DSPS)
				{
					APhysXClothAttacher* pCA = p3DSPS->GetClothAttacher();
					if (0 != pCA)
						pCA->EnableCollision(true);
				}
			}
		}
	}

/*	NxU32 nCount = m_APhysXObjs.size(); 
	for (NxU32 i = 0; i < nCount; ++i)
	{
		A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)m_APhysXObjs[i]->mUserData;
		A3DModelPhysics* p3DMP = pSkinModelPhysSync->GetA3DModelPhysics();
		p3DMP->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON, 0);
	}
	m_APhysXObjs.clear();
*/
}

IActionPlayerBase::IActionPlayerBase(CPhysXObjDynamic& objDyn, const unsigned long sleepTime)
{
	m_pObjDyn = &objDyn;
	m_pModel = objDyn.GetSkinModel();

	FinishCurrentAction();
	m_SleepTime = sleepTime;
	m_pAPhysXCC = 0;
	m_pAPhysXLWCC = 0;
	m_pAPhysXDynCC = 0;
	m_pAPhysXCCBase = 0;
	m_pBrushCC      = 0; 
	m_pAPhysXNxCC   = 0;
}

IActionPlayerBase::~IActionPlayerBase()
{
	ReleaseAllCC();
}

void IActionPlayerBase::ReleaseAllCC()
{
	ReleaseAPhysxCC();
	ReleaseAPhysxLWCC();
	ReleaseAPhysxDynCC();
	ReleaseBrushCC();
	ReleaseAPhysxNxCC();
}

bool IActionPlayerBase::CCPosAvailable(APhysXScene& scene, int ccType)
{
	A3DVECTOR3 pos(m_pObjDyn->GetPos());
	
	const float yOffset = 0.03f;
	

	bool bHasSetCNL = false;
	if ((IPhysXObjBase::OBPI_HAS_APHYSX_CC & ccType) || (IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC & ccType) 
		|| (IPhysXObjBase::OBPI_HAS_APHYSX_LWCC & ccType) || (IPhysXObjBase::OBPI_HAS_APHYSX_NXCC & ccType))
	{
		APhysXCollisionChannel cnl = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
		if (m_pObjDyn->GetCollisionChannel(cnl))
		{
			if ((cnl > APX_COLLISION_CHANNEL_INVALID) && (cnl != APX_COLLISION_CHANNEL_CC_COMMON_HOST))
			{
				bHasSetCNL = true;
				m_pObjDyn->SetIndependentCollisionChannel(true);
			}
		}

		NxVec3 volume(0.3f, 1.6f, 0.3f);
		NxVec3 posFoot = APhysXConverter::A2N_Vector3(pos);
		if (!APhysXCCBase::CheckOverlap(scene, posFoot, volume, APhysXCCBase::APX_VT_CAPSULE))
			return true;
		
		//step up and have another try
		posFoot.y += yOffset;
		if (!APhysXCCBase::CheckOverlap(scene, posFoot, volume, APhysXCCBase::APX_VT_CAPSULE))
			return true;

		if (bHasSetCNL)
		{
			if (APX_COLLISION_CHANNEL_COMMON == cnl)
				m_pObjDyn->SetDefaultCollisionChannel();
			else
				m_pObjDyn->SetIndependentCollisionChannel(false);
		}
		return false;
	}
	if (IPhysXObjBase::OBPI_HAS_BRUSH_CC & ccType)
	{
		pos.y += 0.8f;
		const float radius = 0.3f;
		const float height = 1.f;//胶囊的柱体部分高度，不包括上下半球
		if (BrushcharCtrler::IsPosAvailable(pos, radius, height))
			return true;
		
		//step up and have another try
		pos.y += yOffset; 
		if (BrushcharCtrler::IsPosAvailable(pos, radius, height))
		{
			return true;
		}
		return false;
	}
	return false;
}

void IActionPlayerBase::PlayAction(const unsigned long deltaTime, const bool FirstAction)
{
	// Note: main actor must be runtime only! Or it can't be reset pose to original edited position.
	if (m_pObjDyn->ReadPeopleInfo(IPhysXObjBase::OBPI_IS_MAIN_ACTOR))
	{
		assert(true == m_pObjDyn->ReadFlag(IPhysXObjBase::OBF_RUNTIME_ONLY));
	}

	if (0 != m_pAPhysXCC)
	{
		A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXCC->GetFootPosition());
		A3DVECTOR3 dir = APhysXConverter::N2A_Vector3(m_pAPhysXCC->GetHorizonDir());
		m_pObjDyn->SetPos(pos);
		m_pObjDyn->SetDirAndUp(dir, A3DVECTOR3(0, 1, 0));
	}

	if (0 != m_pAPhysXLWCC)
	{
		A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXLWCC->GetFootPosition());
		m_pObjDyn->SetPos(pos);
	}

	if (0 != m_pBrushCC)
	{
		m_pObjDyn->SetPos(m_pBrushCC->GetFootPosition());
		A3DVECTOR3 vDirH(m_pBrushCC->GetDir());
		vDirH.y = 0;
		vDirH.Normalize();
		if (!vDirH.IsZero())
			m_pObjDyn->SetDirAndUp(vDirH, A3DVECTOR3(0, 1, 0));
	}
	if (0 != m_pAPhysXNxCC)
	{
		A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXNxCC->GetFootPosition());
		A3DVECTOR3 dir = APhysXConverter::N2A_Vector3(m_pAPhysXNxCC->GetHorizonDir());
		m_pObjDyn->SetPos(pos);
		m_pObjDyn->SetDirAndUp(dir, A3DVECTOR3(0, 1, 0));
	}

	OnPlayAction(deltaTime, FirstAction);
}

void IActionPlayerBase::FinishCurrentAction()
{
	m_ElapsedTime = (std::numeric_limits<int>::max)();
	m_ActionTime = 0;
	OnFinishCurrentAction();
}

void IActionPlayerBase::OnAfterPhysXSim()
{
	if (0 != m_pAPhysXDynCC)
	{
		m_pAPhysXDynCC->RefeshCCBySimResult();
		A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetFootPosition());
		m_pObjDyn->SetPos(pos);
		A3DVECTOR3 dir = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetDir());
		A3DVECTOR3 up = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetUpDir());
		m_pObjDyn->SetDirAndUp(dir, up);
	}

//	UpdateInfoFromCC();
}

bool IActionPlayerBase::CreateAPhysxDynCC(APhysXScene& scene, const float yOffset)
{
	APhysXCharacterControllerManager* pCCM = scene.GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);
	
	APhysXCCCapsuleDesc desc;
	desc.height = 1.6f;
	desc.radius = 0.3f;
	desc.stepOffset = 0.3f;
	desc.posFoot = APhysXConverter::A2N_Vector3(m_pObjDyn->GetPos());
	desc.posFoot.y += yOffset;
	desc.flags = APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE;

	bool bHasSetCNL = false;
	APhysXCollisionChannel cnl = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
	if (m_pObjDyn->GetCollisionChannel(cnl))
	{
		if ((cnl > APX_COLLISION_CHANNEL_INVALID) && (cnl != APX_COLLISION_CHANNEL_CC_COMMON_HOST))
		{
			bHasSetCNL = true;
			m_pObjDyn->SetIndependentCollisionChannel(true);
		}
	}

	NxVec3 volume(0.3f, 1.6f, 0.3f);
	const float stepUp = 0.03f;
	if (APhysXCCBase::CheckOverlap(scene, desc.posFoot, volume, APhysXCCBase::APX_VT_CAPSULE))
		desc.posFoot.y += stepUp;

	if (bHasSetCNL)
	{
		if (APX_COLLISION_CHANNEL_COMMON == cnl)
			m_pObjDyn->SetDefaultCollisionChannel();
		else
			m_pObjDyn->SetIndependentCollisionChannel(false);
	}
	
	m_pAPhysXCCBase = m_pAPhysXDynCC = pCCM->CreateDynCC(scene, desc);
	if (0 == m_pAPhysXDynCC)
		return false;
	
	A3DVECTOR3 dir, up;
	m_pObjDyn->GetDirAndUp(dir, up);
	m_pAPhysXDynCC->SetDir(APhysXConverter::A2N_Vector3(dir));
	m_pAPhysXDynCC->userData = m_pObjDyn;
	m_pAPhysXDynCC->SetStepOffset(0.8f);
	RegisterHostModels();
	CreateAPhysxCCPost();
	return true;
}

void IActionPlayerBase::ReleaseAPhysxDynCC()
{
	if (0 == m_pAPhysXDynCC)
		return;
	
	UnRegisterHostModels(*m_pAPhysXDynCC);
	APhysXCharacterControllerManager& ccm = m_pAPhysXDynCC->GetCCManager();
	if (ccm.userData == m_pAPhysXDynCC)
		ccm.userData = 0;
	ccm.ReleaseDynCC(m_pAPhysXDynCC);
	if (m_pAPhysXCCBase == m_pAPhysXDynCC)
		m_pAPhysXCCBase = 0;
	m_pAPhysXDynCC = 0;
}

bool IActionPlayerBase::CreateAPhysxLWCC(APhysXScene& scene, const float yOffset)
{
	APhysXCharacterControllerManager* pCCM = scene.GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXLWCCDesc desc;
	desc.height = 1.6f;
	desc.radius = 0.3f;
	desc.posFoot = APhysXConverter::A2N_Vector3(m_pObjDyn->GetPos());
	desc.posFoot.y += yOffset;

	bool bHasSetCNL = false;
	APhysXCollisionChannel cnl = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
	if (m_pObjDyn->GetCollisionChannel(cnl))
	{
		if ((cnl > APX_COLLISION_CHANNEL_INVALID) && (cnl != APX_COLLISION_CHANNEL_CC_COMMON_HOST))
		{
			bHasSetCNL = true;
			m_pObjDyn->SetIndependentCollisionChannel(true);
		}
	}
	
	NxVec3 volume(0.3f, 1.6f, 0.3f);
	const float stepUp = 0.03f;
	if (APhysXCCBase::CheckOverlap(scene, desc.posFoot, volume, APhysXCCBase::APX_VT_CAPSULE))
		desc.posFoot.y += stepUp;
	
	if (bHasSetCNL)
	{
		if (APX_COLLISION_CHANNEL_COMMON == cnl)
			m_pObjDyn->SetDefaultCollisionChannel();
		else
			m_pObjDyn->SetIndependentCollisionChannel(false);
	}

	m_pAPhysXCCBase = m_pAPhysXLWCC = pCCM->CreateLWCC(scene, desc);
	if (0 == m_pAPhysXLWCC)
		return false;
	
	m_pAPhysXLWCC->userData = m_pObjDyn;
	m_pAPhysXLWCC->SetDetectionDistance(5.0f);
	RegisterHostModels();
	return true;
}

void IActionPlayerBase::ReleaseAPhysxLWCC()
{
	if (0 == m_pAPhysXLWCC)
		return;

	UnRegisterHostModels(*m_pAPhysXLWCC);
	APhysXCharacterControllerManager& ccm = m_pAPhysXLWCC->GetCCManager();
	if (ccm.userData == m_pAPhysXLWCC)
		ccm.userData = 0;
	ccm.ReleaseLWCC(m_pAPhysXLWCC);
	if (m_pAPhysXCCBase == m_pAPhysXLWCC)
		m_pAPhysXCCBase = 0;
	m_pAPhysXLWCC = 0;
}

bool IActionPlayerBase::CreateAPhysxCC(APhysXScene& scene, const float yOffset)
{
	APhysXCharacterControllerManager* pCCM = scene.GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	static int nTest = 0;
	++nTest;
	if (1000 < nTest)
		nTest -= 1000;

	bool enableBoxCC = false;
	bool turnToBoxCC = false;
	if (0 == nTest % 2)
		turnToBoxCC = true;

	APhysXCCDesc* pDesc = 0;
	APhysXCCCapsuleDesc descC;
	APhysXCCBoxDesc descB;
	if (enableBoxCC && turnToBoxCC)
	{
		descB.dimensions.set(0.4f, 0.8f, 0.4f);
		pDesc = &descB;
	}
	else
	{
		descC.height = 1.6f;
		descC.radius = 0.3f;
		pDesc = &descC;
	}

	pDesc->posFoot = APhysXConverter::A2N_Vector3(m_pObjDyn->GetPos());
	pDesc->posFoot.y += yOffset;
	gPhysXCCHitReport->Register(&m_CCHitReceiver);

	bool bHasSetCNL = false;
	APhysXCollisionChannel cnl = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
	if (m_pObjDyn->GetCollisionChannel(cnl))
	{
		if ((cnl > APX_COLLISION_CHANNEL_INVALID) && (cnl != APX_COLLISION_CHANNEL_CC_COMMON_HOST))
		{
			bHasSetCNL = true;
			m_pObjDyn->SetIndependentCollisionChannel(true);
		}
	}
	
	NxVec3 volume(0.3f, 1.6f, 0.3f);
	const float stepUp = 0.03f;
	if (APhysXCCBase::CheckOverlap(scene, pDesc->posFoot, volume, APhysXCCBase::APX_VT_CAPSULE))
		pDesc->posFoot.y += stepUp;
	
	if (bHasSetCNL)
	{
		if (APX_COLLISION_CHANNEL_COMMON == cnl)
			m_pObjDyn->SetDefaultCollisionChannel();
		else
			m_pObjDyn->SetIndependentCollisionChannel(false);
	}

	bool SettingForDebugXAJH = true;
	if (SettingForDebugXAJH)
	{
		descC.height = 1.8f;
		descC.radius = 0.5f;
		descC.speed = 3.5;
		pDesc->mass = 3;//5;
//		pDesc->flags |= APhysXCharacterController::APX_CCF_ATTACHMENT_ONLY_VERTICAL;
		pDesc->flags |= APhysXCharacterController::APX_CCF_CONTROL_JUMPINGDIR_ENABLE;
	}
	
    m_pAPhysXCCBase = m_pAPhysXCC = pCCM->CreateCC(scene, *pDesc);
	if (0 == m_pAPhysXCC)
		return false;

	A3DVECTOR3 dir, up;
	m_pObjDyn->GetDirAndUp(dir, up);
	m_pAPhysXCC->SetDir(APhysXConverter::A2N_Vector3(dir));
	m_pAPhysXCC->userData = m_pObjDyn;
	m_pAPhysXCC->SetStepOffset(0.5f);
	m_pAPhysXCC->SetAttachingPTLinearVelThreshold(0.2f);
//	m_pAPhysXCC->SetAttachingPTLinearVelThreshold(0.2f * m_pAPhysXCC->GetMass() / 1.2f);
	RegisterHostModels();

	if (!SettingForDebugXAJH)
		CreateAPhysxCCPost();
	return true;
}

void IActionPlayerBase::ReleaseAPhysxCC()
{
	if (0 == m_pAPhysXCC)
		return;

	UnRegisterHostModels(*m_pAPhysXCC);
	APhysXCharacterControllerManager& ccm = m_pAPhysXCC->GetCCManager();
	if (ccm.userData == m_pAPhysXCC)
		ccm.userData = 0;
	ccm.ReleaseCC(m_pAPhysXCC);
	if (m_pAPhysXCCBase == m_pAPhysXCC)
		m_pAPhysXCCBase = 0;
	m_pAPhysXCC = 0;
	gPhysXCCHitReport->UnRegister(&m_CCHitReceiver);
}

bool IActionPlayerBase::CreateBrushCC()
{
	float radius = 0.3f;
	float height = 1.f;
	A3DVECTOR3 pos(m_pObjDyn->GetPos());//
	pos.y += 0.8f;
	const float yOffset = 0.03f;
	BrushcharCtrlerDes brushccDes;
	brushccDes.fCapsuleRadius = radius;
	brushccDes.fCylinerHei = height;
	m_pBrushCC = new BrushcharCtrler(brushccDes);
	if (0 == m_pBrushCC)
		return false;
	if (BrushcharCtrler::IsPosAvailable(pos, radius, height))
		m_pBrushCC->SetPos(pos);
	else
	{
		pos.y += yOffset;
		m_pBrushCC->SetPos(pos);
	}
	CreateAPhysxCCPost();
	return true;

}
void IActionPlayerBase::ReleaseBrushCC()
{
	if (m_pBrushCC)
	{
		delete m_pBrushCC;
		m_pBrushCC = NULL;
	}	
}

bool IActionPlayerBase::CreateAPhysxNxCC(APhysXScene& scene, const float yOffset)
{
	APhysXCharacterControllerManager* pCCM = scene.GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);
	
	APhysXCCDesc* pDesc = 0;
	APhysXCCCapsuleDesc descC;
	APhysXCCBoxDesc descB;

	descC.height = 1.6f;
	descC.radius = 0.3f;
	pDesc = &descC;
	pDesc->posFoot = APhysXConverter::A2N_Vector3(m_pObjDyn->GetPos());
	pDesc->posFoot.y += yOffset;
	
	bool bHasSetCNL = false;
	APhysXCollisionChannel cnl = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
	if (m_pObjDyn->GetCollisionChannel(cnl))
	{
		if ((cnl > APX_COLLISION_CHANNEL_INVALID) && (cnl != APX_COLLISION_CHANNEL_CC_COMMON_HOST))
		{
			bHasSetCNL = true;
			m_pObjDyn->SetIndependentCollisionChannel(true);
		}
	}
	
	NxVec3 volume(0.3f, 1.6f, 0.3f);
	const float stepUp = 0.03f;
	if (APhysXCCBase::CheckOverlap(scene, pDesc->posFoot, volume, APhysXCCBase::APX_VT_CAPSULE))
		pDesc->posFoot.y += stepUp;
	
	if (bHasSetCNL)
	{
		if (APX_COLLISION_CHANNEL_COMMON == cnl)
			m_pObjDyn->SetDefaultCollisionChannel();
		else
			m_pObjDyn->SetIndependentCollisionChannel(false);
	}

	bool SettingForDebugXAJH = false;
	if (SettingForDebugXAJH)
	{
		descC.height = 1.8f;
		descC.radius = 0.5f;
		descC.speed = 3.5;
		pDesc->mass = 3;
	}
	
    m_pAPhysXCCBase = m_pAPhysXNxCC = pCCM->CreateNxCC(scene, *pDesc);
	if (0 == m_pAPhysXNxCC)
		return false;
	
	A3DVECTOR3 dir, up;
	m_pObjDyn->GetDirAndUp(dir, up);
	m_pAPhysXNxCC->SetDir(APhysXConverter::A2N_Vector3(dir));
	m_pAPhysXNxCC->userData = m_pObjDyn;
	m_pAPhysXNxCC->SetStepOffset(0.8f);
	RegisterHostModels();
	CreateAPhysxCCPost();
	return true;
}
void IActionPlayerBase::ReleaseAPhysxNxCC()
{
	if (0 == m_pAPhysXNxCC)
		return;
	
	UnRegisterHostModels(*m_pAPhysXNxCC);
	APhysXCharacterControllerManager& ccm = m_pAPhysXNxCC->GetCCManager();
	if (ccm.userData == m_pAPhysXNxCC)
		ccm.userData = 0;
	ccm.ReleaseNxCC(m_pAPhysXNxCC);
	if (m_pAPhysXCCBase == m_pAPhysXNxCC)
		m_pAPhysXCCBase = 0;
	m_pAPhysXNxCC = 0;
}

void IActionPlayerBase::RegisterHostModels()
{
	if (0 == m_pAPhysXCCBase)
		return;

	bool bHasNewCnl = false;
	APhysXCollisionChannel oldCNL, newCNL;
	APhysXSkeletonRBObject* pPSRB = m_pObjDyn->GetSkeletonRBObject();
	if (0 != pPSRB)
	{
		oldCNL = pPSRB->GetCollisionChannel();
		if (m_pAPhysXCCBase->RegisterHostModel(pPSRB, newCNL))
			bHasNewCnl = true;
	}

	if (bHasNewCnl)
	{
		if (oldCNL != newCNL)
		{
			assert(APX_COLLISION_CHANNEL_CC_COMMON_HOST == newCNL);
			// note here: EC model keep the old channel value because RegisterHostModel() update the channel value around ECM 
		}
	}
}

void IActionPlayerBase::UnRegisterHostModels(APhysXCCBase& baseCC)
{
	APhysXSkeletonRBObject* pPSRB = m_pObjDyn->GetSkeletonRBObject();
	if (0 != pPSRB)
	{
		APhysXCollisionChannel oldCNL, origCNL;
		oldCNL = pPSRB->GetCollisionChannel();
		if (baseCC.UnRegisterHostModel(pPSRB, origCNL))
		{
			if (oldCNL != origCNL)
			{
				assert(APX_COLLISION_CHANNEL_COMMON == origCNL);
				m_pObjDyn->SetDefaultCollisionChannel();
			}
		}
	}
}

void ACNPCDance::UpdateMove(float deltaTimeSec)
{
	if (0 == m_pAPhysXCC)
		return;

	static NxVec3 moveDir(0.0f);
	m_pAPhysXCC->SetDir(APhysXConverter::A2N_Vector3(m_pModel->GetDir()));
	m_pAPhysXCC->MoveCC(deltaTimeSec, moveDir);
}

A3DSkinModelActionCore* ACNPCDance::GetAction(const bool FirstAction)
{

// 	A3DSkinModelActionCore* pRtn = 0;
// 	if (FirstAction)
// 	{
// 		pRtn = m_pModel->GetFirstAction();
// 		return pRtn;
// 	}
// 	
// 	pRtn = m_pModel->GetNextAction();
// 	if (0 == pRtn)
// 		pRtn = m_pModel->GetFirstAction();
// 	return pRtn;

	static char* szStandActs[] = 
	{
		"C_stand_F",
			"R_pettyaction_01_F",
			"R_pettyaction_02_F",
			"R_pettyaction_03_F",
			"R_pettyaction_lovely_01_F",
			"R_pettyaction_lovely_02_F",
			"R_pettyaction_lovely_03_F",
			"R_pettyaction_lovely_04_F",
			"R_pettyaction_lovely_05_F",
			"R_pettyaction_lovely_06_F",
			"R_pettyaction_lovely_07_F",
			"R_pettyaction_lovely_08_F",
			"R_pettyaction_lovely_09_F",
			"R_pettyaction_lovely_10_F",
			"R_pettyaction_lovely_11_F",
			"R_pettyaction_lovely_12_F",
			"R_pettyaction_lovely_13_F",
			"R_pettyaction_lovely_14_F",
	};
	static const int nCount = sizeof(szStandActs) / sizeof(char*);	

	static int s_iCur = 0;



	if(s_iCur++ >= nCount) s_iCur = 0;

	return m_pModel->GetAction(szStandActs[s_iCur]);
}

void ACNPCDance::OnPlayAction(const unsigned long deltaTime, const bool FirstAction)
{
	if(m_ElapsedTime > m_ActionTime)
	{
		m_ElapsedTime = 0;
		A3DSkinModelActionCore* pAction = GetAction(FirstAction);
		if(0 == pAction)
		{
			m_ActionTime = 0;
			return;
		}

		m_ActionTime = pAction->GetActionTime() + m_SleepTime;
		m_pModel->PlayActionByNameDC(pAction->GetName());
	}
	m_ElapsedTime += deltaTime;
}

A3DTerrain2* ACWalk::gpTerrain = 0;
CCameraController* ACWalk::gpCameraCtrl = 0;
A3DVECTOR3 ACWalk::GetRandomPos(float radius, const A3DVECTOR3* pCurPos, A3DTerrain2* pTerrain, CCameraController* pCameraCtrl, bool isXAxis)
{
	static const float fmin = 0.5f;
	
	if (0 > radius)
		radius *= -1;
	
	if (fmin > radius)
		radius = 1;
	
	A3DVECTOR3 pos(0);
	if (0 != pCurPos)
		pos = *pCurPos;
	
	if (0 != pCameraCtrl)
	{
		A3DVECTOR3 camPos = pCameraCtrl->GetPos() + pCameraCtrl->GetCamera().GetDir() * radius;
		if (0 == pCurPos)
			pos = camPos;
		else
		{
			A3DVECTOR3 dist = camPos - pos;
			if (radius * radius >= dist.SquaredMagnitude())
				pos = camPos;
			else
			{
				dist.Normalize();
				pos += dist * radius;
			}
		}
	}
	
	float x = 0, z = 0;

	do
	{
		x = a_Random(-radius, radius);
		if ((0 < x) && (x < fmin)) x = fmin;
		if ((0 > x) && (x > -fmin)) x = -fmin;
		pos.x += x;
		
		z = a_Random(-radius, radius);
		if ((0 < z) && (z < fmin)) z = fmin;
		if ((0 > z) && (z > -fmin)) z = -fmin;
		pos.z += z;
	} while ((0 == x) && (0 == z));
	
	if (0 != pTerrain)
		pos.y = pTerrain->GetPosHeight(pos);
	else
	{
		float y = a_Random(-radius, radius);
		if ((0 < y) && (y < fmin)) y = fmin;
		if ((0 > y) && (y > -fmin)) y = -fmin;
		pos.y += y;
	}
	
	if (isXAxis)
		pos.z -= z;
	
	return pos;
}

ACWalk::ACWalk(CPhysXObjDynamic& objDyn) : IActionPlayerBase(objDyn)
{
	m_Flags = 0;
	m_Speed = 1.5f;
	m_ActState = STATE_LEISURE;
	
	m_TargetPos = m_CurPos = m_LastPos = m_pObjDyn->GetPos();
	m_TargetDir = m_pModel->GetDir();
	m_TargetDir.y = 0.0f;
	m_TargetDir.Normalize();
	m_CurFaceDir = m_TargetDir;
	m_fTurnT = -1.0f;
	m_fTheta = 0.0f;

	m_CurVerVel.Clear();

	m_JumpTakeoffSpeed = m_Speed;
	m_JumpTakeoffDir = m_TargetDir;

	m_HalfHeight = 0.0f;
	m_fTumbleT = 0.0f;
	m_TargetDirForHang.zero();
}

void ACWalk::UpdateDirForSmoothTurning(const A3DVECTOR3& dir)
{
	m_TargetDir = dir;
	m_TargetDir.y = 0.0f;
	m_TargetDir.Normalize();
	if (m_TargetDir.IsZero())
		return;
	
	float dotVal = DotProduct(m_TargetDir, m_CurFaceDir);
	if (1.0f > dotVal)
	{
		m_fTurnT = 0.0f;
		m_fTheta = NxMath::acos(dotVal);
	}
}

bool ACWalk::IsNearEnoughToStop(const A3DVECTOR3& posCurrent, A3DVECTOR3* pStep)
{
	A3DVECTOR3 vDist = m_TargetPos - posCurrent;
	if (0.05 > vDist.Magnitude())
	{
		EnterState(STATE_STAND);
		return true;
	}

	if (!ReadFlag(ACF_IS_INAIR) && (NxMath::abs(vDist.x) < 0.05f) && (NxMath::abs(vDist.z) < 0.05f))
	{
		EnterState(STATE_STAND);
		return true;
	}

	if (0 != pStep)
	{
		if (pStep->SquaredMagnitude() >= vDist.SquaredMagnitude())
			*pStep = vDist;
	}
	
	return false;
}

A3DVECTOR3 ACWalk::CalculatStep(float deltaTimeSec)
{
	static const A3DVECTOR3 g_G(0, -9.81f, 0);

	bool bIsJumpTakeoff = false;

	A3DVECTOR3 dHor = GetTargetDir() * deltaTimeSec * m_Speed;
	A3DVECTOR3 dVer = m_CurVerVel * deltaTimeSec;
	if (ReadFlag(ACF_IS_INAIR))
	{
		dHor = m_JumpTakeoffDir * deltaTimeSec * m_JumpTakeoffSpeed;
		dVer += 0.5f * g_G * deltaTimeSec* deltaTimeSec;
	}
	else
	{
		if (!m_CurVerVel.IsZero())
			bIsJumpTakeoff = true;
	}

	A3DVECTOR3 step = dHor + dVer;
	A3DVECTOR3 vNewPos = m_CurPos + step;
	if ((0 != gpTerrain) && (0 == GetAPhysXCCBase()) && (0 == GetBrushCC()))
	{
		float yTerrain = gpTerrain->GetPosHeight(vNewPos);
		if (ReadFlag(ACF_IS_INAIR))
		{
			if (vNewPos.y < yTerrain)
			{
				ClearFlag(ACF_IS_INAIR);
				ClearFlag(ACF_HAS_TUMBLING);
				vNewPos.y = yTerrain;
			}
		}
		else
		{
			vNewPos.y = yTerrain;			
		}
		step = vNewPos - m_CurPos;
	}

	if (bIsJumpTakeoff)
	{
		RaiseFlag(ACF_IS_INAIR);
		m_JumpTakeoffSpeed = m_Speed;
		m_JumpTakeoffDir = GetTargetDir();
	}

	if (ReadFlag(ACF_IS_INAIR))
		m_CurVerVel += g_G * deltaTimeSec; 
	else
		m_CurVerVel.Clear();
	return step;
}

void ACWalk::UpdateMove(float deltaTimeSec)
{
	static const A3DVECTOR3 g_vUp(0, 1, 0);

	m_LastPos = m_pObjDyn->GetPos();
	OnAfterPhysXSim();

	bool bIsHorMovable = false;
	if (ReadFlag(ACF_UPDATED_DIR))
	{
		bIsHorMovable = true;
		ClearFlag(ACF_UPDATED_DIR);
	}
	else
	{
		m_TargetDir.Clear();
		if (!ReadFlag(ACF_DIRTY_TARGET_POS))
		{
			A3DVECTOR3 vDir = m_TargetPos - m_CurPos;
			UpdateDirForSmoothTurning(vDir);
		}
	}
	
	A3DVECTOR3 step = CalculatStep(deltaTimeSec);
	if (!bIsHorMovable)
	{
		if (ReadFlag(ACF_IS_MOVING))
			bIsHorMovable = !IsNearEnoughToStop(m_CurPos, &step);
	}
	
	if (!bIsHorMovable)
		RaiseFlag(ACF_DIRTY_TARGET_POS);

	if (bIsHorMovable || ReadFlag(ACF_IS_INAIR))	
	{
		A3DVECTOR3 vNewPos = m_CurPos + step;
		if (0 != m_pAPhysXLWCC)
			m_pAPhysXLWCC->SetFootPosition(APhysXConverter::A2N_Vector3(vNewPos));
		m_CurPos = vNewPos;
	
		if ((0.0f <= m_fTurnT) && (!m_TargetDir.IsZero()))
		{
			m_fTurnT += deltaTimeSec * APHYSX_PI;
			
			float t = 0;
			if (!NxMath::equals(m_fTheta, 0.0f, 1e-4f))
			{
				t = m_fTurnT / m_fTheta;
				if (m_fTurnT >= m_fTheta)
				{
					t = 1.0f;
					m_fTurnT = -1.0f;
				}
			}
			
			NxVec3 vNewDir = APhysXUtilLib::Slerp(APhysXConverter::A2N_Vector3(m_CurFaceDir), APhysXConverter::A2N_Vector3(m_TargetDir), t);
			m_CurFaceDir = APhysXConverter::N2A_Vector3(vNewDir);

			A3DVECTOR3 dirMod, UpMod;
			m_pObjDyn->GetDirAndUp(dirMod, UpMod);
			UpMod.Normalize();
			if (0.9999f < DotProduct(UpMod, g_vUp))
				m_pObjDyn->SetDirAndUp(m_CurFaceDir, g_vUp);
		}

		if (ReadFlag(ACF_HAS_TUMBLING))
		{
			m_fTumbleT += deltaTimeSec * 360;
			if (360 < m_fTumbleT)
			{
				ClearFlag(ACF_HAS_TUMBLING);
				m_fTumbleT = 0.0f;
				m_pObjDyn->SetDirAndUp(m_CurFaceDir, g_vUp);
			}
			else
			{
				NxVec3 axis = APhysXConverter::A2N_Vector3(CrossProduct(g_vUp, m_CurFaceDir));
				NxQuat quat(m_fTumbleT, axis);
				A3DVECTOR3 v = APhysXConverter::N2A_Vector3(quat.rot(NxVec3(0, -m_HalfHeight, 0))); 

				A3DVECTOR3 center = vNewPos;
				center.y += m_HalfHeight;
				vNewPos = center + v;
				m_pObjDyn->RaiseFlag(IPhysXObjBase::OBF_ASSOCIATE_ALL_ISLOCKED);
				m_pObjDyn->SetPos(vNewPos);
				m_pObjDyn->ClearFlag(IPhysXObjBase::OBF_ASSOCIATE_ALL_ISLOCKED);
				if (0 != m_pAPhysXLWCC)
					m_pAPhysXLWCC->SetFootPosition(APhysXConverter::A2N_Vector3(vNewPos));

				v *= -1;
				A3DVECTOR3 newdir = CrossProduct(APhysXConverter::N2A_Vector3(axis), v);
				m_pObjDyn->SetDirAndUp(newdir, v);
			}
		}
	}

	A3DVECTOR3 vNewCCPos;

	if (0 != m_pAPhysXCC)
	{
		if (ReadFlag(ACF_IS_MOVING))
			m_pAPhysXCC->SetDir(APhysXConverter::A2N_Vector3(m_pModel->GetDir()));
		NxVec3 moveDir(GetTargetDirNx());
		
		if (gEnableTestCCPerformance)
			m_pAPhysXCC->GetAPhysXScene().GetPerfStats()->StartPerfStat("APhysX NxCC Move");
		m_pAPhysXCC->MoveCC(deltaTimeSec, moveDir);
		if (gEnableTestCCPerformance)
			m_pAPhysXCC->GetAPhysXScene().GetPerfStats()->EndPerfStat();
		vNewCCPos = APhysXConverter::N2A_Vector3(m_pAPhysXCC->GetFootPosition());
	}
	if (0 != m_pAPhysXDynCC)
	{
		m_CurPos = m_pObjDyn->GetPos();
		if (!IsHangMode())
		{
			if (ReadFlag(ACF_IS_MOVING))
				m_pAPhysXDynCC->SetDir(APhysXConverter::A2N_Vector3(m_pModel->GetDir()));
		}
		NxVec3 moveDir(GetTargetDirNx());
		if (gEnableTestCCPerformance)
			m_pAPhysXDynCC->GetAPhysXScene().GetPerfStats()->StartPerfStat("APhysX DynCC Move");
		m_pAPhysXDynCC->MoveCC(deltaTimeSec, moveDir);
		if (gEnableTestCCPerformance)
			m_pAPhysXDynCC->GetAPhysXScene().GetPerfStats()->EndPerfStat();
		vNewCCPos = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetFootPosition());
	}
	if (0 != m_pBrushCC)
	{
		m_CurPos = m_pObjDyn->GetPos();
		
		A3DVECTOR3 moveDir(APhysXConverter::N2A_Vector3(GetTargetDirNx())); 
		m_pBrushCC->SetDir(m_pModel->GetDir(), !moveDir.IsZero());
		m_pBrushCC->MoveCC(deltaTimeSec, moveDir);
		vNewCCPos = m_pBrushCC->GetFootPosition();
	}
	if (0 != m_pAPhysXNxCC)
	{
		if (ReadFlag(ACF_IS_MOVING))
			m_pAPhysXNxCC->SetDir(APhysXConverter::A2N_Vector3(m_pModel->GetDir()));
		m_CurPos = m_pObjDyn->GetPos();
		A3DVECTOR3 moveDir(APhysXConverter::N2A_Vector3(GetTargetDirNx())); 
		m_pAPhysXNxCC->MoveCC(deltaTimeSec, APhysXConverter::A2N_Vector3(moveDir));
		vNewCCPos = APhysXConverter::N2A_Vector3(m_pAPhysXNxCC->GetFootPosition());
	}	
}

void ACWalk::OnPlayAction(const unsigned long deltaTime, const bool FirstAction)
{
	if (!ReadFlag(ACF_IS_MOVING))
		Tick_StateStand(deltaTime);

	if (0 == GetAPhysXCCBase() && 0 == GetBrushCC())
	{
		m_pObjDyn->SetPos(m_CurPos);
	}
	UpdateInfoFromCC();
}

void ACWalk::UpdateInfoFromCC()
{
	if (0 != GetAPhysXCCBase())
	{
		if (0 != m_pAPhysXCC)
		{
			m_CurFaceDir = APhysXConverter::N2A_Vector3(m_pAPhysXCC->GetHorizonDir());
			m_CurPos = APhysXConverter::N2A_Vector3(m_pAPhysXCC->GetFootPosition());
		}
		if (0 != m_pAPhysXDynCC)
			m_CurFaceDir = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetHorizonDir());
	}
	if (0 != m_pBrushCC)
	{
		m_CurPos = m_pBrushCC->GetFootPosition();
		m_CurFaceDir = m_pBrushCC->GetDir();
	}
	if (0 != GetAPhysxNxCC())
	{
		m_CurPos = APhysXConverter::N2A_Vector3(m_pAPhysXNxCC->GetFootPosition());
		m_CurFaceDir = APhysXConverter::N2A_Vector3(m_pAPhysXNxCC->GetHorizonDir());
	}
}

void ACWalk::CreateAPhysxCCPost()
{
	if (0 != m_pBrushCC)
	{
		m_pBrushCC->SetSpeed(GetSpeed());
		m_pBrushCC->SetCurVel(m_CurVerVel);
	}
	if (0 != m_pAPhysXCC)
		m_pAPhysXCC->SetSpeed(GetSpeed());
	if (0 != m_pAPhysXDynCC)
		m_pAPhysXDynCC->SetSpeed(GetSpeed());
	if (0 != m_pAPhysXNxCC)
		m_pAPhysXNxCC->SetSpeed(GetSpeed());
}

void ACWalk::OnFinishCurrentAction()
{
	EnterState(STATE_STAND);
}

bool ACWalk::Tick_StateStand(const unsigned long deltaTime)
{
	bool rtn = false;
	if(m_ElapsedTime > m_ActionTime)
	{
		rtn = true;
		m_ElapsedTime = 0;
		m_ActionTime = 2000 + a_Random(100, 2000);
		if (STATE_STAND == m_ActState)
			EnterState(STATE_LEISURE);
		else
		{
			m_ActionTime *= 2;
			EnterState(STATE_STAND);
		}
	}
	m_ElapsedTime += deltaTime;
	return rtn;
}

void ACWalk::EnterState(const ActState newState)
{
	if (newState == m_ActState)
		return;
	
	static char* szStandActs[] = 
	{
		"C_stand_F",
		"R_pettyaction_01_F",
		"R_pettyaction_02_F",
		"R_pettyaction_03_F",
		"R_pettyaction_lovely_01_F",
		"R_pettyaction_lovely_02_F",
		"R_pettyaction_lovely_03_F",
		"R_pettyaction_lovely_04_F",
		"R_pettyaction_lovely_05_F",
		"R_pettyaction_lovely_06_F",
		"R_pettyaction_lovely_07_F",
		"R_pettyaction_lovely_08_F",
		"R_pettyaction_lovely_09_F",
		"R_pettyaction_lovely_10_F",
		"R_pettyaction_lovely_11_F",
		"R_pettyaction_lovely_12_F",
		"R_pettyaction_lovely_13_F",
		"R_pettyaction_lovely_14_F",
	};
	static const int nCount = sizeof(szStandActs) / sizeof(char*);
	
	switch (newState)
	{
	case STATE_STAND:
		m_pModel->PlayActionByNameDC("R_shopstand_F", -1);
		ClearFlag(ACF_IS_MOVING);
		break;
	case STATE_LEISURE:
		m_pModel->PlayActionByNameDC(szStandActs[a_Random(0, nCount - 1)], -1);
		ClearFlag(ACF_IS_MOVING);
		break;
	case STATE_WALK:
		m_pModel->PlayActionByNameDC("R_walk_F", -1);
		RaiseFlag(ACF_IS_MOVING);
		break;
	case STATE_RUN:
		m_pModel->PlayActionByNameDC("R_Run_F", -1);
		RaiseFlag(ACF_IS_MOVING);
		break;
	default:
		return;
	}

	m_ActState = newState;
	return;
}

A3DVECTOR3 ACWalk::GetDeltaMove() const
{
	return m_CurPos - m_LastPos;
}

bool ACWalk::WalkTo(const A3DVECTOR3& pos)
{
	if (IsHangMode())
		return false;

	m_TargetPos = pos;
	const A3DVECTOR3 vPosNow = m_pObjDyn->GetPos();
	
	if (IsNearEnoughToStop(vPosNow))
	{
		RaiseFlag(ACF_DIRTY_TARGET_POS);
		return false;
	}
	
	A3DVECTOR3 vDir = m_TargetPos - vPosNow;
	UpdateDirForSmoothTurning(vDir);
	RaiseFlag(ACF_UPDATED_DIR);
	ClearFlag(ACF_DIRTY_TARGET_POS);
	
	if (IsRunEnable())
		EnterState(STATE_RUN);
	else
		EnterState(STATE_WALK);
	return true;
}

void ACWalk::EnableRun(const bool bEnable)
{
	if (bEnable == IsRunEnable())
		return;

	bEnable? RaiseFlag(ACF_ENABLE_RUN) : ClearFlag(ACF_ENABLE_RUN);
	if (bEnable)
	{
		m_Speed *= 2;
		if (STATE_WALK == m_ActState)
			EnterState(STATE_RUN);
	}
	else
	{
		m_Speed *= 0.5f;
		if (STATE_RUN == m_ActState)
			EnterState(STATE_WALK);
	}
	
	if (0 != m_pAPhysXCC)
		m_pAPhysXCC->SetSpeed(m_Speed);
	if (0 != m_pAPhysXDynCC)
		m_pAPhysXDynCC->SetSpeed(m_Speed);
	if (0 != m_pBrushCC)
		m_pBrushCC->SetSpeed(m_Speed);
	if (0 != m_pAPhysXNxCC)
		m_pAPhysXNxCC->SetSpeed(m_Speed);
}

bool ACWalk::IsHangMode() const
{
	if (0 == m_pAPhysXDynCC)
		return false;

	int IsHang = APhysXDynCharCtrler::APX_CCS_HANGING & m_pAPhysXDynCC->GetStatus();
	return (0 != IsHang);
}

void ACWalk::Jump(float fJumpingSpeed, bool withTumbling)
{
	if (ReadFlag(ACF_IS_INAIR))
		return;

	if (0 >= fJumpingSpeed)
		return;

	m_CurVerVel.Set(0, fJumpingSpeed, 0);
	if (withTumbling)
	{
		RaiseFlag(ACF_HAS_TUMBLING);
		A3DVECTOR3 h = m_pObjDyn->GetHeadTopPos() - m_pObjDyn->GetPos();
		m_HalfHeight = h.Magnitude() * 0.5f;
		m_fTumbleT = 0.0f;
	}
}

ACNPCWalk::ID ACNPCWalk::GetTypeID() const
{
	if (bXAxisRange)
		return AP_WALK_XAXIS;

	if (bFixRange)
		return AP_WALK_AROUND;

	return AP_WALK;
}

void ACNPCWalk::OnPlayAction(const unsigned long deltaTime, const bool FirstAction)
{
	if (!ReadFlag(ACF_IS_MOVING))
	{
		bool StandAgain = Tick_StateStand(deltaTime);
		if (StandAgain)
			++nStandCount;

		bool StartWalk = false;
		if (2 < nStandCount)
			StartWalk = true;
		if (1 < nStandCount)
		{
			float sign = a_Random(-1, 1);
			if (0 < sign)
				StartWalk = true;
		}

		if (StartWalk)
		{
			while (0 != nStandCount)
			{
				A3DVECTOR3 pos;
				if (bXAxisRange)
					pos = GetRandomPos(3.5, &m_pObjDyn->GetNonRuntimePos(), ACWalk::gpTerrain, 0, true);
				else if (bFixRange)
					pos = GetRandomPos(5, &m_pObjDyn->GetNonRuntimePos(), ACWalk::gpTerrain, 0);
				else
					pos = GetRandomPos(5, &m_pObjDyn->GetPos(), ACWalk::gpTerrain, ACWalk::gpCameraCtrl);
				if (WalkTo(pos))
					nStandCount = 0;
			}
		}
	}
	if (0 == GetAPhysXCCBase() && 0 == GetBrushCC())
	{
		m_pObjDyn->SetPos(GetCurrentPos());
	}
	UpdateInfoFromCC();
}

void ACMAWalk::SetMoveFlag(const int flags, const A3DVECTOR3& camDir)
{
	if (IsHangMode())
	{
		NxVec3 vDir(0.0f);
		if (0 == flags)
		{
			if (0 != m_moveFlag)
			{
				m_moveFlag = 0;
				UpdateDirForHang(vDir);
			}
		}
		else
		{
			m_moveFlag = flags;
			APhysXDynCharCtrler* pDynCC = GetAPhysxDynCC();
			NxVec3 g;
			pDynCC->GetNxScene().getGravity(g);
			g.normalize();

			NxVec3 up = pDynCC->GetUpDir();
			NxVec3 ccdir = pDynCC->GetDir();
			NxVec3 axis = up.cross(ccdir);
			float d = DotProduct(camDir, APhysXConverter::N2A_Vector3(axis));
			
			if (MOVE_UP & m_moveFlag)
				vDir = g * -1;
			if (MOVE_DOWN & m_moveFlag)
				vDir = g;
			if (0 > d)
			{
				if (MOVE_KEY_LEFT & m_moveFlag)
					vDir += pDynCC->GetHorizonDir() * -1;
				if (MOVE_KEY_RIGHT & m_moveFlag)
					vDir += pDynCC->GetHorizonDir();
			}
			else if ( 0 < d)
			{
				if (MOVE_KEY_LEFT & m_moveFlag)
					vDir += pDynCC->GetHorizonDir();
				if (MOVE_KEY_RIGHT & m_moveFlag)
					vDir += pDynCC->GetHorizonDir() * -1;
			}
			UpdateDirForHang(vDir);
		}
	}
	else
	{
		if (0 == flags)
		{
			if (0 != m_moveFlag)
			{
				m_moveFlag = 0;
				RaiseFlag(ACF_DIRTY_TARGET_POS);
				if (ReadFlag(ACF_IS_MOVING))
					Stand();
			}
		}
		else
		{
			RaiseFlag(ACF_DIRTY_TARGET_POS);
			m_moveFlag = flags;
			if (!ReadFlag(ACF_IS_MOVING))
			{
				if (IsRunEnable())
					EnterState(STATE_RUN);
				else
					EnterState(STATE_WALK);
			}
			
			static const A3DVECTOR3 g_vUp(0, 1, 0);
			A3DVECTOR3 vDir(0, 0, 0);
			if (MOVE_RIGHTSTRAFE & m_moveFlag)
				vDir = CrossProduct(g_vUp, camDir);
			if (MOVE_LEFTSTRAFE & m_moveFlag)
			{
				A3DVECTOR3 vTemp = CrossProduct(g_vUp, camDir);
				vTemp *= -1;
				vDir += vTemp;
			}
			if (MOVE_FORWARD & m_moveFlag)
				vDir += camDir;
			if (MOVE_BACKWARD & m_moveFlag)
				vDir += camDir * -1;
			
			if (!vDir.IsZero())
			{
				RaiseFlag(ACF_UPDATED_DIR);
				UpdateDirForSmoothTurning(vDir);
			}
		}
	}
}
