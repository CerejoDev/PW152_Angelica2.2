/*
 * FILE: IPropObjBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

RayHitOwner::RayHitOwner()
{
	m_pRH = 0;
	m_pt.x = m_pt.y = 0;
}

IObjBase* RayHitOwner::GetRayHit(POINT* poutPT) const
{
	if (0 != poutPT)
	{
		poutPT->x = m_pt.x;
		poutPT->y = m_pt.y;
	}
	return m_pRH;
}

void RayHitOwner::SetRayHit(IObjBase* pNewHit, const POINT* pPT)
{
	if (0 != pPT)
	{
		m_pt.x = pPT->x;
		m_pt.y = pPT->y;
	}
	if (m_pRH == pNewHit)
		return;

	IPropObjBase* pProp = 0;
	if (0 != m_pRH)
	{
		pProp = m_pRH->GetProperties();
		pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
	}

	if (0 != pNewHit)
	{
		pProp = pNewHit->GetProperties();
		pProp->GetFlags().RaiseFlag(OBF_DRAW_BINDING_BOX);
	}
	m_pRH = pNewHit;

	if (0 == m_pRH)
	{
		m_pt.x = 0;
		m_pt.y = 0;
	}
}

void RayHitOwner::OnDecorateObj(const IObjBase& obj)
{
	if (m_pRH == &obj)
		obj.GetProperties()->GetFlags().RaiseFlag(OBF_DRAW_BINDING_BOX);
	else
		obj.GetProperties()->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
}

void RayHitOwner::OnNotifySleep(const IObjBase& obj)
{
	if (m_pRH == &obj)
	{
		obj.GetProperties()->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
		m_pRH = 0;
	}
}

const TCHAR* IPropObjBase::szEmpty = _T("");
IRenderManager* IPropObjBase::pRenderManager = 0;
IObjStateReport* IPropObjBase::pStateReport = 0;

const TCHAR* IPropObjBase::GetObjStateText(const ObjState objState)
{
	switch (objState)
	{
	case OBJECT_STATE_NULL:
		return _T("Null");
	case OBJECT_STATE_ACTIVE:
		return _T("Actived");
	case OBJECT_STATE_SLEEP:
		return _T("Sleeping");
	case OBJECT_STATE_CORPSE:
		return _T("DeadCorpse");
	}

	assert(!"Unknown ObjState!");
	return szEmpty;
}

void IPropObjBase::SetRenderManager(IRenderManager* pRM)
{
	pRenderManager = pRM;
}

void IPropObjBase::SetStateReport(IObjStateReport* pSR)
{
	pStateReport = pSR;
}

IPropObjBase::IPropObjBase()
{
	m_pUserData = 0;
	m_pBackupProps = 0;
	m_pHostObject  = 0;
	m_nTypeID = ObjManager::OBJ_TYPEID_EMPTY;
	m_bIsUnrealShadow = true;
	SetToEmpty();
}

IPropObjBase::IPropObjBase(const IPropObjBase& rhs)
{
	m_pUserData = 0;
	m_pBackupProps = 0;
	m_pHostObject  = rhs.m_pHostObject;
	m_nTypeID = ObjManager::OBJ_TYPEID_EMPTY;
	m_bIsUnrealShadow = true;
	SetToEmpty();

	m_Status = rhs.m_Status;
	CopyDataForSnap(rhs);
	ObjState osback = m_ObjState;
	m_ObjState = rhs.m_ObjState;
	OnObjStateChange(osback);
}

IPropObjBase::~IPropObjBase()
{
	if (0 != m_pBackupProps)
	{
		m_pBackupProps->SetObjState(OBJECT_STATE_CORPSE);
		delete m_pBackupProps;
		m_pBackupProps = 0;
	}
}

IPropObjBase& IPropObjBase::operator= (const IPropObjBase& rhs)
{
	assert(false == QueryObjState(OBJECT_STATE_CORPSE));
	assert(false == rhs.QueryObjState(OBJECT_STATE_CORPSE));
	m_Status = rhs.m_Status;
	m_pSelGroup = rhs.m_pSelGroup;
	ObjState osback = m_ObjState;
	m_ObjState = rhs.m_ObjState;

	m_Flags = rhs.m_Flags;
	if (rhs.m_ObjIndex != m_ObjIndex)
	{
		if (-1 == rhs.m_ObjIndex)
		{
			// undo action, copy snap property
			CopyDataForSnap(rhs);
			m_pHostObject->OnSetScale(m_fScale);
			m_pHostObject->ApplyPose(false);
			OnObjStateChange(osback);
			if (0 != m_pSelGroup)
			{
				m_pSelGroup->RaiseUpdateMark(PID_ITEM_Scripts_Attached);
				m_pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Pos_xyz);
				m_pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Dir_xyz);
				m_pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Up_xyz);
				m_pSelGroup->RaiseUpdateMark(PID_ITEM_Scale);
			}
			return *this;
		}
		else
		{
			// clone action, copy backup property
			assert(m_nTypeID == rhs.m_nTypeID);
			if (0 != m_pBackupProps)
			{
				assert(0 != rhs.m_pBackupProps);
				*m_pBackupProps = *rhs.m_pBackupProps;
				m_pSelGroup = 0;
				if (OBJECT_STATE_ACTIVE == m_ObjState)
					m_ObjState = OBJECT_STATE_SLEEP;
			}
			else
			{
				assert(0 == rhs.m_pBackupProps);
			}
			m_Flags.ClearFlag(OBF_DRAW_BINDING_BOX);
		}
	}

	m_strLocalName = rhs.m_strLocalName;
	m_fScale = rhs.m_fScale;
	m_matPose = rhs.m_matPose;

	OnObjStateChange(osback);
	m_strScripts = rhs.m_strScripts;
	return *this;
}

void IPropObjBase::CopyDataForSnap(const IPropObjBase& rhs)
{
	m_strLocalName = rhs.m_strLocalName;
	m_pSelGroup = rhs.m_pSelGroup;
	m_fScale = rhs.m_fScale;
	m_matPose = rhs.m_matPose;
	m_Flags = rhs.m_Flags;
	m_strScripts = rhs.m_strScripts;
}

bool IPropObjBase::InitTypeIDAndHostObject(const ObjTypeID& type, IObjBase& hostObject, int UID)
{
	m_Status.RaiseFlag(OBFI_INITED_HOST_OBJECT);
	m_nTypeID	   = type;
	m_pHostObject  = &hostObject;
	InitHostObjectPost();
	m_ObjIndex = UID;
	m_strObjName.Format(_T("%d"), m_ObjIndex);
	m_strLocalName.Empty();
	m_bIsUnrealShadow = false;

	m_pBackupProps = CreateMyselfInstance(false);
	if (0 == m_pBackupProps)
		return false;

	m_pBackupProps->m_ObjIndex = m_ObjIndex;
	return true;
}

bool IPropObjBase::IsActive() const
{
	if (QueryObjState(OBJECT_STATE_ACTIVE))
	{
		if (!m_Status.ReadFlag(OBFI_HAS_ASYN_WORK))
			return true;
	}
	return false;
}

bool IPropObjBase::IsRuntime() const
{
	if (IsActive())
	{
		if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			return true;
	}
	return false;
}

bool IPropObjBase::GetDrawBindingBoxInfo(int& outColor) const
{
	bool bDrawBindingBox = false;
	if (m_Flags.ReadFlag(OBF_DRAW_BINDING_BOX))
	{
		bDrawBindingBox = true;
		outColor = 0xffffff00;
	}

	if (0 != m_pSelGroup)
	{
		bDrawBindingBox = m_pSelGroup->IsDrawItemEnable();
		if (bDrawBindingBox)
			outColor = 0xffffa820;
	}
	return bDrawBindingBox;
}

void IPropObjBase::SetObjName(const TCHAR* szFile)
{
	if (0 == szFile)
	{
		m_strObjName.Format(_T("%d"), m_ObjIndex);
	}
	else
	{
		AString strTitle;
		af_GetFileTitle(_TWC2AS(szFile), strTitle);
		int right = strTitle.Find(".");
		if (0 <= right)
			strTitle = strTitle.Left(right);
		m_strObjName.Format(_T("%s_%d"), _TAS2WC(strTitle), m_ObjIndex);
	}
	if (0 != pStateReport)
	{
		if (0 != m_pHostObject)
			pStateReport->OnNotifyRename(*m_pHostObject);
	}
}

bool IPropObjBase::SetObjLocalName(const TCHAR* szName)
{
	if ((0 == szName) || (0 == *szName))
	{
		if (!m_strLocalName.IsEmpty())
		{
			m_strLocalName.Empty();
			CPhysXObjSelGroup* pSelGroup = GetSelGroup();
			if (0 != pSelGroup)
				pSelGroup->RaiseUpdateMark(PID_ITEM_LocalName);
		}
		return true;
	}

	IObjBase* pParent = dynamic_cast<IObjBase*>(m_pHostObject->GetParent()); 
	if (0 != pParent)
	{
		IObjBase* pChild = pParent->GetChildByLocalName(szName);
		if (0 != pChild)
			return false;
		m_strLocalName = szName;
		CPhysXObjSelGroup* pSelGroup = GetSelGroup();
		if (0 != pSelGroup)
			pSelGroup->RaiseUpdateMark(PID_ITEM_LocalName);
	}
	return false;
}

void IPropObjBase::SetObjState(const ObjState& os)
{
	ObjState osBack = m_ObjState;
	m_ObjState = os;
	OnObjStateChange(osBack);
	if (OBJECT_STATE_NULL == os)
		SetToEmpty();
}

bool IPropObjBase::IsObjAliveState() const
{
	if (OBJECT_STATE_NULL == m_ObjState)
		return false;
	if (OBJECT_STATE_CORPSE == m_ObjState)
		return false;
	return true;
}

void IPropObjBase::OnObjStateChange(const ObjState& oldState) const
{
	if (m_bIsUnrealShadow)
		return;

	if (0 != pRenderManager)
	{
		IRenderable* pIR = m_pHostObject->GetRenderable();
		ExtraRenderable* pER = m_pHostObject->GetExtraRenderable();
		assert(0 != pER);
		if (QueryObjState(OBJECT_STATE_ACTIVE))
		{
			if (OBJECT_STATE_ACTIVE != oldState)
			{
				if (0 != pIR)
					pIR->Register(pRenderManager);
				pER->Register(pRenderManager);
			}
		}		
		else
		{
			if (OBJECT_STATE_ACTIVE == oldState)
			{
				if (0 != pIR)
					pIR->UnRegister(pRenderManager);
				pER->UnRegister(pRenderManager);
			}
		}			
	}

	if (0 != pStateReport)
	{
		if (oldState != m_ObjState)
		{
			if (OBJECT_STATE_NULL == oldState)
			{
				if (QueryObjState(OBJECT_STATE_ACTIVE))
					pStateReport->OnNotifyBorn(*m_pHostObject);
				else if (QueryObjState(OBJECT_STATE_SLEEP))
					pStateReport->OnNotifyBorn(*m_pHostObject);
			}
			else if (OBJECT_STATE_SLEEP == oldState)
			{
				if (QueryObjState(OBJECT_STATE_ACTIVE))
					pStateReport->OnNotifyWakeUp(*m_pHostObject);
				else if (QueryObjState(OBJECT_STATE_CORPSE))
					pStateReport->OnNotifyDie(*m_pHostObject);
			}
			else if (OBJECT_STATE_ACTIVE == oldState)
			{
				if (QueryObjState(OBJECT_STATE_SLEEP))
					pStateReport->OnNotifySleep(*m_pHostObject);
				else if (QueryObjState(OBJECT_STATE_CORPSE))
					pStateReport->OnNotifyDie(*m_pHostObject);
			}
			else if (OBJECT_STATE_CORPSE == oldState)
			{
				if (IsObjAliveState())
					pStateReport->OnNotifyBorn(*m_pHostObject);
			}
		}
	}
}

void IPropObjBase::SetToEmpty()
{
	bool val = m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT);
	m_Status.ClearAllFlags();
	if (val)
		m_Status.RaiseFlag(OBFI_INITED_HOST_OBJECT);

	SetSelGroup(0);
	ObjState osBack = m_ObjState;
	m_ObjState = OBJECT_STATE_NULL;
	OnObjStateChange(osBack);
	SetObjIndex(-1);
	m_strLocalName.Empty();

	m_fScale = 1.0f;
	m_matPose.Identity();

	m_Flags.ClearAllFlags();
	OnSetToEmpty();
	m_strScripts.clear();
}

void IPropObjBase::SetObjIndex(int idx)
{
	m_ObjIndex = idx;
	if (0 != m_pBackupProps)
		m_pBackupProps->m_ObjIndex = m_ObjIndex;
}

void IPropObjBase::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
}

void IPropObjBase::LeaveRuntimePostNotify()
{
	assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
	m_pHostObject->SetGScale(m_pBackupProps->m_fScale);
	m_pHostObject->SetGPose(m_pBackupProps->m_matPose);
}

A3DVECTOR3 IPropObjBase::GetPos(bool bRuntimePos) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
		if (bRuntimePos)
		{
			A3DVECTOR3 pos;
			bool bSuccess = m_pHostObject->OnGetPos(pos);
			assert(true == bSuccess);
			if (bSuccess)
				return pos;
		}
		else
		{
			assert(false == m_pBackupProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
			return m_pBackupProps->GetPos();
		}
	}

	return m_matPose.GetRow(3); 
}

bool IPropObjBase::EnableSetPoseAndScale() const
{
	if (!IsActive())
		return false;

	assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
	return true;
}

void IPropObjBase::SetPos(const A3DVECTOR3& vPos)
{
	A3DVECTOR3 pos = m_matPose.GetRow(3);
	bool equX = NxMath::equals(pos.x, vPos.x, APHYSX_FLOAT_EPSILON);
	bool equY = NxMath::equals(pos.y, vPos.y, APHYSX_FLOAT_EPSILON);
	bool equZ = NxMath::equals(pos.z, vPos.z, APHYSX_FLOAT_EPSILON);
	if (equX && equY && equZ)
		return;

	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		m_matPose.SetRow(3, vPos);

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		if (!equX)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Pos_x);
		if (!equY)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Pos_y);
		if (!equZ)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Pos_z);
	}
}

A3DVECTOR3 IPropObjBase::GetDir(bool bRuntimeDir) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
		if (bRuntimeDir)
		{
			A3DVECTOR3 dir;
			bool bSuccess = m_pHostObject->OnGetDir(dir);
			assert(true == bSuccess);
			if (bSuccess)
				return dir;
		}
		else
		{
			assert(false == m_pBackupProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
			return m_pBackupProps->GetDir();
		}
	}

	return m_matPose.GetRow(2);
}

A3DVECTOR3 IPropObjBase::GetUp(bool bRuntimeUp) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
		if (bRuntimeUp)
		{
			A3DVECTOR3 up;
			bool bSuccess = m_pHostObject->OnGetUp(up);
			assert(true == bSuccess);
			if (bSuccess)
				return up;
		}
		else
		{
			assert(false == m_pBackupProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
			return m_pBackupProps->GetUp();
		}
	}

	return m_matPose.GetRow(1);
}

void IPropObjBase::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	A3DVECTOR3 dir = m_matPose.GetRow(2);
	bool equDX = NxMath::equals(dir.x, vDir.x, APHYSX_FLOAT_EPSILON);
	bool equDY = NxMath::equals(dir.y, vDir.y, APHYSX_FLOAT_EPSILON);
	bool equDZ = NxMath::equals(dir.z, vDir.z, APHYSX_FLOAT_EPSILON);
	A3DVECTOR3 up = m_matPose.GetRow(1);
	bool equUX = NxMath::equals(up.x, vUp.x, APHYSX_FLOAT_EPSILON);
	bool equUY = NxMath::equals(up.y, vUp.y, APHYSX_FLOAT_EPSILON);
	bool equUZ = NxMath::equals(up.z, vUp.z, APHYSX_FLOAT_EPSILON);
	if (equDX && equDY && equDZ && equUX && equUY && equUZ)
		return;

	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		A3DVECTOR3 vRight = CrossProduct(vUp, vDir);
		m_matPose.SetRow(0, vRight);
		m_matPose.SetRow(1, vUp);
		m_matPose.SetRow(2, vDir);
	}

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		if (!equDX)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Dir_x);
		if (!equDY)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Dir_y);
		if (!equDZ)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Dir_z);
		if (!equUX)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Up_x);
		if (!equUY)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Up_y);
		if (!equUZ)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Up_z);
	}
}

A3DMATRIX4 IPropObjBase::GetPose(bool bRuntimePose) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
		if (bRuntimePose)
		{
			bool bWithScaled = false;
			A3DMATRIX4 pose;
			bool bSuccess = m_pHostObject->OnGetPose(pose, bWithScaled);
			assert(true == bSuccess);
			if (bSuccess)
			{
				if (bWithScaled)
				{
					float scale = GetScale(bRuntimePose);
					if (!NxMath::equals(1.0f, scale, APHYSX_FLOAT_EPSILON))
					{
						if (!NxMath::equals(0.0f, scale, APHYSX_FLOAT_EPSILON))
						{
							float cof = 1.0f / scale;
							pose.m[0][0] *= cof;
							pose.m[0][1] *= cof;
							pose.m[0][2] *= cof;
							pose.m[1][0] *= cof;
							pose.m[1][1] *= cof;
							pose.m[1][2] *= cof;
							pose.m[2][0] *= cof;
							pose.m[2][1] *= cof;
							pose.m[2][2] *= cof;
						}
					}
				}
				return pose; 
			}
		}
		else
		{
			assert(false == m_pBackupProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
			return m_pBackupProps->GetPose();
		}
	}

	return m_matPose;
}

void IPropObjBase::SetPose(const A3DMATRIX4& mat44)
{
	bool bIsEqual = true;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (!NxMath::equals(mat44.m[i][j], m_matPose.m[i][j], APHYSX_FLOAT_EPSILON))
			{
				bIsEqual = false;
				break;
			}
		}
		if (!bIsEqual)
			break;
	}

	if (bIsEqual)
		return;

	A3DVECTOR3 up = m_matPose.GetRow(1);
	A3DVECTOR3 dir = m_matPose.GetRow(2);
	A3DVECTOR3 pos = m_matPose.GetRow(3);

	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		m_matPose = mat44;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Pos_x);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Pos_y);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Pos_z);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Dir_x);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Dir_y);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Dir_z);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Up_x);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Up_y);
		pSelGroup->RaiseUpdateMark(PID_SGITEM_Up_z);
	}
}

float IPropObjBase::GetScale(bool bRuntimeScale) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (bRuntimeScale)
			return m_fScale;

		assert(false == m_pBackupProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME));
		return m_pBackupProps->GetScale(false);
	}
	return m_fScale; 
}

void IPropObjBase::SetScale(const float& fScale)
{
	if (!IsActive())
		return;

	if (NxMath::equals(m_fScale, fScale, APHYSX_SCALE_EPSILON))
		return;

	assert(true == m_Status.ReadFlag(OBFI_INITED_HOST_OBJECT));
	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		float oldValue = m_fScale;
		m_fScale = fScale;
		SetScalePostNotify(oldValue);
	}
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_Scale);
}

int IPropObjBase::GetScriptCount() const
{
	return m_strScripts.size();
}

const AString& IPropObjBase::GetScriptName(int index) const
{
	assert(index < (int)m_strScripts.size());
	return m_strScripts[index];
}

void IPropObjBase::SetScriptName(int index, const AString& scriptName)
{
	assert(index < (int)m_strScripts.size());
	m_strScripts[index] = scriptName;
}

AString IPropObjBase::GetScriptNames() const
{
	AString str = "";
	int i;
	for (i = 0; i < (int)m_strScripts.size() - 1; i++)
	{
		str += m_strScripts[i] + ";";
	}
	if (i < (int)m_strScripts.size())
		str += m_strScripts[i];

	return str;
}

void IPropObjBase::SetScriptNames(const char* pScriptNames)
{
	AString scriptNames(pScriptNames);
	m_strScripts.clear();

	int start = 0;
	int nIndex = scriptNames.Find(';');
	while(nIndex != -1)
	{
		if (nIndex <= start)
			break;

		AString str = scriptNames.Mid(start, nIndex - start);
		if (!str.IsEmpty() && str != ";")
			m_strScripts.push_back(str);
		start = nIndex + 1;
		nIndex = scriptNames.Find(';', start);
	}
	if (start < scriptNames.GetLength())
	{
		AString str = scriptNames.Mid(start);
		if (!str.IsEmpty() && str != ";")
			m_strScripts.push_back(str);
	}
}

bool IPropObjBase::SaveToFile(NxStream& nxStream)
{
	SaveVersion(&nxStream);
	nxStream.storeDword(m_ObjIndex);
	APhysXSerializerBase::StoreString(&nxStream, _TWC2AS(m_strObjName));
	APhysXSerializerBase::StoreString(&nxStream, _TWC2AS(m_strLocalName));

	nxStream.storeFloat(m_fScale);
	nxStream.storeBuffer(&m_matPose, sizeof(A3DMATRIX4));
	SaveFlags(nxStream);

	//save script info
	nxStream.storeDword(m_strScripts.size());
	for (size_t i = 0; i < m_strScripts.size(); i++)
	{
		APhysXSerializerBase::StoreString(&nxStream, m_strScripts[i]);
	}
	return true;
}

bool IPropObjBase::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	LoadVersion(&nxStream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVersion = true;

	if(LoadOldArchive(nxStream))
	{
		if (CheckPoseAxis())
			outIsLowVersion = true;
		return true;
	}

	if(m_StreamObjVer >= 0xAA000002)
	{
		int idx = nxStream.readDword();
		const char* pStr = APhysXSerializerBase::ReadString(&nxStream);
		if (0 != pStr)
			m_strObjName = _TAS2WC(pStr);
		else
			m_strObjName.Empty();
		if (m_Status.ReadFlag(OBFI_KEEP_OBJECT_INDEX))
		{
			int nLen = m_strObjName.GetLength();
			int i = nLen - 1;
			while (_T('_') != m_strObjName[i])
				--i;
			if (i != nLen - 1)
				m_strObjName = m_strObjName.Left(i);
			m_strObjName.Format(_T("%s_%d"), m_strObjName, m_ObjIndex);
		}
		else
			SetObjIndex(idx);

		m_strLocalName.Empty();
		if (m_StreamObjVer >= 0xAA000004)
		{
			const char* pStrLocal = APhysXSerializerBase::ReadString(&nxStream);
			if (0 != pStrLocal)
				m_strLocalName = _TAS2WC(pStrLocal);
		}

		m_fScale = nxStream.readFloat();
		nxStream.readBuffer(&m_matPose, sizeof(A3DMATRIX4));
		LoadFlags(nxStream);
		m_pHostObject->SetGPose(m_matPose);
		m_pHostObject->SetGScale(m_fScale);
		if (0 != pStateReport)
		{
			if (0 != m_pHostObject)
			{
				pStateReport->OnNotifyRename(*m_pHostObject);
				pStateReport->OnUpdateStateRedraw();
			}
		}
	}
	if (m_StreamObjVer >= 0xAA000003)
	{
		int nScript = nxStream.readDword();
		for (int i = 0; i < nScript; i++)
		{
			m_strScripts.push_back(APhysXSerializerBase::ReadString(&nxStream));
		}		
	}

	if (CheckPoseAxis())
		outIsLowVersion = true;
	return true;
}

bool IPropObjBase::LoadOldArchive(NxStream& nxStream)
{
	if(m_StreamObjVer >= 0xAA000002)
	{
		m_oadCompatible.oadEnable = false;
		return false;
	}

	m_oadCompatible.oadEnable = true;
	int idx = nxStream.readDword();
	nxStream.readBuffer(&m_oadCompatible.oadCurrentDM, sizeof(int));
	nxStream.readBuffer(&m_oadCompatible.oadDefOnSimDM, sizeof(int));
	NxVec3 center, extents;
	nxStream.readBuffer(&center, sizeof(NxVec3));
	nxStream.readBuffer(&extents, sizeof(NxVec3));
	nxStream.readBuffer(&m_oadCompatible.oadLH, sizeof(float));

	m_oadCompatible.oadFilePath = APhysXSerializerBase::ReadString(&nxStream);
	const char* pStr = APhysXSerializerBase::ReadString(&nxStream);
	if (0 != pStr)
		m_strObjName = _TAS2WC(pStr);
	else
		m_strObjName.Empty();
	if (m_Status.ReadFlag(OBFI_KEEP_OBJECT_INDEX))
	{
		int nLen = m_strObjName.GetLength();
		int i = nLen - 1;
		while (_T('_') != m_strObjName[i])
			--i;
		if (i != nLen - 1)
			m_strObjName = m_strObjName.Left(i);
		m_strObjName.Format(_T("%s_%d"), m_strObjName, m_ObjIndex);
	}
	else
		SetObjIndex(idx);

	m_fScale = nxStream.readFloat();
	nxStream.readBuffer(&m_matPose, sizeof(A3DMATRIX4));
	LoadFlags(nxStream);

	A3DVECTOR3 vUp = m_matPose.GetRow(1);
	A3DVECTOR3 vDir = m_matPose.GetRow(2);
	A3DVECTOR3 vRight = CrossProduct(vUp, vDir);
	m_matPose.SetRow(0, vRight);

	if (!NxMath::equals(1.0f, m_fScale, APHYSX_FLOAT_EPSILON))
	{
		center.x /= m_fScale;
		center.y /= m_fScale; 
		center.z /= m_fScale; 
	}
	m_pHostObject->m_ObjOrigAABB.setCenterExtents(center, extents);
	m_pHostObject->SetGPose(m_matPose);
	m_pHostObject->SetGScale(m_fScale);
	return true;
}

bool IPropObjBase::CheckPoseAxis()
{
	A3DVECTOR3 vRight = m_matPose.GetRow(0);
	A3DVECTOR3 vUp = m_matPose.GetRow(1);
	A3DVECTOR3 vDir = m_matPose.GetRow(2);
	A3DVECTOR3 vNR = CrossProduct(vUp, vDir);
	bool equDX = NxMath::equals(vRight.x, vNR.x, 0.001f);
	bool equDY = NxMath::equals(vRight.y, vNR.y, 0.001f);
	bool equDZ = NxMath::equals(vRight.z, vNR.z, 0.001f);
	if (!equDX || !equDY || !equDZ)
	{
		m_matPose.SetRow(0, vNR);
		return true;
	}
	return false;
}

int	IPropObjBase::GetSaveMask(int EndBitEnum) const
{
	int SaveMask = 0;
	int val = 1 << 0;
	while (0 == (val & EndBitEnum))
	{
		SaveMask |= val;
		val = val << 1;
	}
	return SaveMask;
}

void IPropObjBase::SaveFlags(NxStream& nxStream)
{
	int mask  = GetSaveMask(SAVE_SEPARATOR_OBFI_ALIAS);
	int value = m_Status.GetValue();
	int flag  = value & mask;
	nxStream.storeDword(flag);

	mask  = GetSaveMask(SAVE_SEPARATOR_OBF_ALIAS);
	value = m_Flags.GetValue();
	flag  = value & mask;
	nxStream.storeDword(flag);
}

void IPropObjBase::LoadFlags(NxStream& nxStream)
{
	int flags = nxStream.readDword();
	m_Status.SetValue(flags);
	flags = nxStream.readDword();
	m_Flags.SetValue(flags);
}

bool IPropObjBase::OnSendToPropsGrid()
{
	if (QueryObjState(OBJECT_STATE_NULL))
		return false;
	if (QueryObjState(OBJECT_STATE_CORPSE))
		return false;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	pSelGroup->FillGroupBasic(*this);
	pSelGroup->FillGroupPoseScale(*this);
	return true;
}

bool IPropObjBase::OnPropertyChanged(PropItem& prop)
{
	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_LocalName == propValue)
	{
 		IObjBase* pParent = dynamic_cast<IObjBase*>(m_pHostObject->GetParent());
		if (0 != pParent)
		{
			_bstr_t localName = prop.GetValue();
			TCHAR* pLN = (TCHAR*)localName;
			IObjBase* pChild = pParent->GetChildByLocalName(pLN);
			if (0 == pChild)
				m_pHostObject->GetProperties()->SetObjLocalName(pLN);
		}
		return true;
	}
	if (PID_SUBGROUP_Pos_xyz == propValue)
	{
		A3DVECTOR3 pos;
		bool rtn = CPhysXObjSelGroup::ExtractVector(prop, pos);
		if (rtn)
			m_pHostObject->SetLPos(pos);
		return rtn;
	}
	else if (PID_SGITEM_Pos_x == propValue)
	{
		NxVec3 pos = m_pHostObject->GetLPos();
		pos.x = prop.GetValue();
		m_pHostObject->SetLPos(pos);
		return true;
	}
	else if (PID_SGITEM_Pos_y == propValue)
	{
		NxVec3 pos = m_pHostObject->GetLPos();
		pos.y = prop.GetValue();
		m_pHostObject->SetLPos(pos);
		return true;
	}
	else if (PID_SGITEM_Pos_z == propValue)
	{
		NxVec3 pos = m_pHostObject->GetLPos();
		pos.z = prop.GetValue();
		m_pHostObject->SetLPos(pos);
		return true;
	}
	else if (PID_ITEM_Scale == propValue)
	{
		m_pHostObject->SetLScale(prop.GetValue());
		return true;
	}
	else if (PID_ITEM_Scripts_Attached == propValue)
	{
		SetScriptNames((char*)(_bstr_t)(prop.GetValue()));
		return true;
	}
	return false; 
}

void IPropObjBase::OnUpdatePropsGrid()
{
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	if (0 == pSelGroup)
		return;
	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	const bool u1 = pSelGroup->ReadUpdateMark(PID_GROUP_Basic);
	const bool u2 = pSelGroup->ReadUpdateMark(PID_ITEM_Name);
	const bool u3 = pSelGroup->ReadUpdateMark(PID_ITEM_Type);
	const bool u4 = pSelGroup->ReadUpdateMark(PID_ITEM_State);
	const bool u5 = pSelGroup->ReadUpdateMark(PID_GROUP_PoseScale);
	if (u1 || u2 || u3 || u4 || u5)
	{
		assert(!"Shouldn't be changed or not supported yet!");
	}

    const char* pLocalName = m_pHostObject->GetProperties()->GetObjLocalName_cstr();
	if (pSelGroup->ReadUpdateMark(PID_GROUP_Basic))
	{
		pSelGroup->ExecuteUpdateItem(PID_ITEM_LocalName, pLocalName, bIsMerge);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_ITEM_LocalName))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_LocalName, pLocalName, bIsMerge);
	}

	A3DVECTOR3 pos = GetPos();
	if (0 != m_pHostObject->GetParent())
		pos = APhysXConverter::N2A_Vector3(m_pHostObject->GetLPos());
	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Pos_xyz))
	{
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Pos_x, pos.x, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Pos_y, pos.y, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Pos_z, pos.z, bIsMerge);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Pos_x))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Pos_x, pos.x, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Pos_y))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Pos_y, pos.y, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Pos_z))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Pos_z, pos.z, bIsMerge);
	}
	
	A3DVECTOR3 dir = GetDir();
	A3DVECTOR3 up = GetUp();
	if (0 != m_pHostObject->GetParent())
		m_pHostObject->GetLDirAndUp(dir, up);
	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Dir_xyz))
	{
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Dir_x, dir.x, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Dir_y, dir.y, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Dir_z, dir.z, bIsMerge);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Dir_x))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Dir_x, dir.x, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Dir_y))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Dir_y, dir.y, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Dir_z))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Dir_z, dir.z, bIsMerge);
	}
	
	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Up_xyz))
	{
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Up_x, up.x, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Up_y, up.y, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Up_z, up.z, bIsMerge);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Up_x))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Up_x, up.x, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Up_y))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Up_y, up.y, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Up_z))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Up_z, up.z, bIsMerge);
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_Scale))
	{
		float scale = GetScale();
		if (0 != m_pHostObject->GetParent())
			scale = m_pHostObject->GetLScale();
		pSelGroup->ExecuteUpdateItem(PID_ITEM_Scale, scale, bIsMerge);
	}
}
