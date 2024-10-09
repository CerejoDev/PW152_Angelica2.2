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
	CPhysXObjECModel(const bool isNPC = true);
	virtual ~CPhysXObjECModel() { Release(); };
	
	bool SetPhysSystemState(const bool bEnable);
	bool AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, const float fForceMagnitude, const float fMaxDist);
	
public:
	virtual bool SyncDataPhysXToGraphic();
	virtual bool OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor);

	virtual A3DSkinModel* GetSkinModel() const;
	virtual APhysXSkeletonRBObject* GetSkeletonRBObject() const;
	virtual A3DModelPhysics* GetA3DModelPhysics() const;

	virtual bool SetDefaultCollisionChannel();
	virtual bool SetIndependentCollisionChannel(const bool CCCommonHost = true);
	virtual bool GetCollisionChannel(APhysXCollisionChannel& outCC) const;

private:
	virtual bool OnLoadModel(CRender& render, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj(const bool RunTimeIsEnd);
	virtual void OnControlClothes(const bool IsClose);

	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual void OnRender(A3DViewport& viewport, A3DWireCollector* pWC = 0, bool isDebug = false) const;
	virtual void OnRenderShadow(A3DViewport& viewport) const;

	virtual bool OnResetPose();
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnSetScale(const float& scale);

	virtual void OnChangeDrivenMode(const DrivenMode dmNew);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;
	virtual bool OnPlayAttackAction(const IPhysXObjBase& objTarget);

private:
	// forbidden behavior
	CPhysXObjECModel(const CPhysXObjECModel& rhs);
	CPhysXObjECModel& operator= (const CPhysXObjECModel& rhs);

private:
	CECModel*	m_pECModel;
};

#endif