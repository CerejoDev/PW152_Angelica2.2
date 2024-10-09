/*
 * FILE: RawObjManager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 20011/05/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_RAWOBJMANAGER_H_
#define _APHYSXLAB_RAWOBJMANAGER_H_

class IObjBase;
class IObjStateReport;

class IObjMgrData
{
public:
	IObjMgrData() { SetRawMgrIdx(-1); SetRegionHoldOnIdx(-1); }
	virtual ~IObjMgrData() {};
	virtual IObjMgrData* Clone() const = 0;

	int GetRawMgrIdx() const { return m_idxRawMgr; }
	void SetRawMgrIdx(int idx) { m_idxRawMgr = idx; }

	int GetRegionHoldOnIdx() const { return m_idxHoldOn; }
	void SetRegionHoldOnIdx(int idx) { m_idxHoldOn = idx; }

private:
	int m_idxRawMgr;
	int m_idxHoldOn;
};

class RawObjMgr : public APhysXSerializerBase
{
public:
	class RawOMD : public IObjMgrData
	{
	public:
		virtual RawOMD* Clone() const;
	};

	enum ObjTypeID
	{
		// For compatible old archive...
		// Do NOT change the order, just add new items following the sequence 
		OBJ_TYPEID_MANAGER		= 0,
		OBJ_TYPEID_STATIC		= 1,
		OBJ_TYPEID_SKINMODEL	= 2,
		OBJ_TYPEID_ECMODEL		= 3,
		OBJ_TYPEID_PARTICLE		= 4,
		OBJ_TYPEID_FORCEFIELD	= 5,
		OBJ_TYPEID_REGION		= 6,
		OBJ_MAX_BOUND			= 7,
	};
	static const TCHAR* GetObjTypeIDText(const ObjTypeID id);
	static bool GetObjTypeID(const TCHAR* pStr, ObjTypeID& outTypeID);
	static bool IsPhysXObjType(const ObjTypeID& id);

	struct ObjTypeInfo
	{
		ACString strWCExt;    // the extend name with wildcard (e.g. "*.smd")
		ACString strFilter;
	};
	static const ObjTypeInfo& GetObjTypeInfo(const ObjTypeID id);

	static APhysXU32 GetVersion() { return 0xAA000001; }
	static RawObjMgr* GetInstance();

public:
	void ClearUniqueSequenceID();

	void SetArchiveStyle(bool bIsOld) { m_ArchiveOldStyle = bIsOld; }
	bool GetArchiveStyleIsOld() const { return m_ArchiveOldStyle; }
	bool SaveSerialize(NxStream& stream) const;
	bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion);

	IObjBase* CreateRawObject(const ObjTypeID objType, const IObjMgrData& omd);
	void ReleaseRawObject(IObjBase* pObject);

private:
	RawObjMgr();
	int GetUniqueSequenceID();

private:
	// forbidden behavior
	RawObjMgr(const RawObjMgr& rhs);
	RawObjMgr& operator= (const RawObjMgr& rhs);

private:
	bool m_ArchiveOldStyle;
	int m_SequenceID;

	APtrArray<IObjBase*> m_aObjects;
};

#endif
