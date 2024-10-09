/*
 * FILE: PhysXObjForceField.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/03/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJFORCEFIELD_H_
#define _APHYSXLAB_PHYSXOBJFORCEFIELD_H_

#include "PhysXObjBase.h"

class NxForceFieldKernelAura;

class CPhysXObjForceField : public IPhysXObjBase
{
public:
	CPhysXObjForceField(int FFType);
	virtual ~CPhysXObjForceField();
	
	void SetHostChannel(const APhysXCollisionChannel* pHostCNL = 0);
	bool GetFFVariety(NxForceFieldVariety& outFFV) const;
	bool ReloadModel();
	bool OnDeletionNotify(const APhysXObject& rObject);
	void SetState(bool toRest, bool IsRunning);
	void AttachTo(NxActor* pActor);

	virtual bool SyncDataPhysXToGraphic() { return true; }
	virtual NxActor* GetFirstNxActor() const;
	virtual int  GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetPhysXRigidBodyAABB(NxBounds3& outAABB) const { return false; /* unimplement*/ }

private:	
	virtual bool OnLoadModel(CRender& render, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj(const bool RunTimeIsEnd);
	
	virtual bool OnTickMove(float deltaTimeSec) { return true; }
	virtual bool OnTickAnimation(const unsigned long deltaTime) { return true; }
	virtual void OnRender(A3DViewport& viewport, A3DWireCollector* pWC = 0, bool isDebug = false) const;

	virtual bool OnResetPose() { return true; }
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnSetScale(const float& scale) { return false; }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const { return false; }
	virtual bool OnGetSelectedAABB(A3DAABB& outAABB) const;
	
	void DrawAllFFShapes(A3DWireCollector& wc, const APhysXShapeDescManager& sdm, int color) const;

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return IPhysXObjBase::OnEnterRuntime(aPhysXScene); }
	virtual void OnLeaveRuntime() { IPhysXObjBase::OnLeaveRuntime(); }

private:
	enum CustomKnlID
	{
		CKID_NONE = 0,
		CKID_AURA = 1,
		CKID_RIVE = 2,
	};

private:
	// forbidden behavior
	CPhysXObjForceField(const CPhysXObjForceField& rhs);
	CPhysXObjForceField& operator= (const CPhysXObjForceField& rhs);

private:
	APhysXObjectDescManager m_FFOD;
	APhysXForceFieldObject* m_pAFF;
	NxForceField* m_pFF;
	NxMat34 m_ffMat;

	APhysXCollisionChannel m_hostCNL;
	
	CustomKnlID m_CKID;
	NxForceFieldKernel* m_pCustomKnl;
};

#endif