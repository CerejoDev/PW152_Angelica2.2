/*
 * FILE: Game.h
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
#ifndef _APHYSXLAB_GAME_H_
#define _APHYSXLAB_GAME_H_

#include "APhysX.h"
#include "GraphicsSyncRB.h"
#include "PhysXObjMgr.h"
#include "CommonMovement.h"

#include "..\BrushCollision\SceneExtraConfig.h"//yx
#include "..\BrushCollision\BrushesObjMgr.h"//yx


class CRender;
class CMainActor;
class CCameraController;
class BackgroundTask;
class TestConvexMesh;
class CMOBDamping;
class CPhysXObjECModel;
class ScenarioMgr;
class CECShadowRender;

class CGame
{
public:
	typedef IPhysXObjBase::DrivenMode   DrivenMode;
	typedef CPhysXObjMgr::ObjTypeID     ObjID;
	
	enum GameMode
	{
		GAME_MODE_EDIT     = 0,
		GAME_MODE_SIMULATE = 1,	 // simulate physics...
		GAME_MODE_PLAY     = 2,  // there will be an avatar run in the game...	
	};

	enum ShootMode
	{
		SM_EYE_TO_CENTER	= 0,
		SM_EYE_TO_MOUSE		= 1,
		SM_MOUSE_FALLING    = 2,
		SM_FRONT_FALLING	= 3,
		SM_EACH_ECM			= 4,
		SM_FIXED_POINT_1	= 5,
		SM_FIXED_POINT_2	= 6,
		SM_KINEMATIC		= 7,
		SM_END_FLAG               // this is a flag. No function defined.
	};

	enum ShootStuffID
	{
		SSI_SPHERE		= 0,
		SSI_BOX			= 1,
		SSI_BARREL		= 2,
		SSI_BREAKABLEBOX,
		SSI_BOMB,

		SSI_END_BOUND
	};

public:
	CGame();
	~CGame();

	bool Init(CRender& g3dEngine);
	void Release();
	bool Reset();

	IPhysXObjBase*  CreateObjectRuntime(const ObjID objType, const char* szFile, const NxVec3& pos, const bool OnGround = false, const bool isEnterRuntime = true);
	IPhysXObjBase*	CreateObject(const ObjID objType, const char* szFile, const POINT& pos);
	IPhysXObjBase*	CreateObject(const ObjID objType, const bool isNPC = true) { return m_objMgr.CreateObject(objType, isNPC); }
	void			ReleaseObject(IPhysXObjBase* pObject);

	bool Tick(const unsigned int dwDeltaTime);
	bool Render();
	APhysXScene* GetAPhysXScene() const { return m_pPhysXScene; }

	GameMode GetGameMode() const { return m_iGameMode; }
	void	 SetGameMode(const GameMode iGameMode); 

	DrivenMode GetDrivenMode() const { return m_iDrivenMode; }
	bool	   QueryDrivenMode(const DrivenMode dm) const { return dm == m_iDrivenMode; }
	void	   SetDrivenMode(const DrivenMode dm);

	A3DTerrain2*	GetTerrain() const { return m_pTerrain; }
	const char*     GetTerrainFile() const { return m_strTrnFile; }
	void			LoadTerrainAndPhysXTerrain(const char* szFile);
	bool			GetTerrainPosByCursor(const int x, const int y, A3DVECTOR3& vOutPos);

	bool			GetTerrianGridShow() const { return m_bShowTerrianGrid; }
	void			SetTerrianGridShow(const bool bShow);// { m_bShowTerrianGrid = bShow; }

	IPhysXObjBase*	GetPickedObject() const { return m_pObjPicked; }
	void			SetPickedObject(IPhysXObjBase* pObject = 0);
	void			DelPickedObject() { ReleaseObject(m_pObjPicked); };

	bool LoadPhysXDemoScene(const char* szFilePath);
	void SavePhysXDemoScene(const char* szFilePath);

	void OnLButtonDown(const int x, const int y, const unsigned int nFlags);
	void OnLButtonUp(const int x, const int y, const unsigned int nFlags);
	void OnMButtonUp(const int x, const int y, const unsigned int nFlags);
	void OnMouseMove(const int x, const int y, const unsigned int nFlags);
	bool OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags);

	bool RayTraceObject(const PhysRay& ray, PhysRayTraceHit& hitInfo);
	bool RayTraceObject(const PhysRay& ray, APhysXRayHitInfo& hitInfo);
	IPhysXObjBase* GetHitObject() { return m_pObjRayHit; }
	void SetHitObject(IPhysXObjBase* pNewHit);
	void CloneHitObject();
	bool GetHitObjectFileName(AString& outstrFilePath);

	void SetDragAndDropMove() { m_objMgr.SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE); }
	void SetDragAndDropRotate() { m_objMgr.SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_ROTATE); }
	void SetDragAndDropScale() { m_objMgr.SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_SCALE); }

	void EnablePhysXProfile(const bool bEnable);
	bool IsPhysXProfileEnabled() const { return m_bIsPhysXProfile; }

	void EnablePhysXDebugRender(const bool bEnable);
	bool IsPhysXDebugRenderEnabled() const { return m_bIsPhysXDbgRender; }

	void EnablePhysXHW(const bool bEnable) { m_pPhysXEngine->EnableHardwarePhysX(bEnable); }
	bool IsPhysXHWEnable() const { return m_pPhysXEngine->IsHardwarePhysXEnabled(); }

	CCameraController* GetCameraController() const { return m_pCameraCtrl; }
	void ChangeCameraMode();

	bool GetPhysXPauseState() const { return m_bIsPhysXPause; }
	bool SingleStepPhysXSim() { return m_bIsPhysXPause? m_bIsPhysXStep = true : false; }
	void ChangePhysXPauseState() { if (GAME_MODE_EDIT != GetGameMode()) m_bIsPhysXPause = !m_bIsPhysXPause; }

	ShootMode GetShootMode() const { return m_iSMode; }
	ShootStuffID GetShootType() const { return m_iSSID; }
	void ChangeShootMode();
	void ChangeShootStuffType();
	void ShootAPhysXObject(HWND hWnd, const float scale, const float fVelocity);
	void ReleaseAllShotStuff() { m_objMgr.ReleaseAllShotObjects(); }

	CECShadowRender* GetShadowRender() { return m_pShadowRender; }
	//TestConvexMesh* GetTestConvexMesh() const { return m_pTConvexMesh; }
	void ControlModelClothes();
	
	void AddForceInTheArea();
	void ChangeECModelPhysXState() const;
	void ChangeTestCMState() const;

	void SwitchCCCollision() const;
	void JumpMainActor() const;
	void SwitchMainActor();
	void SwitchMainActorWalkRun() const;
	void IncreaseMainActorCCVolume() const;
	void DecreaseMainActorCCVolume() const;

	void OnKeyDownScenarioMgr(const unsigned int nChar);
	void OnKeyDownEditScenario();

	void EnterTopView();
	void ControlWeatherSurrounding();
	void ShowFFShapes(bool show = true);
	void ReloadFFFile(const AString& theFile);

	void SetMABindingFF(const AString& ff);
	void GetMABindingFF(AString& outFF) const;

	void TrytoHang(const PhysRay& ray);
	
	void TestAndDebug(HWND hWnd, bool fixed = false);
	
private:
	static const char* GetObjName(const ShootStuffID id);
	static ObjID GetObjType(const ShootStuffID id);

	IPhysXObjBase*	CreateObjectAndLoad(const ObjID objType, const char* szFile, const bool isNPC = true);

	bool InitPhysX();
	void ReleasePhysX();
	void ReleaseAllObject();
	bool IsPhysXSimEnable() const;

	void CreateNPCActor();
	CMainActor*	CreateMainActor();
	void		ReleaseMainActor();

	bool LoadSky();
	bool LoadTerrain(const char* szFile);
	bool LoadPhysXTerrain(const char* szFile);
	void ChangeCursor(const bool bIsSysDef = false);

	void LoadCameraPose(const NxStream& nxStream);
	void SaveCameraPose(NxStream& nxStream);

	void StartSimulate();
	float GetForceValue(const float mass, const bool bIsRandom) const;
	bool AddForce(const int x, const int y, const bool bIsRandomForce);
	
	void AppendModel(const int n, const int size);
	CPhysXObjECModel* GetNextECModel_NonShot() const;
	void ChangeECMScale();

private:
	static const unsigned int g_gameSceneVersion;
	static const A3DVECTOR3	g_vAxisX;
	static const A3DVECTOR3	g_vAxisY;
	static const A3DVECTOR3	g_vAxisZ;

	bool m_bInitedStart;
	bool m_bInitedEndOK;

	bool m_bIsPhysXProfile;
	bool m_bIsPhysXDbgRender;
	bool m_bShowTerrianGrid;
	bool m_bShowFFShapes;

	bool m_bIsPhysXPause;
	bool m_bIsPhysXStep; 
	bool m_bIsPickPhysXSomething;
	bool m_bIsCloseModelClothes;
	bool m_bIsEnableWeather;
	bool m_bIsEnableSurrounding;

	GameMode   m_iGameMode;
	DrivenMode m_iDrivenMode;
	ShootMode	 m_iSMode;
	ShootStuffID m_iSSID;

	APhysXEngine* m_pPhysXEngine;
	APhysXScene*  m_pPhysXScene;
	APhysXPicker  m_PhysXPicker;

	CRender*			m_pG3DEngine;
	CECShadowRender*	m_pShadowRender;
	A3DSysCursor*		m_pCursor;
	A3DSkySphere*		m_pA3DSky;
	A3DTerrain2*		m_pTerrain;
	CCameraController*	m_pCameraCtrl;

	AString m_strTrnFile;
	AString m_strFFBindToMA;

	POINT m_ptRayHit;
	IPhysXObjBase* m_pObjPicked;
	IPhysXObjBase* m_pObjRayHit;
	int				m_CMAIdx;
	CMainActor*		m_pCurrentMA;
	APtrArray<CMainActor*> m_MainActors;

	CPhysXObjMgr	m_objMgr;
	BackgroundTask*	m_pBkGrndTask;

	CGraphicsSyncRB* m_pGraphicSync;

//	TestConvexMesh* m_pTConvexMesh;
//	CMOBDamping* m_pCMOBDamping;

	ScenarioMgr* m_pScenarioMgr;
	MMPolicy m_MMP;

	//yx Brushes object manager
	CBrushesObjMgr m_BrushesObjMgr;	
	SceneExtraConfig m_SceneConfig;
	
	bool GetTracePosByCursor(int x, int y, A3DVECTOR3& vHitPos);

public:
	void MoveObj()
	{
		bool bMove = m_BrushesObjMgr.GetMoving();
		m_BrushesObjMgr.SetMoving(!bMove);
	}

	void SwitchCC(int index = -1);
	ACString GetCCName() const;
		
	friend class SceneExtraConfig;
	
	SceneExtraConfig& GetSceneExtraConfig() { return m_SceneConfig; }
	CBrushesObjMgr& GetBrushObjMgr() { return m_BrushesObjMgr; }
};

extern CGame g_Game;

#endif