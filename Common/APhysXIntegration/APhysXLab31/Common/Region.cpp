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

float Region::gDefRadius = 3.0f;

const TCHAR* Region::GetShapeTypeText(const ShapeType st)
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

bool Region::GetShapeType(const TCHAR* pStr, ShapeType& outRT)
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

bool Region::SetDefRadius(float newR)
{
	if (newR < 0)
		return false;

	if (NxMath::equals(newR, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;

	gDefRadius = newR;
	return true;
}

Region::Region(int UID)
{
	CFlagMgr<FlagsInfo>& flags = GetFlags();
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_RTBEGIN);
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_RTEND);
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_ENTER);
	flags.RaiseFlag(OBF_ENABLE_EVENT_REGION_LEAVE);
	m_pShapeImp = 0;
	SetShapeType(ST_BOX);

	assert(0 < UID);
	InitTypeIDAndHostObject(RawObjMgr::OBJ_TYPEID_REGION, *this, UID);
	SetProperties(*this);
	SetObjName(_T("Region"));
	SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
}

Region::Region(const Region& rhs) : IPropObjBase(rhs)
{
	m_pShapeImp = 0;
	SetShapeType(rhs.GetShapeType());
	SetVolume(rhs.GetVolume());
}

Region::~Region()
{
	delete m_pShapeImp;
	m_pShapeImp = 0;
}

Region& Region::operator= (const Region& rhs)
{
	IPropObjBase::operator=(rhs);

	SetShapeType(rhs.GetShapeType());
	SetVolume(rhs.GetVolume());
	return *this;
}

Region::ShapeType Region::GetShapeType() const
{
	return m_pShapeImp->GetShapeType();
}

void Region::SetShapeType(ShapeType st)
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
		A3DVECTOR3 vol(0.0f);
		m_pShapeImp->GetVolume(vol);
		m_pShapeImp->GetProperVolumeForNewType(st, vol);
		m_NewShapeImp->SetVolume(vol);
	}

	delete m_pShapeImp;
	m_pShapeImp = m_NewShapeImp;
}

A3DVECTOR3 Region::GetVolume() const
{
	A3DVECTOR3 rtn(0.0f);
	m_pShapeImp->GetVolume(rtn);
	return rtn;
}

bool Region::SetVolume(const A3DVECTOR3& newVolume)
{
	return m_pShapeImp->SetVolume(newVolume);
}

bool Region::TestOverlap(const A3DOBB& obb) const
{
	float scale = GetScale();
	A3DMATRIX4 mat = GetPose();
	NxMat34 nxmat;
	APhysXConverter::A2N_Matrix44(mat, nxmat);

	NxBox nxobb;
	nxobb.center = APhysXConverter::A2N_Vector3(obb.Center);
	nxobb.extents = APhysXConverter::A2N_Vector3(obb.Extents);
	A3DMATRIX3 rot;
	rot.SetRow(0, obb.XAxis);
	rot.SetRow(1, obb.YAxis);
	rot.SetRow(2, obb.ZAxis);
	APhysXConverter::A2N_Matrix33(rot, nxobb.rot);

	return m_pShapeImp->TestOverlap(nxobb, nxmat, scale);
}

bool Region::TestOverlap(const A3DAABB& aabb) const
{
	A3DOBB obb;
	obb.Build(aabb);
	return TestOverlap(obb);
}

int Region::TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec) const
{
	float scale = GetScale();
	A3DMATRIX4 mat = GetPose();
	NxMat34 nxmat;
	APhysXConverter::A2N_Matrix44(mat, nxmat);
	return m_pShapeImp->TestOverlap(scene, outVec, nxmat, scale);
}

void Region::ToDie(const bool IsAsync)
{
	SetObjState(IPropObjBase::OBJECT_STATE_CORPSE);
}

bool Region::Revive(IEngineUtility& eu)
{
	if (QueryObjState(IPropObjBase::OBJECT_STATE_CORPSE))
		SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	return IsActive();
}

bool Region::GetOBB(A3DOBB& outOBB) const
{
return false;
}

bool Region::GetAABB(A3DAABB& outAABB) const
{
	return false;
}

bool Region::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	if (!IsActive()) return false;

	A3DMATRIX4 mat = GetPose();
	float scale = GetScale();
	if (m_pShapeImp->RayTraceObj(ray, hitInfo, mat, scale))
	{
		hitInfo.UserData = (void*)this;
		return true;
	}
	return false;
}

void Region::RenderExtraData(const IRenderUtility& renderUtil) const
{
	if (!IsActive())
		return;

	A3DWireCollector* pWC = renderUtil.GetWireCollector();
	if (0 == pWC)
		return;

	if (0 == m_pShapeImp)
		return;

	int dwColor = 0;
	if (!GetDrawBindingBoxInfo(dwColor))
		dwColor = 0xff1095DE;

	A3DMATRIX4 mat = GetPose();
	NxMat34 nxmat;
	APhysXConverter::A2N_Matrix44(mat, nxmat);
	float scale = GetScale();
	m_pShapeImp->Draw(*pWC, dwColor, nxmat, scale);
}

bool Region::SaveToFile(NxStream& nxStream)
{
	if (!IPropObjBase::SaveToFile(nxStream))
		return false;

	SaveVersion(&nxStream);
	nxStream.storeDword(GetShapeType());
	nxStream.storeBuffer(&GetVolume(), sizeof(A3DVECTOR3));
	return true;
}

bool Region::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
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

Region* Region::CreateMyselfInstance(bool snapValues) const
{
	Region* pProp = new Region(*this);
	return pProp;
}

bool Region::OnSendToPropsGrid(CBCGPPropList& lstProp)
{
	return true;
}

bool Region::OnPropertyChanged(const CBCGPProp& prop)
{
	return true;
}

void Region::OnUpdatePropsGrid(CBCGPPropList& lstProp)
{
}

bool Region::OnGetPos(A3DVECTOR3& vOutPos) const
{
	vOutPos = GetPos(false);
	return true;
}

bool Region::OnGetDir(A3DVECTOR3& vOutDir) const
{
	vOutDir = GetDir(false);
	return true;
}

bool Region::OnGetUp(A3DVECTOR3& vOutUp) const
{
	vOutUp = GetUp(false);
	return true;
}

bool Region::OnGetPose(A3DMATRIX4& matOutPose) const
{
	matOutPose = GetPose(false);
	return true;
}
