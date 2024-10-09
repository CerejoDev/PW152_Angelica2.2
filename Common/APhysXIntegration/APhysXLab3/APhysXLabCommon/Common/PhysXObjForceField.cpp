/*
 * FILE: PhysXObjForceField.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2012/07/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

const TCHAR* CPhysXObjForceField::GetForceFieldTypeText(const APhysXObjectType ffType)
{
	switch(ffType)
	{
	case APX_OBJTYPE_FORCEFIELD_WIND:
		return _T("Wind");
	case APX_OBJTYPE_FORCEFIELD_VORTEX:
		return _T("Vortex");
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
		return _T("Explosion");
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
		return _T("Gravitation");
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
		return _T("Buyancy");
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
		return _T("Vacuum");
	case APX_OBJTYPE_FORCEFIELD_BOSS:
		return _T("BossAura");
	case APX_OBJTYPE_FORCEFIELD:
		return _T("Custom");
	}
	assert(!"Unknown Force Field Type!");
	return szEmpty;
}

bool CPhysXObjForceField::GetForceFieldType(const TCHAR* pStr, APhysXObjectType& outFFType)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_WIND)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_WIND;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_VORTEX)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_VORTEX;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_EXPLOSION)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_EXPLOSION;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_GRAVITATION)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_GRAVITATION;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_BUOYANCY)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_BUOYANCY;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_VACUUM)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_VACUUM;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_BOSS)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD_BOSS;
		return true;
	}
	if (0 == a_strcmp(pStr, GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD)))
	{
		outFFType = APX_OBJTYPE_FORCEFIELD;
		return true;
	}

	assert(!"Unknown ForceField type text!");
	return false;
}

const TCHAR* CPhysXObjForceField::GetFFCoordonateText(const NxForceFieldCoordinates ffc)
{
	switch(ffc)
	{
	case NX_FFC_CARTESIAN:
		return _T("CARTESIAN");
	case NX_FFC_SPHERICAL:
		return _T("SPHERICAL");
	case NX_FFC_CYLINDRICAL:
		return _T("CYLINDRICAL");
	case NX_FFC_TOROIDAL:
		return _T("TOROIDAL");
	}
	assert(!"Unknown FF Coordinate Type!");
	return szEmpty;
}

bool CPhysXObjForceField::GetFFCoordonate(const TCHAR* pStr, NxForceFieldCoordinates& outFFC)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetFFCoordonateText(NX_FFC_CARTESIAN)))
	{
		outFFC = NX_FFC_CARTESIAN;
		return true;
	}
	if (0 == a_strcmp(pStr, GetFFCoordonateText(NX_FFC_SPHERICAL)))
	{
		outFFC = NX_FFC_SPHERICAL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetFFCoordonateText(NX_FFC_CYLINDRICAL)))
	{
		outFFC = NX_FFC_CYLINDRICAL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetFFCoordonateText(NX_FFC_TOROIDAL)))
	{
		outFFC = NX_FFC_TOROIDAL;
		return true;
	}
	assert(!"Unknown FF Coordinate Text!");
	return false;
}

const TCHAR* CPhysXObjForceField::GetFFScaleText(const NxForceFieldType ffst)
{
	switch(ffst)
	{
	case NX_FF_TYPE_OTHER:
		return _T("OTHER");
	case NX_FF_TYPE_GRAVITATIONAL:
		return _T("GRAVITATIONAL");
	case NX_FF_TYPE_NO_INTERACTION:
		return _T("NO_INTERACTION");
	}
	assert(!"Unknown FF Scale Type!");
	return szEmpty;
}

bool CPhysXObjForceField::GetFFScale(const TCHAR* pStr, NxForceFieldType& outFFST)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetFFScaleText(NX_FF_TYPE_GRAVITATIONAL)))
	{
		outFFST = NX_FF_TYPE_GRAVITATIONAL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetFFScaleText(NX_FF_TYPE_OTHER)))
	{
		outFFST = NX_FF_TYPE_OTHER;
		return true;
	}
	if (0 == a_strcmp(pStr, GetFFScaleText(NX_FF_TYPE_NO_INTERACTION)))
	{
		outFFST = NX_FF_TYPE_NO_INTERACTION;
		return true;
	}
	assert(!"Unknown FF Scale Text!");
	return false;
}

bool CPhysXObjForceField::GetShapeInfo(const APhysXForceFieldObjectDesc& ffDesc, CRegion::ShapeType& outST, A3DVECTOR3* poutVol)
{
	const APhysXShapeDescManager& sdm = ffDesc.mIncludeShapes;
	int nCount = sdm.GetNum();
	if (0 >= nCount)
		return false;

	bool rtn = false;
	APhysXShapeDesc* pDesc = sdm.GetPhysXShapeDesc(0);
	switch(pDesc->GetShapeType())
	{
	case APX_SHAPETYPE_BOX:
		{
			rtn = true;
			outST = CRegion::ST_BOX;
			if (0 != poutVol)
			{
				APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
				poutVol->x = pBox->mDimensions.x;
				poutVol->y = pBox->mDimensions.y;
				poutVol->z = pBox->mDimensions.z;
			}
		}
		break;
	case APX_SHAPETYPE_SPHERE:
		{
			rtn = true;
			outST = CRegion::ST_SPHERE;
			if (0 != poutVol)
			{
				APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
				poutVol->x = pSphere->mRadius;
				poutVol->y = poutVol->z = 0;
			}
		}
		break;
	case APX_SHAPETYPE_CAPSULE:
		{
			rtn = true;
			outST = CRegion::ST_CAPSULE;
			if (0 != poutVol)
			{
				APhysXCapsuleShapeDesc* pCapsule = static_cast<APhysXCapsuleShapeDesc*>(pDesc);
				poutVol->x = pCapsule->mRadius;
				poutVol->y = pCapsule->mHeight;
				poutVol->z = 0;
			}
		}
		break;
	default:
		assert(!"Unknown FF Shape Type!");
	}
	return rtn;
}

bool CPhysXObjForceField::SetShapeInfo(const CRegion::ShapeType& st, APhysXForceFieldObjectDesc& ffDesc, NxVec3* pVol)
{
	APhysXShapeDesc* pShapeDesc = 0;
	APhysXBoxShapeDesc boxDesc;
	APhysXSphereShapeDesc sphereDesc;
	APhysXCapsuleShapeDesc capsuleDesc;
	switch(st)
	{
	case CRegion::ST_BOX:
		if (APX_OBJTYPE_FORCEFIELD_EXPLOSION != ffDesc.GetObjType())
		{
			if (0 != pVol)
				boxDesc.mDimensions = *pVol;
			pShapeDesc = &boxDesc;
		}
		break;
	case CRegion::ST_SPHERE:
		if (0 != pVol)
			sphereDesc.mRadius = pVol->x;
		pShapeDesc = &sphereDesc;
		break;
	case CRegion::ST_CAPSULE:
		if (APX_OBJTYPE_FORCEFIELD_EXPLOSION != ffDesc.GetObjType())
		{
			if (0 != pVol)
			{
				capsuleDesc.mRadius = pVol->x;
				capsuleDesc.mHeight = pVol->y;
			}
			pShapeDesc = &capsuleDesc;
		}
		break;
	default:
		assert(!"Unknown FF Shape Type!");
	}

	if (0 != pShapeDesc)
	{
		ffDesc.mIncludeShapes.Release();
		ffDesc.mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
		return true;
	}
	return false;
}

CPhysXObjForceField::CPhysXObjForceField(int UID, APhysXObjectType FFType)
{
	m_bEnableDraw = false;
	m_DrawColor = 0;
	m_pAFFDesc	= 0;
	m_pAFFObj	= 0;
	m_pNxFF		= 0;

	if (!IsForceFieldType(FFType))
	{
		assert("Wrong force field type input! Using default value: APX_OBJTYPE_FORCEFIELD!");
		FFType = APX_OBJTYPE_FORCEFIELD;
	}
	APhysXObjectDesc* pDesc = m_objDescMgr.CreateAndPushObjectDesc(FFType);
	m_pAFFDesc = static_cast<APhysXForceFieldObjectDesc*>(pDesc);
	if (0 != m_pAFFDesc)
	{
		APhysXBoxShapeDesc boxDesc;
		m_pAFFDesc->mIncludeShapes.CreateAndPushShapeDesc(&boxDesc);
		if (APX_OBJTYPE_FORCEFIELD_VACUUM == FFType)
		{
			NxVec3 g;
			GetScene()->GetAPhysXScene()->GetNxScene()->getGravity(g);
			m_pAFFDesc->mFFLKernelParameters.mConstant = g * -1;
		}
	}

	SetDefaultOBB();
	if (0 <= UID)
	{
		InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_FORCEFIELD, *this, UID);
		SetProperties(*this);
		SetObjName(_T("ForceField"));
		SetRenderable(m_FFRender);

		NxBounds3 nxAABB;
		nxAABB.setCenterExtents(APhysXConverter::A2N_Vector3(m_EditorOBB.Center), APhysXConverter::A2N_Vector3(m_EditorOBB.Extents));
		SetOriginalAABB(nxAABB);
	}
}

CPhysXObjForceField::CPhysXObjForceField(const CPhysXObjForceField& rhs) : IPropPhysXObjBase(rhs)
{
	assert(0 == rhs.m_pAFFObj);
	assert(0 == rhs.m_pNxFF);
	m_bEnableDraw = false;
	m_DrawColor = rhs.m_DrawColor;
	CopyAPhysXObjectDescManager(rhs.m_objDescMgr);
}

CPhysXObjForceField::~CPhysXObjForceField()
{
	if (0 <= GetUID())
	{
		assert(false == IsObjAliveState());
	}
	assert(0 == m_pAFFObj);
	assert(0 == m_pNxFF);
	if (0 < m_objDescMgr.GetNum())
	{
		m_objDescMgr.Release();
		m_pAFFDesc = 0;
	}
}

CPhysXObjForceField& CPhysXObjForceField::operator= (const CPhysXObjForceField& rhs)
{
	IPropPhysXObjBase::operator=(rhs);
	assert(0 == rhs.m_pAFFObj);
	assert(0 == rhs.m_pNxFF);
	CopyAPhysXObjectDescManager(rhs.m_objDescMgr);
	return *this;
}

void CPhysXObjForceField::CopyAPhysXObjectDescManager(const APhysXObjectDescManager& rhs)
{
	APhysXObjectType ot;
	APhysXObjectDesc* pDesc = 0;
	m_objDescMgr.Release();
	m_pAFFDesc = 0;
	int n = rhs.GetNum();
	for (int i = 0; i < n; ++i)
	{
		pDesc = rhs.GetPhysXObjectDesc(i);
		ot = pDesc->GetObjType();
		m_pAFFDesc = static_cast<APhysXForceFieldObjectDesc*>(m_objDescMgr.CreateAndPushObjectDesc(ot, pDesc->GetName()));
		switch(ot)
		{
		case APX_OBJTYPE_FORCEFIELD_WIND:
			{
				APhysXWindForceFieldObjectDesc* pD = static_cast<APhysXWindForceFieldObjectDesc*>(pDesc);
				APhysXWindForceFieldObjectDesc* pMyD = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD_VORTEX:
			{
				APhysXVortexForceFieldObjectDesc* pD = static_cast<APhysXVortexForceFieldObjectDesc*>(pDesc);
				APhysXVortexForceFieldObjectDesc* pMyD = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
			{
				APhysXExplosionForceFieldObjectDesc* pD = static_cast<APhysXExplosionForceFieldObjectDesc*>(pDesc);
				APhysXExplosionForceFieldObjectDesc* pMyD = static_cast<APhysXExplosionForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
			{
				APhysXGravitationForceFieldObjectDesc* pD = static_cast<APhysXGravitationForceFieldObjectDesc*>(pDesc);
				APhysXGravitationForceFieldObjectDesc* pMyD = static_cast<APhysXGravitationForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
			{
				APhysXBuoyancyForceFieldObjectDesc* pD = static_cast<APhysXBuoyancyForceFieldObjectDesc*>(pDesc);
				APhysXBuoyancyForceFieldObjectDesc* pMyD = static_cast<APhysXBuoyancyForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD_VACUUM:
			{
				APhysXVacuumForceFieldObjectDesc* pD = static_cast<APhysXVacuumForceFieldObjectDesc*>(pDesc);
				APhysXVacuumForceFieldObjectDesc* pMyD = static_cast<APhysXVacuumForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD_BOSS:
			{
				APhysXBossForceFieldObjectDesc* pD = static_cast<APhysXBossForceFieldObjectDesc*>(pDesc);
				APhysXBossForceFieldObjectDesc* pMyD = static_cast<APhysXBossForceFieldObjectDesc*>(m_pAFFDesc);
				*pMyD = *pD;
			}
			break;
		case APX_OBJTYPE_FORCEFIELD:
			{
				APhysXForceFieldObjectDesc* pD = static_cast<APhysXForceFieldObjectDesc*>(pDesc);
				*m_pAFFDesc = *pD;
			}
			break;
		}
	}
}

int CPhysXObjForceField::GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor) const
{
	return 0;
}

bool CPhysXObjForceField::SaveToFile(NxStream& nxStream)
{
	if (IPropPhysXObjBase::SaveToFile(nxStream))
	{
		SaveVersion(&nxStream);
		m_objDescMgr.Save(&nxStream);
		return true;
	}
	return false;
}

bool CPhysXObjForceField::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (IPropPhysXObjBase::LoadFromFile(nxStream, outIsLowVersion))
	{
		m_objDescMgr.Release();
		m_pAFFDesc = 0;
		LoadVersion(&nxStream);
		if(m_StreamObjVer >= 0xAA000001)
		{
			m_objDescMgr.Load(&nxStream, 0);
			int n = m_objDescMgr.GetNum();
			if (0 < n)
				m_pAFFDesc = static_cast<APhysXForceFieldObjectDesc*>(m_objDescMgr.GetPhysXObjectDesc(n - 1));
		}
		m_FFRender.SetModelPtr(this);
		if(m_StreamObjVer < GetVersion())
			outIsLowVersion = true;
		return true;
	}
	return false;
}

bool CPhysXObjForceField::OnSendToPropsGrid()
{
	bool bRtn = IPropObjBase::OnSendToPropsGrid();
	if (bRtn)
		GetSelGroup()->FillAllFFProps(*this);
	return bRtn;
}

bool CPhysXObjForceField::OnPropertyChanged(PropItem& prop)
{
	if (IPropObjBase::OnPropertyChanged(prop))
		return true;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	SetSelGroup(0);
	bool bRtnValue = false;

	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_FF_Type == propValue)
	{
		APhysXObjectType newFFT;
		bRtnValue = GetForceFieldType(_bstr_t(prop.GetValue()), newFFT);
		if (bRtnValue)
		{
			APhysXObjectType oldFFT = m_pAFFDesc->GetObjType();
			if (oldFFT != newFFT)
			{
				APhysXForceFieldObjectDesc* pClone = static_cast<APhysXForceFieldObjectDesc*>(m_pAFFDesc->Clone());
				m_objDescMgr.Release();
				m_pAFFDesc = static_cast<APhysXForceFieldObjectDesc*>(m_objDescMgr.CreateAndPushObjectDesc(newFFT));
				// only copy shapes, other parameters keep default value!
				m_pAFFDesc->mIncludeShapes = pClone->mIncludeShapes;
				PropID oldIdx = CPhysXObjSelGroup::GetForceFieldSpecialGroupPropID(oldFFT);
				if (PID_START_BOUND != oldIdx)
					pSelGroup->RaiseUpdateMark(oldIdx, true);
				PropID newIdx = CPhysXObjSelGroup::GetForceFieldSpecialGroupPropID(newFFT);
				if (PID_START_BOUND != newIdx)
					pSelGroup->RaiseUpdateMark(newIdx);
				delete pClone;
				if (PID_GROUP_FF_Explosion == newIdx)
				{
					CRegion::ShapeType st;
					if (GetShapeInfo(*m_pAFFDesc, st))
					{
						if (CRegion::ST_SPHERE != st)
						{
							SetShapeInfo(CRegion::ST_SPHERE, *m_pAFFDesc);
							pSelGroup->RaiseUpdateMarkToShapeVolume(st, true);
							pSelGroup->RaiseUpdateMarkToShapeVolume(CRegion::ST_SPHERE, false);
							pSelGroup->RaiseUpdateMark(PID_ITEM_Shape, false);
						}
					}
					pSelGroup->EnablePropItem(PID_ITEM_Shape, false);
				}
				else
				{
					pSelGroup->EnablePropItem(PID_ITEM_Shape, true);
				}
				if (APX_OBJTYPE_FORCEFIELD_VACUUM == newFFT)
				{
					NxVec3 g;
					GetScene()->GetAPhysXScene()->GetNxScene()->getGravity(g);
					m_pAFFDesc->mFFLKernelParameters.mConstant = g * -1;
				}
			}
			bRtnValue = true;
		}
	}
	else if (PID_ITEM_Coordinate == propValue)
	{
		NxForceFieldCoordinates ffc;
		bRtnValue = GetFFCoordonate(_bstr_t(prop.GetValue()), ffc);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFCoordinates = ffc;
			bRtnValue = true;
		}
	}
	else if (PID_ITEM_Shape == propValue)
	{
		CRegion::ShapeType st;
		bRtnValue = CRegion::GetShapeType(_bstr_t(prop.GetValue()), st);
		if (bRtnValue)
		{
			CRegion::ShapeType oldST;
			if (GetShapeInfo(*m_pAFFDesc, oldST))
			{
				if (oldST != st)
				{
					if (SetShapeInfo(st, *m_pAFFDesc))
					{
						pSelGroup->RaiseUpdateMarkToShapeVolume(oldST, true);
						pSelGroup->RaiseUpdateMarkToShapeVolume(st, false);
					}
				}
			}
			else
			{
				if (SetShapeInfo(st, *m_pAFFDesc))
				{
					pSelGroup->RaiseUpdateMarkToShapeVolume(CRegion::ST_BOX, true);
					pSelGroup->RaiseUpdateMarkToShapeVolume(st, false);
				}
			}
			bRtnValue = true;
		}
	}
	else if (PID_SUBGROUP_Extent_xyz == propValue)
	{
		A3DVECTOR3 vol;
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vol);
		if (bRtnValue)
		{
			APhysXShapeDesc* pDesc = m_pAFFDesc->mIncludeShapes.GetPhysXShapeDesc(0);
			if (APX_SHAPETYPE_BOX == pDesc->GetShapeType())
			{
				APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
				pBox->mDimensions.x = vol.x;
				pBox->mDimensions.y = vol.y;
				pBox->mDimensions.z = vol.z;
			}
			bRtnValue = true;
		}
	}
	else if ((PID_SGITEM_Extent_x == propValue) || (PID_ITEM_Radius == propValue))
	{
		APhysXShapeDesc* pDesc = m_pAFFDesc->mIncludeShapes.GetPhysXShapeDesc(0);
		if (APX_SHAPETYPE_BOX == pDesc->GetShapeType())
		{
			APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
			pBox->mDimensions.x = prop.GetValue();
		}
		else if (APX_SHAPETYPE_SPHERE)
		{
			APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
			pSphere->mRadius = prop.GetValue();
		}
		else if (APX_SHAPETYPE_CAPSULE)
		{
			APhysXCapsuleShapeDesc* pCap = static_cast<APhysXCapsuleShapeDesc*>(pDesc);
			pCap->mRadius = prop.GetValue();
		}
		bRtnValue = true;
	}
	else if ((PID_SGITEM_Extent_y == propValue) || (PID_ITEM_Height == propValue))
	{
		APhysXShapeDesc* pDesc = m_pAFFDesc->mIncludeShapes.GetPhysXShapeDesc(0);
		if (APX_SHAPETYPE_BOX == pDesc->GetShapeType())
		{
			APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
			pBox->mDimensions.y = prop.GetValue();
		}
		else if (APX_SHAPETYPE_CAPSULE)
		{
			APhysXCapsuleShapeDesc* pCap = static_cast<APhysXCapsuleShapeDesc*>(pDesc);
			pCap->mHeight = prop.GetValue();
		}
		bRtnValue = true;
	}
	else if (PID_SGITEM_Extent_z == propValue)
	{
		APhysXShapeDesc* pDesc = m_pAFFDesc->mIncludeShapes.GetPhysXShapeDesc(0);
		if (APX_SHAPETYPE_BOX == pDesc->GetShapeType())
		{
			APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
			pBox->mDimensions.z = prop.GetValue();
		}
		bRtnValue = true;
	}
	else if (PID_SGITEM_RBS_Type == propValue)
	{
		NxForceFieldType fft;
		bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			m_pAFFDesc->mRBType = fft;
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_RBS_Value == propValue)
	{
		m_pAFFDesc->mRBForceScale = prop.GetValue();
		bRtnValue = true;
	}
	else if (PID_SGITEM_ClothS_Type == propValue)
	{
		NxForceFieldType fft;
		bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			m_pAFFDesc->mClothType = fft;
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_ClothS_Value == propValue)
	{
		m_pAFFDesc->mClothForceScale = prop.GetValue();
		bRtnValue = true;
	}
	else if (PID_SGITEM_FluidS_Type == propValue)
	{
		NxForceFieldType fft;
		bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			m_pAFFDesc->mFluidType = fft;
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_FluidS_Value == propValue)
	{
		m_pAFFDesc->mFluidForceScale = prop.GetValue();
		bRtnValue = true;
	}
	else if (PID_SGITEM_SBS_Type == propValue)
	{
		NxForceFieldType fft;
		bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			m_pAFFDesc->mSBType = fft;
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_SBS_Value == propValue)
	{
		m_pAFFDesc->mSBForceScale = prop.GetValue();
		bRtnValue = true;
	}
	else if (PID_SUBGROUP_Force_xyz == propValue)
	{
		A3DVECTOR3 force;
		bool rtn = CPhysXObjSelGroup::ExtractVector(prop, force);
		if (rtn)
		{
			APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
			const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
			APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
			newPara = para;
			newPara.mWindForce = APhysXConverter::A2N_Vector3(force);
			pWind->SetWindFFParameters(newPara);
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_Force_x == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
		APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
		newPara = para;
		newPara.mWindForce.x = prop.GetValue();
		pWind->SetWindFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Force_y == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
		APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
		newPara = para;
		newPara.mWindForce.y = prop.GetValue();
		pWind->SetWindFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Force_z == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
		APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
		newPara = para;
		newPara.mWindForce.z = prop.GetValue();
		pWind->SetWindFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFW_WindT == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
		APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
		newPara = para;
		newPara.mWindTime = prop.GetValue();
		pWind->SetWindFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFW_RestT == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
		APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
		newPara = para;
		newPara.mRestTime = prop.GetValue();
		pWind->SetWindFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFW_CycleT == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pWind->GetWindFFParameters();
		APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters newPara;
		newPara = para;
		newPara.mCycleTime = prop.GetValue();
		pWind->SetWindFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFW_CycleDist == propValue)
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		pWind->mCycleDistance = prop.GetValue();
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_TargetVel == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mTargetVelocity = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_TargetVelScale == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mTargetVelocityScaleTerm = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_TargetR == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mTargetRadius = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_TargetRScale == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mTargetRadiusScaleTerm = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_ElevationForce == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mConstantElevationForce = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_ElevationScale == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mElevationExpandScaleTerm = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_CentripetalForce == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mConstantCentripetalForce = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFV_Noise == propValue)
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pVortex->GetVortexFFParameters();
		APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters newPara;
		newPara = para;
		newPara.mNoise = prop.GetValue();
		pVortex->SetVortexFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_SUBGROUP_G_xyz == propValue)
	{
		A3DVECTOR3 g;
		bool rtn = CPhysXObjSelGroup::ExtractVector(prop, g);
		if (rtn)
		{
			APhysXGravitationForceFieldObjectDesc* pG = static_cast<APhysXGravitationForceFieldObjectDesc*>(m_pAFFDesc);
			pG->SetGravity(APhysXConverter::A2N_Vector3(g));
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_G_x == propValue)
	{
		APhysXGravitationForceFieldObjectDesc* pG = static_cast<APhysXGravitationForceFieldObjectDesc*>(m_pAFFDesc);
		NxVec3 g = pG->GetGravity();
		g.x = prop.GetValue();
		pG->SetGravity(g);
		bRtnValue = true;
	}
	else if (PID_SGITEM_G_y == propValue)
	{
		APhysXGravitationForceFieldObjectDesc* pG = static_cast<APhysXGravitationForceFieldObjectDesc*>(m_pAFFDesc);
		NxVec3 g = pG->GetGravity();
		g.y = prop.GetValue();
		pG->SetGravity(g);
		bRtnValue = true;
	}
	else if (PID_SGITEM_G_z == propValue)
	{
		APhysXGravitationForceFieldObjectDesc* pG = static_cast<APhysXGravitationForceFieldObjectDesc*>(m_pAFFDesc);
		NxVec3 g = pG->GetGravity();
		g.z = prop.GetValue();
		pG->SetGravity(g);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFE_CenterF == propValue)
	{
		APhysXExplosionForceFieldObjectDesc* pExplosion = static_cast<APhysXExplosionForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters& para = pExplosion->GetExplosionFFParameters();
		APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters newPara;
		newPara = para;
		newPara.mCenterForce = prop.GetValue();
		pExplosion->SetExplosionFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFE_Radius == propValue)
	{
		APhysXExplosionForceFieldObjectDesc* pExplosion = static_cast<APhysXExplosionForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters& para = pExplosion->GetExplosionFFParameters();
		APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters newPara;
		newPara = para;
		newPara.mRadius = prop.GetValue();
		pExplosion->SetExplosionFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFE_LifeTime == propValue)
	{
		APhysXExplosionForceFieldObjectDesc* pExplosion = static_cast<APhysXExplosionForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters& para = pExplosion->GetExplosionFFParameters();
		APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters newPara;
		newPara = para;
		newPara.mLifeTime = prop.GetValue();
		pExplosion->SetExplosionFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFE_QuadricFallOff == propValue)
	{
		APhysXExplosionForceFieldObjectDesc* pExplosion = static_cast<APhysXExplosionForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters& para = pExplosion->GetExplosionFFParameters();
		APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters newPara;
		newPara = para;
		newPara.mQuadricFallOff = prop.GetValue();
		pExplosion->SetExplosionFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFB_Density == propValue)
	{
		APhysXBuoyancyForceFieldObjectDesc* pBuoyancy = static_cast<APhysXBuoyancyForceFieldObjectDesc*>(m_pAFFDesc);
		float d = prop.GetValue();
		pBuoyancy->SetDensity(d);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFB_Force == propValue)
	{
		APhysXBossForceFieldObjectDesc* pBoss = static_cast<APhysXBossForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters& para = pBoss->GetBossFFParameters();
		APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters newPara;
		newPara = para;
		newPara.mForceValue = prop.GetValue();
		pBoss->SetBossFFParameters(newPara);
		bRtnValue = true;
	}
	else if (PID_ITEM_FFB_Radius == propValue)
	{
		APhysXBossForceFieldObjectDesc* pBoss = static_cast<APhysXBossForceFieldObjectDesc*>(m_pAFFDesc);
		const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters& para = pBoss->GetBossFFParameters();
		APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters newPara;
		newPara = para;
		newPara.mRadius = prop.GetValue();
		pBoss->SetBossFFParameters(newPara);
		bRtnValue = true;
	}
	else if ((PID_GROUP_FF_Custom <= propValue) && (PID_SGITEM_FOQD_z >= propValue))
	{
		bRtnValue = OnPropertyChangedFFCustom(prop);
	}
	SetSelGroup(pSelGroup);
	return bRtnValue;
}

bool CPhysXObjForceField::OnPropertyChangedFFCustom(const PropItem& prop)
{
	A3DVECTOR3 vecVal(0);
	bool bRtnValue = false;
	const DWORD_PTR propValue = prop.GetData();
	if (PID_SUBGROUP_Const_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mConstant.set(vecVal.x, vecVal.y, vecVal.z);
			return true;
		}
	}
	else if (PID_SGITEM_Const_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mConstant.x = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_Const_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mConstant.y = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_Const_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mConstant.z = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_PMRow0_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier.setRow(0, APhysXConverter::A2N_Vector3(vecVal));
			return true;
		}
	}
	else if (PID_SGITEM_PMR0_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 0) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_PMR0_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 1) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_PMR0_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 2) = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_PMRow1_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier.setRow(1, APhysXConverter::A2N_Vector3(vecVal));
			return true;
		}
	}
	else if (PID_SGITEM_PMR1_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 0) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_PMR1_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 1) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_PMR1_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 2) = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_PMRow2_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier.setRow(2, APhysXConverter::A2N_Vector3(vecVal));
			return true;
		}
	}
	else if (PID_SGITEM_PMR2_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 0) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_PMR2_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 1) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_PMR2_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 2) = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_TargetPos_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mPositionTarget.set(vecVal.x, vecVal.y, vecVal.z);
			return true;
		}
	}
	else if (PID_SGITEM_TP_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionTarget.x = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_TP_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionTarget.y = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_TP_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mPositionTarget.z = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_VMRow0_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier.setRow(0, APhysXConverter::A2N_Vector3(vecVal));
			return true;
		}
	}
	else if (PID_SGITEM_VMR0_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 0) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_VMR0_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 1) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_VMR0_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 2) = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_VMRow1_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier.setRow(1, APhysXConverter::A2N_Vector3(vecVal));
			return true;
		}
	}
	else if (PID_SGITEM_VMR1_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 0) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_VMR1_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 1) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_VMR1_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 2) = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_VMRow2_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier.setRow(2, APhysXConverter::A2N_Vector3(vecVal));
			return true;
		}
	}
	else if (PID_SGITEM_VMR2_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 0) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_VMR2_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 1) = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_VMR2_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 2) = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_TargetVel_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.set(vecVal.x, vecVal.y, vecVal.z);
			return true;
		}
	}
	else if (PID_SGITEM_TV_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.x = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_TV_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.y = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_TV_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.z = prop.GetValue();
		return true;
	}
	else if (PID_ITEM_FFC_TorusR == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mTorusRadius = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_Noise_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mNoise.set(vecVal.x, vecVal.y, vecVal.z);
			return true;
		}
	}
	else if (PID_SGITEM_Noise_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mNoise.x = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_Noise_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mNoise.y = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_Noise_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mNoise.z = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_FalloffLN_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.set(vecVal.x, vecVal.y, vecVal.z);
			return true;
		}
	}
	else if (PID_SGITEM_FOLN_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.x = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_FOLN_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.y = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_FOLN_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.z = prop.GetValue();
		return true;
	}
	else if (PID_SUBGROUP_FalloffQuad_xyz == propValue)
	{
		bRtnValue = CPhysXObjSelGroup::ExtractVector(prop, vecVal);
		if (bRtnValue)
		{
			m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.set(vecVal.x, vecVal.y, vecVal.z);
			return true;
		}
	}
	else if (PID_SGITEM_FOQD_x == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.x = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_FOQD_y == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.y = prop.GetValue();
		return true;
	}
	else if (PID_SGITEM_FOQD_z == propValue)
	{
		m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.z = prop.GetValue();
		return true;
	}

	return false;
}

void CPhysXObjForceField::OnUpdatePropsGrid()
{
	IPropObjBase::OnUpdatePropsGrid();
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 == pSelGroup)
		return;
	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	const bool u1 = pSelGroup->ReadUpdateMark(PID_GROUP_FF_Common);
	const bool u2 = pSelGroup->ReadUpdateMark(PID_GROUP_FF_Scale);
	const bool u3 = pSelGroup->ReadUpdateMark(PID_SUBGROUP_RBScale);
	const bool u4 = pSelGroup->ReadUpdateMark(PID_SUBGROUP_ClothScale);
	const bool u5 = pSelGroup->ReadUpdateMark(PID_SUBGROUP_FluidScale);
	const bool u6 = pSelGroup->ReadUpdateMark(PID_SUBGROUP_SBScale);
	const bool u7 = pSelGroup->ReadUpdateMark(PID_SUBGROUP_PosMultiplier);
	const bool u8 = pSelGroup->ReadUpdateMark(PID_SUBGROUP_VelMultiplier);
	if (u1 || u2 || u3 || u4 || u5 || u6 || u7 || u8)
	{
		assert(!"Shouldn't be changed or not supported yet!");
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_FF_Type))
		pSelGroup->ExecuteUpdateItem(PID_ITEM_FF_Type, GetForceFieldTypeText(m_pAFFDesc->GetObjType()), bIsMerge);

	if (pSelGroup->ReadUpdateMark(PID_ITEM_Coordinate))
		pSelGroup->ExecuteUpdateItem(PID_ITEM_Coordinate, GetFFCoordonateText(m_pAFFDesc->mFFCoordinates), bIsMerge);

	A3DVECTOR3 vol(0);
	CRegion::ShapeType st = CRegion::ST_BOX;

	if (pSelGroup->ReadUpdateMark(PID_GROUP_ShapeVolume))
	{
		GetShapeInfo(*m_pAFFDesc, st, &vol);
		pSelGroup->FillGroupShapeVolume(st, vol, bIsMerge);
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_Shape))
	{
		GetShapeInfo(*m_pAFFDesc, st, &vol);
		pSelGroup->ExecuteUpdateItem(PID_ITEM_Shape, CRegion::GetShapeTypeText(st), bIsMerge);
	}

	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Extent_xyz) || pSelGroup->ReadUpdateMark(PID_SGITEM_Extent_x)
		|| pSelGroup->ReadUpdateMark(PID_SGITEM_Extent_y) || pSelGroup->ReadUpdateMark(PID_SGITEM_Extent_z)
		|| pSelGroup->ReadUpdateMark(PID_ITEM_Radius) || pSelGroup->ReadUpdateMark(PID_ITEM_Height))
	{
		GetShapeInfo(*m_pAFFDesc, st, &vol);
		pSelGroup->ExecuteUpdateItemToShapeVolume(st, vol, bIsMerge);
	}

	PropID idx = pSelGroup->GetForceFieldSpecialGroupPropID(m_pAFFDesc->GetObjType());
	if (pSelGroup->ReadUpdateMark(idx))
		pSelGroup->FillAllFFProps(*this, false);

	if (pSelGroup->ReadUpdateMark(PID_SGITEM_RBS_Type))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_RBS_Type, GetFFScaleText(m_pAFFDesc->mRBType), bIsMerge);
	if (pSelGroup->ReadUpdateMark(PID_SGITEM_RBS_Value))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_RBS_Value, m_pAFFDesc->mRBForceScale, bIsMerge);

	if (pSelGroup->ReadUpdateMark(PID_SGITEM_ClothS_Type))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_ClothS_Type, GetFFScaleText(m_pAFFDesc->mClothType), bIsMerge);
	if (pSelGroup->ReadUpdateMark(PID_SGITEM_ClothS_Value))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_ClothS_Value, m_pAFFDesc->mClothForceScale, bIsMerge);

	if (pSelGroup->ReadUpdateMark(PID_SGITEM_FluidS_Type))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_FluidS_Type, GetFFScaleText(m_pAFFDesc->mFluidType), bIsMerge);
	if (pSelGroup->ReadUpdateMark(PID_SGITEM_FluidS_Value))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_FluidS_Value, m_pAFFDesc->mFluidForceScale, bIsMerge);

	if (pSelGroup->ReadUpdateMark(PID_SGITEM_SBS_Type))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_SBS_Type, GetFFScaleText(m_pAFFDesc->mSBType), bIsMerge);
	if (pSelGroup->ReadUpdateMark(PID_SGITEM_SBS_Value))
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_SBS_Value, m_pAFFDesc->mSBForceScale, bIsMerge);

	if (APX_OBJTYPE_FORCEFIELD_WIND == m_pAFFDesc->GetObjType())
	{
		APhysXWindForceFieldObjectDesc* pWind = static_cast<APhysXWindForceFieldObjectDesc*>(m_pAFFDesc);
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Force_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Force_x, pWind->GetWindFFParameters().mWindForce.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Force_y, pWind->GetWindFFParameters().mWindForce.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Force_z, pWind->GetWindFFParameters().mWindForce.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Force_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Force_x, pWind->GetWindFFParameters().mWindForce.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Force_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Force_y, pWind->GetWindFFParameters().mWindForce.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Force_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Force_z, pWind->GetWindFFParameters().mWindForce.z, bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFW_WindT))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFW_WindT, pWind->GetWindFFParameters().mWindTime, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFW_RestT))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFW_RestT, pWind->GetWindFFParameters().mRestTime, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFW_CycleT))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFW_CycleT, pWind->GetWindFFParameters().mCycleTime, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFW_CycleDist))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFW_CycleDist, pWind->mCycleDistance, bIsMerge);
	}
	else if (APX_OBJTYPE_FORCEFIELD_VORTEX == m_pAFFDesc->GetObjType())
	{
		APhysXVortexForceFieldObjectDesc* pVortex = static_cast<APhysXVortexForceFieldObjectDesc*>(m_pAFFDesc);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_TargetVel))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_TargetVel, pVortex->GetVortexFFParameters().mTargetVelocity, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_TargetVelScale))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_TargetVelScale, pVortex->GetVortexFFParameters().mTargetVelocityScaleTerm, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_TargetR))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_TargetR, pVortex->GetVortexFFParameters().mTargetRadius, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_TargetRScale))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_TargetRScale, pVortex->GetVortexFFParameters().mTargetRadiusScaleTerm, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_ElevationForce))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_ElevationForce, pVortex->GetVortexFFParameters().mConstantElevationForce, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_ElevationScale))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_ElevationScale, pVortex->GetVortexFFParameters().mElevationExpandScaleTerm, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_CentripetalForce))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_CentripetalForce, pVortex->GetVortexFFParameters().mConstantCentripetalForce, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFV_Noise))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFV_Noise, pVortex->GetVortexFFParameters().mNoise, bIsMerge);
	}
	else if (APX_OBJTYPE_FORCEFIELD_GRAVITATION == m_pAFFDesc->GetObjType())
	{
		APhysXGravitationForceFieldObjectDesc* pG = static_cast<APhysXGravitationForceFieldObjectDesc*>(m_pAFFDesc);
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_G_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_G_x, pG->GetGravity().x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_G_y, pG->GetGravity().y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_G_z, pG->GetGravity().z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_G_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_G_x, pG->GetGravity().x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_G_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_G_y, pG->GetGravity().y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_G_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_G_z, pG->GetGravity().z, bIsMerge);
		}
	}
	else if (APX_OBJTYPE_FORCEFIELD_EXPLOSION == m_pAFFDesc->GetObjType())
	{
		APhysXExplosionForceFieldObjectDesc* pEx = static_cast<APhysXExplosionForceFieldObjectDesc*>(m_pAFFDesc);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFE_CenterF))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFE_CenterF, pEx->GetExplosionFFParameters().mCenterForce, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFE_Radius))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFE_Radius, pEx->GetExplosionFFParameters().mRadius, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFE_LifeTime))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFE_LifeTime, pEx->GetExplosionFFParameters().mLifeTime, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFE_QuadricFallOff))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFE_QuadricFallOff, pEx->GetExplosionFFParameters().mQuadricFallOff, bIsMerge);
	}
	else if (APX_OBJTYPE_FORCEFIELD_BUOYANCY == m_pAFFDesc->GetObjType())
	{
		APhysXBuoyancyForceFieldObjectDesc* pBu = static_cast<APhysXBuoyancyForceFieldObjectDesc*>(m_pAFFDesc);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFB_Density))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFB_Density, pBu->GetDensity(), bIsMerge);
	}
	else if (APX_OBJTYPE_FORCEFIELD_BOSS == m_pAFFDesc->GetObjType())
	{
		APhysXBossForceFieldObjectDesc* pBo = static_cast<APhysXBossForceFieldObjectDesc*>(m_pAFFDesc);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFB_Force))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFB_Force, pBo->GetBossFFParameters().mForceValue, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFB_Radius))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFB_Radius, pBo->GetBossFFParameters().mRadius, bIsMerge);
	}
	else if (APX_OBJTYPE_FORCEFIELD == m_pAFFDesc->GetObjType())
	{
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Const_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Const_x, m_pAFFDesc->mFFLKernelParameters.mConstant.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Const_y, m_pAFFDesc->mFFLKernelParameters.mConstant.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Const_z, m_pAFFDesc->mFFLKernelParameters.mConstant.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Const_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Const_x, m_pAFFDesc->mFFLKernelParameters.mConstant.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Const_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Const_y, m_pAFFDesc->mFFLKernelParameters.mConstant.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Const_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Const_z, m_pAFFDesc->mFFLKernelParameters.mConstant.z, bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_PMRow0_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR0_x, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 0), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR0_y, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 1), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR0_z, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 2), bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR0_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR0_x, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 0), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR0_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR0_y, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 1), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR0_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR0_z, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(0, 2), bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_PMRow1_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR1_x, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 0), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR1_y, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 1), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR1_z, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 2), bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR1_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR1_x, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 0), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR1_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR1_y, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 1), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR1_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR1_z, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(1, 2), bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_PMRow2_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR2_x, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 0), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR2_y, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 1), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR2_z, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 2), bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR2_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR2_x, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 0), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR2_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR2_y, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 1), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_PMR2_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_PMR2_z, m_pAFFDesc->mFFLKernelParameters.mPositionMultiplier(2, 2), bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_TargetPos_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_TP_x, m_pAFFDesc->mFFLKernelParameters.mPositionTarget.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_TP_y, m_pAFFDesc->mFFLKernelParameters.mPositionTarget.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_TP_z, m_pAFFDesc->mFFLKernelParameters.mPositionTarget.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_TP_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_TP_x, m_pAFFDesc->mFFLKernelParameters.mPositionTarget.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_TP_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_TP_y, m_pAFFDesc->mFFLKernelParameters.mPositionTarget.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_TP_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_TP_z, m_pAFFDesc->mFFLKernelParameters.mPositionTarget.z, bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_VMRow0_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR0_x, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 0), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR0_y, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 1), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR0_z, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 2), bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR0_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR0_x, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 0), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR0_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR0_y, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 1), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR0_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR0_z, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(0, 2), bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_VMRow1_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR1_x, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 0), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR1_y, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 1), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR1_z, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 2), bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR1_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR1_x, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 0), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR1_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR1_y, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 1), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR1_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR1_z, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(1, 2), bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_VMRow2_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR2_x, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 0), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR2_y, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 1), bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR2_z, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 2), bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR2_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR2_x, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 0), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR2_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR2_y, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 1), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_VMR2_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_VMR2_z, m_pAFFDesc->mFFLKernelParameters.mVelocityMultiplier(2, 2), bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_TargetVel_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_TV_x, m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_TV_y, m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_TV_z, m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_TV_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_TV_x, m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_TV_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_TV_y, m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_TV_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_TV_z, m_pAFFDesc->mFFLKernelParameters.mVelocityTarget.z, bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_ITEM_FFC_TorusR))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_FFC_TorusR, m_pAFFDesc->mFFLKernelParameters.mTorusRadius, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Noise_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Noise_x, m_pAFFDesc->mFFLKernelParameters.mNoise.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Noise_y, m_pAFFDesc->mFFLKernelParameters.mNoise.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Noise_z, m_pAFFDesc->mFFLKernelParameters.mNoise.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Noise_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Noise_x, m_pAFFDesc->mFFLKernelParameters.mNoise.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Noise_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Noise_y, m_pAFFDesc->mFFLKernelParameters.mNoise.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_Noise_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_Noise_z, m_pAFFDesc->mFFLKernelParameters.mNoise.z, bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_FalloffLN_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOLN_x, m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOLN_y, m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOLN_z, m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_FOLN_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOLN_x, m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_FOLN_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOLN_y, m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_FOLN_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOLN_z, m_pAFFDesc->mFFLKernelParameters.mFalloffLinear.z, bIsMerge);
		}
		if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_FalloffQuad_xyz))
		{
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOQD_x, m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.x, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOQD_y, m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.y, bIsMerge);
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOQD_z, m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.z, bIsMerge);
		}
		else
		{
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_FOQD_x))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOQD_x, m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.x, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_FOQD_y))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOQD_y, m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.y, bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_SGITEM_FOQD_z))
				pSelGroup->ExecuteUpdateItem(PID_SGITEM_FOQD_z, m_pAFFDesc->mFFLKernelParameters.mFalloffQuadratic.z, bIsMerge);
		}
	}
}

CPhysXObjForceField* CPhysXObjForceField::CreateMyselfInstance(bool snapValues) const
{
	CPhysXObjForceField* pProp = 0;
	if (snapValues)
		pProp = new CPhysXObjForceField(*this);
	else
		pProp = new CPhysXObjForceField;
	return pProp;
}

void CPhysXObjForceField::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropPhysXObjBase::EnterRuntimePreNotify(aPhysXScene);
	CPhysXObjForceField* pBK = dynamic_cast<CPhysXObjForceField*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void CPhysXObjForceField::LeaveRuntimePostNotify()
{
	IPropPhysXObjBase::LeaveRuntimePostNotify();
	CPhysXObjForceField* pBK = dynamic_cast<CPhysXObjForceField*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

bool CPhysXObjForceField::OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	if (0 != pPSToRuntime)
		OnInstancePhysXObj(*pPSToRuntime);
	return IObjBase::OnDoWakeUpTask(eu, pPSToRuntime);
}

bool CPhysXObjForceField::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	int nCount = m_objDescMgr.GetNum();
	assert(0 < nCount);
	if (0 >= nCount)
		return false;

	if (0 != m_pNxFF)
	{
		if (&m_pNxFF->getScene() == aPhysXScene.GetNxScene())
			return true;

		ReleasePhysXObj();
	}

	assert(0 == m_pAFFObj);
	APhysXObjectInstanceDesc objInstanceDesc;
	objInstanceDesc.mPhysXObjectDesc = m_objDescMgr.GetPhysXObjectDesc(nCount - 1);
	A3DMATRIX4 mtPose(A3DMATRIX4::IDENTITY);
	GetGPose(mtPose);
	APhysXConverter::A2N_Matrix44(mtPose, objInstanceDesc.mGlobalPose);
	m_pAFFObj = static_cast<APhysXForceFieldObject*>(aPhysXScene.CreatePhysXObject(objInstanceDesc));
	if (0 != m_pAFFObj)
		m_pNxFF = m_pAFFObj->GetNxForceField();

	return (0 == m_pNxFF)? false : true;
}

void CPhysXObjForceField::OnReleasePhysXObj()
{
	if (0 != m_pNxFF)
	{
		assert(m_pNxFF->userData == m_pAFFObj);
	}
	if (0 != m_pAFFObj)
	{
		m_pAFFObj->GetPhysXScene()->ReleasePhysXObject(m_pAFFObj, true);
		m_pNxFF = 0;
		m_pAFFObj = 0;
	}
}

bool CPhysXObjForceField::OnGetPos(A3DVECTOR3& vOutPos) const
{
	if (0 == m_pAFFObj)
	{
		vOutPos = GetPos(false);
	}
	else
	{
		NxMat34 matPose = m_pAFFObj->GetPose();
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
				matPose.multiply(pActor->getGlobalPose(), matPose);
		}
		vOutPos = APhysXConverter::N2A_Vector3(matPose.t);
	}
	return true;
}

void CPhysXObjForceField::OnSetPos(const A3DVECTOR3& vPos)
{
	if (0 != m_pAFFObj)
	{
		NxMat34 matPose = m_pAFFObj->GetPose();
		matPose.t = APhysXConverter::A2N_Vector3(vPos);
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
			{
				NxMat34 pose = pActor->getGlobalPose();
				NxMat34 mat;
				pose.getInverse(mat);
				mat.multiply(matPose.t, matPose.t);
			}
		}
		m_pAFFObj->SetPose(matPose);
	}
	m_EditorOBB.Center = vPos;
}

bool CPhysXObjForceField::OnGetDir(A3DVECTOR3& vOutDir) const
{
	if (0 == m_pAFFObj)
	{
		vOutDir = GetDir(false);
	}
	else
	{
		NxMat34 matPose = m_pAFFObj->GetPose();
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
				matPose.multiply(pActor->getGlobalPose(), matPose);
		}
		vOutDir = APhysXConverter::N2A_Vector3(matPose.M.getColumn(2));
	}
	return true;
}

bool CPhysXObjForceField::OnGetUp(A3DVECTOR3& vOutUp) const
{
	if (0 == m_pAFFObj)
	{
		vOutUp = GetUp(false);
	}
	else
	{
		NxMat34 matPose = m_pAFFObj->GetPose();
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
				matPose.multiply(pActor->getGlobalPose(), matPose);
		}
		vOutUp = APhysXConverter::N2A_Vector3(matPose.M.getColumn(1));
	}
	return true;
}

void CPhysXObjForceField::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (0 != m_pAFFObj)
	{
		NxMat34 matPose = m_pAFFObj->GetPose();
		NxVec3 nxDir = APhysXConverter::A2N_Vector3(vDir);
		NxVec3 nxUp = APhysXConverter::A2N_Vector3(vUp);
		NxVec3 nxX = nxUp.cross(nxDir);
		matPose.M.setColumn(0, nxX);
		matPose.M.setColumn(1, nxUp);
		matPose.M.setColumn(2, nxDir);
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
			{
				NxMat34 pose = pActor->getGlobalPose();
				NxMat34 mat;
				pose.getInverse(mat);
				mat.multiply(matPose, matPose);
			}
		}
		m_pAFFObj->SetPose(matPose);
	}
	m_EditorOBB.ZAxis = vDir;
	m_EditorOBB.YAxis = vUp;
	m_EditorOBB.XAxis = CrossProduct(vUp, vDir);
	m_EditorOBB.CompleteExtAxis();
}

bool CPhysXObjForceField::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	outWithScaled = false;
	if (0 == m_pAFFObj)
	{
		matOutPose = GetPose(false);
	}
	else
	{
		NxMat34 matPose = m_pAFFObj->GetPose();
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
				matPose.multiply(pActor->getGlobalPose(), matPose);
		}
		APhysXConverter::N2A_Matrix44(matPose, matOutPose);
	}
	return true;
}

void CPhysXObjForceField::OnSetPose(const A3DMATRIX4& matPose)
{
	if (0 != m_pAFFObj)
	{
		NxMat34 nxmatPose;
		APhysXConverter::A2N_Matrix44(matPose, nxmatPose);
		if (0 != m_pNxFF)
		{
			NxActor* pActor = m_pNxFF->getActor();
			if (0 != pActor)
			{
				NxMat34 pose = pActor->getGlobalPose();
				NxMat34 mat;
				pose.getInverse(mat);
				nxmatPose.multiply(nxmatPose, mat);
			}
		}
		m_pAFFObj->SetPose(nxmatPose);
	}
	m_EditorOBB.Center = matPose.GetRow(3);
	m_EditorOBB.ZAxis = matPose.GetRow(2);
	m_EditorOBB.YAxis = matPose.GetRow(1);
	m_EditorOBB.XAxis = matPose.GetRow(0);
	m_EditorOBB.CompleteExtAxis();
}

void CPhysXObjForceField::SetDefaultOBB()
{
	m_EditorOBB.Clear();
	m_EditorOBB.Extents.Set(0.3f, 0.3f, 0.3f);
	m_EditorOBB.XAxis.Set(1, 0, 0);
	m_EditorOBB.YAxis.Set(0, 1, 0);
	m_EditorOBB.ZAxis.Set(0, 0, 1);
	m_EditorOBB.CompleteExtAxis();
}

CPhysXObjForceField* CPhysXObjForceField::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_FORCEFIELD, true, *m_pScene);
	CPhysXObjForceField* pObj = dynamic_cast<CPhysXObjForceField*>(pObjBase);
	if (0 != pObj)
	{
		*pObj = *this;
		pObj->m_FFRender.SetModelPtr(pObj);
	}
	return pObj;
}

void CPhysXObjForceField::OnDoSleepTask()
{
	OnReleasePhysXObj();
}

void CPhysXObjForceField::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
{
	m_bEnableDraw = false;
	if (IsRuntime())
	{
		if (!m_pScene->IsPhysXDebugRenderEnabled())
			return;
	}

	IPhysXObjBase::BuildExtraRenderDataMyself(er, dwColor);
	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		er.AddSphere(m_EditorOBB.Center, 0.1f, 0xff00ff00);
		int dwTheColor = 0xff6520EE;
		if (0 == dwColor)
			er.AddOBB(m_EditorOBB, dwTheColor);
	}
	if (GetFlags().ReadFlag(OBF_DRAW_BINDING_BOX) || (0 != GetSelGroup()))
	{
		m_bEnableDraw = true;
		m_DrawColor = 0xffffff00;
	}
}

void CPhysXObjForceField::DrawAllFFShapes(A3DWireCollector& wc) const
{
	if (!m_bEnableDraw)
		return;
	int nCount = m_objDescMgr.GetNum();
	if (0 >= nCount)
		return;

	A3DMATRIX4 mtPose(A3DMATRIX4::IDENTITY);
	GetGPose(mtPose);
	NxMat34 matGPose;
	APhysXConverter::A2N_Matrix44(mtPose, matGPose);

	APhysXObjectDesc* pDesc = m_objDescMgr.GetPhysXObjectDesc(nCount - 1);
	APhysXForceFieldObjectDesc* pFFOD = static_cast<APhysXForceFieldObjectDesc*>(pDesc);
	nCount = pFFOD->mIncludeShapes.GetNum();
	for (int i = 0; i < nCount; ++i)
	{
		APhysXShapeDesc* pDesc = pFFOD->mIncludeShapes.GetPhysXShapeDesc(i);
		switch(pDesc->GetShapeType())
		{
		case APX_SHAPETYPE_BOX:
			{
				APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
				NxMat34 mat(matGPose);
				mat.multiply(matGPose, pBox->mLocalPose);
				APhysXDrawBasic::DrawBox(wc, NxBox(mat.t, pBox->mDimensions, mat.M), m_DrawColor);
			}
			break;
		case APX_SHAPETYPE_SPHERE:
			{
				APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
				NxMat34 mat(matGPose);
				mat.multiply(matGPose, pSphere->mLocalPose);
				APhysXDrawBasic::DrawSphere(wc, pSphere->mRadius, m_DrawColor, 0xffffffff, &mat);
			}
			break;
		case APX_SHAPETYPE_CAPSULE:
			{
				APhysXCapsuleShapeDesc* pCapsule = static_cast<APhysXCapsuleShapeDesc*>(pDesc);
				NxMat34 mat(matGPose);
				mat.multiply(matGPose, pCapsule->mLocalPose);
				APhysXDrawBasic::DrawCapsule(wc, pCapsule->mRadius, pCapsule->mHeight, m_DrawColor, 0xffffffff, &mat);
			}
			break;
		case APX_SHAPETYPE_CONVEX:
			{
				APhysXConvexShapeDesc* pConvex = static_cast<APhysXConvexShapeDesc*>(pDesc);
				NxMat34 mat(matGPose);
				mat.multiply(matGPose, pConvex->mLocalPose);
				NxConvexShapeDesc descConvexShape;
				pConvex->GetNxShapeDesc(descConvexShape);
				NxConvexMesh* pMesh = static_cast<NxConvexMesh*>(descConvexShape.meshData);
				NxConvexMeshDesc descConvexMesh;
				pMesh->saveToDesc(descConvexMesh);
				APhysXDrawBasic::DrawConvex(wc, descConvexMesh, m_DrawColor, &mat);
			}
			break;
		}
	}
}

bool CPhysXObjForceField::OnDeletionNotify(APhysXObject& object)
{
	if (m_pAFFObj == &object)
	{
		m_pNxFF = 0;
		m_pAFFObj = 0;
		return true;
	}
	return false;
}

bool CPhysXObjForceField::ImportAndExportFFData(bool import, const AString& strFile)
{
	if (import)
	{
		APhysXObjectDescManager	objDescMgr;
		APhysXObjectDesc* pDesc = objDescMgr.GetPhysXObjectDesc(strFile);
		if (0 == pDesc)
		{
			a_LogOutput(1, "CPhysXObjForceField::ImportAndExportFFData: Fail to import file!");
			return false;
		}
		if (!pDesc->IsForceField())
		{
			a_LogOutput(1, "CPhysXObjForceField::ImportAndExportFFData: The imported descriptor is not FF descriptor!");
			return false;
		}
		CopyAPhysXObjectDescManager(objDescMgr);
		return true;
	}
	else
		return m_objDescMgr.Save(strFile);
	return false;
}

bool CPhysXObjForceField::SetShapeInfo(const CRegion::ShapeType& st, const A3DVECTOR3& vol)
{
	assert(0 != m_pAFFDesc);
	NxVec3 nxVol(vol.x, vol.y, vol.z);
	return SetShapeInfo(st, *m_pAFFDesc, &nxVol);
}

bool CPhysXObjForceField::GetAPhysXInstanceAABB(NxBounds3& outAABB) const
{
	if (0 != m_pAFFObj)
	{
		outAABB.setCenterExtents(APhysXConverter::A2N_Vector3(m_EditorOBB.Center), APhysXConverter::A2N_Vector3(m_EditorOBB.Extents));
		return true;
	}
	return false;
}

bool CPhysXObjForceField::OnLoadModel(IEngineUtility& eu, const char* szFile)
{
	SetObjName(_T("ForceField"));
	m_FFRender.SetModelPtr(this);
	return true;
}

void CPhysXObjForceField::OnReleaseModel()
{

}

bool CPhysXObjForceField::OnGetModelAABB(A3DAABB& outAABB) const
{
	if (0 == m_pAFFObj)
		return false;

	outAABB.Center = m_EditorOBB.Center;
	outAABB.Extents = m_EditorOBB.Extents;
	outAABB.CompleteMinsMaxs();
	return true;
}
