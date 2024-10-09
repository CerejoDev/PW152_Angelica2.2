/*
 * FILE: ECGraphicsModule.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DTypes.h>
#include <AExpDecl.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;
class A3DGFXExMan;
class ECGFXCaster;
class ECImageRes;
class A3DGfxEngine;
class A3DGfxInterface;
class A3DViewport;
class A3DRenderTarget;
class A3DMtlBlur;
class A3DSimpleQuad;
class A3DLPPRender;
class A3DPostEffectGameImp;
class A3DGFXEx;
class ECTriangleMan;
class ECSimpleBloomRender;
class ECLPPLightMan;

struct GLOW_TYPE_PARAM;

enum FULLGLOW_TYPE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

typedef void (*LPRENDERFORBLOOM)(A3DViewport * pViewport, void * pArg);

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECGraphicsModule
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECGraphicsModule
{
public:		//	Types

	friend class ECPlatform;

protected:	//	Types


public:	//	Constructor and Destructor

	ECGraphicsModule();
	virtual ~ECGraphicsModule();

public:		//	Attributes

public:		//	Operations

	//	Begin render
	bool BeginRender();
	//	Do post effects
	bool DoPostEffects(A3DViewport* pViewport);
	//	End render
	bool EndRender(bool bPresent=true);
	//	Present
	bool Present();

	//	Change display mode
	bool ChangeDisplayMode(int iWidth, int iHeight, int iFullScreen, int iVSync=-1);
	//	Check if device is in fullscreen state
	bool IsFullScreen();

	//	Prepare occlusion query system, this function should be called at the beginning of rendering
	void PrepareOccluderQuery(A3DViewport* pViewport);
	//	Build occlusion query states, this function should be called each frame after all opaque objects
	//	have been rendered and z buffer has been established
	void BuildOccluderQueryStates(A3DViewport* pViewport);

	//	Set sharpen texture flag
	void SetSharpenTexFlag(bool bEnable);
	bool GetSharpenTexFlag() const { return m_bSharpenTex; }

	//	Get render time
	DWORD GetRenderTime() { return m_dwRenderTime; }
	DWORD GetPresentTime() { return m_dwPresentTime; }
	DWORD GetTickTime() { return m_dwTickTime; }

	//	Get interfaces
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }
	A3DGfxEngine* GetGFXEngine() { return m_pA3DGfxEngine; }
	A3DLPPRender* GetLPPRender() { return m_pLPPRender; }
	ECLPPLightMan* GetECLPPLightMan() { return m_pECLPPLightMan; }
	A3DGFXExMan* GetA3DGfxExMan() { return m_pA3DGfxExMan; }
	A3DPostEffectGameImp* GetPostEffectMan() { return m_pPostEffectMan; }
	//	Get frame buffer
	A3DRenderTarget* GetFrameBuffer();

	ECTriangleMan* GetTriangleMan() { return m_pTriangleMan; }	//	Triangle manager for shadow render
	void SetTriangleMan(ECTriangleMan* pTriangleMan){	m_pTriangleMan = pTriangleMan;	}

	//------------------ GFX Caster Interface ------------------------------------------------
	//	Play a automatical GFX
	bool PlayAutoGFXEx(const char* szFile, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, float fScale=0.0f);
	//	Load normal gfx
	A3DGFXEx* LoadGFXEx(const char* szFile);
	//	Release normal gfx
	void ReleaseGFXEx(A3DGFXEx* pGFX, bool bCacheRelease = true);
	//	Play a normal gfx
	bool PlayGFXEx(A3DGFXEx* pGFX, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, float fScale);
	bool PlayGFXEx(A3DGFXEx* pGFX, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, float fScale);
	//	Register all gfx and models hold by GFX caster
	void RegisterAllGfx(A3DViewport* pViewport);
	//	Actually render all registered gfx
	void RenderAllGfx(A3DViewport* pViewport, bool bOnlyRegistered = false, A3DLPPRender* pLPPRender = NULL);

	//	Get device window
	HWND GetDeviceWnd() const { return m_hDevWnd; }

protected:	//	Attributes

	HWND					m_hDevWnd;			//	Device window handle
	ECPlatform*				m_pECPlatform;		//	Platform object
	A3DGfxEngine*			m_pA3DGfxEngine;	//	A3D Graphic FX engine
	A3DEngine*				m_pA3DEngine;		//	A3DEngine object
	A3DDevice*				m_pA3DDevice;		//	A3DDevice object

	A3DGFXExMan*			m_pA3DGfxExMan;		//	GFX object manager
	ECGFXCaster*			m_pGFXCaster;		//	GFX caster
	A3DGfxInterface*		m_pA3DGfxInterface;	//	GFX interface

	DWORD					m_dwRenderTime;		//	Last render time between BeginRender and EndRender
	DWORD					m_dwPresentTime;	//	Last present time
	DWORD					m_dwTickTime;		//	Last tick time

	ECTriangleMan*			m_pTriangleMan;		//	Triangle manager for shadow render

	A3DLPPRender*			m_pLPPRender;		//	LPP render
	ECLPPLightMan*			m_pECLPPLightMan;	//	LPP light manager
	A3DPostEffectGameImp*	m_pPostEffectMan;	//	Post effect manager

	bool					m_bSharpenTex;		//	true, sharpen texture

protected:	//	Operations

	//	Initialize object
	bool Init(ECPlatform* pECPlatform, HWND hDevWnd, A3DGfxInterface* pGfxInterface);
	//	Release object
	void Release();
	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D engine
	void ReleaseA3DEngine();
	//	Initialize A3D Graphic FX engine
	bool InitA3DGfxEngine(A3DGfxInterface* pGfxInterface);
	//	Release GFX engine
	void ReleaseGFXEngine();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


