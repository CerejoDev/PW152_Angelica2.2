/*
 * FILE: UserContactReport.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2012/01/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

ApxContactReport::ApxContactReport()
{
	m_RelLVelThreshold = 5.0f;
	m_RelAVelThreshold = 30.0f;
	m_pGAM = 0;
}

void ApxContactReport::RegisterActor(ApxActorBase* pAAB)
{
	if (0 == pAAB)
		return;

	int nCount = m_RegActors.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_RegActors[i] == pAAB)
			return;
	}
	m_RegActors.Add(pAAB);
}

void ApxContactReport::UnRegisterActor(ApxActorBase* pAAB)
{
	if (0 == pAAB)
		return;

	int nCount = m_RegActors.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_RegActors[i] == pAAB)
		{
			m_RegActors.RemoveAtQuickly(i);
			return;
		}
	}
}

void ApxContactReport::ClearAllRegisteredActors()
{
	m_RegActors.RemoveAll(false);
}

void ApxContactReport::SetRelativeLinearVelocityThreshold(float rlvt)
{
	if (0 < rlvt)
		m_RelLVelThreshold = rlvt;
}

void ApxContactReport::SetRelativeAngularVelocityThreshold(float ravt)
{
	if (0 < ravt)
		m_RelAVelThreshold = ravt;
}

ApxActorBase* ApxContactReport::FindTheActor(IPhysXObjBase* pObj)
{
	assert(0 != pObj);
	IPhysXObjBase* pRegObj = 0;
	int nCount = m_RegActors.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		pRegObj = m_RegActors[i]->GetObjDyn();
		if (pRegObj == pObj)
			return m_RegActors[i];
	}
	return 0; 
}

bool ApxContactReport::IsAttacked(ApxActorBase* pHit)
{
	int nCount = m_HitActors.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_HitActors[i] == pHit)
			return true;
	}
	return false;
}

bool ApxContactReport::GetInfoFromCR(NxContactPair& pair, IAssailable*& poutAttacker, IPhysXObjBase*& poutAttackerObj, NxActor*& poutAttackerRB, NxActor*& poutHitRB, ApxActorBase*& poutHitActor, int& outRelCode)
{
	outRelCode = 0;
	IPhysXObjBase* pObj0 = ObjManager::GetInstance()->GetPhysXObject(*(pair.actors[0]));
	IPhysXObjBase* pObj1 = ObjManager::GetInstance()->GetPhysXObject(*(pair.actors[1]));
	if ((0 == pObj0) || (0 == pObj1))
		return false;

	poutAttacker = pObj0;
	poutAttackerObj = pObj0;
	poutAttackerRB = pair.actors[0];
	poutHitRB = pair.actors[1];
	poutHitActor = FindTheActor(pObj1);
	if (0 == poutHitActor)
	{
		poutAttacker = pObj1;
		poutAttackerObj = pObj1;
		poutAttackerRB = pair.actors[1];
		poutHitRB = pair.actors[0];
		poutHitActor = FindTheActor(pObj0);
	}
	if (0 != poutHitActor)
	{
		outRelCode = pObj0->GetRelationshipCode(*pObj1);
		if (0 == outRelCode)
		{
			CPhysXObjECModel* pECMObj0 = 0;
			CPhysXObjECModel* pECMObj1 = 0;
			if (ObjManager::OBJ_TYPEID_ECMODEL == pObj0->GetProperties()->GetObjType())
				pECMObj0 = dynamic_cast<CPhysXObjECModel*>(pObj0);
			if (ObjManager::OBJ_TYPEID_ECMODEL == pObj1->GetProperties()->GetObjType())
				pECMObj1 = dynamic_cast<CPhysXObjECModel*>(pObj1);
			if ((0 != pECMObj0) && (0 != pECMObj1))
			{
				CECModel* pECM0 = pECMObj0->GetECModel();
				CECModel* pECM1 = pECMObj1->GetECModel();
				int nAlls = pECM0->GetChildCount();
				for (int i = 0; i < nAlls; ++i)
				{
					if (pECM1 == pECM0->GetChildModel(i))
						return false;
				}
				nAlls = pECM1->GetChildCount();
				for (int i = 0; i < nAlls; ++i)
				{
					if (pECM0 == pECM1->GetChildModel(i))
						return false;
				}
			}
		}
		return true;
	}

	poutAttacker = 0;
	poutAttackerObj = 0;
	poutAttackerRB = 0;
	poutHitRB = 0;
	return false;
}

void ApxContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	if (0 == m_pGAM)
		return;
	if (NX_NOTIFY_ON_START_TOUCH != events)
		return;
	if (pair.isDeletedActor[0])
		return;
	if (pair.isDeletedActor[1])
		return;
	if (0 != (NX_IGNORE_PAIR & pair.actors[0]->getContactReportFlags()))
		return;
	if (0 != (NX_IGNORE_PAIR & pair.actors[1]->getContactReportFlags()))
		return;

	IAssailable* pAttacker = 0;
	IPhysXObjBase* pAttackerObj = 0;
	NxActor* pAttackerRB = 0;
	NxActor* pHitRB = 0;
	ApxActorBase* pHitActor = 0;
	int nRelCode = 0;
	if (!GetInfoFromCR(pair, pAttacker, pAttackerObj, pAttackerRB, pHitRB, pHitActor, nRelCode))
		return;
	if (IsAttacked(pHitActor))
		return;
	if (0 != nRelCode)
		return;

	NxVec3 vAttLVel = pAttackerRB->getLinearVelocity();
	NxVec3 v1 = pHitActor->GetGroundMoveController()->GetVelocity();
	NxVec3 vRelL = v1 - vAttLVel;
	float fRelL = vRelL.normalize();
	NxVec3 vAttAVel = pAttackerRB->getAngularVelocity();
	float fRelA = vAttAVel.normalize();
	if (GetRelativeLinearVelocityThreshold() > fRelL)
	{
		if (GetRelativeAngularVelocityThreshold() > fRelA)
			return;
	}
	if (!pHitActor->OnPreAttackedQuery())
		return;
	if (pHitActor->GetGroundMoveController()->IsPushingObj(pAttackerObj))
		return;
	
	// compute the central contact pos
	NxContactStreamIterator csi(pair.stream);
	int iPointNum = 0;
	NxVec3 vCentralContactPos(0.0f);
	while(csi.goNextPair())
	{
		while(csi.goNextPatch())
		{
			const NxVec3& contactNormal = csi.getPatchNormal();
			while(csi.goNextPoint())
			{
				iPointNum++;				
				const NxVec3& contactPoint = csi.getPoint();
				vCentralContactPos += contactPoint;
			}
		}
	}
	vCentralContactPos /= (float)iPointNum;
	NxVec3 vDir = pAttackerRB->getPointVelocity(vCentralContactPos);
	float ptVel = vDir.normalize();

	ApxAttackActionInfo aaai;
	aaai.mApxAttack.mIsPhysXDetection = true;
	aaai.mApxAttack.mAttackType = 2;
	aaai.mApxAttack.mpTarget = pHitActor;
	aaai.mApxDamage.mpAttacker = pAttacker;
	aaai.mApxDamage.mHitRBActor = pHitRB;
	aaai.mApxDamage.mHitPos = vCentralContactPos;
	aaai.mApxDamage.mHitDir = vDir;
	aaai.UpdateDamageSource();

	PhysXRBDesc rbDesc;
	rbDesc.IsKinematic = pAttackerRB->readBodyFlag(NX_BF_KINEMATIC);
	rbDesc.ContactPointVelocity = ptVel * vDir;
	rbDesc.ContactRBLinearVelocity = vAttLVel;
	rbDesc.ContactRBAngularVelocity = vAttAVel;
	rbDesc.ContactRBMass = pAttackerRB->getMass();
	APtrArray<NxActor*> Actors;
	int nCount = pAttackerObj->GetNxActors(Actors);
	for (int i = 0; i < nCount; ++i)
		rbDesc.SkeletonRBMass += Actors[i]->getMass();

	pHitActor->OnPreAttackedNotify(aaai, &rbDesc);
	m_pGAM->AddCurrentDamage(aaai);
	m_HitActors.Add(pHitActor);
}

void CCHitReport::onOverlapBegin(const APhysXCCOverlap& ol)
{
	OutputDebugString(_T("---onOverlapBegin---\n"));

	NxActor* pObj = 0;
	NxShape* pShape = 0;
	NxShapeType st;
	NxVec3 vol;
	TString strD;

	int nSS = ol.nbNxActors;
	for (int i = 0; i < nSS; ++i)
	{
		NxActor* pObj = ol.nxactors[i];
		strD.Format(_T("index = %d, name = %s\n"), i, _TAS2WC(pObj->getName()));
		OutputDebugString(strD);

		int nshape = pObj->getNbShapes();
		for (int j = 0; j < nshape; ++j)
		{
			pShape = pObj->getShapes()[j];
			st = pShape->getType();
			vol.set(0.0f);
			if (NX_SHAPE_BOX == st)
			{
				NxBoxShape* pb = pShape->isBox();
				vol = pb->getDimensions();
				strD.Format(_T("	NxShapeType = %d, vol: x = %f, y = %f, z = %f\n"), st, vol.x, vol.y, vol.z);
				OutputDebugString(strD);
			}
			else if (NX_SHAPE_CAPSULE == st)
			{
				NxCapsuleShape* pc = pShape->isCapsule();
				vol.x = pc->getHeight();
				vol.y = pc->getRadius();
				vol.z = 0;
				strD.Format(_T("	NxShapeType = %d, vol: x = %f, y = %f, z = %f\n"), st, vol.x, vol.y, vol.z);
				OutputDebugString(strD);
			}
			else if (NX_SHAPE_SPHERE == st)
			{
				NxSphereShape* ps = pShape->isSphere();
				vol.x = ps->getRadius();
				vol.y = vol.z = 0;
				strD.Format(_T("	NxShapeType = %d, vol: x = %f, y = %f, z = %f\n"), st, vol.x, vol.y, vol.z);
				OutputDebugString(strD);
			}
			else
			{
				strD.Format(_T("	NxShapeType = %d\n"), st);
				OutputDebugString(strD);
			}
		}
	}
	OutputDebugString(_T("---onOverlapBegin(end)---\n"));
}

void CCHitReport::onOverlapContinue(const APhysXCCOverlap& ol)
{
	//	OutputDebugString(_T("---onOverlapContinue---\n"));
}

void CCHitReport::onOverlapEnd(APhysXCharacterController& APhysXCC)
{
	OutputDebugString(_T("---onOverlapEnd---\n"));
}

