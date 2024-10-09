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

/*class IActionPlayerBase;
class A3DSkinModel;
class APhysXSkeletonRBObject;
typedef unsigned int APhysXCollisionChannel;
class NxGroupsMask;
class NxActor;
class APhysXCharacterController;
class A3DModelPhysics;*/

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
	CPhysXObjDynamic();
	virtual ~CPhysXObjDynamic() = 0;

	bool ApplyPhysXBody(const bool bEnable, APhysXScene* pScene = 0);
//	bool UpdateNxActorsAttributes(const NxGroupsMask* pGroupMask, const bool ChangeToKinematic, NxActor**ppoutFirstActor);
//	bool PlayAttackAction(const IPhysXObjBase& objTarget);

	A3DVECTOR3 GetHeadTopPos(A3DMATRIX4* pOutMat = 0) const;
//	float GetHeight() const { return m_Height; }

/*	int  GetCCFlags() const;
	void SetCCFlags(const int flags);
	
	int  GetPMFlags(const bool forceRefresh = false) const;
	void SetPMFlags(const int flags);
	
	void SetPeopleFlags(const int flags, APhysXScene& aPhysXScene);

	CollisionChannel GetChannel(const bool forceRefresh = false) const;
	void SetChannel(const CollisionChannel channel);
*/
	bool  GetCollisionChannel(APhysXCollisionChannel& outChannel) const;
	float GetYOffsetForRevive() const;

	virtual CDynProperty* GetProperties() = 0;
	virtual const CDynProperty* GetProperties() const = 0;
	virtual A3DSkinModel* GetSkinModel() const = 0;
	virtual A3DModelPhysics* GetA3DModelPhysics() const = 0;
	virtual APhysXSkeletonRBObject* GetSkeletonRBObject() const = 0;
	virtual bool SetCollisionChannel(const CollisionChannel cnl) = 0;

//	virtual NxActor* GetFirstNxActor() const;
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;

private:
	// forbidden behavior
	CPhysXObjDynamic(const CPhysXObjDynamic& rhs);
	CPhysXObjDynamic& operator= (const CPhysXObjDynamic& rhs);

//	virtual void OnSetToEmpty();
/*	virtual void OnCopyBasicProperty(const IPhysXObjBase& obj);
	virtual bool OnLoadDeserialize(NxStream& stream, const unsigned int nVersion);
	virtual void OnSaveSerialize(NxStream& stream) const;
	virtual bool OnPlayAttackAction(const IPhysXObjBase& objTarget) { return false; };
*/
	virtual bool OnTickMove(float deltaTimeSec);

/*	void ApplyCCFlags(const int flags);
	void ApplyPMFlags(const int flags);
	void ApplyChannel(const CollisionChannel cnl);
*/
protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene);
	virtual void OnLeaveRuntime();

	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual void OnChangeDrivenMode(const DrivenMode& dmNew);

/*	void SetPlayEnable(const bool bEnable) { m_PlayEnable = bEnable; }
	*/
/*
private:
	bool m_PlayEnable;
	bool m_IsRestart;
	IActionPlayerBase* m_pAC;
	
	int m_FlagsCC; 
	int m_FlagsPM;  // flags for PhysX model
	CollisionChannel m_Channel;
	int m_FlagsCCRuntime;
	int m_FlagsPMRuntime;
	CollisionChannel m_ChannelRuntime;

	APhysXScene* pCCLiveIn;
	PeopleInfo FlagOfCCType;

	float m_Height;*/
};

#endif