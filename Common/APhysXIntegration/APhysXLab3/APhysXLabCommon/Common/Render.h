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

#include <FlagMgr.h>
#include "A3DGfxInterface.h"
#include "RenderInterface.h"

class A3DVECTOR3;
class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DLight;
class A3DLPPLightDir;
class A3DGfxEngine;
class IGfx;
class A3DFont;
class A3DCursor;
class A3DSky;
class A3DTerrain2;
class A3DSkinModel;
class APhysXTerrain;
class ECWorldRender;
class ECPlatform;
class ECGraphicsModule;
class ECSceneModule;
class ApxAttackSkillGfxMan;

class CRender : public IRenderUtility, public IRenderManager
{
public:
	CRender();
	~CRender();

	bool IsReady() const { return m_flags.ReadFlag(RENDER_INIT_ENDOK); }
	bool Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen);
	void Release();

	bool ResizeDevice(const int iWid, const int iHei) const;
	bool BeginRender() const;
	bool EndRender();

	void DrawBackground() const;
	void TextOut(int x, int y, const ACHAR* szText, int iLen, DWORD color) const;

	void EnableDrawCameraDir(const bool bEnable = true) { bEnable? m_flags.RaiseFlag(RENDER_DRAW_CAMERA_DIR) : m_flags.ClearFlag(RENDER_DRAW_CAMERA_DIR); }
	void DrawPhysXTerrainRange(const A3DVECTOR3& center, APhysXTerrain* pPtrn = 0, A3DTerrain2* pAtrn = 0) { m_trnCenter = center; m_pPhysXTrn = pPtrn; m_pA3DTrn = pAtrn; };
/*	void TextOutAnimation(const int x, const int y, const ACHAR* szText, const DWORD color, const unsigned int nMaxDrawMilliSec = 1000);

	void SetFrontSight(const int xCenter, const int yCenter, const int HalfSideLen, const DWORD color);
	bool GetFrontSight(int& xCenter, int& yCenter, int& HalfSideLen, DWORD* pColor = 0) const;
	void ClearFrontSight() { m_FrontSight.bIsEnable = false; }; 
	
	void DrawForceBar(const int xCenter, const int yCenter, const DWORD color, const bool StartHoldingForce = false);
	float GetForcePercent() const { return m_ForceSize * 1.0f / gForceBarLen; }
	void ClearForceBar() { m_ForceBar.bIsEnable = false; m_ForceSize = 0; }; 
*/
	void DrawProgressBar(float percent, int xCenterPos, int yCenterPos, const DWORD color, int width, int height);
	void ClearProgressBar() { m_LoadingBar.bIsEnable = false; } 
	int GetXCenter() const { return m_xCenter; }
	int GetYCenter() const { return m_yCenter; }

	void SetTerrainWater(A3DTerrainWater& trnWater) { m_pA3DTrnWater = &trnWater; }
	void SetAttackSkillGfxMan(ApxAttackSkillGfxMan* pAASGM);
	ApxAttackSkillGfxMan* GetAttackSkillGfxMan() const { return m_pAASGM; }

	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	A3DCamera* GetCamera() const { return m_pCamera; }
	A3DViewport* GetViewport() const { return m_pViewport; }
	A3DWireCollector* GetWCTopZBUf() { return &m_WCTopZbuf; }
	A3DWireCollector* GetWireCollector() const { return m_pA3DEngine->GetA3DWireCollector(); }
	A3DFlatCollector* GetFlatCollector() const { return m_pA3DEngine->GetA3DFlatCollector(); }
	ECWorldRender*	GetWorldRenderer() const { return	m_pWorldRender; }
#ifdef _ANGELICA3
	A3DGfxEngine* GetGFXEngine() const { return m_pGfxEngine; }
	ECGraphicsModule* GetECGraphicsModule() const { return m_pECGraphic; }

	void BuildShadowCasters(APtrArray<A3DShadowCaster*>& aCasters);
	void RenderShadowMap(A3DViewport* pViewport, const APtrArray<A3DShadowCaster*>& aCasters, int iSlice);
#else
	void RenderOrnaments(bool bUpdateShadowMap, int nRenderMode);
	bool RenderForShadowMap(A3DViewport* pViewport);
#endif

	bool OnSetCursor() const;
	bool ShowCursor(A3DCursor* pA3DCursor = 0) const;
	bool SetLightInfo(A3DSkinModel& skinModel) const;
/*	//bool SetSky(A3DSky& sky) const;
	void SetupCamera(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) const;
*/
	A3DLight* GetDirectionalLight() const { return m_pDirLight; }

	// render all the renderable stuff registered, called by the game's Render()function...
	void RenderStuff();

	// Note: we can only register one Terrain!
	void RegisterTerrain(A3DTerrain2* pTerrain);
	void UnregesiterTerrain();
	A3DTerrain2* GetTerrain() const { return m_pTerrain;}

	// Note: we can only register one sky!
	void RegisterSky(A3DSky* pSky) { m_pSky = pSky; }
	void UnregisterSky() { m_pSky = NULL; }
	A3DSky* GetSky() const { return m_pSky; }
	void RenderLPP(A3DViewport* pViewport) const { Render(pViewport); }
	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck = true) const;

private:
	bool InitLight();
	bool InitCamera(float fRatio);
	bool InitViewport(const RECT& Rect);
	void ClearViewport() const;

//	void DrawTextAnimation();
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
	
/*	struct TextNode
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
*/
	enum FlagInfo
	{
		RENDER_INIT_START		= (1<<0),
		RENDER_INIT_ENDOK		= (1<<1),
		RENDER_DRAW_CAMERA_DIR	= (1<<2),
	};

private:
	static const unsigned int gForceBarLen;

	CFlagMgr<FlagInfo> m_flags;

	A3DEngine*		m_pA3DEngine;
	A3DDevice*		m_pA3DDevice;
	A3DCamera*		m_pCamera;
	A3DViewport*	m_pViewport;
	A3DFont*		m_pSysFont;

#ifdef _ANGELICA3
	A3DEnvironment* m_pA3DEnvmnt;
	A3DLPPLightDir* m_pLPPDirLight;
	A3DGfxEngine*	m_pGfxEngine;
	IGfx*			m_pIEditorGfx;

	ECPlatform*		m_pECPlatform;
	ECGraphicsModule* m_pECGraphic;
#else
	A3DSkinRender*  m_psr1;
#endif

	A3DLight*		m_pDirLight;
	A3DWireCollector m_WCTopZbuf;

	ApxAttackSkillGfxMan* m_pAASGM;

/*	DWORD			m_HoldTime;
	unsigned int	m_ForceSize; 
	Rect2D			m_ForceBar;
	Rect2D			m_FrontSight;
*/
	float			m_fLoadPercent;
	Rect2D			m_LoadingBar;

	mutable int     m_xCenter;
	mutable int  	m_yCenter;

// std::list<TextNode*> m_lstText;

	APhysXTerrain*  m_pPhysXTrn;
	A3DTerrain2*	m_pA3DTrn;
	A3DVECTOR3		m_trnCenter;
	A3DTerrainWater*	m_pA3DTrnWater;

	A3DTerrain2*	m_pTerrain;
	A3DSky*			m_pSky;

	// real render implementation...
	ECWorldRender*		m_pWorldRender;
};

#ifdef _ANGELICA3
class IGfx : public A3DGfxInterface
{
public:	
	IGfx(CRender& render);
	virtual ~IGfx();

	void SetAttackSkillGfxMan(ApxAttackSkillGfxMan* pAASGM);
	virtual A3DSkillGfxEventMan* GetSkillGfxEventMan() const;
	virtual A3DSkillGfxComposerMan* GetSkillGfxComposerMan() const;

	virtual A3DSkinModel* LoadA3DSkinModel(const char* szFile, int iSkinFlag) const;
	virtual void ReleaseA3DSkinModel(A3DSkinModel* pModel) const;

	virtual bool LoadECMHullData(const char* szChfFile, ECMHullDataLoad* pECMHullData) const;
	virtual const char * GetECMHullPath() const;

	virtual bool GetModelUpdateFlag() const;
	virtual bool GetModelLight(const A3DVECTOR3& vPos, A3DSkinModelLight& light) const;
	virtual void ECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel) const;
	virtual float GetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm) const;
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel) const;

	virtual AM3DSoundBuffer* LoadNonLoopSound(const char* szFile, int nPriority) const;
	virtual AM3DSoundBuffer* LoadLoopSound(const char* szFile) const;
	virtual void ReleaseSoundNonLoop(AM3DSoundBuffer*& pSound) const;
	virtual void ReleaseSoundLoop(AM3DSoundBuffer*& pSound) const;

	virtual A3DCameraBase* GetA3DCamera() const;

	virtual float GetAverageFrameRate() const;
	virtual bool GetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount) const;

	virtual bool PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const;
	virtual void SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier) const;
	virtual void SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const;
	virtual bool SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE* shake) const;

	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath) const {return NULL;}
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const{ }

private:
	mutable bool m_Inited;
	CRender& m_render;
	A3DSkillGfxComposerMan* m_pSGCM;
};
#endif

#endif