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
	virtual ~CPhysXObjStatic() { assert(false == m_propStatic.IsObjAliveState()); };

	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;

private:
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	
	virtual bool OnTickMove(float deltaTimeSec) { return true; }
	virtual bool OnTickAnimation(const unsigned long deltaTime) { return true; }
	virtual bool SyncDataPhysXToGraphicMyself() { return true; }
	virtual CPhysXObjStatic* NewObjAndCloneData() const;

	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos); 
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;

private:
	// forbidden behavior
	CPhysXObjStatic(const CPhysXObjStatic& rhs);
	CPhysXObjStatic& operator= (const CPhysXObjStatic& rhs);

private:
	A3DLitModel*	m_pObjModel;
	APhysXObject*	m_pObjPhysX;
	AString			m_strFilePhysX;
	CObjProperty	m_propStatic;
	LitModelRenderable	m_LMRender;
};

#endif