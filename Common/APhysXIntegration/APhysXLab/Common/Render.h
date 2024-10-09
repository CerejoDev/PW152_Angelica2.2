/*
 * FILE: Render.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_RENDER_H_
#define _APHYSXLAB_RENDER_H_

#include <list>
#include <A3DWireCollector.h>

class A3DVECTOR3;
class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DLight;
class A3DFont;
class A3DCursor;
class A3DSky;
class A3DTerrain2;
class A3DSkinModel;
class APhysXTerrain;

class CRender
{
public:
	CRender();
	~CRender();

	bool EngineIsReady() const { return m_bInitedEndOK; }
	bool Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen);
	void Release();

	bool ResizeDevice(const int iWid, const int iHei) const;
	bool BeginRender() const;
	bool EndRender();
	void ClearViewport() const;

	void EnableDrawCameraDir(const bool bEnable = true) { m_bDrawCameraDir = bEnable; };
	void DrawPhysXTerrainRange(const A3DVECTOR3& center, APhysXTerrain* pPtrn = 0, A3DTerrain2* pAtrn = 0) { m_trnCenter = center; m_pPhysXTrn = pPtrn; m_pA3DTrn = pAtrn; };
	void TextOut(int x, int y, const ACHAR* szText, DWORD color) const;
	void TextOutAnimation(const int x, const int y, const ACHAR* szText, const DWORD color, const unsigned int nMaxDrawMilliSec = 1000);

	void SetFrontSight(const int xCenter, const int yCenter, const int HalfSideLen, const DWORD color);
	bool GetFrontSight(int& xCenter, int& yCenter, int& HalfSideLen, DWORD* pColor = 0) const;
	void ClearFrontSight() { m_FrontSight.bIsEnable = false; }; 
	
	void DrawForceBar(const int xCenter, const int yCenter, const DWORD color, const bool StartHoldingForce = false);
	float GetForcePercent() const { return m_ForceSize * 1.0f / gForceBarLen; }
	void ClearForceBar() { m_ForceBar.bIsEnable = false; m_ForceSize = 0; }; 

	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	A3DCamera* GetCamera() const { return m_pCamera; }
	A3DViewport* GetViewport() const { return m_pViewport; }
	A3DWireCollector* GetWCTopZBUf() { return &m_WCTopZbuf; }

	bool OnSetCursor() const;
	bool ShowCursor(A3DCursor* pA3DCursor = 0) const;
	bool SetSky(A3DSky& sky) const;
	bool SetLightInfo(A3DSkinModel& skinModel) const;
	void SetupCamera(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) const;

	A3DLight* GetDirectionalLight() { return m_pDirLight; }
private:
	bool InitA3DEngine(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen, RECT& outRect);
	bool InitLight();
	bool InitCamera();
	bool InitViewport(const RECT& Rect);

	void DrawTextAnimation();
	void DrawPhysXTerrainRange();

private:
	// forbidden behavior
	CRender(const CRender& rhs);
	CRender& operator= (const CRender& rhs);
	
	struct Rect2D
	{
		Rect2D() : bIsEnable(false) {}
		
		bool bIsEnable;
		int l;
		int t;
		int r;
		int b;
		DWORD color;
	};
	
	struct TextNode
	{
		TextNode() : nStartCount(0), nMaxCount(0), CompletePercent(0) {}
		bool IsValid(const int nowCount);

		int xPos;
		int yPos;
		int nStartCount;
		int nMaxCount;
		float CompletePercent;
		DWORD color;
		ACString strText;
	};

#if _MSC_VER >= 1400   //  VC2005 is 1400
	struct IsGoToDie : public std::unary_function<TextNode*, bool> 
	{
		IsGoToDie(const int nowCount) : NowCount(nowCount) {}

		bool operator() (TextNode* val) const
		{
			if ((0 != val) && val->IsValid(NowCount))
				return false;

			delete val;
			return true;
		}

		const int NowCount;
	};
#endif

	template <class T>
	struct DeleteIt : public std::unary_function<T*, void>
	{
		void operator() (T* pVal) const { delete pVal;}
	};

private:
	static const unsigned int gForceBarLen;

	bool			m_bInitedStart;
	bool			m_bInitedEndOK;

	bool            m_bDrawCameraDir;
	A3DEngine*		m_pA3DEngine;
	A3DDevice*		m_pA3DDevice;
	A3DCamera*		m_pCamera;
	A3DViewport*	m_pViewport;
	A3DLight*		m_pDirLight;
	A3DFont*		m_pSysFont;
	A3DWireCollector m_WCTopZbuf;

	DWORD			m_HoldTime;
	unsigned int	m_ForceSize; 
	Rect2D			m_ForceBar;
	Rect2D			m_FrontSight;

	mutable int     m_xCenter;
	mutable int  	m_yCenter;

	std::list<TextNode*> m_lstText;

	APhysXTerrain*  m_pPhysXTrn;
	A3DTerrain2*	m_pA3DTrn;
	A3DVECTOR3		m_trnCenter;
};

extern CRender g_Render;

#endif