/*
 * FILE: PhysXObjParticle.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/03/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include <A3DGFXExMan.h>
#include <A3DGFXEx.h>
#ifdef _ANGELICA3
	#include <A3DGfxEngine.h>
#endif

CPhysXObjParticle::CPhysXObjParticle(int UID)
{
	m_pGfxExMan = 0;
	m_pGfxEx = 0;
	SetDefaultOBB();
	m_propParticle.InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_PARTICLE, *this, UID);
	SetProperties(m_propParticle);
	SetRenderable(m_GFXRender);
}

void CPhysXObjParticle::SetDefaultOBB()
{
	m_EmitterOBB.Clear();
	m_EmitterOBB.Extents.Set(0.3f, 0.3f, 0.3f);
	m_EmitterOBB.XAxis.Set(1, 0, 0);
	m_EmitterOBB.YAxis.Set(0, 1, 0);
	m_EmitterOBB.ZAxis.Set(0, 0, 1);
	m_EmitterOBB.CompleteExtAxis();
}

bool CPhysXObjParticle::OnLoadModel(IEngineUtility& eu, const char* szFile)
{
	if (0 == m_pGfxExMan)
	{
		m_pGfxExMan = eu.GetA3DGfxExMan();
		if (0 == m_pGfxExMan)
		{
			a_LogOutput(1, "CPhysXObjParticle::OnLoadModel: Failed to get A3DGFXExMan!");
			return false;
		}
	}

#ifdef _ANGELICA3
	GFXPhysXSceneShell* pPSS = eu.GetGFXPhysXSceneShell();
	A3DGFXEx* pGfxEx = m_pGfxExMan->LoadGfx(szFile, pPSS, true);
#else
	A3DGFXEx* pGfxEx = m_pGfxExMan->LoadGfx(eu.GetA3DDevice(), szFile);
#endif
	if (0 == pGfxEx)
	{
		a_LogOutput(1, "CPhysXObjParticle::OnLoadModel: Failed to load gfx file! (Name: %s)", szFile);
		return false;
 	}

	m_pGfxEx = pGfxEx;
	m_GFXRender.SetModelPtr(m_pGfxEx);
	return true;
}

void CPhysXObjParticle::OnReleaseModel()
{
	if (0 != m_pGfxExMan)
		m_pGfxExMan->CacheReleasedGfx(m_pGfxEx);
	if (0 != m_pGfxEx)
	{
		m_pGfxEx = 0;
		SetDefaultOBB();
	}
}

bool CPhysXObjParticle::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert(0 != m_pGfxEx);
	return m_pGfxEx->Start();
}

void CPhysXObjParticle::OnReleasePhysXObj()
{
	if(0 != m_pGfxEx)
		m_pGfxEx->Stop();
}

bool CPhysXObjParticle::OnTickAnimation(const unsigned long deltaTime)
{
	assert(0 != m_pGfxEx);
	return m_pGfxEx->TickAnimation(deltaTime);
}

CPhysXObjParticle* CPhysXObjParticle::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_PARTICLE, true, *m_pScene);
	CPhysXObjParticle* pObj = dynamic_cast<CPhysXObjParticle*>(pObjBase);
	if (0 != pObj)
		pObj->m_propParticle = m_propParticle;
	return pObj;
}

void CPhysXObjParticle::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
{
	assert(0 != m_pGfxEx);
	IPhysXObjBase::BuildExtraRenderDataMyself(er, dwColor);
	if (!m_propParticle.m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		er.AddSphere(m_EmitterOBB.Center, 0.1f, 0xffff0000);
		int dwTheColor = 0xffff0000;
		if (0 == dwColor)
			er.AddOBB(m_EmitterOBB, dwTheColor);
	}
}

bool CPhysXObjParticle::OnGetPos(A3DVECTOR3& vOutPos) const
{
	if (0 == m_pGfxEx)
		return false;

	vOutPos = m_pGfxEx->GetPos();
	return true; 
}

void CPhysXObjParticle::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(0 != m_pGfxEx);
	m_pGfxEx->SetPos(vPos);
	m_EmitterOBB.Center = vPos;
}

bool CPhysXObjParticle::OnGetDir(A3DVECTOR3& vOutDir) const
{
	if (0 == m_pGfxEx)
		return false;

	// Note: there is no proper interface to get dir from A3DGFXEx. We work around it.
	vOutDir = m_EmitterOBB.ZAxis;
	return true; 
}

bool CPhysXObjParticle::OnGetUp(A3DVECTOR3& vOutUp) const
{
	if (0 == m_pGfxEx)
		return false;

	// Note: there is no proper interface to get up from A3DGFXEx. We work around it.
	vOutUp = m_EmitterOBB.YAxis;
	return true; 
}

void CPhysXObjParticle::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(0 != m_pGfxEx);
	m_pGfxEx->SetDirAndUp(vDir, vUp);
	m_EmitterOBB.ZAxis = vDir;
	m_EmitterOBB.YAxis = vUp;
	m_EmitterOBB.XAxis = CrossProduct(vUp, vDir);
	m_EmitterOBB.CompleteExtAxis();
}

bool CPhysXObjParticle::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	if (0 == m_pGfxEx)
		return false;

	// Note: there is no proper interface to get up from A3DGFXEx. We work around it.
	matOutPose.SetRow(0, m_EmitterOBB.XAxis);
	matOutPose.SetRow(1, m_EmitterOBB.YAxis);
	matOutPose.SetRow(2, m_EmitterOBB.ZAxis);
	matOutPose.SetRow(3, m_EmitterOBB.Center);
	outWithScaled = false;
	return true; 
}

void CPhysXObjParticle::OnSetPose(const A3DMATRIX4& matPose)
{
	OnSetPos(matPose.GetRow(3));
	OnSetDirAndUp(matPose.GetRow(2), matPose.GetRow(1));
}

bool CPhysXObjParticle::GetAPhysXInstanceAABB(NxBounds3& outAABB) const
{
	if (0 != m_pGfxEx)
	{
		outAABB.setCenterExtents(APhysXConverter::A2N_Vector3(m_EmitterOBB.Center), APhysXConverter::A2N_Vector3(m_EmitterOBB.Extents));
		return true;
	}
	return false;
}

bool CPhysXObjParticle::OnGetModelAABB(A3DAABB& outAABB) const
{
	if (0 == m_pGfxEx)
		return false;
	
	outAABB.Center = m_EmitterOBB.Center;
	outAABB.Extents = m_EmitterOBB.Extents;
	outAABB.CompleteMinsMaxs();
	return true;
}

int CPhysXObjParticle::GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor) const
{
	return 0;
}

bool CPhysXObjParticle::ControlState(bool EnablePlay)
{
	assert(0 != m_pGfxEx);
	if (EnablePlay)
	{
		if (ST_STOP == m_pGfxEx->GetState() || m_pGfxEx->IsParticleEmitStopped())
			m_pGfxEx->Start();
	}
	else
	{
		if (ST_PLAY == m_pGfxEx->GetState())
			m_pGfxEx->StopParticleEmit();
	}
	
	return true;
}
