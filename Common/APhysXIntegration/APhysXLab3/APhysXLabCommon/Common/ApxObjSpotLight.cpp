
#include "stdafx.h"

#include "ECWorldRender.h"
#ifdef _ANGELICA3
	#include <A3DLPPLight.h>
	#include <A3DLPPRender.h>
#endif

ApxObjSpotLight::ApxObjSpotLight(int UID)
{
#ifdef _ANGELICA3
	 m_pLPPSpotLight = 0;
#else
	m_pSpotLight = 0;
#endif

	if (0 <= UID)
	{
		InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_SPOTLIGHT, *this, UID);
		SetProperties(*this);
		SetObjName(_T("SpotLight"));

		m_aabb.setCenterExtents(NxVec3(0.0f), NxVec3(0.3f));
		SetOriginalAABB(m_aabb);
		SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	}

	m_Range = 60.0f;
	m_AngleDegreeTheta = 10.0f;
	m_AngleDegreePhi = 60.0f;
	m_Attenuation.Set(1, 0, 0);
	m_color.Set(1, 1, 1, 1);
}

ApxObjSpotLight& ApxObjSpotLight::operator= (const ApxObjSpotLight& rhs)
{
	IPropObjBase::operator=(rhs);
	SetRange(rhs.GetRange());
	SetInnerAngle(rhs.GetInnerAngle());
	SetOuterAngle(rhs.GetOuterAngle());
	SetAttenuation(rhs.GetAttenuation());
	SetColor(rhs.GetColor());
	return *this;
}

ApxObjSpotLight* ApxObjSpotLight::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_SPOTLIGHT, true, *m_pScene);
	ApxObjSpotLight* pObj = dynamic_cast<ApxObjSpotLight*>(pObjBase);
	if (0 != pObj)
		*pObj = *this;
	return pObj;
}

 ApxObjSpotLight::~ApxObjSpotLight() 
{	
	if (0 <= GetUID()) { assert(false == IsObjAliveState()); } 
}

void ApxObjSpotLight::OnSetScene(Scene* pScene)
{
	if (pScene == m_pScene)
		return;

#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		if (0 != m_pScene)
			m_pScene->GetRender()->GetWorldRenderer()->GetLPPRender()->GetLightMan()->RemoveLight(m_pLPPSpotLight);
	}
	
	if (0 != pScene)
	{
		if (CreateSpotLight())
			pScene->GetRender()->GetWorldRenderer()->GetLPPRender()->GetLightMan()->AddLight(m_pLPPSpotLight);
	}
	else
	{
		delete m_pLPPSpotLight;
		m_pLPPSpotLight = NULL;
	}
#else
	if (0 != m_pSpotLight)
	{
		if (0 != m_pScene)
		{
			A3DLightMan* pLightMan = m_pScene->GetRender()->GetA3DEngine()->GetA3DLightMan();
			if (0 != pLightMan)
			{
				pLightMan->ReleaseLight(m_pSpotLight);
				m_pSpotLight = 0;
			}
		}
	}

	assert(0 == m_pSpotLight);
	if (0 != pScene)
	{
		A3DLightMan* pLightMan = pScene->GetRender()->GetA3DEngine()->GetA3DLightMan();
		CreateSpotLight(*pLightMan);
	}
#endif
}

#ifdef _ANGELICA3
bool ApxObjSpotLight::CreateSpotLight()
{
	if (0 != m_pLPPSpotLight)
		return false;

	m_pLPPSpotLight = new A3DLPPLightSpot;
	if (0 == m_pLPPSpotLight)
		return false;

	m_pLPPSpotLight->SetRange(m_Range);
	m_pLPPSpotLight->SetAngles(DEG2RAD(m_AngleDegreeTheta), DEG2RAD(m_AngleDegreePhi));
	m_pLPPSpotLight->SetAttenuation(m_Attenuation.x, m_Attenuation.y, m_Attenuation.z);
	m_pLPPSpotLight->SetColor(m_color);
	m_pLPPSpotLight->SetPos(GetPos(false));
	m_pLPPSpotLight->SetDir(GetDir(false));
	return true;
}
#else
bool ApxObjSpotLight::CreateSpotLight(A3DLightMan& LightMgr)
{
	if (0 != m_pSpotLight)
		return false;

	A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(GetGPos());
	A3DVECTOR3 dir, up;
	GetGDirAndUp(dir, up);
	bool bRtn = LightMgr.CreateSpotLight(1, &m_pSpotLight, pos, dir, m_color, 
		A3DCOLORVALUE(1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 
		m_Range, 1.0f, DEG2RAD(m_AngleDegreeTheta), DEG2RAD(m_AngleDegreePhi), m_Attenuation.Magnitude());
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Attenuation0 = m_Attenuation.x;
		lp.Attenuation1 = m_Attenuation.y;
		lp.Attenuation2 = m_Attenuation.z;
		m_pSpotLight->SetLightParam(lp);
	}
	return bRtn;
}
#endif

void ApxObjSpotLight::SetRange(float fRange)
{
	m_Range = fRange;
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetRange(m_Range);
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Range = fRange;
		m_pSpotLight->SetLightParam(lp);
	}
#endif
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_Range);
}

void ApxObjSpotLight::SetInnerAngle(float fInnerAngleDegree)
{
	m_AngleDegreeTheta = NxMath::mod(fInnerAngleDegree, 360);
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetAngles(DEG2RAD(m_AngleDegreeTheta), DEG2RAD(m_AngleDegreePhi));
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Theta = DEG2RAD(m_AngleDegreeTheta);
		lp.Phi = DEG2RAD(m_AngleDegreePhi);
		m_pSpotLight->SetLightParam(lp);
	}
#endif
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_InnerTheta);
}

void ApxObjSpotLight::SetOuterAngle(float fOuterAngleDegree)
{
	m_AngleDegreePhi = NxMath::mod(fOuterAngleDegree, 360);
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetAngles(DEG2RAD(m_AngleDegreeTheta), DEG2RAD(m_AngleDegreePhi));
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Theta = DEG2RAD(m_AngleDegreeTheta);
		lp.Phi = DEG2RAD(m_AngleDegreePhi);
		m_pSpotLight->SetLightParam(lp);
	}
#endif
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_OuterPhi);
}

void ApxObjSpotLight::SetAttenuation(const A3DVECTOR3& attenuation)
{
	bool equX = NxMath::equals(m_Attenuation.x, attenuation.x, APHYSX_FLOAT_EPSILON);
	bool equY = NxMath::equals(m_Attenuation.y, attenuation.y, APHYSX_FLOAT_EPSILON);
	bool equZ = NxMath::equals(m_Attenuation.z, attenuation.z, APHYSX_FLOAT_EPSILON);
	if (equX && equY && equZ)
		return;

	m_Attenuation = attenuation;
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetAttenuation(m_Attenuation.x, m_Attenuation.y, m_Attenuation.z);
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Attenuation0 = m_Attenuation.x;
		lp.Attenuation1 = m_Attenuation.y;
		lp.Attenuation2 = m_Attenuation.z;
		m_pSpotLight->SetLightParam(lp);
	}
#endif

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		if (!equX)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Attenuation_x);
		if (!equY)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Attenuation_y);
		if (!equZ)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Attenuation_z);
	}
}

void ApxObjSpotLight::SetColor(const A3DCOLORVALUE& color)
{
	bool equR = NxMath::equals(m_color.r, color.r, APHYSX_FLOAT_EPSILON);
	bool equG = NxMath::equals(m_color.g, color.g, APHYSX_FLOAT_EPSILON);
	bool equB = NxMath::equals(m_color.b, color.b, APHYSX_FLOAT_EPSILON);
	bool equA = NxMath::equals(m_color.a, color.a, APHYSX_FLOAT_EPSILON);
	if (equR && equG && equB && equA)
		return;

	m_color = color;
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetColor(m_color);
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Diffuse = m_color;
		m_pSpotLight->SetLightParam(lp);
	}
#endif

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		if (!equR)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Color_r);
		if (!equG)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Color_g);
		if (!equB)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Color_b);
		if (!equA)
			pSelGroup->RaiseUpdateMark(PID_SGITEM_Color_a);
	}
}

bool ApxObjSpotLight::SaveToFile(NxStream& nxStream)
{
	if (IPropObjBase::SaveToFile(nxStream))
	{
		SaveVersion(&nxStream);
		nxStream.storeFloat(m_Range);
		nxStream.storeFloat(m_AngleDegreeTheta);
		nxStream.storeFloat(m_AngleDegreePhi);
		nxStream.storeBuffer(&m_Attenuation, sizeof(A3DVECTOR3));
		nxStream.storeBuffer(&m_color, sizeof(A3DCOLORVALUE));
		return true;
	}
	return false;
}

bool ApxObjSpotLight::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (IPropObjBase::LoadFromFile(nxStream, outIsLowVersion))
	{
		LoadVersion(&nxStream);
		if(m_StreamObjVer >= 0xAA000001)
		{
			SetRange(nxStream.readFloat());
			SetInnerAngle(nxStream.readFloat());
			SetOuterAngle(nxStream.readFloat());
			A3DVECTOR3 atten;
			nxStream.readBuffer(&atten, sizeof(A3DVECTOR3));
			SetAttenuation(atten);
			A3DCOLORVALUE col;
			nxStream.readBuffer(&col, sizeof(A3DCOLORVALUE));
			SetColor(col);
		}
		if(m_StreamObjVer < GetVersion())
			outIsLowVersion = true;
		return true;
	}
	return false;
}

ApxObjSpotLight* ApxObjSpotLight::CreateMyselfInstance(bool snapValues) const
{
	ApxObjSpotLight* pProp = 0;
	if (snapValues)
		pProp = new ApxObjSpotLight(*this);
	else
		pProp = new ApxObjSpotLight;
	return pProp;
}

void ApxObjSpotLight::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropObjBase::EnterRuntimePreNotify(aPhysXScene);
	ApxObjSpotLight* pBK = dynamic_cast<ApxObjSpotLight*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void ApxObjSpotLight::LeaveRuntimePostNotify()
{
	IPropObjBase::LeaveRuntimePostNotify();
	ApxObjSpotLight* pBK = dynamic_cast<ApxObjSpotLight*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

void ApxObjSpotLight::OnTick(float dt)
{
	IObjBase::OnTick(dt);
	IObjBase::BuildExtraRenderData();
}

void ApxObjSpotLight::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
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

bool ApxObjSpotLight::OnSendToPropsGrid()
{
	bool bRtn = IPropObjBase::OnSendToPropsGrid();
	if (bRtn)
		GetSelGroup()->FillGroupSpotLight(*this);
	return bRtn;
}

bool ApxObjSpotLight::OnPropertyChanged(PropItem& prop)
{
	if (IPropObjBase::OnPropertyChanged(prop))
		return true;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	SetSelGroup(0);
	bool bRtnValue = false;

	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_Range == propValue)
	{
		SetRange(prop.GetValue());
		bRtnValue = true;
	}
	else if (PID_ITEM_InnerTheta == propValue)
	{
		float val = prop.GetValue();
		if (val < 0.0f)
		{
			val = 0.0f;
			prop.SetValue(val);
		}
		else if (val > m_AngleDegreePhi)
		{
			val = m_AngleDegreePhi;
			prop.SetValue(val);
		}
		SetInnerAngle(val);
		bRtnValue = true;
	}
	else if (PID_ITEM_OuterPhi == propValue)
	{
		float val = prop.GetValue();
		if (val < 0.0f)
		{
			val = 0.0f;
			prop.SetValue(val);
		}
		else if (!(val < 90.0f))
		{
			val = 89.9999f;
			prop.SetValue(val);
		}
		SetOuterAngle(val);
		bRtnValue = true;
	}
	else if (PID_SUBGROUP_Attenuation_xyz == propValue)
	{
		A3DVECTOR3 atten;
		bool rtn = CPhysXObjSelGroup::ExtractVector(prop, atten);
		if (rtn)
		{
			SetAttenuation(atten);
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_Attenuation_x == propValue)
	{
		A3DVECTOR3 atten = GetAttenuation();
		atten.x = prop.GetValue();
		SetAttenuation(atten);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Attenuation_y == propValue)
	{
		A3DVECTOR3 atten = GetAttenuation();
		atten.y = prop.GetValue();
		SetAttenuation(atten);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Attenuation_z == propValue)
	{
		A3DVECTOR3 atten = GetAttenuation();
		atten.z = prop.GetValue();
		SetAttenuation(atten);
		bRtnValue = true;
	}
	else if (PID_SUBGROUP_Color_rgba == propValue)
	{
		A3DCOLORVALUE color;
		bool rtn = CPhysXObjSelGroup::ExtractColor(prop, color);
		if (rtn)
		{
			color.Clamp();
			SetColor(color);
			CPhysXObjSelGroup::ApplyColorValue(prop,color);
			bRtnValue = true;
		}
	}
	else if (PID_SGITEM_Color_r == propValue)
	{
		A3DCOLORVALUE color = GetColor();
		color.r = prop.GetValue();
		if (color.r > 1.0f)
		{
			color.r = 1.0f;
			prop.SetValue(color.r);
		}
		else if (color.r < 0.0f)
		{
			color.r = 0.0f;
			prop.SetValue(color.r);
		}
		SetColor(color);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Color_g == propValue)
	{
		A3DCOLORVALUE color = GetColor();
		color.g = prop.GetValue();
		if (color.g > 1.0f)
		{
			color.g = 1.0f;
			prop.SetValue(color.g);
		}
		else if (color.g < 0.0f)
		{
			color.g = 0.0f;
			prop.SetValue(color.g);
		}
		SetColor(color);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Color_b == propValue)
	{
		A3DCOLORVALUE color = GetColor();
		color.b = prop.GetValue();
		if (color.b > 1.0f)
		{
			color.b = 1.0f;
			prop.SetValue(color.b);
		}
		else if (color.b < 0.0f)
		{
			color.b = 0.0f;
			prop.SetValue(color.b);
		}
		SetColor(color);
		bRtnValue = true;
	}
	else if (PID_SGITEM_Color_a == propValue)
	{
		A3DCOLORVALUE color = GetColor();
		color.a = prop.GetValue();
		if (color.a > 1.0f)
		{
			color.a = 1.0f;
			prop.SetValue(color.a);
		}
		else if (color.a < 0.0f)
		{
			color.a = 0.0f;
			prop.SetValue(color.a);
		}
		SetColor(color);
		bRtnValue = true;
	}
	SetSelGroup(pSelGroup);
	return bRtnValue;
}

void ApxObjSpotLight::OnUpdatePropsGrid()
{
	IPropObjBase::OnUpdatePropsGrid();
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 == pSelGroup)
		return;
	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	if (pSelGroup->ReadUpdateMark(PID_GROUP_SpotLight))
	{
		pSelGroup->FillGroupSpotLight(*this);
		return;
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_Range))
	{
		float fRange = GetRange();
		pSelGroup->ExecuteUpdateItem(PID_ITEM_Range, fRange, bIsMerge);
	}
	if (pSelGroup->ReadUpdateMark(PID_ITEM_InnerTheta))
	{
		float fTheta = GetInnerAngle();
		pSelGroup->ExecuteUpdateItem(PID_ITEM_SocketName, fTheta, bIsMerge);
	}
	if (pSelGroup->ReadUpdateMark(PID_ITEM_OuterPhi))
	{
		float fPhi = GetOuterAngle();
		pSelGroup->ExecuteUpdateItem(PID_ITEM_OuterPhi, fPhi, bIsMerge);
	}

	A3DVECTOR3 atten = GetAttenuation();
	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Attenuation_xyz))
	{
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Attenuation_x, atten.x, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Attenuation_y, atten.y, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Attenuation_z, atten.z, bIsMerge);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Attenuation_x))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Attenuation_x, atten.x, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Attenuation_y))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Attenuation_y, atten.y, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Attenuation_z))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Attenuation_z, atten.z, bIsMerge);
	}

	A3DCOLORVALUE color = GetColor();
	if (pSelGroup->ReadUpdateMark(PID_SUBGROUP_Color_rgba))
	{
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_r, color.r, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_g, color.g, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_b, color.b, bIsMerge);
		pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_a, color.a, bIsMerge);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Color_r))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_r, color.r, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Color_g))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_g, color.g, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Color_b))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_b, color.b, bIsMerge);
		if (pSelGroup->ReadUpdateMark(PID_SGITEM_Color_a))
			pSelGroup->ExecuteUpdateItem(PID_SGITEM_Color_a, color.a, bIsMerge);
	}
}

bool ApxObjSpotLight::OnGetPos(A3DVECTOR3& vOutPos) const
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		vOutPos = m_pLPPSpotLight->GetPos();
		return true; 
	}
	else
		return false;
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		vOutPos = lp.Position;
		return true; 
	}
	else
		return false;
#endif
}

bool ApxObjSpotLight::OnGetDir(A3DVECTOR3& vOutDir) const
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		vOutDir = m_pLPPSpotLight->GetDir();
		return true; 
	}
	else
		return false;
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		vOutDir = lp.Direction;
		return true; 
	}
	else
		return false;
#endif
}

bool ApxObjSpotLight::OnGetUp(A3DVECTOR3& vOutUp) const
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		vOutUp.Set(0.0f, 1.0f, 0.0f);
		vOutUp -= DotProduct(vOutUp, m_pLPPSpotLight->GetDir()) * m_pLPPSpotLight->GetDir();
		vOutUp.Normalize();
		return true; 
	}
	else
		return false;
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		A3DVECTOR3 vDir = lp.Direction;
		vOutUp.Set(0.0f, 1.0f, 0.0f);
		vOutUp -= DotProduct(vOutUp, vDir) * vDir;
		vOutUp.Normalize();
		return true; 
	}
	else
		return false;
#endif
}

bool ApxObjSpotLight::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		A3DVECTOR3 vPos, vUp, vDir, vRight;
		vPos = m_pLPPSpotLight->GetPos();
		vDir = m_pLPPSpotLight->GetDir();
		vUp.Set(0.0f, 1.0f, 0.0f);
		vUp -= DotProduct(vUp, vDir) * vDir;
		vUp.Normalize();
		vRight = CrossProduct(vUp, vDir);

		matOutPose.SetRow(0, vRight);
		matOutPose.SetRow(1, vUp);
		matOutPose.SetRow(2, vDir);
		matOutPose.SetRow(3, vPos);
		outWithScaled = false;
		return true; 
	}
	else
		return false;
#else
	if (0 != m_pSpotLight)
	{
		A3DVECTOR3 vPos, vUp, vDir, vRight;
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		vPos = lp.Position;
		vDir = lp.Direction;
		vUp.Set(0.0f, 1.0f, 0.0f);
		vUp -= DotProduct(vUp, vDir) * vDir;
		vUp.Normalize();
		vRight = CrossProduct(vUp, vDir);

		matOutPose.SetRow(0, vRight);
		matOutPose.SetRow(1, vUp);
		matOutPose.SetRow(2, vDir);
		matOutPose.SetRow(3, vPos);
		outWithScaled = false;
		return true; 
	}
	else
		return false;
#endif
}

void ApxObjSpotLight::OnSetPos(const A3DVECTOR3& vPos)
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetPos(vPos);
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Position = vPos;
		m_pSpotLight->SetLightParam(lp);
	}
#endif
}

void ApxObjSpotLight::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		m_pLPPSpotLight->SetDir(vDir);
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Direction = vDir;
		m_pSpotLight->SetLightParam(lp);
	}
#endif
}

void ApxObjSpotLight::OnSetPose(const A3DMATRIX4& matPose)
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		m_pLPPSpotLight->SetPos(matPose.GetRow(3));
		m_pLPPSpotLight->SetDir(matPose.GetRow(2));
	}
#else
	if (0 != m_pSpotLight)
	{
		A3DLIGHTPARAM lp = m_pSpotLight->GetLightparam();
		lp.Position = matPose.GetRow(3);
		lp.Direction = matPose.GetRow(2);
		m_pSpotLight->SetLightParam(lp);
	}
#endif
}

bool ApxObjSpotLight::OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
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

bool ApxObjSpotLight::OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	if (0 != m_pScene)
	{
#ifdef _ANGELICA3
		if (CreateSpotLight())
			m_pScene->GetRender()->GetWorldRenderer()->GetLPPRender()->GetLightMan()->AddLight(m_pLPPSpotLight);
#else
		A3DLightMan* pLightMan = m_pScene->GetRender()->GetA3DEngine()->GetA3DLightMan();
		CreateSpotLight(*pLightMan);
#endif
	}
	return IObjBase::OnDoWakeUpTask(eu, pPSToRuntime);

}

void ApxObjSpotLight::OnDoSleepTask()
{
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
	{
		if(m_pScene)
			m_pScene->GetRender()->GetWorldRenderer()->GetLPPRender()->GetLightMan()->RemoveLight(m_pLPPSpotLight);

		delete m_pLPPSpotLight;
		m_pLPPSpotLight = NULL;
	}
#else
	if (0 != m_pSpotLight)
	{
		A3DLightMan* pLightMan = m_pScene->GetRender()->GetA3DEngine()->GetA3DLightMan();
		if (0 != pLightMan)
		{
			pLightMan->ReleaseLight(m_pSpotLight);
			m_pSpotLight = 0;
		}
	}
#endif
}

void ApxObjSpotLight::Enable(bool bEnable)
{ 
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight) m_pLPPSpotLight->Enable(bEnable);
#else
	if (0 != m_pSpotLight)
	{
		if (bEnable)
			m_pSpotLight->TurnOn();
		else
			m_pSpotLight->TurnOff();
	}
#endif
}

bool ApxObjSpotLight::IsEnabled() const
{ 
#ifdef _ANGELICA3
	if (0 != m_pLPPSpotLight)
		return m_pLPPSpotLight->IsEnabled();
	else
		return false;
#else
	if (0 != m_pSpotLight)
		return m_pSpotLight->IsOn();
	else
		return false;
#endif
}
