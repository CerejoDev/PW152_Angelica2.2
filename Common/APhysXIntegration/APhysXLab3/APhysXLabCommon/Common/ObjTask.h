/*
* FILE: ObjTask.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/06/15
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_OBJTASK_H_
#define _APHYSXLAB_OBJTASK_H_

class IObjBase;
class IEngineUtility;

template<class T>
class IResult
{
public:
	IResult(const T& expectValue) : m_StanderdValue(expectValue) {}

	void CheckResultValue(IObjBase& obj, const T& val)
	{
		if (val == m_StanderdValue)
			m_objEqualSV.Add(&obj);
	}
	int GetNbObjectsEqualExpect() const { return m_objEqualSV.GetSize(); }

	const T& GetExpectResult() const { return m_StanderdValue; }
	void SetExpectResult(const T& expectValue) { m_StanderdValue = expectValue; }

private:
	APtrArray<IObjBase*> m_objEqualSV;
	T m_StanderdValue;
};

class ITask
{
public:
	ITask() { m_TypeRequirement = 0xffffffff; }
	ITask(int t) { m_TypeRequirement = t; }
	bool IsRightType(IObjBase* pObj) const;
	virtual void Execute(IObjBase* pObj) const = 0;

private:
	int m_TypeRequirement;
};

class TaskSleepToDie : public ITask
{
public:
	TaskSleepToDie(bool IsAsync, bool IsSleep) : m_IsAsync(IsAsync), m_IsSleep(IsSleep) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	bool m_IsAsync;
	bool m_IsSleep;
};

class TaskWakeUp : public ITask
{
public:
	TaskWakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime) : m_eu(eu), m_pPSToR(pPSToRuntime) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	IEngineUtility& m_eu;
	APhysXScene* m_pPSToR;
};

class TaskRevive : public ITask
{
public:
	TaskRevive(IEngineUtility& eu) : m_eu(eu) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	IEngineUtility& m_eu;
};

class TaskEnterLeaveRuntime : public ITask
{
public:
	TaskEnterLeaveRuntime() : m_IsEn(false), m_pScene(0) {}
	TaskEnterLeaveRuntime(APhysXScene& Scene) : m_IsEn(true), m_pScene(&Scene) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	bool m_IsEn;
	APhysXScene* m_pScene;
};

class TaskRayTrace : public ITask
{
public:
	TaskRayTrace(const PhysRay& Ray, PhysRayTraceHit& hitInfo, bool getRootObj, int objTypeFilter)
	{
		m_pRay = &Ray;
		m_pHitInfo = &hitInfo;
		m_getRootObj = getRootObj;
		m_objTypeFilter = objTypeFilter;
	}
	virtual void Execute(IObjBase* pObj) const;

private:
	const PhysRay* m_pRay;
	PhysRayTraceHit* m_pHitInfo;
	bool m_getRootObj;
	int m_objTypeFilter;
};

class TaskCheckDefPropUpdate : public ITask
{
public:
	TaskCheckDefPropUpdate(IEngineUtility& eu, APhysXScene& scene, bool bExpectValue)
		: ITask(ObjManager::OBJ_TYPEID_PHYSX), m_eu(eu), m_scene(scene), m_Result(bExpectValue) {}
	virtual void Execute(IObjBase* pObj) const;
	const IResult<bool>& GetExpectResult() const { return m_Result; }

private:
	IEngineUtility& m_eu;
	APhysXScene& m_scene;
	mutable IResult<bool> m_Result;
};

class TaskPhysXObjOnOff : public ITask
{
public:
	TaskPhysXObjOnOff() : ITask(ObjManager::OBJ_TYPEID_PHYSX), m_IsOn(false), m_pScene(0) {}
	TaskPhysXObjOnOff(APhysXScene& Scene) : ITask(ObjManager::OBJ_TYPEID_PHYSX), m_IsOn(true), m_pScene(&Scene) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	bool m_IsOn;
	APhysXScene* m_pScene;
};

class TaskControlClothes : public ITask
{
public:
	TaskControlClothes(bool IsClose) : ITask(ObjManager::OBJ_TYPEID_PHYSX), m_IsClose(IsClose) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	bool m_IsClose;
};

class TaskSetDrivenMode : public ITask
{
public:
	typedef IPropPhysXObjBase::DrivenMode	DrivenMode;

	TaskSetDrivenMode(const DrivenMode& dm, bool IsOnPlay) : ITask(ObjManager::OBJ_TYPEID_PHYSX), m_dm(dm), m_IsOnPlay(IsOnPlay) {}
	virtual void Execute(IObjBase* pObj) const;

private:
	DrivenMode m_dm;
	bool m_IsOnPlay;
};

class TaskSyncData : public ITask
{
public:
	TaskSyncData() : ITask(ObjManager::OBJ_TYPEID_PHYSX) {}
	virtual void Execute(IObjBase* pObj) const;
};

#endif