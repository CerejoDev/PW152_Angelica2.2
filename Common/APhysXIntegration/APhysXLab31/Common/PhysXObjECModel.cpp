/*
* FILE: PhysXObjECModel.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/07/08
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/
#include "stdafx.h"

CPhysXObjECModel::CPhysXObjECModel(int UID)
{
	m_pECModel = 0;
	m_propECM.InitTypeIDAndHostObject(RawObjMgr::OBJ_TYPEID_ECMODEL, *this, UID);
	SetProperties(m_propECM);
}

bool CPhysXObjECModel::SyncDataPhysXToGraphic()
{
	if (!m_propECM.IsActive()) return true;

	if (0 != m_pECModel)
	{
		A3DVECTOR3 lastPos = m_propECM.GetPos();
		bool bRtn = m_pECModel->SyncModelPhys();
		m_propECM.SyncPoseDataFromPhysX(lastPos);
		return bRtn;
	}

	return true;
}

A3DSkinModel* CPhysXObjECModel::GetSkinModel() const
{
	return (0 == m_pECModel)? 0 : m_pECModel->GetA3DSkinModel(); 
}

A3DModelPhysics* CPhysXObjECModel::GetA3DModelPhysics() const
{
	return (0 == m_pECModel)? 0 : m_pECModel->GetModelPhysics();
}

APhysXSkeletonRBObject* CPhysXObjECModel::GetSkeletonRBObject() const
{
	A3DModelPhysSync* p3DMPS = (0 == m_pECModel)? 0 : m_pECModel->GetPhysSync();
	return (0 == p3DMPS)? 0 : p3DMPS->GetSkeletonRBObject();
}

bool CPhysXObjECModel::OnLoadModel(IEngineUtility& eu, const char* szFile)
{
	CECModel* pModel = new CECModel;
	if (0 == pModel)
	{
		a_LogOutput(1, "CPhysXObjECModel::OnLoadModel: Not enough memory!");
		return false;
	}

	if (!pModel->Init(eu.GetGFXEngine(), eu.GetGFXPhysXSceneShell()))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjECModel::OnLoadModel: Failed to initialize EC model!");
		return false;
	}

	if (!pModel->Load(szFile, true, 0, true, true, false))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjECModel::OnLoadModel: Failed to load EC model!");
		return false;
	}
	pModel->SetUpdateLightInfoFlag(true);

	OnReleaseModel();
	m_pECModel = pModel;
	
	A3DSkinModel* pSM = GetSkinModel();
	if (0 != pSM)
	{
		AString SkinFile(szFile);
		const int pos = SkinFile.ReverseFind('.');
		if (0 < pos)
			SkinFile.CutRight(SkinFile.GetLength() - pos);
		SkinFile +=  "_Skin.ini";
		af_GetFullPath(SkinFile, SkinFile);

		AIniFile cfgINI;
		if (cfgINI.Open(SkinFile))
		{
			AString str, SkinPath;
			const int nCount = cfgINI.GetValueAsInt("Skins", "Count", 0);
			for (int i = 0; i < nCount; ++i)
			{
				str.Format("Item%d", i);
				SkinPath = cfgINI.GetValueAsString("Skins", str);
				if (!SkinPath.IsEmpty())
					pSM->AddSkinFile(SkinPath);
			}
			cfgINI.Close();
		}
	}
	
	return true;
}

void CPhysXObjECModel::OnReleaseModel()
{
	A3DRELEASE(m_pECModel);
}

bool CPhysXObjECModel::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert((0 != m_pECModel));
	if (0 == m_pECModel)
		return false;

	if (m_propECM.GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY))
	{
		SetPhysSystemState(true);
		m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON);
	}
	else
	{
		// make sure release PhysX body, because the ECM probably had been instanced PhysX body by the GFXPhysXSceneShell callback!
		SetPhysSystemState(false);
	}
	return true;
}

void CPhysXObjECModel::OnReleasePhysXObj()
{
	if (0 == m_pECModel)
		return;

	SetPhysSystemState(false);
}

bool CPhysXObjECModel::OnTickAnimation(const unsigned long dwDeltaTime)
{
	CPhysXObjDynamic::OnTickAnimation(dwDeltaTime);

	assert(0 != m_pECModel);
	if (0 != m_pECModel)
		m_pECModel->Tick(dwDeltaTime);
	return true;
}

void CPhysXObjECModel::OnRender(A3DViewport& viewport, bool bDoVisCheck) const
{
	//??? just ignore the bDoVisCheck parameter...
	assert(0 != m_pECModel);
	m_pECModel->Render(&viewport);
}

bool CPhysXObjECModel::OnGetPos(A3DVECTOR3& vOutPos) const
{
	if (0 == m_pECModel)
		return false;
	
	// don't use the value of m_pECModel->GetPos()
	// here is the correct way
	vOutPos = m_pECModel->GetA3DSkinModel()->GetPos();
	return true;
}

void CPhysXObjECModel::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(0 != m_pECModel);
	m_pECModel->SetPos(vPos);
}

bool CPhysXObjECModel::OnGetDir(A3DVECTOR3& vOutDir) const
{
	if (0 == m_pECModel)
		return false;

	vOutDir = m_pECModel->GetA3DSkinModel()->GetDir();
	return true;
}

bool CPhysXObjECModel::OnGetUp(A3DVECTOR3& vOutUp) const
{
	if (0 == m_pECModel)
		return false;

	vOutUp = m_pECModel->GetA3DSkinModel()->GetUp();
	return true;
}

void CPhysXObjECModel::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(0 != m_pECModel);
	m_pECModel->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjECModel::OnGetPose(A3DMATRIX4& matOutPose) const
{
	if (0 == m_pECModel)
		return false;

	matOutPose = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
	return true;
}

void CPhysXObjECModel::OnSetPose(const A3DMATRIX4& matPose)
{
	assert(0 != m_pECModel);
	m_pECModel->SetAbsoluteTM(matPose);
}

bool CPhysXObjECModel::OnSetScale(const float& scale)
{
	assert(0 != m_pECModel);
	if(0 != m_pECModel)
	{
		if(!m_pECModel->ScaleBoneEx("000", A3DVECTOR3(1.0f, 1.0f, scale)))
			return m_pECModel->ScaleBoneEx("Dummy01", A3DVECTOR3(1.0f, 1.0f, scale));
		else
			return true;
	}

	return false;
}

void CPhysXObjECModel::OnControlClothes(const bool IsClose)
{
	if (0 == m_pECModel)
		return;

	if (IsClose)
		m_pECModel->CloseAllClothes();
	else
		m_pECModel->OpenAllClothes(1);
}

bool CPhysXObjECModel::OnResetPose()
{
	CDynProperty* pProp = GetProperties();
	assert(0 != pProp);
	pProp->FinishCurrentAction();

	assert(0 != m_pECModel);
	m_pECModel->GetA3DSkinModel()->ResetToInitPose();
	return true;
}

void CPhysXObjECModel::OnChangeDrivenMode(const DrivenMode& dmNew)
{
	if (0 == m_pECModel)
		return;

	A3DModelPhysics* p3DMP = m_pECModel->GetModelPhysics();
	if (0 != p3DMP)
		p3DMP->ChangePhysState(dmNew);

	int nCount = m_pECModel->GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		const char* pStr = m_pECModel->GetHangerName(i);
		m_pECModel->ChangeChildModelPhysState(pStr, dmNew);
	}

	CPhysXObjDynamic::OnChangeDrivenMode(dmNew);
}

bool CPhysXObjECModel::OnGetModelAABB(A3DAABB& outAABB) const
{
	if (0 == m_pECModel)
		return false;
	
	outAABB = m_pECModel->GetModelAABB();
	outAABB.CompleteMinsMaxs();
	return true;
}

bool CPhysXObjECModel::SetCollisionChannel(const CollisionChannel cnl)
{
	if (0 == m_pECModel)
		return false;

	APhysXCollisionChannel cnlCurrent;
	if (!GetCollisionChannel(cnlCurrent))
		return false;

	switch (cnl)
	{
	case CNL_COMMON:
		if (APX_COLLISION_CHANNEL_COMMON == cnlCurrent)
			break;
		return m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON);
	case CNL_INDEPENDENT:
		if (APX_COLLISION_CHANNEL_INVALID > cnlCurrent)
			break;
		return m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_INDEPENDENT);
	case CNL_INDEPENDENT_CCHOST:
		if (APX_COLLISION_CHANNEL_CC_COMMON_HOST == cnlCurrent)
			break;
		return m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_CC_COMMON_HOST);
	default:
		return false;
	}
	return true;
}

/*
bool CPhysXObjECModel::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, const float fForceMagnitude, const float fMaxDist)
{
	if (0 == m_pECModel)
		return false;
	
	return m_pECModel->AddForce(vStart, vDir, fForceMagnitude, fMaxDist);
}

bool CPhysXObjECModel::OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor) 
{
	RaiseFlag(OBFI_ON_HIT_SKILL);
	SetDrivenMode(DRIVEN_BY_PURE_PHYSX);
	AddForce(hitForceInfo.mHitPos - hitForceInfo.mHitDir, hitForceInfo.mHitDir, hitForceInfo.mHitForceMagnitude, 2.0f);
	ClearFlag(OBFI_ON_HIT_SKILL);
	return true;
}

bool CPhysXObjECModel::OnPlayAttackAction(const IPhysXObjBase& objTarget)
{
	assert(0 != m_pECModel);
	if(0 != m_pECModel)
	{
		m_pECModel->PlayAttackAction("PhysXHit", 0, reinterpret_cast<int>(this), reinterpret_cast<int>(&objTarget), 0);
		return true;
	}

	return false;
}
*/
bool CPhysXObjECModel::SetPhysSystemState(const bool bEnable)
{
	assert(0 != m_pECModel);
	if (0 == m_pECModel)
		return false;

	if (bEnable)
		return m_pECModel->EnablePhysSystem(false);

	m_pECModel->DisablePhysSystem();
	return true;
}