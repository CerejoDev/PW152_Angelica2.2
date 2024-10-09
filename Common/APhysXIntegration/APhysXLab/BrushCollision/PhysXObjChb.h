/*
 * FILE: PhysXObjChb.h
 *
 * DESCRIPTION: load .chb file
 *
 * 
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJCHB_H_
#define _APHYSXLAB_PHYSXOBJCHB_H_

#include "PhysXObjBase.h"
#include "ConvexHullDataSet.h"
#include <vector.h>

using namespace CHBasedCD;

class APhysXObject;

class CPhysXObjChb : public IPhysXObjBase
{
public:	
	CPhysXObjChb();
	virtual ~CPhysXObjChb() { Release(); };

	virtual bool SyncDataPhysXToGraphic() { return true; }
	virtual NxActor* GetFirstNxActor() const;
	virtual int  GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool GetPhysXRigidBodyAABB(NxBounds3& outAABB) const;

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
	virtual bool OnSetScale(const float& scale);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return IPhysXObjBase::OnEnterRuntime(aPhysXScene); }
	virtual void OnLeaveRuntime() { IPhysXObjBase::OnLeaveRuntime(); }

private:
	// forbidden behavior
	CPhysXObjChb(const CPhysXObjChb& rhs);
	CPhysXObjChb& operator= (const CPhysXObjChb& rhs);

private:
	CHBasedCD::CConvexHullDataSet*	m_pObjModel;
	abase::vector<APhysXObject*>  		m_pAllObjPhysX;
	AString			m_strFilePhysX;
	A3DEngine* m_pEngine;
};

#endif