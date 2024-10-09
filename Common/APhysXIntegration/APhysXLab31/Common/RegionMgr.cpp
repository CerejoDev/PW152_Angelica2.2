/*
 * FILE: RegionMgr.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"


RegionMgr::RegionOMD* RegionMgr::RegionOMD::Clone() const
{
	return new RegionOMD(*this);
}

RegionMgr::~RegionMgr()
{
	ReleaseRegionToEnd(0);
}

bool RegionMgr::SaveSerialize(NxStream& stream) const
{
	int a=0;
	return false;
}

bool RegionMgr::LoadDeserialize(NxStream& stream, bool& outIsLowVersion)
{
	int a=0;
	return false;
}

Region* RegionMgr::CreateRegionObject()
{
	RegionOMD rgOMD;
	int idx = m_Regions.GetSize();
	rgOMD.SetObjMgrIdx(idx);

	IObjBase* pObj = RawObjMgr::GetInstance()->CreateRawObject(RawObjMgr::OBJ_TYPEID_REGION, rgOMD);
	if (0 == pObj)
		return 0;

	Region* pRegion = static_cast<Region*>(pObj);
	m_Regions.Add(pRegion);
	return pRegion;
}

void RegionMgr::ReleaseRegionObject(Region* pObject)
{
	RegionOMD* pRgOMD = static_cast<RegionOMD*>(pObject->GetMgrData());
	if (0 == pRgOMD)
	{
		assert(!"Shouldn't be here! Debug it!");
	}
	else
	{
		int idx = pRgOMD->GetObjMgrIdx();
		assert(-1 != idx);
		assert(pObject == m_Regions[idx]);
		m_Regions.RemoveAtQuickly(idx);
		if (idx < m_Regions.GetSize())
		{
			RegionOMD* pSwapped = static_cast<RegionOMD*>(m_Regions[idx]->GetMgrData());
			assert(0 != pSwapped);
			pSwapped->SetObjMgrIdx(idx);
		}
	}
	pObject->ToDie();
	RawObjMgr::GetInstance()->ReleaseRawObject(pObject);
}

void RegionMgr::ReleaseRegionToEnd(const int idxFrom)
{
	if (0 > idxFrom)
	{
		assert(0 <= idxFrom);
		return;
	}

	const int nAllCount = SizeAllRegions();
	if (idxFrom >= nAllCount)
	{
		if (0 == nAllCount)
			return;

		assert(idxFrom < nAllCount);
		return;
	}

	for (int i = idxFrom; i < nAllCount; ++i)
	{
		m_Regions[i]->ToDie();
		RawObjMgr::GetInstance()->ReleaseRawObject(m_Regions[i]);
		m_Regions[i] = 0;
	}
	m_Regions.RemoveAt(idxFrom, nAllCount - idxFrom);
}

const Region* RegionMgr::GetRegion(int index) const
{
	assert(0 <= index);
	assert(index < SizeAllRegions());
	return m_Regions[index];
}

bool RegionMgr::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	const void* backup = hitInfo.UserData;

	int MaxCount = SizeAllRegions();
	for (int i = 0; i < MaxCount; ++i)
	{
		m_Regions[i]->RayTraceObj(ray, hitInfo);
	}

	if (backup == hitInfo.UserData)
		return false;
	return true;
}

