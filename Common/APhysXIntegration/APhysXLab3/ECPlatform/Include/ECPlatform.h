/*
 * FILE: ECPlatform.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECPLATFORM_H_
#define _ECPLATFORM_H_

#include <Windows.h>
#include <ABaseDef.h>
#include <AExpDecl.h>
#include <AList2.h>
#include <A3DVector.h>
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//#define PLATFORM_GRAPHICSMODULE		0x00000001
//#define PLATFORM_SCRIPTMODULE		0x00000002
//#define PLATFORM_INPUTMODULE		0x00000004
///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class ECGraphicsModule;
class ECScriptModule;
class ECSceneModule;
class ECInputModule;
class ECAudioModule;

class ECPlatformSettings;
class A3DEngine;
class A3DDevice;
class A3DGfxInterface;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECLoadProgressHandler		Can listen resource load step
//	
///////////////////////////////////////////////////////////////////////////
class ECLoadProgressHandler
{
public:
	//	Step load progress
	virtual void StepLoadProgress(int iStep) = 0;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECPlatformInit
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECPlatformInit
{
public:		//	Types

public:		//	Constructor and Destructor

	ECPlatformInit();

public:		//	Attributes

	HINSTANCE	m_hInstance;	//	App instance handle
	HWND		m_hWnd;			//	Device window handle

	//DWORD		m_dwModuleFlag;	//	Which module will be created from this flag

	int			m_iScreenW;		//	The width of screen
	int			m_iScreenH;		//	The Height of screen
	bool		m_bFullScreen;	//	Full screen flag
	bool		m_bVSync;		//	Vertical sync flag

	bool		m_bEnableDebug;	//	Enable script debug

	A3DGfxInterface* m_pGfxInterface;	//	GFX custom interface
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECPlatform
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECPlatform
{
public:		//	Types

	

public:		//	Constructor and Destructor

	ECPlatform();
	virtual ~ECPlatform();

public:		//	Attributes

public:		//	Operations

	//	Initialize environment. This function should be static because af_Initlize should be called first in client.
	static bool InitEnv(const char* szBaseDir=NULL);
	//	Initialize platform modules
	bool InitModules(const ECPlatformInit& init);
	//	Release platform
	void Release();
	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Get interfaces
	A3DEngine* GetA3DEngine();
	A3DDevice* GetA3DDevice();

	// Create scene module
	ECSceneModule* CreateSceneModule();
	// Release a scene module
	void ReleaseSceneModule(ECSceneModule*);

	//	Get app instance handle
	HINSTANCE GetAppInst() const { return m_hInstance; }
	//	Get device window handle
	HWND GetWnd() const { return m_hWnd; }

	// Get platform implement
	static  ECPlatform*	GetPlatform() { return m_pPlatformImp;}	
	//	Create ECPlatform implement
	static  bool CreatePlatform();
	//	Destroy ECPlatform implement
	static  void DestroyPlatform();

	//	Get modules
	ECPlatformSettings* GetSettings(){ return m_pSettings;}
	ECGraphicsModule* GetGraphicsModule() { return m_pGraphics; }
	ECScriptModule* GetScriptModule() { return m_pScriptModule; }
	ECInputModule* GetInputModule() { return m_pInputModule; }
	ECAudioModule* GetAudioModule() { return m_pAudioModule; }

	//	Step load progress
	void LoadProgressStep();
	void LoadProgressStepComp(float fStepComp);
	void SetLoadProgressHandler(ECLoadProgressHandler* pHander)	{ m_pLoadProgressHandler = pHander; }

protected:	//	Attributes

	HINSTANCE	m_hInstance;	//	Instance handle
	HWND		m_hWnd;			//	Device window handle

	static  ECPlatform*	m_pPlatformImp;		// Platform implement

	ECPlatformSettings* m_pSettings;		//	Platform setting
	ECGraphicsModule*	m_pGraphics;		//	Graphics module	
	ECScriptModule*		m_pScriptModule;	//	Script module
	ECInputModule*		m_pInputModule;		//	Input module
	ECAudioModule*		m_pAudioModule;		//	Audio module

	ECLoadProgressHandler* m_pLoadProgressHandler;
	float m_fStepComp;
	float m_fProgCnt;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Global variables
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_ECPLATFORM_H_
