/*
 * FILE: Trigger.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

Trigger::Trigger()
{
	m_BeforLeaveRuntime = false;
	m_pRuntimeScene = 0;
	m_nbUEs = 0;
}

void Trigger::Register(IUserEvent* pUserEvent)
{
	if (0 == pUserEvent)
		return;
	m_UserEvents.UniquelyAdd(pUserEvent);
	m_nbUEs = m_UserEvents.GetSize();
}

void Trigger::UnRegister(IUserEvent* pUserEvent)
{
	int idx = m_UserEvents.Find(pUserEvent);
	if(0 > idx)
		return;
	m_UserEvents.RemoveAtQuickly(idx);
	m_nbUEs = m_UserEvents.GetSize();
}

void Trigger::EnterRuntime(APhysXScene& aPhysXScene)
{
	if (0 != m_pRuntimeScene)
	{
		assert(!"Shouldn't be here! Debug it!");
		return;
	}
	m_pRuntimeScene = &aPhysXScene;
	if (0 >= m_nbUEs)
		return;

	CRegion* pR = 0;
	APtrArray<IObjBase*> Regions;
	int nCount = ObjManager::GetInstance()->GetLiveObjects(Regions, ObjManager::OBJ_TYPEID_REGION);
	for (int i = 0; i < nCount; ++i)
	{
		pR = dynamic_cast<CRegion*>(Regions[i]);
		if (pR->GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_RTBEGIN))
		{
			for (int j = 0; j < m_nbUEs; ++j)
				m_UserEvents[j]->RuntimeBegin(*pR);
		}
	}
}

void Trigger::LeaveRuntime()
{
	if (0 >= m_nbUEs)
	{
		m_pRuntimeScene = 0;
		return;
	}

	m_BeforLeaveRuntime = true;
	RegionEvents();
	m_BeforLeaveRuntime = false;
	m_pRuntimeScene = 0;

	CRegion* pR = 0;
	APtrArray<IObjBase*> Regions;
	int nCount = ObjManager::GetInstance()->GetLiveObjects(Regions, ObjManager::OBJ_TYPEID_REGION);
	for (int i = 0; i < nCount; ++i)
	{
		pR = dynamic_cast<CRegion*>(Regions[i]);
		if (pR->GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_RTEND))
		{
			for (int j = 0; j < m_nbUEs; ++j)
				m_UserEvents[j]->RuntimeEnd(*pR);
		}
	}
}

void Trigger::RegionEvents()
{
	if (0 == m_pRuntimeScene)
		return;

	CRegion* pR = 0;
	APtrArray<IObjBase*> Regions;
	int nCount = ObjManager::GetInstance()->GetLiveObjects(Regions, ObjManager::OBJ_TYPEID_REGION);
	for (int i = 0; i < nCount; ++i)
	{
		pR = dynamic_cast<CRegion*>(Regions[i]);
		CheckRegionObjects(*pR);
	}
}

void Trigger::CheckRegionObjects(CRegion& region)
{
	assert(0 !=	m_pRuntimeScene);
	assert(0 == m_EntryObjs.GetSize());
	assert(0 == m_HoldObjs.GetSize());
	assert(0 == m_LeaveObjs.GetSize());
	if (!m_BeforLeaveRuntime)
		GenerateCandidate(region);

	EventsMaker(region);
}

void Trigger::EventsMaker(CRegion& rgn)
{
	rgn.UpdateHoldOnObjsLeave(m_HoldObjs, m_LeaveObjs);
	m_HoldObjs.RemoveAll(false);
	int nLeave = m_LeaveObjs.GetSize();
	if (0 < nLeave)
	{
		if (rgn.GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_LEAVE))
		{
			for (int j = 0; j < m_nbUEs; ++j)
				m_UserEvents[j]->LeaveRegion(rgn, nLeave, m_LeaveObjs.GetData());
		}
		m_LeaveObjs.RemoveAll(false);
	}

	int nEnter = m_EntryObjs.GetSize();
	if (0 < nEnter)
	{
		rgn.UpdateHoldOnObjsEnter(m_EntryObjs);
		if (rgn.GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_ENTER))
		{
			for (int j = 0; j < m_nbUEs; ++j)
				m_UserEvents[j]->EnterRegion(rgn, nEnter, m_EntryObjs.GetData());
		}
		m_EntryObjs.RemoveAll(false);
	}
}

void Trigger::GenerateCandidate(const CRegion& region)
{
	assert(0 ==	m_bufNxActors.GetSize());
	ObjManager* pObjsMgr = ObjManager::GetInstance();
	int filterMask = region.GetFilterMask();

	IObjBase* pObj = 0;
	int nbActors = region.TestOverlap(*m_pRuntimeScene, m_bufNxActors);
	for (int i = 0; i < nbActors; ++i)
	{
		pObj = pObjsMgr->GetPhysXObject(*(m_bufNxActors[i]));
		if (0 != pObj)
		{
			ObjManager::ObjTypeID ot = pObj->GetProperties()->GetObjType();
			if (filterMask & ot)
				AddToEnterCandidate(region, pObj);
		}
	}
	m_bufNxActors.RemoveAll(false);

	APtrArray<IObjBase*> allObjs;
	int nAll = pObjsMgr->GetLiveObjects(allObjs, filterMask);
	for (int i = 0; i < nAll; ++i)
	{
		if (TestRegionAndNonPhysXObject(region, *(allObjs[i])))
			AddToEnterCandidate(region, allObjs[i]);
	}
}

bool Trigger::TestRegionAndNonPhysXObject(const CRegion& region, const IObjBase& obj)
{
	if (obj.GetProperties()->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY))
		return false;
	
	NxBox obb;
	if (obj.GetOBB(obb))
		return region.TestOverlap(obb);

	NxBounds3 nxAABB;
	if (obj.GetObjAABB(nxAABB))
		return region.TestOverlap(nxAABB);

	return false;
}

void Trigger::AddToEnterCandidate(const CRegion& region, IObjBase* pObj)
{
	assert(0 != pObj);
	int nSize = m_EntryObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (m_EntryObjs[i] == pObj)
			return;
	}
	nSize = m_HoldObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (m_HoldObjs[i] == pObj)
			return;
	}

	int HoldIdx = pObj->GetMgrData()->GetRegionHoldOnIdx(region);
	if (0 > HoldIdx)
		m_EntryObjs.Add(pObj);
	else
	{
		assert(HoldIdx < region.GetNbHoldOnObjects());
		m_HoldObjs.Add(pObj);
	}
}

void Trigger::OnNotifyDie(const IObjBase& obj)
{
	if (0 == m_pRuntimeScene)
		return;

	CRegion* pR = 0;
	APtrArray<IObjBase*> Regions;
	int nCount = ObjManager::GetInstance()->GetLiveObjects(Regions, ObjManager::OBJ_TYPEID_REGION);
	for (int i = 0; i < nCount; ++i)
	{
		pR = dynamic_cast<CRegion*>(Regions[i]);
		NotifyObjectDie(*pR, obj);
	}
}

void Trigger::NotifyObjectDie(CRegion& region, const IObjBase& obj)
{
	assert(0 != m_pRuntimeScene);
	assert(0 < m_UserEvents.GetSize());

	ObjMgrData* pMgrData = obj.GetMgrData();
	if (0 == pMgrData)
		return;
	int HoldIdx = pMgrData->GetRegionHoldOnIdx(region);
	if (0 > HoldIdx)
		return;

	IObjBase* pObj = region.GetHoldOnObject(HoldIdx);
	if (&obj != pObj)
	{
		assert(!"Shouldn't be here! Debug it!");
		return;
	}
	m_LeaveObjs.Add(pObj);
	region.UpdateHoldOnObjsLeave(m_LeaveObjs);
	m_LeaveObjs.RemoveAll(false);

	if (region.GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_LEAVE))
	{
		for (int j = 0; j < m_nbUEs; ++j)
			m_UserEvents[j]->LeaveRegion(region, 1, &pObj);
	}
}
