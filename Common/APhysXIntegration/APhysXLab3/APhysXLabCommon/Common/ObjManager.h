/*
 * FILE: ObjManager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/06/01
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_OBJMANAGER_H_
#define _APHYSXLAB_OBJMANAGER_H_

//#define ENABLE_MULLITHREAD_TEST

class Scene;
class IObjBase;
class CECModel;
class IPhysXObjBase;
class IObjStateReport;
class CPhysXObjECModel;
class ITask;

class ObjMgrData
{
public:
	ObjMgrData() { m_RootIndex = -1; m_AllIndex = -1; }

	int GetRootIndex() const { return m_RootIndex; }
	void SetRootIndex(int idx) { m_RootIndex = idx; }

	int GetAllIndex() const { return m_AllIndex; }
	void SetAllIndex(int idx) { m_AllIndex = idx; }

	int GetRegionHoldOnIdx(const CRegion& rgn) const;
	void SetRegionHoldOnIdx(const CRegion& rgn, int idx);

private:
	struct RegionHoldOn
	{
		RegionHoldOn(){ m_pRgn = 0; m_idxHoldOn = -1; };
		RegionHoldOn(const CRegion& rgn, int idx) { m_pRgn = &rgn; m_idxHoldOn = idx; };
		const CRegion* m_pRgn;
		int m_idxHoldOn;
	};

private:
	int m_RootIndex;
	int m_AllIndex;
	std::vector<RegionHoldOn>  m_RgnHold;
};

class ObjManager : public APhysXSerializerBase, public ApxObjOperationReport
{
public:
	enum ObjTypeID
	{
		// For compatible old archive...
		// Do NOT change the order, just add new items following the sequence
		OBJ_TYPEID_EMPTY		= 0,
		OBJ_TYPEID_STATIC		= (1<<0),
		OBJ_TYPEID_SKINMODEL	= (1<<1),
		OBJ_TYPEID_ECMODEL		= (1<<2),
		OBJ_TYPEID_PARTICLE		= (1<<3),
		OBJ_TYPEID_FORCEFIELD	= (1<<4),
		OBJ_TYPEID_REGION		= (1<<5),
		OBJ_TYPEID_DUMMY		= (1<<6),
		OBJ_TYPEID_SMSOCKET		= (1<<7),
		OBJ_TYPEID_SOCKETSM		= (1<<8),
		OBJ_TYPEID_SPOTLIGHT	= (1<<9),
		OBJ_TYPEID_CAMERA		= (1<<10),

		OBJ_TYPEID_PHYSX		= OBJ_TYPEID_STATIC | OBJ_TYPEID_SKINMODEL | OBJ_TYPEID_ECMODEL | OBJ_TYPEID_PARTICLE | OBJ_TYPEID_FORCEFIELD,
		OBJ_TYPEID_ALL			= 0xffffffff,
	};
	static const TCHAR* GetObjTypeIDText(const ObjTypeID id);
	static bool GetObjTypeID(const TCHAR* pStr, ObjTypeID& outTypeID);
	static bool IsPhysXObjType(const ObjTypeID& id);
	static void IterateChildren(const IObjBase& theObj, const ITask& task);

	struct ObjTypeInfo
	{
		ACString strWCExt;    // the extend name with wildcard (e.g. "*.smd")
		ACString strFilter;
	};
	static const ObjTypeInfo& GetObjTypeInfo(const ObjTypeID id);

	static APhysXU32 GetVersion() { return 0xAA000002; }
	static ObjManager* GetInstance();

	static bool SaveObject(NxStream& stream, IObjBase* pObj);
	static IObjBase* LoadObject(Scene& scene, NxStream& stream, bool& outIsLowVersion, IObjBase* pParent, bool bIsOldArchiveStyle = false, bool bIsKeepIdx = false);

public:
	void ClearUniqueSequenceID();

	void SetArchiveStyle(bool bIsOld) { m_ArchiveOldStyle = bIsOld; }
	bool GetArchiveStyleIsOld() const { return m_ArchiveOldStyle; }
	bool SaveSerialize(NxStream& stream) const;
	bool LoadDeserialize(Scene& scene, NxStream& stream, bool& outIsLowVersion);
	bool CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene);

	IObjBase* CreateObject(const ObjTypeID objType, bool bIsRootObj, Scene& scene);
	void ReleaseObject(IObjBase* pObject);
	void ReleaseAllRootObjects();
	void ReleaseAllShotObjects();

	// toupdate: support smart find & filter
	IObjBase* FindObj(const char* objName);

	IPhysXObjBase* GetPhysXObject(NxActor& actor) const;
	CPhysXObjECModel* GetECModelObject(CECModel* pECM) const;
	int GetLiveObjects(APtrArray<IObjBase*>& outObjs, int objTypeFilter) const;
	int GetRootLiveObjects(APtrArray<IObjBase*>& outObjs, int objTypeFilter) const;

	bool EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();
	void Tick(float dt);

	void EnableAllPhysX(APhysXScene& aScene);
	void DisableAllPhysX();
	void ControlModelClothes(bool bEnable);
	void SetDrivenMode(int dm);
	bool SyncDataPhysXToGraphic();
	bool WakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime = 0);
	bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool getRootObj = true, int objTypeFilter = OBJ_TYPEID_ALL) const;

	void OnDeletionNotify(APhysXObject& object);
	virtual void OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild);
	virtual void OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild);

private:
	friend class IObjBase;
	typedef		APtrArray<IObjBase*>	Vessel;

	class Condition
	{
	public:
		Condition() { m_bIsRTOnly = false; m_bIsShotStuff = false; }
		void SetRuntimeCondition() { m_bIsRTOnly = true; m_bIsShotStuff = false; }
		void SetShotStuffCondition() { m_bIsRTOnly = false; m_bIsShotStuff = true; }
		bool CheckCondition(IObjBase* pObj) const;

	private:
		bool m_bIsRTOnly;
		bool m_bIsShotStuff;
	};

	ObjManager();
	~ObjManager();
	int GetUniqueSequenceID();

	IObjBase* CreateObject(const ObjTypeID objType);
	bool BuildManagerData(IObjBase& object, bool bIsRootObj);
	void EraseManagerData(IObjBase& object);
	bool AddRootManagerData(IObjBase& object, ObjMgrData& OMD);
	bool RemoveRootManagerData(IObjBase& object, ObjMgrData& OMD);

	void ReMapNxActor(NxActor* pTarget = 0) const;
	int  GetRootLiveObjs(int objTypeFilter, Vessel* pOut = 0) const;
	void ReleaseConditionObjs(const Condition& cond);
	void ExecuteTask(const ITask& task);

private:
	// forbidden behavior
	ObjManager(const ObjManager& rhs);
	ObjManager& operator= (const ObjManager& rhs);

private:
#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutex m_Locker;
#endif

	mutable bool m_RemapEnable;
	bool m_ArchiveOldStyle;
	int m_SequenceID;
	int m_CountChecker;

	Vessel m_RootObjs;
	mutable std::map<NxActor*, IPhysXObjBase*> m_ActorToModel;

	Vessel m_AllObjs;
};

#endif
