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

CPhysXObjParticle::CPhysXObjParticle() : IPhysXObjBase(CPhysXObjMgr::OBJ_TYPEID_PARTICLE)
{
	m_pGfxEx = 0;
	m_EmitterOBB.Clear();
	m_EmitterOBB.Extents.Set(0.3f, 0.3f, 0.3f);
	m_EmitterOBB.XAxis.Set(1, 0, 0);
	m_EmitterOBB.YAxis.Set(0, 1, 0);
	m_EmitterOBB.ZAxis.Set(0, 0, 1);
	m_EmitterOBB.CompleteExtAxis();
}

bool CPhysXObjParticle::OnLoadModel(CRender& render, const char* szFile)
{
	// note here, szFile should be under the dir of "\Gfx\" and only the file name without any path name...
	AString strOnlyFileName;
	af_GetFileTitle(szFile, strOnlyFileName);

	A3DGFXEx* pGfxEx = AfxGetGFXExMan()->LoadGfx(render.GetA3DDevice(), strOnlyFileName);
	if(0 == pGfxEx)
	{
		a_LogOutput(1, "CPhysXObjParticle::OnLoadModel: Failed to load gfx file! (Name: %s)", szFile);
		return false;
 	}

	m_pGfxEx = pGfxEx;
	return true;
}

void CPhysXObjParticle::OnReleaseModel()
{
	AfxGetGFXExMan()->CacheReleasedGfx(m_pGfxEx);
	m_pGfxEx = 0;
	m_EmitterOBB.Clear();
	m_EmitterOBB.Extents.Set(0.3f, 0.3f, 0.3f);
	m_EmitterOBB.XAxis.Set(1, 0, 0);
	m_EmitterOBB.YAxis.Set(0, 1, 0);
	m_EmitterOBB.ZAxis.Set(0, 0, 1);
	m_EmitterOBB.CompleteExtAxis();
}

bool CPhysXObjParticle::OnTickAnimation(const unsigned long deltaTime)
{
	assert(0 != m_pGfxEx);
	return m_pGfxEx->TickAnimation(deltaTime);
}

void CPhysXObjParticle::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	assert(0 != m_pGfxEx);
	if (ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if(ST_PLAY == m_pGfxEx->GetState())
			AfxGetGFXExMan()->RegisterGfx(m_pGfxEx);
	}
	else
	{
		assert(0 != pWC);
		if (!ReadFlag(OBF_DRAW_BINDING_BOX))
			pWC->AddOBB(m_EmitterOBB, 0xffff0000);
		pWC->AddSphere(m_EmitterOBB.Center, 0.1f, 0xffff0000);
	}
}

void CPhysXObjParticle::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(0 != m_pGfxEx);
	m_pGfxEx->SetPos(vPos);
	m_EmitterOBB.Center = vPos;
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

bool CPhysXObjParticle::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	assert(0 != m_pGfxEx);
	return m_pGfxEx->Start();
}

void CPhysXObjParticle::OnReleasePhysXObj(const bool RunTimeIsEnd)
{
	if(0 != m_pGfxEx)
		m_pGfxEx->Stop();
}

bool CPhysXObjParticle::GetPhysXRigidBodyAABB(NxBounds3& outAABB) const
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

int CPhysXObjParticle::GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor) const
{
	return 0;
}

NxActor* CPhysXObjParticle::GetFirstNxActor() const
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
