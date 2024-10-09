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
			A3DMATRIX4 gPoseBackup;
			GetGPose(gPoseBackup);
			A3DMATRIX4 idPose(A3DMATRIX4::IDENTITY);
			SetGPose(idPose);
			// Make sure OnTickAnimation() was called at least once 
			// before instance PhysX Objects, due to init some internal data of model
			OnTickAnimation(0);
			if (OnInstancePhysXObj(*pPhysXScene))
			{
				if (bForInitFlag)
					m_pPropPhysX->InstancePhysXPostNotify();
			}
			if (bForInitAABB)
			{
				A3DAABB aabb;
				NxBounds3 nxAABB;
				if (GetAPhysXInstanceAABB(nxAABB))
				{
					SetOriginalAABB(nxAABB);
					APhysXConverter::N2A_AABB(nxAABB, aabb);
					m_pPropPhysX->SetOriginalLocalHeight(aabb);
				}
				else
				{
					if (GetModelAABB(aabb))
					{
						APhysXConverter::A2N_AABB(aabb, nxAABB);
						SetOriginalAABB(nxAABB);
						m_pPropPhysX->SetOriginalLocalHeight(aabb);
					}
				}
			}
			OnReleasePhysXObj();
			SetGPose(gPoseBackup);
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

bool IPhysXObjBase::OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	// Note: We assume all properties member variables are correct.
	// Just load the model, and apply the pose, the flags/states, and all other settings.
	if (m_pPropPhysX->IsEmptyFilePathName())
	{
		a_LogOutput(1, "Warning in %s: Empty model file path name! Name: %s", __FUNCTION__, m_pPropPhysX->GetObjName_cstr());
		return false;
	}

	if (!DoLoadModelInternal(eu, m_pPropPhysX->GetFilePathName(), pPSToRuntime))
		return false;

	return true;
}

void IPhysXObjBase::WakeUpPost(APhysXScene* pPSToRuntime)
{
	if (0 != pPSToRuntime)
	{
		if (m_pScene->QueryRPTStateEnable(GetGPos()))
			InstancePhysXObj(*pPSToRuntime);
	}
}

bool IPhysXObjBase::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	bool bEnablePhysX = true;
	if (!m_pScene->QueryRPTMode(RPT_DISABLE_TEST))
	{
		if (!m_pScene->QueryRPTStateEnable(GetGPos()))
			bEnablePhysX = false;
	}
	if (bEnablePhysX)
		InstancePhysXObj(aPhysXScene);

	//set convex brush file name
	if (GetBrushFileName().IsEmpty())
	{
		AString strFile = m_pPropPhysX->GetFilePathName();
		if (!strFile.IsEmpty())
		{
#if defined(_ANGELICA3)
			af_ChangeFileExt(strFile, ".chb");
#elif defined(_ANGELICA21)
			af_ChangeFileExt(strFile, ".chf");
#endif
			SetBrushFileName(strFile);
		}
	}

	return true;
}

void IPhysXObjBase::OnLeaveRuntime()
{
	ReleasePhysXObj();

	//set convex brush file name to null
	SetBrushFileName(NULL);
}

bool IPhysXObjBase::InstancePhysXObj(APhysXScene& aPhysXScene)
{
	bool rtn = false;
	if (m_pPropPhysX->IsActive())
	{
		rtn = OnInstancePhysXObj(aPhysXScene);
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
		else
		{
			m_pPropPhysX->GetFlags().ClearFlag(OBF_ENABLE_PHYSX_BODY);
			const char*	pStr = m_pPropPhysX->GetObjName_cstr();
			a_LogOutput(1, "Warning in %s: Can't instance PhysX body! Clear the flag: OBF_ENABLE_PHYSX_BODY. Name: %s", __FUNCTION__, pStr);
		}
	}

	IterateChildren(TaskPhysXObjOnOff(aPhysXScene));
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
	IterateChildren(TaskPhysXObjOnOff());
}

void IPhysXObjBase::ControlClothes(const bool IsClose)
{
	if (!m_pPropPhysX->IsActive())
		return;
	OnControlClothes(IsClose);
}

bool IPhysXObjBase::GetModelAABB(A3DAABB& outAABB) const
{
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_NULL))
		return false;
	if (m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	return OnGetModelAABB(outAABB);
}

void IPhysXObjBase::OnDoSleepTask()
{
	// Note: Do not destroy any m_Properties's member variables! We maybe wake up this object later.
	OnSleep();
	ReleasePhysXObj();
	OnReleaseModel();
}

void IPhysXObjBase::OnTick(float dt)
{
	IObjBase::OnTick(dt);

	if (m_pPropPhysX->IsActive())
	{
		OnTickMove(dt);
		unsigned long deltaTime = unsigned int(dt * 1000);
		if (OnTickAnimation(deltaTime))
			BuildExtraRenderData();
	}
}

bool IPhysXObjBase::SyncDataPhysXToGraphic()
{
	bool bRtn = SyncDataPhysXToGraphicMyself();
	IterateChildren(TaskSyncData());
	return bRtn;
}

void IPhysXObjBase::SetProperties(IPropPhysXObjBase& prop)
{
	IObjBase::SetProperties(prop);
	m_pPropPhysX = &prop;
}

void IPhysXObjBase::ResetRuntimePose()
{
	if (!m_pPropPhysX->IsActive())
		return;

	if (!OnResetPose())
		return;

	ApplyPose(true);
}

void IPhysXObjBase::ApplyPose(bool bApplyRuntimePose)
{
	IObjBase::ApplyPose(bApplyRuntimePose);
	// must update internal data for SM & ECM
	OnTickAnimation(0);
}

bool IPhysXObjBase::SaveSerialize(NxStream& stream) const
{
	return m_pPropPhysX->SaveToFile(stream);
}

bool IPhysXObjBase::LoadDeserialize(NxStream& stream, bool& outIsLowVersion, bool bIsKeepIdx)
{
	if (bIsKeepIdx)
		m_pPropPhysX->m_Status.RaiseFlag(OBFI_KEEP_OBJECT_INDEX);
	else
		m_pPropPhysX->m_Status.ClearFlag(OBFI_KEEP_OBJECT_INDEX);
	bool bRtn = m_pPropPhysX->LoadFromFile(stream, outIsLowVersion);
	m_pPropPhysX->m_Status.ClearFlag(OBFI_KEEP_OBJECT_INDEX);
	return bRtn;
}

bool IPhysXObjBase::CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene)
{
	bool bRtn = CheckDefPropsUpdateOnlyMySelf(eu, scene);
	TaskCheckDefPropUpdate task(eu, scene, true);
	IterateChildren(task);
	bool bRtnChild = false;
	if (0 < task.GetExpectResult().GetNbObjectsEqualExpect())
		bRtnChild = true;
	return bRtn || bRtnChild;
}

bool IPhysXObjBase::CheckDefPropsUpdateOnlyMySelf(IEngineUtility& eu, APhysXScene& scene)
{
	if (!m_pPropPhysX->QueryObjState(IPropObjBase::OBJECT_STATE_SLEEP))
	{
		assert(!"Shouldn't be here! Only support sleep state!");
		return false;
	}

	if (ObjManager::OBJ_TYPEID_FORCEFIELD != m_pPropPhysX->GetObjType())
	{
		if (m_pPropPhysX->IsEmptyFilePathName())
		{
			assert(!"Shouldn't be here! Empty model file path name!");
			a_LogOutput(1, "Warning in %s: Empty model file path name! Name: %s", __FUNCTION__, m_pPropPhysX->GetObjName_cstr());
			return false;
		}
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

bool IPhysXObjBase::GetOBB(NxBox& outObjOBB) const
{
	return IObjBase::GetOBB(outObjOBB);
}

bool IPhysXObjBase::GetObjAABB(NxBounds3& outObjAABB) const
{
	if (m_pPropPhysX->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (GetAPhysXInstanceAABB(outObjAABB))
			return true;
	}

	if (IObjBase::GetObjAABB(outObjAABB))
		return true;
	
	A3DAABB aabb;
	if (GetModelAABB(aabb))
	{
		outObjAABB.set(APhysXConverter::A2N_Vector3(aabb.Mins), APhysXConverter::A2N_Vector3(aabb.Maxs));
		return true;
	}
	return false;
}

void IPhysXObjBase::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
{
	if (0 == dwColor)
		return;

	NxBox nxOBB;
	if (GetOBB(nxOBB))
	{
		A3DOBB obb;
		obb.Center = APhysXConverter::N2A_Vector3(nxOBB.center);
		obb.Extents = APhysXConverter::N2A_Vector3(nxOBB.extents);
		obb.XAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(0));
		obb.YAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(1));
		obb.ZAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(2));
		obb.CompleteExtAxis();
		er.AddOBB(obb, dwColor);
		return;
	}

	A3DAABB aabb;
	NxBounds3 nxAABB;
	if (GetObjAABB(nxAABB))
	{
		APhysXConverter::N2A_AABB(nxAABB, aabb);
		er.AddAABB(aabb, dwColor);
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
		er.AddAABB(aabb, dwColor);
	}
}

