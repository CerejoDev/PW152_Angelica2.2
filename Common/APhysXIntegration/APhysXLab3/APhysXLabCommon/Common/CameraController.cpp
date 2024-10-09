/*
 * FILE: CameraController.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/03
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

#include <zmouse.h>
#include "CameraController.h"

const A3DVECTOR3 CCameraController::g_vUp(0, 1, 0);
const float CCameraController::g_DefFactor = 9;
const float CCameraController::g_MinFactor = 1;
const float CCameraController::g_MaxFactor = 30;

CECWrapper::CECWrapper()
{
	m_cecData.nMode = MODE_FREE_FLY;
}

void CECWrapper::InitCamPose(A3DCamera& cam)
{
	m_cecData.camPosBack = cam.GetPos();
	m_cecData.camDirBack = cam.GetDir();
	m_cecData.camUpBack  = cam.GetUp();
}

void CECWrapper::SyncCoreDataFrom(const CameraExtCtrl& inOther, RecItemType dirtyItem)
{
	switch(dirtyItem)
	{
	case RIT_CEC_MODE:
		m_cecData.nMode	= inOther.nMode;
		break;
	case RIT_CEC_RADIUS:
		m_cecData.fRadius = inOther.fRadius;
		break;
	case RIT_CEC_FOCUSPOS:
		m_cecData.vFocusPos = inOther.vFocusPos;
		break;
	case RIT_CEC_BACKPOS:
		m_cecData.camPosBack = inOther.camPosBack;
		break;
	case RIT_CEC_BACKDIR:
		m_cecData.camDirBack = inOther.camDirBack;
		break;
	case RIT_CEC_BACKUP:
		m_cecData.camUpBack = inOther.camUpBack;
		break;
	case RIT_CEC_BACK_FLAG:
		m_cecData.bHasBackUpPose = inOther.bHasBackUpPose;
		break;
	case RIT_SIGN_UNKNOWN:
	case RIT_SIGN_END:
		m_cecData = inOther;
	}
}

void CECWrapper::SyncCoreDataTo(CameraExtCtrl& outOther, RecItemType dirtyItem) const
{
	switch(dirtyItem)
	{
	case RIT_CEC_MODE:
		outOther.nMode = m_cecData.nMode;
		break;
	case RIT_CEC_RADIUS:
		outOther.fRadius = m_cecData.fRadius;
		break;
	case RIT_CEC_FOCUSPOS:
		outOther.vFocusPos = m_cecData.vFocusPos;
		break;
	case RIT_CEC_BACKPOS:
		outOther.camPosBack = m_cecData.camPosBack;
		break;
	case RIT_CEC_BACKDIR:
		outOther.camDirBack = m_cecData.camDirBack;
		break;
	case RIT_CEC_BACKUP:
		outOther.camUpBack = m_cecData.camUpBack;
		break;
	case RIT_CEC_BACK_FLAG:
		outOther.bHasBackUpPose = m_cecData.bHasBackUpPose;
		break;
	case RIT_SIGN_UNKNOWN:
	case RIT_SIGN_END:
		outOther = m_cecData;
	}
}

bool CECWrapper::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	if(nVersion >= 0xCF000001)
	{
		stream.readBuffer(&m_cecData.bHasBackUpPose, sizeof(bool));
		stream.readBuffer(&m_cecData.nMode, sizeof(CameraMode));
		m_cecData.fRadius = stream.readFloat();
		stream.readBuffer(&m_cecData.vFocusPos, sizeof(A3DVECTOR3));
		stream.readBuffer(&m_cecData.camPosBack, sizeof(A3DVECTOR3));
		stream.readBuffer(&m_cecData.camDirBack, sizeof(A3DVECTOR3));
		stream.readBuffer(&m_cecData.camUpBack, sizeof(A3DVECTOR3));
	}
	return true;
}

void CECWrapper::SaveData(NxStream& stream)
{
	stream.storeDword(GetVersion());
	stream.storeBuffer(&m_cecData.bHasBackUpPose, sizeof(bool));
	stream.storeBuffer(&m_cecData.nMode, sizeof(CameraMode));
	stream.storeFloat(m_cecData.fRadius);
	stream.storeBuffer(&m_cecData.vFocusPos, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_cecData.camPosBack, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_cecData.camDirBack, sizeof(A3DVECTOR3));
	stream.storeBuffer(&m_cecData.camUpBack, sizeof(A3DVECTOR3));
}

void CECWrapper::SetMode(CameraMode cm, int* pInOutItemCodes)
{
	int newCM = (int)cm;
	if (m_cecData.nMode == newCM)
		return;

	m_cecData.nMode = (int)cm;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_MODE;
}

bool CECWrapper::SetRadius(float r, int* pInOutItemCodes)
{
	if (CCameraController::g_MinFactor > r)
		r = CCameraController::g_MinFactor;
	if (CCameraController::g_MaxFactor < r)
		r = CCameraController::g_MaxFactor;
	if (NxMath::equals(r, m_cecData.fRadius, APHYSX_FLOAT_EPSILON))
		return false;

	m_cecData.fRadius = r;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_RADIUS;
	return true;
}

void CECWrapper::SetFocusPos(const A3DVECTOR3& fp, int* pInOutItemCodes)
{
	if (NxMath::equals(fp.x, m_cecData.vFocusPos.x, APHYSX_FLOAT_EPSILON))
	{
		if (NxMath::equals(fp.y, m_cecData.vFocusPos.y, APHYSX_FLOAT_EPSILON))
		{
			if (NxMath::equals(fp.z, m_cecData.vFocusPos.z, APHYSX_FLOAT_EPSILON))
				return;
		}
	}

	m_cecData.vFocusPos = fp;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_FOCUSPOS;
}

void CECWrapper::SetCamBackUpFlag(bool bVal, int* pInOutItemCodes)
{
	m_cecData.bHasBackUpPose = bVal;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_BACK_FLAG;
}

void CECWrapper::SetCamBackUpPos(const A3DVECTOR3& p, int* pInOutItemCodes)
{
	m_cecData.camPosBack = p;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_BACKPOS;
}

void CECWrapper::SetCamBackUpDir(const A3DVECTOR3& d, int* pInOutItemCodes)
{
	m_cecData.camDirBack = d;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_BACKDIR;
}

void CECWrapper::SetCamBackUpUp(const A3DVECTOR3& u, int* pInOutItemCodes)
{
	m_cecData.camUpBack = u;
	if (0 != pInOutItemCodes)
		*pInOutItemCodes |= IC_BACKUP;
}

void CECWrapper::SetCamBackUp(const A3DVECTOR3& p, const A3DVECTOR3& d, const A3DVECTOR3& u, int* pInOutItemCodes)
{
	m_cecData.bHasBackUpPose = true;
	m_cecData.camPosBack = p;
	m_cecData.camDirBack = d;
	m_cecData.camUpBack = u;
	if (0 != pInOutItemCodes)
	{
		*pInOutItemCodes |= IC_BACK_FLAG;
		*pInOutItemCodes |= IC_BACKPOS;
		*pInOutItemCodes |= IC_BACKDIR;
		*pInOutItemCodes |= IC_BACKUP;
	}
}

CCameraController::CCameraController(A3DCamera& InitedCamera, TString* pName) : m_Camera(InitedCamera)
{
	assert(g_DefFactor >= g_MinFactor);
	assert(g_DefFactor <= g_MaxFactor);

	m_ItemChangeFlags = 0;
	m_UserInputFreFlyFlags = CMF_NONE;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bEnableMove = true;
	m_CoreData.InitCamPose(InitedCamera);
	m_CoreData.SetRadius(g_DefFactor, 0);
	RaiseFlag(Object_Flag_NoDeleteAsChild);
	m_pTerr = 0;

	TString strName(_T("Camera"));
	if (0 != pName)
	{
		if (0 < pName->GetLength())
			strName = *pName;
	}
	InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_CAMERA, *this, -1);
	SetProperties(*this);
	SetObjName(strName);
	SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
}

CCameraController::~CCameraController()
{
	Release();
}

void CCameraController::SetScreenSize(const int x, const int y)
{
	m_nWidth = x;
	m_nHeight = y;
}

void CCameraController::SetUserInputFlagsForTickFreeFly(int flags)
{
	assert(MODE_FREE_FLY == m_CoreData.GetMode());
	m_UserInputFreFlyFlags = flags;
}

void CCameraController::LogChanges(CRecording& rec)
{
	int nCheckCount = 0;
	int nCount = GetCountOfBitValueIsOne(m_ItemChangeFlags);
	rec.LogItemData(RIT_COUNT, AutoData(nCount));
	if (IC_MODE & m_ItemChangeFlags)
	{
		int cm = (int)m_CoreData.GetMode();
		rec.LogItemData(RIT_CEC_MODE, AutoData(cm));
		++nCheckCount;
	}
	if (IC_RADIUS & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_CEC_RADIUS, AutoData(m_CoreData.GetRadius()));
		++nCheckCount;
	}
	if (IC_FOCUSPOS & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_CEC_FOCUSPOS, AutoData(m_CoreData.GetFocusPos()));
		++nCheckCount;
	}
	if (IC_POS & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_POS, AutoData(GetGPos()));
		++nCheckCount;
	}
	if (IC_DIR & m_ItemChangeFlags)
	{
		A3DVECTOR3 vDir, vUp; 
		GetGDirAndUp(vDir, vUp);
		rec.LogItemData(RIT_DIR, AutoData(vDir));
		++nCheckCount;
	}
	if (IC_UP & m_ItemChangeFlags)
	{
		A3DVECTOR3 vDir, vUp; 
		GetGDirAndUp(vDir, vUp);
		rec.LogItemData(RIT_UP, AutoData(vUp));
		++nCheckCount;
	}
	if (IC_BACKPOS & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_CEC_BACKPOS, AutoData(m_CoreData.GetCamBKPos()));
		++nCheckCount;
	}
	if (IC_BACKDIR & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_CEC_BACKDIR, AutoData(m_CoreData.GetCamBKDir()));
		++nCheckCount;
	}
	if (IC_BACKUP & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_CEC_BACKUP, AutoData(m_CoreData.GetCamBKUp()));
		++nCheckCount;
	}
	if (IC_BACK_FLAG & m_ItemChangeFlags)
	{
		rec.LogItemData(RIT_CEC_BACK_FLAG, AutoData(m_CoreData.GetCamBackUpFlag()));
		++nCheckCount;
	}
	assert(nCount == nCheckCount);
	ClearItemChangeFlags();
}

int CCameraController::ReproChanges(CRecording& rec)
{
	AutoData reproValue(0);
	bool bRtn = rec.ReproItemData(RIT_COUNT, &reproValue);
	assert(true == bRtn);

	int nCheckFlag = 0;
	RecItemType rit = RIT_SIGN_UNKNOWN;
	int nCount = reproValue.nVal;
	for (int i = 0; i < nCount; ++i)
	{
		bRtn = rec.ReproItemData(rit, reproValue);
		assert(true == bRtn);
		switch(rit)
		{
		case RIT_CEC_MODE:
			m_CoreData.SetMode(CameraMode(reproValue.nVal), &m_ItemChangeFlags);
			nCheckFlag |= IC_MODE;
			break;
		case RIT_CEC_RADIUS:
			m_CoreData.SetRadius(reproValue.fVal, &m_ItemChangeFlags);
			nCheckFlag |= IC_RADIUS;
			break;
		case RIT_CEC_FOCUSPOS:
			m_CoreData.SetFocusPos(reproValue.vecVal.GetA3DVector3(), &m_ItemChangeFlags);
			nCheckFlag |= IC_FOCUSPOS;
			break;
		case RIT_POS:
			SetGPos(reproValue.vecVal.GetNxVec3());
			nCheckFlag |= IC_POS;
			break;
		case RIT_DIR:
			{
			NxVec3 dir = reproValue.vecVal.GetNxVec3();
			++i;
			RecItemType ritUp = RIT_SIGN_UNKNOWN;
			bRtn = rec.ReproItemData(ritUp, reproValue);
			assert(true == bRtn);
			assert(RIT_UP == ritUp);
			NxVec3 up = reproValue.vecVal.GetNxVec3();
			SetGDirAndUp(dir, up);
			nCheckFlag |= IC_DIR;
			nCheckFlag |= IC_UP;
			}
			break;
		case RIT_UP:
			assert(true == false);
			break;
		case RIT_CEC_BACKPOS:
			m_CoreData.SetCamBackUpPos(reproValue.vecVal.GetA3DVector3(), &m_ItemChangeFlags);
			nCheckFlag |= IC_BACKPOS;
			break;
		case RIT_CEC_BACKDIR:
			m_CoreData.SetCamBackUpDir(reproValue.vecVal.GetA3DVector3(), &m_ItemChangeFlags);
			nCheckFlag |= IC_BACKDIR;
			break;
		case RIT_CEC_BACKUP:
			m_CoreData.SetCamBackUpUp(reproValue.vecVal.GetA3DVector3(), &m_ItemChangeFlags);
			nCheckFlag |= IC_BACKUP;
			break;
		case RIT_CEC_BACK_FLAG:
			m_CoreData.SetCamBackUpFlag(reproValue.bVal, &m_ItemChangeFlags);
			nCheckFlag |= IC_BACK_FLAG;
			break;
		default:
			assert(!"Shouldn't be here! Debug it!");
		}
	}
	assert(m_ItemChangeFlags == nCheckFlag);
	ClearItemChangeFlags();
	return nCheckFlag;
}

void CCameraController::SyncCameraData(const CCameraController& other, int changeFlags)
{
	if (IC_MODE & changeFlags)
	{
		CameraMode cm = other.GetMode();
		m_CoreData.SetMode(cm, 0);
		IObjBase* pFocus = other.GetFocusObj();
		if (pFocus != GetFocusObj())
			SetFocusObj(pFocus);
	}
	if (IC_RADIUS & changeFlags)
	{
		m_CoreData.SetRadius(other.m_CoreData.GetRadius(), 0);
	}
	if (IC_FOCUSPOS & changeFlags)
	{
		m_CoreData.SetFocusPos(other.m_CoreData.GetFocusPos(), 0);
	}
	if (IC_POS & changeFlags)
	{
		SetGPos(other.GetGPos());
	}
	bool bUpdatedDir = false;
	if (IC_DIR & changeFlags)
	{
		A3DVECTOR3 vDir, vUp; 
		other.GetGDirAndUp(vDir, vUp);
		SetGDirAndUp(vDir, vUp);
		bUpdatedDir = true;
	}
	if (IC_UP & changeFlags)
	{
		if (!bUpdatedDir)
		{
			A3DVECTOR3 vDir, vUp; 
			other.GetGDirAndUp(vDir, vUp);
			SetGDirAndUp(vDir, vUp);
		}
	}
	if (IC_BACKPOS & changeFlags)
	{
		m_CoreData.SetCamBackUpPos(other.m_CoreData.GetCamBKPos(), 0);
	}
	if (IC_BACKDIR & changeFlags)
	{
		m_CoreData.SetCamBackUpDir(other.m_CoreData.GetCamBKDir(), 0);
	}
	if (IC_BACKUP & changeFlags)
	{
		m_CoreData.SetCamBackUpUp(other.m_CoreData.GetCamBKUp(), 0);
	}
	if (IC_BACK_FLAG & changeFlags)
	{
		m_CoreData.SetCamBackUpFlag(other.m_CoreData.GetCamBackUpFlag(), 0);
	}
	if (0xffffffff == changeFlags)
	{
		SetTerrain(other.GetTerrain());
		SetScreenSize(other.m_nWidth, other.m_nHeight);
	}
	ClearItemChangeFlags();
}

void CCameraController::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropObjBase::EnterRuntimePreNotify(aPhysXScene);
	CCameraController* pBK = dynamic_cast<CCameraController*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void CCameraController::LeaveRuntimePostNotify()
{
	IPropObjBase::LeaveRuntimePostNotify();
	CCameraController* pBK = dynamic_cast<CCameraController*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

void CCameraController::SetFocusPosition(const A3DVECTOR3& pos)
{
	m_CoreData.SetFocusPos(pos + A3DVECTOR3(0, 0.8f, 0), &m_ItemChangeFlags);
}

A3DVECTOR3& CCameraController::GetPosGroundCull(A3DVECTOR3& Pos, A3DTerrain2& Terrain) const
{
	float fHeight = 0.0f;
	if (GetTerrainHeight(&Terrain, Pos, fHeight))
		a_ClampFloor(Pos.y, fHeight + 1);
	return Pos;
}

void CCameraController::AddTerrainHeight(A3DTerrain2& Terrain)
{
	A3DVECTOR3 vPos = m_Camera.GetPos();
	GetPosGroundCull(vPos, Terrain);
	SetGPos(vPos);
}

bool CCameraController::GetDefBindPos(A3DVECTOR3& outFocusPos, A3DViewport* pVP) const
{
	assert(m_nWidth > 0);
	assert(m_nHeight > 0);
	if (0 >= m_nWidth) return false;
	if (0 >= m_nHeight) return false;

	if (m_pTerr == NULL)
	{
		outFocusPos = m_Camera.GetPos() + m_Camera.GetDir() * 10;
		return true;
	}

	A3DVECTOR3 cameraDir = m_Camera.GetDir();
	if (0 < cameraDir.y)
	{
		cameraDir.y = -cameraDir.y;
		a_ClampRoof(cameraDir.y, -0.1f);
		cameraDir.Normalize();
	}

	if (0 == pVP)
	{
		outFocusPos = m_Camera.GetPos() + cameraDir * 10;
		GetTerrainHeight(m_pTerr, outFocusPos, outFocusPos.y);
	}
	else
	{
		A3DVECTOR3 cameraPos = m_Camera.GetPos();
		A3DVECTOR3 ZInfinite(float(m_nWidth) * 0.5f, float(m_nHeight) * 0.5f, 1.0f);
		pVP->InvTransform(ZInfinite, ZInfinite);
		A3DVECTOR3 vDelta = ZInfinite - cameraPos;

		RAYTRACERT rt;
		if (!m_pTerr->RayTrace(cameraPos, vDelta, 1, &rt))
			return false;

		outFocusPos = rt.vHitPos;
	}
	return true;
}

void CCameraController::EntryBindingMode(IObjBase* pBindObj, const bool bStorePose, const bool IsKeepDir)
{
	A3DVECTOR3  posBack = m_Camera.GetPos();
	A3DVECTOR3  dirBack = m_Camera.GetDir();
	A3DVECTOR3  upBack = m_Camera.GetUp();
	if (!SetFocusObj(pBindObj))
		return;

	if (bStorePose)
	{
		if (!m_CoreData.GetCamBackUpFlag())
			m_CoreData.SetCamBackUp(posBack, dirBack, upBack, &m_ItemChangeFlags);
	}
}

void CCameraController::SetBindPosition(bool bKeepDir)
{
	IObjBase* pFocus = GetFocusObj();
	if (0 == pFocus)
		return;

	m_CoreData.SetMode(MODE_BINDING, &m_ItemChangeFlags);
	A3DVECTOR3 vBindPos = APhysXConverter::N2A_Vector3(pFocus->GetGPos());
	SetFocusPosition(vBindPos);

	const A3DVECTOR3& vFocusPos = m_CoreData.GetFocusPos();
	A3DVECTOR3 cameraDir = m_Camera.GetDir();
	if (!bKeepDir)
	{
		cameraDir = vFocusPos - m_Camera.GetPos();
		cameraDir.Normalize();
		A3DVECTOR3 up = g_vUp;
		float angle = DotProduct(cameraDir, up);
		if ((0.6f > angle) && (-0.6 < angle))
		{
			A3DVECTOR3 vRight = CrossProduct(up, cameraDir);
			up = CrossProduct(cameraDir, vRight);
		}
		else
		{
			A3DVECTOR3 vRight = CrossProduct(cameraDir, A3DVECTOR3(0, 0, 1));
			up = CrossProduct(cameraDir, vRight);
		}
		SetGDirAndUp(cameraDir, up);
	}

	A3DVECTOR3 cameraPos;
	if (0 == m_pTerr)
		cameraPos = vFocusPos - cameraDir * m_CoreData.GetRadius();
	else
		cameraPos = CalculateCameraPosition(*m_pTerr);
	SetGPos(cameraPos);
}

void CCameraController::EntryFreeFlyMode(const bool bRestorePose)
{
	m_CoreData.SetMode(MODE_FREE_FLY, &m_ItemChangeFlags);
	SetFocusObj(0);
	if (bRestorePose)
	{
		if (m_CoreData.GetCamBackUpFlag())
		{
			m_CoreData.SetCamBackUpFlag(false, &m_ItemChangeFlags);
			SetGPos(m_CoreData.GetCamBKPos());
			SetGDirAndUp(m_CoreData.GetCamBKDir(), m_CoreData.GetCamBKUp());
		}
	}
	else
	{
		if (!m_CoreData.GetCamBackUpFlag())
			m_CoreData.SetCamBackUp(m_Camera.GetPos(), m_Camera.GetDir(), m_Camera.GetUp(), &m_ItemChangeFlags);
	}
}

float CCameraController::GetFocusFactorPercent() const
{
	if (!QueryMode(MODE_BINDING))
		return 1.0f;

	float radius = m_CoreData.GetRadius();
	float d = (radius - CCameraController::g_MinFactor) / (CCameraController::g_MaxFactor - CCameraController::g_MinFactor);
	a_Clamp(d, 0.0f, 1.0f);
	return d;
}

void CCameraController::UpdateFocusFactor(const int zDelta)
{
	if (!QueryMode(MODE_BINDING))
		return;

	const int nDelta = zDelta / WHEEL_DELTA;
	float radius = m_CoreData.GetRadius();
	radius -= nDelta;
	if (!m_CoreData.SetRadius(radius, &m_ItemChangeFlags))
		return;

	A3DVECTOR3 cameraPos;
	if (0 == m_pTerr)
		cameraPos = m_Camera.GetPos() + m_Camera.GetDir() * nDelta;
	else
		cameraPos = CalculateCameraPosition(*m_pTerr);
	SetGPos(cameraPos);
}

void CCameraController::ApplyUserInputForFreeFly(int cmfs, float deltaTimeSec)
{
	assert(MODE_FREE_FLY == m_CoreData.GetMode());
	A3DVECTOR3 vDirH = m_Camera.GetDir();
	A3DVECTOR3 vRightH = m_Camera.GetRight();
	A3DVECTOR3 vUpH = m_Camera.GetUp();

	A3DVECTOR3 vDelta(0.0f);
	if (CMF_MOVE_FORWARD & cmfs)
		vDelta += vDirH;
	if (CMF_MOVE_BACKWARD & cmfs)
		vDelta -= vDirH;

	if (CMF_MOVE_LEFTSTRAFE & cmfs)
		vDelta -= vRightH;
	if (CMF_MOVE_RIGHTSTRAFE & cmfs)
		vDelta += vRightH;

	if (CMF_MOVE_UP & cmfs)
		vDelta += vUpH;
	if (CMF_MOVE_DOWN & cmfs)
		vDelta -= vUpH;

	float fSpeed = 20.0f;
	if (CMF_HIGH_SPEED & cmfs)
		fSpeed *= 2.5f;

	vDelta = a3d_Normalize(vDelta) * (fSpeed * deltaTimeSec);
	static const float Epsilon = 0.00001f;
	bool bMoveCamera = false;
	if (Epsilon <= abs(vDelta.x))
		bMoveCamera = true;
	if (Epsilon <= abs(vDelta.y))
		bMoveCamera = true;
	if (Epsilon <= abs(vDelta.z))
		bMoveCamera = true;
	if (bMoveCamera)
	{
		A3DVECTOR3 vPos = m_Camera.GetPos();
		A3DVECTOR3 vNewPos = vPos + vDelta;
		if (0 != m_pTerr)
			GetPosGroundCull(vNewPos, *m_pTerr);
		SetGPos(vNewPos);
	}
}

void CCameraController::RotateCamera(const int dx, const int dy)
{
	if (QueryMode(MODE_FREE_FLY))
	{
		RotateCameraFreeFly(dx, dy);
		return;
	}
	
	RotateCameraBinding(dx, dy, m_pTerr);
}

void CCameraController::RotateCameraFreeFly(const int dx, const int dy)
{
	const float f1 = 0.03f;
	if (0 != dx)
		m_Camera.DegDelta(dx * f1);
	
	if (0 != dy)
		m_Camera.PitchDelta(-dy * f1);

	SetGPos(m_Camera.GetPos());
	SetGDirAndUp(m_Camera.GetDir(), m_Camera.GetUp());
}

bool CCameraController::HasVerticalRotation(const int dy, const A3DVECTOR3& vDir)
{
	static const float Epsilon = 0.05f;

	if (0 == dy)
		return false;

	if ((Epsilon > (1 + vDir.y)) && (0 > dy))
	{
		if (Epsilon > abs(vDir.x))
			return false;
	}

	if ((Epsilon > (1 - vDir.y)) && (0 < dy))
	{
		if (Epsilon > abs(vDir.x))
			return false;
	}

	return true;
}

void CCameraController::RotateCameraBinding(const int dx, const int dy, A3DTerrain2* pTerrain)
{
	const float f1 = 0.01f;

	bool bIsChanged = false;
	A3DVECTOR3 cameraPos = m_Camera.GetPos();
	A3DVECTOR3 cameraDir = m_Camera.GetDir();
	const A3DVECTOR3& vFocusPos = m_CoreData.GetFocusPos();

	if (0 != dx)
	{
		bIsChanged = true;
		cameraPos = a3d_RotatePosAroundLine(cameraPos, vFocusPos, g_vUp, dx * f1);
	}

	if (HasVerticalRotation(dy, cameraDir))
	{
		bIsChanged = true;
		A3DVECTOR3 axis = m_Camera.GetRight();
		cameraPos = a3d_RotatePosAroundLine(cameraPos, vFocusPos, axis, -dy * f1); 
	}

	if (!bIsChanged)
		return;

	cameraDir = vFocusPos - cameraPos;
	cameraDir.Normalize();
	A3DVECTOR3 xaxis = CrossProduct(m_Camera.GetUp(), cameraDir);
	A3DVECTOR3 up = CrossProduct(cameraDir, xaxis);
	SetGDirAndUp(cameraDir, up);
	
	if (0 != pTerrain)
		cameraPos = CalculateCameraPosition(*pTerrain);
	SetGPos(cameraPos);
}

A3DVECTOR3 CCameraController::CalculateCameraPosition(A3DTerrain2& Terrain) const
{
	const A3DVECTOR3& vFocusPos = m_CoreData.GetFocusPos();
	A3DVECTOR3 vDir = m_Camera.GetDir() * -1;

	float fovY, front, back, ratio; 
	m_Camera.GetProjectionParam(&fovY, &front, &back, &ratio);
	float yHalfSide = front * tan(fovY * 0.5f);
	float xHalfSide = yHalfSide * ratio;

	// calculate the left-bottom and right-bottom position...
	// in the near plane of viewing frustum
	A3DVECTOR3 xAxis = CrossProduct(g_vUp, vDir);
	xAxis.Normalize();
	A3DVECTOR3 vBottomMiddle = vFocusPos + m_Camera.GetUp() * -1 * yHalfSide;
	A3DVECTOR3 vBottomLeft  = vBottomMiddle - xAxis * xHalfSide;
	A3DVECTOR3 vBottomRight = vBottomMiddle + xAxis * xHalfSide;

	// do the raycast to get the closest information
	float radius = m_CoreData.GetRadius();
	RAYTRACERT* pRT = 0;
	RAYTRACERT rtLeft;
	RAYTRACERT rtRight;
	bool bLeft  = Terrain.RayTrace(vBottomLeft, vDir, radius, &rtLeft);
	bool bRight = Terrain.RayTrace(vBottomRight, vDir, radius, &rtRight);

	if (!bLeft && !bRight)
		return vFocusPos + vDir * (radius + front);

	if (!bLeft && bRight)
		pRT = &rtRight;
	else if (bLeft && !bRight)
		pRT = &rtLeft;
	else
		pRT = (rtLeft.fFraction < rtRight.fFraction)? &rtLeft : &rtRight;

	// adjust the result according to the angle
	float theta = DEG2RAD(90) - acos(DotProduct(m_Camera.GetDir(), pRT->vNormal));
	float disH = yHalfSide / tan(theta);

	A3DVECTOR3 vDis = vDir * (radius * pRT->fFraction - disH);
	return vFocusPos + vDis;
}

void CCameraController::OnUpdateGRot()
{
	A3DVECTOR3 vDir, vUp; 
	GetGDirAndUp(vDir, vUp);
	m_Camera.SetDirAndUp(vDir, g_vUp); // note: the camera always using g_vUp
	m_ItemChangeFlags |= IC_DIR;
	m_ItemChangeFlags |= IC_UP;
}

void CCameraController::OnUpdateGPos()
{
	A3DVECTOR3 vPos = APhysXConverter::N2A_Vector3(GetGPos());
	m_Camera.SetPos(vPos);
	m_ItemChangeFlags |= IC_POS;
}

ApxObjBase* CCameraController::NewObjAndCloneData() const
{
	assert(!"can not clone camera controller!");
	return NULL;
}

void CCameraController::OnUpdatePosChanged(const NxVec3& deltaMove)
{
	assert(0 == GetParent());
	SetBindPosition(true);
}

void CCameraController::OnFocusObjChanged()
{
	SetBindPosition(true);
}

void CCameraController::OnTick(float dt)
{
	if (!m_bEnableMove)
		return;

	if (QueryMode(MODE_FREE_FLY))
		ApplyUserInputForFreeFly(m_UserInputFreFlyFlags, dt);
	m_UserInputFreFlyFlags = 0;
}

bool CCameraController::OnGetPos(A3DVECTOR3& vOutPos) const
{
	vOutPos = GetPos();
	return true;
}

void CCameraController::OnSetPos(const A3DVECTOR3& vPos)
{
	assert(!"Shouldn't be here! Debug it!");
}

bool CCameraController::OnGetDir(A3DVECTOR3& vOutDir) const
{
	vOutDir = m_Camera.GetDir();
	return true;
}

bool CCameraController::OnGetUp(A3DVECTOR3& vOutUp) const
{
	vOutUp = m_Camera.GetUp();
	return true;
}

void CCameraController::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	assert(!"Shouldn't be here! Debug it!");
}

bool CCameraController::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	matOutPose = m_Camera.GetVPTM();
	outWithScaled = false;
	return true;
}

void CCameraController::OnSetPose(const A3DMATRIX4& matPose)
{
	assert(!"Shouldn't be here! Debug it!");
}

ICamOptTask::ICamOptTask()
{
	m_pWorker = 0;
}

bool ICamOptTask::ExecuteTask()
{
	if (0 == m_pWorker)
		return false;

	OnExecuteTask(*m_pWorker);
	return true;
}

void CamOptTask_EntryFreeFlyMode::OnExecuteTask(CCameraController& worker)
{
	worker.EntryFreeFlyMode(m_bRestorePose);
}

CamOptTask_EntryBindingMode::CamOptTask_EntryBindingMode(IObjBase* pBindObj, const bool bStorePose, const bool IsKeepDir)
{
	m_pBindObj = pBindObj;
	m_bStorePose = bStorePose;
	m_bIsKeepDir = IsKeepDir;
}

void CamOptTask_EntryBindingMode::OnExecuteTask(CCameraController& worker)
{
	worker.EntryBindingMode(m_pBindObj, m_bStorePose, m_bIsKeepDir);
}

void CamOptTask_RotateCamera::OnExecuteTask(CCameraController& worker)
{
	worker.RotateCamera(m_dx, m_dy);
}

void CamOptTask_UpdateFocusFactor::OnExecuteTask(CCameraController& worker)
{
	worker.UpdateFocusFactor(m_zDelta);
}

CCamCtrlAgent::CCamCtrlAgent(CCameraController& normalCam, CRecording& InitRec) : m_Recording(InitRec)
{
	m_bIsSameReplay = false;
	m_pNormalCam = &normalCam;
	m_pReplayCam = 0;
}

CCamCtrlAgent::~CCamCtrlAgent()
{
	delete m_pReplayCam;
}

void CCamCtrlAgent::NotifyReplayBegin(A3DCamera& reproCam)
{
	TString strname(_T("ReproCam"));
	m_pReplayCam = new CCameraController(reproCam, &strname);
	if (0 != m_pReplayCam)
	{
		m_bIsSameReplay = true;
		m_pReplayCam->SyncCameraData(*m_pNormalCam, 0xffffffff);
		m_pNormalCam->ClearItemChangeFlags();
	}
}

void CCamCtrlAgent::NotifyReplayEnd()
{
	if (0 == m_pReplayCam)
		return;

	std::vector<ApxObjBase*> children;
	int nChildren = m_pReplayCam->GetChildNum();
	for (int i = 0; i < nChildren; ++i)
		children.push_back(m_pReplayCam->GetChild(i));
	for (int i = 0; i < nChildren; ++i)
		m_pNormalCam->AddChild(children[i]);

	delete m_pReplayCam;
	m_pReplayCam = 0;
	m_bIsSameReplay = false;
}

bool CCamCtrlAgent::EnforceSyncReplayCamera()
{
	if (0 == m_pReplayCam)
		return false;

	m_bIsSameReplay = true;
	m_pNormalCam->SyncCameraData(*m_pReplayCam, 0xffffffff);
	return true;
}

const CCameraController* CCamCtrlAgent::GetActiveCamera(bool bIsReferReproEvent) const
{
	if (m_Recording.IsReplaying())
	{
		if (0 == m_pReplayCam)
			return m_pNormalCam;

		if (!bIsReferReproEvent)
			return m_pReplayCam;

		if (m_Recording.IsEventReproducing())
			return m_pReplayCam;
	}
	return m_pNormalCam;
}

void CCamCtrlAgent::ExecuteCameraOperation(ICamOptTask& icot)
{
	CCameraController* pActive = m_pNormalCam;
	if (m_Recording.IsReplaying())
	{
		if (m_Recording.IsEventReproducing())
			pActive = m_pReplayCam;
		else
			m_bIsSameReplay = false;
		if (0 == pActive)
			pActive = m_pNormalCam;
	}

	assert(0 != pActive);
	pActive->ClearItemChangeFlags();
	icot.SetWorker(*pActive);
	icot.ExecuteTask();
	if (pActive == m_pReplayCam)
	{
		int flags = pActive->ReproChanges(m_Recording);
		if (m_bIsSameReplay)
			m_pNormalCam->SyncCameraData(*m_pReplayCam, flags);
	}

	if (m_Recording.IsRecording())
		m_pNormalCam->LogChanges(m_Recording);
}

void CCamCtrlAgent::Tick(float dtSec, bool bIsPause)
{
	int reproFlags = 0;
	bool bInRepro = false;
	if (0 != m_pReplayCam)
	{
		if (m_pReplayCam->QueryMode(MODE_FREE_FLY))
		{
			if (!bIsPause)
				reproFlags = TickUserInputForFreeFly(false);
			m_pReplayCam->SetUserInputFlagsForTickFreeFly(reproFlags);
		}
		m_pReplayCam->Tick(dtSec);
		bInRepro = true;
	}

	if (m_pNormalCam->QueryMode(MODE_FREE_FLY))
	{
		int flags = TickUserInputForFreeFly(bInRepro);
		if (m_bIsSameReplay)
		{
			if (0 == flags)
				flags = reproFlags;
			else
				m_bIsSameReplay = false;
		}
		m_pNormalCam->SetUserInputFlagsForTickFreeFly(flags);
	}
	m_pNormalCam->Tick(dtSec);
}

int CCamCtrlAgent::TickUserInputForFreeFly(bool bQueryOnly)
{
	int rtnFlags = CMF_NONE;

	if (m_Recording.QueryAsyncKeyDown(VK_CONTROL, bQueryOnly))
		return rtnFlags;
	if (m_Recording.QueryAsyncKeyDown(VK_MENU, bQueryOnly))
		return rtnFlags;

	if (m_Recording.QueryAsyncKeyDown('W', bQueryOnly))
		rtnFlags |= CMF_MOVE_FORWARD;
	if (m_Recording.QueryAsyncKeyDown('S', bQueryOnly))
		rtnFlags |= CMF_MOVE_BACKWARD;

	if (m_Recording.QueryAsyncKeyDown('A', bQueryOnly))
		rtnFlags |= CMF_MOVE_LEFTSTRAFE;
	if (m_Recording.QueryAsyncKeyDown('D', bQueryOnly))
		rtnFlags |= CMF_MOVE_RIGHTSTRAFE;

	if (m_Recording.QueryAsyncKeyDown('Q', bQueryOnly))
		rtnFlags |= CMF_MOVE_UP;
	if (m_Recording.QueryAsyncKeyDown('E', bQueryOnly))
		rtnFlags |= CMF_MOVE_DOWN;

	if (m_Recording.QueryAsyncKeyDown(VK_SHIFT, bQueryOnly))
		rtnFlags |= CMF_HIGH_SPEED;
	return rtnFlags;
}


