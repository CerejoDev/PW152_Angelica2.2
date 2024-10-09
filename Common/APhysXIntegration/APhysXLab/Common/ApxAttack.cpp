#include "PhysXObjBase.h"

#include "APxAttack.h"

namespace _SGC 
{

//-------------------------------------------------------------------------------------
// implementation of ApxAttackEvent

void ApxAttackEvent::HitTarget()
{
	A3DSkillGfxEvent::HitTarget();

	ApxAttackSkillGfxMan* pAttackMan = (ApxAttackSkillGfxMan *)m_pMan;
	GfxHitPos tmpPos = enumHitCenter;
	A3DVECTOR3 vCaster, vTarget;
	pAttackMan->GetPositionById(m_nCasterID, vCaster, tmpPos);
	pAttackMan->GetPositionById(m_nTargetID, vTarget, tmpPos);

	ApxHitForceInfo apxHitForceInfo;
	apxHitForceInfo.mHitDir = vTarget - vCaster;
	apxHitForceInfo.mHitDir.Normalize();

	
	apxHitForceInfo.mHitForceMagnitude = 	pAttackMan->GetHitForceMag();
	apxHitForceInfo.mHitPos = pAttackMan->GetHitPos();

	IPhysXObjBase* pObj = (IPhysXObjBase* ) m_nTargetID;
	pObj->OnHitbyForce(apxHitForceInfo);

}


//-------------------------------------------------------------------------------------
// implementation of ApxAttackSkillGfxMan

A3DSkillGfxEvent* ApxAttackSkillGfxMan::CreateOneEvent(GfxMoveMode mode)
{

	return new ApxAttackEvent(this, mode);
}

bool ApxAttackSkillGfxMan::GetPositionById(clientid_t nId, A3DVECTOR3& vPos, GfxHitPos HitPos, const char* szHook /* = NULL */, bool bRelHook /* = false */, const A3DVECTOR3* pOffset /* = NULL */, const char* szHanger /* = NULL */ , bool bChildHook /* = false */)
{
	IPhysXObjBase* pObj = (IPhysXObjBase* ) nId;

	if(pObj == m_pHitTarget)
	{
		vPos = m_vHitPos;
	}
	else
	{
		vPos = pObj->GetPos();
		vPos.y += 1.0f;
	}

	return true;
}

bool ApxAttackSkillGfxMan::GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp)
{
	IPhysXObjBase* pObj = (IPhysXObjBase* ) nId;
	
	pObj->GetDirAndUp(vDir, vUp);


	return true;
}

float ApxAttackSkillGfxMan::GetTargetScale(clientid_t nTargetId)
{

	return 1.0f;
}


static ApxAttackSkillGfxMan s_apxAttackSkillGfxMan;

A3DSkillGfxMan* AfxGetSkillGfxEventMan()
{
	return &s_apxAttackSkillGfxMan;
}

}		// end of namespace _SGC 
