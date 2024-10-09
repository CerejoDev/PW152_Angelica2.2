/*
 * FILE: UserContactReport.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2012/01/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_USERCONTACTREPORT_H_
#define _APHYSXLAB_USERCONTACTREPORT_H_

class ApxContactReport : public NxUserContactReport
{
public:
	ApxContactReport();
	void InitCR(GeneralAttackMgr& gam) { m_pGAM = &gam; }
	void ReleaseCR() { m_pGAM = 0; }

	void RegisterActor(ApxActorBase* pAAB);
	void UnRegisterActor(ApxActorBase* pAAB);
	void ClearAllRegisteredActors();

	// only handle the contact report if the value >= threshold
	// range: [0, +)   default value: 5
	void SetRelativeLinearVelocityThreshold(float rlvt);
	float GetRelativeLinearVelocityThreshold() const { return m_RelLVelThreshold; }
	// only handle the contact report if the value >= threshold
	// range: [0, +)   default value: 30
	void SetRelativeAngularVelocityThreshold(float ravt);
	float GetRelativeAngularVelocityThreshold() const { return m_RelAVelThreshold; }

	void ClearCRHit() { m_HitActors.RemoveAll(false); }

	virtual void onContactNotify(NxContactPair& pair, NxU32 events);

private:
	ApxActorBase* FindTheActor(IPhysXObjBase* pObj);
	bool IsAttacked(ApxActorBase* pHit);
	bool GetInfoFromCR(NxContactPair& pair, IAssailable*& poutAttacker,	IPhysXObjBase*& poutAttackerObj, NxActor*& poutAttackerRB, 
											NxActor*& poutHitRB, ApxActorBase*& poutHitActor, int& outRelCode);

private:
	float m_RelLVelThreshold;
	float m_RelAVelThreshold;
	GeneralAttackMgr*	m_pGAM;	
	APtrArray<ApxActorBase*>	m_RegActors;
	APtrArray<ApxActorBase*>	m_HitActors;
};

class APhysXCharacterController;
class CCHitReport : public APhysXCCHitReport
{
public:
	virtual void onShapeHit(const APhysXCCShapeHit& hit) { }//OutputDebugString("---onShapeHit---\n"); }
	virtual void onControllerHit(const APhysXCCsHit& hit) { }//OutputDebugString("---onControllerHit---\n"); }

	virtual void onOverlapBegin(const APhysXCCOverlap& ol);
	virtual void onOverlapContinue(const APhysXCCOverlap& ol);
	virtual void onOverlapEnd(APhysXCharacterController& controller);
};

#endif
