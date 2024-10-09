/*
 * FILE: Trigger.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_TRIGGER_H_
#define _APHYSXLAB_TRIGGER_H_

class Trigger : public IObjStateReport
{
public:
	Trigger();
	void Register(IUserEvent* pUserEvent);
	void UnRegister(IUserEvent* pUserEvent);

	void EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();
	void RegionEvents();

	virtual void OnNotifyDie(const IObjBase& obj);

private:
	void CheckRegionObjects(CRegion& region);
	void GenerateCandidate(const CRegion& region);
	bool TestRegionAndNonPhysXObject(const CRegion& region, const IObjBase& obj);
	void AddToEnterCandidate(const CRegion& region, IObjBase* pObj);
	void EventsMaker(CRegion& rgn);
	void NotifyObjectDie(CRegion& region, const IObjBase& obj);

private:
	bool		m_BeforLeaveRuntime;
	APhysXScene* m_pRuntimeScene;
	int						m_nbUEs;
	APtrArray<IUserEvent*>	m_UserEvents;

	APtrArray<NxActor*>  m_bufNxActors;
	APtrArray<IObjBase*> m_EntryObjs;
	APtrArray<IObjBase*> m_HoldObjs;
	APtrArray<IObjBase*> m_LeaveObjs;
};

#endif
