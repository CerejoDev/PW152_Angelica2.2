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
	CPhysXObjStatic(int UID = -1);
	virtual ~CPhysXObjStatic() { ToDie(false); };

	virtual bool SyncDataPhysXToGraphic() { return true; }
//	virtual NxActor* GetFirstNxActor() const;
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;

private:	
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	
	virtual bool OnTickMove(float deltaTimeSec) { return true; }
	virtual bool OnTickAnimation(const unsigned long deltaTime) { return true; }
	virtual void OnRender(A3DViewport& viewport, bool bDoVisCheck) const;

	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos); 
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
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
	CObjProperty	m_propStatic;
};

#endif