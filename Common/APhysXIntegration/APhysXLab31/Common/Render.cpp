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

//#include <algorithm>
#include <windows.h>
//#include "A3D.h"
//#include "ALog.h"
#include <A3DLPPLight.h>
#include <A3DGfxEngine.h>
#include <A3DOccQueryMan.h>
#include "ECWorldRender.h"

const unsigned int CRender::gForceBarLen = 50;

CRender::CRender()
{
	m_pA3DEngine   = 0;
	m_pA3DEnvmnt   = 0;
	m_pA3DDevice   = 0;
	m_pCamera	   = 0;
	m_pViewport	   = 0;
	m_pSysFont	   = 0;
	m_pDirLight	   = 0;
	m_pLPPDirLight = 0;

	m_pGfxEngine   = 0;
	m_pIEditorGfx  = 0;

//	m_ForceSize  = 0;
	m_xCenter = m_yCenter = 0;

	m_pPhysXTrn	= 0;
	m_pA3DTrn	= 0;
	m_trnCenter.Set(0, 0, 0);
	m_pA3DTrnWater = 0;

	m_pWorldRender = 0;
	m_pTerrain = 0;
	m_pSky = 0;
}

CRender::~CRender()
{
//	std::for_each(m_lstText.begin(), m_lstText.end(), DeleteIt<TextNode>());
	if (m_flags.ReadFlag(RENDER_INIT_START))
		Release();
}

void CRender::Release()
{
	m_flags.ClearFlag(RENDER_INIT_START);
	m_flags.ClearFlag(RENDER_INIT_ENDOK);

	A3DRELEASE(m_pWorldRender);
	delete m_pIEditorGfx;
	m_pIEditorGfx = 0;
	//A3DRELEASE(m_pIEditorGfx);
	A3DRELEASE(m_pGfxEngine);
	m_WCTopZbuf.Release();
	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pViewport);
	delete m_pLPPDirLight;
	m_pLPPDirLight = 0;
	A3DRELEASE(m_pDirLight);
	A3DRELEASE(m_pA3DEngine);
	
	m_pSysFont	 = 0;
	m_pA3DDevice = 0;
	m_pA3DEnvmnt = 0;
	m_pA3DTrnWater = 0;
}

bool CRender::Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen)
{
	if (m_flags.ReadFlag(RENDER_INIT_START))
		Release();

	m_flags.RaiseFlag(RENDER_INIT_START);
	RECT rect;
	if (!InitA3DEngine(hInst, hWnd, bFullScreen, rect))
		return false;
	
	if (!InitLight())
		return false;

	float fRatio = (rect.right - rect.left) * 1.0f / (rect.bottom - rect.top) * 1.0f;
	if (!InitCamera(fRatio))
		return false;

	if (!InitViewport(rect))
		return false;

	if (!InitGfxEngine())
		return false;

	m_pSysFont = m_pA3DEngine->GetSystemFont();
	m_WCTopZbuf.Init(m_pA3DEngine);

	// init the ECWorlderRender...
	m_pWorldRender = new ECWorldRender(*this);
	if (0 == m_pWorldRender)
		a_LogOutput(1, "CRender::Init: Failed to create ECWorldRender!");
	else
	{
		if (!m_pWorldRender->Init(GetA3DEngine()))
		{
			a_LogOutput(1, "CRender::Init: Failed to initalize ECWorldRender!");
			delete m_pWorldRender;
			m_pWorldRender = 0;
		}

		// add light...
		if(m_pLPPDirLight)
			m_pWorldRender->AddLight(*m_pLPPDirLight);
	}

	m_flags.RaiseFlag(RENDER_INIT_ENDOK);
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

	m_pA3DEnvmnt = m_pA3DEngine->GetA3DEnvironment();
	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iWidth	= 0;
	rtFmt.iHeight	= 0;
	rtFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	rtFmt.fmtDepth	= A3DFMT_UNKNOWN;
	
	if (0 == m_pA3DEngine)
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Not enough memory!");
		delete m_pA3DEngine;
		m_pA3DEngine = 0;
		m_pA3DEnvmnt = 0;
		m_pA3DDevice = 0;
		return false;
	}


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
						A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(-1.0f, -1.0f, 0.0f),
						A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 0.5f), 
						A3DCOLORVALUE(1.0f), 
						A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
	if (!rtn)
	{
		a_LogOutput(1, "CRender::InitLightAndFog: Failed to create directional light!");
		return false;
	}
	A3DCOLORVALUE colAmbient(0.588f, 0.588f, 0.588f, 1.0f);
	m_pA3DDevice->SetAmbient(colAmbient.ToRGBAColor());
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	const A3DLIGHTPARAM& lp = m_pDirLight->GetLightparam();
	m_pA3DEnvmnt->SetMainLightDirPos(lp.Direction);
	m_pA3DEnvmnt->SetMainLightColor(lp.Diffuse);
	m_pA3DEnvmnt->SetAmbient1(m_pA3DDevice->GetAmbientValue());

	assert(0 == m_pLPPDirLight);
	m_pLPPDirLight = new A3DLPPLightDir;
	if (0 == m_pLPPDirLight)
	{
		a_LogOutput(1, "CRender::InitLightAndFog: Failed to create A3DLPPLightDir!");
	}
	else
	{
		m_pLPPDirLight->EnableShadow(true);
		m_pLPPDirLight->SetPriority(A3DLPPLight::PRIORITY_MAJOR);
		m_pLPPDirLight->SetSpecFactor(1.0f);
	}
	return true;
}

bool CRender::InitCamera(float fRatio)
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

	if (!m_pCamera->Init(m_pA3DDevice, DEG2RAD(56.0f), 0.1f, 2000.0f, fRatio))
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

	assert(0 == m_pViewport);
	m_pViewport = new A3DViewport; 
	if (0 == m_pViewport)
	{
		a_LogOutput(1, "CRender::InitViewport: Not enough memory!");
		return false;
	}

	if (!m_pViewport->Init(m_pA3DDevice, Rect.left, Rect.top, 
						   Rect.right - Rect.left, Rect.bottom - Rect.top,
						   0.0f, 1.0f, true, true, A3DCOLORRGB(64, 64, 64)))
	{
		a_LogOutput(1, "CRender::InitViewport: Failed to initialize viewport!");
		return false;
	}
	m_pViewport->SetCamera(m_pCamera);
	m_pViewport->Active();
	A3DConfig::GetInstance()->RT_SetOccluderEnableFlag(true);

	m_xCenter = (Rect.right - Rect.left) / 2;
	m_yCenter = (Rect.bottom - Rect.top) / 2;
	return true;
}

bool CRender::InitGfxEngine()
{
	assert(0 != m_pA3DEngine);
	if (0 == m_pA3DEngine)
	{
		a_LogOutput(1, "CRender::InitGfxEngine: 'm_pA3DEngine' is invalid!");
		return false;
	}

	assert(0 == m_pGfxEngine);
	m_pGfxEngine = new A3DGfxEngine; 
	if (0 == m_pGfxEngine)
	{
		a_LogOutput(1, "CRender::InitGfxEngine: Not enough memory!");
		return false;
	}

	assert(0 == m_pIEditorGfx);
	m_pIEditorGfx = new IGfx(*this); 
	if (0 == m_pIEditorGfx)
	{
		a_LogOutput(1, "CRender::InitGfxEngine: Not enough memory!");
		delete m_pGfxEngine;
		m_pGfxEngine = 0;
		return false;
	}

//	m_pGfxEngine->GetA3DGfxExMan()->Set2DNoMoveCam(true);
	A3DGfxEngine::GfxEngineInit initPara;
	initPara.pEngine = m_pA3DEngine;
	initPara.pEventSystem = 0;
	initPara.pInterface = m_pIEditorGfx;
	if (!m_pGfxEngine->Init(initPara)) 
	{
		a_LogOutput(1, "CRender::InitGfxEngine(), failed to initialize A3DGfxEngine.");
		delete m_pIEditorGfx;
		m_pIEditorGfx = 0;
		delete m_pGfxEngine;
		m_pGfxEngine = 0;
		return false;
	}

	/*if (!m_pIEditorGfx->Init(m_pGfxEngine))
	{
		a_LogOutput(1, "CRender::InitA3DGfxEngine(), failed to initialize EditorGfxInterface.");
		delete m_pIEditorGfx;
		m_pIEditorGfx = 0;
		A3DRELEASE(m_pGfxEngine);
		return false;
	}*/
	return true;
}

bool CRender::ResizeDevice(const int iWid, const int iHei) const
{
	if (!IsReady() || 0 >= iWid || 0 >= iHei)
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
	if (!IsReady())
	{
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}

	if (!m_pA3DEngine->BeginRender())
		return false;

	const A3DLIGHTPARAM& lPara = m_pDirLight->GetLightparam();
	m_pLPPDirLight->SetDir(lPara.Direction);
	m_pLPPDirLight->SetColor(lPara.Diffuse);
	A3DCOLORVALUE colSun = lPara.Diffuse;
	if (0 != m_pA3DTrnWater)
	{
		if (m_pA3DTrnWater->IsUnderWater(m_pCamera->GetPos()))
			colSun *= A3DCOLORVALUE(m_pA3DTrnWater->GetWaterColor());
	}
	m_pA3DEnvmnt->SetMainLightColor(colSun);
	return true;
}

bool CRender::EndRender()
{
	assert(IsReady());
	if (!IsReady())
		return false;

	/*DrawTextAnimation();
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
*/
	if (m_flags.ReadFlag(RENDER_DRAW_CAMERA_DIR))
	{
		m_WCTopZbuf.AddLine_2D(A3DVECTOR3(float(m_xCenter - 5), float(m_yCenter), 0), A3DVECTOR3(float(m_xCenter + 6), float(m_yCenter), 0), A3DCOLORRGB(0, 192, 0));
		m_WCTopZbuf.AddLine_2D(A3DVECTOR3(float(m_xCenter), float(m_yCenter - 5), 0), A3DVECTOR3(float(m_xCenter), float(m_yCenter + 6), 0), A3DCOLORRGB(0, 192, 0));
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

void CRender::ClearViewport() const
{
	assert(IsReady());
	if (!IsReady())
		return;

	m_pViewport->Active();
	m_pViewport->ClearDevice();
}

void CRender::DrawBackground() const
{
	A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();
	pFC->AddRect_2D(0, 0, m_xCenter * 2, m_yCenter * 2, A3DCOLORRGB(50, 50, 50), 1.0f);
	pFC->Flush_2D();
}

void CRender::TextOut(int x, int y, const ACHAR* szText, int iLen, DWORD color) const
{
	if (0 != m_pSysFont)
		m_pSysFont->AddText(x, y, szText, iLen, color);
}
/*
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
}*/

bool CRender::OnSetCursor() const
{
	assert(IsReady());
	if (!IsReady())
		return false;

	A3DCursor* pCursor = m_pA3DDevice->GetCursor();
	if (0 == pCursor)
		return false;

	pCursor->OnSysSetCursor();
	return true;
}

bool CRender::ShowCursor(A3DCursor* pA3DCursor) const
{
	assert(IsReady());
	if (!IsReady())
		return false;

	m_pA3DDevice->SetCursor(pA3DCursor);
	m_pA3DDevice->ShowCursor(true);
	return true;
}

bool CRender::SetLightInfo(A3DSkinModel& skinModel) const
{
	assert(IsReady());
	if (!IsReady())
		return false;

	const A3DLIGHTPARAM& lp = m_pDirLight->GetLightparam();

	A3DSkinModelLight li;
	li.colAmbient	= m_pA3DDevice->GetAmbientValue();
	li.dl_vDir		= lp.Direction;
	li.dl_color		= A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);//lp.Diffuse;
	li.pl_bEnable	= false;
	skinModel.SetLightInfo(li);
	return true;
}
/*//del sky
bool CRender::SetSky(A3DSky& sky) const
{
	assert(EngineIsReady());
	if (!EngineIsReady())
		return false;

	return m_pA3DEngine->SetSky(&sky);
}

void CRender::SetupCamera(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) const
{
	assert(0 != m_pCamera);
	if (0 == m_pCamera)
		return;

	m_pCamera->SetPos(vPos);
	m_pCamera->SetDirAndUp(vDir, vUp);
}*/
/*
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
}*/

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
	float fHeight = 0.0f;
	
	for(int i = 0; i < iPosNum; ++i)
	{
		arrPos[i] = m_trnCenter;
		arrPos[i].x += fRadius * NxMath::cos(Theta);
		arrPos[i].z += fRadius * NxMath::sin(Theta);
		arrPos[i].y = fStep;
		if (m_pA3DTrn->GetPosHeight(arrPos[i], fHeight))
			arrPos[i].y += fHeight;
		Theta += Delta;
	}
	
	for(int j = 0; j < iPosNum - 1; ++j)
		m_WCTopZbuf.Add3DLine(arrPos[j], arrPos[j + 1], 0xffffff00);
	m_WCTopZbuf.Add3DLine(arrPos[iPosNum - 1], arrPos[0], 0xffffff00);
}

void CRender::RenderStuff()
{
	//	Build occluder data
	A3DOccQueryMan* pOccQueryMan = GetA3DEngine()->GetA3DOccQueryMan();
	if (pOccQueryMan && A3DConfig::GetInstance()->RT_GetOccluderEnableFlag())
		pOccQueryMan->PrepareRender(GetViewport());


	//	Update the environment lighting...
	A3DEnvironment* pA3DEnv = GetA3DEngine()->GetA3DEnvironment();
	A3DLight* pLight = GetDirectionalLight();
	A3DCOLORVALUE colWater(1.0f);

	A3DCOLORVALUE colSun = pLight->GetLightparam().Diffuse;
	pA3DEnv->SetMainLightDirPos(pLight->GetLightparam().Direction);
	pA3DEnv->SetMainLightColor(colSun);
	pA3DEnv->SetAmbient1(A3DCOLORVALUE(0.4f, 0.4f, 0.4f, 1.0f) * colWater);
	pA3DEnv->SetAmbient2(A3DCOLORVALUE(0.0, 0.4f, 0.0f, 1.0f) * colWater);
	pA3DEnv->SetShadowDeepness(0.2f);

	// render the sky...
	m_pSky->Render(m_pViewport);
	
	// real render...
	m_pWorldRender->Render(m_pViewport, m_pLPPDirLight);
	
	// render some extra data...
	RenderExtraData(*this);

	// build occluder states...
	if (pOccQueryMan && A3DConfig::GetInstance()->RT_GetOccluderEnableFlag())
		pOccQueryMan->BuildOccluderQueryState(GetViewport());

	//	m_pWorldRender->GetShadowAdjuster()->Render();
	// m_pG3DEngine->DrawBackground();
}


void CRender::RegisterTerrain(A3DTerrain2*  pTerrain)
{
	if(m_pWorldRender)
	{
		m_pWorldRender->OnTerrainInit(*pTerrain);
	}
	
	m_pTerrain = pTerrain;
}

void CRender::UnregesiterTerrain()
{
	if(m_pWorldRender)
	{
		m_pWorldRender->OnTerrainRelease();
	}
	m_pTerrain = NULL;
}



//---------------------------------------------------------------------
// Implementation of IGfx...

A3DSkillGfxEventMan* IGfx::GetSkillGfxEventMan() const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

A3DSkillGfxComposerMan* IGfx::GetSkillGfxComposerMan() const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

A3DSkinModel* IGfx::LoadA3DSkinModel(const char* szFile, int iSkinFlag) const
{
	return CPhysXObjSkinModel::LoadSkinModel(m_render, szFile, iSkinFlag);
}

void IGfx::ReleaseA3DSkinModel(A3DSkinModel* pModel) const
{
	CPhysXObjSkinModel::ReleaseSkinModel(pModel);
}

bool IGfx::LoadECMHullData(const char* szChfFile, ECMHullDataLoad* pECMHullData) const
{
	return false;
}

const char * IGfx::GetECMHullPath() const
{
	return "";
}

bool IGfx::GetModelUpdateFlag() const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

bool IGfx::GetModelLight(const A3DVECTOR3& vPos, A3DSkinModelLight& light) const
{
	return false;
}

void IGfx::ECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel) const
{
	// virtual function
	assert(!"Oops! Not implementation!");
}

float IGfx::GetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

bool IGfx::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

AM3DSoundBuffer* IGfx::LoadNonLoopSound(const char* szFile, int nPriority) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

AM3DSoundBuffer* IGfx::LoadLoopSound(const char* szFile) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

void IGfx::ReleaseSoundNonLoop(AM3DSoundBuffer*& pSound) const
{
	// virtual function
	assert(!"Oops! Not implementation!");
}

void IGfx::ReleaseSoundLoop(AM3DSoundBuffer*& pSound) const
{
	// virtual function
	assert(!"Oops! Not implementation!");
}

AudioEngine::EventInstance* IGfx::CreateAudioEventInstance(const char* szEventFullPath) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

void IGfx::ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const
{
	assert(!"Oops! Not implementation!");
}

A3DCameraBase* IGfx::GetA3DCamera() const
{
	return m_render.GetCamera();
}

float IGfx::GetAverageFrameRate() const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

bool IGfx::GetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

bool IGfx::PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

void IGfx::SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier, const ECMODEL_SHAKE* shake) const
{
	// virtual function
	assert(!"Oops! Not implementation!");
}

void IGfx::SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const
{
	// virtual function
	assert(!"Oops! Not implementation!");
}

bool IGfx::SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE* shake) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}

