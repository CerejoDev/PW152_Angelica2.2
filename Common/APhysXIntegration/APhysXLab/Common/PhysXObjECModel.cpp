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
#pragma warning(disable: 4786)

#include "stdafx.h"
#include <string>
#include "ShadowRender.h"

CPhysXObjECModel::CPhysXObjECModel(const bool isNPC) : CPhysXObjDynamic(CPhysXObjMgr::OBJ_TYPEID_ECMODEL)
{
	m_pECModel = 0;
}

bool CPhysXObjECModel::OnLoadModel(CRender& render, const char* szFile)
{
	CECModel* pModel = new CECModel;
	if (0 == pModel)
	{
		a_LogOutput(1, "CPhysXObjECModel::OnLoadModel: Not enough memory!");
		return false;
	}

	if (!pModel->Load(szFile, true, 0, true, true, false))
	{
		A3DRELEASE(pModel);
		a_LogOutput(1, "CPhysXObjECModel::OnLoadModel: Failed to initialize skin model!");
		return false;
	}
	pModel->SetUpdateLightInfoFlag(true);

/*
	// some testing code to change the ECModel's major color hue
	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();
	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));
	const A3DLIGHTPARAM& lp = render.GetDirectionalLight()->GetLightparam();
	
	LightInfo.colAmbient	= render.GetA3DDevice()->GetAmbientValue()  * A3DCOLORVALUE(1.0f, 0.0f, 0.0f, 1.0f);
	LightInfo.vLightDir		= lp.Direction;
	LightInfo.colDirDiff	= lp.Diffuse; // * A3DCOLORVALUE(0.0f, 1.0f, 0.0f, 0.5f);
	LightInfo.colDirSpec	= lp.Specular;
	LightInfo.bPtLight		= false;
	
	pSkinModel->SetLightInfo(LightInfo);
	
*/


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
					pSM->AddSkinFile(SkinPath, true);
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

bool CPhysXObjECModel::OnTickAnimation(const unsigned long dwDeltaTime)
{
	CPhysXObjDynamic::OnTickAnimation(dwDeltaTime);

	assert(0 != m_pECModel);
	if (0 != m_pECModel)
		m_pECModel->Tick(dwDeltaTime);
	return true;
}

static void GetAABBByOBB(A3DAABB& aabb, const A3DOBB& obb)
{
	A3DVECTOR3 aVertPos[8];
	obb.GetVertices(aVertPos, NULL, true);
	aabb.Build(aVertPos, 8);
}

static bool RenderForShadow(A3DViewport * pViewport, void * pArg)
{
	A3DSkinModel * pModel = (A3DSkinModel *) pArg;
	
	if( !pModel )
		return false;
	
	g_Render.GetA3DDevice()->SetZTestEnable(true);
	g_Render.GetA3DDevice()->SetZWriteEnable(true);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_Render.GetA3DDevice()->SetAlphaRef(84);
	
	pModel->RenderAtOnce(pViewport, A3DSkinModel::RAO_NOMATERIAL, false);
	
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	g_Render.GetA3DDevice()->SetZTestEnable(true);
	g_Render.GetA3DDevice()->SetZWriteEnable(true);
	return true;
}

void CPhysXObjECModel::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	assert(0 != m_pECModel);
	CPhysXObjDynamic::OnRender(viewport, pWC, isDebug);
	m_pECModel->Render(&viewport);
}

void CPhysXObjECModel::OnRenderShadow(A3DViewport& viewport) const
{
	// render the shadow of EC Model...
	if (g_Game.GetShadowRender())
	{
		A3DAABB aabb;
		aabb = m_pECModel->GetModelAABB();
		aabb.Extents += A3DVECTOR3(3.0f, 3.0f, 3.0f);
		
		g_Game.GetShadowRender()->AddShadower(aabb.Center, aabb, SHADOW_RECEIVER_TERRAIN, 
			RenderForShadow, m_pECModel->GetA3DSkinModel());
	}
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

void CPhysXObjECModel::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(0 != m_pECModel);
	m_pECModel->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjECModel::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert((0 != m_pECModel));
	if (0 == m_pECModel)
		return false;
	
	if (ReadFlag(OBF_ENABLE_PHYSX_BODY))
	{
		SetPhysSystemState(true);
		m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON);
	}
	return true;
}

void CPhysXObjECModel::OnReleasePhysXObj(const bool RunTimeIsEnd)
{
	if (0 == m_pECModel)
		return;

	SetPhysSystemState(false);
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

bool CPhysXObjECModel::SyncDataPhysXToGraphic()
{
	if (!IsActive()) return true;
	
	if (0 != m_pECModel)
		return m_pECModel->SyncModelPhys();
		
	return true;
}

bool CPhysXObjECModel::OnResetPose()
{
	FinishCurrentAction();
	assert(0 != m_pECModel);
	m_pECModel->GetA3DSkinModel()->ResetToInitPose();
	return true;
}

void CPhysXObjECModel::OnChangeDrivenMode(const DrivenMode dmNew)
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

A3DSkinModel* CPhysXObjECModel::GetSkinModel() const
{
	return (0 == m_pECModel)? 0 : m_pECModel->GetA3DSkinModel(); 
}

bool CPhysXObjECModel::SetDefaultCollisionChannel()
{
	if (0 == m_pECModel)
		return false;
	
	return m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON);
}

bool CPhysXObjECModel::SetIndependentCollisionChannel(const bool CCCommonHost)
{
	if (0 == m_pECModel)
		return false;

	APhysXCollisionChannel ccCurrent;
	if (!GetCollisionChannel(ccCurrent))
		return false;

	if (CCCommonHost)
	{
		if (APX_COLLISION_CHANNEL_CC_COMMON_HOST == ccCurrent)
			return true;
		m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_CC_COMMON_HOST);
	}
	else
	{
		if (APX_COLLISION_CHANNEL_INVALID > ccCurrent)
			return true;
		m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_INDEPENDENT);
	}
	return true;
}

bool CPhysXObjECModel::GetCollisionChannel(APhysXCollisionChannel& outCC) const
{
	if (0 == m_pECModel)
		return false;

	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if (0 == pSRB)
		return false;

	outCC = pSRB->GetCollisionChannel();
	return true;
}

bool CPhysXObjECModel::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, const float fForceMagnitude, const float fMaxDist)
{
	if (0 == m_pECModel)
		return false;
	
	return m_pECModel->AddForce(vStart, vDir, fForceMagnitude, fMaxDist);
}

APhysXSkeletonRBObject* CPhysXObjECModel::GetSkeletonRBObject() const
{
	A3DModelPhysSync* p3DMPS = (0 == m_pECModel)? 0 : m_pECModel->GetPhysSync();
	return (0 == p3DMPS)? 0 : p3DMPS->GetSkeletonRBObject();
}

A3DModelPhysics* CPhysXObjECModel::GetA3DModelPhysics() const
{
	return (0 == m_pECModel)? 0 : m_pECModel->GetModelPhysics();
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