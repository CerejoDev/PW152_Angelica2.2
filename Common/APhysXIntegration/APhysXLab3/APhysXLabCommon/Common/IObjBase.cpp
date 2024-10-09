/*
 * FILE: IObjBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "BrushesObj.h"

IObjBase::IObjBase()
{
	m_pOMD = 0;
	m_pProperties = 0;
	m_pRenderable = 0;
	m_pObjFocus = NULL;
	m_pBrushObj = NULL;
	m_strBrushFile.Empty();

	m_pScene = NULL;
	m_dt = 0.0f;
}

IPropObjBase* IObjBase::CreatePropSnap() const
{
	assert(0 != m_pProperties);
	return m_pProperties->CreateMyselfInstance(true);
}

int IObjBase::GetRelationshipCode(const IObjBase& objTest)
{
	int nCode = 0;
	if (&objTest == this)
		return nCode;

	ApxObjBase* pParent = GetParent();
	while (0 != pParent)
	{
		if (this == pParent)
			break;
		++nCode;
		if (&objTest == pParent)
			return nCode; 
		pParent = pParent->GetParent();
	}

	nCode = 0;
	pParent = objTest.GetParent();
	while (0 != pParent)
	{
		if (&objTest == pParent)
			break;
		--nCode;
		if (this == pParent)
			return nCode;
		pParent = pParent->GetParent();
	}
	return 0;
}

IObjBase* IObjBase::GetChildByLocalName(const TCHAR* szName)
{
	if (0 == szName)
		return 0;

	IObjBase* pChild = 0;
	int nChildren = GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(GetChild(i));
		if (pChild->GetProperties()->GetObjLocalName() == szName)
			return pChild;
	}
	return 0;
}

void IObjBase::IterateChildren(const ITask& task) const
{
	ObjManager::IterateChildren(*this, task);
}

void IObjBase::Sleep(const bool IsAsync)
{
	if (IsAsync)
		m_pProperties->m_Status.RaiseFlag(OBFI_GOTO_SLEEP);
	else
		DoSleepTask();

	IterateChildren(TaskSleepToDie(IsAsync, true));
}

bool IObjBase::WakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	bool bRtn = WakeUpMyself(eu, pPSToRuntime);
	IterateChildren(TaskWakeUp(eu, pPSToRuntime));
	return bRtn;
}

bool IObjBase::WakeUpMyself(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	if (m_pProperties->IsActive())
		return true;
	if (m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	bool bIsNULL = m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_NULL);
	assert(false == bIsNULL);
	if (bIsNULL)
	{
		a_LogOutput(1, "Warning in %s: Try to wakeup empty object! Name: %s", __FUNCTION__, m_pProperties->GetObjName_cstr());
		return false;
	}

	if (!OnDoWakeUpTask(eu, pPSToRuntime))
		return false;
	assert(m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_ACTIVE));

	float fScale = GetGScale();
	bool bIsRumtime = m_pProperties->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME);
	if (!bIsRumtime)
		fScale = m_pProperties->GetScale(false);
	OnSetScale(fScale);
	ApplyPose(bIsRumtime);
	m_pProperties->m_Status.ClearFlag(OBFI_HAS_ASYN_WORK);
	WakeUpPost(pPSToRuntime);

	//wake up brush
	if (m_pBrushObj)
	{
		A3DMATRIX4 trans(A3DMATRIX4::IDENTITY);
		GetGPose(trans);
		m_pBrushObj->Load(trans, GetGScale());
	}
	return true;
}

bool IObjBase::OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	m_pProperties->SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	return true;
}

void IObjBase::ToDie(const bool IsAsync)
{
	if (IsAsync)
		m_pProperties->m_Status.RaiseFlag(OBFI_GOTO_DESTROY);
	else
		ToDieMySelf();
	IterateChildren(TaskSleepToDie(IsAsync, false));	
}

void IObjBase::ToDieMySelf()
{
	if (m_pProperties->IsObjAliveState())
	{
		while (0 < m_Observers.GetSize())
			m_Observers[0]->SetFocusObj(0);
		SetFocusObj(0);

		LeaveRuntimeMySelf();
		DoSleepTask();
		m_pProperties->SetObjState(IPropObjBase::OBJECT_STATE_CORPSE);
		A3DRELEASE(m_pBrushObj);
	}
}

bool IObjBase::Revive(IEngineUtility& eu)
{
	if (!m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return true;

	m_pProperties->SetObjState(IPropObjBase::OBJECT_STATE_SLEEP);
	bool bRtn = WakeUp(eu);

	IterateChildren(TaskRevive(eu));
	return bRtn;
}

bool IObjBase::EnterRuntime(APhysXScene& aPhysXScene)
{
	//RaiseFlag(ApxObjBase::Object_Flag_UpdateTransform_Async);
	ClearFlag(ApxObjBase::Object_Flag_UpdateTransform_Async);
	bool bRtn = false; 
	if (m_pProperties->IsActive())
	{
		if (m_pProperties->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			bRtn = true;
		}
		else
		{
			m_pProperties->EnterRuntimePreNotify(aPhysXScene);
			if (OnEnterRuntime(aPhysXScene))
			{
				m_pProperties->EnterRuntimePostNotify(aPhysXScene);
				m_pProperties->m_Status.RaiseFlag(OBFI_IS_IN_RUNTIME);
				bRtn = true;
			}
			else
			{
				bRtn = true;
			}

			if (!m_strBrushFile.IsEmpty())
				LoadBrushObj();
		}
	}

	IterateChildren(TaskEnterLeaveRuntime(aPhysXScene));
	return bRtn;
}

void IObjBase::LeaveRuntimeMySelf()
{
	ClearFlag(ApxObjBase::Object_Flag_UpdateTransform_Async);
	if (m_pProperties->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		m_pProperties->m_Status.ClearFlag(OBFI_IS_IN_RUNTIME);
		m_pProperties->LeaveRuntimePreNotify();
		OnLeaveRuntime();
		m_pProperties->LeaveRuntimePostNotify();
		
		//release brush object
		A3DRELEASE(m_pBrushObj);
	}
}

void IObjBase::LeaveRuntime()
{
	LeaveRuntimeMySelf();
	IterateChildren(TaskEnterLeaveRuntime());
}

bool IObjBase::SaveSerialize(NxStream& stream) const
{
	return m_pProperties->SaveToFile(stream);
}

bool IObjBase::LoadDeserialize(NxStream& stream, bool& outIsLowVersion, bool bIsKeepIdx)
{
	if (bIsKeepIdx)
		m_pProperties->m_Status.RaiseFlag(OBFI_KEEP_OBJECT_INDEX);
	else
		m_pProperties->m_Status.ClearFlag(OBFI_KEEP_OBJECT_INDEX);
	bool bRtn = m_pProperties->LoadFromFile(stream, outIsLowVersion);
	m_pProperties->m_Status.ClearFlag(OBFI_KEEP_OBJECT_INDEX);
	return bRtn;
}

bool IObjBase::GetOBB(NxBox& outObjOBB) const
{
	if (m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_NULL))
		return false;
	if (m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	if (!m_pProperties->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB))
		return false;

	return ApxObjBase::GetOBB(outObjOBB);
}

bool IObjBase::GetObjAABB(NxBounds3& outObjAABB) const
{
	if (m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_NULL))
		return false;
	if (m_pProperties->QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		return false;

	if (!m_pProperties->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB))
		return false;

	return ApxObjBase::GetObjAABB(outObjAABB);
}

bool IObjBase::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool getRootObj, int objTypeFilter) const
{
	if (!m_pProperties->IsActive()) return false;

	void* pBackup = hitInfo.UserData;
	if (objTypeFilter & m_pProperties->GetObjType())
		OnRayTraceMyself(ray, hitInfo);

	IterateChildren(TaskRayTrace(ray, hitInfo, getRootObj, objTypeFilter));
	if (pBackup == hitInfo.UserData)
		return false;

	if (getRootObj)
		hitInfo.UserData = (void*)this;
	return true;
}

bool IObjBase::OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	return ApxObjBase::RayTraceOBB(ray, hitInfo, false);
}

void IObjBase::DoSleepTask()
{
	OnDoSleepTask();
	m_pProperties->SetObjState(IPropObjBase::OBJECT_STATE_SLEEP);
	m_pProperties->m_Status.ClearFlag(OBFI_GOTO_SLEEP);
	m_pProperties->m_Status.ClearFlag(OBFI_HAS_ASYN_WORK);

	//sleep brush
	if (m_pBrushObj)
	{
		m_pBrushObj->Release();
	}
}

void IObjBase::SetOriginalAABB(const NxBounds3& aabb)
{
	m_ObjOrigAABB = aabb;
	if (0 != m_pProperties)
		m_pProperties->m_Status.RaiseFlag(OBFI_INITED_PHYSX_AABB);
}

bool IObjBase::GetOriginalAABB(NxBounds3& outAABB) const
{
	if (0 != m_pProperties)
	{
		if (!m_pProperties->m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB))
			return false;
	}

	outAABB = m_ObjOrigAABB;
	return true;
}

void IObjBase::BuildExtraRenderData()
{
	if (!m_pProperties->IsActive())
		return;

	ExtraRenderable* pER = GetExtraRenderable();
	pER->Clear();

	int dwColor = 0;
	bool bDrawBindingBox = m_pProperties->GetDrawBindingBoxInfo(dwColor);
	BuildExtraRenderDataMyself(*pER, dwColor);

	if (bDrawBindingBox)
	{
		if (0 < GetChildNum())
		{
			const NxBounds3& NxAABB = ApxObjBase::GetAABB();
			A3DAABB aabb;
			APhysXConverter::N2A_AABB(NxAABB, aabb);
			pER->AddAABB(aabb, 0xff60a020);
		}
	}
}

void IObjBase::ApplyPose(bool bApplyRuntimePose)
{
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	if (bApplyRuntimePose)
	{
		GetGPose(mat);
		OnSetPose(mat);
	}
	else
	{
		mat = m_pProperties->GetPose(false);
		OnSetPose(mat);
	}
}

void IObjBase::OnTick(float dt)
{
	m_dt = dt;

	if (m_pProperties->m_Status.ReadFlag(OBFI_GOTO_DESTROY))
	{
		ToDie();
		return;
	}

	if (m_pProperties->m_Status.ReadFlag(OBFI_GOTO_SLEEP))
	{
		DoSleepTask();
		return;
	}
}

void IObjBase::OnUpdateParent()
{
	bool bItemEnable = false;
	ApxObjBase::OnUpdateParent();
	IObjBase* pParent = dynamic_cast<IObjBase*>(GetParent()); 
	if (0 == pParent)
		m_pProperties->SetObjLocalName(0);
	else
	{
		bItemEnable = true;
		IObjBase* pChild = pParent->GetChildByLocalName(m_pProperties->GetObjLocalName());
		if (0 != pChild)
			m_pProperties->SetObjLocalName(0);
	}

	CPhysXObjSelGroup* pSelGroup = m_pProperties->GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->ExecuteItemEnable(PID_ITEM_LocalName, bItemEnable);
}

void IObjBase::OnUpdateGRot()
{
	NxVec3 nxGDir, nxGUp;
	GetGDirAndUp(nxGDir, nxGUp);
	if (!m_pProperties->EnableSetPoseAndScale())
	{
		A3DVECTOR3 vPDir = m_pProperties->GetDir();
		A3DVECTOR3 vPUp = m_pProperties->GetUp();
		NxVec3 nxPDir = APhysXConverter::A2N_Vector3(vPDir);
		NxVec3 nxPUp = APhysXConverter::A2N_Vector3(vPUp);
		if (!nxPDir.equals(nxGDir, APHYSX_FLOAT_EPSILON) || !nxPUp.equals(nxGUp, APHYSX_FLOAT_EPSILON))
			SetGDirAndUp(vPDir, vPUp);
		return;
	}

	A3DVECTOR3 vGDir = APhysXConverter::N2A_Vector3(nxGDir);
	A3DVECTOR3 vGUp = APhysXConverter::N2A_Vector3(nxGUp);
	OnSetDirAndUp(vGDir, vGUp);
	m_pProperties->SetDirAndUp(vGDir, vGUp);

	if (0 != m_pBrushObj)
	{
		A3DMATRIX4 trans(A3DMATRIX4::IDENTITY);
		GetGPose(trans);
		m_pBrushObj->SetRotTrans(trans);
	}
}

void IObjBase::OnUpdateGPos()
{
	NxVec3 nxGPos = GetGPos();
	A3DVECTOR3 vPPos = m_pProperties->GetPos();
	if (!m_pProperties->EnableSetPoseAndScale())
	{
		NxVec3 nxPPos = APhysXConverter::A2N_Vector3(vPPos);
		if (!nxPPos.equals(nxGPos, APHYSX_FLOAT_EPSILON))
			SetGPos(vPPos);
		return;
	}

	A3DVECTOR3 vGPos = APhysXConverter::N2A_Vector3(nxGPos);
	OnSetPos(vGPos);
	m_pProperties->SetPos(vGPos);

	if (0 != m_pBrushObj)
		m_pBrushObj->SetPosition(vGPos);

	NxVec3 d;
	d.x = nxGPos.x - vPPos.x;
	d.y = nxGPos.y - vPPos.y;
	d.z = nxGPos.z - vPPos.z;
	for (int i = 0; i < m_Observers.GetSize(); ++i)
		m_Observers[i]->OnUpdatePosChanged(d);
}

void IObjBase::OnUpdateGScale()
{
	float fGScale = GetGScale();
	float ns = m_pProperties->GetScale();

	if (m_pProperties->EnableSetPoseAndScale() && OnSetScale(fGScale))
	{	
		if (NxMath::equals(ns, fGScale, APHYSX_FLOAT_EPSILON))
			return;
		m_pProperties->SetScale(fGScale);
		if (0 != m_pBrushObj)
			m_pBrushObj->SetScale(fGScale);
	}
	else
	{
		if (NxMath::equals(ns, fGScale, APHYSX_FLOAT_EPSILON))
			return;
		SetGScale(ns);
	}
}

void IObjBase::OnRelease()
{
	ToDieMySelf();
	ObjManager::GetInstance()->EraseManagerData(*this);
}

bool IObjBase::SetFocusObj(IObjBase* pFocus)
{
	if (m_pObjFocus == pFocus)
		return true;
	if (this == pFocus)
		return false;

	if (0 != pFocus)
	{
		if (0 != GetParent())
			return false;
	}

	if (0 != m_pObjFocus)
		m_pObjFocus->RemoveObserver(this);
	m_pObjFocus = pFocus;
	if (0 != pFocus)
		pFocus->m_Observers.Add(this);

	OnFocusObjChanged();
	return true;
}

void IObjBase::RemoveObserver(IObjBase* pObserver)
{
	int index = m_Observers.Find(pObserver);
	if (index != -1)
		m_Observers.RemoveAt(index);
}

void IObjBase::OnUpdatePosChanged(const NxVec3& deltaMove)
{
	assert(0 == GetParent());
	assert(0 != m_pObjFocus);
	NxVec3 p = GetGPos();
	p += deltaMove;

	float y = p.y;
	if (GetTerrainHeight(m_pScene->GetTerrain(), APhysXConverter::N2A_Vector3(p), y))
		p.y = y;
	SetGPos(p);
}

bool IObjBase::LoadBrushObj()
{
	if (0 == m_pBrushObj)
		m_pBrushObj = new CBrushesObj;
	else
		m_pBrushObj->Release();

	if (0 == m_pBrushObj)
		return false;

	m_pBrushObj->SetStrFile(m_strBrushFile);
	A3DMATRIX4 trans(A3DMATRIX4::IDENTITY);
	GetGPose(trans);
	return m_pBrushObj->Load(trans, GetGScale());
}

bool IObjBase::SetBrushFileName(const char* strFileName)
{
	if (strFileName && IsFileExist(strFileName, false))
	{
		m_strBrushFile = strFileName;
		return true;
	}

	m_strBrushFile.Empty();
	return false;
}

bool IObjBase::RayTraceBrush(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool bTraceChildren) const
{
	if (!m_pProperties->IsActive()) return false;
	
	CHBasedCD::RayBrushTraceInfo info;
	IObjBase* pHitObj = NULL;
	A3DVECTOR3 vNormal;

	if (m_pBrushObj != NULL)
	{
		A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
		GetGPose(mat);
		info.Init(ray.vStart, ray.vDelta, mat, GetGScale());
		
		if (m_pBrushObj->RayTrace(&info) && info.fFraction < hitInfo.t)
		{
			hitInfo.t = info.fFraction;
			mat._41 = mat._42 = mat._43 = 0;
			hitInfo.vNormal = info.ClipPlane.GetNormal() * mat;//local coordinate
			hitInfo.UserData = (void*)this;
		}
	}
	
	if (bTraceChildren)
	{
		for(int i = 0; i < GetChildNum(); i++)
		{
			ApxObjBase* pChild = GetChild(i);
			IObjBase* pObj = dynamic_cast<IObjBase*>(pChild);
			if (pObj)
			{
				pObj->RayTraceBrush(ray, hitInfo, bTraceChildren);
			}
		}
	}

	if (hitInfo.t < 1)
	{		
		return true;
	}
	return false;
}
