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
#include "GameProperty.h"

class A3DSkinModel;
class A3DModelPhysSyncData;
class A3DModelPhysics;
class CClothSkinConfig;
class APhysXSkeletonRBObject;

class CPhysXObjSkinModel : public CPhysXObjDynamic
{
public:
	static A3DSkinModel* LoadSkinModel(CRender& render, const char* szFile, int iSkinFlag = A3DSkinModel::LSF_DEFAULT);
	static void ReleaseSkinModel(A3DSkinModel*& pSkinModel);

public:
	CPhysXObjSkinModel(int UID = -1);
	virtual ~CPhysXObjSkinModel() { ToDie(false); };

public:
	virtual bool SyncDataPhysXToGraphic();

	virtual CDynProperty* GetProperties() { return &m_propSM; }
	virtual const CDynProperty* GetProperties() const { return  &m_propSM; }
	virtual A3DSkinModel* GetSkinModel() const { return m_pSkinModel; }
	virtual A3DModelPhysics* GetA3DModelPhysics() const;
	virtual APhysXSkeletonRBObject* GetSkeletonRBObject() const;
/*	virtual bool OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor);
*/
	virtual bool SetCollisionChannel(const CollisionChannel cnl);

private:
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	virtual void OnControlClothes(const bool IsClose);

	virtual bool OnTickAnimation(const unsigned long deltaTime);
	virtual void OnRender(A3DViewport& viewport, bool bDoVisCheck) const;

	virtual bool OnResetPose();
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale);

	virtual void OnChangeDrivenMode(const DrivenMode& dmNew);
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

private:
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
	CDynProperty			m_propSM;
};

#endif