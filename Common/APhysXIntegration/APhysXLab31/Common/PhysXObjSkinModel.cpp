/*
* FILE: PhysXObjSkinModel.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/05/19
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"

CPhysXObjSkinModel::CPhysXObjSkinModel(int UID)
{
	m_pSkinModel = 0;
	m_pSyncData  = 0;
	m_pModelPhys = 0;

	m_iCSStartID = 0;
	m_pCSConfig  = 0;
	m_propSM.InitTypeIDAndHostObject(RawObjMgr::OBJ_TYPEID_SKINMODEL, *this, UID);
	SetProperties(m_propSM);
}

bool CPhysXObjSkinModel::SyncDataPhysXToGraphic()
{
	if (!m_propSM.IsActive()) return true;

	if (0 != m_pModelPhys)
	{
		A3DVECTOR3 lastPos = m_propSM.GetPos();
		bool bRtn = m_pModelPhys->SyncAfterPhysXSim();
		m_propSM.SyncPoseDataFromPhysX(lastPos);
		return bRtn;
	}

	return true;
}

A3DModelPhysics* CPhysXObjSkinModel::GetA3DModelPhysics() const
{
	return m_pModelPhys;
}

APhysXSkeletonRBObject* CPhysXObjSkinModel::GetSkeletonRBObject() const
{
	A3DModelPhysSync* p3DMPS =  (0 == m_pModelPhys)? 0 : m_pModelPhys->GetModelSync();
	return (0 == p3DMPS)? 0 : p3DMPS->GetSkeletonRBObject();
}

bool CPhysXObjSkinModel::OnLoadModel(IEngineUtility& eu, const char* szFile)
{
	CRender* pRender = eu.GetCRender();
	if (0 == pRender)
		return false;

	A3DSkinModel* pModel = LoadSkinModel(*pRender, szFile);
	if (0 == pModel)
		return false;
	
	A3DModelPhysSyncData* pSyncData = new A3DModelPhysSyncData;
	if (0 == pSyncData)
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjSkinModel::OnLoadModel: Not enough memory!(A3DModelPhysSyncData)");
		return false;
	}

	AString strPhysXSyncFile = szFile;
	af_ChangeFileExt(strPhysXSyncFile, ".mphy");
	if(!pSyncData->Load(strPhysXSyncFile))
	{
		delete pSyncData;
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjSkinModel::OnLoadModel: Failed to initialize A3DModelPhysSyncData!");
		return false;
	}

	OnReleaseModel();

	af_ChangeFileExt(strPhysXSyncFile, ".csk");
	m_pCSConfig  = new CClothSkinConfig(strPhysXSyncFile);
	m_pSyncData  = pSyncData;
	m_pSkinModel = pModel;
	m_iCSStartID = m_pSkinModel->GetSkinNum();

	return true;
}

A3DSkinModel* CPhysXObjSkinModel::LoadSkinModel(CRender& render, const char* szFile, int iSkinFlag)
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (0 == pModel)
	{
		a_LogOutput(1, "CPhysXObjSkinModel::LoadSkinModel: Not enough memory!");
		return 0;
	}
	
	if (!pModel->Init(render.GetA3DEngine()))
	{
		ReleaseSkinModel(pModel);
		a_LogOutput(1, "CPhysXObjSkinModel::LoadSkinModel: Failed to initialize skin model!");
		return 0;
	}
	
	if (!pModel->Load(szFile, iSkinFlag))
	{
		ReleaseSkinModel(pModel);
		a_LogOutput(1, "CPhysXObjSkinModel::LoadSkinModel: Failed to load smd file! (Name: %s)", szFile);
		return 0;
	}

	render.SetLightInfo(*pModel);
	return pModel;
}

void CPhysXObjSkinModel::ReleaseSkinModel(A3DSkinModel*& pSkinModel)
{
	A3DRELEASE(pSkinModel);
	pSkinModel = 0;
}

void CPhysXObjSkinModel::OnReleaseModel()
{
	delete m_pCSConfig;
	m_pCSConfig  = 0;
	m_iCSStartID = 0;

	A3DRELEASE(m_pModelPhys);
	delete m_pSyncData;
	m_pSyncData = 0;
	A3DRELEASE(m_pSkinModel);
}

bool CPhysXObjSkinModel::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	if (!m_propSM.GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY))
		return true;

	assert((0 != m_pSkinModel) && (0 != m_pSyncData));
	A3DModelPhysics* pNewModel = new A3DModelPhysics;
	if (0 == pNewModel)
	{
		a_LogOutput(1, "CPhysXObjSkinModel::InstancePhysXObj: Not enough memory!");
		return false;
	}

	if (!pNewModel->Init(&aPhysXScene, m_pSkinModel))
	{
		a_LogOutput(1, "CPhysXObjSkinModel::InstancePhysXObj: Failed to initialize A3DModelPhysics!");
		delete pNewModel;
		return false;
	}

	if (!pNewModel->CreateModelSync(m_pSyncData))
	{
		a_LogOutput(1, "CPhysXObjSkinModel::InstancePhysXObj: Failed to CreateModelSync()!");
		delete pNewModel;
		return false;
	}

	ReleasePhysXObj();
	m_pModelPhys = pNewModel;
	m_pModelPhys->GetModelSync()->Teleport();
	m_pModelPhys->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON, 0);
	OpenSkinCloth();
	return true;
}

void CPhysXObjSkinModel::OnReleasePhysXObj()
{
	if (0 == m_pSkinModel)
	{
		assert(0 == m_pCSConfig);
		assert(0 == m_pModelPhys);
		assert(0 == m_pSyncData);
		return;
	}

	CloseSkinCloth();
	A3DRELEASE(m_pModelPhys);
}

bool CPhysXObjSkinModel::OnTickAnimation(const unsigned long dwDeltaTime)
{
	CPhysXObjDynamic::OnTickAnimation(dwDeltaTime);

	assert(0 != m_pSkinModel);
	if (0 != m_pModelPhys)
		m_pModelPhys->UpdateBeforePhysXSim(dwDeltaTime);
	else
		m_pSkinModel->Update(dwDeltaTime);
	return true; 
}

void CPhysXObjSkinModel::OnRender(A3DViewport& viewport, bool bDoVisCheck) const
{
	assert(0 != m_pSkinModel);
	if(bDoVisCheck)
		m_pSkinModel->Render(&viewport);
	else
		m_pSkinModel->Render(&viewport, A3DSkinModel::VIS_NOT_CHECK);
}

bool CPhysXObjSkinModel::OnGetPos(A3DVECTOR3& vOutPos) const
{
	if (0 == m_pSkinModel)
		return false;

	vOutPos = m_pSkinModel->GetPos();
	return true;
}

void CPhysXObjSkinModel::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(0 != m_pSkinModel);
	m_pSkinModel->SetPos(vPos);
}

bool CPhysXObjSkinModel::OnGetDir(A3DVECTOR3& vOutDir) const
{
	if (0 == m_pSkinModel)
		return false;

	vOutDir = m_pSkinModel->GetDir();
	return true;
}

bool CPhysXObjSkinModel::OnGetUp(A3DVECTOR3& vOutUp) const
{
	if (0 == m_pSkinModel)
		return false;

	vOutUp = m_pSkinModel->GetUp();
	return true;
}

void CPhysXObjSkinModel::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(0 != m_pSkinModel);
	m_pSkinModel->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjSkinModel::OnGetPose(A3DMATRIX4& matOutPose) const
{
	if (0 == m_pSkinModel)
		return false;

	matOutPose = m_pSkinModel->GetAbsoluteTM();
	return true;
}

void CPhysXObjSkinModel::OnSetPose(const A3DMATRIX4& matPose)
{
	assert(0 != m_pSkinModel);
	m_pSkinModel->SetAbsoluteTM(matPose);
}

bool CPhysXObjSkinModel::OnSetScale(const float& scale)
{
	assert(0 != m_pSkinModel);
	if(0 != m_pSkinModel)
	{
		// following codes refer to CECModel::ScaleBoneEx()
		A3DBone* pBone = m_pSkinModel->GetSkeleton()->GetBone("000", NULL);
		if (0 == pBone)
			return false;
		
		pBone->SetLocalScale(1, 1);
		pBone->SetWholeScale(scale);
		return true;
	}
	return false;
}

void CPhysXObjSkinModel::OpenSkinCloth()
{
	if (0 == m_pModelPhys)
		return;

	if (0 == m_pSkinModel)
		return;

	int nMax = 0;
	if (0 != m_pCSConfig)
		nMax = m_pCSConfig->GetClothSkinNum();
	
	int	iSkinSlotStart = m_pSkinModel->GetSkinNum();	
	for(int i = 0; i < nMax; ++i)
	{
		ClothSkinItem* pClothSkin = m_pCSConfig->GetClothSkin(i);
		if (!m_pModelPhys->AddSkinFile(pClothSkin->mSkinFilePath))
			continue;

		int iSkinSlot = m_pSkinModel->GetSkinNum() - iSkinSlotStart;
		m_pModelPhys->OpenClothSkin(iSkinSlot, 2);
		
		A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSkinSlot);
		if (0 != pSkin)
			pSkin->EnableClothes(true);
	}
}

void CPhysXObjSkinModel::CloseSkinCloth()
{
	if (0 == m_pModelPhys)
		return;

	if (0 == m_pSkinModel)
		return;

	// note here: we should remove the item from the back to front...
	// remove all cloth skin items added...
	for(int i = m_pSkinModel->GetSkinNum(); i > m_iCSStartID; --i)
		m_pModelPhys->RemoveSkinItem(i - 1);
}

void CPhysXObjSkinModel::OnControlClothes(const bool IsClose)
{
	if (IsClose)
		CloseSkinCloth();
	else
		OpenSkinCloth();
}

bool CPhysXObjSkinModel::OnResetPose()
{
	CDynProperty* pProp = GetProperties();
	assert(0 != pProp);
	pProp->FinishCurrentAction();

	assert(0 != m_pSkinModel);
	m_pSkinModel->ResetToInitPose();
	return true;
}

void CPhysXObjSkinModel::OnChangeDrivenMode(const DrivenMode& dmNew)
{
	if (0 == m_pModelPhys)
		return;

	m_pModelPhys->ChangePhysState(dmNew);
	CPhysXObjDynamic::OnChangeDrivenMode(dmNew);
}

bool CPhysXObjSkinModel::OnGetModelAABB(A3DAABB& outAABB) const
{
	if (0 == m_pSkinModel)
		return false;
	
	outAABB = m_pSkinModel->GetModelAABB();
	outAABB.CompleteMinsMaxs();
	return true;
}

bool CPhysXObjSkinModel::SetCollisionChannel(const CollisionChannel cnl)
{
	if (0 == m_pModelPhys)
		return false;

	APhysXCollisionChannel cnlCurrent;
	if (!GetCollisionChannel(cnlCurrent))
		return false;

	switch (cnl)
	{
	case CNL_COMMON:
		if (APX_COLLISION_CHANNEL_COMMON == cnlCurrent)
			break;
		return m_pModelPhys->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON, 0);
	case CNL_INDEPENDENT:
		if (APX_COLLISION_CHANNEL_INVALID > cnlCurrent)
			break;
		return m_pModelPhys->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_INDEPENDENT, 0);
	case CNL_INDEPENDENT_CCHOST:
		if (APX_COLLISION_CHANNEL_CC_COMMON_HOST == cnlCurrent)
			break;
		return m_pModelPhys->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_AS_CC_COMMON_HOST, 0);
	default:
		return false;
	}
	return true;
}

/*
bool CPhysXObjSkinModel::OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor)
{
	RaiseFlag(OBFI_ON_HIT_SKILL);
	SetDrivenMode(DRIVEN_BY_PURE_PHYSX);
	if (0 != pHitActor)
	{
		A3DVECTOR3 force = hitForceInfo.mHitDir * hitForceInfo.mHitForceMagnitude;
		pHitActor->addForceAtPos(APhysXConverter::A2N_Vector3(force), APhysXConverter::A2N_Vector3(hitForceInfo.mHitPos));
	}
	ClearFlag(OBFI_ON_HIT_SKILL);
	return true;
}
*/