//	Game.h

#pragma once

#include <ABaseDef.h>
#include <A3DVector.h>
#include <AString.h>

#include "APhysX.h"

#include "PhysRayTrace.h"

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

class CPhysTerrain;
class CPhysObjectMan;
class A3DSkySphere;
class A3DSysCursor;
class APhysXScene;
class CPhysObject;

class A3DGFXEx;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CGame
//	
///////////////////////////////////////////////////////////////////////////

class CGame
{
public:		//	Types

	enum
	{
		GAME_MODE_EDIT,			// edit mode...
		
		GAME_MODE_SIMULATE,		// simulate physics...

		GAME_MODE_PLAY,			// play mode, there will be an avatar run in the game

	};

public:		//	Constructor and Destructor

	CGame();
	virtual ~CGame();

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init();
	//	Release game
	void Release();
	//	Reset game
	bool Reset();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render();

	//	Show or hide cursor
	void ShowCursor(bool bShow);

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Load physX terrain data
	bool LoadPhysXTerrain(const char* szFile);

	void LoadTerrainAndPhysXTerrain(const char* szFile);

	//	Set picked object
	void SetPickedObject(CPhysObject* pObject, bool bNewObject = true);
	//	Get picked object
	CPhysObject* GetPickedObject() { return m_pPickedObject; }
	//	Set up picked object
	bool SetupPickedObject(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir , const A3DVECTOR3& vUp);

	//	Get instance handle
	HINSTANCE GetInstance() { return m_hInstance; }
	//	Get window handle
	HWND GetWindow() { return m_hWnd; }
	//	Get terrain initialized flag
	bool IsTerrainInit() { return m_pTerrain ? true : false; }
	//	Get terrain
	CPhysTerrain* GetTerrain() { return m_pTerrain; }
	//	Get sky
	A3DSkySphere* GetSky() { return m_pA3DSky; }
	//	Get physical scene
	APhysXScene* GetPhysXScene() { return m_pPhysXScene; }
	//	Get current terrain file name
	const char* GetTerrainFile() { return m_strTrnFile; }

	//	Set / Get wire mode flag
	void SetWireModeFlag(bool bWire) { m_bWireMode = bWire; }
	bool GetWireModeFlag() { return m_bWireMode; }
	//	Set / Get physical state
	void SetPhysState(int iState);
	int GetPhysState() { return m_iPhysState; }

	void SetGameMode(int iGameMode); 
	int GetGameMode() { return m_iGameMode; }

	void EnablePhysXProfile(bool bEnable);
	bool IsPhysXProfileEnabled() { return m_bEnablePhysXProfile; }

	void EnablePhysXDebugRender(bool bEnable);
	bool IsPhysXDebugRenderEnabled() { return m_bEnablePhysXDebugRender; }


	APhysXPicker* GetPhysXPicker()
	{
		return &m_PhysXPicker;
	}

	// szFilePath is global path...
	bool LoadPhysXDemoScene(const char* szFilePath);
	bool SavePhysXDemoScene(const char* szFilePath);

	bool LoadCameraPose(NxStream* pNxStream);
	bool SaveCameraPose(NxStream* pNxStream);

	bool LoadGfxEx(const char* szFileName, const A3DVECTOR3& vPos);

	bool RayTracePhysObject(const PhysRay& ray);
	CPhysObject* GetHitObject() { return m_pHitObject; }

	void PickHitObject()
	{
		SetPickedObject(m_pHitObject, false);
	}
	
	void RemovePickedObject();

	void TurnOnPhysX();
	void TurnOffPhysX();
	
	void TestWindFF();
	void TestCloth();

	void PhysXCoreDump(const char* szFileName);

protected:	//	Attributes

	CPhysTerrain*		m_pTerrain;
	CPhysObjectMan*		m_pObjectMan;
	A3DSkySphere*		m_pA3DSky;
	A3DSysCursor*		m_pCursor;
	APhysXScene*		m_pPhysXScene;
	CPhysObject*		m_pPickedObject;	//	Picked object
	CPhysObject*		m_pHitObject;		//	Raytrace hit object

	bool m_bPickNewObject;			// I pick a new object or an existing object?


	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	DWORD		m_dwTickTime;		//	Current tick time
	AString		m_strTrnFile;		//	Current terrain file

	bool		m_bWireMode;		//	Wire mode
	int			m_iPhysState;		//	Physical state

	int m_iGameMode;
	
	bool m_bEnablePhysXProfile;
	bool m_bEnablePhysXDebugRender;

	APhysXPicker m_PhysXPicker;

	AString m_strTerrainFileName;

	APhysXArray<A3DGFXEx* > m_arrGfxExes;


protected:	//	Operations

	//	Load sky
	bool LoadSky();
	//	Initialize physical module
	bool InitPhysX();
	//	Release physical module
	void ReleasePhysX();
};

extern CGame g_Game;

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

