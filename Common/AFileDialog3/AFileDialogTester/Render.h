/*
 * FILE: Render.h
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2004/4/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_RENDER_H_
#define _RENDER_H_

#include "AChar.h"
#include <A3DTypes.h>

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
class A3DLight;	
class A3DFont;
class A3DStringSet;
class A3DCamera;
class A3DViewport;
class A3DRenderTarget;
class A3DPixelShader;
class A3DStream;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CRender
//
///////////////////////////////////////////////////////////////////////////

class CRender
{
public:		//	Types

public:		//	Constructor and Destructor

	CRender();
	virtual ~CRender() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd);
	//	Release game
	void Release();

	//	Resize device
	bool ResizeDevice(int iWid, int iHei);
	//	Begin render
	bool BeginRender(bool bDoWarps);
	//	End render
	bool EndRender(bool bDoWarps);
	//	Do warps
	bool DoWarps();

	//	Output text
	void TextOut(int x, int y, const ACHAR* szText, DWORD color);

	//	Draw 2D rectangle
	void Draw2DRect(const A3DRECT & rc, DWORD dwCol, bool bFrame);

	//	Get interface
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() {	return m_pA3DDevice; }
	A3DCamera* GetA3DCamera() { return m_pA3DCamera; }
	A3DViewport* GetA3DViewport() { return m_pA3DViewport; }
	A3DLight* GetDirLight() { return m_pDirLight; }
	A3DRenderTarget * GetFrameTarget() { return m_pFrameTarget; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;		//	A3D engine
	A3DDevice*		m_pA3DDevice;		//	A3D device
	A3DCamera*		m_pA3DCamera;		//	A3D camera
	A3DViewport*	m_pA3DViewport;		//	A3D viewport
	A3DRenderTarget*m_pFrameTarget;		//	A3D frame target which may be used as texture
	A3DLight*		m_pDirLight;		//	Directional light
	A3DLight*		m_pDynamicLight;
	A3DFont*		m_pFont;			//	Font object

	HINSTANCE		m_hInstance;		//	Handle of instance
	HWND			m_hRenderWnd;		//	Render window's handle
	int				m_iRenderWid;		//	Render size
	int				m_iRenderHei;

	// space warp objects.
	A3DPixelShader *		m_pTLWarpShader;		// TL warp render pixel shader
	A3DStream *				m_pTLWarpStream;		// stream used to show space warps using TL verts
	int						m_nMaxTLWarpVerts;		// max verts number of TL warp stream
	int						m_nMaxTLWarpIndices;	// max indices number of TL warp stream
	A3DPixelShader *		m_pWarpShader;
	A3DStream *				m_pWarpStream;
	int						m_nMaxWarpVerts;
	int						m_nMaxWarpIndices;
	bool			m_bEngineOK;		//	Engine is ready

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
	//	Create frame target
	bool CreateFrameTarget();
};

extern CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

#endif	//	_RENDER_H_
