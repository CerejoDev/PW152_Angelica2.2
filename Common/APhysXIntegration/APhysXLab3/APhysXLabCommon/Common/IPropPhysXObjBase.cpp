/*
 * FILE: IPropPhysXObjBase.cpp
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

const TCHAR* IPropPhysXObjBase::GetDrivenModeTextChinese(const DrivenMode dm)
{
	switch (dm)
	{
	case DRIVEN_BY_PURE_PHYSX:
		return _T("物理");
	case DRIVEN_BY_ANIMATION:
		return _T("动画");
	case DRIVEN_BY_PART_PHYSX:
		return _T("混合");
	}

	assert(!"Unknown DrivenMode!");
	return szEmpty;
}

const TCHAR* IPropPhysXObjBase::GetDrivenModeText(const DrivenMode dm)
{
	switch (dm)
	{
		case DRIVEN_BY_PURE_PHYSX:
			return _T("pure PhysX");
		case DRIVEN_BY_ANIMATION:
			return _T("animation");
		case DRIVEN_BY_PART_PHYSX:
			return _T("mixed");
	}

	assert(!"Unknown DrivenMode!");
	return szEmpty;
}

bool IPropPhysXObjBase::GetDrivenMode(const TCHAR* pStr, DrivenMode& outDM)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetDrivenModeText(DRIVEN_BY_PURE_PHYSX)))
	{
		outDM = DRIVEN_BY_PURE_PHYSX;
		return true;
	}
	if (0 == a_strcmp(pStr, GetDrivenModeText(DRIVEN_BY_ANIMATION)))
	{
		outDM = DRIVEN_BY_ANIMATION;
		return true;
	}
	if (0 == a_strcmp(pStr, GetDrivenModeText(DRIVEN_BY_PART_PHYSX)))
	{
		outDM = DRIVEN_BY_PART_PHYSX;
		return true;
	}

	assert(!"Unknown DrivenModeText!");
	return false;
}

IPropPhysXObjBase::IPropPhysXObjBase()
{
	OnSetToEmpty();
}

IPropPhysXObjBase::IPropPhysXObjBase(const IPropPhysXObjBase& rhs) : IPropObjBase(rhs)
{
	m_dmCurrent = rhs.m_dmCurrent;
	m_dmDefOnSim = rhs.m_dmDefOnSim;
	m_OriginalLocalHeight = rhs.m_OriginalLocalHeight;
	m_pHostObject->m_ObjOrigAABB = rhs.m_pHostObject->m_ObjOrigAABB;
	SetFilePathName(rhs.m_strFilePath);
}

IPropPhysXObjBase& IPropPhysXObjBase::operator= (const IPropPhysXObjBase& rhs)
{
	m_dmCurrent	 = rhs.m_dmCurrent;
	m_dmDefOnSim = rhs.m_dmDefOnSim;
	m_OriginalLocalHeight = rhs.m_OriginalLocalHeight;
	if ((0 != m_pHostObject) && (0 != rhs.m_pHostObject))
		m_pHostObject->m_ObjOrigAABB = rhs.m_pHostObject->m_ObjOrigAABB;
	if (m_strFilePath != rhs.m_strFilePath)
		SetFilePathName(rhs.m_strFilePath);
	IPropObjBase::operator=(rhs);
	if (0 != GetSelGroup())
		GetSelGroup()->RaiseUpdateMark(PID_ITEM_OnSimDVN);
	return *this;
}

void IPropPhysXObjBase::InstancePhysXPostNotify()
{
	m_Status.RaiseFlag(OBFI_INITED_APHYSX_FLAGS); 
}

void IPropPhysXObjBase::OnSetToEmpty()
{
	m_dmCurrent	 = DRIVEN_BY_ANIMATION;
	m_dmDefOnSim = DRIVEN_BY_PURE_PHYSX;
	m_OriginalLocalHeight = 0.0f;

	SetFilePathName(0);
	GetFlags().RaiseFlag(OBF_ENABLE_PHYSX_BODY);
}

void IPropPhysXObjBase::SetOriginalLocalHeight(const A3DAABB& aabb)
{
	A3DVECTOR3 pos = GetPos();
	m_OriginalLocalHeight = aabb.Maxs.y - pos.y;
	if (NxMath::equals(m_OriginalLocalHeight, 0, APHYSX_FLOAT_EPSILON)) m_OriginalLocalHeight = 0;
}

void IPropPhysXObjBase::SetFilePathName(const char* szFile)
{
	if (0 == szFile)
	{
		SetObjName(0);
		m_strFilePath.Empty();
	}
	else
	{
		SetObjName(_TAS2WC(szFile));
		af_GetRelativePath(szFile, m_strFilePath);
	}
}

void IPropPhysXObjBase::SetDrivenMode(const DrivenMode& dm, bool bSetDefOnSim)
{
	SetDrivenModeMyself(dm, bSetDefOnSim);
	if (0 != m_pHostObject)
		m_pHostObject->IterateChildren(TaskSetDrivenMode(dm, bSetDefOnSim));
}

void IPropPhysXObjBase::SetDrivenModeMyself(const DrivenMode& dm, bool bSetDefOnSim)
{
	if (bSetDefOnSim)
	{
		if (GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED_ONSIM))
		{
			if (!m_Status.ReadFlag(OBFI_ENFORCE_SETDRIVENMODE))
				return;
		}
	
		if (dm == m_dmDefOnSim)
			return;

		m_dmDefOnSim = dm;

		CPhysXObjSelGroup* pSelGroup = GetSelGroup();
		if (0 != pSelGroup)
			pSelGroup->RaiseUpdateMark(PID_ITEM_OnSimDVN);
		return;
	}

	if (GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED))
	{
		if (!m_Status.ReadFlag(OBFI_ENFORCE_SETDRIVENMODE))
			return;
	}
	if (GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED_ONSIM))
	{
		if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			if (!m_Status.ReadFlag(OBFI_ENFORCE_SETDRIVENMODE))
				return;
		}
	}

	if (dm == m_dmCurrent)
		return;

	if (0 != m_pHostObject)
	{
		IPhysXObjBase* pPhysXObj = static_cast<IPhysXObjBase*>(m_pHostObject);
		pPhysXObj->OnChangeDrivenMode(dm);
	}
	m_dmCurrent = dm;
}

IPropPhysXObjBase::DrivenMode IPropPhysXObjBase::GetDrivenMode(bool bGetDefOnSim) const
{
	if (bGetDefOnSim)
		return m_dmDefOnSim;

	return m_dmCurrent;
}

bool IPropPhysXObjBase::QueryDrivenMode(const DrivenMode& dm, bool bQDefOnSim) const
{
	if (bQDefOnSim)
		return dm == m_dmDefOnSim;

	return dm == m_dmCurrent;
}

void IPropPhysXObjBase::SyncPoseDataFromPhysX()
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		{
			A3DMATRIX4 matNew = GetPose();
			A3DMATRIX4 matOld;
			m_pHostObject->GetGPose(matOld);
			SetPose(matNew);
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					if (!NxMath::equals(matNew.m[i][j], matOld.m[i][j], 0.0001f))
						m_pHostObject->SetGPose(matNew);
				}
			}
		}
	}
}

bool IPropPhysXObjBase::EnableSetPoseAndScale() const
{
	if (!IPropObjBase::EnableSetPoseAndScale())
		return false;

	if (QueryDrivenMode(DRIVEN_BY_PURE_PHYSX))
		return false;

	return true;
}

void IPropPhysXObjBase::SetScalePostNotify(const float& oldScale)
{
	IPhysXObjBase* pPhysXObj = static_cast<IPhysXObjBase*>(m_pHostObject);
	pPhysXObj->OnTickAnimation(0); // must update internal data
}

bool IPropPhysXObjBase::SaveToFile(NxStream& nxStream)
{
	if (!IPropObjBase::SaveToFile(nxStream))
		return false;

	SaveVersion(&nxStream);
	nxStream.storeBuffer(&m_dmCurrent, sizeof(DrivenMode));
	nxStream.storeBuffer(&m_dmDefOnSim, sizeof(DrivenMode));
	NxVec3 center, ext;
	m_pHostObject->m_ObjOrigAABB.getCenter(center);
	m_pHostObject->m_ObjOrigAABB.getExtents(ext);
	nxStream.storeBuffer(&center, sizeof(NxVec3));
	nxStream.storeBuffer(&ext, sizeof(NxVec3));
	nxStream.storeBuffer(&m_OriginalLocalHeight, sizeof(float));
	APhysXSerializerBase::StoreString(&nxStream, m_strFilePath);
	return true;
}

bool IPropPhysXObjBase::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (!IPropObjBase::LoadFromFile(nxStream, outIsLowVersion))
		return false;

	if (m_oadCompatible.oadEnable)
	{
		m_dmCurrent = DrivenMode(m_oadCompatible.oadCurrentDM);
		m_dmDefOnSim = DrivenMode(m_oadCompatible.oadDefOnSimDM);
		m_OriginalLocalHeight = m_oadCompatible.oadLH;
		m_strFilePath = m_oadCompatible.oadFilePath;
		return true;
	}

	LoadVersion(&nxStream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVersion = true;

	if(m_StreamObjVer >= 0xAA000001)
	{
		nxStream.readBuffer(&m_dmCurrent, sizeof(DrivenMode));
		nxStream.readBuffer(&m_dmDefOnSim, sizeof(DrivenMode));
		NxVec3 center, extents;
		nxStream.readBuffer(&center, sizeof(NxVec3));
		nxStream.readBuffer(&extents, sizeof(NxVec3));
		nxStream.readBuffer(&m_OriginalLocalHeight, sizeof(float));
		m_strFilePath = APhysXSerializerBase::ReadString(&nxStream);
		m_pHostObject->m_ObjOrigAABB.setCenterExtents(center, extents);
	}
	return true;
}

bool IPropPhysXObjBase::OnSendToPropsGrid()
{
	bool bRtn = IPropObjBase::OnSendToPropsGrid();
	if (bRtn)
		GetSelGroup()->FillGroupDrivenBy(*this);
	return bRtn;
}

bool IPropPhysXObjBase::OnPropertyChanged(PropItem& prop)
{
	if (IPropObjBase::OnPropertyChanged(prop))
		return true;

	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_OnSimDVN == propValue)
	{
		DrivenMode dm;
		bool rtn = GetDrivenMode(_bstr_t(prop.GetValue()), dm);
		if (rtn)
			SetDrivenMode(dm, true);
		return rtn;
	}
	return false; 
}

void IPropPhysXObjBase::OnUpdatePropsGrid()
{
	IPropObjBase::OnUpdatePropsGrid();

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 == pSelGroup)
		return;
	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	const bool u1 = pSelGroup->ReadUpdateMark(PID_ITEM_File);
	const bool u2 = pSelGroup->ReadUpdateMark(PID_GROUP_DrivenBy);
	const bool u3 = pSelGroup->ReadUpdateMark(PID_ITEM_CurntDVN);
	if (u1 || u2 || u3)
	{
		assert(!"Shouldn't be changed or not supported yet!");
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_OnSimDVN))
		pSelGroup->ExecuteUpdateItem(PID_ITEM_OnSimDVN, GetDrivenModeText(GetDrivenMode(true)), bIsMerge);
}
