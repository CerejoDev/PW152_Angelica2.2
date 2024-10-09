/*
 * FILE: GraphicsSyncRB.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_GRAPHICSSYNCRB_H_
#define _APHYSXLAB_GRAPHICSSYNCRB_H_

class CGraphicsSyncRB: public APhysXUserGraphicsSync
{
public:
	CGraphicsSyncRB(A3DWireCollector& WC) : m_pWC(&WC) {}
	
	void SetWC(A3DWireCollector& WC) { m_pWC = &WC; };

	virtual void SyncGraphicsData(const APhysXObject* pObject)
	{
		assert(pObject->IsRigidBody());
		if(!pObject->IsRigidBody())
			return;

		APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)pObject;
		NxActor* pActor = pRBObj->GetNxActor(0);
		NxShape* pShape = pActor->getShapes()[0];
		NxSphereShape* pSphere = pShape->isSphere();

		// now, only support the actor which has one sphere shape
		if (0 == pSphere)
			return;

		A3DMATRIX4 mat44;
		mat44.Identity();
		APhysXConverter::N2A_Matrix44(pObject->GetPose(), mat44);
		m_pWC->AddSphere(A3DVECTOR3(0), pSphere->getRadius(), 0x7000ff00, &mat44);
	}

	// currently, don't support the following actions
	virtual void ReleaseGraphicsData() {}
	virtual void SyncPhysicsData(APhysXObject* pPhysXObject) {}

private:
	A3DWireCollector* m_pWC;

};

#endif