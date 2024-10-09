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
	m_pRuntimeScene = 0;
	m_pUserEvent = 0;
	m_pRegions = 0;
	m_pObjsMgr = 0;
}

Trigger::~Trigger()
{
	int nCount = m_RegionHoldOn.GetSize();
	for (int i = 0; i < nCount; ++i)
		delete m_RegionHoldOn[i];
	m_RegionHoldOn.RemoveAll();
}

void Trigger::InitUserEvent(IUserEvent* pUserEvent)
{
	m_pUserEvent = pUserEvent;
}

void Trigger::InitManagers(RegionMgr* pRegions, CPhysXObjMgr* pObjsMgr)
{
	m_pRegions = pRegions;
	m_pObjsMgr = pObjsMgr;
}

bool Trigger::IsInited() const
{
	if (0 == m_pUserEvent)
		return false;
	if (0 == m_pRegions)
		return false;
	if (0 == m_pObjsMgr)
		return false;
	return true;
}

bool Trigger::IsTriggerType(const IObjBase& obj)
{
	RawObjMgr::ObjTypeID ot = obj.GetProperties()->GetObjType();
	if (RawObjMgr::OBJ_TYPEID_STATIC == ot)
		return true;
	if (RawObjMgr::OBJ_TYPEID_SKINMODEL == ot)
		return true;
	if (RawObjMgr::OBJ_TYPEID_ECMODEL == ot)
		return true;
	return false;
}

void Trigger::EnterRuntime(APhysXScene& aPhysXScene)
{
	if (!IsInited())
		return;
	m_pRuntimeScene = &aPhysXScene;

	const Region* pR = 0;
	int nCount = m_pRegions->SizeAllRegions();
	for (int i = 0; i < nCount; ++i)
	{
		pR = m_pRegions->GetRegion(i);
		if (!pR->IsActive())
			continue;
		if (pR->GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_RTBEGIN))
			m_pUserEvent->RuntimeBegin(*pR);
	}
}

void Trigger::LeaveRuntime()
{
	if (!IsInited())
		return;
	m_pRuntimeScene = 0;

	const Region* pR = 0;
	int nCount = m_pRegions->SizeAllRegions();
	for (int i = 0; i < nCount; ++i)
	{
		pR = m_pRegions->GetRegion(i);
		if (!pR->IsActive())
			continue;
		if (pR->GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_RTEND))
			m_pUserEvent->RuntimeEnd(*pR);
	}
}

void Trigger::RegionEvents()
{
	if (!IsInited())
		return;
	if (0 == m_pRuntimeScene)
		return;

	bool bEnableEnter = false;
	bool bEnableLeave = false;
	const Region* pR = 0;
	int nCount = m_pRegions->SizeAllRegions();
	for (int i = 0; i < nCount; ++i)
	{
		pR = m_pRegions->GetRegion(i);
		if (!pR->IsActive())
			continue;
		bEnableEnter = pR->GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_ENTER);
		bEnableLeave = pR->GetFlags().ReadFlag(OBF_ENABLE_EVENT_REGION_LEAVE);
		if (bEnableEnter || bEnableLeave)
			CheckRegionObjects(*pR);
	}
}

void Trigger::CheckRegionObjects(const Region& region)
{
	assert(true == IsInited());
	assert(0 !=	m_pRuntimeScene);
	GenerateCandidate(region);

	int idx = -1;
	int nCount = m_RegionHoldOn.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (&region == m_RegionHoldOn[i]->pRegion)
		{
			idx = i;
			break;
		}
	}

	if (0 > idx)
	{
		if (0 == m_bufCandidate.GetSize())
			return;

		InRegionInfo* pNewRI = new InRegionInfo;
		if (0 == pNewRI)
		{
			m_bufCandidate.RemoveAll(false);
			return;
		}

		pNewRI->pRegion = &region;
		idx = m_RegionHoldOn.GetSize();
		m_RegionHoldOn.Add(pNewRI);
	}

	InRegionInfo* pRI = m_RegionHoldOn[idx];
	if (0 == pRI->HoldOnObjs.size())
	{
		if (0 == m_bufCandidate.GetSize())
		{
			delete pRI;
			m_RegionHoldOn.RemoveAt(idx);
			return;
		}
	}
	
	int nHoldOn = EventsMaker(*pRI);
	if (0 == nHoldOn)
	{
		delete pRI;
		m_RegionHoldOn.RemoveAt(idx);
	}
	m_bufCandidate.RemoveAll(false);
}

int Trigger::EventsMaker(InRegionInfo& ri)
{
	ObjInfo* pObjInfo = 0;
	size_t nSize = ri.HoldOnObjs.size();
	for (int i = 0; i < nSize; ++i)
	{
		pObjInfo = &(ri.HoldOnObjs[i]);
		pObjInfo->bIsLeaving = true;
	}

	int HoldIdx = -1;
	IObjBase* pObj = 0;
	assert(0 == m_EntryObjs.GetSize());

	int nCandi = m_bufCandidate.GetSize();
	for (int j = 0; j < nCandi; ++j)
	{
		pObj = m_bufCandidate[j];
		HoldIdx = pObj->GetMgrData()->GetRegionHoldOnIdx();
		if (0 > HoldIdx)
			m_EntryObjs.Add(pObj);
		else
		{
			assert(HoldIdx < nSize);
			ri.HoldOnObjs[HoldIdx].bIsLeaving = false;
		}
	}

	int k = 0;
	assert(0 == m_LeaveObjs.GetSize());
	while(true)
	{
		size_t nCount = ri.HoldOnObjs.size();
		if (k >= nCount)
			break;

		if (!ri.HoldOnObjs[k].bIsLeaving)
		{
			++k;
			continue;
		}

		pObj = ri.HoldOnObjs[k].pObj;
		pObj->GetMgrData()->SetRegionHoldOnIdx(-1);
		m_LeaveObjs.Add(pObj);
		if (k < (nCount - 1))
		{
			ri.HoldOnObjs[k].pObj = ri.HoldOnObjs[nCount - 1].pObj;
			ri.HoldOnObjs[k].bIsLeaving = ri.HoldOnObjs[nCount - 1].bIsLeaving;
			ri.HoldOnObjs[k].pObj->GetMgrData()->SetRegionHoldOnIdx(k);
		}
		ri.HoldOnObjs.pop_back();
	}

	int nLeave = m_LeaveObjs.GetSize();
	if (0 < nLeave)
	{
		m_pUserEvent->LeaveRegion(*(ri.pRegion), nLeave, m_LeaveObjs[0]);
		m_LeaveObjs.RemoveAll(false);
	}
	int nEnter = m_EntryObjs.GetSize();
	if (0 < nEnter)
	{
		m_pUserEvent->EnterRegion(*(ri.pRegion), nEnter, m_EntryObjs[0]);
		for (int n = 0; n < nEnter; ++n)
		{
			ObjInfo newEntry;
			newEntry.pObj = pObj;
			newEntry.bIsLeaving = false;
			int idx = int(ri.HoldOnObjs.size());
			pObj->GetMgrData()->SetRegionHoldOnIdx(idx);
			ri.HoldOnObjs.push_back(newEntry);
		}
		m_EntryObjs.RemoveAll(false);
	}

	return int(ri.HoldOnObjs.size());
}

void Trigger::GenerateCandidate(const Region& region)
{
	assert(0 ==	m_bufNxActors.GetSize());
	assert(0 ==	m_bufCandidate.GetSize());

	IObjBase* pObj = 0;
	int nbActors = region.TestOverlap(*m_pRuntimeScene, m_bufNxActors);
	for (int i = 0; i < nbActors; ++i)
	{
		pObj = m_pObjsMgr->GetObject(*(m_bufNxActors[i]));
		if (IsTriggerType(*pObj))
			AddToEnterCandidate(pObj);
	}
	m_bufNxActors.RemoveAll(false);

	int nAll = m_pObjsMgr->SizeAllLiveObjs();
	for (int i = 0; i < nAll; ++i)
	{
		pObj = m_pObjsMgr->GetObject(i);
		if (IsTriggerType(*pObj))
		{
			if (TestRegionAndNonPhysXObject(region, *pObj))
				AddToEnterCandidate(pObj);
		}
	}
}

bool Trigger::TestRegionAndNonPhysXObject(const Region& region, const IObjBase& obj)
{
	if (obj.GetProperties()->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY))
		return false;
	
	A3DOBB obb;
	if (obj.GetOBB(obb))
		return region.TestOverlap(obb);

	A3DAABB aabb;
	if (obj.GetAABB(aabb))
		return region.TestOverlap(aabb);

	return false;
}

void Trigger::AddToEnterCandidate(IObjBase* pObj)
{
	assert(0 != pObj);
	int nSize = m_bufCandidate.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (m_bufCandidate[i] == pObj)
			return;
	}
	m_bufCandidate.Add(pObj);
}
