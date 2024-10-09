/*
 * FILE: IObjBase.h
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
#ifndef _APHYSXLAB_IOBJBASE_H_
#define _APHYSXLAB_IOBJBASE_H_

#include "ApxObjBase.h"

class IPropObjBase;
class IRenderable;
class ExtraRenderable;
class CBrushesObj;
class ITask;
class ObjMgrData;
class IEngineUtility;
class Scene;

class IObjBase : public ApxObjBase
{
public:
	IObjBase();
	virtual ~IObjBase() {};

	void SetMgrData(ObjMgrData* omd) { m_pOMD = omd; }
	ObjMgrData* GetMgrData() const { return m_pOMD; } 

	IPropObjBase* GetProperties() const { return m_pProperties; }
	IPropObjBase* CreatePropSnap() const;
	// return > 0, objTest is parent (1 parent, 2 grandparent .....)
	// return = 0, independent or myself
	// return < 0, objTest is child (-1 child, -2 grandchild .....)
	int GetRelationshipCode(const IObjBase& objTest);
	IObjBase* GetChildByLocalName(const TCHAR* szName);

	// major stuff to render
	IRenderable* GetRenderable() const { return m_pRenderable; }
	// extra stuff to render, such as debug information
	ExtraRenderable* GetExtraRenderable() const { return &m_extraRenderable; }

	void Sleep(const bool IsAsync = false);
	bool WakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime = 0); 
	void ToDie(const bool IsAsync = false);
	bool Revive(IEngineUtility& eu);

	bool EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();

	virtual bool SaveSerialize(NxStream& stream) const;
	virtual bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion, bool bIsKeepIdx = false);
 
	virtual bool GetOBB(NxBox& outObjOBB) const;
	virtual bool GetObjAABB(NxBounds3& outObjAABB) const;

	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool getRootObj, int objTypeFilter) const;
	virtual CBrushesObj* GetBrushObj() const { return m_pBrushObj; }
	virtual bool RayTraceBrush(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool bTraceChildren = true) const;

	// Enable/Disable my function
	virtual void Enable(bool bEnable) {}
	virtual bool IsEnabled() const  { return true;}

	//return false if file not exist
	bool SetBrushFileName(const char* strFileName);
	AString GetBrushFileName() const { return m_strBrushFile; }

	bool SetFocusObj(IObjBase* pFocus);
	IObjBase* GetFocusObj() const { return m_pObjFocus; }

	void SetScene(Scene* pScene) { OnSetScene(pScene); m_pScene = pScene;}
	Scene* GetScene() const { return m_pScene; }

protected:
	void IterateChildren(const ITask& task) const;
	void DoSleepTask();
	void SetRenderable(IRenderable& renderable) { m_pRenderable = &renderable; }
	void SetOriginalAABB(const NxBounds3& aabb);
	bool GetOriginalAABB(NxBounds3& outAABB) const;
	void BuildExtraRenderData();

	virtual void SetProperties(IPropObjBase& prop) { m_pProperties = &prop; }
	virtual void ApplyPose(bool bApplyRuntimePose);

	virtual void OnTick(float dt);
	virtual void OnUpdateParent();
	virtual void OnUpdateGRot();
	virtual void OnUpdateGPos();
	virtual void OnUpdateGScale();
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor) = 0;
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime) = 0;

	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) = 0;
	virtual void OnLeaveRuntime() = 0;
	virtual bool OnResetPose() = 0;
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const = 0;
	virtual void OnSetPos(const A3DVECTOR3& vPos) = 0;
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const = 0;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const = 0;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) = 0;
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const = 0;
	virtual void OnSetPose(const A3DMATRIX4& matPose) = 0;
	virtual bool OnSetScale(const float& scale) = 0;

	virtual void OnFocusObjChanged(){}
	//position of focused object changed notification, update position according to focused object
	virtual void OnUpdatePosChanged(const NxVec3& deltaMove);

	virtual void OnSetScene(Scene* pScene) {};

private:
	bool WakeUpMyself(IEngineUtility& eu, APhysXScene* pPSToRuntime);
	void LeaveRuntimeMySelf();
	void ToDieMySelf();
	void RemoveObserver(IObjBase* pObserver);
	virtual void OnRelease();
	
	virtual void OnDoSleepTask() = 0;
	virtual void WakeUpPost(APhysXScene* pPSToRuntime) {}
	virtual bool OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;

	bool LoadBrushObj();
	
private:
	friend class IPropObjBase;
	friend class IPropPhysXObjBase;

	// forbidden behavior
	IObjBase(const IObjBase& rhs);
	IObjBase& operator= (const IObjBase& rhs);

private:
	ObjMgrData*	m_pOMD;
	IPropObjBase*	m_pProperties;
	IRenderable*	m_pRenderable;
	mutable ExtraRenderable m_extraRenderable;

	CBrushesObj* m_pBrushObj;//brush (for brush cc collision)
	AString m_strBrushFile;//convex brush file name

	// note: at present, the focus/observer objects are not support serialization
	IObjBase*				m_pObjFocus;	//object that this object focus on
	APtrArray<IObjBase*>	m_Observers;	//objects that focus on this

protected:
	Scene* m_pScene;		// keep a pointer to the scene which creates me.

	// dt of the current frame
	float m_dt;
};

#endif
