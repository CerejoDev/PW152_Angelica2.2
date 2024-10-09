/*
 * FILE: ObjDummy.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/07/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_OBJDUMMY_H_
#define _APHYSXLAB_OBJDUMMY_H_
 
#include "IObjBase.h"

class CDummy : public IObjBase, public IPropObjBase
{
public:	
	static APhysXU32 GetVersion() { return 0xAA000001; }

	CDummy(int UID = -1);
	virtual ~CDummy() {	if (0 <= GetUID()) { assert(false == IsObjAliveState()); } }
	CDummy& operator= (const CDummy& rhs);

	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid();
	virtual bool OnPropertyChanged(PropItem& prop);
	virtual void OnUpdatePropsGrid();

protected:
	CDummy(const CDummy& rhs) : IPropObjBase(rhs) {}
	virtual CDummy* CreateMyselfInstance(bool snapValues) const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePostNotify();
	virtual void OnTick(float dt);
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime) { return IObjBase::OnDoWakeUpTask(eu, pPSToRuntime); }

	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return true; }
	virtual void OnLeaveRuntime() {}
	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos) {}
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) {}
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose) {}
	virtual bool OnSetScale(const float& scale) { return false; }

private:
	virtual CDummy* NewObjAndCloneData() const;

	virtual void OnDoSleepTask() {}
	virtual bool OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

private:
	NxBounds3 m_aabb;
};

#endif