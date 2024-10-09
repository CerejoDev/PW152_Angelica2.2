/*
* FILE: PhysXObjDynamic.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/05/19
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJDYNAMIC_H_
#define _APHYSXLAB_PHYSXOBJDYNAMIC_H_

#include "PhysXObjBase.h"

class CPhysXObjDynamic : public IPhysXObjBase
{
public:
	enum CollisionChannel
	{
		CNL_UNKNOWN				= 0,
		CNL_COMMON				= 1,
		CNL_INDEPENDENT			= 2,
		CNL_INDEPENDENT_CCHOST  = 3,
	};

public:
	CPhysXObjDynamic() { m_pAdvanceIA = 0; m_pParent = 0; }
	bool ApplyPhysXBody(const bool bEnable, APhysXScene* pScene = 0);

	float GetHeight() const;
	A3DVECTOR3 GetHeadTopPos(A3DMATRIX4* pOutMat = 0) const;
	bool  GetCollisionChannel(APhysXCollisionChannel& outChannel) const;
	float GetYOffsetForRevive() const;
	void  SetAdvanceAssailable(IAssailable* pIA) { m_pAdvanceIA = pIA; }

	CPhysXObjDynamic* GetParentModel() const { return m_pParent; }
	void SetParentModel(CPhysXObjDynamic* parent, const AString& hangerName);
	AString GetParentHangerName() const { return m_HangerName; }

	virtual CDynProperty* GetProperties() = 0;
	virtual const CDynProperty* GetProperties() const = 0;
	virtual A3DSkinModel* GetSkinModel() const = 0;
	virtual CECModel* GetECModel() const { return NULL; }

	virtual A3DModelPhysics* GetA3DModelPhysics() const = 0;
	virtual APhysXSkeletonRBObject* GetSkeletonRBObject() const = 0;
	virtual bool SetCollisionChannel(const CollisionChannel cnl) = 0;
	virtual bool AddForce(const NxVec3& pt, const NxVec3& dirANDmag, int iPhysForceType, float fSweepRadius = 0.0f) = 0;

	virtual IAssailable* GetAssailableInterface();
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual void OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker = 0);
	virtual bool OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo);

protected:
	virtual void OnLeaveRuntime();
	virtual bool OnTickAnimation(const unsigned long deltaTime);

private:
	// forbidden behavior
	CPhysXObjDynamic(const CPhysXObjDynamic& rhs);
	CPhysXObjDynamic& operator= (const CPhysXObjDynamic& rhs);

	virtual bool OnTickMove(float deltaTimeSec);
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;

private:
	IAssailable* m_pAdvanceIA;

	CPhysXObjDynamic* m_pParent;
	AString			  m_HangerName;
};

#endif