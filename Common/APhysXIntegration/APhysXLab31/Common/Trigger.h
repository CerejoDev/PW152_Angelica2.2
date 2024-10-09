/*
 * FILE: Trigger.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 20011/05/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_TRIGGER_H_
#define _APHYSXLAB_TRIGGER_H_

class Trigger
{
public:
	Trigger();
	~Trigger();
	void InitUserEvent(IUserEvent* pUserEvent);
	void InitManagers(RegionMgr* pRegions, CPhysXObjMgr* pObjsMgr);

	void EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();
	void RegionEvents();

private:
	struct ObjInfo
	{
		IObjBase* pObj;
		bool bIsLeaving;
	};

	struct InRegionInfo
	{
		const Region* pRegion;
		std::vector<ObjInfo> HoldOnObjs;
	};

private:
	bool IsInited() const;
	bool IsTriggerType(const IObjBase& obj);
	void CheckRegionObjects(const Region& region);
	void GenerateCandidate(const Region& region);
	bool TestRegionAndNonPhysXObject(const Region& region, const IObjBase& obj);
	void AddToEnterCandidate(IObjBase* pObj);
	int EventsMaker(InRegionInfo& ri);

private:
	APhysXScene*	m_pRuntimeScene;
	IUserEvent*		m_pUserEvent;
	RegionMgr*		m_pRegions;
	CPhysXObjMgr*	m_pObjsMgr;

	APtrArray<NxActor*> m_bufNxActors;
	APtrArray<IObjBase*> m_EntryObjs;
	APtrArray<IObjBase*> m_LeaveObjs;
	APtrArray<IObjBase*> m_bufCandidate;
	APtrArray<InRegionInfo*> m_RegionHoldOn;
};


#endif
