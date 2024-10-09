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

A3DWireCollector* IPhysXObjBase::pWCTopZBuf = 0;

IPhysXObjBase::IPhysXObjBase(const int type)
{
	m_nTypeID = type; 
	SetToEmpty();
}

void IPhysXObjBase::SetToEmpty()
{
	m_flags = OBF_ENABLE_PHYSX_BODY | OBFI_NEW_PEOPLE_INFO;
	m_PeopleFlags = OBPI_IS_NONE_PEOPLE;
	m_eDrivenMode = DRIVEN_BY_ANIMATION;
	m_eDMDefOnSim = DRIVEN_BY_PURE_PHYSX;
	
	m_AABBRelativeCenter.Set(0, 0, 0);
	m_OriginalExtents.Set(0, 0, 0);
	m_strFilePath.Empty();

	m_ObjState = OBJECT_STATE_NULL;
	m_fScale = 1;
	
	m_vPos.Set(0, 0, 0);
	m_vDir.Set(0, 0, 1);
	m_vUp.Set(0, 1, 0);

	m_ascLocalPose.Identity();
	m_pAssociate = 0;

	m_CtrlGroup = -1;
	m_MotionDist = -1;
	OnSetToEmpty();
}

void IPhysXObjBase::CopyBasicProperty(const IPhysXObjBase& obj)
{
	m_flags			 = obj.m_flags;
	m_PeopleFlags	 = obj.m_PeopleFlags;
	m_eDrivenMode	 = obj.m_eDrivenMode;
	m_eDMDefOnSim	 = obj.m_eDMDefOnSim;

	m_AABBRelativeCenter = obj.m_AABBRelativeCenter;
	m_OriginalExtents = obj.m_OriginalExtents;
	m_strFilePath = obj.m_strFilePath;

	m_ObjState = obj.m_ObjState;
	m_fScale   = obj.m_fScale;

	m_vPos = obj.m_vPos;
	m_vDir = obj.m_vDir;
	m_vUp  = obj.m_vUp;

	m_CtrlGroup = obj.m_CtrlGroup;
	m_MotionDist = obj.m_MotionDist;

	m_refFrame = obj.m_refFrame;
	OnCopyBasicProperty(obj);

	if (OBJECT_STATE_ACTIVE == m_ObjState)
		m_ObjState = OBJECT_STATE_SLEEP;
}

void IPhysXObjBase::ClearPeopleInfo(const PeopleInfo flag)
{
	const int backup = m_PeopleFlags;

	bool EnableClear = false;
	if ((OBPI_HAS_APHYSX_CC == flag) || (OBPI_HAS_APHYSX_LWCC == flag) || (OBPI_HAS_APHYSX_DYNCC == flag) 
		|| (OBPI_HAS_BRUSH_CC == flag) || (OBPI_HAS_APHYSX_NXCC))
	{
		m_PeopleFlags &= ~flag;
		EnableClear = true;
	}

	if (!ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if ((OBPI_IS_MAIN_ACTOR == flag) || (OBPI_IS_NPC == flag))
			EnableClear = true;
	}

	if (!EnableClear)
		return;

	switch (flag)
	{
	case OBPI_IS_MAIN_ACTOR:
		if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR))
			m_PeopleFlags = OBPI_IS_NONE_PEOPLE;
		break;
	case OBPI_IS_NPC:
		if (ReadPeopleInfo(OBPI_IS_NPC))
			m_PeopleFlags = OBPI_IS_NONE_PEOPLE;
		break;
	}

	if (backup != m_PeopleFlags)
	{
		RaiseFlag(OBFI_NEW_PEOPLE_INFO);
		if (ReadPeopleInfo(OBPI_IS_NONE_PEOPLE))
			SetDefaultDMOnSim(DRIVEN_BY_PURE_PHYSX);
		else
		{
			if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
				SetDefaultDMOnSim(DRIVEN_BY_ANIMATION);
		}
	}
}

void IPhysXObjBase::RaisePeopleInfo(const PeopleInfo flag)
{
	const int backup = m_PeopleFlags;
	switch (flag)
	{
	case OBPI_IS_NONE_PEOPLE:
		if (!ReadFlag(OBFI_IS_IN_RUNTIME))
			m_PeopleFlags = OBPI_IS_NONE_PEOPLE;
		break;
	case OBPI_IS_MAIN_ACTOR:
		if (!ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			if (OBPI_HAS_APHYSX_DYNCC & flag)
				m_PeopleFlags = OBPI_IS_MAIN_ACTOR | OBPI_HAS_APHYSX_DYNCC | OBPI_ACTION_WALK;
			else
				m_PeopleFlags = OBPI_IS_MAIN_ACTOR | OBPI_HAS_APHYSX_CC | OBPI_ACTION_WALK;
		}
		break;
	case OBPI_IS_NPC:
		if (!ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			if (OBPI_HAS_APHYSX_DYNCC & flag)
				m_PeopleFlags = OBPI_IS_NPC | OBPI_HAS_APHYSX_DYNCC | OBPI_ACTION_WALK;
			else
				m_PeopleFlags = OBPI_IS_NPC | OBPI_HAS_APHYSX_CC | OBPI_ACTION_DANCE;
		}
		break;
	case OBPI_HAS_APHYSX_CC:
		if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
		{
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_LWCC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_DYNCC;
			m_PeopleFlags &= ~OBPI_HAS_BRUSH_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_NXCC;
			m_PeopleFlags |= OBPI_HAS_APHYSX_CC;
		}
		break;
	case OBPI_HAS_APHYSX_LWCC:
		if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
		{
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_DYNCC;
			m_PeopleFlags &= ~OBPI_HAS_BRUSH_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_NXCC;
			m_PeopleFlags |= OBPI_HAS_APHYSX_LWCC;
		}
		break;
	case OBPI_HAS_APHYSX_DYNCC:
		if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
		{
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_LWCC;
			m_PeopleFlags &= ~OBPI_HAS_BRUSH_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_NXCC;
			m_PeopleFlags |= OBPI_HAS_APHYSX_DYNCC;
		}
		break;
	case OBPI_HAS_BRUSH_CC:
		if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
		{
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_LWCC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_DYNCC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_NXCC;
			m_PeopleFlags |= OBPI_HAS_BRUSH_CC;
		}
		break;
	case OBPI_HAS_APHYSX_NXCC:
		if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
		{
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_CC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_LWCC;
			m_PeopleFlags &= ~OBPI_HAS_APHYSX_DYNCC;
			m_PeopleFlags &= ~OBPI_HAS_BRUSH_CC;
			m_PeopleFlags |= OBPI_HAS_APHYSX_NXCC;
			
		}
		break;
	default:
		if (ReadPeopleInfo(OBPI_IS_NPC))
		{
			m_PeopleFlags &= 0xffffff0f;
			m_PeopleFlags |= flag;
		}
		break;
	}
	
	if (backup != m_PeopleFlags)
	{
		RaiseFlag(OBFI_NEW_PEOPLE_INFO);
		if (ReadPeopleInfo(OBPI_IS_NONE_PEOPLE))
			SetDefaultDMOnSim(DRIVEN_BY_PURE_PHYSX);
		else
		{
			if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR) || ReadPeopleInfo(OBPI_IS_NPC))
				SetDefaultDMOnSim(DRIVEN_BY_ANIMATION);
		}
	}
}

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

bool IPhysXObjBase::DoLoadModelInternal(CRender& render, const char* szFile, APhysXScene* pPhysXScene, const bool OpenEditInfo)
{
	if (!OnLoadModel(render, szFile))
		return false;

	m_ObjState = OBJECT_STATE_ACTIVE;
	const DrivenMode backup = m_eDrivenMode;
	if (0 != pPhysXScene)
	{
		bool bForAABB = !ReadFlag(OBFI_INITED_AABB);
		bool bForPMFlags = !ReadFlag(OBFI_INITED_PMFLAGS);
		if (bForAABB || bForPMFlags)
		{
			OnSetPos(m_vPos);   // set to the right position
			// Make sure OnTickAnimation() was called at least once 
			// before instance PhysX Objects, due to init some internal data
			OnTickAnimation(0);
			OnInstancePhysXObj(*pPhysXScene);

			if (bForPMFlags)
				OnInstancePhysXObjPost(*pPhysXScene);
			A3DAABB aabb;
			if (bForAABB && OnGetSelectedAABB(aabb))
			{
				m_OriginalExtents = aabb.Extents;
				m_AABBRelativeCenter = aabb.Center - m_vPos;
				if (NxMath::equals(m_AABBRelativeCenter.x, 0, 0.00001f)) m_AABBRelativeCenter.x = 0;
				if (NxMath::equals(m_AABBRelativeCenter.y, 0, 0.00001f)) m_AABBRelativeCenter.y = 0;
				if (NxMath::equals(m_AABBRelativeCenter.z, 0, 0.00001f)) m_AABBRelativeCenter.z = 0;
				RaiseFlag(OBFI_INITED_AABB);
			}
			OnReleasePhysXObj(true);
		}
	}

	m_eDrivenMode = backup;
	OnChangeDrivenMode(m_eDrivenMode);

	if (OpenEditInfo)
	{
		RaiseFlag(OBF_DRAW_BINDING_BOX);
		RaiseFlag(OBF_DRAW_REF_FRAME);
	}

	return true;
}

bool IPhysXObjBase::LoadModel(CRender& render, const char* szFile, APhysXScene* pPhysXScene, const bool OpenEditInfo)
{
	assert(OBJECT_STATE_NULL == m_ObjState);
	if (OBJECT_STATE_NULL != m_ObjState)
		return false;

	if (0 != szFile)
	{
		if (!DoLoadModelInternal(render, szFile, pPhysXScene, OpenEditInfo))
			return false;

		af_GetRelativePath(szFile, m_strFilePath);
	}

	ClearFlag(OBFI_HAS_ASYN_WORK);
	return true;
}

void IPhysXObjBase::Release(const bool IsAsync)
{
	if (IsAsync)
		RaiseFlag(OBFI_GOTO_SLEEP);
	else
		DoDestroyTask();
}

void IPhysXObjBase::DoDestroyTask()
{
	DoSleepTask();
	SetToEmpty();
}

void IPhysXObjBase::DoSleepTask()
{
	// Note: Do not destroy any member variables! We maybe wake up this object later.
	OnSleep();
	ReleasePhysXObj();
	OnReleaseModel();

	m_ObjState = OBJECT_STATE_SLEEP;
	ClearFlag(OBFI_GOTO_SLEEP);
	ClearFlag(OBFI_HAS_ASYN_WORK);
}

void IPhysXObjBase::ApplyPose()
{
	OnSetPos(m_vPos);
	OnSetDirAndUp(m_vDir, m_vUp);
	OnSetScale(m_fScale);

	// must update internal data for SM & ECM
	OnTickAnimation(0);
}

void IPhysXObjBase::Sleep(const bool IsAsync)
{
	if (IsAsync)
		RaiseFlag(OBFI_GOTO_SLEEP);
	else
		DoSleepTask();
}

bool IPhysXObjBase::WakeUp(CRender& render, APhysXScene* pPhysXScene)
{
	// Note: We assume all member variables are correct.
	// Just load the model, and apply the pose.
	if (OnWakeUp(render))
	{
		ClearFlag(OBFI_HAS_ASYN_WORK);
		return true;
	}

	if (IsActive())
		return true;

	assert(OBJECT_STATE_NULL != m_ObjState);
	if (OBJECT_STATE_NULL == m_ObjState)
		return false;

	assert(!m_strFilePath.IsEmpty());
	if (m_strFilePath.IsEmpty())
		return false;

	if (!DoLoadModelInternal(render, m_strFilePath, pPhysXScene, false))
		return false;

	ApplyPose();
	m_ObjState = OBJECT_STATE_ACTIVE;
	ClearFlag(OBFI_HAS_ASYN_WORK);

	if ((0 != pPhysXScene) && !ReadFlag(OBFI_IS_IN_LOADING))
		InstancePhysXObj(*pPhysXScene);
	return true;
}

bool IPhysXObjBase::EnterRuntime(APhysXScene& aPhysXScene)
{
	if (!IsActive())
		return false;

	if (!OnEnterRuntime(aPhysXScene))
		return false;

	RaiseFlag(OBFI_IS_IN_RUNTIME);
	return true;
}

void IPhysXObjBase::LeaveRuntime()
{
	ClearFlag(OBFI_IS_IN_RUNTIME);
	OnLeaveRuntime();
}

bool IPhysXObjBase::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	if (!InstancePhysXObj(aPhysXScene))
		return false;

	m_BackUpFlags = m_flags;
	m_BackUpPeopleFlags = m_PeopleFlags;
	if (ReadPeopleInfo(OBPI_IS_NPC))
	{
		RaiseFlag(OBFI_HAS_BACKUP_POSE);
		m_vBackUpPos = m_vPos;
		m_vBackUpDir = m_vDir;
		m_vBackUpUp  = m_vUp; 
	}

	return true;
}

void IPhysXObjBase::OnLeaveRuntime()
{
	ReleasePhysXObj();

	ClearFlag(OBF_FORCE_LIVING);
	if (ReadFlag(OBFI_HAS_BACKUP_POSE))
	{
		ClearFlag(OBFI_HAS_BACKUP_POSE);
		SetPos(m_vBackUpPos);
		SetDirAndUp(m_vBackUpDir, m_vBackUpUp);
	}

	m_flags = m_BackUpFlags;
	m_PeopleFlags = m_BackUpPeopleFlags;
}

bool IPhysXObjBase::InstancePhysXObj(APhysXScene& aPhysXScene)
{
	if (!IsActive())
		return false;
	
	bool rtn = OnInstancePhysXObj(aPhysXScene);
	if (rtn)
	{
		if (ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			if ((DRIVEN_BY_ANIMATION != m_eDrivenMode))
				OnChangeDrivenMode(m_eDrivenMode);
		}
		else
		{
			if (!ReadFlag(OBF_CLOSE_DEFAULT_DM))
			{
				if ((DRIVEN_BY_ANIMATION != m_eDMDefOnSim) || (m_eDrivenMode != m_eDMDefOnSim))
				{
					OnChangeDrivenMode(m_eDMDefOnSim);
					m_eDrivenMode = m_eDMDefOnSim;
				}
			}
		}
		OnInstancePhysXObjPost(aPhysXScene);
	}

	return rtn;
}

void IPhysXObjBase::ReleasePhysXObj(const bool RunTimeIsEnd)
{
	OnReleasePhysXObj(RunTimeIsEnd);
}

void IPhysXObjBase::ControlClothes(const bool IsClose)
{
	if (!IsActive())
		return;

	OnControlClothes(IsClose);
}

bool IPhysXObjBase::TickMove(float deltaTimeSec)
{
	if (!IsActive())
		return true;
	
	return OnTickMove(deltaTimeSec);
}

bool IPhysXObjBase::TickAnimation(const unsigned long deltaTime)
{
	if (ReadFlag(OBFI_GOTO_DESTROY))
	{
		DoDestroyTask();
		return true;
	}

	if (ReadFlag(OBFI_GOTO_SLEEP))
	{
		DoSleepTask();
		return true;
	}

	if (!IsActive())
		return true;

	if (ReadFlag(OBFI_IS_IN_RUNTIME) && (DRIVEN_BY_PURE_PHYSX == GetDrivenMode()))
	{
		if (ReadFlag(OBFI_HAS_BACKUP_POSE))
			SetPos(GetPos());
	}

	return OnTickAnimation(deltaTime);
}

void IPhysXObjBase::Render(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	if (!IsActive()) return;

	if (ReadFlag(OBF_DRAW_BINDING_BOX) && (0 != pWC))
	{
		A3DOBB obb;
		if (GetOBB(obb))
			pWC->AddOBB(obb, 0xffffff00);
		else
		{
			A3DAABB aabb;
			if (GetAABB(aabb))
				pWC->AddAABB(aabb, 0xffffff00);
		}
	}

	if (ReadFlag(OBF_DRAW_MODEL_AABB))
	{
		A3DWireCollector* pWCUsing = (0 != pWCTopZBuf)? pWCTopZBuf : pWC;
		if (0 != pWCUsing)
		{
			A3DAABB aabb;
			if (GetModelAABB(aabb))
			{
				const float MinVal = 0.001f;
				if ((MinVal > aabb.Extents.x) && (MinVal > aabb.Extents.y) && (MinVal > aabb.Extents.z))
				{
					aabb.Extents.Set(0.1f, 0.1f, 0.1f);
					aabb.CompleteMinsMaxs();
					pWC->AddAABB(aabb, 0xff77BB77);
				}
				else
					pWC->AddAABB(aabb, 0xff00ff00);
			}
		}
	}
	OnRender(viewport, pWC, isDebug);
	if (ReadFlag(OBF_DRAW_REF_FRAME))
	{
		if (0 != pWCTopZBuf)
			m_refFrame.Draw(*pWCTopZBuf);
		else if (0 != pWC)
			m_refFrame.Draw(*pWC);
	}
}

void IPhysXObjBase::ResetPose()
{
	if (!IsActive())
		return;
	
	if (!OnResetPose())
		return;

	ApplyPose();
}

A3DVECTOR3 IPhysXObjBase::GetNonRuntimePos() const
{
	if (ReadFlag(OBFI_HAS_BACKUP_POSE))
		return m_vBackUpPos;

	return m_vPos;
}

A3DVECTOR3 IPhysXObjBase::GetPos() const
{
	if (ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		A3DVECTOR3 pos;
		if (OnGetPos(pos))
			return pos;
	}

	return m_vPos; 
}

void IPhysXObjBase::SetPos(const A3DVECTOR3& vPos)
{
	m_refFrame.SetPos(vPos);
	if (IsActive())
		OnSetPos(vPos);

	m_vPos = vPos;
	if ((0 == m_pAssociate) || ReadFlag(OBF_ASSOCIATE_ALL_ISLOCKED))
		return;

	A3DVECTOR3 t = m_vPos + m_ascLocalPose.GetRow(3);
	m_pAssociate->SetPos(t);
}

void IPhysXObjBase::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	m_refFrame.SetDirAndUp(vDir, vUp);
	if (IsActive())
		OnSetDirAndUp(vDir, vUp);

	m_vDir = vDir;
	m_vUp  = vUp;
	if ((0 == m_pAssociate) || ReadFlag(OBF_ASSOCIATE_ALL_ISLOCKED))
		return;
	if (ReadFlag(OBF_ASSOCIATE_ROT_ISLOCKED))
		return;

	A3DMATRIX4 rot = m_ascLocalPose;
	rot.SetRow(3, A3DVECTOR3(0));
	A3DVECTOR3 d = m_vDir * rot; 
	A3DVECTOR3 u = m_vUp * rot; 
	m_pAssociate->SetDirAndUp(d, u);
}

void IPhysXObjBase::SetScale(const float& fScale)
{
	if (!IsActive())
		return;

	if (OnSetScale(fScale))
	{
		if (ReadFlag(OBFI_INITED_AABB))
			m_AABBRelativeCenter *= fScale / m_fScale;
		m_fScale = fScale;
		TickAnimation(0); // must update internal data 
	}
}

void IPhysXObjBase::SetRefFrameEditMode(const EditType et)
{
	OnSetRefFrameEditMode(et);
	m_refFrame.SetEditType(et); 
}

void IPhysXObjBase::SetDrivenMode(const DrivenMode dm)
{
	if (ReadFlag(OBF_DRIVENMODE_ISLOCKED))
		return;

	if (dm == m_eDrivenMode)
		return;

	OnChangeDrivenMode(dm);
	m_eDrivenMode = dm;
}

void IPhysXObjBase::UpdatePoseFromRefFrame()
{
	const EditType optType = m_refFrame.GetEditType();
	if (CCoordinateDirection::EDIT_TYPE_MOVE == optType)
	{
		SetPos(m_refFrame.GetPos());
	}
	else if (CCoordinateDirection::EDIT_TYPE_ROTATE == optType)
	{
		A3DVECTOR3 dir, up;
		m_refFrame.GetDirAndUp(dir, up);
		SetDirAndUp(dir, up);
	}
	else if (CCoordinateDirection::EDIT_TYPE_SCALE == optType)
	{
		SetScale(m_refFrame.GetScale());
	}
}

bool IPhysXObjBase::LoadDeserialize(CRender& render, NxStream& stream, const unsigned int nVersion, APhysXScene& aPhysXScene)
{
	if (nVersion < 0xCC000001)
		return false;

	if (nVersion >= 0xCC000001)
	{
		stream.readBuffer(&m_vPos, sizeof(A3DVECTOR3));
		stream.readBuffer(&m_vDir, sizeof(A3DVECTOR3));
		stream.readBuffer(&m_vUp,  sizeof(A3DVECTOR3));
	}

	if (nVersion >= 0xCC000002)
		stream.readBuffer(&m_fScale, sizeof(float));

	if (nVersion >= 0xCC000003)
		stream.readBuffer(&m_eDrivenMode, sizeof(DrivenMode));

	if (nVersion >= 0xCC00000C)
		stream.readBuffer(&m_eDMDefOnSim, sizeof(DrivenMode));

	if (nVersion >= 0xCC000004)
	{
		if (nVersion == 0xCC000004)
		{
			bool InitedAABB = false;
			stream.readBuffer(&InitedAABB, sizeof(bool));
		}
		else
		{
			stream.readBuffer(&m_flags, sizeof(int));
			if (nVersion >= 0xCC00000D)
				stream.readBuffer(&m_PeopleFlags, sizeof(DrivenMode));
		}
		stream.readBuffer(&m_AABBRelativeCenter, sizeof(A3DVECTOR3));
		stream.readBuffer(&m_OriginalExtents, sizeof(A3DVECTOR3));
	}

	if (nVersion >= 0xCC000006)
	{
		if (!OnLoadDeserialize(stream, nVersion))
			return false;
	}

	if (nVersion >= 0xCC000009)
	{
		stream.readBuffer(&m_CtrlGroup, sizeof(int));
		stream.readBuffer(&m_MotionDist, sizeof(float));
	}

	APhysXScene* pScene = 0;
	if (!ReadFlag(OBFI_INITED_AABB) || !ReadFlag(OBFI_INITED_PMFLAGS))
		pScene = &aPhysXScene;

	if (nVersion < 0xCC000008)
	{
		pScene = &aPhysXScene;
		ClearFlag(OBFI_INITED_PMFLAGS);
	}

	if (nVersion < 0xCC00000E)
	{
		pScene = &aPhysXScene;
		ClearFlag(OBFI_INITED_AABB);
	}

	if (nVersion == 0xCC00000D)
	{
		if (11 == m_PeopleFlags)
			m_PeopleFlags = OBPI_IS_NPC | OBPI_ACTION_WALK_AROUND;
		else
			m_PeopleFlags = OBPI_IS_NONE_PEOPLE;
	}

	if (nVersion <= 0xCC00000D)
		RaiseFlag(OBF_ENABLE_PHYSX_BODY);

	const int flagsBackUp = m_flags;
	m_strFilePath = APhysXSerializerBase::ReadString(&stream);

	const bool ToInitAABB = !ReadFlag(OBFI_INITED_AABB);
	RaiseFlag(OBFI_NEW_PEOPLE_INFO);
	RaiseFlag(OBFI_IS_IN_LOADING);
	m_ObjState = OBJECT_STATE_SLEEP;
	if (!WakeUp(render, pScene))
	{
		SetToEmpty();
		return false;
	}
	ClearFlag(OBFI_IS_IN_LOADING);

	m_refFrame.SetPos(m_vPos);
	m_refFrame.SetDirAndUp(m_vDir, m_vUp);
	if (ToInitAABB && (1.0f != m_fScale))
		m_AABBRelativeCenter *= m_fScale;

	if (0 == pScene)
	{
		const int flagsNow = GetPreservableFlags();
		assert(flagsBackUp == flagsNow);
	}
	else
	{
		assert(true == ReadFlag(OBFI_INITED_AABB));
		assert(true == ReadFlag(OBFI_INITED_PMFLAGS));
	}
	return true;
}

int IPhysXObjBase::GetPreservableFlags() const
{
	int SaveMask = 1 << 31;
	while (0 == (SaveMask & ALIAS_SAVE_SEPARATOR))
		SaveMask = SaveMask >> 1;

	int flag = m_flags & SaveMask;
	return flag;
}

bool IPhysXObjBase::SaveSerialize(NxStream& stream) const
{
	int flag = GetPreservableFlags();

	stream.storeBuffer(&m_vPos, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_vDir, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_vUp, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_fScale, sizeof(float));
	stream.storeBuffer(&m_eDrivenMode, sizeof(DrivenMode));
	stream.storeBuffer(&m_eDMDefOnSim, sizeof(DrivenMode));
	stream.storeBuffer(&flag, sizeof(int));
	stream.storeBuffer(&m_PeopleFlags, sizeof(int));
	stream.storeBuffer(&m_AABBRelativeCenter, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_OriginalExtents, sizeof(A3DVECTOR3));
	OnSaveSerialize(stream);
	stream.storeBuffer(&m_CtrlGroup, sizeof(int));
	stream.storeBuffer(&m_MotionDist, sizeof(float));
	APhysXSerializerBase::StoreString(&stream, m_strFilePath);
	return true;
}

bool IPhysXObjBase::IsActive() const
{
	if (OBJECT_STATE_ACTIVE == m_ObjState)
	{
		if (!ReadFlag(OBFI_HAS_ASYN_WORK))
			return true;
	}
	return false;
}

void IPhysXObjBase::GetGlobalPose(NxMat34& mat34) const
{
	A3DVECTOR3 vRight = CrossProduct(m_vUp, m_vDir);
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	mat.SetRow(0, vRight);
	mat.SetRow(1, m_vUp);
	mat.SetRow(2, m_vDir);
	mat.SetRow(3, m_vPos);
	APhysXConverter::A2N_Matrix44(mat, mat34);
}

bool IPhysXObjBase::GetOBB(A3DOBB& outOBB) const
{
	if (OBJECT_STATE_NULL == m_ObjState)
		return false;

	if (ReadFlag(OBFI_IS_IN_RUNTIME))
		return false;

	if (!ReadFlag(OBFI_INITED_AABB))
		return false;

	A3DMATRIX4 rot = m_refFrame.GetRotate();
	outOBB.Center = m_AABBRelativeCenter * rot + m_refFrame.GetPos();
	outOBB.Extents = m_OriginalExtents * m_fScale;
	outOBB.XAxis = m_refFrame.GetAxisVec(CCoordinateDirection::AXIS_X);
	outOBB.YAxis = m_refFrame.GetAxisVec(CCoordinateDirection::AXIS_Y);
	outOBB.ZAxis = m_refFrame.GetAxisVec(CCoordinateDirection::AXIS_Z);
	outOBB.CompleteExtAxis();
	return true;
}

bool IPhysXObjBase::GetAABB(A3DAABB& outAABB) const
{
	if (OBJECT_STATE_NULL == m_ObjState)
		return false; 

	if (ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (OnInitPhysXAABB(outAABB))
			return true;
	}
	
	if (!ReadFlag(OBFI_INITED_AABB))
		return false;

	A3DMATRIX4 rot = m_refFrame.GetRotate();
	A3DVECTOR3 newC = m_AABBRelativeCenter * rot + GetPos();

	NxMat34 mat;
	APhysXConverter::A2N_Matrix44(rot, mat);

	NxBounds3 aabb;
	aabb.boundsOfOBB(mat.M, APhysXConverter::A2N_Vector3(newC), APhysXConverter::A2N_Vector3(m_OriginalExtents * m_fScale));

	NxVec3 tc, te;
	aabb.getCenter(tc);
	aabb.getExtents(te);
	outAABB.Center  = APhysXConverter::N2A_Vector3(tc);
	outAABB.Extents = APhysXConverter::N2A_Vector3(te);
	outAABB.CompleteMinsMaxs();
	return true;
}

bool IPhysXObjBase::GetModelAABB(A3DAABB& outAABB) const
{
	if (OBJECT_STATE_NULL == m_ObjState)
		return false; 
	
    return OnGetModelAABB(outAABB);
}

bool IPhysXObjBase::OnGetSelectedAABB(A3DAABB& outAABB) const
{
	return OnInitPhysXAABB(outAABB);
}

void IPhysXObjBase::GetNxActorAABB(NxActor& actor, NxBounds3& outAABB) const
{
	outAABB.setEmpty();

	NxBounds3 temp;
	NxU32 nShape = actor.getNbShapes();
	NxShape* const* ppShape = actor.getShapes();

	for (NxU32 i = 0; i < nShape; ++i)
	{
		ppShape[i]->getWorldBounds(temp);
		outAABB.combine(temp);
	}
	outAABB.include(actor.getGlobalPosition());
}

bool IPhysXObjBase::OnInitPhysXAABB(A3DAABB& outAABB) const
{
	static const NxVec3 gZero(0, 0, 0);

	NxBounds3 aabb;
	bool rtn = GetPhysXRigidBodyAABB(aabb);

	outAABB.Mins = APhysXConverter::N2A_Vector3(aabb.min);
	outAABB.Maxs = APhysXConverter::N2A_Vector3(aabb.max);
	outAABB.AddVertex(GetPos());
	outAABB.CompleteCenterExts();
	
	return rtn;
}

void IPhysXObjBase::GetPhysXRigidBodyAABB(APhysXRigidBodyObject& RB, NxBounds3& outAABB) const
{
	outAABB.setEmpty();
	NxBounds3 temp;
	int nCount = RB.GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
	{
		NxActor* pActor = RB.GetNxActor(i);
		GetNxActorAABB(*pActor, temp);
		outAABB.combine(temp);
	}
}

bool IPhysXObjBase::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	if (!IsActive()) return false;
	
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
