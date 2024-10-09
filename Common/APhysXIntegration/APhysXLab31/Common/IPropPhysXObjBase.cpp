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
	m_OriginalAABBLocalCenter = rhs.m_OriginalAABBLocalCenter;
	m_OriginalAABBExtents = rhs.m_OriginalAABBExtents;
	m_OriginalLocalHeight = rhs.m_OriginalLocalHeight;
	SetFilePathName(rhs.m_strFilePath);
}

IPropPhysXObjBase& IPropPhysXObjBase::operator= (const IPropPhysXObjBase& rhs)
{
	m_dmCurrent	 = rhs.m_dmCurrent;
	m_dmDefOnSim = rhs.m_dmDefOnSim;
	m_OriginalAABBLocalCenter = rhs.m_OriginalAABBLocalCenter;
	m_OriginalAABBExtents = rhs.m_OriginalAABBExtents;
	m_OriginalLocalHeight = rhs.m_OriginalLocalHeight;
	SetFilePathName(rhs.m_strFilePath);
	IPropObjBase::operator=(rhs);
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
	m_OriginalAABBLocalCenter.Set(0, 0, 0);
	m_OriginalAABBExtents.Set(0, 0, 0);
	m_OriginalLocalHeight = 0.0f;

	SetFilePathName(0);
	GetFlags().RaiseFlag(OBF_ENABLE_PHYSX_BODY);
}

void IPropPhysXObjBase::SetOriginalAABB(const A3DAABB& aabb)
{
	m_OriginalAABBExtents = aabb.Extents;
	m_OriginalAABBLocalCenter = aabb.Center - GetPos();
	if (NxMath::equals(m_OriginalAABBLocalCenter.x, 0, APHYSX_FLOAT_EPSILON)) m_OriginalAABBLocalCenter.x = 0;
	if (NxMath::equals(m_OriginalAABBLocalCenter.y, 0, APHYSX_FLOAT_EPSILON)) m_OriginalAABBLocalCenter.y = 0;
	if (NxMath::equals(m_OriginalAABBLocalCenter.z, 0, APHYSX_FLOAT_EPSILON)) m_OriginalAABBLocalCenter.z = 0;
	m_Status.RaiseFlag(OBFI_INITED_PHYSX_AABB);
}

void IPropPhysXObjBase::SetOriginalLocalHeight(const A3DAABB& aabb)
{
	A3DVECTOR3 pos = GetPos();
	m_OriginalLocalHeight = aabb.Maxs.y - pos.y;
	if (NxMath::equals(m_OriginalLocalHeight, 0, APHYSX_FLOAT_EPSILON)) m_OriginalLocalHeight = 0;
}

void IPropPhysXObjBase::SetFilePathName(const char* szFile)
{
	SetObjName(szFile);
	if (0 == szFile)
		m_strFilePath.Empty();
	else
		af_GetRelativePath(szFile, m_strFilePath);
}

void IPropPhysXObjBase::SetDrivenMode(const DrivenMode& dm, bool bSetDefOnSim)
{
	if (bSetDefOnSim)
	{
		if (dm == m_dmDefOnSim)
			return;

		m_dmDefOnSim = dm;

		CPhysXObjSelGroup* pSelGroup = GetSelGroup();
		if (0 != pSelGroup)
			pSelGroup->RaiseUpdateMark(PID_ITEM_OnSimDVN);
		return;
	}

//	if (ReadFlag(OBF_DRIVENMODE_ISLOCKED))
//		return;

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

void IPropPhysXObjBase::SyncPoseDataFromPhysX(const A3DVECTOR3& lastPos)
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		{
			// In ECM internal, only skin model's position was updated by PhysX data(although, all bones were updated correctly)
			// So we only update position value.
			A3DVECTOR3 posNow = GetPos();
			bool equX = NxMath::equals(lastPos.x, posNow.x, 0.01f);
			bool equY = NxMath::equals(lastPos.y, posNow.y, 0.01f);
			bool equZ = NxMath::equals(lastPos.z, posNow.z, 0.01f);
			if (!equX || !equY || !equZ)
				SetPos(posNow);
		}
	}
}

bool IPropPhysXObjBase::EnableSetPose() const
{
	if (!IPropObjBase::EnableSetPose())
		return false;

	if (QueryDrivenMode(DRIVEN_BY_PURE_PHYSX))
		return false;

	return true;
}

void IPropPhysXObjBase::SetScalePreNotify(const float& newScale)
{
	if (m_Status.ReadFlag(OBFI_INITED_PHYSX_AABB))
		m_OriginalAABBLocalCenter *= newScale / GetScale(true);
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
	nxStream.storeBuffer(&m_OriginalAABBLocalCenter, sizeof(A3DVECTOR3));
	nxStream.storeBuffer(&m_OriginalAABBExtents, sizeof(A3DVECTOR3));
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
		m_OriginalAABBLocalCenter = m_oadCompatible.oadLC;
		m_OriginalAABBExtents = m_oadCompatible.oadExt;
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
		nxStream.readBuffer(&m_OriginalAABBLocalCenter, sizeof(A3DVECTOR3));
		nxStream.readBuffer(&m_OriginalAABBExtents, sizeof(A3DVECTOR3));
		nxStream.readBuffer(&m_OriginalLocalHeight, sizeof(float));
		m_strFilePath = APhysXSerializerBase::ReadString(&nxStream);
	}
	return true;
}

bool IPropPhysXObjBase::OnSendToPropsGrid(CBCGPPropList& lstProp)
{
	bool bRtn = IPropObjBase::OnSendToPropsGrid(lstProp);
	if (bRtn)
		GetSelGroup()->FillGroupDrivenBy(*this);
	return bRtn;
}

bool IPropPhysXObjBase::OnPropertyChanged(const CBCGPProp& prop)
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

void IPropPhysXObjBase::OnUpdatePropsGrid(CBCGPPropList& lstProp)
{
	IPropObjBase::OnUpdatePropsGrid(lstProp);

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
