/*
 * FILE: GameProperty.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/10/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

const CGameProperty::GameModeDesc CGameProperty::gGMDesc[] =
{
	GAME_MODE_EMPTY,	 _T("¿ÕÏÐ"), _T("µ±Ç°×´Ì¬: ¿ÕÏÐ"),
	GAME_MODE_EDIT,		 _T("±à¼­"), _T("µ±Ç°×´Ì¬: ±à¼­"),
	GAME_MODE_SIMULATE,	 _T("Ä£Äâ"), _T("µ±Ç°×´Ì¬: Ä£Äâ"),
	GAME_MODE_PLAY,		 _T("ÓÎÏ·"), _T("µ±Ç°×´Ì¬: ÓÎÏ·"),
};

CGameProperty::CGameProperty(CGame& game, CPhysXObjMgr& objMgr)
{
	m_pGame	  = &game;
	m_pObjMgr = &objMgr;

	m_pPhysXEngine = 0;
	m_pPhysXScene  = 0;
	m_pGMD = &gGMDesc[GAME_MODE_EMPTY];
	m_pObjPicked = 0;
	m_ptLastMousePos.x = m_ptLastMousePos.y = 0;
	m_SelGroup.SetRayHitOwner(m_RHOwner);
	m_cmdQueue.SetSelGroup(m_SelGroup);
	m_strTrnFile.Empty();

	m_iSMode = SM_EYE_TO_CENTER;
	m_iSSID	 = SSI_SPHERE;	

	IPropObjBase::SetStateReport(&m_OSRMgr);
	m_SelGroup.SetStateReport(&m_OSRMgr);
	m_OSRMgr.RegisterReceiver(m_SelGroup);
	m_OSRMgr.RegisterReceiver(m_RHOwner);
}

bool CGameProperty::SaveToFile(NxStream& nxStream, A3DCamera& camera)
{
	SaveVersion(&nxStream);
	APhysXSerializerBase::StoreString(&nxStream, m_strTrnFile);

	nxStream.storeBuffer(&camera.GetPos(), sizeof(A3DVECTOR3));
	nxStream.storeBuffer(&camera.GetDir(), sizeof(A3DVECTOR3));
	nxStream.storeBuffer(&camera.GetUp(), sizeof(A3DVECTOR3));
	m_pObjMgr->SaveSerialize(nxStream);
	return true;
}

bool CGameProperty::LoadFromFile(NxStream& nxStream, A3DCamera& camera, bool& outIsLowVersion)
{
	LoadVersion(&nxStream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVersion = true;

	if(m_StreamObjVer < 0xAA000002)
		RawObjMgr::GetInstance()->SetArchiveStyle(true);
	else
		RawObjMgr::GetInstance()->SetArchiveStyle(false);

	if(m_StreamObjVer >= 0xAA000001)
	{
		const char* pTrnFile = APhysXSerializerBase::ReadString(&nxStream);
		m_pGame->LoadTerrainAndPhysXTerrain(pTrnFile);

		A3DVECTOR3 vPos, vDir, vUp;	
		nxStream.readBuffer(&vPos, sizeof(A3DVECTOR3));
		nxStream.readBuffer(&vDir, sizeof(A3DVECTOR3));
		nxStream.readBuffer(&vUp, sizeof(A3DVECTOR3));
		camera.SetPos(vPos);
		camera.SetDirAndUp(vDir, vUp);
	}

	bool bIsLowVer = false;
	if (!m_pObjMgr->LoadDeserialize(nxStream, bIsLowVer))
		return false;

	if (bIsLowVer)
		outIsLowVersion = true;
	return true;
}

const char* CGameProperty::GetActiveSceneFile() const
{
	if (m_strActiveScene.IsEmpty())
		return 0;

	return m_strActiveScene; 
}

bool CGameProperty::IsRuntime() const
{
	if (QueryGameMode(GAME_MODE_SIMULATE))
		return true;
	if (QueryGameMode(GAME_MODE_PLAY))
		return true;
	return false;
}

void CGameProperty::SetGameMode(const GameMode& gm)
{
	assert(0 != m_pGame);
	if (QueryGameMode(gm))
		return;

	switch(gm)
	{
	case GAME_MODE_EMPTY:
		SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);
		m_pGame->OnEntryEmptyMode();
		m_cmdQueue.ClearAllCmds();
		m_status.ClearFlag(GAME_IS_PHYSX_SIM_PAUSE);
		m_status.ClearFlag(GAME_IS_PHYSX_SIM_STEP);
		break;
	case GAME_MODE_EDIT:
		SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);
		m_pGame->OnEntryEditMode();
		m_status.ClearFlag(GAME_IS_PHYSX_SIM_PAUSE);
		m_status.ClearFlag(GAME_IS_PHYSX_SIM_STEP);
		break;
//	case GAME_MODE_SIMULATE:
//		m_pGame->OnEntrySimMode();   Deprecated sim mode, replaced by play mode  
//		break;
	case GAME_MODE_PLAY:
		m_pGame->OnEntryPlayMode();
		break;
	default:
		assert(!"Unknown Game Mode!");
	}
	m_pGMD = &gGMDesc[gm];
	if (GAME_MODE_EMPTY == gm)
	{
		SetTerrainFileName(0);
		SetActiveSceneFile(0);
	}
}

bool CGameProperty::QueryGameMode(const GameMode& gm) const
{
	return (gm == m_pGMD->iState);
}

const TCHAR* CGameProperty::GetGameModeDesc(bool bIsFullDesc) const
{
	if (bIsFullDesc)
		return m_pGMD->pFullDesc;

	return m_pGMD->pDesc;
}

void CGameProperty::SetAPhysXInfo(APhysXEngine& engine, APhysXScene& scene)
{
	m_pPhysXEngine = &engine;
	m_pPhysXScene  = &scene;

	m_pPhysXEngine->EnableHardwarePhysX(IsPhysHWEnabled());
	m_pPhysXScene->EnableTerrainDebugRender(IsShowTerrianGridEnabled());
	m_pPhysXScene->EnablePerfStatsRender(IsPhysXProfileEnabled());
	m_pPhysXScene->EnableDebugRender(IsPhysXDebugRenderEnabled());
}

void CGameProperty::SetTerrainFileName(const char* pName)
{
	assert(false == QueryGameMode(GAME_MODE_SIMULATE));
	assert(false == QueryGameMode(GAME_MODE_PLAY));
	if (0 == pName)
	{
		m_strTrnFile.Empty();
	}
	else
	{
		m_strTrnFile = pName;
		SetGameMode(GAME_MODE_EDIT);
	}
}

void CGameProperty::SetActiveSceneFile(const char* pPath)
{
	assert(false == QueryGameMode(GAME_MODE_SIMULATE));
	assert(false == QueryGameMode(GAME_MODE_PLAY));
	if (0 == pPath)
		m_strActiveScene.Empty();
	else
		m_strActiveScene = pPath;
}

void CGameProperty::SetPickedObject(IObjBase* pObject, bool changeSelGroup)
{
	m_pObjPicked = pObject;
	if (changeSelGroup)
	{
		if (0 != pObject)
		{
			if (!m_status.ReadFlag(CGameProperty::GAME_CREATE_AND_PICK_STH))
				m_cmdQueue.SubmitBeforeExecution(CmdSelReplace());
			m_SelGroup.ReplaceWithObject(pObject);
		}
	}
}

void CGameProperty::SetRayHitObject(IObjBase* pNewHit, const CPoint& pt)
{
	m_RHOwner.SetRayHit(pNewHit, pt);
}

void CGameProperty::EnableShowTerrianGrid(const bool bShow)
{
	if (bShow)
		m_status.RaiseFlag(GAME_DRAW_TERRAIN_GRID);
	else
		m_status.ClearFlag(GAME_DRAW_TERRAIN_GRID);

	if (0 != m_pPhysXScene)
		m_pPhysXScene->EnableTerrainDebugRender(bShow);	
}

void CGameProperty::EnablePhysXProfile(const bool bEnable)
{
	if (bEnable)
		m_status.RaiseFlag(GAME_DRAW_PHYSX_PRIFILE);
	else
		m_status.ClearFlag(GAME_DRAW_PHYSX_PRIFILE);

	if (0 != m_pPhysXScene)
		m_pPhysXScene->EnablePerfStatsRender(bEnable);
}

void CGameProperty::EnablePhysXDebugRender(const bool bEnable)
{
	if (bEnable)
		m_status.RaiseFlag(GAME_DRAW_PHYSX_DBGRENDER);
	else
		m_status.ClearFlag(GAME_DRAW_PHYSX_DBGRENDER);

	if(0 != m_pPhysXScene)
		m_pPhysXScene->EnableDebugRender(bEnable);
}

void CGameProperty::EnablePhysXHW(const bool bEnable)
{
	if (bEnable)
		m_status.RaiseFlag(GAME_ENABLE_PHYSX_HW);
	else
		m_status.ClearFlag(GAME_ENABLE_PHYSX_HW);

	if(0 != m_pPhysXEngine)
		m_pPhysXEngine->EnableHardwarePhysX(bEnable);
}

bool CGameProperty::PhysXSimASingleStep()
{
	if (IsPhysXSimPauseState())
	{
		m_status.RaiseFlag(GAME_IS_PHYSX_SIM_STEP);
		return true;
	}

	return false;
}

void CGameProperty::ChangePhysXPauseState() 
{
	if (QueryGameMode(GAME_MODE_PLAY) || QueryGameMode(GAME_MODE_SIMULATE))
	{
		if (m_status.ReadFlag(GAME_IS_PHYSX_SIM_PAUSE))
			m_status.ClearFlag(GAME_IS_PHYSX_SIM_PAUSE);
		else
			m_status.RaiseFlag(GAME_IS_PHYSX_SIM_PAUSE);
	}
}

bool CGameProperty::IsPhysXSimEnable()
{
	if (!m_status.ReadFlag(GAME_IS_PHYSX_SIM_PAUSE))
		return true;

	if (m_status.ReadFlag(GAME_IS_PHYSX_SIM_STEP))
	{
		m_status.ClearFlag(GAME_IS_PHYSX_SIM_STEP);
		return true;
	}

	return false;
}

void CGameProperty::SetDrivenMode(const DrivenMode& dm)
{
	assert(0 != m_pObjMgr);
	m_pObjMgr->GetProperties()->SetDrivenMode(dm);
}

CGameProperty::DrivenMode CGameProperty::GetDrivenMode() const
{
	return m_pObjMgr->GetProperties()->GetDrivenMode();
}

bool CGameProperty::QueryDrivenMode(const DrivenMode& dm) const
{
	return m_pObjMgr->GetProperties()->QueryDrivenMode(dm);
}

void CGameProperty::SetRefFrameEditMode(const EditType& et)
{
	assert(0 != m_pObjMgr);
	assert(false == QueryGameMode(GAME_MODE_SIMULATE));
	assert(false == QueryGameMode(GAME_MODE_PLAY));
	m_SelGroup.SetRefFrameEditMode(et);
}

CGameProperty::EditType CGameProperty::GetRefFrameEditMode() const
{
	assert(0 != m_pObjMgr);
	return m_SelGroup.GetRefFrameEditMode();
}

void CGameProperty::ChangeShootStuffType(int shootID)
{
	if ((0 <= shootID) && (shootID < SSI_END_BOUND))
	{
		m_iSSID = ShootStuffID(shootID);
		return;
	}

	int temp = m_iSSID;
	++temp;
	if (SSI_END_BOUND == temp)
		temp = 0;
	m_iSSID = ShootStuffID(temp);
}

void CGameProperty::ChangeShootMode(int shootMode)
{
	if ((0 <= shootMode) && (shootMode < SM_END_FLAG))
	{
		m_iSMode = ShootMode(shootMode);
		return;
	}

	int temp = m_iSMode;
	++temp;
	if (SM_END_FLAG == temp)
		temp = 0;
	m_iSMode = ShootMode(temp);
}

bool CGameProperty::GetHitObjectFileName(AString& outstrFilePath, CPoint* poutPT)
{
	outstrFilePath.Empty();
	IObjBase* pObj = GetRayHitObject(poutPT);
	if (0 == pObj)
		return false;

	IPropObjBase* pProp = pObj->GetProperties();
	if (!RawObjMgr::IsPhysXObjType(pProp->GetObjType()))
		return false;

	IPhysXObjBase* pPhysXObj = static_cast<IPhysXObjBase*>(pObj);
	outstrFilePath = pPhysXObj->GetProperties()->GetFilePathName();
	return true;
}

void CGameProperty::SetLastMousePos(const CPoint& pt)
{
	m_ptLastMousePos.x = pt.x; 
	m_ptLastMousePos.y = pt.y;
}