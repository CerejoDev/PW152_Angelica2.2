/*
 * FILE: PhysXObjBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

IPhysXObjBase::IPhysXObjBase() : m_pPropPhysX(0)
{

}

bool IPhysXObjBase::DoLoadModelInternal(IEngineUtility& eu, const char* szFile, APhysXScene* pPhysXScene)
{
	if (!OnLoadModel(eu, szFile))
		return false;

	m_pPropPhysX->SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	if (0 != pPhysXScene)
	{
		const bool bForInitAABB = !m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB);
		const bool bForInitFlag = !m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_APHYSX_FLAGS);
		if (bForInitAABB || bForInitFlag)
		{
			OnSetPos(m_pPropPhysX->GetPos());   // set to the right position
			// Make sure OnTickAnimation() was called at least once 
			// before instance PhysX Objects, due to init some internal data of model
			OnTickAnimation(0);
			OnInstancePhysXObj(*pPhysXScene);

			if (bForInitFlag)
				m_pPropPhysX->InstancePhysXPostNotify();
			if (bForInitAABB)
			{
				A3DAABB aabb;
				if (OnGetSelectedAABB(aabb))
				{
					m_pPropPhysX->SetOriginalAABB(aabb);
					m_pPropPhysX->SetOriginalLocalHeight(aabb);
				}
				else
				{
					if (GetModelAABB(aabb))
						m_pPropPhysX->SetOriginalLocalHeight(aabb);
				}
			}
			OnReleasePhysXObj();
		}
	}
	return true;
}

bool IPhysXObjBase::LoadModel(IEngineUtility& eu, const char* szFile, APhysXScene* pPhysXScene, const bool bEnableDrawEditInfo)
{
	bool bIsNULL = m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_NULL);
	assert(true == bIsNULL);
	if (!bIsNULL)
		return false;

	if (0 != szFile)
	{
		m_pPropPhysX->SetFilePathName(szFile);
		if (!DoLoadModelInternal(eu, szFile, pPhysXScene))
		{
			m_pPropPhysX->SetFilePathName(0);
			return false;
		}
		if (bEnableDrawEditInfo)
			m_pPropPhysX->m_Flags.RaiseFlag(OBF_DRAW_BINDING_BOX);
	}

	m_pPropPhysX->m_Status.ClearFlag(OBFI_HAS_ASYN_WORK);
	return true;
}

void IPhysXObjBase::ToDie(const bool IsAsync)
{
	if (IsAsync)
		m_pPropPhysX->m_Status.RaiseFlag(OBFI_GOTO_DESTROY);
	else
		DoDestroyTask();
}

void IPhysXObjBase::Sleep(const bool IsAsync)
{
	if (IsAsync)
		m_pPropPhysX->m_Status.RaiseFlag(OBFI_GOTO_SLEEP);
	else
		DoSleepTask();
}

bool IPhysXObjBase::WakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	// Note: We assume all properties member variables are correct.
	// Just load the model, and apply the pose, the flags/states, and all other settings.
	if (OnWakeUp(eu, pPSToRuntime))
	{
		m_pPropPhysX->m_Status.ClearFlag(OBFI_HAS_ASYN_WORK);
		return true;
	}

	if (m_pPropPhysX->IsActive())
		return true;
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	bool bIsNULL = m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_NULL);
	assert(false == bIsNULL);
	if (bIsNULL)
	{
		a_LogOutput(1, "Warning in %s: Try to wakeup empty object! Name: %s", __FUNCTION__, m_pPropPhysX->GetObjName_cstr());
		return false;
	}

	if (m_pPropPhysX->IsEmptyFilePathName())
	{
		a_LogOutput(1, "Warning in %s: Empty model file path name! Name: %s", __FUNCTION__, m_pPropPhysX->GetObjName_cstr());
		return false;
	}

	if (!DoLoadModelInternal(eu, m_pPropPhysX->GetFilePathName(), pPSToRuntime))
		return false;

	ApplyPose(m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
	OnSetScale(m_pPropPhysX->GetScale(false));
	m_pPropPhysX->SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	m_pPropPhysX->m_Status.ClearFlag(OBFI_HAS_ASYN_WORK);

	if ((0 != pPSToRuntime) && !m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_LOADING))
		InstancePhysXObj(*pPSToRuntime);
	return true;
}

bool IPhysXObjBase::Revive(IEngineUtility& eu)
{
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		m_pPropPhysX->SetObjState(IPropObjBase::OBJECT_STATE_SLEEP);

	return WakeUp(eu);
}

bool IPhysXObjBase::EnterRuntime(APhysXScene& aPhysXScene)
{
	if (!m_pPropPhysX->IsActive())
		return false;

	if (m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		return true;

	if (!OnEnterRuntime(aPhysXScene))
		return false;

	m_pPropPhysX->m_Status.RaiseFlag(OBFI_IS_IN_RUNTIME);
	return true;
}

void IPhysXObjBase::LeaveRuntime()
{
	if (!m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		return;

	m_pPropPhysX->m_Status.ClearFlag(OBFI_IS_IN_RUNTIME);
	OnLeaveRuntime();
}

bool IPhysXObjBase::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	m_pPropPhysX->CopyToBackup();
	return InstancePhysXObj(aPhysXScene);
}

void IPhysXObjBase::OnLeaveRuntime()
{
	ReleasePhysXObj();
	m_pPropPhysX->RestoreFromBackup();
	ResetPose();
}

bool IPhysXObjBase::InstancePhysXObj(APhysXScene& aPhysXScene)
{
	if (!m_pPropPhysX->IsActive())
		return false;

	bool rtn = OnInstancePhysXObj(aPhysXScene);
	if (rtn)
	{
		if (m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			if (!m_pPropPhysX->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION))
				OnChangeDrivenMode(m_pPropPhysX->GetDrivenMode());
		}
		else
		{
			if (!m_pPropPhysX->m_Status.ReadFlag(OBFI_DISABLE_DEF_DMONSIM))
			{
				DrivenMode dm = m_pPropPhysX->GetDrivenMode(true);
				m_pPropPhysX->m_Status.RaiseFlag(OBFI_ENFORCE_SETDRIVENMODE);
				m_pPropPhysX->SetDrivenMode(dm);
				m_pPropPhysX->m_Status.ClearFlag(OBFI_ENFORCE_SETDRIVENMODE);
			}
		}
		m_pPropPhysX->InstancePhysXPostNotify();
	}

	return rtn;
}

void IPhysXObjBase::ReleasePhysXObj()
{
	OnReleasePhysXObj();
	if (!m_pPropPhysX->m_Status.ReadFlag(OBFI_DISABLE_DEF_DMONSIM))
	{
		m_pPropPhysX->m_Status.RaiseFlag(OBFI_ENFORCE_SETDRIVENMODE);
		m_pPropPhysX->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);
		m_pPropPhysX->m_Status.ClearFlag(OBFI_ENFORCE_SETDRIVENMODE);
	}
}

bool IPhysXObjBase::GetAPhysXAABB(A3DAABB& outAABB) const
{
	NxBounds3 aabb;
	bool rtn = GetAPhysXInstanceAABB(aabb);

	outAABB.Mins = APhysXConverter::N2A_Vector3(aabb.min);
	outAABB.Maxs = APhysXConverter::N2A_Vector3(aabb.max);
	outAABB.AddVertex(m_pPropPhysX->GetPos());
	outAABB.CompleteCenterExts();
	return rtn;
}

bool IPhysXObjBase::GetModelAABB(A3DAABB& outAABB) const
{
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_NULL))
		return false;
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	return OnGetModelAABB(outAABB);
}

void IPhysXObjBase::DoSleepTask()
{
	// Note: Do not destroy any m_Properties's member variables! We maybe wake up this object later.
	OnSleep();
	ReleasePhysXObj();
	OnReleaseModel();

	m_pPropPhysX->SetObjState(IPropObjBase::OBJECT_STATE_SLEEP);
	m_pPropPhysX->m_Status.ClearFlag(OBFI_GOTO_SLEEP);
	m_pPropPhysX->m_Status.ClearFlag(OBFI_HAS_ASYN_WORK);
}

void IPhysXObjBase::DoDestroyTask()
{
	DoSleepTask();
	m_pPropPhysX->SetObjState(IPropObjBase::OBJECT_STATE_CORPSE);
}

bool IPhysXObjBase::TickMove(float deltaTimeSec)
{
	if (!m_pPropPhysX->IsActive())
		return true;

	return OnTickMove(deltaTimeSec);
}

bool IPhysXObjBase::TickAnimation(const unsigned long deltaTime)
{
	if (m_pPropPhysX->m_Status.ReadFlag(OBFI_GOTO_DESTROY))
	{
		DoDestroyTask();
		return true;
	}

	if (m_pPropPhysX->m_Status.ReadFlag(OBFI_GOTO_SLEEP))
	{
		DoSleepTask();
		return true;
	}

	if (!m_pPropPhysX->IsActive())
		return true;

	return OnTickAnimation(deltaTime);
}

void IPhysXObjBase::SetProperties(IPropPhysXObjBase& prop)
{
	IObjBase::SetProperties(prop);
	m_pPropPhysX = &prop;
}

A3DShadowCaster* IPhysXObjBase::GetShadowCaster()
{
	if (!m_pPropPhysX->IsActive())
		return 0;

	A3DAABB aabb;
	if (!GetModelAABB(aabb))
		return 0;

	m_ShadowCaster.vMaxs = aabb.Maxs;
	m_ShadowCaster.vMins = aabb.Mins;
	return &m_ShadowCaster;
}

void IPhysXObjBase::ResetPose()
{
	if (!m_pPropPhysX->IsActive())
		return;

	if (!OnResetPose())
		return;

	ApplyPose(m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
}

void IPhysXObjBase::ApplyPose(bool bRuntimePose)
{
	OnSetPose(m_pPropPhysX->GetPose(bRuntimePose));
	// must update internal data for SM & ECM
	OnTickAnimation(0);
}

bool IPhysXObjBase::SaveSerialize(NxStream& stream) const
{
	return m_pPropPhysX->SaveToFile(stream);
}

bool IPhysXObjBase::LoadDeserialize(NxStream& stream, bool& outIsLowVersion)
{
	return m_pPropPhysX->LoadFromFile(stream, outIsLowVersion);
}

bool IPhysXObjBase::CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene)
{
	if (!m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_SLEEP))
	{
		assert(!"Shouldn't be here! Only support sleep state!");
		return false;
	}

	if (m_pPropPhysX->IsEmptyFilePathName())
	{
		assert(!"Shouldn't be here! Empty model file path name!");
		a_LogOutput(1, "Warning in %s: Empty model file path name! Name: %s", __FUNCTION__, m_pPropPhysX->GetObjName_cstr());
		return false;
	}

	const bool bBFAABB = !m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB);
	const bool bBFFlag = !m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_APHYSX_FLAGS);
	if (!bBFAABB && !bBFFlag)
		return false;

	if (!DoLoadModelInternal(eu, m_pPropPhysX->GetFilePathName(), &scene))
		return false;

	if (!m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_SLEEP))
		DoSleepTask();

	const bool bAFAABB = !m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB);
	const bool bAFFlag = !m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_APHYSX_FLAGS);
	if ((bBFAABB != bAFAABB) || (bBFFlag != bAFFlag))
		return true;

	return false;
}

bool IPhysXObjBase::GetOBB(A3DOBB& outOBB) const
{
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_NULL))
		return false;
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	if (!m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB))
		return false;

	if (m_pPropPhysX->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		return false;

	A3DMATRIX4 trans = m_pPropPhysX->GetPose();
	outOBB.Center = m_pPropPhysX->GetOriginalAABBLC() * trans;
	outOBB.Extents = m_pPropPhysX->GetOriginalAABBExt() * m_pPropPhysX->GetScale();
	outOBB.XAxis = trans.GetRow(0);
	outOBB.YAxis = trans.GetRow(1);
	outOBB.ZAxis = trans.GetRow(2);
	outOBB.CompleteExtAxis();
	return true;
}

bool IPhysXObjBase::GetAABB(A3DAABB& outAABB) const
{
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_NULL))
		return false; 
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false; 

	if (m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (GetAPhysXAABB(outAABB))
			return true;
	}

	if (!m_pPropPhysX->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB))
		return false;

	A3DMATRIX4 rot = m_pPropPhysX->GetPose();
	A3DVECTOR3 newC = m_pPropPhysX->GetOriginalAABBLC() * rot;
	A3DVECTOR3 halfDims = m_pPropPhysX->GetOriginalAABBExt() * m_pPropPhysX->GetScale();

	NxMat34 mat;
	APhysXConverter::A2N_Matrix44(rot, mat);
	NxBounds3 aabb;
	aabb.boundsOfOBB(mat.M, APhysXConverter::A2N_Vector3(newC), APhysXConverter::A2N_Vector3(halfDims));

	NxVec3 tc, te;
	aabb.getCenter(tc);
	aabb.getExtents(te);
	outAABB.Center  = APhysXConverter::N2A_Vector3(tc);
	outAABB.Extents = APhysXConverter::N2A_Vector3(te);
	outAABB.CompleteMinsMaxs();
	return true;
}

bool IPhysXObjBase::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	if (!m_pPropPhysX->IsActive()) return false;

	A3DOBB obb;
	if (!GetOBB(obb))
	{
		A3DAABB aabb;
		if (!GetAABB(aabb))
		{
			if (!GetModelAABB(aabb))
				return false;
		}
		obb.Build(aabb);
	}

	PhysOBB rtOBB;
	rtOBB.Set(obb.XAxis, obb.YAxis, obb.ZAxis, obb.Center, obb.Extents);

	if(RayTraceOBB(ray, rtOBB, hitInfo))
	{
		hitInfo.UserData = (void*)this;
		return true;
	}
	return false;
}

void IPhysXObjBase::Render(A3DViewport* pViewport, bool bDoVisCheck /*= true*/) const
{
	if (!m_pPropPhysX->IsActive())
		return;
	if (0 == pViewport)
		return;
	OnRender(*pViewport, bDoVisCheck);
}

void IPhysXObjBase::RenderExtraData(const IRenderUtility& renderUtil) const
{
	if (!m_pPropPhysX->IsActive()) return;

	A3DWireCollector* pWC = renderUtil.GetWireCollector();
	if (0 == pWC)
		return;

	int dwColor = 0;
	if (!m_pPropPhysX->GetDrawBindingBoxInfo(dwColor))
		return;

	A3DOBB obb;
	if (GetOBB(obb))
	{
		pWC->AddOBB(obb, dwColor);
		return;
	}

	A3DAABB aabb;
	if (GetAABB(aabb))
	{
		pWC->AddAABB(aabb, dwColor);
		return;
	}

	if (GetModelAABB(aabb))
	{
		const float MinVal = 0.001f;
		if ((MinVal > aabb.Extents.x) && (MinVal > aabb.Extents.y) && (MinVal > aabb.Extents.z))
		{
			aabb.Extents.Set(0.1f, 0.1f, 0.1f);
			aabb.CompleteMinsMaxs();
		}
		pWC->AddAABB(aabb, dwColor);
	}
}

/*
void IPhysXObjBase::UpdateStateSync(const A3DVECTOR3& vCenter, const float fThreshold, CRender& render, APhysXScene& aPhysXScene)
{
	if (OBJECT_STATE_NULL == m_ObjState)
		return;
	if (ReadFlag(OBF_RUNTIME_ONLY))
		return;
	if (ReadFlag(OBF_FORCE_LIVING))
		return;

	A3DVECTOR3 vDist = GetPos() - vCenter;
	bool bNeedToBeRelease = (fThreshold < vDist.Magnitude())? true : false;
	
	if (IsActive() && bNeedToBeRelease)
		Sleep();
	
	if (!IsActive() && !bNeedToBeRelease)
		WakeUp(render, &aPhysXScene);
}

void IPhysXObjBase::UpdateStateAsync(const A3DVECTOR3& vCenter, const float fThreshold, BackgroundTask& bkTask)
{
	if (OBJECT_STATE_NULL == m_ObjState)
		return; 

	if (ReadFlag(OBF_RUNTIME_ONLY))
		return;
	if (ReadFlag(OBF_FORCE_LIVING))
		return;
	if (ReadFlag(OBFI_HAS_ASYN_WORK))
		return;

	A3DVECTOR3 vDist = GetPos() - vCenter;
	bool bNeedToBeRelease = (fThreshold < vDist.Magnitude())? true : false;
	
	if (IsActive() && bNeedToBeRelease)
	{
		RaiseFlag(OBFI_HAS_ASYN_WORK);
		bkTask.AddTask(*this, true);
	}

	if (!IsActive() && !bNeedToBeRelease)
	{
		RaiseFlag(OBFI_HAS_ASYN_WORK);
		bkTask.AddTask(*this, false);
	}
} 

void IPhysXObjBase::SetAssociateObj(IPhysXObjBase* pAscObj, const NxMat34* pLocalPose, bool IsRotLocked)
{
	if (this == pAscObj)
		return;

	m_pAssociate = pAscObj;
	if (0 != pLocalPose)
		APhysXConverter::N2A_Matrix44(*pLocalPose, m_ascLocalPose);
	else
		m_ascLocalPose.Identity();

	if (0 != m_pAssociate)
	{
		A3DVECTOR3 t = m_vPos + m_ascLocalPose.GetRow(3);
		m_pAssociate->SetPos(t);

		A3DMATRIX4 rot = m_ascLocalPose;
		rot.SetRow(3, A3DVECTOR3(0));
		A3DVECTOR3 d = m_vDir * rot; 
		A3DVECTOR3 u = m_vUp * rot; 
		m_pAssociate->SetDirAndUp(d, u);
	}

	if (IsRotLocked)
		RaiseFlag(OBF_ASSOCIATE_ROT_ISLOCKED);

	if (0 == m_pAssociate)
	{
		ClearFlag(OBF_ASSOCIATE_ROT_ISLOCKED);
		ClearFlag(OBF_ASSOCIATE_ALL_ISLOCKED);
	}
}

IPhysXObjBase* IPhysXObjBase::GetAssociateObj(NxMat34* pOutLocalPose)
{
	if(0 != pOutLocalPose)
		APhysXConverter::A2N_Matrix44(m_ascLocalPose, *pOutLocalPose);
	return m_pAssociate;
}

CPhysXObjForceField* IPhysXObjBase::GetAssociateFF(NxMat34* pOutLocalPose)
{
	IPhysXObjBase* pObj = GetAssociateObj(pOutLocalPose);
	if (0 == pObj)
		return 0;

	if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType())
		return 0;

	return static_cast<CPhysXObjForceField*>(pObj);
}
*/