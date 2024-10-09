/*
* FILE: PhysXObjDynamic.cpp
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

void CPhysXObjDynamic::OnLeaveRuntime()
{
	IPhysXObjBase::OnLeaveRuntime();
	ResetRuntimePose();
}

bool CPhysXObjDynamic::OnTickMove(float deltaTimeSec)
{
	CDynProperty* pProps = GetProperties();
	if (pProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (!pProps->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		{
			pProps->UpdateMove(deltaTimeSec);
			pProps->DoMoveCC(deltaTimeSec);
		}

		if (m_pScene->QueryRPTStateEnable(GetGPos()))
		{
			if (0 == GetSkeletonRBObject())
				InstancePhysXObj(*(m_pScene->GetAPhysXScene()));
		}
		else
		{
			if (0 != GetSkeletonRBObject())
				ReleasePhysXObj();
		}
	}
	return true;
}

bool CPhysXObjDynamic::OnTickAnimation(const unsigned long dwDeltaTime)
{
	CDynProperty* pProps = GetProperties();
	// Note: main actor must be runtime only! Or it can't be reset pose to original edited position.
	if (pProps->QueryRoleType(CDynProperty::ROLE_ACTOR_AVATAR))
	{
		assert(true == pProps->GetFlags().ReadFlag(OBF_RUNTIME_ONLY));
	}

	return pProps->PlayAction(dwDeltaTime);
}

bool CPhysXObjDynamic::ApplyPhysXBody(const bool bEnable, APhysXScene* pScene)
{
	CDynProperty* pProp = GetProperties();
	bool lastVal = pProp->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	if (bEnable == lastVal)
		return true;

	if (bEnable)
	{
		pProp->GetFlags().RaiseFlag(OBF_ENABLE_PHYSX_BODY);
		if (pProp->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			assert(0 != pScene);
			if (0 == pScene)
				return false;
			if (m_pScene->QueryRPTStateEnable(GetGPos()))
			{
				if (!InstancePhysXObj(*pScene))
					return false;
			}

			// to deal with the case: CC is existing with No-RBbody then create RB Body. to set channel correct!!
			pProp->GetCCMgr().RegisterHostModels(this);
		}
	}
	else
	{
		pProp->GetFlags().ClearFlag(OBF_ENABLE_PHYSX_BODY);
		if (pProp->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			ReleasePhysXObj();
		pProp->GetCCMgr().UnRegisterHostModels(this);
	}
	return true;
}

float CPhysXObjDynamic::GetHeight() const
{
	const CDynProperty* pProp = GetProperties();
	float height = pProp->GetOriginalLoacalHeight();
	height *= pProp->GetScale();
	height *= 1.05f;
	return height;
}

A3DVECTOR3 CPhysXObjDynamic::GetHeadTopPos(A3DMATRIX4* pOutMat) const
{
	A3DSkinModel* pModel = GetSkinModel();
	A3DMATRIX4 mat = pModel->GetAbsoluteTM();
	if (0 != pOutMat)
		*pOutMat = mat;

	float height = GetHeight();
	A3DVECTOR3 up(0, height, 0); 
	A3DVECTOR3 newUp = up * mat; 
	return newUp;
}

bool CPhysXObjDynamic::GetCollisionChannel(APhysXCollisionChannel& outChannel) const
{
	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if (0 == pSRB)
		return false;

	outChannel = pSRB->GetCollisionChannel();
	return true;
}

float CPhysXObjDynamic::GetYOffsetForRevive() const
{
	float yOffset = 0;
	const CDynProperty* pProp = GetProperties();
	if (pProp->QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
		return yOffset;
		
	if (pProp->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME) && pProp->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
	{
		A3DAABB aabb;
		NxBounds3 nxAABB;
		if (GetObjAABB(nxAABB))
		{
			APhysXConverter::N2A_AABB(nxAABB, aabb);
		}
		else
		{
			if (!GetModelAABB(aabb))
				aabb.Mins.y = aabb.Maxs.y = 0;
		}
		yOffset = (aabb.Maxs.y - aabb.Mins.y) * 0.5f;
	}
	return yOffset;
}

void CPhysXObjDynamic::SetParentModel(CPhysXObjDynamic* parent, const AString& hangerName)
{
	m_pParent = parent;
	if (0 == parent)
		m_HangerName.Empty();
	else
		m_HangerName = hangerName;
}

IAssailable* CPhysXObjDynamic::GetAssailableInterface()
{
	if (0 == m_pAdvanceIA)
		return this;
	return m_pAdvanceIA; 
}

int CPhysXObjDynamic::GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor) const
{
	APhysXCCBase* pCC = GetProperties()->GetCCMgr().GetAPhysXCCBase();
	if ((0 != pTestActor) && (0 != pCC))
	{
		if (pCC->IsComponent(*pTestActor))
			outVec.Add(pTestActor);
	}

	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return 0;
	
	int nCount = pPSRB->GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
		outVec.Add(pPSRB->GetNxActor(i));
	return nCount;
}

bool CPhysXObjDynamic::GetAPhysXInstanceAABB(NxBounds3& outAABB) const
{
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return false;

	pPSRB->GetAABB(outAABB);
	int nCount = pPSRB->GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
		outAABB.include(pPSRB->GetNxActor(i)->getGlobalPosition());
	return true;
}

void CPhysXObjDynamic::OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker)
{
	bool bIsBreakable = false;
	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if (0 != pSRB)
	{
		if (0 != pSRB->IsBreakable())
			bIsBreakable = true;
	}
	if (bIsBreakable)
	{
		inoutAAAI.mApxDamage.mHitRBActor = 0;
//		inoutAAAI.mApxDamage.mHitMomentum = 1000;
		inoutAAAI.mApxDamage.mHitMomentum = 1140;
		inoutAAAI.mApxDamage.mHitDamage = 40;
	}
	else
	{
		IPropPhysXObjBase* pProp = GetProperties();
		if (!pProp->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
			pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
		inoutAAAI.mApxDamage.mHitMomentum = 1;
		inoutAAAI.mApxDamage.mHitDamage = 40;
	}
}

bool CPhysXObjDynamic::OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo)
{
	NxVec3 mom = apxDamageInfo.mHitDir * apxDamageInfo.mHitMomentum;
	NxActor* pActor = apxDamageInfo.mHitRBActor;
	if (0 == pActor)
//		return AddForce(apxDamageInfo.mHitPos, mom, CECModel::PFT_IMPULSE, 0.1f);
		return AddForce(apxDamageInfo.mHitPos, mom, CECModel::PFT_FORCE, 0.15f);

	pActor->addForceAtPos(mom, apxDamageInfo.mHitPos, NX_IMPULSE);
	return true;
}
