/*
 * FILE: OverlapBounceDamping.h
 *
 * DESCRIPTION: 
 *		It will generate a great bounce while the kinematic actors that are overlaped deeply each other
 *      turn into dynamic actors. This class is used for reduce the bounce as far as possible.
 *
 *		Note:
 *          1. Considering performance, it is not supported the instance of class was applied to multi-scene. 
 *          2. The appended models(NxActors) must not be released/deleted during the instance of class is working.
 *			3. The instance of class must be destructed before the applied NxScene was released
 *
 * CREATED BY: Yang Liu, 2009/08/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _OVERLAP_BOUNCE_DAMPING_H_
#define _OVERLAP_BOUNCE_DAMPING_H_

#include "NxUserContactReport.h"
#include <map>

#define NOMINMAX   // required for PhysX SDK. see PhysX doc "SDK Initialization"
#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4786)

class APhysXSkeletonRBObject;

class CMOBDamping : public NxUserContactModify
{
public:
	CMOBDamping() : m_bIsInitedCS(false) {}
	virtual ~CMOBDamping() { CloseCM(); if (m_bIsInitedCS) DeleteCriticalSection(&m_csCM); }

	virtual bool onContactConstraint(NxU32& changeFlags, const NxShape* shape0, const NxShape* shape1,  
		const NxU32 featureIndex0, const NxU32 featureIndex1, NxContactCallbackData& data);
	
	/*
	clear any previous information. prepare the contact modify. 
	*/
	void OpenCM();

	/*
	clear any previous information. prepare the contact modify. And append a model.
	see AppendModel() for more description.
	*/
	bool OpenCM(APhysXSkeletonRBObject& sklRB);

	/*
	append the model that will be affected
	
	Parameters:
		[in]  sklRB		The model  
	  
	Return Value: 
		Return false if the parameter model and the existing models are not in the same NxScene. Or return true.
		If there is no existing model, append the model and return true.
		  
	Remarks:
		This function should be called after any OpenCM(). 
	*/
	bool AppendModel(APhysXSkeletonRBObject& sklRB);

	/*
	close the contact modify. 
	*/
	void CloseCM() { m_Scene.RestoreAll(); }

	/*
	update tick info. 
	
	Remarks:
		This function should be called after PhysX simulation.
		This function is independence from the state of contact modify.
		It can be called safety even after CloseCM() or before OpenCM(). 

		The sleeping NxActors will be removed from the appended list.
		If the list is empty, CloseCM() will be called automatic.
	*/
	void UpdateTick() { m_Scene.UpdateTick(); }

	/*
	an interface that used to query current state. 
	*/
	bool IsWorking() const { return m_Scene.IsWorking(); }

private:
	class SceneNode
	{
	public:
		SceneNode() { m_bIsInstalledCM = false; m_pScene = 0; m_pCMBack = 0; }
		bool AddActor(NxActor& ac);
		bool SetContactModify(CMOBDamping* pCM);
		bool UpdateTick();
		void RestoreAll();
		bool IsWorking() const { return (0 != m_pScene); }
		bool IsTheSameScene(NxScene& sc);

	private:
		bool m_bIsInstalledCM;
		NxScene* m_pScene;
		NxUserContactModify* m_pCMBack;
		std::map<NxActor*, bool> m_mapActInfo;
	};

private:
	bool m_bIsInitedCS;
	RTL_CRITICAL_SECTION m_csCM;
	SceneNode m_Scene;
};

#pragma warning(pop)
#endif