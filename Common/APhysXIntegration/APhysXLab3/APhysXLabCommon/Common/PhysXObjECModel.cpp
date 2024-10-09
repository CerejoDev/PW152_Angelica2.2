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
	m_propECM.InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_ECMODEL, *this, UID);
	SetProperties(m_propECM);
	SetRenderable(m_ECMRender);
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

#ifdef _ANGELICA3
	if (!pModel->Init(eu.GetGFXEngine(), eu.GetGFXPhysXSceneShell()))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjECModel::OnLoadModel: Failed to initialize EC model!");
		return false;
	}
#endif

	bool bIsRelative = true;
	AString strBaseBK = af_GetBaseDir();
	AString strFile = szFile;
	strFile.MakeLower();
	int nL = strFile.Find("models");
	if (0 != nL)
	{
		bIsRelative = false;
		AString strBaseNew = strFile.Left(nL - 1);
		af_SetBaseDir(strBaseNew);
	}

	if (!pModel->Load(szFile, true, 0, true, true, false))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "Warning in %s: Failed to load EC model %s!", __FUNCTION__, szFile);
		if (!bIsRelative)
			af_SetBaseDir(strBaseBK);
		return false;
	}
	if (!bIsRelative)
		af_SetBaseDir(strBaseBK);
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
				{
#ifdef _ANGELICA3
					pSM->AddSkinFile(SkinPath);
#else
					pSM->AddSkinFile(SkinPath, true);
#endif
				}
			}
			cfgINI.Close();
		}
	}

	m_ECMRender.SetModelPtr(m_pECModel);
	return true;
}

void CPhysXObjECModel::OnReleaseModel()
{
	CPhysXObjECModel* pHost = dynamic_cast<CPhysXObjECModel*>(GetParentModel());
	if (0 != pHost)
	{
		CECModel* pHostECM = pHost->GetECModel();
		pHostECM->CloseChildModelLink(GetParentHangerName());
		pHostECM->RemoveChildModel(GetParentHangerName(), false);
	}

	if (0 != m_pECModel)
	{
		for (int i = 0; i < m_pECModel->GetChildCount(); )
		{
			CECModel* pChild = m_pECModel->GetChildModel(i);
			if (0 != pChild)
			{
				CPhysXObjECModel* pObj = ObjManager::GetInstance()->GetECModelObject(pChild);
				if (0 != pObj)
				{
					m_pECModel->CloseChildModelLink(pObj->GetParentHangerName());
					m_pECModel->RemoveChildModel(pObj->GetParentHangerName(), false);
				}
				else
					i++;
			}
		}
	}
	A3DRELEASE(m_pECModel);
}

bool CPhysXObjECModel::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert((0 != m_pECModel));
	if (0 == m_pECModel)
		return false;

	if (m_propECM.GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY))
	{
		if (SetPhysSystemState(true))
		{
			// to check PhysX is real instanced well, due to "SetPhysSystemState" always return true
			if (0 == GetSkeletonRBObject())
			{
				SetPhysSystemState(false);
				return false;
			}
			m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON);
			APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
			if (0 != pSRB)
			{
				APhysXBreakable* pB = pSRB->IsBreakable();
				if (0 != pB)
					pB->mBreakUserData = this;
			}
			return true;
		}
	}
	else
	{
		// make sure release PhysX body, because the ECM probably had been instanced PhysX body by the GFXPhysXSceneShell callback!
		if (SetPhysSystemState(false))
			return true;
	}
	return false;
}

void CPhysXObjECModel::OnReleasePhysXObj()
{
	if (0 == m_pECModel)
		return;

	SetPhysSystemState(false);
	assert(0 == GetSkeletonRBObject());
}

bool CPhysXObjECModel::OnTickAnimation(const unsigned long dwDeltaTime)
{
	CPhysXObjDynamic::OnTickAnimation(dwDeltaTime);

	assert(0 != m_pECModel);
	if (0 != m_pECModel)
	{
/*		A3DModelPhysSync* p3DMPS = m_pECModel->GetPhysSync();
		if (0 != p3DMPS)
		{
			if (m_pScene->QueryRPTStateEnable(GetGPos(), false))
				p3DMPS->m_bEnableUpPoseByAnim = true;
			else
				p3DMPS->m_bEnableUpPoseByAnim = false;
		}
*/
		m_pECModel->Tick(dwDeltaTime);
	}
	return true;
}

bool CPhysXObjECModel::SyncDataPhysXToGraphicMyself()
{
	if (m_propECM.IsActive())
	{
		if (0 != m_pECModel)
		{
#ifdef _ANGELICA21
			bool bRtn = m_pECModel->SyncModelPhys(DWORD(m_dt));
#else
			bool bRtn = m_pECModel->SyncModelPhys();
#endif
			m_propECM.SyncPoseDataFromPhysX();
			return bRtn;
		}
	}
	return true;
}

CPhysXObjECModel* CPhysXObjECModel::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_ECMODEL, true, * m_pScene);
	CPhysXObjECModel* pObj = dynamic_cast<CPhysXObjECModel*>(pObjBase);
	if (0 != pObj)
		pObj->m_propECM = m_propECM;
	return pObj;
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

bool CPhysXObjECModel::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	if (0 == m_pECModel)
		return false;

	matOutPose = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
	A3DVECTOR3 vRow = matOutPose.GetRow(0);
	if (fabs(vRow.Magnitude() - 1) < 1E-3f)
		outWithScaled = false;
	else
		outWithScaled = true;
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
		A3DSkinModel* pSM = m_pECModel->GetA3DSkinModel();
		A3DBone* pBone = pSM->GetSkeleton()->GetBone("000", NULL);
		if (0 == pBone)
			pBone = pSM->GetSkeleton()->GetBone("Dummy01", NULL);
		if (0 != pBone)
		{
			float fScale = pBone->GetWholeScale();
			if (NxMath::equals(scale, fScale, APHYSX_FLOAT_EPSILON))
				return true;
		}
		if(!m_pECModel->ScaleBoneEx("000", A3DVECTOR3(1.0f, 1.0f, scale)))
		{
			if (m_pECModel->ScaleBoneEx("Dummy01", A3DVECTOR3(1.0f, 1.0f, scale)))
			{
				m_pECModel->GetA3DSkinModel()->GetSkeleton()->CalcFootOffset();
				return true;
			}
			return false;
		}
		else
		{
			m_pECModel->GetA3DSkinModel()->GetSkeleton()->CalcFootOffset();
			return true;
		}
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

bool CPhysXObjECModel::AddForce(const NxVec3& pt, const NxVec3& dirANDmag, int iPhysForceType, float fSweepRadius)
{
	assert(0 != m_pECModel);
	float mag = dirANDmag.magnitude();
	NxVec3 dir = dirANDmag;
	dir.normalize();
	NxVec3 start = pt - dir * 0.1f;
#ifdef _ANGELICA21
	return m_pECModel->AddForce(APhysXConverter::N2A_Vector3(start), A3DVECTOR3(dir.x, dir.y, dir.z), mag, 3, iPhysForceType, fSweepRadius);
#else
	return m_pECModel->AddForce(APhysXConverter::N2A_Vector3(start), A3DVECTOR3(dir.x, dir.y, dir.z), mag, 3, iPhysForceType);
#endif
}
