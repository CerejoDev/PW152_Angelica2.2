/*
 * FILE: ObjDummy.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/07/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

CDummy::CDummy(int UID)
{
	assert(0 <= UID);
	InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_DUMMY, *this, UID);
	SetProperties(*this);
	SetObjName(_T("Dummy"));

	m_aabb.setCenterExtents(NxVec3(0.0f), NxVec3(0.3f));
	SetOriginalAABB(m_aabb);
	SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
}

CDummy& CDummy::operator= (const CDummy& rhs)
{
	IPropObjBase::operator=(rhs);
	return *this;
}

CDummy* CDummy::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_DUMMY, true, *m_pScene);
	CDummy* pObj = dynamic_cast<CDummy*>(pObjBase);
	if (0 != pObj)
		*pObj = *this;
	return pObj;
}

bool CDummy::SaveToFile(NxStream& nxStream)
{
	if (IPropObjBase::SaveToFile(nxStream))
	{
		SaveVersion(&nxStream);
		return true;
	}
	return false;
}

bool CDummy::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (IPropObjBase::LoadFromFile(nxStream, outIsLowVersion))
	{
		LoadVersion(&nxStream);
		if(m_StreamObjVer < GetVersion())
			outIsLowVersion = true;
		return true;
	}
	return false;
}

CDummy* CDummy::CreateMyselfInstance(bool snapValues) const
{
	CDummy* pProp = new CDummy(*this);
	return pProp;
}

void CDummy::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropObjBase::EnterRuntimePreNotify(aPhysXScene);
	CDummy* pBK = dynamic_cast<CDummy*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void CDummy::LeaveRuntimePostNotify()
{
	IPropObjBase::LeaveRuntimePostNotify();
	CDummy* pBK = dynamic_cast<CDummy*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

void CDummy::OnTick(float dt)
{
	IObjBase::OnTick(dt);
	IObjBase::BuildExtraRenderData();
}

void CDummy::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
{
	if (IsRuntime())
	{
		if (!m_pScene->IsPhysXDebugRenderEnabled())
			return;
	}

	NxBox nxOBB;
	if (GetOBB(nxOBB))
	{
		A3DOBB obb;
		obb.Center = APhysXConverter::N2A_Vector3(nxOBB.center);
		obb.XAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(0));
		obb.YAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(1));
		obb.ZAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(2));
		if (0 == dwColor)
		{
			dwColor = 0xff2065EE;
			NxVec3 exts;
			m_aabb.getExtents(exts);
			obb.Extents = APhysXConverter::N2A_Vector3(exts);
		}
		else
		{
			obb.Extents = APhysXConverter::N2A_Vector3(nxOBB.extents);
		}
		obb.CompleteExtAxis();
		er.AddOBB(obb, dwColor);
	}

	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		er.AddSphere(APhysXConverter::N2A_Vector3(GetGPos()), 0.1f, 0xffff0000);
}

bool CDummy::OnSendToPropsGrid()
{
	return IPropObjBase::OnSendToPropsGrid();
}

bool CDummy::OnPropertyChanged(PropItem& prop)
{
	return IPropObjBase::OnPropertyChanged(prop);
}

void CDummy::OnUpdatePropsGrid()
{
	IPropObjBase::OnUpdatePropsGrid();
}

bool CDummy::OnGetPos(A3DVECTOR3& vOutPos) const
{
	vOutPos = GetPos(false);
	return true; 
}

bool CDummy::OnGetDir(A3DVECTOR3& vOutDir) const
{
	vOutDir = GetDir(false);
	return true; 
}

bool CDummy::OnGetUp(A3DVECTOR3& vOutUp) const
{
	vOutUp = GetUp(false);
	return true; 
}

bool CDummy::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	matOutPose = GetPose(false);
	outWithScaled = false;
	return true; 
}

bool CDummy::OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	NxMat34 mat;
	GetGRot(mat.M);
	mat.t = GetGPos();
	NxVec3 exts;
	m_aabb.getExtents(exts);
	exts *= GetScale(); 

	PhysOBB rtOBB;
	rtOBB.Set(APhysXConverter::N2A_Vector3(mat.M.getColumn(0)), 
		APhysXConverter::N2A_Vector3(mat.M.getColumn(1)), 
		APhysXConverter::N2A_Vector3(mat.M.getColumn(2)), 
		APhysXConverter::N2A_Vector3(mat.t), 
		APhysXConverter::N2A_Vector3(exts)
		);

	if (::RayTraceOBB(ray, rtOBB, hitInfo))
	{
		hitInfo.UserData = (void*)this;
		return true;
	}
	return false;
}

