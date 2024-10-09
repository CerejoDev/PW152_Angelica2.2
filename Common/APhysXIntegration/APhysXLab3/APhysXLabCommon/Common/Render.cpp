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

#include <windows.h>
#include <A3DGFXExMan.h>
#include "ECWorldRender.h"
#include "ApxAttack.h"
#include "A3DSkillGfxComposer.h"

#ifdef _ANGELICA3
	#include <A3DLPPLight.h>
	#include <A3DLPPRender.h>
	#include <A3DOccQueryMan.h>
	#include <A3DGfxEngine.h>
	#include "ECPlatform.h"
	#include "ECGraphicsModule.h"
	#include "ECTriangleMan.h"

	static A3DLPPLightSpot* s_pLPPCameraSpotLight = NULL;
#else
	#include <A3DSceneRenderConfig.h>
#endif

const unsigned int CRender::gForceBarLen = 50;

static A3DLight* s_pCameraSpotLight = NULL;

CRender::CRender()
{
	m_pA3DEngine   = 0;
	m_pA3DDevice   = 0;
	m_pCamera	   = 0;
	m_pViewport	   = 0;
	m_pSysFont	   = 0;

#ifdef _ANGELICA3
	m_pA3DEnvmnt   = 0;
	m_pLPPDirLight = 0;
	m_pGfxEngine   = 0;
	m_pIEditorGfx  = 0;

	m_pECPlatform  = 0;
	m_pECGraphic   = 0;
#endif

	m_pDirLight	   = 0;
	m_pAASGM	   = 0;

//	m_ForceSize  = 0;
	m_fLoadPercent = 0.0f;
	m_xCenter = m_yCenter = 0;

	m_pPhysXTrn	= 0;
	m_pA3DTrn	= 0;
	m_trnCenter.Set(0, 0, 0);
	m_pA3DTrnWater = 0;

	m_pTerrain = 0;
	m_pSky = 0;
	m_pWorldRender = 0;
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
	UnregisterSky();
	UnregesiterTerrain();
	m_pA3DTrnWater	= 0;
	m_pA3DTrn		= 0;
	m_pPhysXTrn		= 0;

	m_pAASGM = 0;
	m_WCTopZbuf.Release();
	A3DRELEASE(m_pDirLight);

#ifdef _ANGELICA3
	m_pECGraphic = 0;
	m_pECPlatform->Release();
	m_pECPlatform = 0;

	delete m_pIEditorGfx;
	m_pIEditorGfx = 0;
	m_pGfxEngine = 0;
	delete m_pLPPDirLight;
	m_pLPPDirLight = 0;
	m_pA3DEnvmnt = 0;
#else
	if (0 != m_pA3DEngine)
	{
		m_pA3DEngine->SetActiveCamera(0);
		m_pA3DEngine->SetActiveViewport(0);
	}
#endif

	m_pSysFont = 0;
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pCamera);
	m_pA3DDevice = 0;
#ifndef _ANGELICA3
	A3DRELEASE(m_pA3DEngine);
#endif
	m_pA3DEngine = 0;
}

bool CRender::Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen)
{
	if (m_flags.ReadFlag(RENDER_INIT_START))
		Release();

	m_flags.RaiseFlag(RENDER_INIT_START);
	RECT rect;
	rect.left = rect.top = rect.right = rect.bottom = 0;
	GetClientRect(hWnd, &rect);

#ifdef _ANGELICA3
	ECPlatformInit init;
	init.m_hInstance		= hInst;
	init.m_hWnd				= hWnd;
	init.m_pGfxInterface	= NULL;
	init.m_iScreenW			= rect.right;
	init.m_iScreenH			= rect.bottom;
	init.m_bFullScreen		= bFullScreen;

	assert(0 == m_pIEditorGfx);
	m_pIEditorGfx = new IGfx(*this); 
	if (0 == m_pIEditorGfx)
	{
		a_LogOutput(1, "Error in %s, Not enough memory!", __FUNCTION__);
		return false;
	}
	init.m_pGfxInterface = m_pIEditorGfx;
	m_pECPlatform = ECPlatform::GetPlatform();
	if (!m_pECPlatform->InitModules(init))
	{
		a_LogOutput(1, "Error in %s, Failed to initialize ECPlatform modules", __FUNCTION__);
		return false;
	}
	m_pECGraphic = m_pECPlatform->GetGraphicsModule();
	m_pA3DEngine = m_pECPlatform->GetA3DEngine();
	m_pA3DEnvmnt = m_pA3DEngine->GetA3DEnvironment();
	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();
	m_pGfxEngine = m_pECGraphic->GetGFXEngine();
#else
	assert(0 == m_pA3DEngine);
	m_pA3DEngine = new A3DEngine;  
	if (0 == m_pA3DEngine)
	{
		a_LogOutput(1, "CRender::Init: Not enough memory!");
		return false;
	}

	A3DDEVFMT devFmt;
	devFmt.bWindowed	= !bFullScreen;
	devFmt.nWidth		= rect.right;
	devFmt.nHeight		= rect.bottom;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;	//	Hardware render
	if (!m_pA3DEngine->Init(hInst, hWnd, &devFmt, dwDevFlags))
	{
		a_LogOutput(1, "CRender::Init: Failed to initialize A3D engine!");
		delete m_pA3DEngine;
		m_pA3DEngine = 0;
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();
	m_pA3DEngine->GetA3DSkinMan()->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");
	m_psr1 = m_pA3DEngine->GetA3DSkinMan()->GetDefSkinRender();
	m_psr1->SetDefaultAlphaRef(0.33f);
#endif

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
	}

	if (!InitLight())
		return false;

	float fRatio = (rect.right - rect.left) * 1.0f / (rect.bottom - rect.top) * 1.0f;
	if (!InitCamera(fRatio))
		return false;

	if (!InitViewport(rect))
		return false;

	m_pSysFont = m_pA3DEngine->GetSystemFont();
	m_WCTopZbuf.Init(m_pA3DEngine);

#ifdef _ANGELICA3
	// add light...
	if(m_pLPPDirLight)
		m_pWorldRender->AddLight(*m_pLPPDirLight);
#endif

	m_flags.RaiseFlag(RENDER_INIT_ENDOK);
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
		A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 0.5f), 
		A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

	if (!rtn)
	{
		a_LogOutput(1, "CRender::InitLightAndFog: Failed to create directional light!");
		return false;
	}

	A3DCOLORVALUE colAmbient(0.3f, 0.3f, 0.3f, 1.0f);
	m_pA3DDevice->SetAmbient(colAmbient.ToRGBAColor());
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	const A3DLIGHTPARAM& lp = m_pDirLight->GetLightparam();
#ifdef _ANGELICA3
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
#else
	m_pA3DEngine->GetA3DSkinMan()->SetDirLight(m_pDirLight);
	A3DSceneRenderConfig* pRenderCfg = m_pA3DEngine->GetSceneRenderConfig();
//	pRenderCfg->SetAmbient(A3DCOLORVALUE(0.88941181f, 1.0305884f, 1.0682354f, 1.0f), A3DCOLORVALUE(0.76705891f, 0.56000006f, 0.43294120f, 1.0f));
//	pRenderCfg->SetAmbient(A3DCOLORVALUE(0.48941181f, 0.54f, 0.682354f, 1.0f), A3DCOLORVALUE(0.5705891f, 0.3000006f, 0.253294120f, 1.0f));
	pRenderCfg->SetAmbient(A3DCOLORVALUE(0.141181f, 0.24f, 0.34f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
	pRenderCfg->SetSunLightDir(A3DVECTOR4(lp.Direction.x, lp.Direction.y, lp.Direction.z, 0));
	pRenderCfg->SetSunLightColor(lp.Diffuse);
#endif

#if 0
	// create the spot light attached to the camera...
	bool bCreateCameraSpotLight = m_pA3DEngine->GetA3DLightMan()->CreateSpotLight(2, &s_pCameraSpotLight, 
		A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(0.0f, -1.0f, 0.0f),
		A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 0.5f), 
		A3DCOLORVALUE(1.0f), 
		A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 
		10.0f,
		1.0f,
		APHYSX_PI /3.0f,
		APHYSX_PI / 2,
		1.0f
		);

	s_pLPPCameraSpotLight = new A3DLPPLightSpot;
	s_pLPPCameraSpotLight->SetAngles(DEG2RAD(10.0f), DEG2RAD(40.0f));
	// s_pLPPCameraSpotLight->SetAttenuation(1.0f, 3.0f, 0.0f);
	s_pLPPCameraSpotLight->SetAttenuation(1.0f, 0.0f, 0.0f);
	s_pLPPCameraSpotLight->SetRange(60.0f);
	s_pLPPCameraSpotLight->SetColor(A3DCOLORVALUE(1.0f, 0.0f, 0.0f, 1.0f));
	//	pSpotLight->SetColor(A3DCOLORVALUE(0.0f));
	//	pSpotLight->EnableShadow(true);
	s_pLPPCameraSpotLight->Enable(true);
	m_pWorldRender->GetLPPRender()->GetLightMan()->AddLight(s_pLPPCameraSpotLight);
#endif

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

#ifdef _ANGELICA3
	if (!m_pViewport->Init(m_pA3DDevice, Rect.left, Rect.top, 
						   Rect.right - Rect.left, Rect.bottom - Rect.top,
						   0.0f, 1.0f, true, true, A3DCOLORRGB(64, 64, 64)))
#else
	A3DVIEWPORTPARAM Params;
	Params.X	  = Rect.left;
	Params.Y	  = Rect.top;
	Params.Width  = Rect.right - Rect.left;
	Params.Height = Rect.bottom - Rect.top;
	Params.MinZ	  = 0.0f;
	Params.MaxZ	  = 1.0f;
	if (!m_pViewport->Init(m_pA3DDevice, &Params, true, true, A3DCOLORRGB(64, 64, 64)))
#endif
	{
		a_LogOutput(1, "CRender::InitViewport: Failed to initialize viewport!");
		return false;
	}
	m_pViewport->SetCamera(m_pCamera);
	m_pViewport->Active();
#ifdef _ANGELICA3
	A3DConfig::GetInstance()->RT_SetOccluderEnableFlag(true);
#endif

	m_xCenter = (Rect.right - Rect.left) / 2;
	m_yCenter = (Rect.bottom - Rect.top) / 2;
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
		return false;

#ifdef _ANGELICA3
	if (!m_pECGraphic->BeginRender())
		return false;
	m_pECGraphic->PrepareOccluderQuery(m_pViewport);
#else
	m_pA3DEngine->BeginRender();
#endif

#if 0

	if(s_pCameraSpotLight)
	{

		A3DLIGHTPARAM lp = s_pCameraSpotLight->GetLightparam();
		lp.Position = GetCamera()->GetPos();
		lp.Direction = GetCamera()->GetDir();

		s_pCameraSpotLight->SetLightParam(lp);
	}

	if(s_pLPPCameraSpotLight)
	{
		s_pLPPCameraSpotLight->SetPos(GetCamera()->GetPos());
		s_pLPPCameraSpotLight->SetDir(GetCamera()->GetDir());
	}

#endif

#ifdef _ANGELICA3
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
#endif

	ClearViewport();
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
	if (m_LoadingBar.bIsEnable)
	{
		if (0 > m_fLoadPercent)
			m_fLoadPercent = 0.0f;
		if (1 < m_fLoadPercent)
			m_fLoadPercent = 1.0f;
		unsigned int nSize = unsigned int((m_LoadingBar.r - m_LoadingBar.l) * m_fLoadPercent); 
		m_WCTopZbuf.AddRect_2D(m_LoadingBar.l, m_LoadingBar.t, m_LoadingBar.r, m_LoadingBar.b, m_LoadingBar.color);
		A3DFlatCollector* pFC = m_pA3DEngine->GetA3DFlatCollector();
		pFC->AddRect_2D(m_LoadingBar.l, m_LoadingBar.t + 2, m_LoadingBar.l + nSize, m_LoadingBar.b - 1, m_LoadingBar.color);

		int txtW = 0, txtH = 0;
		TString strText(_T("Loading record episode..."));
		m_pSysFont->GetTextExtent(strText, strText.GetLength(), &txtW, &txtH);
		int xp = m_LoadingBar.l - 5 - txtW;
		int yp = m_yCenter - int(txtH * 0.5f);
		TextOut(xp, yp, strText, strText.GetLength(), m_LoadingBar.color);
	}
	else
	{
		if (m_flags.ReadFlag(RENDER_DRAW_CAMERA_DIR))
		{
			m_WCTopZbuf.AddLine_2D(A3DVECTOR3(float(m_xCenter - 5), float(m_yCenter), 0), A3DVECTOR3(float(m_xCenter + 6), float(m_yCenter), 0), A3DCOLORRGB(0, 192, 0));
			m_WCTopZbuf.AddLine_2D(A3DVECTOR3(float(m_xCenter), float(m_yCenter - 5), 0), A3DVECTOR3(float(m_xCenter), float(m_yCenter + 6), 0), A3DCOLORRGB(0, 192, 0));
		}

		if ((0 != m_pPhysXTrn) && (0 != m_pA3DTrn))
			DrawPhysXTerrainRange();
	}
	
#ifdef _ANGELICA3
	m_pECGraphic->DoPostEffects(m_pViewport);
#endif

	m_pA3DDevice->SetZTestEnable(false);
	m_WCTopZbuf.Flush();
	m_pA3DDevice->SetZTestEnable(true);

#ifdef _ANGELICA3
	if (!m_pECGraphic->EndRender())
		return false;
#else
	if (!m_pA3DEngine->EndRender())
		return false;
	return m_pA3DEngine->Present();
#endif
	return true;
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
	{
#ifdef _ANGELICA3
		m_pSysFont->AddText(x, y, szText, iLen, color);
#else
		m_pSysFont->TextOut(x, y, szText, color);
#endif
	}
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

void CRender::SetAttackSkillGfxMan(ApxAttackSkillGfxMan* pAASGM)
{
	m_pAASGM = pAASGM;
	if (0 != pAASGM)
		ApxAttackEvent::SetRenderManager(this);
	else
		ApxAttackEvent::SetRenderManager(0);
}

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

#ifdef _ANGELICA3
	A3DSkinModelLight li;
	li.colAmbient	= m_pA3DDevice->GetAmbientValue();
	li.dl_vDir		= lp.Direction;
	li.dl_color		= A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);//lp.Diffuse;
	li.pl_bEnable	= false;
	skinModel.SetLightInfo(li);
#else
	A3DSceneRenderConfig* pRenderCfg = m_pA3DEngine->GetSceneRenderConfig();

	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));
	LightInfo.colAmbient = pRenderCfg->GetAmbient();
	LightInfo.vLightDir	 = lp.Direction;
	LightInfo.colDirDiff = lp.Diffuse;
	LightInfo.colDirSpec = lp.Specular;
	LightInfo.bPtLight	 = false;

	float fminDist = FLT_MAX;
	ApxObjSpotLight* ptheSpot = 0;
	APtrArray<IObjBase*> arrLights;
	int nLights = ObjManager::GetInstance()->GetLiveObjects(arrLights, ObjManager::OBJ_TYPEID_SPOTLIGHT);
	for (int i = 0; i < nLights; ++i)
	{
		ApxObjSpotLight* pSpot = dynamic_cast<ApxObjSpotLight*>(arrLights[i]);
		if (!pSpot->IsEnabled())
			continue;
		A3DVECTOR3 smp = skinModel.GetPos();
		NxVec3 dV = pSpot->GetGPos() - NxVec3(smp.x, smp.y, smp.z);
		float d = dV.normalize();
		if (d > pSpot->GetRange())
			continue;
		if (d > fminDist)
			continue;
		A3DVECTOR3 dir, up;
		pSpot->GetGDirAndUp(dir, up);
		float v = NxVec3(dir.x, dir.y, dir.z).dot(dV);
		if (0 < v)
			continue;
		fminDist = d;
		ptheSpot = pSpot;
	}
	if (0 != ptheSpot)
	{
		LightInfo.bPtLight = true;
		LightInfo.colPtDiff = ptheSpot->GetColor();
		LightInfo.colPtAmb = LightInfo.colAmbient;
		LightInfo.vPtAtten = ptheSpot->GetAttenuation();
		LightInfo.fPtRange = ptheSpot->GetRange();
		LightInfo.vPtLightPos = APhysXConverter::N2A_Vector3(ptheSpot->GetGPos());
	}
	skinModel.SetLightInfo(LightInfo);
#endif
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

void CRender::DrawProgressBar(float percent, int xCenterPos, int yCenterPos, const DWORD color, int width, int height)
{
	m_fLoadPercent = percent;
	m_LoadingBar.bIsEnable = true;
	m_LoadingBar.l = xCenterPos - int(width * 0.5f);
	m_LoadingBar.b = yCenterPos	+ int(height * 0.5f);
	m_LoadingBar.r = xCenterPos + int(width * 0.5f);
	m_LoadingBar.t = yCenterPos - int(height * 0.5f);
	m_LoadingBar.color = color;
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
	float fHeight = 0.0f;
	
	for(int i = 0; i < iPosNum; ++i)
	{
		arrPos[i] = m_trnCenter;
		arrPos[i].x += fRadius * NxMath::cos(Theta);
		arrPos[i].z += fRadius * NxMath::sin(Theta);
		arrPos[i].y = fStep;
		if (GetTerrainHeight(m_pA3DTrn, arrPos[i], fHeight))
			arrPos[i].y += fHeight;
		Theta += Delta;
	}
	
	for(int j = 0; j < iPosNum - 1; ++j)
		m_WCTopZbuf.Add3DLine(arrPos[j], arrPos[j + 1], 0xffffff00);
	m_WCTopZbuf.Add3DLine(arrPos[iPosNum - 1], arrPos[0], 0xffffff00);
}

#ifdef _ANGELICA3
void CRender::BuildShadowCasters(APtrArray<A3DShadowCaster*>& aCasters)
{
	int i;
	A3DAABB aabb;
	A3DShadowCaster *pCaster = NULL;
	for (i = 0; i < m_arrLitModel.GetSize(); i++)
	{
		aabb = m_arrLitModel[i]->GetModelAABB();
		aabb.CompleteMinsMaxs();
		pCaster = new A3DShadowCaster;
		pCaster->vMins = aabb.Mins;
		pCaster->vMaxs = aabb.Maxs;
		aCasters.Add(pCaster);
	}
	for (i = 0; i < m_arrSkinModel.GetSize(); i++)
	{
		aabb = m_arrSkinModel[i]->GetModelAABB();
		aabb.CompleteMinsMaxs();
		pCaster = new A3DShadowCaster;
		pCaster->vMins = aabb.Mins;
		pCaster->vMaxs = aabb.Maxs;
		aCasters.Add(pCaster);
	}

	//gfxex shadow caster?
	
	for (i = 0; i < m_arrECM.GetSize(); i++)
	{
		aabb = m_arrECM[i]->GetModelAABB();
		aabb.CompleteMinsMaxs();
		pCaster = new A3DShadowCaster;
		pCaster->vMins = aabb.Mins;
		pCaster->vMaxs = aabb.Maxs;
		aCasters.Add(pCaster);
	}
}

void CRender::RenderShadowMap(A3DViewport* pViewport, const APtrArray<A3DShadowCaster*>& aCasters, int iSlice)
{
	assert(m_arrLitModel.GetSize() + m_arrSkinModel.GetSize() + m_arrECM.GetSize() == aCasters.GetSize());
	int i, k = 0;
	A3DLitModel::VISCHECKFLAG flag = A3DLitModel::VIS_NOT_CHECK;
	DWORD dwSliceMask = 1 << iSlice;
	for (i = 0; i < m_arrLitModel.GetSize(); i++)
	{
		if (aCasters[k++]->dwSlices & dwSliceMask)
			m_arrLitModel[i]->Render(pViewport, flag);
	}
	A3DSkinModel::VISCHECKFLAG flag1 = A3DSkinModel::VIS_NOT_CHECK;
	for (i = 0; i < m_arrSkinModel.GetSize(); i++)
	{
		if (aCasters[k++]->dwSlices & dwSliceMask)
			m_arrSkinModel[i]->Render(pViewport, flag1);
	}
/*	for (i = 0; i < m_arrGfx.GetSize(); i++)
	{
		if (aCasters[k++]->dwSlices & dwSliceMask)
		{
			if(ST_PLAY == m_arrGfx[i]->GetState())
				GetGFXEngine()->GetA3DGfxExMan()->RegisterGfx(m_arrGfx[i]);
		}
	}
*/	for (i = 0; i < m_arrECM.GetSize(); i++)
	{
		if (aCasters[k++]->dwSlices & dwSliceMask)
			m_arrECM[i]->Render(pViewport);
	}
}
#else
void CRender::RenderOrnaments(bool bUpdateShadowMap, int nRenderMode)
{
	int nSize = m_arrLitModel.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_arrLitModel[i]->Render(m_pViewport, bUpdateShadowMap, nRenderMode);
}

bool CRender::RenderForShadowMap(A3DViewport* pViewport)
{
	int nSize = m_arrSkinModel.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (m_arrSkinModel[i]->GetTransparent() < 0)
			m_arrSkinModel[i]->Render(pViewport);
	}

	nSize = m_arrECM.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		A3DSkinModel* pSkinModel = m_arrECM[i]->GetA3DSkinModel();
		if (pSkinModel->GetTransparent() < 0)
			pSkinModel->Render(pViewport);
	}
	return true;
}
#endif

void CRender::Render(A3DViewport* pViewport, bool bDoVisCheck) const
{
	int i;
#ifdef _ANGELICA3
	A3DLitModel::VISCHECKFLAG flag;
	if (bDoVisCheck)
		flag = A3DLitModel::VIS_CHECK;		//	Do visibility check
	else
		flag = A3DLitModel::VIS_NOT_CHECK;
	int nSize = m_arrLitModel.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		m_arrLitModel[i]->Render(pViewport, flag);
	}

	A3DSkinModel::VISCHECKFLAG flag1;
	if (bDoVisCheck)
		flag1 = A3DSkinModel::VIS_CHECK;		//	Do visibility check
	else
		flag1 = A3DSkinModel::VIS_NOT_CHECK;
	nSize = m_arrSkinModel.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		m_arrSkinModel[i]->Render(pViewport, flag1);
	}

	nSize = m_arrGfx.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		if(ST_PLAY == m_arrGfx[i]->GetState())
			GetGFXEngine()->GetA3DGfxExMan()->RegisterGfx(m_arrGfx[i]);
	}
#else
	int nSize = m_arrLitModel.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		m_arrLitModel[i]->SetVisibility(bDoVisCheck);
		m_arrLitModel[i]->Render(pViewport);
	}

	nSize = m_arrSkinModel.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		SetLightInfo(*(m_arrSkinModel[i]));
		m_arrSkinModel[i]->Render(pViewport, bDoVisCheck);
	}

	nSize = m_arrGfx.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		if(ST_PLAY == m_arrGfx[i]->GetState())
			AfxGetGFXExMan()->RegisterGfx(m_arrGfx[i]);
	}

#endif

	nSize = m_arrECM.GetSize();
	for (i = 0; i < nSize; ++i)
	{
#ifndef _ANGELICA3
		A3DSkinModel* pSkinModel = m_arrECM[i]->GetA3DSkinModel();
		pSkinModel->SetExtraEmissive(m_arrECM[i]->GetExtraEmissive());
		SetLightInfo(*pSkinModel);
#endif
		m_arrECM[i]->Render(pViewport);
	}

	A3DWireCollector* pWC = GetWireCollector();
	if (0 == pWC)
		return;

	nSize = m_arrFF.GetSize();
	for (i = 0; i < nSize; ++i)
		m_arrFF[i]->DrawAllFFShapes(*pWC);

	nSize = m_arrWireRender.GetSize();
	for (i = 0; i < nSize; ++i)
	{
		int k;
		const AArray<ExtraRenderable::AABB>& aabbs = m_arrWireRender[i]->GetAABBArray();
		int nCount = aabbs.GetSize();
		for (k = 0; k < nCount; ++k)
			pWC->AddAABB(aabbs[k].aabb, aabbs[k].dwColor);

		const AArray<ExtraRenderable::OBB>& obbs = m_arrWireRender[i]->GetOBBArray();
		nCount = obbs.GetSize();
		for (k = 0; k < nCount; ++k)
			pWC->AddOBB(obbs[k].obb, obbs[k].dwColor);

		const AArray<ExtraRenderable::Sphere>& spheres = m_arrWireRender[i]->GetSphereArray();
		nCount = spheres.GetSize();
		for (k = 0; k < nCount; ++k)
			pWC->AddSphere(spheres[k].vCenter, spheres[k].radius, spheres[k].dwColor, &spheres[k].mat);

		const AArray<ExtraRenderable::Capsule>& capsules = m_arrWireRender[i]->GetCapsuleArray();
		nCount = capsules.GetSize();
		for (k = 0; k < nCount; ++k)
			pWC->AddCapsule(capsules[k].capsule, capsules[k].dwColor, &capsules[k].mat);

		const AArray<ExtraRenderable::Line>& lines = m_arrWireRender[i]->GetLineArray();
		nCount = lines.GetSize();
		for (k = 0; k < nCount; ++k)
			pWC->Add3DLine(lines[k].vStart, lines[k].vEnd, lines[k].dwColor);
		
		const APtrArray<ExtraRenderable::RenderData3D*>& renderDatas = m_arrWireRender[i]->GetRenderDataArray();
		nCount = renderDatas.GetSize();
		for (k = 0; k < nCount; ++k)
			pWC->AddRenderData_3D(renderDatas[k]->aVerts, renderDatas[k]->iNumVert, renderDatas[k]->aIndex, renderDatas[k]->iNumIdx, renderDatas[k]->dwColor);
	}
}

void CRender::RenderStuff()
{

#if 0

	// some test codes...

	static A3DLPPLightDir* s_pLPPLight = NULL;
	if(!s_pLPPLight) 
		s_pLPPLight = new A3DLPPLightDir;

	if(s_pLPPLight)
	{
		s_pLPPLight->EnableShadow(true);
		s_pLPPLight->SetSpecFactor(1.0f);

		const A3DLIGHTPARAM& lPara = m_pDirLight->GetLightparam();
		A3DVECTOR3 vDir(1.0f, -1.0f, 1.0f);
		vDir.Normalize();
		s_pLPPLight->SetDir(vDir);
		s_pLPPLight->SetColor(lPara.Diffuse);

		m_pWorldRender->Render(m_pViewport, s_pLPPLight);

	}

#endif

#ifdef _ANGELICA3
	//	Update the environment lighting...
	A3DEnvironment* pA3DEnv = GetA3DEngine()->GetA3DEnvironment();
	A3DLight* pLight = GetDirectionalLight();
	A3DCOLORVALUE colWater(1.0f);

	A3DCOLORVALUE colSun = pLight->GetLightparam().Diffuse;
	pA3DEnv->SetMainLightDirPos(pLight->GetLightparam().Direction);
	pA3DEnv->SetMainLightColor(colSun);
	pA3DEnv->SetAmbient1(A3DCOLORVALUE(0.4f, 0.4f, 0.4f, 1.0f) * colWater);
	pA3DEnv->SetAmbient2(A3DCOLORVALUE(0.0, 0.4f, 0.0f, 1.0f) * colWater);

	// render the sky...
	//m_pSky->Render(m_pViewport);

	// real render...
	m_pWorldRender->Render(m_pViewport, m_pLPPDirLight);
#else

	/*if (0 != m_pSky)
	{
		m_pSky->SetCamera(m_pCamera);
		m_pSky->Render();
	}*/

//	if (0 != m_pTerrain)
//		m_pTerrain->Render(m_pViewport, false, false);

	Render(m_pViewport, true);
#endif

/*
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
*/
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
// remove the following assert, by wenfeng.
//#undef assert
//#define assert(x)

#ifdef _ANGELICA3
//---------------------------------------------------------------------
// Implementation of IGfx...

IGfx::IGfx(CRender& render) : m_render(render)
{
	m_Inited = false;
	m_pSGCM = new A3DSkillGfxComposerMan;
}

IGfx::~IGfx()
{
	if (0 != m_pSGCM)
	{
		if (m_Inited)
			m_pSGCM->Release();
		m_Inited = false;
		delete m_pSGCM;
		m_pSGCM = 0;
	}
}

A3DSkillGfxEventMan* IGfx::GetSkillGfxEventMan() const
{
	return m_render.GetAttackSkillGfxMan();
}

A3DSkillGfxComposerMan* IGfx::GetSkillGfxComposerMan() const
{
	if (!m_Inited)
	{
		if (0 != m_pSGCM)
		{
			m_pSGCM->Init(m_render.GetGFXEngine());
			m_Inited = true;
		}
	}
	return m_pSGCM;
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
	// 注：地宫和洞口会出问题 未测试，  建筑上无效果 // 效率高 By： Newer   2011-2-25
	float h = 0.0f;
	A3DTerrain2* pTerrain = m_render.GetTerrain();
	if (0 != pTerrain)
		pTerrain->GetPosHeight(vPos, h, pNorm);
	return h;
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
	A3DTerrain2* pTerrain = m_render.GetTerrain();
	int _nVertCount = 0;
	int _nIndexCount = 0;
	int nTriVertCount = 0;
	int nTriIndexCount = 0;
	int i;

	if (pTerrain)
	{
		int nx, nz;
		nx = nz = int(fRadus * 2.0f / pTerrain->GetGridSize()) + 2;
		_nVertCount = (nx + 1) * (nz + 1);
		_nIndexCount = nx * nz * 6;

		if (_nVertCount > nVertCount || _nIndexCount > nIndexCount)
			return false;

		RAYTRACERT trace;
		A3DVECTOR3 v = vCenter;
		v.y += 1.0f;

		int iNumVert, iNumIndex;
		if (!pTerrain->RayTrace(v, A3DVECTOR3(0, -4.f, 0), 1.0f, &trace)
			|| !pTerrain->GetFacesOfArea(trace.vHitPos, nx, nz, pVerts, pIndices,iNumVert, iNumIndex))
		{
			_nVertCount = 0;
			_nIndexCount = 0;
		}
		else
		{
			pVerts += _nVertCount;
			pIndices += _nIndexCount;
		}
	}

	ECTriangleMan* pTriangleMan = m_render.GetECGraphicsModule()->GetTriangleMan();

	if (pTriangleMan)
	{
		TMan_Overlap_Info info;
		info.vStart = vCenter;
		info.vStart.y += 1.0f;
		info.vExtent = A3DVECTOR3(fRadus, 4.0f, fRadus);
		info.vDelta = A3DVECTOR3(0.0f, -4.0f, 0.0f);
		info.pCamera = m_render.GetCamera();
		info.bCheckNormal = true;
		pTriangleMan->TestOverlap(info);

		nTriVertCount = (int)info.pVert.size();
		nTriIndexCount = (int)info.pIndex.size();

		if (nTriVertCount && nTriIndexCount && nTriVertCount + _nVertCount <= nVertCount && nTriIndexCount + _nIndexCount <= nIndexCount)
		{
			for (i = 0; i < nTriVertCount; i++)
				pVerts[i] = info.pVert[i];

			for (i = 0; i < nTriIndexCount; i++)
				pIndices[i] = _nVertCount + info.pIndex[i];
		}
		else
		{
			nTriVertCount = 0;
			nTriIndexCount = 0;
		}
	}

	nVertCount = nTriVertCount + _nVertCount;
	nIndexCount = nTriIndexCount + _nIndexCount;
	return true;
}

bool IGfx::PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const
{
	A3DSkillGfxComposerMan* pMan = GetSkillGfxComposerMan();
	pMan->Play(strAtkFile, SerialID, nCasterID, nCastTargetID, pFixedPoint);

	A3DSkillGfxEventMan* pEventMan = GetSkillGfxEventMan(); 
	A3DSkillGfxEvent* pEvent = pEventMan->GetSkillGfxEvent(nCasterID, SerialID);
	if (!pEvent)
		return true;

	pEvent = pEvent->GetLast();
	pEvent->SetDivisions(nDivisions);

	TARGET_DATA td;
	td.idTarget = 2;
	td.nDamage = 100;
	td.dwModifier = 0;
	pEvent->AddOneTarget(td);
	return true;
}

void IGfx::SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier) const
{
	// virtual function
	assert(!"Oops! Not implementation!");
}

void IGfx::SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const
{
	// empty implement. This function was called after skill attack action
}

bool IGfx::SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE* shake) const
{
	assert(!"Oops! Not implementation!");
	return 0;
}
#endif
