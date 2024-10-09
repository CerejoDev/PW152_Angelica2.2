/*
 * FILE: PhysXObjStatic.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJSTATIC_H_
#define _APHYSXLAB_PHYSXOBJSTATIC_H_

#include "PhysXObjBase.h"

class A3DLitModel;
class APhysXObject;

class CPhysXObjStatic : public IPhysXObjBase
{
public:	
	CPhysXObjStatic();
	virtual ~CPhysXObjStatic() { Release(); };

	virtual bool SyncDataPhysXToGraphic() { return true; }
	virtual NxActor* GetFirstNxActor() const;
	virtual int  GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetPhysXRigidBodyAABB(NxBounds3& outAABB) const;

	virtual void  UpdatePos(const A3DVECTOR3& vPos);
	virtual void UpdateDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

private:	
	virtual bool OnLoadModel(CRender& render, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj(const bool RunTimeIsEnd);
	
	virtual bool OnTickMove(float deltaTimeSec);
	virtual bool OnTickAnimation(const unsigned long deltaTime) { return true; }
	virtual void OnRender(A3DViewport& viewport, A3DWireCollector* pWC = 0, bool isDebug = false) const;

	virtual bool OnResetPose() { return true; }
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnSetScale(const float& scale);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return IPhysXObjBase::OnEnterRuntime(aPhysXScene); }
	virtual void OnLeaveRuntime() { IPhysXObjBase::OnLeaveRuntime(); }

private:
	// forbidden behavior
	CPhysXObjStatic(const CPhysXObjStatic& rhs);
	CPhysXObjStatic& operator= (const CPhysXObjStatic& rhs);

private:
	A3DLitModel*	m_pObjModel;
	APhysXObject*	m_pObjPhysX;
	AString			m_strFilePhysX;
};

#endif