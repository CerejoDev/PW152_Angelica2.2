/*
* FILE: AResManager.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include <windows.h>
#include <windowsx.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DMacros.h>
#include "AResManager.h"
#include "AResFactory.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

AResManager* g_pResMan;
IResLog* g_pResLog;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	AResManager is a singleton object, call CreateResMan multiple times will only the first object created
//	Create one AResManager object, which creates and manages all resources
//	Pass one pA3DDevice pointer for AResManager, then AResManager will use this engine
//	Pass NULL will make AResManager create one A3DEngine for use
AResManager* AResManager::CreateResMan(IResInterface* pResEngineProxy)
{
	if (g_pResMan)
		return g_pResMan;

	g_pResMan = new AResManager;
	switch (g_pResMan->Init(pResEngineProxy))
	{
	case INIT_OK:
		g_pResMan->SetResFactory(MakeSharedPtr<AResFactory>(g_pResMan));
		return g_pResMan;
	case INIT_ALREADYINITED:
		return g_pResMan;
	case INIT_FAILED:
	default:
		return NULL;
	}
}

void AResManager::ReleaseResMan()
{
	if (!g_pResMan)
		return;

	g_pResMan->Release();
	delete g_pResMan;
	g_pResMan = NULL;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResManager
//	
///////////////////////////////////////////////////////////////////////////

AResManager::AResManager()
: m_pResEngineProxy(NULL)
, m_bInited(false)
{
	//CreateErrLog();
}

AResManager::~AResManager(void)
{
	Release();
	//ReleaseErrLog();
}

AResManager::INITSTATUS AResManager::Init(IResInterface* pResEngineProxy)
{
	if (m_bInited)
		return INIT_ALREADYINITED;

	if (!pResEngineProxy)
		return INIT_FAILED;

	g_pResLog = pResEngineProxy->GetResLog();
	ASSERT( g_pResLog );
	if (!g_pResLog)
		return INIT_FAILED;

	AResInitialize();
	m_pResEngineProxy = pResEngineProxy;
	return INIT_OK;
}

/*
AResManager::INITSTATUS AResManager::Init(const AResInit& ResInit)
{
	if (m_bInited)
		return INIT_ALREADYINITED;

	if (!ResInit.m_pA3DEngine || !ResInit.m_pA3DGfxEngine)
		m_bManagedEngine = true;

	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	//	Use reference engine
	if (!m_bManagedEngine)
	{
		m_pA3DEngine = ResInit.m_pA3DEngine;
		m_pA3DGfxEngine = ResInit.m_pA3DGfxEngine;

		//	Use dll libaray mode with Angelica linked as a static library means we must 
		//	Set angelica global variables independently 
		//	(because we can not share the exe's angelica's global variable)
		af_SetBaseDir(szCurDir);

		if (!CreateSptForest(m_pA3DEngine))
		{
			g_pResLog->Log("%s, Failed to create spt forest.", __FUNCTION__);
			return INIT_FAILED;
		}

		m_bInited = true;
		return INIT_OK;
	}
	else
	{
		// use self managed engine
		af_Initialize();
		af_SetBaseDir(szCurDir);

		if (!CreateEngine())
		{
			g_pResLog->Log("%s, Failed to create A3DEngine", __FUNCTION__);
			return INIT_FAILED;
		}

		m_pA3DDeivce = m_pA3DEngine->GetA3DDevice();
		if (!CreateA3DGfxEngine(m_pA3DEngine))
		{
			g_pResLog->Log("%s, Failed to create A3DGfxEngine", __FUNCTION__);
			return INIT_FAILED;
		}

		if (!CreateSptForest(m_pA3DEngine))
		{
			g_pResLog->Log("%s, Failed to create spt forest.", __FUNCTION__);
			return INIT_FAILED;
		}

		m_bInited = true;
		return INIT_OK;
	}
}
*/

void AResManager::Release()
{
	if (m_pResFactory.get())
		m_pResFactory->Release();
/*
	ReleaseSptForest();

	if (m_bManagedEngine)
	{
		ReleaeA3DGfxEngine();
		ReleaseEngine();
	}
	else
	{
		m_pA3DEngine = NULL;
		m_pA3DDeivce = NULL;
		m_pA3DGfxEngine = NULL;
	}
*/

	m_pResEngineProxy = NULL;
	m_bInited = false;
}

void AResManager::SetResFactory(const AResFactroyPtr& pResFactory)
{
	m_pResFactory = pResFactory;
}

//	Load a resource from relative path under Angelica base directory
//	Return ARes::NullObject() if file not exist, or file could not be loaded...
AResPtr AResManager::LoadRes(const char* szPath, bool bRequireExist)
{	
	return m_pResFactory->MakeResource(szPath, bRequireExist);
}
/*
void AResManager::CreateErrLog()
{
	g_pResLog->Init("ARes.log", "Angelica Res Tool Log");
}

void AResManager::ReleaseErrLog()
{
	g_pResLog->Release();
}
*/
/*
bool AResManager::CreateEngine()
{
	g_pResLog->Log("No engine passed in, create ARes's own engine.");

	const int iRenderWid = 640;
	const int iRenderHei = 480;
	HWND hWnd = _CreateHiddenWnd(_GetModuleInstance(), iRenderWid, iRenderHei);
	if (!hWnd)
	{
		g_pResLog->Log("%s, Failed to create hidden window.", __FUNCTION__);
		return false;
	}

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	A3DDEVFMT devFmt;
	memset(&devFmt, 0, sizeof(devFmt));
	devFmt.bWindowed	= true;
	devFmt.nWidth		= rcClient.right;
	devFmt.nHeight		= rcClient.bottom;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	m_pA3DEngine = new A3DEngine();
	if (!m_pA3DEngine->Init(_GetModuleInstance(), hWnd, &devFmt, A3DDEV_ALLOWMULTITHREAD))
	{
		g_pResLog->Log("%s, Failed to initialize A3DEngine", __FUNCTION__);
		return false;
	}

	m_pA3DDeivce = m_pA3DEngine->GetA3DDevice();
	return true;
}

void AResManager::ReleaseEngine()
{
	A3DRELEASE( m_pA3DEngine );
	
	m_pA3DDeivce = NULL;
}

bool AResManager::CreateA3DGfxEngine(A3DEngine* pA3DEngine)
{
	g_pResLog->Log("No Gfx engine passed in, create ARes's own Gfx engine.");

	ASSERT( !m_pA3DGfxEngine && !m_pGfxInterface );
	ASSERT( pA3DEngine );

	g_LuaStateMan.Init();


	m_pGfxPhysShell = new GFXPhysXSceneShell();
	m_pA3DGfxEngine = new A3DGfxEngine();
	m_pGfxInterface = new AResGfxInterface();
	if (!m_pA3DGfxEngine->Init(pA3DEngine, m_pGfxInterface))
	{
		g_pResLog->Log("%s, Failed to init A3DGfxEngine.", __FUNCTION__);
		return false;
	}

	if (!m_pGfxInterface->Init(m_pA3DGfxEngine))
	{
		g_pResLog->Log("%s, Failed to initialize A3DGfxInterface", __FUNCTION__);
		return false;
	}
		
	m_pA3DGfxExMan = m_pA3DGfxEngine->GetA3DGfxExMan();
	return true;
}

void AResManager::ReleaeA3DGfxEngine()
{
	A3DRELEASE(m_pGfxInterface);
	A3DRELEASE(m_pA3DGfxEngine);
	m_pA3DGfxExMan = NULL;
	delete m_pGfxPhysShell;
	g_LuaStateMan.Release();
}

bool AResManager::CreateSptForest(A3DEngine* pA3DEngine)
{
	ARectF rcFake(-512.0f, 512.0f, 512.0f, -512.0f);
	m_pSptForest = new ECTreeForest();
	if (!m_pSptForest->Init(pA3DEngine, rcFake, 128.0f, "Trees\\SpeedWind.ini"))
	{
		return false;
	}

	m_pSptForest->SetLODDist(40.f, 250.f);
	return true;
}

void AResManager::ReleaseSptForest()
{
	A3DRELEASE(m_pSptForest);
}
*/

/*
//	Simple Wrapper Function
A3DGFXEx* AResManager::LoadGfx(const char* szPath)
{
	if (!m_pResEngineProxy)
	{
		g_pResLog->Log("%s, Invalid state! no EngineProxy instance.", __FUNCTION__);
		return NULL;
	}
	
	ASSERT( m_pGfxPhysShell );

	return m_pA3DGfxExMan->LoadGfx(szPath, m_pGfxPhysShell);
}

void AResManager::ReleaseGfx(A3DGFXEx* pGfx)
{
	if (!m_pResEngineProxy)
	{
		g_pResLog->Log("%s, Invalid state! no EngineProxy instance.", __FUNCTION__);
		return;
	}

	m_pA3DGfxExMan->CacheReleasedGfx(pGfx);
}
*/