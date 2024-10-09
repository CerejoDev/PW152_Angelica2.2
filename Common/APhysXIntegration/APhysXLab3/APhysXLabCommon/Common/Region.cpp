/*
 * FILE: Region.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

float CRegion::gDefRadius = 3.0f;

const TCHAR* CRegion::GetShapeTypeText(const ShapeType st)
{
	switch (st)
	{
	case ST_BOX:
		return _T("Box");
	case ST_SPHERE:
		return _T("Sphere");
	case ST_CAPSULE:
		return _T("Capsule");
	}

	assert(!"Unknown ShapeType!");
	return szEmpty;
}

bool CRegion::GetShapeType(const TCHAR* pStr, ShapeType& outRT)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetShapeTypeText(ST_BOX)))
	{
		outRT = ST_BOX;
		return true;
	}
	if (0 == a_strcmp(pStr, GetShapeTypeText(ST_SPHERE)))
	{
		outRT = ST_SPHERE;
		return true;
	}
	if (0 == a_strcmp(pStr, GetShapeTypeText(ST_CAPSULE)))
	{
		outRT = ST_CAPSULE;
		return true;
	}

	assert(!"Unknown ShapeTypeText!");
	return false;
}

bool CRegion::SetDefRadius(float newR)
{
	if (newR < 0)
		return false;

	if (NxMath::equals(newR, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;

	gDefRadius = newR;
	return true;
}

CRegion::CRegion(int UID)
{
	CFlagMgr<FlagsInfo>& flags = GetFlags();
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_RTBEGIN);
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_RTEND);
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_ENTER);
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_LEAVE);
	int filter = ObjManager::OBJ_TYPEID_STATIC | ObjManager::OBJ_TYPEID_SKINMODEL | ObjManager::OBJ_TYPEID_ECMODEL;
	SetFilterMask(filter);
	m_pShapeImp = 0;
	SetShapeType(ST_BOX);

	assert(0 <= UID);
	InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_REGION, *this, UID);
	SetProperties(*this);
	SetObjName(_T("Region"));
	if (0 != m_pShapeImp)
	{
		SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
		NxBox obb;
		m_pShapeImp->GetScaledOBB(obb);
		NxMat34 mat34;
		GetGRot(mat34.M);
		mat34.t = GetGPos();
		obb.rotate(mat34, obb);
		NxBounds3 aabb;
		aabb.boundsOfOBB(obb.rot, obb.center, obb.extents);
		SetOriginalAABB(aabb);
		m_pShapeImp->SetScale(GetScale());
	}
}

CRegion::CRegion(const CRegion& rhs) : IPropObjBase(rhs)
{
	m_FilterMask = rhs.m_FilterMask;
	m_pShapeImp = 0;
	SetShapeType(rhs.GetShapeType());
	SetVolume(rhs.GetVolume());
}

CRegion::~CRegion()
{
	if (0 <= GetUID())
	{
		assert(false == IsObjAliveState());
	}
	delete m_pShapeImp;
	m_pShapeImp = 0;
}

CRegion& CRegion::operator= (const CRegion& rhs)
{
	IPropObjBase::operator=(rhs);

	m_FilterMask = rhs.m_FilterMask;
	SetShapeType(rhs.GetShapeType());
	SetVolume(rhs.GetVolume());
	return *this;
}

CRegion::ShapeType CRegion::GetShapeType() const
{
	return m_pShapeImp->GetShapeType();
}

void CRegion::SetShapeType(ShapeType st)
{
	if (0 != m_pShapeImp)
	{
		if (m_pShapeImp->GetShapeType() == st)
			return;
	}

	IShapeImp* m_NewShapeImp = 0;
	switch(st)
	{
	case ST_BOX:
		m_NewShapeImp = new ShapeBoxImp;
		break;
	case ST_SPHERE:
		m_NewShapeImp = new ShapeSphereImp;
		break;
	case ST_CAPSULE:
		m_NewShapeImp = new ShapeCapsuleImp;
		break;
	default:
		assert(!"Shouldn't be here! Debug it!");
		return;
	}

	if (0 == m_NewShapeImp)
		return;

	if (0 != m_pShapeImp)
	{
		NxVec3 vol(0.0f);
		m_pShapeImp->GetVolume(vol);
		m_pShapeImp->GetProperVolumeForNewType(st, vol);
		m_NewShapeImp->SetVolume(vol);
		m_NewShapeImp->SetScale(m_pShapeImp->GetScale());

		NxBounds3 aabb;
		if (GetOriginalAABB(aabb))
		{
			NxVec3 center;
			aabb.getCenter(center);
			NxVec3 ext(vol);
			if (ST_SPHERE == st)
				ext.y = ext.z = ext.x;
			if (ST_CAPSULE == st)
			{
				ext.z = ext.x;
				ext.y += ext.x * 2;
			}
			aabb.setCenterExtents(center, ext);
			SetOriginalAABB(aabb);
		}
	}

	delete m_pShapeImp;
	m_pShapeImp = m_NewShapeImp;
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_GROUP_ShapeVolume, true);
}

A3DVECTOR3 CRegion::GetVolume(bool withScaled) const
{
	NxVec3 vol(0.0f);
	m_pShapeImp->GetVolume(vol);
	if (withScaled)
		vol *= GetScale();
	return APhysXConverter::N2A_Vector3(vol);
}

bool CRegion::SetVolume(const A3DVECTOR3& newVolume, bool withScaled)
{
	A3DVECTOR3 volOld = GetVolume(withScaled);
	bool equX = NxMath::equals(volOld.x, newVolume.x, APHYSX_FLOAT_EPSILON);
	bool equY = NxMath::equals(volOld.y, newVolume.y, APHYSX_FLOAT_EPSILON);
	bool equZ = NxMath::equals(volOld.z, newVolume.z, APHYSX_FLOAT_EPSILON);
	if (equX && equY && equZ)
		return true;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		ShapeType st = GetShapeType();
		pSelGroup->RaiseUpdateMarkToShapeVolume(st);
	}

	NxVec3 vol(newVolume.x, newVolume.y, newVolume.z);
	if (withScaled)
	{
		float s = GetScale();
		if (!NxMath::equals(0.0f, s, APHYSX_FLOAT_EPSILON))
		{
			vol.x /= s;
			vol.y /= s;
			vol.z /= s;
		}
	}
	return m_pShapeImp->SetVolume(vol);
}

void CRegion::SetFilterMask(int filter)
{
	m_FilterMask = filter;
	m_FilterMask &= ~ObjManager::OBJ_TYPEID_REGION;
}

bool CRegion::TestOverlap(const NxBox& obb) const
{
	NxMat34 mat34;
	GetGRot(mat34.M);
	mat34.t = GetGPos();
	return m_pShapeImp->TestOverlap(obb, mat34);
}

bool CRegion::TestOverlap(const NxBounds3& aabb) const
{
	NxBox obb;
	aabb.getCenter(obb.center);
	aabb.getExtents(obb.extents);
	obb.rot.id();
	return TestOverlap(obb);
}

int CRegion::TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec) const
{
	NxMat34 mat34;
	GetGRot(mat34.M);
	mat34.t = GetGPos();
	return m_pShapeImp->TestOverlap(scene, outVec, mat34);
}

bool CRegion::TestOverlap(const IObjBase& obj) const
{
	const IObjBase* pObj = &obj;
	int nbHoldObjs = m_HoldOnObjs.GetSize();
	for (int i = 0; i < nbHoldObjs; ++i)
	{
		if (pObj == m_HoldOnObjs[i])
			return true;
	}
	return false;
}

IObjBase* CRegion::GetHoldOnObject(int idx)
{
	IObjBase* pRtnObj = 0;
	int nbHoldObjs = m_HoldOnObjs.GetSize();
	if (0 < nbHoldObjs)
	{
		if (0 <= idx)
		{
			if (idx < nbHoldObjs)
				pRtnObj = m_HoldOnObjs[idx];
		}
	}
	return pRtnObj;
}

void CRegion::UpdateHoldOnObjsEnter(const APtrArray<IObjBase*>& arrEnterObjs)
{
	IObjBase* pObj = 0;
	int nbObjs = arrEnterObjs.GetSize();
	int idxRtn = 0;
	for (int i = 0; i < nbObjs; ++i)
	{
		int idx = m_HoldOnObjs.GetSize();
		pObj = arrEnterObjs[i];
		idxRtn = m_HoldOnObjs.Add(pObj);
		pObj->GetMgrData()->SetRegionHoldOnIdx(*this, idxRtn);
	}
}

void CRegion::UpdateHoldOnObjsLeave(const APtrArray<IObjBase*>& arrLeaveObjs)
{
	IObjBase* pObj = 0;
	int nbObjs = arrLeaveObjs.GetSize();
	for (int i = 0; i < nbObjs; ++i)
	{
		pObj = arrLeaveObjs[i];
		int idx = pObj->GetMgrData()->GetRegionHoldOnIdx(*this);
		pObj->GetMgrData()->SetRegionHoldOnIdx(*this, -1);
		assert(0 <= idx);
		int nbHoldObjs = m_HoldOnObjs.GetSize();
		assert(idx < nbHoldObjs);
		if (idx < (nbHoldObjs - 1))
		{
			m_HoldOnObjs[idx] = m_HoldOnObjs[nbHoldObjs - 1];
			m_HoldOnObjs[idx]->GetMgrData()->SetRegionHoldOnIdx(*this, idx);
		}
		m_HoldOnObjs.RemoveTail();
	}
}

void CRegion::UpdateHoldOnObjsLeave(const APtrArray<IObjBase*>& arrKeepObjs, APtrArray<IObjBase*>& outLeaveObjs)
{
	IObjBase* pObj = 0;
	IObjBase* pTemp = 0;
	int idxNewHold = 0;
	int nbObjs = arrKeepObjs.GetSize();
	int nbHoldObjs = m_HoldOnObjs.GetSize();
	assert(nbObjs <= nbHoldObjs);
	for (int i = 0; i < nbObjs; ++i)
	{
		pObj = arrKeepObjs[i];
		int idx = pObj->GetMgrData()->GetRegionHoldOnIdx(*this);
		if (0 > idx)
		{
			assert(!"Shouldn't be here! Debug it!");
			continue;
		}
		if (idx >= nbHoldObjs)
		{
			assert(!"Shouldn't be here! Debug it!");
			continue;
		}
		assert(pObj == m_HoldOnObjs[idx]);
		assert(idxNewHold < nbHoldObjs);
		if (idxNewHold != idx)
		{
			pTemp = m_HoldOnObjs[idxNewHold];
			m_HoldOnObjs[idxNewHold] = m_HoldOnObjs[idx];
			m_HoldOnObjs[idxNewHold]->GetMgrData()->SetRegionHoldOnIdx(*this, idxNewHold);
			m_HoldOnObjs[idx] = pTemp;
			m_HoldOnObjs[idx]->GetMgrData()->SetRegionHoldOnIdx(*this, idx);
		}
		++idxNewHold;
	}

	for (int j = idxNewHold; j < nbHoldObjs; ++j)
	{
		pObj = m_HoldOnObjs[j];
		outLeaveObjs.Add(pObj);
		pObj->GetMgrData()->SetRegionHoldOnIdx(*this, -1);
	}
	if (idxNewHold < nbHoldObjs)
	{
		int nCount = nbHoldObjs - idxNewHold;
		m_HoldOnObjs.RemoveAt(idxNewHold, nCount);
	}
}

CRegion* CRegion::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_REGION, true, *m_pScene);
	CRegion* pObj = dynamic_cast<CRegion*>(pObjBase);
	if (0 != pObj)
		*pObj = *this;
	return pObj;
}

bool CRegion::GetObjAABB(NxBounds3& outObjAABB) const
{
	if (ST_BOX == m_pShapeImp->GetShapeType())
		return ApxObjBase::GetObjAABB(outObjAABB);

	NxMat34 mat34;
	GetGRot(mat34.M);
	mat34.t = GetGPos();
	m_pShapeImp->GetScaledAABB(outObjAABB, mat34);
	return true;
}

bool CRegion::SaveToFile(NxStream& nxStream)
{
	if (!IPropObjBase::SaveToFile(nxStream))
		return false;

	SaveVersion(&nxStream);
	nxStream.storeDword(GetShapeType());
	nxStream.storeBuffer(&GetVolume(), sizeof(A3DVECTOR3));
	return true;
}

bool CRegion::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (!IPropObjBase::LoadFromFile(nxStream, outIsLowVersion))
		return false;

	LoadVersion(&nxStream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVersion = true;

	if(m_StreamObjVer >= 0xAA000001)
	{
		ShapeType stNew = ShapeType(nxStream.readDword());
		A3DVECTOR3 vol;
		nxStream.readBuffer(&vol, sizeof(A3DVECTOR3));
		SetShapeType(stNew);
		SetVolume(vol);
	}
	return true;
}

CRegion* CRegion::CreateMyselfInstance(bool snapValues) const
{
	CRegion* pProp = new CRegion(*this);
	return pProp;
}

void CRegion::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropObjBase::EnterRuntimePreNotify(aPhysXScene);
	CRegion* pBK = dynamic_cast<CRegion*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void CRegion::LeaveRuntimePostNotify()
{
	IPropObjBase::LeaveRuntimePostNotify();
	CRegion* pBK = dynamic_cast<CRegion*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

void CRegion::OnTick(float dt)
{
	IObjBase::OnTick(dt);
	if (0 != m_pShapeImp)
		IObjBase::BuildExtraRenderData();
}

void CRegion::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
{
	if (IsRuntime())
	{
		if (!m_pScene->IsPhysXDebugRenderEnabled())
			return;
	}

	int dwTheColor = 0xff1095DE;
	if (0 != dwColor)
		dwTheColor = dwColor;

	NxMat34 mat34;
	GetGRot(mat34.M);
	mat34.t = GetGPos();
	m_pShapeImp->Draw(er, dwTheColor, mat34);
}

bool CRegion::OnSendToPropsGrid()
{
	bool bRtn = IPropObjBase::OnSendToPropsGrid();
	if (bRtn)
		GetSelGroup()->FillGroupShapeVolume(*this);
	return bRtn;
}

bool CRegion::OnPropertyChanged(PropItem& prop)
{
	if (IPropObjBase::OnPropertyChanged(prop))
		return true;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	SetSelGroup(0);
	bool bRtnValue = false;

	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_Shape == propValue)
	{
		ShapeType st;
		bRtnValue = GetShapeType(_bstr_t(prop.GetValue()), st);
		if (bRtnValue)
		{
			ShapeType oldST = GetShapeType();
			SetShapeType(st);
			pSelGroup->RaiseUpdateMarkToShapeVolume(oldST, true);
			pSelGroup->RaiseUpdateMarkToShapeVolume(st, false);
		}
	}
	else if (PID_SUBGROUP_Extent_xyz == propValue)
	{
		A3DVECTOR3 vol;
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vol);
		if (bRtnValue)
			SetVolume(vol, false);
	}
	else if ((PID_SGITEM_Extent_x == propValue) || (PID_ITEM_Radius == propValue))
	{
		A3DVECTOR3 vol = GetVolume(false);
		vol.x = prop.GetValue();
		SetVolume(vol, false);
		bRtnValue = true;
	}
	else if ((PID_SGITEM_Extent_y == propValue) || (PID_ITEM_Height == propValue))
	{
		A3DVECTOR3 vol = GetVolume(false);
		vol.y = prop.GetValue();
		SetVolume(vol, false);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Extent_z == propValue)
	{
		A3DVECTOR3 vol = GetVolume(false);
		vol.z = prop.GetValue();
		SetVolume(vol, false);
		bRtnValue = true;
	}
	SetSelGroup(pSelGroup);
	return bRtnValue;
}

void CRegion::OnUpdatePropsGrid()
{
	IPropObjBase::OnUpdatePropsGrid();

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 == pSelGroup)
		return;
	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	if (pSelGroup->ReadUpdateMark(PID_GROUP_ShapeVolume))
	{
		pSelGroup->FillGroupShapeVolume(*this);
		return;
	}

	A3DVECTOR3 vol = GetVolume(false);
	ShapeType st = GetShapeType();
	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Extent_xyz) || pSelGroup->ReadUpdateMark(PID_SGITEM_Extent_x)
		|| pSelGroup->ReadUpdateMark(PID_SGITEM_Extent_y) || pSelGroup->ReadUpdateMark(PID_SGITEM_Extent_z)
		|| pSelGroup->ReadUpdateMark(PID_ITEM_Radius) || pSelGroup->ReadUpdateMark(PID_ITEM_Height))
	{
		pSelGroup->ExecuteUpdateItemToShapeVolume(st, vol, bIsMerge);
	}
}

bool CRegion::OnGetPos(A3DVECTOR3& vOutPos) const
{
	vOutPos = GetPos(false);
	return true;
}

bool CRegion::OnGetDir(A3DVECTOR3& vOutDir) const
{
	vOutDir = GetDir(false);
	return true;
}

bool CRegion::OnGetUp(A3DVECTOR3& vOutUp) const
{
	vOutUp = GetUp(false);
	return true;
}

bool CRegion::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	matOutPose = GetPose(false);
	outWithScaled = false;
	return true;
}

bool CRegion::OnSetScale(const float& scale)
{
	if (0 != m_pShapeImp)
	{
		m_pShapeImp->SetScale(scale);
		return true;
	}
	return false;
}

bool CRegion::OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	NxMat34 mat;
	GetGRot(mat.M);
	mat.t = GetGPos();

	if (m_pShapeImp->RayTraceObj(ray, hitInfo, mat))
	{
		hitInfo.UserData = (void*)this;
		return true;
	}
	return false;
}