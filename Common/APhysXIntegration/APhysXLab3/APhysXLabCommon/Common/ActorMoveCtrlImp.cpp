/*
* FILE: ActorMoveCtrlImp.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/08/02
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"
#include "Collision.h"

using namespace CHBasedCD;

A3DTerrain2* ActorGroundMoveCtrlImp::gpTerrain = 0;
ActorGroundMoveCtrlImp::ActorGroundMoveCtrlImp(CAPhysXCCMgr* pCCMgr)
{
	m_LastFrameIsPushing = false;
	m_PresentIsPushing = false;
	m_pCCMgr = pCCMgr;
	m_CurVerticalVel.set(0.0);
	if (0 != m_pCCMgr)
	{
		assert(0 != m_pCCMgr->GetHostObject());
		m_pCCMgr->newStyleTest = true;
		m_pCCMgr->m_UserData = this;
		m_vExtents = m_pCCMgr->GetDefaultVolume();
		m_vExtents.y *= 0.5f; 
	}

	m_bBlocked = false;
}

NxVec3 ActorGroundMoveCtrlImp::GetVelocity() const
{
	if (IsBlocked())
		return NxVec3(0, 0, 0);
	if (0 != m_pCCMgr)
	{
		BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
		if (0 != pBrushCC)
		{
 			return APhysXConverter::A2N_Vector3(pBrushCC->GetVelocity());
		}

		APhysXHostCCBase* pHostCCBase = m_pCCMgr->GetAPhysXHostCCBase();
		if (0 != pHostCCBase)
			return pHostCCBase->GetInstantVelocity();
	}

	NxVec3 vel(m_CurVerticalVel);
	vel += ApxActorGroundMoveController::GetVelocity();
	return vel;
}

bool ActorGroundMoveCtrlImp::CheckVolumeValid(const NxVec3& vPos, const NxVec3& vExtents)
{
	if (0 == m_pCCMgr)
		return ApxActorGroundMoveController::CheckVolumeValid(vPos, vExtents);

	NxVec3 posFoot;
	GetFootPos(posFoot, &vPos);
	NxVec3 volume(vExtents);
	volume.y *= 2;
	return m_pCCMgr->CheckVolumeValid(posFoot, volume, APhysXCCBase::APX_VT_CAPSULE);
}

bool ActorGroundMoveCtrlImp::IsInAir() const
{
	if (0 != m_pCCMgr)
	{
		if (!m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY))
			return m_pCCMgr->IsInAir();
	}

	return CheckFlag(ACF_IS_INAIR);
}

void ActorGroundMoveCtrlImp::GetGroundPos(NxVec3& vPos) const
{
	if (0 != m_pCCMgr)
	{
		APhysXHostCCBase* pCC = m_pCCMgr->GetAPhysXHostCCBase();
		if (0 != pCC)
		{
			pCC->GetGroundPos(m_vExtents.y * 2, vPos);
			return;
		}
		BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
		if (0 != pBrushCC)
		{
			float heiht = 0;
			A3DVECTOR3 vStart = APhysXConverter::N2A_Vector3(vPos);
			vStart.y += m_vExtents.y;
			const float dist = 0.1f;
			env_trace_t env;
			env.vStart = vStart;
			env.vDelta = A3DVECTOR3(0, -dist - m_vExtents.y, 0);
			env.dwCheckFlag = CDR_BRUSH | CDR_TERRAIN;
			env.fCapsuleHei = 2 * (m_vExtents.y - m_vExtents.x);
			env.fCapsuleR = m_vExtents.x;
			env.vTerStart = vStart;
			env.vTerStart.y -= m_vExtents.y;
			if (GetCollisionSupplier()->CollideWithEnv(&env))
				vPos.y -= (dist + m_vExtents.y) * env.fFraction;
			return;
		}
	}

	if (0 != gpTerrain)
	{
		float fHeight = 0.0f;
		if (GetTerrainHeight(gpTerrain, APhysXConverter::N2A_Vector3(vPos), fHeight))
			vPos.y = fHeight;
	}
}

void ActorGroundMoveCtrlImp::PrepareTick()
{
	if (0 != m_pCCMgr)
	{
		if (0 != m_pCCMgr->GetAPhysXDynCC())
			m_LastPos = m_vPos;
		if (m_pCCMgr->SyncDynCCPose(m_vPos, m_vHeadDir, m_vHeadUp))
			m_vPos += m_vHeadUp * m_vExtents.y;
	}
}

void ActorGroundMoveCtrlImp::PreTickMove(float dt)
{
	ApxActorGroundMoveController::PreTickMove(dt);
	if (0 != m_pCCMgr)
	{
		NxVec3 posFoot;
		GetFootPos(posFoot);
		m_pCCMgr->PreMoveCC(dt, &posFoot);
	}
}

void ActorGroundMoveCtrlImp::TickMove(float dt)
{
	m_PresentIsPushing = false;
	m_PushingObjs.RemoveAll(false);
	if (0 != m_pCCMgr && !IsBlocked())
	{
		NxVec3 moveDir(0.0f);
		if(CheckFlag(ApxActor_MoveFlag_Driven))
			moveDir = m_vMoveDir;

		if (0 == m_pCCMgr->GetAPhysXDynCC())
			m_LastPos = m_vPos;
		if (m_pCCMgr->MoveCC(dt, moveDir))
		{
			if (m_pCCMgr->SyncNonDynCCPose(m_vPos, m_vHeadDir, m_vHeadUp))
				m_vPos += m_vHeadUp * m_vExtents.y;
			return;
		}
	}

	ApxActorMoveController::TickMove(dt);
}

void ActorGroundMoveCtrlImp::PostTickMove(float dt)
{
	if (m_LastFrameIsPushing != m_PresentIsPushing)
	{
		if (m_LastFrameIsPushing)
			m_uGroundMoveState = ApxActor_GroundMoveState_StandInPlace;
		m_LastFrameIsPushing = m_PresentIsPushing;
	}
	ApxActorGroundMoveController::PostTickMove(dt);
}

void ActorGroundMoveCtrlImp::DoJump(float fUpSpeed)
{
	if (0 >= fUpSpeed)
		return;

	if (0 != m_pCCMgr)
	{
		APhysXHostCCBase* pCC = m_pCCMgr->GetAPhysXHostCCBase();
		if (0 != pCC)
		{
			pCC->JumpCC(fUpSpeed, false);
			return;
		}
		BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
		if (0 != pBrushCC)
		{
			pBrushCC->JumpCC(fUpSpeed, false);
			return;
		}
	}

	RaiseFlag(ACF_HAS_NEW_JUMP);
	m_CurVerticalVel += NxVec3(0, fUpSpeed, 0);
}

float ActorGroundMoveCtrlImp::GetDistToGround()
{
	if ((0 == m_pCCMgr) || (m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY)))
	{
		if (0 != gpTerrain)
		{
			float fHeight = 0.0f;
			if (GetTerrainHeight(gpTerrain, APhysXConverter::N2A_Vector3(m_vPos), fHeight))
			{
				fHeight -= m_vExtents.y;
				return fHeight;
			}
		}
		return 0.0f;
	}

	APhysXCCBase* pCCBase = m_pCCMgr->GetAPhysXCCBase();
	if (0 != pCCBase)
		return pCCBase->GetDistToGround();

	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		return pBrushCC->GetDistToGround();
	}
	return 0.0f;
}

bool ActorGroundMoveCtrlImp::DoStartSwing(const NxVec3& vDir)
{
	if (0 == m_pCCMgr)
		return false;

	APhysXDynCharCtrler* pDynCC = m_pCCMgr->GetAPhysXDynCC();
	if (0 == pDynCC)
		return false;

	return pDynCC->HangStart(vDir);
}

void ActorGroundMoveCtrlImp::DoEndSwing()
{
	if (0 == m_pCCMgr)
		return;

	APhysXDynCharCtrler* pDynCC = m_pCCMgr->GetAPhysXDynCC();
	if (0 == pDynCC)
		return;

	pDynCC->HangEnd();
}

void ActorGroundMoveCtrlImp::OnSetPos()
{
	if ((0 == m_pCCMgr) || (m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY)))
		return;

	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		pBrushCC->SetPos(APhysXConverter::N2A_Vector3(m_vPos));
		return;
	}

	APhysXCCBase* pCCBase = m_pCCMgr->GetAPhysXCCBase();
	if (0 != pCCBase)
	{
		NxVec3 posFoot;
		GetFootPos(posFoot);
		pCCBase->SetFootPosition(posFoot);
	}
}

void ActorGroundMoveCtrlImp::OnSetExtents()
{
	if ((0 == m_pCCMgr) || (m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY)))
		return;

	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		CCapsule& capsule = pBrushCC->GetCapsule();
		capsule.SetRadius(m_vExtents.x);
		capsule.SetHeight(2*(m_vExtents.y - m_vExtents.x));
		CDR_INFO& info = pBrushCC->GetCDRInfo();
		info.fCapsuleR = capsule.GetRadius();
		info.fCapsuleHei = capsule.GetHeight();
		info.fCapsuleCenterHei = capsule.GetCenterHei();
		return;
	}

	APhysXCCBase* pCCBase = m_pCCMgr->GetAPhysXCCBase();
	if (0 != pCCBase)
	{
		NxVec3 volume(m_vExtents);
		volume.y *= 2;
		pCCBase->ChangeVolume(volume);
	}
}

void ActorGroundMoveCtrlImp::OnSetMoveDir()
{

}

void ActorGroundMoveCtrlImp::OnSetHeadDir()
{
	if ((0 == m_pCCMgr) || (m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY)))
		return;

	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		pBrushCC->SetDir(APhysXConverter::N2A_Vector3(m_vHeadDir));
		return;
	}

	APhysXHostCCBase* pHostCCBase = m_pCCMgr->GetAPhysXHostCCBase();
	if (0 != pHostCCBase)
	{
		if (!m_pCCMgr->IsHangMode())
			pHostCCBase->SetDir(m_vHeadDir);
	}
}

void ActorGroundMoveCtrlImp::OnSetMoveSpeed()
{
	if ((0 == m_pCCMgr) || (m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY)))
		return;

	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		pBrushCC->SetSpeed(m_fMoveSpeed);
		pBrushCC->GetCDRInfo().fSpeed = m_fMoveSpeed;
		return;
	}

	APhysXHostCCBase* pHostCCBase = m_pCCMgr->GetAPhysXHostCCBase();
	if (0 != pHostCCBase)
		pHostCCBase->SetSpeed(m_fMoveSpeed);
}

void ActorGroundMoveCtrlImp::OnSetGravity()
{
	if ((0 == m_pCCMgr) || (m_pCCMgr->QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY)))
		return;

	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		pBrushCC->GetCDRInfo().fGravityAccel = GetGravity();
		return;
	}

	APhysXHostCCBase* pHostCCBase = m_pCCMgr->GetAPhysXHostCCBase();
	if (0 != pHostCCBase)
	{
		NxVec3 g(0, -1, 0);
		g *= GetGravity();
		NxScene* pScene = pHostCCBase->GetAPhysXScene().GetNxScene();
		pScene->setGravity(g);
	}
}

void ActorGroundMoveCtrlImp::GetFootPos(NxVec3& posFoot, const NxVec3* pCenterPos) const
{
	if (0 == pCenterPos)
		posFoot = m_vPos;
	else
		posFoot = *pCenterPos;
	posFoot -= m_vHeadUp * m_vExtents.y;
}

bool ActorGroundMoveCtrlImp::IsPushingObj(IPhysXObjBase* pObj) const
{
	int nCount = m_PushingObjs.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_PushingObjs[i] == pObj)
			return true;
	}
	return false; 
}

void ActorGroundMoveCtrlImp::OnPushObj(IPhysXObjBase* pObj)
{
	m_PresentIsPushing = true;
	if (!IsPushingObj(pObj))
		m_PushingObjs.Add(pObj);
	ApxActorGroundMoveController::OnPushObj(pObj);
}
