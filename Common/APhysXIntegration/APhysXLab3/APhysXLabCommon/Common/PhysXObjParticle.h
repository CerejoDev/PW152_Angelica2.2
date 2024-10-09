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
class A3DGFXEx;

class CPhysXObjParticle : public IPhysXObjBase
{
public:	
	CPhysXObjParticle(int UID = -1);
	virtual ~CPhysXObjParticle() { assert(false == m_propParticle.IsObjAliveState()); };

	bool ControlState(bool EnablePlay);
	virtual int GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;

private:
	void SetDefaultOBB();

	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	
	virtual bool OnTickMove(float deltaTimeSec) { return true; }
	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual bool SyncDataPhysXToGraphicMyself() { return true; }
	virtual CPhysXObjParticle* NewObjAndCloneData() const;

	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale) { return false; /* unimplement*/ }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

private:
	// forbidden behavior
	CPhysXObjParticle(const CPhysXObjParticle& rhs);
	CPhysXObjParticle& operator= (const CPhysXObjParticle& rhs);

private:
	A3DGFXExMan*	m_pGfxExMan;
	A3DGFXEx*		m_pGfxEx;
	A3DOBB			m_EmitterOBB;
	CObjProperty	m_propParticle;
	GFXRenderable	m_GFXRender;
};

#endif