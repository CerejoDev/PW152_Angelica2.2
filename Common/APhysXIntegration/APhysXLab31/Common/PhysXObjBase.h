/*
 * FILE: PhysXObjBase.h
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
#ifndef _APHYSXLAB_PHYSXOBJBASE_H_
#define _APHYSXLAB_PHYSXOBJBASE_H_

class IEngineUtility;
class IObjMgrData;
#include "IObjBase.h"
#include "IPropPhysXObjBase.h"

class IPhysXObjBase: public IObjBase
{
public:
	typedef IPropPhysXObjBase::DrivenMode	DrivenMode;

	IPhysXObjBase();
	virtual ~IPhysXObjBase() {};

	IPropPhysXObjBase* GetProperties() const { return m_pPropPhysX; }

	bool LoadModel(IEngineUtility& eu, const char* szFile, APhysXScene* pPhysXScene = 0, const bool bEnableDrawEditInfo = true);
	void Sleep(const bool IsAsync = false);
	bool WakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime = 0); 
	void ToDie(const bool IsAsync = false);
	bool Revive(IEngineUtility& eu);

	bool EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();

	bool InstancePhysXObj(APhysXScene& aPhysXScene);
	void ReleasePhysXObj();

/*	void UpdateStateSync(const A3DVECTOR3& vCenter, const float fThreshold, CRender& render, APhysXScene& aPhysXScene); 
	void UpdateStateAsync(const A3DVECTOR3& vCenter, const float fThreshold, BackgroundTask& bkTask); 
*/
	bool TickMove(float deltaTimeSec);
	bool TickAnimation(const unsigned long deltaTime);

	void ResetPose();

/*	void SetAssociateObj(IPhysXObjBase* pAscObj, const NxMat34* pLocalPose = 0, bool IsRotLocked = false);
	IPhysXObjBase* GetAssociateObj(NxMat34* pOutLocalPose = 0);
	CPhysXObjForceField* GetAssociateFF(NxMat34* pOutLocalPose = 0);
*/

	virtual bool SaveSerialize(NxStream& stream) const;
	virtual bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion);
	virtual bool CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene);
 
	virtual bool SyncDataPhysXToGraphic() = 0;
//	virtual NxActor* GetFirstNxActor() const = 0;
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const = 0;
	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const = 0;

	/* not supported if pure PhysX driven mode using in runtime */
	virtual bool GetOBB(A3DOBB& outOBB) const;
	virtual bool GetAABB(A3DAABB& outAABB) const;

//	virtual bool OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor = 0) { return true; }
	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;
	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck /*= true*/) const;
	virtual void RenderExtraData(const IRenderUtility& renderUtil) const;

protected:
	virtual void SetProperties(IPropPhysXObjBase& prop);
	virtual A3DShadowCaster* GetShadowCaster();
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) = 0;
	virtual void OnLeaveRuntime() = 0;

private:
	void DoSleepTask();
	void DoDestroyTask();
	bool DoLoadModelInternal(IEngineUtility& eu, const char* szFile, APhysXScene* pPhysXScene);
	void ApplyPose(bool bRuntimePose);

	bool GetAPhysXAABB(A3DAABB& outAABB) const;

private:
	friend class IObjPropertyBase;
	friend class IPropPhysXObjBase;

	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile) = 0; 
	virtual void OnReleaseModel() = 0;

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene) = 0;
	virtual void OnReleasePhysXObj() = 0;

	virtual bool OnTickMove(float deltaTimeSec) = 0;
	virtual bool OnTickAnimation(const unsigned long deltaTime) = 0;
	virtual void OnRender(A3DViewport& viewport, bool bDoVisCheck) const = 0;

	virtual void OnChangeDrivenMode(const DrivenMode& dmNew) {}
	virtual void OnSleep() {}
	virtual bool OnWakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime = 0) { return false; }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const = 0;
	virtual bool OnGetSelectedAABB(A3DAABB& outAABB) const { return GetAPhysXAABB(outAABB); }

protected:
	bool GetModelAABB(A3DAABB& outAABB) const;

private:
	// forbidden behavior
	IPhysXObjBase(const IPhysXObjBase& rhs);
	IPhysXObjBase& operator= (const IPhysXObjBase& rhs);

private:
	IPropPhysXObjBase* m_pPropPhysX;
	A3DShadowCaster  m_ShadowCaster;
};

#endif
