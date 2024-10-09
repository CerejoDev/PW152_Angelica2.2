/*
* FILE: ECSceneModule.h
*
* DESCRIPTION: 
*
* CREATED BY: Yongdong Wang, 2009/9/1
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#pragma once 

#include <A3DViewport.h>
#include <hashtab.h>
#include <A3DTerrainWaterRender.h>
#include <AArray.h>
#include <A3DGrassLand.h>
#include "ECTerrain.h"

class ECPlatform;
class ECTerrain;
class ECScene;
class ECResLoadModule;
class ECSceneObjectLoader;
class ECSceneObject;
class ECResLoader;
class ECDungeon;
class ECCameraPosState;
class ECTriangleMan;
class ECTerrainWater;
class A3DTerrainWater;
class GFXPhysXSceneShell;
class A3DTerrain2;
class A3DTerrainDecalCallback;
class A3DGrassLandCallback;
class A3DGrassLand;
class A3DGrassLoader;
class A3DLPPRender;
class ECSceneRenderer;
class ECShadowRender;
class A3DGrassInteractor;
///////////////////////////////////////////////////////////////////////////
//	
//	ECSceneModule class
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECSceneModule
{
	friend class ECScene;
	typedef abase::hashtab<ECSceneObjectLoader*,int,abase::_hash_function> ObjectLoaderTable;

public:
	struct SCENEMODULE_PARAM 
	{
		SCENEMODULE_PARAM()
		{
			bMultiThreadLoad = false;
			fViewRadius = 100.0f;
			pUserResLoadFunc = NULL;
			szMapName = "";
			szDetailedTex = "";
			iWaterRenderLevel = 1;
			iTrnRenderLevel = 1;
			fGrassLODLevel = 1.0f;
			pGFXPhysXSceneShell = NULL;
		}
		bool bMultiThreadLoad; //  load resource with multi-thread.
		float fViewRadius; // terrain view radius
		const char* szMapName; // map name coming from ECInstance
		const char* szDetailedTex; // detail texture for outline
		void* pUserResLoadFunc; // user resource loader callback
		int iWaterRenderLevel; // water render level [1, 3]
		int iTrnRenderLevel;	//	Terrain render level [1, 2]
		float fGrassLODLevel;	//	Grass lod level [0.0f, 1.0f]
		GFXPhysXSceneShell* pGFXPhysXSceneShell;
	};
	struct SCENEMODULE_INNERPARAM
	{
		bool bMultiThreadLoad;
		AString strMapName;
	};
public:
	ECSceneModule(ECPlatform* platform);
	virtual ~ECSceneModule();

	void RegisterUserObjLoader(int type,ECSceneObjectLoader* pLoader);
	bool Load(const SCENEMODULE_PARAM& param);
	void Release(); // release scene module

	void Tick(DWORD dt,const A3DVECTOR3& vLoadCenter,const A3DVECTOR3& vCamera); // Tick scene module	
	

	//	Build scene renderer
	void BuildSceneRenderer(A3DViewport* pViewport, ECSceneRenderer* pRenderer);

	bool RenderECScene(A3DViewport* pViewport);		// Render scene objects
	bool RenderWater(const A3DTerrainWaterRender::RenderParam* pRenderParam);		// Render water
	//	Render occluders
	void RenderOccluders(A3DViewport* pViewport);
	// render grass 
	bool RenderGrasses(A3DViewport* pViewport);
	bool RenderGrassesOntoGBuf(A3DViewport* pViewport, A3DLPPRender* pLPPRender);
	bool RenderGrassesLPPForward(A3DViewport* pViewport, A3DLPPRender* pLPPRender);

	bool IsUnderWater(const A3DVECTOR3& vPos);
	float GetWaterHeight(const A3DVECTOR3& vPos);

	void SetViewRadius(float fRadius);
	void SetLoadCenter(const A3DVECTOR3& vCenter);

	//	Set terrain rendering quality level
	//	iLevel: [0, 1]
	void SetTerrainRenderLevel(int iLevel);
	//	Set water rendering quality level
	//	iLevel: [0, 2]
	void SetWaterRenderLevel(int iLevel);

	void SetMapName(const char* szName) { m_scnInnerParam.strMapName = szName;}

	void EnableSound(bool bEnable) { m_bSoundEnable = bEnable;}
	void EnableMusic(bool bEnable) { m_bMusicEnable = bEnable;}

	bool GetSoundFlag() const { return m_bSoundEnable;}
	bool GetMusicFlag() const { return m_bMusicEnable;}
	
	const AString&					GetMapName() { return m_scnInnerParam.strMapName;}
	ECTerrain*						GetECTerrain() { return m_pECTerrain; }
	ECScene*						GetECScene() { return m_pScene;}
	ECResLoadModule*				GetResLoadModule() {return m_pResLoadModule;}
	ECPlatform*						GetECPlatform() { return m_pECPlatform;}
	ECTriangleMan*					GetTriangleMan();
	A3DTerrainWater*				GetTerrainWater() const { return m_pTerrainWater;}
	A3DGrassLand*					GetGrassLand() const { return m_pGrassLand;}
	A3DGrassInteractor*				GetGrassInteractor() const { return m_pGrassLand ? m_pGrassLand->GetGrassInteractor():NULL;};
	A3DTerrainDecalCallback*		GetTerrainDecalCallBack() const { return m_pTerrainDecalCallBack; }
	const SCENEMODULE_INNERPARAM&	GetSceneModuleInnerParam() { return m_scnInnerParam;}

	ECCameraPosState*				GetCameraPosState();

	friend 	DWORD					GetResFromResLoader(ECResLoader* loader);


protected:

	bool  InitECTerrain(const char* szFile,const char* szTilePath,const SCENEMODULE_PARAM& param,float cx,float cz);
	bool  InitECScene(const char* szFile);
	void  ReleaseECTerrain();
	void  ReleaseECScene();
	void  FeedUserLoaderToScene();

	void Tick_Audio(DWORD dt,const A3DVECTOR3& vCameraPos);

private:
	ECPlatform*				m_pECPlatform;
	ECTerrain*				m_pECTerrain;
	ECScene*				m_pScene;
	ECResLoadModule*		m_pResLoadModule;
	
	ECCameraPosState*		m_pCameraPosState;
	
	// cache the user loader. These loaders will be registered into ECScene's loader list before loading ECScene resource.
	ObjectLoaderTable		m_UserObjLoaderTable;

	SCENEMODULE_INNERPARAM	m_scnInnerParam;

	A3DTerrainDecalCallback*m_pTerrainDecalCallBack;
	A3DTerrainWater*		m_pTerrainWater;
	A3DGrassLand*			m_pGrassLand;
	A3DGrassLandCallback*	m_pGrassLandCallBack;
	A3DGrassLoader*			m_pGrassLoader;

	bool					m_bSoundEnable;
	bool					m_bMusicEnable;
};

///////////////////////////////////////////////////////////////////////////
//	
//	ECSceneRenderInfo class
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECSceneRenderer
{
public:		//	Types

	//	Object mask
	enum
	{
		MASK_ECMODEL	= 0x00000001,
		MASK_BUILDING	= 0x00000002,
		MASK_GFX		= 0x00000004,
		MASK_CRITTER	= 0x00000008,
		MASK_ROAD		= 0x00000010,
		MASK_GROUNDDECAL= 0x00000020,
	};

	friend class ECSceneModule;
	friend class ECScene;
	friend class ECSceneObjectMgr;

public:		//	Constructor and Destructor

	ECSceneRenderer();
	virtual ~ECSceneRenderer();

public:		//	Attributes

public:		//	Operations

	//	Reset renderer
	void Reset();

	//	Render objects
	void RenderObjects(A3DViewport* pViewport, DWORD dwMask);
	//	Register shadow casters
	//	iGenStep: see ECShadowRender::STEP_xxx
	void RegisterShadowCasters(ECShadowRender* pShadowRender, DWORD dwMask, int iGenStep);
	//	Render onto shadow map
	//	iGenStep: see ECShadowRender::STEP_xxx
	void RenderOntoShadowMap(A3DViewport* pViewport, int iSlice, DWORD dwMask, int iGenStep);

	//	Get terrain
	ECTerrain* GetTerrain() { return m_pTerrain; }

protected:	//	Attributes

	ECTerrain*		m_pTerrain;

	APtrArray<ECSceneObject*>	m_aScnECModels;
	APtrArray<ECSceneObject*>	m_aScnBuildings;
	APtrArray<ECSceneObject*>	m_aScnGfxs;
	APtrArray<ECSceneObject*>	m_ScnCritters;

	APtrArray<ECSceneObject*>	m_aScnRoads;
	APtrArray<ECSceneObject*>	m_aScnGroundDecals;

protected:	//	Operations

};
