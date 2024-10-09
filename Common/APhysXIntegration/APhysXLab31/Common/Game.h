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

#include "EngineInterface.h"
#include "BrushesObjMgr.h"

class CRender;
class CMainActor;
class CCameraController;
//class BackgroundTask;
//class ScenarioMgr;
class ECWorldRender;

class CGame
{
public:
	typedef RawObjMgr::ObjTypeID			ObjID;
	typedef IPropPhysXObjBase::DrivenMode	DrivenMode;

public:
	CGame();
	~CGame();

	bool Init(CRender& g3dEngine);
	void Release();

	CGameProperty*		GetProperties() { return &m_Properties; }
	CCameraController*	GetCameraController() const { return m_pCameraCtrl; }

	A3DTerrain2*	GetTerrain() const { return m_pTerrain; }
	A3DSkySphere*	GetSky() const { return m_pA3DSky; }

	void			LoadTerrainAndPhysXTerrain(const char* szFile);

	IPhysXObjBase*	CreateObject(const ObjID objType, const char* szFile, const POINT& pos);
	IPhysXObjBase*  CreateObjectRuntime(const ObjID objType, const char* szFile, const A3DVECTOR3& pos, const bool OnGround = false, const bool isEnterRuntime = true);
	void			ReleaseLabPhysXObject(IPhysXObjBase* pObject);
	void			ReleaseAllSelectedObjects();

	Region*	CreateRegion(const POINT& pos);
	void	ReleaseRegion(Region* pObject);

	bool Tick(const unsigned int dwDeltaTime);
	bool Render();
//	APhysXScene* GetAPhysXScene() const { return m_pPhysXScene; }

	void OnLButtonDown(const int x, const int y, const unsigned int nFlags);
	void OnLButtonUp(const int x, const int y, const unsigned int nFlags);
//	void OnMButtonUp(const int x, const int y, const unsigned int nFlags);
	void OnMouseMove(const int x, const int y, const unsigned int nFlags);
	bool OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags);

	bool RayTraceObject(const CPoint& pt, const PhysRay& ray, PhysRayTraceHit& hitInfo);
	void CloneObjects();

	void SavePhysXDemoScene(const char* szFilePath);
	bool LoadPhysXDemoScene(const char* szFilePath);

	void SetUserEvent(IUserEvent* pUserEvent);
	void ChangeCameraMode();
	void JumpMainActor() const;
	void SwitchMainActor();
	void SwitchMainActorWalkRun() const;
	const TCHAR* GetCurrentMainActorCCType() const;
	void SwitchMainActorCCType(CAPhysXCCMgr::CCType ct);

/*	bool RayTraceObject(const PhysRay& ray, APhysXRayHitInfo& hitInfo);

	void ControlModelClothes();
	
	void AddForceInTheArea();
	void ChangeECModelPhysXState() const;

	void SwitchCCCollision() const;

	void OnKeyDownScenarioMgr(const unsigned int nChar);
	void OnKeyDownEditScenario();

	void EnterTopView();
	void ControlWeatherSurrounding();
	void ShowFFShapes(bool show = true);
	void ReloadFFFile(const AString& theFile);

	void SetMABindingFF(const AString& ff);
	void GetMABindingFF(AString& outFF) const;

	void TrytoHang();
	
	void TestAndDebug(HWND hWnd, bool fixed = false);
	*/
	void ShootAPhysXObject(HWND hWnd, const float scale, const float fVelocity);
	void ReleaseAllShotStuff() { m_objMgr.ReleaseAllShotObjects(); }

private:
	static const char* GetObjName(const CGameProperty::ShootStuffID id);
	static ObjID GetObjType(const CGameProperty::ShootStuffID id);

	bool LoadSky();
	bool LoadMusic(const char* szFile);
	bool LoadTerrain(const char* szFile);
	bool LoadPhysXTerrain(const char* szFile);
	void ChangeCursor(const bool bIsSysDef = false);

	bool InitPhysX();
	void ReleasePhysX();
	void StartSimulate();

	IPhysXObjBase* CreateObjectAndLoad(const ObjID objType, const char* szFile);
	bool GetTerrainPosByCursor(const int x, const int y, A3DVECTOR3& vOutPos);

	void ReleaseAllObject();

	CMainActor*	CreateMainActor();
	void		ReleaseMainActor();

/*	float GetForceValue(const float mass, const bool bIsRandom) const;
	bool AddForce(const int x, const int y, const bool bIsRandomForce);
*/

	void MoveLight(DWORD dwTime);

private:
	friend class CGameProperty;

	void OnEntryEmptyMode();
	void OnEntryEditMode();
	void OnEntrySimMode();
	void OnEntryPlayMode();

private:
//	static const unsigned int g_gameSceneVersion;
	static const A3DVECTOR3	g_vAxisX;
	static const A3DVECTOR3	g_vAxisY;
	static const A3DVECTOR3	g_vAxisZ;
	CGameProperty m_Properties;

	CRender*			m_pG3DEngine;
	CCameraController*	m_pCameraCtrl;
	A3DSkySphere*		m_pA3DSky;
	A3DSysCursor*		m_pCursor;
	AMSoundStream*		m_pBackMusic;
	A3DTerrain2*		m_pTerrain;

	APhysXEngine* m_pPhysXEngine;
	APhysXScene*  m_pPhysXScene;

	APhysXPicker  m_PhysXPicker;

/*	bool m_bShowFFShapes;

	bool m_bIsCloseModelClothes;
	bool m_bIsEnableWeather;
	bool m_bIsEnableSurrounding;

	AString m_strFFBindToMA;*/

	int				m_CMAIdx;
	CMainActor*		m_pCurrentMA;
	APtrArray<CMainActor*> m_MainActors;

	RegionMgr		m_RegionMgr;
	CPhysXObjMgr	m_objMgr;
	LoadPackage		m_LoadPackage;
	CBrushesObjMgr  m_BrushesObjMgr;
	Trigger			m_Trigger;

/*	BackgroundTask*	m_pBkGrndTask;
	ScenarioMgr* m_pScenarioMgr;
	MMPolicy m_MMP;*/
};

#endif