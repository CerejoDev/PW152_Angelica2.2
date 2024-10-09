/*
 * FILE: PhysXObjParticle.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/03/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJPARTICLE_H_
#define _APHYSXLAB_PHYSXOBJPARTICLE_H_

#include "PhysXObjBase.h"
class A3DGFXExMan;

class CPhysXObjParticle : public IPhysXObjBase
{
public:	
	CPhysXObjParticle(int UID = -1);
	virtual ~CPhysXObjParticle() { ToDie(false); };

	virtual bool SyncDataPhysXToGraphic() { return true; }
//	virtual NxActor* GetFirstNxActor() const;
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;
	virtual void RenderExtraData(const IRenderUtility& renderUtil) const;

	bool ControlState(bool EnablePlay);

private:
	void SetDefaultOBB();

	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	
	virtual bool OnTickMove(float deltaTimeSec) { return true; }
	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual void OnRender(A3DViewport& viewport, bool bDoVisCheck) const;

	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale) { return false; /* unimplement*/ }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return IPhysXObjBase::OnEnterRuntime(aPhysXScene); }
	virtual void OnLeaveRuntime() { IPhysXObjBase::OnLeaveRuntime(); }

private:
	// forbidden behavior
	CPhysXObjParticle(const CPhysXObjParticle& rhs);
	CPhysXObjParticle& operator= (const CPhysXObjParticle& rhs);

private:
	A3DGFXExMan*	m_pGfxExMan;
	A3DGFXEx*		m_pGfxEx;
	A3DOBB			m_EmitterOBB;
	CObjProperty	m_propParticle;
};

#endif