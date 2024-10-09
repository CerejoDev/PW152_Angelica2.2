/*
 * FILE: RegionMgr.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 20011/05/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_REGIONMGR_H_
#define _APHYSXLAB_REGIONMGR_H_

class RegionMgr
{
public:
	RegionMgr() {}
	~RegionMgr();

	bool SaveSerialize(NxStream& stream) const;
	bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion);

	Region* CreateRegionObject();
	void ReleaseRegionObject(Region* pObject);
	void ReleaseRegionToEnd(const int idxFrom); // delete the range [idxFrom, end)

	const Region* GetRegion(int index) const;
	int SizeAllRegions() const { return m_Regions.GetSize(); }

	bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const; 

private:
	// forbidden behavior
	RegionMgr(const RegionMgr& rhs);
	RegionMgr& operator= (const RegionMgr& rhs);

	class RegionOMD : public IObjMgrData
	{
	public:
		RegionOMD() { SetObjMgrIdx(-1); }

		int GetObjMgrIdx() const { return m_idxObjs; }
		void SetObjMgrIdx(int idx) { m_idxObjs = idx; }

		virtual RegionOMD* Clone() const;

	private:
		int m_idxObjs;
	};

private:
	APtrArray<Region*> m_Regions;
};

#endif
