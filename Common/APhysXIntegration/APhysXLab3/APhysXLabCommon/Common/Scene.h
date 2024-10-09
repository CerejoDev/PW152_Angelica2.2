#pragma once

#include "EngineInterface.h"
#include "BrushesObjMgr.h"
#include "FlagMgr.h"
#include "PhysXObjSelGroup.h"
#include "ApxAttack.h"
#include "TimerEvent.h"
#include "UserContactReport.h"
#include "Recording.h"
#include "Recover.h"

class CRender;
class CMainActor;
class CCameraController;
class ECWorldRender;
class ApxActorBase;
class BackgroundTask;

#ifdef _ANGELICA3
	#include "ECScene.h"
	class ECSceneModule;
	class ECPLoaderCB : public IResLoaderCallBack
	{
	public:
		ECPLoaderCB() { m_bIsRuntime = false; m_pPhysXScene = 0; }

		virtual void LoadBuliding(int id, ECBuilding* pBuilding);
		virtual void ReleaseBuilding(int id, ECBuilding* pBuilding);
		virtual void LoadECModel(int id, const char* szFile = NULL) {}
		virtual void ReleaseECModel(int id, const char* szFile = NULL) {}

		void EnterRuntime(APhysXScene& aPhysXScene);
		void LeaveRuntime();

	private:
		struct ItemData
		{
			ItemData() { m_ID = -1, m_pBrush = 0; m_pAPhysXDesc = 0; m_pAPhysXObj = 0; }

			int m_ID;
			CBrushesObj* m_pBrush;
			APhysXObjectInstanceDesc* m_pAPhysXDesc;
			APhysXObject* m_pAPhysXObj;
		};

	private:
		bool m_bIsRuntime;
		APhysXScene* m_pPhysXScene; 
		std::vector<ItemData> m_arrItemData;
	};
#else
	class CECTriangleMan;
	class CECShadowRender;
#endif

enum RagdollPerfTest
{
	RPT_DISABLE_TEST = 0, 
	RPT_NO_RAGDOLL = 1, 
	RPT_ALL_LOAD = 2, 
	RPT_DYN_LOAD = 3,
	RPT_DYN_SIM  = 4,
};

class Scene : public APhysXSerializerBase, public IRoleCallback, public APhysXUserCreationDeletionReport
{
public:
	static APhysXU32 GetVersion() { return 0xAA000004; }
	static APhysXU32 GetObjDocVersion() { return 0xBB000001; }

	typedef ObjManager::ObjTypeID			ObjID;
	typedef IPropPhysXObjBase::DrivenMode	DrivenMode;

	enum SceneMode
	{
		SCENE_MODE_EMPTY	= 0, 
		SCENE_MODE_PLAY_OFF = 1,
		SCENE_MODE_PLAY_ON  = 2,
	};

	enum ShootMode
	{
		SM_EYE_TO_CENTER = 0,
		SM_EYE_TO_MOUSE	,
		SM_MOUSE_FALLING,
		SM_FRONT_FALLING,
		//	SM_KINEMATIC,
		SM_END_FLAG               // this is a flag. No function defined.
	};

	enum ShootStuffID
	{
		SSI_SPHERE = 0,
		SSI_BOX,
		SSI_BARREL,
		//	SSI_BREAKABLEBOX,
		//	SSI_BOMB,
		SSI_END_BOUND
	};
	
public:
	Scene();
	virtual ~Scene();

	bool Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen);
	void Release();
	bool LoadTerrainAndPhysXTerrain(const char* szFile);

	int GetRandSeed() const;
	void SetRandSeed(int rs);
	bool Replay(const char* szFile);
	void MakeRecordingFileName(bool bIsEpisode, AString& outFullPathName);
	float GetReproTickSec() const { return m_pRecording->GetReproTickSec(); }
	bool QueryAsyncKeyDown(int vKey) { return m_pRecording->QueryAsyncKeyDown(vKey); }

	A3DTerrain2*	GetTerrain() const { return m_pTerrain; }
	A3DSkySphere*	GetSky() const { return m_pA3DSky; }
	CRender*		GetRender() const { return m_pG3DEngine; }
	APhysXScene*	GetAPhysXScene() const { return m_pPhysXScene; }
	LoadPackage&	GetLoadPackage() { return m_LoadPackage; }
	ObjStateReportMgr& GetOSRMgr() { return m_OSRMgr; }
	ApxObjOptReportMgr& GetAOORMgr() { return m_AOORMgr; }
	CPhysXObjSelGroup& GetSelGroup() { return m_SelGroup; }
	CRecording&		GetRecording() { return *m_pRecording; }
	const CCameraController* GetActiveSmartCamCtrl(bool bIsReferReproEvent) const { return m_pCamCtrlAgent->GetActiveCamera(bIsReferReproEvent); }
	CCameraController* GetNormalCamCtrl() const { return m_pCameraCtrl; }
	CCamCtrlAgent*	GetCamCtrlAgent() const { return m_pCamCtrlAgent; }

	void RegisterRegionEvent(IUserEvent* pUserEvent);
	void UnRegisterRegionEvent(IUserEvent* pUserEvent);
	void RegisterTimerEvent(IEventNotify& notify, float delayTime, TimerEvent::DelayType type = TimerEvent::DT_TIME, int nbEvents = 1);
	void RegisterTimerEvent(TimerEvent* pTEvent);
	void UnRegisterTimerEvent(TimerEvent* pTEvent);
	void RegisterPhysXContactDamage(ApxActorBase* pActor);
	void UnRegisterPhysXContactDamage(ApxActorBase* pActor);

	void SavePhysXDemoScene(const char* szFilePath);
	bool LoadPhysXDemoScene(const char* szFilePath);
	void SavePhysXDemoScene(APhysXUserStream& saveStream);
	bool LoadPhysXDemoScene(APhysXUserStream& loadStream, bool& outIsLowVer);

	bool GetCurrentMouseClientPoint(HWND hWnd, POINT& outPT);
	void ScreenPointToRay(const int x, const int y, PhysRay& outRay);
	bool IsRuntime() const { return QuerySceneMode(SCENE_MODE_PLAY_ON); }
	void SetSceneMode(const SceneMode& sm);
	bool QuerySceneMode(const SceneMode& sm) const;
	const TCHAR* GetSceneModeDesc(bool bIsFullDesc = false) const;

	void	   SetDrivenMode(const DrivenMode& dm);
	DrivenMode GetDrivenMode() const { return m_iDrivenMode; }
	bool	   QueryDrivenMode(const DrivenMode& dm) const;

	ShootStuffID GetShootType() const { return m_iSSID; }
	ShootMode GetShootMode() const { return m_iSMode; }
	void ChangeShootStuffType(int shootID = -1);
	void ChangeShootMode(int shootMode = -1);
	void ShootAPhysXObject(HWND hWnd, const float scale, const float fVelocity);
	void ReleaseAllShotStuff() { ObjManager::GetInstance()->ReleaseAllShotObjects(); }

	void EnableShowTerrianGrid(const bool bEnable);
	bool IsShowTerrianGridEnabled() const { return m_status.ReadFlag(GAME_DRAW_TERRAIN_GRID); }
	void EnablePhysXProfile(const bool bEnable);
	bool IsPhysXProfileEnabled() const { return m_status.ReadFlag(GAME_DRAW_PHYSX_PRIFILE); }
	void EnablePhysXDebugRender(const bool bEnable);
	bool IsPhysXDebugRenderEnabled() const { return m_status.ReadFlag(GAME_DRAW_PHYSX_DBGRENDER); }
	void EnablePhysXHW(const bool bEnable);
	bool IsPhysHWEnabled() const { return m_status.ReadFlag(GAME_ENABLE_PHYSX_HW); }
	const char* GetActiveSceneFile() const;
	const char* GetTerrainFile() const { return m_strTrnFile; }

	bool PhysXSimASingleStep();
	void ChangePhysXPauseState();
	bool IsPhysXSimPauseState() const { return m_status.ReadFlag(GAME_IS_PHYSX_SIM_PAUSE); }
	bool IsPhysXSimEnable() const;

	void GetAllAssailablePhysXObj(IPhysXObjBase* pCaster, APtrArray<IAssailable*>& outArray);
	IPhysXObjBase* GetPhysXObj(const char* objName);
	void ChangeCameraMode(bool bEnforceSyncReplayCam = false);
	bool GetTerrainPosByCursor(const int x, const int y, A3DVECTOR3& vOutPos);

	const TCHAR* GetRPTModeText() const;
	void SetRPTMode(RagdollPerfTest mode);
	void EnumNextRPTMode();
	bool QueryRPTMode(RagdollPerfTest mode) const;
	bool QueryRPTStateEnable(const NxVec3& pos, bool bIsLoadPhysX = true) const;
	bool Tick(float dtSec);
	bool Render();
	void RenderNextFrameInfo() { m_generalAttMgr.RenderNextFrameDebugData(); }

	bool CreateFFSubObject();
	IPhysXObjBase*  CreateObjectRuntime(const ObjID objType, const char* szFile, const A3DVECTOR3& pos, const bool OnGround = false, const bool isEnterRuntime = true);
	IObjBase*		CreateObjectRuntime(const ObjID objType, const A3DVECTOR3& pos, const bool isEnterRuntime = true);
	void			ReleaseObject(IObjBase* pObject);
	void			ReleaseAllObjects();

	void		ReleaseMainActor(CMainActor* pMA);
	CMainActor* CreateMainActor(const A3DVECTOR3& pos, const char* szFile = 0);
	CMainActor* GetMainActor(int index) const { assert(index >= 0 && index < m_MainActors.GetSize()); return m_MainActors[index]; }
	CMainActor* GetCurrentActor() const { return m_pCurrentMA; }
	int			GetMainActorCount() const { return m_MainActors.GetSize(); }
	void		JumpMainActor() const;
	int			SwitchMainActor();
	void		SwitchMainActorWalkRun() const;
	const TCHAR* GetCurrentMainActorCCType() const;
	void		SwitchMainActorCCType(CAPhysXCCMgr::CCType ct);
	void		ChangeMainActorPushForce(float deltaValue);
	void		TrytoSwing();
	void		SwitchAllPhysXState();
	void		ControlModelClothes();

	void		ReleaseNPCActor(IRoleActor* pRA);
	IRoleActor* CreateNPCActor(const char* szFile = 0, const A3DVECTOR3* pPos = 0);
	IRoleActor* GetNPCActor(int index) const { assert(index >= 0 && index < m_NPCActors.GetSize()); return m_NPCActors[index]; }
	int			GetNPCActorCount() const { return m_NPCActors.GetSize(); }
	ApxActorBase* GetActorBase(IObjBase* pObj) const;

	virtual void OnObjectDeletionNotify(APhysXObject* pObject);
	virtual IRoleActor* CreateNPCRole(CPhysXObjDynamic& dynObj);
	virtual bool OnLButtonDown(const int x, const int y, const unsigned int nFlags);
	virtual void OnLButtonUp(const int x, const int y, const unsigned int nFlags);
	virtual void OnMButtonUp(const int x, const int y, const unsigned int nFlags){}
	virtual void OnMouseMove(const int x, const int y, const unsigned int nFlags);
	virtual bool OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags);

	IObjBase* GetRayHitObject() { return m_RHOwner.GetRayHit(); }
	bool RayTraceObject(const POINT& pt, const PhysRay& ray, PhysRayTraceHit& outHitInfo, bool onlyPhysX = false, APhysXRaycastFilterType apxRFType = APX_RFTYPE_ALL, APhysXRayHitInfo* poutHitInfo = 0);

	//extra data can be rendered in user defined function
	void RegisterRenderExtra(void (*renderExtraCallBack)(CRender*)) { _renderExtra = renderExtraCallBack; }
	void UnRegisterRenderExtra() { _renderExtra = 0; }

	//script operation
	void SetDefScript(const AString& defScript) { m_defScript = defScript; }
	const AString& GetDefScript() const { return m_defScript; }
	void SetScripts(const std::vector<AString>& scripts) { m_strScripts = scripts; }
	const std::vector<AString>& GetScripts() const { return m_strScripts; }

	void EnableActorIK(bool bEnable);
	bool IsActorIKEnabled() const;

	bool UpdatePropForBuildScene(IObjBase* pObj);
	void ToBreakAndRagdoll();

	void SetMinTickTime(int val) { m_minTickTime = val; }
	int GetMinTickTime() const { return m_minTickTime; }

protected:
	enum GameFlag
	{
		GAME_INIT_START				= (1<<0),
		GAME_INIT_ENDOK				= (1<<1),
		GAME_DRAW_TERRAIN_GRID		= (1<<2),
		GAME_DRAW_PHYSX_PRIFILE		= (1<<3),
		GAME_DRAW_PHYSX_DBGRENDER	= (1<<4),
		GAME_ENABLE_PHYSX_HW		= (1<<5),
		GAME_IS_PHYSX_SIM_PAUSE		= (1<<6),
		GAME_IS_PHYSX_SIM_STEP		= (1<<7),
		GAME_IS_PICK_PHYSX_STH		= (1<<8),
		GAME_CREATE_AND_PICK_STH	= (1<<9),
		GAME_IS_OWNER_TERRAIN		= (1<<10),
		GAME_IS_REPROEPI_LOADING	= (1<<11),
		GAME_IS_MODELCLOTHES_ENABLE = (1<<12),
	};

	CFlagMgr<GameFlag>& GetStatus() { return m_status; }
	RayHitOwner&		GetRayHitOwner() { return m_RHOwner; }
	IPhysXObjBase* CreateObjectAndLoad(const ObjID objType, const char* szFile);

	void SaveAnObject(const char* szFilePath);
	IObjBase* LoadAnObject(const char* szFilePath);

private:
	static const char* GetObjName(const ShootStuffID id);
	static ObjID GetObjType(const ShootStuffID id);

	bool IsReady() const { return m_status.ReadFlag(GAME_INIT_ENDOK); }
	bool LoadSky();
	bool LoadMusic(const char* szFile);
	bool LoadPW2Map(const char* szFile);
	bool LoadTerrain(const char* szFile);
	bool LoadPhysXTerrain(const char* szFile);
	void CommitTerrain(A3DTerrain2& terrain, const char* szFile);
	void ReleaseTerrain();
	void ChangeCursor(const bool bIsSysDef = false);

	bool InitPhysX();
	bool CreateAScene();
	void ReleasePhysX();

	bool GetHitObjectFileName(AString& outstrFilePath, POINT* poutPT = 0);
	void MoveLight(DWORD dwTime);

	void SetEmptyMode();
	void SetPlayOffMode();
	void SetPlayOnMode();

	void StartSimulate();
	void EndSimulate();
	void ApplyPushForce();

	virtual void OnSceneModeChange(const SceneMode& newSM) {}

private:
	struct SceneModeDesc 
	{
		const SceneMode	iState;
		const TCHAR*	pDesc;
		const TCHAR*	pFullDesc;
	};
	static const SceneModeDesc gGMDesc[];

	static const A3DVECTOR3	g_vAxisX;
	static const A3DVECTOR3	g_vAxisY;
	static const A3DVECTOR3	g_vAxisZ;

	CRender*			m_pG3DEngine;
	CCamCtrlAgent*		m_pCamCtrlAgent;
	CCameraController*	m_pCameraCtrl;
	A3DSkySphere*		m_pA3DSky;
	A3DSysCursor*		m_pCursor;
	AMSoundStream*		m_pBackMusic;
	A3DTerrain2*		m_pTerrain;
	//render call back 
	void (*_renderExtra)(CRender* render); // toupdate use obj instead of callback function

	ECPlatform*			m_pECPlatform;
	ECSceneModule*		m_pSceneModule;

	APhysXEngine*		m_pPhysXEngine;
	APhysXScene*		m_pPhysXScene;
	GeneralAttackMgr	m_generalAttMgr;

	mutable const SceneModeDesc* m_pGMD;
	DrivenMode			m_iDrivenMode;
	ShootMode			m_iSMode;
	ShootStuffID		m_iSSID;
	CFlagMgr<GameFlag>	m_status;

	RayHitOwner			m_RHOwner;
	ObjStateReportMgr	m_OSRMgr;
	ApxObjOptReportMgr	m_AOORMgr;
	CPhysXObjSelGroup	m_SelGroup;
	APhysXPicker		m_PhysXPicker;
	LoadPackage			m_LoadPackage;
	Trigger				m_Trigger;
	TimerEventMgr		m_TEventMgr;
	ApxContactReport	m_ContactReport;
	ModelBreakReport	m_BreakReport;
	CCHitReport			m_CCHitReceiver;
	CRecording*			m_pRecording;
#ifdef _ANGELICA3
	ECPLoaderCB			m_MapLoaderCB;
#else
	CECTriangleMan*		m_pTriangleMan;
	CECShadowRender*	m_pShadowRender;
#endif

	RagdollPerfTest		m_RPTMode;
	int					m_RPTOutputTickNum;
	float				m_fDynLoadRadius;
	float				m_MAPushForce;
	int					m_CMAIdx;
	CMainActor*			m_pCurrentMA;
	APtrArray<CMainActor*> m_MainActors;
	APtrArray<IRoleActor*> m_NPCActors;

	int m_minTickTime;
	AString	m_strTrnFile;
	AString m_strActiveScene;

	//script data
	std::vector<AString> m_strScripts;//script for scene
	AString              m_defScript;//default script for all

	BackgroundTask*		m_pBkGrndTask;
};
