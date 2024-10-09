#include "stdafx.h"
#include "ActorMoveControllerImp.h"
#include "Collision.h"

//   ------ Deprecated! "ActorMoveCtrlImp.cpp" instead of this ------

using namespace CHBasedCD;
GroundMoveControllerBrush::GroundMoveControllerBrush()
{
	BrushcharCtrlerDes des;
	des.fCapsuleRadius = m_vExtents.x;
	des.fCylinerHei = 2 * (m_vExtents.y - m_vExtents.x);
	des.fGravityAccel = GetGravity();
	des.fSpeed = m_fMoveSpeed;

	m_pBrushCC = new BrushcharCtrler(des);
}
GroundMoveControllerBrush::~GroundMoveControllerBrush()
{
	delete m_pBrushCC;
}

bool GroundMoveControllerBrush::IsInAir() const
{
	return m_pBrushCC->IsInAir();
}
bool GroundMoveControllerBrush::CheckVolumeValid(const NxVec3& vPos, const NxVec3& vExtents)
{
	A3DVECTOR3 vStart = APhysXConverter::N2A_Vector3(vPos);
	return BrushcharCtrler::IsPosAvailable(vStart, vExtents.x, 2 * (vExtents.y - vExtents.x));
}

void GroundMoveControllerBrush::PreTickMove(float dt)
{
	m_pBrushCC->UpdateDynDes();	
}

void GroundMoveControllerBrush::TickMove(float dt)
{
	if(CheckFlag(ApxActor_MoveFlag_Driven))
		m_pBrushCC->MoveCC(dt, APhysXConverter::N2A_Vector3(m_vMoveDir));
	else
		m_pBrushCC->MoveCC(dt, A3DVECTOR3(0, 0, 0));

	m_vPos = APhysXConverter::A2N_Vector3(m_pBrushCC->GetCapsule().GetPos());

	m_vHeadDir = APhysXConverter::A2N_Vector3(m_pBrushCC->GetDir());
}
void GroundMoveControllerBrush::DoJump(float fUpSpeed)
{
	m_pBrushCC->JumpCC(fUpSpeed);
}
float GroundMoveControllerBrush::GetDistToGround()
{
	CCapsule& capsule = m_pBrushCC->GetCapsule();
	float dist = 10;

	env_trace_t env;
	env.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	env.fCapsuleHei = capsule.GetHeight();
	env.fCapsuleR = capsule.GetRadius();
	env.vStart = APhysXConverter::N2A_Vector3(m_vPos);
	env.vDelta = A3DVECTOR3(0, -dist, 0);
	env.vTerStart = env.vStart;
	env.vTerStart.y -= capsule.GetCenterHei();
	
	if (GetCollisionSupplier()->CollideWithEnv(&env))
	{
		return env.fFraction * dist;
	}
	return dist;
}

void GroundMoveControllerBrush::OnSetPos()
{
	CCapsule& capsule = m_pBrushCC->GetCapsule();
	capsule.SetPos(APhysXConverter::N2A_Vector3(m_vPos));
}
void GroundMoveControllerBrush::OnSetExtents()
{
	CCapsule& capsule = m_pBrushCC->GetCapsule();
	capsule.SetRadius(m_vExtents.x);
	capsule.SetHeight(2*(m_vExtents.y - m_vExtents.x));
	CDR_INFO& info = m_pBrushCC->GetCDRInfo();
	info.fCapsuleR = capsule.GetRadius();
	info.fCapsuleHei = capsule.GetHeight();
	info.fCapsuleCenterHei = capsule.GetCenterHei();
}
void GroundMoveControllerBrush::OnSetMoveDir()
{
}
void GroundMoveControllerBrush::OnSetHeadDir()
{
	m_pBrushCC->SetDir(APhysXConverter::N2A_Vector3(m_vHeadDir));
}
void GroundMoveControllerBrush::OnSetMoveSpeed()
{
	m_pBrushCC->SetSpeed(m_fMoveSpeed);
	m_pBrushCC->GetCDRInfo().fSpeed = m_fMoveSpeed;
}

void GroundMoveControllerBrush::OnSetGravity()
{
	m_pBrushCC->GetCDRInfo().fGravityAccel = GetGravity();
}

void GroundMoveControllerBrush::GetGroundPos(NxVec3& vPos) const
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
	{
		vPos.y -= (dist + m_vExtents.y) * env.fFraction;
	}
	
}

NxVec3 GroundMoveControllerBrush::GetVelocity() const
{
	CDR_INFO& info = m_pBrushCC->GetCDRInfo();
	if (info.bBlocked)
		return NxVec3(0, 0, 0);
	return APhysXConverter::A2N_Vector3(info.vAbsVelocity);
}