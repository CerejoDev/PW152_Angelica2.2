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
#include "PropPhysXObjImp.h"
#include <map>

class BackgroundTask;
class MMPolicy;

class CPhysXObjMgr : public IPhysXObjBase, public APhysXUserCreationDeletionReport
{
public:
	typedef RawObjMgr::ObjTypeID		ObjTypeID;

	CPhysXObjMgr(int UID = -1);
	~CPhysXObjMgr();

	IPhysXObjBase* CreateLabPhysXObject(const ObjTypeID objType, bool IsShotStuff = false);
	void ReleaseLabPhysXObject(IPhysXObjBase* pObject);

	void ReleaseAllShotObjects();
	void ReleaseAllRuntimeObjects();
	void ReleaseObjectToEnd(const int idxFrom); // delete the range [idxFrom, end)

//	void UpdateNeighborObjects(const A3DVECTOR3& vCenter, const float radius, CRender& render,
//							   APhysXScene& aPhysXScene, const bool bForceAll = false,
//							   BackgroundTask* pBKTask = 0);

	virtual bool SaveSerialize(NxStream& stream) const;
	virtual bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion);
	virtual bool CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene);

	virtual bool SyncDataPhysXToGraphic();

	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
		{ return RayTraceObj(ray, hitInfo, RawObjMgr::OBJ_TYPEID_MANAGER); }

	// It means no filter if the value is OBJ_TYPEID_MANAGER
	bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const ObjTypeID filter) const; 

//	virtual NxActor* GetFirstNxActor() const { return 0; /* unimplement*/ }
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const { return false; /* unimplement*/ }

	int SizeAllLiveObjs() const;
	int SizeModelLiveObjs() const;
	int SizeModelObjs() const { return m_aObjects.GetSize(); }
	int SizeAllObjs() const { return m_aObjects.GetSize() + m_objShotStuffs.GetSize(); }
	IPhysXObjBase* GetObject(const int index) const;
	IPhysXObjBase* GetObject(NxActor& actor);

//	void AddCMM(MMPolicy& mmp);
//	void ControlWeather(const bool Enable);

private:
	void CPhysXObjMgr::OnObjectDeletionNotify(APhysXObject* pObject);
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile)
	{
		assert(!"Oops! This function shouldn't be called! Debug it!");
		return false;
	}
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
//	virtual void OnControlClothes(const bool IsClose);

	virtual bool OnTickMove(float deltaTimeSec);
	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual void OnRender(A3DViewport& viewport, bool bDoVisCheck) const {}

	virtual bool OnResetPose();
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const { return false; }
	virtual void OnSetPos(const A3DVECTOR3& vPos); 
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const { return false; }
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const { return false; }
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose) const { return false; }
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale);

	virtual void OnChangeDrivenMode(const DrivenMode& dmNew);
	virtual bool OnWakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime = 0);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const { return false; /* unimplement*/ }

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene);
	virtual void OnLeaveRuntime();

private:
	void ReMapNxActor(NxActor* pTarget = 0);

private:
	// forbidden behavior
	CPhysXObjMgr(const CPhysXObjMgr& rhs);
	CPhysXObjMgr& operator= (const CPhysXObjMgr& rhs);

	class LabPhysXOMD : public IObjMgrData
	{
	public:
		LabPhysXOMD() { SetObjMgrIdx(-1); SetShotMgrIdx(-1); }

		int GetObjMgrIdx() const { return m_idxObjs; }
		void SetObjMgrIdx(int idx) { m_idxObjs = idx; }

		int GetShotMgrIdx() const { return m_idxShotStuffs; }
		void SetShotMgrIdx(int idx) { m_idxShotStuffs = idx; }

		virtual LabPhysXOMD* Clone() const;

	private:
		int m_idxObjs;
		int m_idxShotStuffs;
	};

private:
	std::map<NxActor*, IPhysXObjBase*> m_ActorToModel;
	APtrArray<IPhysXObjBase*> m_aObjects;
	APtrArray<IPhysXObjBase*> m_objShotStuffs;
	CMgrProperty m_propMgr;
};

#endif