/*
* FILE: PhysXObjSkinModel.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/05/19
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJSKINMODEL_H_
#define _APHYSXLAB_PHYSXOBJSKINMODEL_H_

#include "PhysXObjDynamic.h"

class A3DSkinModel;
class A3DModelPhysSyncData;
class A3DModelPhysics;
class CClothSkinConfig;

class CPhysXObjSkinModel : public CPhysXObjDynamic
{
public:
	CPhysXObjSkinModel(const bool isNPC = true);
	virtual ~CPhysXObjSkinModel() { Release(); };

public:
	virtual bool SyncDataPhysXToGraphic();
	virtual bool OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor);

	virtual A3DSkinModel* GetSkinModel() const { return m_pSkinModel; }
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

	virtual bool OnResetPose();
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnSetScale(const float& scale);

	virtual void OnChangeDrivenMode(const DrivenMode dmNew);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

	A3DSkinModel* LoadSkinModel(CRender& render, const char* szFile);
	void OpenSkinCloth();
	void CloseSkinCloth();
	
private:
	// forbidden behavior
	CPhysXObjSkinModel(const CPhysXObjSkinModel& rhs);
	CPhysXObjSkinModel& operator= (const CPhysXObjSkinModel& rhs);

protected:
	A3DSkinModel*			m_pSkinModel;
	A3DModelPhysSyncData*	m_pSyncData;
	A3DModelPhysics*		m_pModelPhys;		//	Model physics proxy

	int						m_iCSStartID;
	CClothSkinConfig*		m_pCSConfig;
};

#endif