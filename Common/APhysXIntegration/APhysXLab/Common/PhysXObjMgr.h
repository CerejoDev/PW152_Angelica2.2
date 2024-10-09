/*
* FILE: PhysXObjMgr.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/05/18
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJMGR_H_
#define _APHYSXLAB_PHYSXOBJMGR_H_

#include "PhysXObjBase.h"
#include <map>

class BackgroundTask;
class MMPolicy;

class CPhysXObjMgr : public IPhysXObjBase, public APhysXUserCreationDeletionReport
{
public:
	enum ObjTypeID
	{
		// For compatible old archive...
		// Do NOT change the order, just add new items following the sequence 
		OBJ_TYPEID_MANAGER   = 0,
		OBJ_TYPEID_STATIC    = 1,
		OBJ_TYPEID_SKINMODEL = 2,
		OBJ_TYPEID_ECMODEL	 = 3,
		OBJ_TYPEID_PARTICLE  = 4,
		OBJ_TYPEID_FORCEFIELD = 5,
		OBJ_TYPEID_CHB        = 6,
		OBJ_MAX_BOUND		 = 7,
	};

	struct ObjInfo
	{
		ACString strWCExt;    // the extend name with wildcard (e.g. "*.smd")
		ACString strFilter;
	};

	static const ObjInfo& GetObjInfo(const ObjTypeID id);

public:
	CPhysXObjMgr() : IPhysXObjBase(OBJ_TYPEID_MANAGER) {}
	virtual ~CPhysXObjMgr() { Release(); }

	IPhysXObjBase* CreateShotObject(const ObjTypeID objType);
	void ReleaseAllShotObjects();
	void ReleaseAllRuntimeObjects();

	IPhysXObjBase* CreateObject(const ObjTypeID objType, const bool isNPC = true);
	void ReleaseObject(IPhysXObjBase* pObject);
	void ReleaseObjectToEnd(const int idxFrom); // delete the range [idxFrom, end)

	void SetDrawModelAABB(const bool bEnable);
	void UpdateNeighborObjects(const A3DVECTOR3& vCenter, const float radius, CRender& render,
							   APhysXScene& aPhysXScene, const bool bForceAll = false,
							   BackgroundTask* pBKTask = 0);

	virtual bool LoadDeserialize(CRender& render, NxStream& stream, const unsigned int nVersion, APhysXScene& aPhysXScene);
	virtual bool SaveSerialize(NxStream& stream) const;

	virtual bool SyncDataPhysXToGraphic();

	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
		{ return RayTraceObj(ray, hitInfo, OBJ_TYPEID_MANAGER); }

	// It means no filter if the value is OBJ_TYPEID_MANAGER
	bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const ObjTypeID filter) const; 

	virtual NxActor* GetFirstNxActor() const { return 0; /* unimplement*/ }
	virtual int  GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetPhysXRigidBodyAABB(NxBounds3& outAABB) const { return false; /* unimplement*/ }

	int SizeModels() const { return m_aObjects.GetSize(); }
	int SizeAll() const { return m_aObjects.GetSize() + m_objShotStuffs.GetSize(); }
	IPhysXObjBase* GetObject(const int index) const;
	IPhysXObjBase* GetObject(NxActor& actor);

	void AddCMM(MMPolicy& mmp);
	void ControlWeather(const bool Enable);

private:
	void CPhysXObjMgr::OnObjectDeletionNotify(APhysXObject* pObject);
	virtual bool OnLoadModel(CRender& render, const char* szFile)
	{
		assert(!"Oops! This function shouldn't be called! Debug it!");
		return false;
	}
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj(const bool RunTimeIsEnd);
	virtual void OnControlClothes(const bool IsClose);

	virtual bool OnTickMove(float deltaTimeSec);
	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual void OnRender(A3DViewport& viewport, A3DWireCollector* pWC = 0, bool isDebug = false) const;
	virtual void OnRenderShadow(A3DViewport& viewport) const;

	virtual bool OnResetPose();
	virtual void OnSetPos(const A3DVECTOR3& vPos); 
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnSetScale(const float& scale);

	virtual void OnSetRefFrameEditMode(const EditType et);
	virtual void OnChangeDrivenMode(const DrivenMode dmNew);
	virtual bool OnWakeUp(CRender& render, APhysXScene* pPhysXScene = 0);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const { return false; /* unimplement*/ }

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene);
	virtual void OnLeaveRuntime();

private:
	IPhysXObjBase* CreateObject(const ObjTypeID objType, const bool isNPC, const bool IsShotStuff);
	void ReMapNxActor(NxActor* pTarget = 0);

private:
	// forbidden behavior
	CPhysXObjMgr(const CPhysXObjMgr& rhs);
	CPhysXObjMgr& operator= (const CPhysXObjMgr& rhs);

private:
	std::map<NxActor*, IPhysXObjBase*> m_ActorToModel;
	APtrArray<IPhysXObjBase*> m_aObjects;
	APtrArray<IPhysXObjBase*> m_objShotStuffs;
	
	friend class CBrushesObjMgr;
};

#endif