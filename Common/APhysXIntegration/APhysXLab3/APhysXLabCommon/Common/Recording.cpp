/*
 * FILE: Recording.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2012/01/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include <A3DCamera.h>
#include <A3DViewport.h>
#include "Recording.h"

#ifndef AFILE_OPENFOREDIT
	#define AFILE_OPENFOREDIT			0x00000040 
#endif

void IOptEvent::AddUserDebugString(const AString& str)
{
	AString strF = "		" + str;
	m_strDebug.push_back(strF);
}

void IOptEvent::SaveUserDebugData(ADiskFile& adfDebug)
{
	size_t nStr = m_strDebug.size();
	for (size_t i = 0; i < nStr; ++i)
		adfDebug.WriteLine(m_strDebug[i]);
	m_strDebug.clear();
}

OptKeyBoard::OptKeyBoard()
{
	m_bIsKeyDown = true;
	m_nChar = 0;
	m_nRepCnt = 0;
	m_nFlags = 0;
}

OptKeyBoard::OptKeyBoard(bool bIsDown, unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags)
{
	m_bIsKeyDown = bIsDown;
	m_nChar = nChar;
	m_nRepCnt = nRepCnt;
	m_nFlags = nFlags;
}

IOptEvent* OptKeyBoard::Clone() const
{
	return new OptKeyBoard(*this);
}

bool OptKeyBoard::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	if(nVersion >= 0x0F000001)
	{
		stream.readBuffer(&m_bIsKeyDown, sizeof(bool));
		m_nChar = stream.readDword();
		m_nRepCnt = stream.readDword();
		m_nFlags = stream.readDword();
	}
	return true;
}

void OptKeyBoard::SaveData(NxStream& stream)
{
	stream.storeDword(GetVersion());
	stream.storeBuffer(&m_bIsKeyDown, sizeof(bool));
	stream.storeDword(m_nChar);
	stream.storeDword(m_nRepCnt);
	stream.storeDword(m_nFlags);
}

void OptKeyBoard::ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const
{
	if (m_bIsKeyDown)
		uie.eventOnKeyDown(m_nChar, m_nRepCnt, m_nFlags);
	else
		uie.eventOnKeyUp(m_nChar, m_nRepCnt, m_nFlags);
}

void OptKeyBoard::SaveDebugString(ADiskFile& adfDebug) const
{
	AString strLine;
	if (m_bIsKeyDown)
		strLine.Format("	OnKeyDown  nChar = %d, nRepCnt = %d, nFlags = %d", m_nChar, m_nRepCnt, m_nFlags);
	else
		strLine.Format("	OnKeyUp  nChar = %d, nRepCnt = %d, nFlags = %d", m_nChar, m_nRepCnt, m_nFlags);
	adfDebug.WriteLine(strLine);
}

OptMouse::OptMouse()
{
	m_bMType = OMT_LBTN_DOWN;
	m_ptX = 0;
	m_ptY = 0;
	m_zDelta = 0;
	m_nFlags = 0;
}

OptMouse::OptMouse(OptMouseType omt, int ptx, int pty, unsigned int nFlags)
{
	m_bMType = omt;
	m_ptX = ptx;
	m_ptY = pty;
	m_zDelta = 0;
	m_nFlags = nFlags;
}

OptMouse::OptMouse(int ptx, int pty, int zd, unsigned int nFlags)
{
	m_bMType = OMT_WHEEL;
	m_ptX = ptx;
	m_ptY = pty;
	m_zDelta = zd;
	m_nFlags = nFlags;
}

IOptEvent* OptMouse::Clone() const
{
	return new OptMouse(*this);
}

bool OptMouse::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	if(nVersion >= 0x1F000001)
	{
		stream.readBuffer(&m_bMType, sizeof(OptMouseType));
		m_ptX = stream.readDword();
		m_ptY = stream.readDword();
		if (OMT_WHEEL == m_bMType)
			m_zDelta = stream.readDword();
		m_nFlags = stream.readDword();
	}
	return true;
}

void OptMouse::SaveData(NxStream& stream)
{
	stream.storeDword(GetVersion());
	stream.storeBuffer(&m_bMType, sizeof(OptMouseType));
	stream.storeDword(m_ptX);
	stream.storeDword(m_ptY);
	if (OMT_WHEEL == m_bMType)
		stream.storeDword(m_zDelta);
	stream.storeDword(m_nFlags);
}

void OptMouse::ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const
{
	switch (m_bMType)
	{
	case OMT_LBTN_DBLCLK:
		uie.eventOnLButtonDblClk(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_LBTN_DOWN:
		uie.eventOnLButtonDown(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_LBTN_UP:
		uie.eventOnLButtonUp(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_MBTN_DBLCLK:
		uie.eventOnMButtonDblClk(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_MBTN_DOWN:
		uie.eventOnMButtonDown(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_MBTN_UP:
		uie.eventOnMButtonUp(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_RBTN_DBLCLK:
		uie.eventOnRButtonDblClk(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_RBTN_DOWN:
		uie.eventOnRButtonDown(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_RBTN_UP:
		uie.eventOnRButtonUp(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_MOVE:
		uie.eventOnMouseMove(m_ptX, m_ptY, m_nFlags);
		break;
	case OMT_WHEEL:
		uie.eventOnMouseWheel(m_ptX, m_ptY, m_zDelta, m_nFlags);
		break;
	}
}

void OptMouse::SaveDebugString(ADiskFile& adfDebug) const
{
	AString strName;
	switch (m_bMType)
	{
	case OMT_LBTN_DBLCLK:
		strName = "OnLButtonDblClk";
		break;
	case OMT_LBTN_DOWN:
		strName = "OnLButtonDown";
		break;
	case OMT_LBTN_UP:
		strName = "OnLButtonUp";
		break;
	case OMT_MBTN_DBLCLK:
		strName = "OnMButtonDblClk";
		break;
	case OMT_MBTN_DOWN:
		strName = "OnMButtonDown";
		break;
	case OMT_MBTN_UP:
		strName = "OnMButtonDown";
		break;
	case OMT_RBTN_DBLCLK:
		strName = "OnRButtonDblClk";
		break;
	case OMT_RBTN_DOWN:
		strName = "OnRButtonDown";
		break;
	case OMT_RBTN_UP:
		strName = "OnRButtonDown";
		break;
	case OMT_MOVE:
		strName = "OnMouseMove";
		break;
	case OMT_WHEEL:
		strName = "OnMouseWheel";
		break;
	}

	AString strLine;
	if (OMT_WHEEL == m_bMType)
		strLine.Format("	%s  nFlags = %d, zDelta = %d, X = %d, Y = %d", strName, m_nFlags, m_zDelta, m_ptX, m_ptY);
	else
		strLine.Format("	%s  nFlags = %d, X = %d, Y = %d", strName, m_nFlags, m_ptX, m_ptY);
	adfDebug.WriteLine(strLine);
}

OptSize::OptSize()
{
	m_nType = 0;
	m_nCX = 0;
	m_nCY = 0;
}

OptSize::OptSize(unsigned int nType, int cx, int cy)
{
	m_nType = nType;
	m_nCX = cx;
	m_nCY = cy;
}

IOptEvent* OptSize::Clone() const
{
	return new OptSize(*this);
}

bool OptSize::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	if(nVersion >= 0x0F000001)
	{
		m_nType = stream.readDword();
		m_nCX = stream.readDword();
		m_nCY = stream.readDword();
	}
	return true;
}

void OptSize::SaveData(NxStream& stream)
{
	stream.storeDword(GetVersion());
	stream.storeDword(m_nType);
	stream.storeDword(m_nCX);
	stream.storeDword(m_nCY);
}

void OptSize::ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const
{
	AutoData ad;
	ad.szeVal.x = m_nCX;
	ad.szeVal.y = m_nCY;
	rec.OnReproCamAndVP(RIT_CVC_VPSIZE, ad);
}

void OptSize::SaveDebugString(ADiskFile& adfDebug) const
{
	AString strLine;
	strLine.Format("	OnSize  nType = %d, cx = %d, cy = %d", m_nType, m_nCX, m_nCY);
	adfDebug.WriteLine(strLine);
}

CameraExtCtrl::CameraExtCtrl()
{
	bHasBackUpPose = false;
	nMode = 0;

	fRadius = 0.0f;
	vFocusPos.Set(0, 0, 0);

	camPosBack.Set(0, 0, 0);
	camDirBack.Set(0, 0, 0);
	camUpBack.Set(0, 0, 0);
}

CameraViewportCtrl::CameraViewportCtrl()
{
	m_bHasValidateData = false;
	m_camFov = 0.0f;
	m_camFront = 0.0f;
	m_camBack = 0.0f;
	m_camRatio = 0.0f;
	m_vpX = 0;
	m_vpY = 0;
	m_vpWidth = 0;
	m_vpHeight = 0;
	m_vpMinZ = 0.0f;
	m_vpMaxZ = 0.0f;
	m_vpColorClear = 0;
}

CameraViewportCtrl::CameraViewportCtrl(const A3DCamera& cam, const A3DViewport& vp)
{
	InitCVC(cam, vp);
}

void CameraViewportCtrl::InitCVC(const A3DCamera& cam, const A3DViewport& vp)
{
	m_bHasValidateData = true;
	// todo: temp code
	A3DCamera& ncCam = const_cast<A3DCamera&>(cam);
	ncCam.GetProjectionParam(&m_camFov, &m_camFront, &m_camBack, &m_camRatio);
	// todo: temp code
	A3DViewport& ncvp = const_cast<A3DViewport&>(vp);
	A3DVIEWPORTPARAM* pPara = ncvp.GetParam();
	m_vpX = pPara->X;
	m_vpY = pPara->Y;
	m_vpWidth = pPara->Width;
	m_vpHeight = pPara->Height;
	m_vpMinZ = pPara->MinZ;
	m_vpMaxZ = pPara->MaxZ;
	m_vpColorClear = ncvp.GetClearColor();
}

bool CameraViewportCtrl::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	if(nVersion >= 0x2F000001)
	{
		m_camFov = stream.readFloat();
		m_camFront = stream.readFloat();
		m_camBack = stream.readFloat();
		m_camRatio = stream.readFloat();
		m_vpX = stream.readDword();
		m_vpY = stream.readDword();
		m_vpWidth = stream.readDword();
		m_vpHeight = stream.readDword();
		m_vpMinZ = stream.readFloat();
		m_vpMaxZ = stream.readFloat();
		m_vpColorClear = stream.readDword();
	}
	m_bHasValidateData = true;
	return true;
}

void CameraViewportCtrl::SaveData(NxStream& stream)
{
	assert(true == m_bHasValidateData);
	stream.storeDword(GetVersion());
	stream.storeFloat(m_camFov);
	stream.storeFloat(m_camFront);
	stream.storeFloat(m_camBack);
	stream.storeFloat(m_camRatio);
	stream.storeDword(m_vpX);
	stream.storeDword(m_vpY);
	stream.storeDword(m_vpWidth);
	stream.storeDword(m_vpHeight);
	stream.storeFloat(m_vpMinZ);
	stream.storeFloat(m_vpMaxZ);
	stream.storeDword(m_vpColorClear);
}

bool CameraViewportCtrl::UpdateCamAndVP(A3DCamera* pCam, A3DViewport* pVp, A3DDevice* pDevice) const
{
	if (!m_bHasValidateData)
		return false;

	A3DVIEWPORTPARAM Params;
	if (0 != pVp)
	{
		Params.X	  = m_vpX;
		Params.Y	  = m_vpY;
		Params.Width  = m_vpWidth;
		Params.Height = m_vpHeight;
		Params.MinZ	  = m_vpMinZ;
		Params.MaxZ	  = m_vpMaxZ;
	}
	if (0 == pDevice)
	{
		if (0 != pCam)
			pCam->SetProjectionParam(m_camFov, m_camFront, m_camBack, m_camRatio);
		if (0 != pVp)
		{
			pVp->SetParam(&Params);
			pVp->SetClearColor(m_vpColorClear);
		}
	}
	else
	{
		if (0 != pCam)
			pCam->Init(pDevice, m_camFov, m_camFront, m_camBack, m_camRatio);
		if (0 != pVp)
			pVp->Init(pDevice, &Params, true, true, m_vpColorClear);
	}
	return true;
}

int RecItemData::GetDataSize(RecItemType dType, const APhysXU32 nVersion)
{
	static bool bInited = false;
	static int sizeTable[RIT_SIGN_END];

	assert(RIT_SIGN_UNKNOWN < dType);
	assert(RIT_SIGN_END > dType);
	if (bInited)
	{
		assert(0 < sizeTable[dType]);
		return sizeTable[dType];
	}

	for (int i = 0; i < RIT_SIGN_END; ++i)
		sizeTable[i] = -1;

	sizeTable[RIT_CEC_MODE] = sizeof(int);
	sizeTable[RIT_CEC_RADIUS] = sizeof(float);
	sizeTable[RIT_CEC_FOCUSPOS] = sizeof(RecVector3);
	sizeTable[RIT_CEC_BACKPOS] = sizeof(RecVector3);
	sizeTable[RIT_CEC_BACKDIR] = sizeof(RecVector3);
	sizeTable[RIT_CEC_BACKUP] = sizeof(RecVector3);
	sizeTable[RIT_CEC_BACK_FLAG] = sizeof(int);

	sizeTable[RIT_CVC_FOV] = sizeof(float);
	sizeTable[RIT_CVC_FRONT] = sizeof(float);
	sizeTable[RIT_CVC_BACK] = sizeof(float);

	sizeTable[RIT_CVC_VPPT] = sizeof(RecSizeInt);
	sizeTable[RIT_CVC_VPSIZE] = sizeof(RecSizeInt);
	sizeTable[RIT_CVC_VPMINMAX] = sizeof(RecRangeFloat);
	sizeTable[RIT_CVC_VPCOLOR] = sizeof(int);

	sizeTable[RIT_POS] = sizeof(RecVector3);
	sizeTable[RIT_DIR] = sizeof(RecVector3);
	sizeTable[RIT_UP] = sizeof(RecVector3);

	sizeTable[RIT_CAM_MOVE_CTRL] = sizeof(int);
	sizeTable[RIT_MA_MOVE_CTRL] = sizeof(int);
	sizeTable[RIT_CRNT_MOUSE_PT] = sizeof(RecSizeInt);
	sizeTable[RIT_COUNT] = sizeof(int);
	bInited = true;
	return sizeTable[dType];
}

void RecItemData::ReadData(NxStream& stream, const APhysXU32 nVersion)
{
	dType = RecItemType(stream.readDword());
	stream.readBuffer(&value, GetDataSize(dType, nVersion));
}

void RecItemData::SaveData(NxStream& stream)
{
	stream.storeDword(dType);
	stream.storeBuffer(&value, GetDataSize(dType, GetVersion()));
}

RecFrameInfo::RecFrameInfo()
{
	bIsInEPIS = false;
	dtTickSec = 0.0f;
	m_pActiveEvent = 0;
	m_pLastReproEvent = 0;
	m_idxReproItem = 0;
}

RecFrameInfo::~RecFrameInfo()
{
	ClearAllLog();
}

IOptEvent* RecFrameInfo::CreateEvent(NxStream& stream, bool& outIsLowVersion)
{
	IOptEvent* pEvent = 0;
	int nType = stream.readDword();
	OptType ot = OptType(nType);
	switch (ot)
	{
	case OT_KEYBOARD:
		pEvent = new OptKeyBoard;
		break;
	case OT_MOUSE:
		pEvent = new OptMouse;
		break;
	case OT_SIZE:
		pEvent = new OptSize;
		break;
	default:
		assert(!"Shouldn't be here! Debug it!");
	}

	if (0 != pEvent)
		pEvent->LoadData(stream, outIsLowVersion);
	return pEvent;
}

bool RecFrameInfo::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	IOptEvent* pEvent = 0;
	if(nVersion >= 0x1D000001)
	{
		stream.readBuffer(&bIsInEPIS, sizeof(bool));
		dtTickSec = stream.readFloat();

		assert(0 == arrEvents.size());
		bool bIsLowVer = false;
		size_t nEvents = stream.readDword();
		for (size_t i = 0; i < nEvents; ++i)
		{
			bIsLowVer = false;
			pEvent = CreateEvent(stream, bIsLowVer);
			if (bIsLowVer)
				outIsLowVersion = true;
			if (0 != pEvent)
				arrEvents.push_back(pEvent);
		}

		int flags = stream.readDword();
		m_Flags.SetValue(flags);
		if (FI_HAS_ITEM_DATA & flags)
		{
			unsigned int nRIDVer = stream.readDword();
			APhysXU32 nRIDCurrentVer = RecItemData::GetVersion();
			if(!GetVersionPrefixNumber(nRIDVer) == GetVersionPrefixNumber(nRIDCurrentVer))
				return false;
			if(nRIDVer < nRIDCurrentVer)
				outIsLowVersion = true;

			RecItemData rid;
			m_idxReproItem = 0;
			size_t nItems = stream.readDword();
			for (size_t i = 0; i < nItems; ++i)
			{
				rid.ReadData(stream, nRIDVer);
				arrItems.push_back(rid);
			}
		}
		if (FI_HAS_ASYNC_KEYBRD & flags)
			m_asyncKeyBoard.LoadData(stream, outIsLowVersion);
	}
	return true;
}

void RecFrameInfo::SaveData(NxStream& stream)
{
	stream.storeDword(GetVersion());
	stream.storeBuffer(&bIsInEPIS, sizeof(bool));
	stream.storeFloat(dtTickSec);

	size_t nEvents = arrEvents.size();
	stream.storeDword(nEvents);
	for (size_t i = 0; i < nEvents; ++i)
	{
		stream.storeDword(arrEvents[i]->GetOptType());
		arrEvents[i]->SaveData(stream);
	}

	int flags = m_Flags.GetValue();
	stream.storeDword(flags);
	if (FI_HAS_ITEM_DATA & flags)
	{
		stream.storeDword(RecItemData::GetVersion());
		size_t nItems = arrItems.size();
		stream.storeDword(nItems);
		for (size_t i = 0; i < nItems; ++i)
			arrItems[i].SaveData(stream);
	}
	if (FI_HAS_ASYNC_KEYBRD & flags)
		m_asyncKeyBoard.SaveData(stream);
}

void RecFrameInfo::SaveDebugData(ADiskFile& adfDebug, bool bIsReplay)
{
	size_t nEvents = arrEvents.size();
	AString strLine;
	if (0 < nEvents)
	{
		if (bIsReplay)
		{
			size_t nItems = arrItems.size();
			if (m_idxReproItem != nItems)
			{
				strLine.Format("	Warning: reproItems = %d, total items = %d", m_idxReproItem, nItems);
				adfDebug.WriteLine(strLine);
			}
		}
	}
	for (size_t i = 0; i < nEvents; ++i)
	{
		arrEvents[i]->SaveDebugString(adfDebug);
		arrEvents[i]->SaveUserDebugData(adfDebug);
	}
}

void RecFrameInfo::ClearAllLog()
{
	bIsInEPIS = false;
	dtTickSec = -1;
	m_pActiveEvent = 0;
	m_pLastReproEvent = 0;
	size_t nEvents = arrEvents.size();
	for (size_t i = 0; i < nEvents; ++i)
		delete arrEvents[i];
	arrEvents.clear();

	m_Flags.ClearAllFlags();
	m_asyncKeyBoard.Clear();

	m_idxReproItem = 0;
	arrItems.clear();
}

void RecFrameInfo::AddEvent(IOptEvent& optEvent)
{
	arrEvents.push_back(&optEvent);
	m_pActiveEvent = &optEvent;
}

bool RecFrameInfo::AddUserDbgInfo(const AString& str)
{
	if (0 != m_pActiveEvent)
	{
		m_pActiveEvent->AddUserDebugString(str);
		return true;
	}

	if (0 != m_pLastReproEvent)
	{
		m_pLastReproEvent->AddUserDebugString(str);
		return true;
	}
	return false;
}

RecItemData* RecFrameInfo::ReproItemData()
{
	if (m_idxReproItem >= arrItems.size())
	{
		assert(!"Shouldn't be here! Debug it!");
		return 0;
	}

	size_t idx = m_idxReproItem;
	++m_idxReproItem;
	return &(arrItems[idx]);
}

void RecFrameInfo::ReproduceAllEvents(IUserInputEvent& uie, CRecording& rec)
{
	size_t nEvents = arrEvents.size();
	for (size_t i = 0; i < nEvents; ++i)
	{
		m_pActiveEvent = arrEvents[i];
		m_pActiveEvent->ExecuteOptEvent(uie, rec);
		m_pLastReproEvent = m_pActiveEvent;
	}
	m_pActiveEvent = 0;
}

IOptEvent* RecFrameInfo::GetRecentEvent() const
{
	if (0 != m_pActiveEvent)
		return m_pActiveEvent;
	if (0 != m_pLastReproEvent)
		return m_pLastReproEvent;
	return 0;
}

int GetCountOfBitValueIsOne(int n)
{
	int nCount = 0;
	while (0 != n)
	{
		n = n & (n - 1);
		++nCount;
	}
	return nCount;
}

A3DVECTOR3 CRecording::g_vUp(0, 1, 0);

CRecording::CRecording()
{
	m_Status.ClearAllFlags();
	m_Status.RaiseFlag(SF_ENABLE_RECORD);
	m_pScene = 0;
	m_pReplayUIE = 0;
	m_randSeed = 0;

	m_nCountBeforFirstEvent = 0;
	m_pActiveCam = 0;
	m_pActiveVP = 0;

	m_RecFileType = RT_REC_WHOLE;
	m_nNextEpiRecIndex = -1;
	m_nEpiStartFrame = -1;
	m_nEpiEndFrame = -1;
	m_nCurrentFrameNum = 0;
	m_nTotalFrameCount = 0;

	m_maxReplayDelayTime = 2.0f;
	m_curReplayDelayTime = 0.0f;

	m_pRecFile = 0;
	m_pDebugInfo = 0;
	m_pEpiDebugInfo = 0;
}

CRecording::~CRecording()
{
	End();
}

void CRecording::InitScene(Scene& scene)
{
	if (0 != m_pRecFile)
		return;
	m_pScene = &scene;

	CRender* pRender = m_pScene->GetRender();
	A3DDevice* pDevice = pRender->GetA3DDevice();
	m_pActiveCam = pRender->GetCamera();
	m_pActiveVP  = pRender->GetViewport();
	SyncCameraViewToOriginal(pDevice);
	m_origVP.SetCamera(&m_origCam);
}

void CRecording::EnableDebugFile(bool bEnable)
{
	if (bEnable)
		m_Status.RaiseFlag(SF_ENABLE_DEBUG);
	else
		m_Status.ClearFlag(SF_ENABLE_DEBUG);
}

void CRecording::AddUserDebugString(const AString& str)
{
	if (0 == m_pDebugInfo)
		return;

	if (m_Status.ReadFlag(SF_REPLAY_DELAY))
		return;

	if (m_frmCurrent.AddUserDbgInfo(str))
		return;

	m_strDebugBuf.push_back(str);
}

bool CRecording::RecordEnable(bool bEnable)
{
	if (IsRecording())
		return false;

	if (bEnable)
		m_Status.RaiseFlag(SF_ENABLE_RECORD);
	else
		m_Status.ClearFlag(SF_ENABLE_RECORD);
	return true;
}

bool CRecording::GetRecordEnableState() const
{
	return m_Status.ReadFlag(SF_ENABLE_RECORD);
}

bool CRecording::CreateRecEpiFile()
{
	assert(0 < m_nEpiStartFrame);
	if (0 >= m_nEpiStartFrame)
		return false;

	assert(m_nEpiStartFrame < m_nCurrentFrameNum);
	if (m_nEpiStartFrame >= m_nCurrentFrameNum)
		return false;

	APhysXUserStream* pEpiFile = new APhysXUserStream(m_EpiFileName, false);
	if (0 == pEpiFile)
		return false;

	int nTotalFrm = m_nCurrentFrameNum - m_nEpiStartFrame;
	pEpiFile->storeDword(GetVersion());
	pEpiFile->storeDword(nTotalFrm);
	pEpiFile->storeDword(RT_REC_EPISODE);
	pEpiFile->storeDword(m_nEpiStartFrame);

	static const char* pTitle = "MainRecFile:";
	APhysXSerializerBase::StoreString(pEpiFile, pTitle);

	AString strRelPath;
	af_GetRelativePath(m_RecFileName, strRelPath);
	APhysXSerializerBase::StoreString(pEpiFile, strRelPath);
	delete pEpiFile;
	return true;
}

bool CRecording::LoadRecEpiFile()
{
	assert(0 != m_pRecFile);
	assert(0 < m_nEpiStartFrame);
	m_nEpiEndFrame = m_nEpiStartFrame + m_nTotalFrameCount;

	const char* pTitle = APhysXSerializerBase::ReadString(m_pRecFile);
	AString strRelPath = APhysXSerializerBase::ReadString(m_pRecFile);
	delete m_pRecFile;
	m_pRecFile = 0;

	m_pRecFile= new APhysXUserStream(strRelPath, true);
	if (0 == m_pRecFile)
		return false;

	unsigned int nVersion = m_pRecFile->readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;

	bool bIsLowVersion = false;
	if(nVersion < GetVersion())
		bIsLowVersion = true;

	if(nVersion >= 0x1C000001)
	{
		int tfc = m_pRecFile->readDword();
		RecType rt = RecType(m_pRecFile->readDword());
		if (RT_REC_WHOLE != rt)
			return false;
		int esf = m_pRecFile->readDword();
		assert(-1 == esf);
	}
	return true;
}

bool CRecording::ModifyRecFile()
{
	ADiskFile adfFile;
	if (!adfFile.Open(m_RecFileName, AFILE_OPENFOREDIT | AFILE_BINARY))
		return false;

	int size = sizeof(int);
	assert(0 < m_nTotalFrameCount);
	adfFile.Seek(sizeof(GetVersion()), AFILE_SEEK_SET);
	adfFile.Write(&m_nTotalFrameCount, size, (DWORD*)&size);
	adfFile.Close();
	return true;
}

bool CRecording::InitDebugFile(const char* szRecFile, bool bIsEpiType)
{
	if (bIsEpiType)
	{
		if (!m_Status.ReadFlag(SF_ENABLE_DEBUG))
			return true;

		AString strFile(szRecFile);
		int nPos = strFile.ReverseFind('.');
		if (0 >= nPos)
			return false;

		assert(0 == m_pEpiDebugInfo);
		AString strDF = strFile.Left(nPos);
		if (IsRecording())
		{
			strDF += "_RecordingOriginal_Episode.txt";
			DWORD nGetPos = m_pDebugInfo->GetPos();
			char* pRDName = m_pDebugInfo->GetRelativeName();
			m_adfDebug.Close();
			m_pDebugInfo = 0;

			CopyFileExA(pRDName, strDF, 0, 0, false, 0);
			if (m_adfDebug.Open(pRDName, AFILE_OPENFOREDIT | AFILE_TEXT))
			{
				m_pDebugInfo = &m_adfDebug;
				m_pDebugInfo->Seek(nGetPos, AFILE_SEEK_SET);
			}
			if (m_adfEpiDebug.Open(strDF, AFILE_OPENFOREDIT | AFILE_TEXT))
			{
				m_pEpiDebugInfo = &m_adfEpiDebug;
				m_pEpiDebugInfo->Seek(nGetPos, AFILE_SEEK_SET);
			}
			if ((0 == m_pDebugInfo) || (0 == m_pEpiDebugInfo))
				return false;
		}
		else
		{
			strDF += "_ReplayLog_Episode.txt";
			if (m_adfEpiDebug.Open(strDF, AFILE_CREATENEW | AFILE_TEXT))
				m_pEpiDebugInfo = &m_adfEpiDebug;
		}
	}
	else
	{
		if (!m_Status.ReadFlag(SF_ENABLE_DEBUG))
			return true;

		AString strFile(szRecFile);
		int nPos = strFile.ReverseFind('.');
		if (0 >= nPos)
			return false;

		AString strDF = strFile.Left(nPos);
		if (IsRecording())
			strDF += "_RecordingOriginal.txt";
		else
			strDF += "_ReplayLog.txt";

		if (m_adfDebug.Open(strDF, AFILE_CREATENEW | AFILE_TEXT))
			m_pDebugInfo = &m_adfDebug;
	}
	return true;
}

void CRecording::SyncCameraViewToOriginal(A3DDevice* pDevice)
{
	assert(0 != m_pActiveCam);
	assert(0 != m_pActiveVP);

	m_origCVC.InitCVC(*m_pActiveCam, *m_pActiveVP);
	m_origCVC.UpdateCamAndVP(&m_origCam, &m_origVP, pDevice);
	
	m_origCam.SetPos(m_pActiveCam->GetPos());
	m_origCam.SetDirAndUp(m_pActiveCam->GetDir(), g_vUp);
}

bool CRecording::Start(const char* szFileName)
{
	if (!GetRecordEnableState())
		return false;

	if (0 != m_pRecFile)
		return false;

	assert(0 != m_pScene);
	m_Status.ClearFlag(SF_EPISODE_BEGIN);
	m_Status.ClearFlag(SF_EPISODE);
	m_Status.ClearFlag(SF_EPISODE_END);
	m_pRecFile = new APhysXUserStream(szFileName, false);
	if (0 == m_pRecFile)
		return false;

	m_RecFileName = szFileName;
	m_nNextEpiRecIndex  = 1;
	m_nEpiStartFrame = -1;
	m_nEpiEndFrame	 = -1;
	m_nTotalFrameCount = 0;
	m_pRecFile->storeDword(GetVersion());
	m_pRecFile->storeDword(m_nTotalFrameCount);
	m_pRecFile->storeDword(RT_REC_WHOLE);
	m_pRecFile->storeDword(m_nEpiStartFrame);
	m_pScene->SavePhysXDemoScene(*m_pRecFile);
	m_pRecFile->storeDword(m_randSeed);
	m_origCVC.InitCVC(*m_pActiveCam, *m_pActiveVP);
	m_origCVC.SaveData(*m_pRecFile);

	m_nCurrentFrameNum = 1;
	InitDebugFile(szFileName, false);
	return true;
}

void CRecording::End()
{
	EpisodeEnd();
	m_Status.ClearFlag(SF_EPISODE_END);

	bool bIsReplayLoading = m_Status.ReadFlag(SF_REPLAY_LOADING);
	if (0 != m_pRecFile)
	{
		if (!bIsReplayLoading)
		{
			delete m_pRecFile;
			m_pRecFile = 0;
		}
	}
	if (IsRecording())
		ModifyRecFile();
	if (0 != m_pDebugInfo)
	{
		m_pDebugInfo = 0;
		m_adfDebug.Close();
	}

	if (!bIsReplayLoading)
	{
		CCamCtrlAgent* pAgent = m_pScene->GetCamCtrlAgent();
		if (0 != pAgent)
			pAgent->NotifyReplayEnd();
		m_Status.ClearFlag(SF_REPLAY);
		m_Status.ClearFlag(SF_REPLAY_DELAY);
	}
	m_frmCurrent.ClearAllLog();
}

bool CRecording::EpisodeBegin(const char* szFileName)
{
	if (!IsRecording())
		return false;
	if (m_Status.ReadFlag(SF_EPISODE))
		return false;

	if (m_Status.ReadFlag(SF_EPISODE_END))
		m_Status.ClearFlag(SF_EPISODE_END);
	m_Status.RaiseFlag(SF_EPISODE_BEGIN);

	m_EpiFileName = szFileName;
	++m_nNextEpiRecIndex; 
	m_nEpiStartFrame = -1;
	return true;
}

void CRecording::EpisodeEnd()
{
	if (IsRecordingEpisode())
		CreateRecEpiFile();

	if (0 != m_pEpiDebugInfo)
	{
		m_pEpiDebugInfo = 0;
		m_adfEpiDebug.Close();
	}

	if (m_Status.ReadFlag(SF_EPISODE_BEGIN))
		m_Status.ClearFlag(SF_EPISODE_BEGIN);

	if (m_Status.ReadFlag(SF_EPISODE))
		m_Status.ClearFlag(SF_EPISODE);
}

bool CRecording::Replay(const char* szFileName)
{
	if (0 != m_pRecFile)
		return false;

	assert(0 != m_pScene);
	m_pRecFile = new APhysXUserStream(szFileName, true);
	if (0 == m_pRecFile)
		return false;

	m_Status.RaiseFlag(SF_REPLAY);
	m_Status.RaiseFlag(SF_REPLAY_LOADING);

	unsigned int nVersion = m_pRecFile->readDword();
	if(!CheckVersionPrefix(nVersion))
	{
		m_Status.ClearFlag(SF_REPLAY);
		m_Status.ClearFlag(SF_REPLAY_LOADING);
		End();
		return false;
	}
	bool bIsLowVersion = false;
	if(nVersion < GetVersion())
		bIsLowVersion = true;

	m_nEpiEndFrame = -1;
	m_curReplayDelayTime = 0.0f;
	if(nVersion >= 0x1C000001)
	{
		m_nTotalFrameCount = m_pRecFile->readDword();
		m_RecFileType = RecType(m_pRecFile->readDword());
		m_nEpiStartFrame = m_pRecFile->readDword();
		if (RT_REC_WHOLE == m_RecFileType)
		{
			assert(-1 == m_nEpiStartFrame);
		}
		else
		{
			if (!LoadRecEpiFile())
			{
				m_Status.ClearFlag(SF_REPLAY);
				m_Status.ClearFlag(SF_REPLAY_LOADING);
				End();
				return false;
			}
		}

		if (!m_pScene->LoadPhysXDemoScene(*m_pRecFile, bIsLowVersion))
		{
			m_Status.ClearFlag(SF_REPLAY);
			m_Status.ClearFlag(SF_REPLAY_LOADING);
			End();
			return false;
		}
		SyncCameraViewToOriginal(0);

		m_randSeed = m_pRecFile->readDword();
		bool bIsLowVer = false;
		m_origCVC.LoadData(*m_pRecFile, bIsLowVer);
		m_origCVC.UpdateCamAndVP(&m_origCam, &m_origVP);
	}

	m_nCurrentFrameNum = 1;
	if (!ReadNextFrameInfo())
	{
		m_Status.ClearFlag(SF_REPLAY);
		m_Status.ClearFlag(SF_REPLAY_LOADING);
		End();
		return false;
	}
	CCamCtrlAgent* pAgent = m_pScene->GetCamCtrlAgent();
	if (0 != pAgent)
		pAgent->NotifyReplayBegin(m_origCam);

	assert(true == IsEqualCameraPose());
	m_Status.ClearFlag(SF_REPLAY_LOADING);
	m_Status.RaiseFlag(SF_REPLAY_DELAY);
	InitDebugFile(szFileName, false);
	return true;
}

void CRecording::ReplayExecute()
{
	m_Status.RaiseFlag(SF_REPLAY_LOADING);
	assert(0 != m_pReplayUIE);
	m_pReplayUIE->EntryPlayMode();
	m_Status.ClearFlag(SF_REPLAY_LOADING);
}

bool CRecording::ReadNextFrameInfo()
{
	assert(true == IsReplaying());
	assert(0 != m_pReplayUIE);

	assert(m_frmCurrent.m_idxReproItem == m_frmCurrent.arrItems.size());
	m_frmCurrent.ClearAllLog();
	m_Status.ClearFlag(SF_REPLAY_END);
	m_Status.ClearFlag(SF_EPISODE_END);

	if (0 < m_nEpiEndFrame)
	{
		if (m_nEpiEndFrame == m_nCurrentFrameNum)
		{
			End();
			m_Status.RaiseFlag(SF_REPLAY_END);
			return false;
		}
	}

	bool bIsLowVersion = false;
	if (m_frmCurrent.LoadData(*m_pRecFile, bIsLowVersion))
	{
		if (m_frmCurrent.bIsInEPIS)
		{
			if (RT_REC_EPISODE == m_RecFileType)
			{
				if (m_nEpiStartFrame <= m_nCurrentFrameNum)
				{
					assert(0 < m_nEpiStartFrame);
					m_Status.RaiseFlag(SF_EPISODE);
				}
			}
			return true;
		}
		if (!m_Status.ReadFlag(SF_EPISODE))
			return true;

		m_Status.ClearFlag(SF_EPISODE);
	}

	End();
	m_Status.RaiseFlag(SF_REPLAY_END);
	return false;
}

bool CRecording::IsEqualCameraPose() const
{
	A3DVECTOR3 opos = m_origCam.GetPos();
	A3DVECTOR3 apos = m_pActiveCam->GetPos();
	if (!NxMath::equals(opos.x, apos.x, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(opos.y, apos.y, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(opos.z, apos.z, APHYSX_FLOAT_EPSILON))
		return false;

	A3DVECTOR3 odir = m_origCam.GetDir();
	A3DVECTOR3 adir = m_pActiveCam->GetDir();
	if (!NxMath::equals(odir.x, adir.x, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(odir.y, adir.y, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(odir.z, adir.z, APHYSX_FLOAT_EPSILON))
		return false;

	A3DVECTOR3 oup = m_origCam.GetUp();
	A3DVECTOR3 aup = m_pActiveCam->GetUp();
	if (!NxMath::equals(oup.x, aup.x, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(oup.y, aup.y, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(oup.z, aup.z, APHYSX_FLOAT_EPSILON))
		return false;

	return true;
}

bool CRecording::IsEqualCameraAllData() const
{
	// todo: temp code
	A3DViewport& ncvp = const_cast<A3DViewport&>(m_origVP);
	A3DVIEWPORTPARAM* pori = ncvp.GetParam();
	A3DVIEWPORTPARAM* pact = m_pActiveVP->GetParam();
	if (pori->X != pact->X)
		return false;
	if (pori->Y != pact->Y)
		return false;
	if (pori->Width != pact->Width)
		return false;
	if (pori->Height != pact->Height)
		return false;
	if (!NxMath::equals(pori->MinZ, pact->MinZ, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(pori->MaxZ, pact->MaxZ, APHYSX_FLOAT_EPSILON))
		return false;

	// todo: temp code
	A3DCamera& ncCam = const_cast<A3DCamera&>(m_origCam);
	float ofov, ofront, oback, oratio;
	ncCam.GetProjectionParam(&ofov, &ofront, &oback, &oratio);
	float afov, afront, aback, aratio;
	m_pActiveCam->GetProjectionParam(&afov, &afront, &aback, &aratio);
	if (!NxMath::equals(ofov, afov, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(ofront, afront, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(oback, aback, APHYSX_FLOAT_EPSILON))
		return false;
	if (!NxMath::equals(oratio, aratio, APHYSX_FLOAT_EPSILON))
		return false;

	return IsEqualCameraPose();
}

void CRecording::UpdateTickForRec(float dtSec)
{
	if (IsReplaying())
	{
		if (m_Status.ReadFlag(SF_REPLAY_DELAY))
		{
			m_curReplayDelayTime += dtSec;
		}
		else
		{
			SaveDebugData();
			++m_nCurrentFrameNum;
			ReadNextFrameInfo();
		}
		return;
	}

	if (IsReplayEnd())
		m_Status.ClearFlag(SF_REPLAY_END);

	if (IsRecording())
	{
		m_frmCurrent.bIsInEPIS = m_Status.ReadFlag(SF_EPISODE);
		m_frmCurrent.dtTickSec = dtSec;
		m_frmCurrent.SaveData(*m_pRecFile);
		SaveDebugData();
		if (m_Status.ReadFlag(SF_CREATE_EPIDBFILE))
		{
			m_Status.ClearFlag(SF_CREATE_EPIDBFILE);
			InitDebugFile(m_EpiFileName, true);
		}
		++m_nTotalFrameCount;
		++m_nCurrentFrameNum;
		m_frmCurrent.ClearAllLog();
		if (m_Status.ReadFlag(SF_EPISODE_BEGIN))
		{
			m_Status.ClearFlag(SF_EPISODE_BEGIN);
			m_Status.RaiseFlag(SF_EPISODE);
			m_Status.RaiseFlag(SF_CREATE_EPIDBFILE);
			m_nEpiStartFrame = m_nCurrentFrameNum;
		}
	}
}

int CRecording::GetRandSeed() const
{
	return m_randSeed;
}

void CRecording::SetRandSeed(int rs)
{
	if (m_randSeed != rs)
		m_randSeed = rs;
}

bool CRecording::IsRecording() const
{
	if (0 == m_pRecFile)
		return false;
	return !m_Status.ReadFlag(SF_REPLAY);
}

bool CRecording::IsReplaying() const
{
	if (0 == m_pRecFile)
		return false;
	return m_Status.ReadFlag(SF_REPLAY);
}

bool CRecording::IsRecordingEpisode() const
{
	if (!IsRecording())
		return false;
	return m_Status.ReadFlag(SF_EPISODE);
}

bool CRecording::IsReplayLoadingEpisode(float* pLoadingPercent) const
{
	if (!IsReplaying())
		return false;
	if (RT_REC_WHOLE == m_RecFileType)
		return false;
	if (m_Status.ReadFlag(SF_EPISODE))
	{
		assert(m_nEpiStartFrame <= m_nCurrentFrameNum);
		return false;
	}

	if (0 != pLoadingPercent)
		*pLoadingPercent = m_nCurrentFrameNum * 1.0f / m_nEpiStartFrame;
	return true;
}

bool CRecording::IsReplayEnd() const
{
	return m_Status.ReadFlag(SF_REPLAY_END);
}

bool CRecording::IsEventReproducing() const
{
	if (IsReplaying())
	{
		if (0 != m_frmCurrent.GetActiveEvent())
			return true;
		if (m_Status.ReadFlag(SF_REPLAY_LOADING))
			return true;
	}
	return false;
}

int CRecording::GetNextRecordingEpisodeIndex() const
{
	if (IsRecording())
	{
		assert(0 < m_nNextEpiRecIndex);
		return m_nNextEpiRecIndex;
	}
	return -1;
}

float CRecording::GetReproTickSec() const
{
	if (IsReplaying())
		return m_frmCurrent.dtTickSec;
	return -1;
}

bool CRecording::GetInstantReproEventScreenRay(const int x, const int y, PhysRay& outRay) const
{
	if (!IsEventReproducing())
		return false;

	IOptEvent* pAE = m_frmCurrent.GetActiveEvent();
	if (OT_MOUSE != pAE->GetOptType())
		return false;

	int origX = 0; 
	int origY = 0; 
	OptMouse* pMouseEvent = dynamic_cast<OptMouse*>(pAE);
	pMouseEvent->GetMousePoint(origX, origY);
	if (x == origX)
	{
		if (y == origY)
		{
			A3DVECTOR3 avNearPos(float(x), float(y), 0.0f);
			A3DVECTOR3 avFarPos(float(x), float(y), 1.0f);
			// todo: temp code
			A3DViewport& ncvp = const_cast<A3DViewport&>(m_origVP);
			ncvp.InvTransform(avNearPos, avNearPos);
			ncvp.InvTransform(avFarPos, avFarPos);
			outRay.vStart = avNearPos;
			outRay.vDelta = avFarPos - avNearPos;
			return true;
		}
	}
	return false;
}

const A3DCamera* CRecording::GetOriginalCamera() const
{
	if (IsReplaying())
		return &m_origCam;
	return 0;
}

void CRecording::LogEvent(const IOptEvent& optE)
{
	if (!IsRecording())
		return;

	IOptEvent* pEvent = optE.Clone();
	if (0 != pEvent)
	{
		if (0 == m_nCountBeforFirstEvent)
			m_nCountBeforFirstEvent = m_strDebugBuf.size();
		m_frmCurrent.AddEvent(*pEvent);
	}
}

void CRecording::ReproAllEvents()
{
	if (!IsReplaying())
		return;

	if (m_Status.ReadFlag(SF_REPLAY_DELAY))
	{
		if (m_curReplayDelayTime > m_maxReplayDelayTime)
		{
			m_Status.ClearFlag(SF_REPLAY_DELAY);
			ReplayExecute();
			m_curReplayDelayTime = 0.0f;
			return;
		}
	}

	if (0 == m_nCountBeforFirstEvent)
		m_nCountBeforFirstEvent = m_strDebugBuf.size();

	assert(0 != m_pReplayUIE);
	m_frmCurrent.ReproduceAllEvents(*m_pReplayUIE, *this);
}

void CRecording::LogItemData(const RecItemType& rit, const AutoData& value)
{
	if (!IsRecording())
		return;

	m_frmCurrent.m_Flags.RaiseFlag(FI_HAS_ITEM_DATA);
	RecItemData rid;
	rid.dType = rit;
	rid.value = value;
	m_frmCurrent.arrItems.push_back(rid);
}

bool CRecording::ReproItemData(const RecItemType& rit, AutoData* poutValue)
{
	RecItemType outRit;
	AutoData outVal(0);
	if (!ReproItemData(outRit, outVal))
		return false;

	assert(rit == outRit);
	if (0 != poutValue)
		*poutValue = outVal;
	return true;
}

bool CRecording::ReproItemData(RecItemType& outRit, AutoData& outValue)
{
	if (m_Status.ReadFlag(SF_REPLAY_DELAY))
		return false;
	if (!m_frmCurrent.m_Flags.ReadFlag(FI_HAS_ITEM_DATA))
		return false;

	const RecItemData* pRID = m_frmCurrent.ReproItemData();
	if (0 == pRID)
		return false;

	outRit = pRID->dType;
	outValue = pRID->value;
	return true;
}

bool CRecording::QueryAsyncKeyDown(int vKey, bool bQueryOnly)
{
	int nRtn = 0;
	if (IsRecording())
	{
		if (bQueryOnly)
			nRtn = m_frmCurrent.m_asyncKeyBoard.IsKeyDown(vKey, SimpleAsyncRec::SAR_QUERY_ONLY);
		else
			nRtn = m_frmCurrent.m_asyncKeyBoard.IsKeyDown(vKey, SimpleAsyncRec::SAR_QUERY_SAVE);
		if (-1 == nRtn)
		{
			AString str;
			str.Format("call GetAsyncKeyState() with key = %d more than once! Probably losing some info.", vKey);
			a_LogOutput(1, "Warning in %s, %s", __FUNCTION__, str);
			AString str2 = "Warning: " + str;
			AddUserDebugString(str2);
		}
		if ((0 < nRtn) && !bQueryOnly)
			m_frmCurrent.m_Flags.RaiseFlag(FI_HAS_ASYNC_KEYBRD);
	}
	else if (IsReplaying())
	{
		if (m_Status.ReadFlag(SF_REPLAY_DELAY))
			bQueryOnly = true;
		if (bQueryOnly)
			nRtn = m_frmCurrent.m_asyncKeyBoard.IsKeyDown(vKey, SimpleAsyncRec::SAR_QUERY_ONLY);
		else
			nRtn = m_frmCurrent.m_asyncKeyBoard.IsKeyDown(vKey, SimpleAsyncRec::SAR_CACHE_REPLAY);
	}
	else
		nRtn = m_frmCurrent.m_asyncKeyBoard.IsKeyDown(vKey, SimpleAsyncRec::SAR_QUERY_ONLY);
	return (0 < nRtn)? true : false;
}

bool CRecording::OnReproCamAndVP(const RecItemType& rit, const AutoData& value)
{
	assert(true == IsEventReproducing());
	if (RIT_CVC_FOV == rit)
	{
		m_origCVC.m_camFov = value.fVal;
		m_origCVC.UpdateCamAndVP(&m_origCam, 0);
		return true;
	}
	if (RIT_CVC_FRONT == rit)
	{
		m_origCVC.m_camFront = value.fVal;
		m_origCVC.UpdateCamAndVP(&m_origCam, 0);
		return true;
	}
	if (RIT_CVC_BACK == rit)
	{
		m_origCVC.m_camBack = value.fVal;
		m_origCVC.UpdateCamAndVP(&m_origCam, 0);
		return true;
	}
	if (RIT_CVC_VPPT == rit)
	{
		m_origCVC.m_vpX = value.szeVal.x;
		m_origCVC.m_vpY = value.szeVal.y;
		m_origCVC.UpdateCamAndVP(0, &m_origVP);
		return true;
	}
	if (RIT_CVC_VPSIZE == rit)
	{
		m_origCVC.m_vpWidth = value.szeVal.x;
		m_origCVC.m_vpHeight = value.szeVal.y;
		m_origCVC.UpdateCamAndVP(0, &m_origVP);
		m_origCVC.m_camRatio = float(m_origCVC.m_vpWidth) / float(m_origCVC.m_vpHeight);
		m_origCVC.UpdateCamAndVP(&m_origCam, 0);
		return true;
	}
	if (RIT_CVC_VPMINMAX == rit)
	{
		m_origCVC.m_vpMinZ = value.rgnVal.a;
		m_origCVC.m_vpMaxZ = value.rgnVal.b;
		m_origCVC.UpdateCamAndVP(0, &m_origVP);
		return true;
	}
	if (RIT_CVC_VPCOLOR == rit)
	{
		m_origCVC.m_vpColorClear = value.nVal;
		m_origCVC.UpdateCamAndVP(0, &m_origVP);
		return true;
	}
	return false;
}

void CRecording::SaveDebugData()
{
	if (0 == m_pDebugInfo)
		return;

	int filter = ObjManager::OBJ_TYPEID_ALL;
	filter &= ~ObjManager::OBJ_TYPEID_STATIC;
	APtrArray<IObjBase*> Objs;
	int nCount = ObjManager::GetInstance()->GetRootLiveObjects(Objs, filter);

	bool bOutputPose = true;
	bool bIsInEpisode = m_frmCurrent.bIsInEPIS;
	size_t nEvents = m_frmCurrent.GetEventCount();
	size_t nTotalUserDbgInfo = m_strDebugBuf.size();
	if (!bIsInEpisode && (0 == nEvents) && (0 == nTotalUserDbgInfo))
	{
		if (!bOutputPose)
			return;
		if (0 == nCount)
			return;
	}

	AString strFrmNum, strTitle, strTemp;
	strFrmNum.Format("#%08d", m_nCurrentFrameNum);
	m_pDebugInfo->WriteLine(strFrmNum);
	if (0 != m_pEpiDebugInfo)
		m_pEpiDebugInfo->WriteLine(strFrmNum);
	if (bIsInEpisode)
		strTitle.Format("	Tick time = %f, Total %d events, Episode", m_frmCurrent.dtTickSec, nEvents);
	else
		strTitle.Format("	Tick time = %f, Total %d events", m_frmCurrent.dtTickSec, nEvents);
	m_pDebugInfo->WriteLine(strTitle);
	if (0 != m_pEpiDebugInfo)
		m_pEpiDebugInfo->WriteLine(strTitle);

	if (0 < nTotalUserDbgInfo)
	{
		assert(m_nCountBeforFirstEvent <= nTotalUserDbgInfo);
		if (0 == m_nCountBeforFirstEvent)
			m_nCountBeforFirstEvent = nTotalUserDbgInfo;
		for (size_t i = 0; i < m_nCountBeforFirstEvent; ++i)
		{
			strTemp = "	" + m_strDebugBuf[i];
			m_pDebugInfo->WriteLine(strTemp);
			if (0 != m_pEpiDebugInfo)
				m_pEpiDebugInfo->WriteLine(strTemp);
		}
		if (m_nCountBeforFirstEvent < nTotalUserDbgInfo)
		{
			IOptEvent* pEvent = m_frmCurrent.GetRecentEvent();
			if (0 == pEvent)
			{
				for (size_t i = m_nCountBeforFirstEvent; i < nTotalUserDbgInfo; ++i)
				{
					strTemp = "	" + m_strDebugBuf[i];
					m_pDebugInfo->WriteLine(strTemp);
					if (0 != m_pEpiDebugInfo)
						m_pEpiDebugInfo->WriteLine(strTemp);
				}
			}
			else
			{
				for (size_t i = m_nCountBeforFirstEvent; i < nTotalUserDbgInfo; ++i)
					pEvent->AddUserDebugString(m_strDebugBuf[i]);
			}
		}
	}
	m_nCountBeforFirstEvent = 0;
	m_strDebugBuf.clear();
	m_frmCurrent.SaveDebugData(*m_pDebugInfo, IsReplaying());
	if (0 != m_pEpiDebugInfo)
		m_frmCurrent.SaveDebugData(*m_pEpiDebugInfo, IsReplaying());

	// output object pose info
	if (bOutputPose)
	{
		AString strLine;
		strLine.Format("	Before Simulation total %d root objects(except static objects)", nCount);
		m_pDebugInfo->WriteLine(strLine);
		if (0 != m_pEpiDebugInfo)
			m_pEpiDebugInfo->WriteLine(strLine);

		AString strPrefix = "	";
		for (int i = 0; i < nCount; ++i)
			SaveObjectPose(*(Objs[i]), strPrefix);
	}
}

void CRecording::SaveObjectPose(IObjBase& obj, const AString& strPrefix)
{
	assert(0 != m_pDebugInfo);

	AString strName;
	strName.Format("%sName: %s", strPrefix, obj.GetProperties()->GetObjName());
	m_pDebugInfo->WriteLine(strName);
	if (0 != m_pEpiDebugInfo)
		m_pEpiDebugInfo->WriteLine(strName);

	NxVec3 pos, dir, up;
	pos = obj.GetGPos();
	obj.GetGDirAndUp(dir, up);

	AString strPose;
//	strPose.Format("%spos = %.3f, %.3f, %.3f   dir = %.3f, %.3f, %.3f   up = %.3f, %.3f, %.3f",
	strPose.Format("%spos = %f, %f, %f   dir = %f, %f, %f   up = %f, %f, %f",
		strPrefix, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y, up.z);
	m_pDebugInfo->WriteLine(strPose);
	if (0 != m_pEpiDebugInfo)
		m_pEpiDebugInfo->WriteLine(strPose);

	IObjBase* pChild = 0;
	AString strNextPF = strPrefix + "	";
	int nChildren = obj.GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = static_cast<IObjBase*>(obj.GetChild(i));
		if (0 != pChild)
			SaveObjectPose(*pChild, strNextPF);
	}
}

