/*
 * FILE: PhysXObjBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJBASE_H_
#define _APHYSXLAB_PHYSXOBJBASE_H_

#include "IObjBase.h"

class IEngineUtility;
class GeneralAttackMgr;
class ApxActorBase;
struct ApxAttackActionInfo;
struct ApxDamageInfo;

struct PhysXRBDesc 
{
	PhysXRBDesc()
	{
		IsKinematic = false;
		ContactRBMass = SkeletonRBMass = 0.0f;
		ContactPointVelocity.zero();
		ContactRBLinearVelocity.zero();
		ContactRBAngularVelocity.zero();
	}

	bool IsKinematic; 
	float ContactRBMass;
	float SkeletonRBMass;

	NxVec3 ContactPointVelocity;
	NxVec3 ContactRBLinearVelocity;
	NxVec3 ContactRBAngularVelocity;
};

class IAssailable
{
public:
	static void SetGeneralAttackMgr(GeneralAttackMgr* pGAM) { pgGAM = pGAM; }
	static GeneralAttackMgr* GetGeneralAttackMgr() { return pgGAM; }

	virtual ~IAssailable() {}
	virtual void GetGPose(A3DMATRIX4& mtPose) const = 0;
	virtual IObjBase* GetObject() = 0;
	virtual ApxActorBase* GetActorBase() = 0;

	// return true if attacker(this object) hit target success or return false
	virtual bool OnAttackingCheckHit(const ApxAttackActionInfo& apxAttActInfo) { return true; }
	// notify the attacker(this object) while the target hit event returned 
	virtual void OnAttackingHitEventPostNotify(const ApxDamageInfo& apxDamageInfo, IAssailable* pTarget, bool bTargetIsEnable) {}

	// return true if the target(this object) can be attacked or return false
	virtual bool OnPreAttackedQuery() { return true; }
	// notify the target(this object) to set the parameters of attack struct.
	// pAttacker is not null if target is hit from PhysX Contact Report.
	virtual void OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker = 0) {}
	// return true if the target(this object) can be attacked again(not die) or return false
	virtual bool OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo) { return true; }

private:
	static GeneralAttackMgr* pgGAM;
};

class IPhysXObjBase : public IObjBase, public IAssailable
{
public:
	typedef IPropPhysXObjBase::DrivenMode	DrivenMode;

	IPhysXObjBase();
	IPropPhysXObjBase* GetProperties() const { return m_pPropPhysX; }
	bool LoadModel(IEngineUtility& eu, const char* szFile, APhysXScene* pPhysXScene = 0, const bool bEnableDrawEditInfo = true);

	bool InstancePhysXObj(APhysXScene& aPhysXScene);
	void ReleasePhysXObj();
	void ControlClothes(const bool IsClose = true);
	bool SyncDataPhysXToGraphic();

	virtual bool SaveSerialize(NxStream& stream) const;
	virtual bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion, bool bIsKeepIdx = false);
	virtual bool CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene);
	virtual IAssailable* GetAssailableInterface() { return this; }
 	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const = 0;

	/* not supported if pure PhysX driven mode using in runtime */
	virtual bool GetOBB(NxBox& outObjOBB) const;
	virtual bool GetObjAABB(NxBounds3& outObjAABB) const;
	virtual void GetGPose(A3DMATRIX4& mtPose) const { ApxObjBase::GetGPose(mtPose); }
	virtual IPhysXObjBase* GetObject() { return this; }
	virtual ApxActorBase* GetActorBase() { return 0; }

	friend class CCarryable;

protected:
	virtual void SetProperties(IPropPhysXObjBase& prop);
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene);
	virtual void OnLeaveRuntime();

	void ResetRuntimePose();
	bool GetModelAABB(A3DAABB& outAABB) const;

private:
	bool CheckDefPropsUpdateOnlyMySelf(IEngineUtility& eu, APhysXScene& scene);
	bool DoLoadModelInternal(IEngineUtility& eu, const char* szFile, APhysXScene* pPhysXScene);
	void ApplyPose(bool bApplyRuntimePose);

private:
	friend class IPropPhysXObjBase;

	virtual void OnDoSleepTask();
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime);
	virtual void WakeUpPost(APhysXScene* pPSToRuntime);

	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const = 0;
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile) = 0; 
	virtual void OnReleaseModel() = 0;

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene) = 0;
	virtual void OnReleasePhysXObj() = 0;
	virtual void OnControlClothes(const bool IsClose) {}

	virtual void OnTick(float dt);
	virtual bool OnTickMove(float deltaTimeSec) = 0;
	virtual bool OnTickAnimation(const unsigned long deltaTime) = 0;
	virtual bool SyncDataPhysXToGraphicMyself() = 0;

	virtual void OnChangeDrivenMode(const DrivenMode& dmNew) {}
	virtual void OnSleep() {}
	virtual bool OnWakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime = 0) { return false; }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const = 0;

private:
	// forbidden behavior
	IPhysXObjBase(const IPhysXObjBase& rhs);
	IPhysXObjBase& operator= (const IPhysXObjBase& rhs);

private:
	IPropPhysXObjBase* m_pPropPhysX;
};

#endif
