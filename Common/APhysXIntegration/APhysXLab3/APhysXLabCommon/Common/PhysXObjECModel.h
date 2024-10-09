/*
* FILE: PhysXObjECModel.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/07/08
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJECMODEL_H_
#define _APHYSXLAB_PHYSXOBJECMODEL_H_

#include "PhysXObjDynamic.h"

class CECModel;
class APhysXSkeletonRBObject;

class CPhysXObjECModel : public CPhysXObjDynamic
{
public:
	CPhysXObjECModel(int UID = -1);
	virtual ~CPhysXObjECModel() { assert(false == m_propECM.IsObjAliveState()); };
	
public:
	virtual CDynProperty* GetProperties() {	return  &m_propECM;	}
	virtual const CDynProperty* GetProperties() const { return  &m_propECM;	}
	virtual A3DSkinModel* GetSkinModel() const;
	virtual A3DModelPhysics* GetA3DModelPhysics() const;
	virtual APhysXSkeletonRBObject* GetSkeletonRBObject() const;
	virtual bool SetCollisionChannel(const CollisionChannel cnl);
	virtual bool AddForce(const NxVec3& pt, const NxVec3& dirANDmag, int iPhysForceType, float fSweepRadius = 0.0f);
	virtual CECModel* GetECModel() const { return m_pECModel; }

private:
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	virtual void OnControlClothes(const bool IsClose);
	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual bool SyncDataPhysXToGraphicMyself();
	virtual CPhysXObjECModel* NewObjAndCloneData() const;

	virtual bool OnResetPose();
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale);

	virtual void OnChangeDrivenMode(const DrivenMode& dmNew);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

	bool SetPhysSystemState(const bool bEnable);

private:
	// forbidden behavior
	CPhysXObjECModel(const CPhysXObjECModel& rhs);
	CPhysXObjECModel& operator= (const CPhysXObjECModel& rhs);

private:
	CECModel*		m_pECModel;
	CDynProperty	m_propECM;
	ECMRenderable	m_ECMRender;
};

#endif