/*
 * FILE: DetectFilterMgr.h
 *
 * DESCRIPTION: 
 *		manager the temporary filter for PhysX raycast/sweep
 *
 * CREATED BY: Yang Liu, 2011/12/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_APHYSXDETECTFILTERMGR_H_
#define _APHYSXLAB_APHYSXDETECTFILTERMGR_H_

class DetectFilterMgr
{
public:
	DetectFilterMgr();
	~DetectFilterMgr();

	bool FilterBegin(APhysXScene& scene);
	void FilterEnd();

	NxU32 GetActiveGroup() const;
	void FilterOutGroup(APhysXCollisionGroup group)
	{
		assert(APX_COLLISION_CHANNEL_INVALID != m_TempCNL);
		m_ActiveGroup &= ~(1 << group);
	}

	const NxGroupsMask* GetGroupsMask() const;
	void FilterOutDynObject(CPhysXObjDynamic* pObj);

private:
	struct CNLInfo
	{
		CNLInfo() { pSRBO = 0; backupCNL = APX_COLLISION_CHANNEL_INVALID; pActor = 0; }
		CNLInfo(APhysXSkeletonRBObject* psrbo, APhysXCollisionChannel cnl) { pSRBO = psrbo; backupCNL = cnl; pActor = 0; }
		APhysXSkeletonRBObject* pSRBO;
		APhysXCollisionChannel backupCNL;
		NxActor* pActor;
		NxGroupsMask backupGM;
	};

private:
	APhysXScene* m_pAPhysXScene;
	APhysXCollisionChannelManager* m_pCCM;
	APhysXCollisionChannel m_TempCNL;

	NxU32 m_ActiveGroup;
	NxGroupsMask m_GroupMask;
	std::vector<CNLInfo> m_BackupInfo;
};

#endif
