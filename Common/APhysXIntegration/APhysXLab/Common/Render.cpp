/*
 * FILE: Render.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

#include <algorithm>
#include <windows.h>
#include "A3D.h"
#include "ALog.h"

// global render...
CRender g_Render;

const unsigned int CRender::gForceBarLen = 50;

CRender::CRender()
{
	m_bInitedStart = false;
	m_bInitedEndOK = false;
	m_bDrawCameraDir = false;
	m_pA3DEngine = 0;
	m_pA3DDevice = 0;
	m_pCamera	 = 0;
	m_pViewport	 = 0;
	m_pDirLight	 = 0;
	m_pSysFont	 = 0;
	m_ForceSize  = 0;
	m_xCenter = m_yCenter = 0;

	m_pPhysXTrn	= 0;
	m_pA3DTrn	= 0;
	m_trnCenter.Set(0, 0, 0);
}

CRender::~CRender()
{
	std::for_each(m_lstText.begin(), m_lstText.end(), DeleteIt<TextNode>());
	if (m_bInitedStart)
		Release();
}

void CRender::Release()
{
	m_bInitedStart = false;
	m_bInitedEndOK = false;
	m_WCTopZbuf.Release();
	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pDirLight);
	A3DRELEASE(m_pA3DEngine);
	
	m_pSysFont	 = 0;
	m_pA3DDevice = 0;
}

bool CRender::Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen)
{
	if (m_bInitedStart)
		Release();

	m_bInitedStart = true;
	RECT rect;
	if (!InitA3DEngine(hInst, hWnd, bFullScreen, rect))
		return false;
	
	if (!InitLight())
		return false;

	if (!InitCamera())
		return false;

	if (!InitViewport(rect))
		return false;

	m_pSysFont = m_pA3DEngine->GetSystemFont();
	m_WCTopZbuf.Init(m_pA3DEngine);
	m_bInitedEndOK = true;
	return true;
}

bool CRender::InitA3DEngine(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen, RECT& outRect)
{
	assert(0 == m_pA3DEngine);
	m_pA3DEngine = new A3DEngine;  
	if (0 == m_pA3DEngine)
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Not enough memory!");
		return false;
	}
	
	outRect.left = outRect.top = outRect.right = outRect.bottom = 0;
	GetClientRect(hWnd, &outRect);

	A3DDEVFMT devFmt;
	devFmt.bWindowed	= !bFullScreen;
	devFmt.nWidth		= outRect.right;
	devFmt.nHeight		= outRect.bottom;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;
	
	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;	//	Hardware render
	if (!m_pA3DEngine->Init(hInst, hWnd, &devFmt, dwDevFlags))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to initialize A3D engine!");
		delete m_pA3DEngine;
		m_pA3DEngine = 0;
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();
	m_pA3DEngine->GetA3DSkinMan()->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");
	return true;
}

bool CRender::InitLight()
{
	assert(0 != m_pA3DEngine);
	if (0 == m_pA3DEngine)
	{
		a_LogOutput(1, "CRender::InitLightAndFog: 'm_pA3DEngine' is invalid!");
		return false;
	}

	assert(0 == m_pDirLight);
	bool rtn = m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, 
						A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(1.0f, -1.0f, -1.0f),
						A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f), 
						A3DCOLORVALUE(0.6f, 0.6f, 0.6f, 1.0f), 
						A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 0.0f));
	if (!rtn)
	{
		a_LogOutput(1, "CRender::InitLightAndFog: Failed to create direcional light!");
		return false;
	}

	m_pA3DEngine->GetA3DSkinMan()->SetDirLight(m_pDirLight);
	return true;
}

bool CRender::InitCamera()
{
	assert(0 != m_pA3DDevice);
	if (0 == m_pA3DDevice)
	{
		a_LogOutput(1, "CRender::InitCamera: 'm_pA3DDevice' is invalid!");
		return false;
	}

	assert(0 == m_pCamera);
	m_pCamera = new A3DCamera; 
	if (0 == m_pCamera)
	{
		a_LogOutput(1, "CRender::InitCamera: Not enough memory!");
		return false;
	}
	
	if (!m_pCamera->Init(m_pA3DDevice, DEG2RAD(56.0f), 0.1f, 2000.0f))
	{
		a_LogOutput(1, "CRender::InitCamera: Failed to initialize camera!");
		delete m_pCamera;
		m_pCamera = 0;
		return false;
	}

	m_pCamera->SetPos(A3DVECTOR3(0.0F, 0.0f, 0.0f));
	m_pCamera->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	return true; 
}

bool CRender::InitViewport(const RECT& Rect)
{
	assert(0 != m_pA3DEngine);
	if (0 == m_pA3DEngine)
	{
		a_LogOutput(1, "CRender::InitViewport: 'm_pA3DEngine' is invalid!");
		return false;
	}

	assert(0 != m_pCamera);
	if (0 == m_pCamera)
	{
		a_LogOutput(1, "CRender::InitViewport: 'm_pCamera' is invalid!");
		return false;
	}

	m_pA3DDevice->SetAmbient(A3DCOLORRGB(150, 150, 150));
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	assert(0 == m_pViewport);
	if (!m_pA3DDevice->CreateViewport(&m_pViewport, Rect.left, Rect.top, 
					Rect.right - Rect.left, Rect.bottom - Rect.top, 
					0.0f, 1.0f, true, true, A3DCOLORRGB(64, 64, 64)))
	{
		a_LogOutput(1, "CRender::InitViewport: Failed to create viewport!");
		return false;
	}
	m_pViewport->SetCamera(m_pCamera);

	m_xCenter = (Rect.right - Rect.left) / 2;
	m_yCenter = (Rect.bottom - Rect.top) / 2;
	return true;
}

bool CRender::ResizeDevice(const int iWid, const int iHei) const
{
	if (!EngineIsReady() || 0 >= iWid || 0 >= iHei)
		return false;

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, true, SDM_WIDTH | SDM_HEIGHT);

	A3DVIEWPORTPARAM Params;
	Params.X	  = 0;
	Params.Y	  = 0;
	Params.Width  = iWid;
	Params.Height = iHei;
	Params.MinZ	  = 0.0f;
	Params.MaxZ	  = 1.0f;
	m_pViewport->SetParam(&Params);
	m_pViewport->Active();

	float fRatio = float(iWid) / iHei;
	m_pCamera->SetProjectionParam(m_pCamera->GetFOV(), 0.1f, 2000.0f, fRatio);

	m_xCenter = iWid / 2;
	m_yCenter = iHei / 2;
	return true;
}

bool CRender::BeginRender() const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	return m_pA3DEngine->BeginRender();
}

bool CRender::EndRender()
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	DrawTextAnimation();
	if (m_FrontSight.bIsEnable)
		m_WCTopZbuf.AddRect_2D(m_FrontSight.l, m_FrontSight.t, m_FrontSight.r, m_FrontSight.b, m_FrontSight.color);

	if (m_ForceBar.bIsEnable)
	{
		static const int MaxMSec = 3000;
		static const float LenPerMSec = gForceBarLen * 1.0f / MaxMSec;

		int interval = GetTickCount() - m_HoldTime;
		if (0 > interval)
			interval += std::numeric_limits<DWORD>::max();
		m_ForceSize = (interval % MaxMSec) * LenPerMSec;

		m_WCTopZbuf.AddRect_2D(m_ForceBar.l, m_ForceBar.t, m_ForceBar.r, m_ForceBar.b, m_ForceBar.color);
		if (0 < m_ForceSize)
			m_WCTopZbuf.AddRect_2D(m_ForceBar.l, m_ForceBar.b - 2, m_ForceBar.l + m_ForceSize, m_ForceBar.b - 1, m_ForceBar.color);
	}

	if (m_bDrawCameraDir)
	{
		m_WCTopZbuf.AddLine_2D(A3DVECTOR3(m_xCenter - 5, m_yCenter, 0), A3DVECTOR3(m_xCenter + 6, m_yCenter, 0), A3DCOLORRGB(0, 192, 0));
		m_WCTopZbuf.AddLine_2D(A3DVECTOR3(m_xCenter, m_yCenter - 5, 0), A3DVECTOR3(m_xCenter, m_yCenter + 6, 0), A3DCOLORRGB(0, 192, 0));
	}
	
	if ((0 != m_pPhysXTrn) && (0 != m_pA3DTrn))
		DrawPhysXTerrainRange();

	m_pA3DDevice->SetZTestEnable(false);
	m_WCTopZbuf.Flush();
	m_pA3DDevice->SetZTestEnable(true);

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

void CRender::TextOut(int x, int y, const ACHAR* szText, DWORD color) const
{
	if (0 != m_pSysFont)
		m_pSysFont->TextOut(x, y, szText, color);
}

bool CRender::TextNode::IsValid(const int nowCount) 
{
	int delCount = nowCount - nStartCount;
	if (0 > delCount)
		delCount += std::numeric_limits<int>::max();
	
	CompletePercent = delCount * 1.0f / nMaxCount;
	if (delCount < nMaxCount)
		return true;
	
	return false;
}

void CRender::TextOutAnimation(const int x, const int y, const TCHAR* szText, const DWORD color, const unsigned int nMaxDrawMilliSec)
{
	TextNode* pNode = new TextNode;
	if (0 == pNode)
		return;

	pNode->xPos = x;
	pNode->yPos = y;
	pNode->nStartCount = GetTickCount();
	pNode->nMaxCount = nMaxDrawMilliSec;
	pNode->color = color;
	pNode->strText = szText;
	m_lstText.push_back(pNode);
}

void CRender::DrawTextAnimation()
{
	if (0 == m_pSysFont)
		return;

	const int Now = GetTickCount();
#if _MSC_VER >= 1400   //  VC2005 is 1400
	m_lstText.remove_if(IsGoToDie(Now));
#else
	// Just workaround for the stupid VC6.  
	std::list<TextNode*>::iterator it = m_lstText.begin(); 
	std::list<TextNode*>::iterator itEnd = m_lstText.end(); 
	for (; it != itEnd;)
	{
		TextNode* pn = *it;
		if (pn->IsValid(Now))
		{
			++it;
			continue;
		}

		delete pn;
		it = m_lstText.erase(it);
	}
#endif

	std::list<TextNode*>::const_iterator c_it = m_lstText.begin(); 
	std::list<TextNode*>::const_iterator c_itEnd = m_lstText.end(); 
	for (; c_it != c_itEnd; ++c_it)
	{
		TextNode* pn = *c_it;
		int y = pn->yPos - pn->CompletePercent * 10.0f;
		m_pSysFont->TextOut(pn->xPos, y, pn->strText, pn->color);
	}
}

void CRender::ClearViewport() const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return;

	m_pViewport->Active();
	m_pViewport->ClearDevice();
}

bool CRender::OnSetCursor() const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	A3DCursor* pCursor = m_pA3DDevice->GetCursor();
	if (0 == pCursor)
		return false;

	pCursor->OnSysSetCursor();
	return true;
}

bool CRender::ShowCursor(A3DCursor* pA3DCursor) const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	m_pA3DDevice->SetCursor(pA3DCursor);
	m_pA3DDevice->ShowCursor(true);
	return true;
}

bool CRender::SetSky(A3DSky& sky) const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	return m_pA3DEngine->SetSky(&sky);
}

bool CRender::SetLightInfo(A3DSkinModel& skinModel) const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	const A3DLIGHTPARAM& lp = m_pDirLight->GetLightparam();

	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));
	LightInfo.colAmbient = m_pA3DDevice->GetAmbientValue();
	LightInfo.vLightDir	 = lp.Direction;
	LightInfo.colDirDiff = lp.Diffuse;
	LightInfo.colDirSpec = lp.Specular;
	LightInfo.bPtLight	 = false;
	skinModel.SetLightInfo(LightInfo);
	return true;
}

void CRender::SetupCamera(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) const
{
	assert(0 != m_pCamera);
	if (0 == m_pCamera)
		return;

	m_pCamera->SetPos(vPos);
	m_pCamera->SetDirAndUp(vDir, vUp);
}

void CRender::SetFrontSight(const int xCenter, const int yCenter, const int HalfSideLen, const DWORD color)
{
	m_FrontSight.bIsEnable = true;
	m_FrontSight.color	   = color;

	m_FrontSight.l = xCenter - HalfSideLen;
	m_FrontSight.t = yCenter - HalfSideLen;
	m_FrontSight.r = xCenter + HalfSideLen;
	m_FrontSight.b = yCenter + HalfSideLen;
}

bool CRender::GetFrontSight(int& xCenter, int& yCenter, int& HalfSideLen, DWORD* pColor) const
{
	if (!m_FrontSight.bIsEnable)
		return false;

	xCenter = (m_FrontSight.l + m_FrontSight.r) * 0.5;
	yCenter = (m_FrontSight.t + m_FrontSight.b) * 0.5;
	HalfSideLen = xCenter - m_FrontSight.l;
	if (0 != pColor)
		*pColor = m_FrontSight.color;
	return true;
}

void CRender::DrawForceBar(const int xCenter, const int yCenter, const DWORD color, const bool StartHoldingForce)
{
	if (StartHoldingForce)
		m_HoldTime = GetTickCount();

	m_ForceBar.bIsEnable = true;
	m_ForceBar.l = xCenter;
	m_ForceBar.b = yCenter - 4;
	m_ForceBar.r = m_ForceBar.l + gForceBarLen;
	m_ForceBar.t = m_ForceBar.b - 3;
	m_ForceBar.color = color;
}

void CRender::DrawPhysXTerrainRange()
{
	assert(0 != m_pPhysXTrn);
	if (0 == m_pPhysXTrn)
		return;

	assert(0 != m_pA3DTrn);
	if (0 == m_pA3DTrn)
		return;

	assert(0 != m_pCamera);
	if (0 == m_pCamera)
		return;

	static const float fStep = 0.3f;
	static const int iPosNum = 60;
	static float Delta = APHYSX_PI * 2 / iPosNum;
	static A3DVECTOR3 arrPos[iPosNum];
	
	float fRadius = m_pPhysXTrn->GetSimRadius();
	float Theta = 0.0f;
	
	for(int i = 0; i < iPosNum; ++i)
	{
		arrPos[i] = m_trnCenter;
		arrPos[i].x += fRadius * NxMath::cos(Theta);
		arrPos[i].z += fRadius * NxMath::sin(Theta);
		arrPos[i].y = m_pA3DTrn->GetPosHeight(arrPos[i]) + fStep;
		Theta += Delta;
	}
	
	for(int j = 0; j < iPosNum - 1; ++j)
		m_WCTopZbuf.Add3DLine(arrPos[j], arrPos[j + 1], 0xffffff00);
	m_WCTopZbuf.Add3DLine(arrPos[iPosNum - 1], arrPos[0], 0xffffff00);
}
