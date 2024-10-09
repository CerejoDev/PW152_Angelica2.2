/*
 * FILE: Game.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include <A3DGfxEngine.h>
#include <A3DGFXExMan.h>
#include <A3DLPPLight.h>
#include "ECWorldRender.h"
#include "Collision.h"

// some config variable
bool gChangeECMScale = false;
bool gEnableDynamicAPhysXCC = true;
bool gDefaultAPhysXCCIsOpen = true;
bool gEnableMainActorPhysXBody = false;
int gTotalMACount = 1;
int gTotalNPCCount = 0;

// for test & debug Rive FF
bool gTestRiveFF = false;

class UserEventsTest : public IUserEvent
{
public:
	virtual void RuntimeBegin(const Region& place)
	{
		a_LogOutput(1, "RuntimeBegin: %s", place.GetShapeTypeText(place.GetShapeType()));
	}
	virtual void RuntimeEnd(const Region& place)
	{
		a_LogOutput(1, "RuntimeEnd: %s", place.GetShapeTypeText(place.GetShapeType()));
	}

	virtual void EnterRegion(const Region& place, unsigned int nbEntities, IObjBase* entities)
	{
		a_LogOutput(1, "EnterRegion: %s, %d, %d", place.GetShapeTypeText(place.GetShapeType()), nbEntities, entities);
	}
	virtual void LeaveRegion(const Region& place, unsigned int nbEntities, IObjBase* entities)
	{
		a_LogOutput(1, "LeaveRegion: %s, %d, %d", place.GetShapeTypeText(place.GetShapeType()), nbEntities, entities);
	}
} gTestUE;

//const unsigned int CGame::g_gameSceneVersion = 0xCC00000F;
const A3DVECTOR3 CGame::g_vAxisX(1.0f, 0.0f, 0.0f);
const A3DVECTOR3 CGame::g_vAxisY(0.0f, 1.0f, 0.0f);
const A3DVECTOR3 CGame::g_vAxisZ(0.0f, 0.0f, 1.0f);

CGame::CGame() : m_Properties(*this, m_objMgr)
{
	m_pG3DEngine  = 0;
	m_pCameraCtrl = 0;
	m_pA3DSky	  = 0;
	m_pCursor	  = 0;
	m_pBackMusic  = 0;
	m_pTerrain	  = 0;

	m_pPhysXEngine = 0;
	m_pPhysXScene  = 0;
	m_objMgr.GetProperties()->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);

	m_CMAIdx	  = -1;
	m_pCurrentMA  = 0;
	m_Properties.GetOSRMgr().RegisterReceiver(m_BrushesObjMgr);
	m_Trigger.InitManagers(&m_RegionMgr, &m_objMgr);
/*	m_bShowFFShapes		= false;

	m_bIsCloseModelClothes = false;
	m_bIsEnableWeather = false;
	m_bIsEnableSurrounding = false;

	m_pBkGrndTask = 0;
	a_InitRandom();
	m_pScenarioMgr = 0;*/
}

CGame::~CGame()
{
	if (m_Properties.m_status.ReadFlag(CGameProperty::GAME_INIT_START))
		Release();
}

const char* CGame::GetObjName(const CGameProperty::ShootStuffID id)
{
	static bool bInited = false;
	static char* theInfo[CGameProperty::SSI_END_BOUND];
	
	assert(id < CGameProperty::SSI_END_BOUND);
	if (bInited)
		return (id == CGameProperty::SSI_END_BOUND)? theInfo[0] : theInfo[id];
	
	bInited = true;
	theInfo[CGameProperty::SSI_SPHERE] = "Models\\PhysXComponents\\球\\球.ecm3";
	theInfo[CGameProperty::SSI_BOX]    = "Models\\PhysXComponents\\箱子\\箱子.ecm3";
	theInfo[CGameProperty::SSI_BARREL] = "Models\\PhysXComponents\\桶\\桶.ecm3";
//	theInfo[CGameProperty::SSI_BREAKABLEBOX] = "Models\\PhysXComponents\\箱子破损\\箱子破损.ecm3";
//	theInfo[CGameProperty::SSI_BOMB] = "ForceField\\explo.xff";

	return GetObjName(id);
}

CGame::ObjID CGame::GetObjType(const CGameProperty::ShootStuffID id)
{
//	if (SSI_BOMB == id)
//		return RawObjMgr::OBJ_TYPEID_FORCEFIELD;
//	else
		return RawObjMgr::OBJ_TYPEID_ECMODEL;
}

bool CGame::Init(CRender& g3dEngine)
{
	assert(g3dEngine.IsReady());
	if (!g3dEngine.IsReady())
		return false;

	if (m_Properties.m_status.ReadFlag(CGameProperty::GAME_INIT_START))
		Release();
	m_Properties.m_status.RaiseFlag(CGameProperty::GAME_INIT_START);
	
	m_pG3DEngine = &g3dEngine;
	//g_pDevice = g3dEngine.GetA3DDevice();

//	if (!AfxGetGFXExMan()->Init(g3dEngine.GetA3DDevice()))
//		return false;
//	_SGC::AfxGetSkillGfxEventMan()->Init(g3dEngine.GetA3DDevice());
	
	m_pCameraCtrl = new CCameraController(*g3dEngine.GetCamera());
	if (0 == m_pCameraCtrl)
	{
		a_LogOutput(1, "CGame::Init: Failed to create CCameraController!");
		return false;
	}

	if (!InitPhysX())
	{
		delete m_pCameraCtrl;
		m_pCameraCtrl = 0;
		return false;
	}
	
	if (!LoadSky())
	{
		ReleasePhysX();
		delete m_pCameraCtrl;
		m_pCameraCtrl = 0;
		return false;
	}

	m_pCursor = new A3DSysCursor;
	if (0 == m_pCursor)
		a_LogOutput(1, "CGame::Init: Failed to create A3DSysCursor!");
	else
	{
		if (!m_pCursor->Load(m_pG3DEngine->GetA3DDevice(), "Cursors\\normal.ani"))
		{
			a_LogOutput(1, "CGame::Init: Failed to load cursor!");
			delete m_pCursor;
			m_pCursor = 0;
		}
	}
	SetUserEvent(&gTestUE);

	IPropObjBase::SetRenderManager(m_pG3DEngine);
	m_LoadPackage.SetRender(g3dEngine);
	m_Properties.m_status.RaiseFlag(CGameProperty::GAME_INIT_ENDOK);
	return true;
}

void CGame::Release()
{
	m_Properties.m_status.ClearFlag(CGameProperty::GAME_INIT_START);
	m_Properties.m_status.ClearFlag(CGameProperty::GAME_INIT_ENDOK);
	m_Properties.SetGameMode(CGameProperty::GAME_MODE_EMPTY);
	if (0 != m_pPhysXScene)
	{
		m_pPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
		m_pPhysXScene = 0;
	}

	m_BrushesObjMgr.Release();
	//	delete m_pScenarioMgr;
	//	m_pScenarioMgr = 0;
	//	delete m_pBkGrndTask;
	//	m_pBkGrndTask = 0;


	if (0 != m_pBackMusic)
	{
		m_pBackMusic->Stop();
		m_pBackMusic->Release();
		delete m_pBackMusic;
		m_pBackMusic = 0;
	}

	if (0 != m_pCursor)
	{
		ChangeCursor(true);
		delete m_pCursor;
		m_pCursor = 0;
	}
	
	A3DRELEASE(m_pA3DSky);
	m_pG3DEngine->UnregisterSky();

	delete m_pCameraCtrl;
	m_pCameraCtrl = 0;

	//	_SGC::AfxGetSkillGfxEventMan()->Release();
	//	AfxGetGFXExMan()->Release();
	//	g_LuaStateMan.Release();

	//	g_pDevice = 0;
	m_pG3DEngine = 0;
}

bool CGame::InitPhysX()
{
	assert(0 != m_pG3DEngine);
	m_pPhysXEngine = APhysXEngine::GetPtrInstance();

	//	Initialize physical engine
	AString strPhysXPath = af_GetBaseDir();
	strPhysXPath += "\\PhysX";

	APhysXEngineDesc apxEngineDesc;
	apxEngineDesc.mPhysXCorePath = strPhysXPath;
	apxEngineDesc.mA3DEngine = m_pG3DEngine->GetA3DEngine();
	apxEngineDesc.mEnableVRD = true;

	if (!m_pPhysXEngine->Init(apxEngineDesc))
	{
		a_LogOutput(1, "CGame::InitPhysX: Failed to initialzie physical engine!");
		m_pPhysXEngine = 0;
		return false;
	}

	m_Properties.EnablePhysXHW(true);
	// turn off the debug render of physX joints and joint limits...
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 0);
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 0);
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 0);
//	gPhysXCreationDeletionReport->Register(&m_objMgr);

	//	Create physical scene
	APhysXSceneDesc apxSceneDesc;
	apxSceneDesc.mEnableDebugRender = true;
	apxSceneDesc.mNxSceneDesc.groundPlane = true;
	apxSceneDesc.mNxSceneDesc.gravity = APHYSX_STD_GRAVITY;
	apxSceneDesc.mDefaultMaterial.mRestitution = 0.3f;
	apxSceneDesc.mDefaultMaterial.mStaticFriction = 0.5f;
	apxSceneDesc.mDefaultMaterial.mDynamicFriction = 0.5f;
	apxSceneDesc.mTimeStep = 0.02f;

	m_pPhysXScene = m_pPhysXEngine->CreatePhysXScene(apxSceneDesc);
	if (0 != m_pPhysXScene)
	{
		m_Properties.SetAPhysXInfo(*m_pPhysXEngine, *m_pPhysXScene);
		m_LoadPackage.GetGFXPhysXSceneShell()->PhysXSceneSwitchOn(*m_pPhysXScene);
	}

	m_PhysXPicker.Init(m_pPhysXScene, m_pG3DEngine->GetViewport());
//	m_pScenarioMgr = new ScenarioMgr(*m_pPhysXScene->GetNxScene());
	return true;
}

void CGame::ReleasePhysX()
{
	m_PhysXPicker.Release();
	if (0 != m_pPhysXScene)
	{
		m_pPhysXScene->EndSimulate();
		m_pPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
		m_pPhysXScene = 0;
	}
}

bool CGame::LoadSky()
{
	assert(0 == m_pA3DSky);
	m_pA3DSky = new A3DSkySphere;
	if (0 == m_pA3DSky)
	{
		a_LogOutput(1, "CGame::LoadSky: Not enough memory!");
		return false;
	}

	if (!m_pA3DSky->Init(m_pG3DEngine->GetA3DEngine(), "Cloudy\\Roof.bmp", "Cloudy\\01.bmp", "Cloudy\\02.bmp"))
	{
		a_LogOutput(1, "CGame::LoadSky: Failed to create sphere sky!");
		delete m_pA3DSky;
		m_pA3DSky = 0;
		return false;
	}

	m_pA3DSky->SetFlySpeedU(0.01f);
	m_pA3DSky->SetFlySpeedV(0.01f);
	m_pG3DEngine->RegisterSky(m_pA3DSky);
	return true;
}

bool CGame::LoadMusic(const char* szFile)
{
	assert(0 == m_pBackMusic);
	m_pBackMusic = new AMSoundStream;
	if (0 == m_pBackMusic)
	{
		a_LogOutput(1, "CGame::LoadMusic: Not enough memory!");
		return false;
	}

	if (!m_pBackMusic->Init(m_pG3DEngine->GetA3DEngine()->GetAMEngine()->GetAMSoundEngine(), szFile))
	{
		a_LogOutput(1, "CGame::LoadMusic, Failed to load background music %s.", szFile);
		return false;
	}

	m_pBackMusic->Play();
	return true;
}

void CGame::LoadTerrainAndPhysXTerrain(const char* szFile)
{
	if (!m_Properties.IsReady())
	{
		AfxMessageBox(_T("请先初始化后再加载地形！"), MB_OK | MB_ICONERROR);
		return;
	}

	AString strFile(szFile);
	if (!LoadTerrain(strFile))
	{
		AfxMessageBox(_T("加载地形数据失败！"), MB_OK | MB_ICONERROR);
		return;
	}

	af_ChangeFileExt(strFile, "_ptb.cfg");
	if (!af_IsFileExist(strFile, false))
		return;

	if(!LoadPhysXTerrain(strFile))
	{
		AfxMessageBox(_T("加载PhysX地形数据失败！"), MB_OK | MB_ICONWARNING);
		return;
	}
	ChangeCursor(false);
}

bool CGame::LoadTerrain(const char* szFile)
{
	A3DTerrain2* pNewTerrain = new A3DTerrain2;
	if (0 == pNewTerrain)
	{
		a_LogOutput(1, "CGame::LoadTerrain: Not enough memory!");
		return false;
	}

	if (!pNewTerrain->Init(m_pG3DEngine->GetA3DEngine(), 512.0f, 512.0f))
	{
		a_LogOutput(1, "CGame::LoadTerrain: Failed to initialize terrain!");
		A3DRELEASE(pNewTerrain);
		return false;
	}

	if (!pNewTerrain->Load(szFile, 0.0f, 0.0f, false))
	{
		a_LogOutput(1, "CGame::LoadTerrain: Failed to load A3DTerrain2!");
		A3DRELEASE(pNewTerrain);
		return false;
	}

	pNewTerrain->SetLODType(A3DTerrain2::LOD_NOHEIGHT);

	A3DRELEASE(m_pTerrain);
	m_pTerrain = pNewTerrain;
	m_Properties.SetTerrainFileName(szFile);

	m_pTerrain->SetLoadCenter(m_pCameraCtrl->GetPos());
	m_pCameraCtrl->AddTerrainHeigt(*m_pTerrain);
	m_pG3DEngine->RegisterTerrain(m_pTerrain);
	GetCollisionSupplier()->Init(m_pTerrain, &m_BrushesObjMgr);
	return true;
}

bool CGame::LoadPhysXTerrain(const char* szFile)
{
	assert(0 != m_pPhysXScene);
	if (0 == m_pPhysXScene)
		return false;

	m_pPhysXScene->ReleaseTerrain();
	return m_pPhysXScene->InitTerrain(szFile);
}

bool CGame::GetTerrainPosByCursor(const int x, const int y, A3DVECTOR3& vOutPos)
{
	if (!m_Properties.IsReady())
		return false;

	if (0 == m_pTerrain)
		return false;

	A3DVECTOR3 vDest = A3DVECTOR3(float(x), float(y), 1.0f);
	if (!m_pG3DEngine->GetViewport()->InvTransform(vDest, vDest))
		return false;

	A3DVECTOR3 vStart = m_pCameraCtrl->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;

	RAYTRACERT rt;
	if (!m_pTerrain->RayTrace(vStart, vDelta, 1.0f, &rt))
		return false;

	vOutPos = rt.vHitPos;
	return true;
}

void CGame::ChangeCursor(const bool bIsSysDef)
{
	if (bIsSysDef)
		m_pG3DEngine->ShowCursor(0);
	else
		m_pG3DEngine->ShowCursor(m_pCursor);
}

bool CGame::Tick(const unsigned int dwDeltaTime)
{
	assert(0 != GetTerrain());

	float dtSec = dwDeltaTime * 0.001f;
	dtSec = m_pPhysXScene->GetExactSimulateTime(dtSec);
	if (dtSec < 0.0001f)
		return true;
	unsigned int dtMSec = unsigned int(dtSec * 1000); // note: only used for angelica tick, don't used for PhysX tick/sim

	MoveLight(dtMSec);

	if (!m_Properties.IsPhysXSimEnable())
	{
		m_pCameraCtrl->MoveCamera(dtSec, GetTerrain());
		return false;
	}
	m_Trigger.RegionEvents();

	A3DCamera& Camera = m_pCameraCtrl->GetCamera();
	A3DVECTOR3 vCamPos = Camera.GetPos();
	m_pTerrain->Update(dtMSec, vCamPos);

	m_BrushesObjMgr.Tick(dwDeltaTime);
	for (int k = 0; k < m_MainActors.GetSize(); k++)
		m_MainActors[k]->UpdateDynDes();

	if ((0 != m_pCurrentMA) && m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
	{
//		m_objMgr.UpdateNeighborObjects(m_pCurrentMA->GetPos(), 100, *m_pG3DEngine, *m_pPhysXScene, false, m_pBkGrndTask);
		if (m_pCurrentMA->IsKeepWalking())
		{
			int x, y;
			if (m_pCurrentMA->GetTargetPT(x, y))
			{
				A3DVECTOR3 vHitPos;
				if (GetTerrainPosByCursor(x, y, vHitPos))
					m_pCurrentMA->WalkTo(vHitPos);
			}
		}

		if (CCameraController::MODE_BINDING == m_pCameraCtrl->GetMode())
			m_pCurrentMA->ScanControlKey(Camera.GetDirH());
	}

	/*if (gTestRiveFF && (0 != pMoveActor))
	{
		NxVec3 pos = pMoveActor->getGlobalPosition();
		NxVec3 d = TargetPos - pos;
		if (d.magnitude() < 0.1f)
			pMoveActor->setLinearVelocity(NxVec3(0.0f));
	}

	if (0 != m_pScenarioMgr)
		m_pScenarioMgr->TickScenario(dtSec);
	m_MMP.TickScenario(dtSec);*/
	m_objMgr.TickMove(dtSec);
	APhysXCharacterControllerManager* pCCM = m_pPhysXEngine->GetAPhysXCCManager();
	if (0 != pCCM)
		pCCM->Tick();
	m_objMgr.TickAnimation(dtMSec);
//	const int nSize = m_MainActors.GetSize();
//	for (int i = 0; i < nSize; ++i)
//		m_MainActors[i]->TickAura();

	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())  // "OnSimulate" means in the physX state instead of edit state 
	{
		APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
		if (0 != pPhysXTerrain)
		{
			if (0 != m_pCurrentMA)
				pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(m_pCurrentMA->GetPos()));
			else
				pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(vCamPos));
		}
	}
	m_pPhysXEngine->Simulate(dtSec);
	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
		m_objMgr.SyncDataPhysXToGraphic();

	m_pG3DEngine->GetA3DEngine()->Tick(dwDeltaTime);
	m_pA3DSky->Tick(dwDeltaTime);
//	m_pA3DTerrain->Update(dwDeltaTime, g_Render.GetCamera()->GetPos());

	static const float TIME_TICKANIMATION =	0.03f;
	static float AnimTime = 0;
	if ((AnimTime += dtSec) >= TIME_TICKANIMATION)
	{
		AnimTime -= TIME_TICKANIMATION;
		Camera.UpdateEar();
	}

//	AfxGetGFXExMan()->Tick(dtMSec);
//	_SGC::AfxGetSkillGfxEventMan()->Tick(dtMSec);

	if (0 == m_pCurrentMA)
		m_pCameraCtrl->MoveCamera(dtSec, GetTerrain());
	else
		m_pCameraCtrl->MoveCamera(dtSec, GetTerrain(), m_pCurrentMA->GetDeltaMove());
	return true;
}

bool CGame::Render()
{
	if (0 == GetTerrain())
		return true;

	// key render routine...
	m_pG3DEngine->RenderStuff();

	// Render Gfx
	A3DEngine* pA3DEngine = m_pG3DEngine->GetA3DEngine();
	A3DGFXExMan* pGfxExMan = m_LoadPackage.GetGFXEngine()->GetA3DGfxExMan();
	if (0 != pGfxExMan)
	{
		A3DViewport* pA3DViewport = m_pG3DEngine->GetViewport();
		pGfxExMan->RenderAllGfxSkinModel(pA3DViewport);

		A3DDevice* pA3DDevice = m_pG3DEngine->GetA3DDevice();
		bool bAlphaBlend = pA3DDevice->GetAlphaBlendEnable();
		pA3DDevice->SetAlphaBlendEnable(true);
		A3DSkinRender* pSkinRender = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
		if (0 != pSkinRender)
			pSkinRender->Render(pA3DViewport, A3DSkinRender::RD_ALL);
		pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);

		pGfxExMan->RenderAllGfx(pA3DViewport);
	}
	
	const bool bIsDebugRender = m_Properties.IsPhysXDebugRenderEnabled();
	// render the current PhysX Terrain's range...
	if (m_Properties.IsShowTerrianGridEnabled() && (0 != m_pPhysXScene))
	{
		if (bIsDebugRender)
		{
			APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
			if ((0 != pPhysXTerrain) && (0 != m_pTerrain))
			{
				A3DVECTOR3 vProjPos = m_pCameraCtrl->GetPos();
				m_pTerrain->GetPosHeight(vProjPos, vProjPos.y);
				m_pG3DEngine->DrawPhysXTerrainRange(vProjPos, pPhysXTerrain, m_pTerrain);
			}	
		}
	}

	// render the reference frame of selected objects...
	m_Properties.GetSelGroup().DrawFrame(pA3DEngine->GetA3DWireCollector());

	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_MainActors[i]->Render(*pA3DEngine->GetA3DWireCollector(), bIsDebugRender, m_pCurrentMA);

	if (bIsDebugRender && m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
		m_BrushesObjMgr.Render(pA3DEngine->GetA3DFlatCollector());

/*	_SGC::AfxGetSkillGfxEventMan()->Render();
*/

	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
		m_pPhysXScene->DebugRender();

/*	// try to render the global axis...
	if(GetAsyncKeyState(VK_F9) & 0x8000)
	{
		NxVec3 vCamPos = APhysXConverter::A2N_Vector3(m_pCameraCtrl->GetCamera().GetPos() + 3.0f * m_pCameraCtrl->GetCamera().GetDir());
		APhysXUtilLib::DebugDrawArraw(*pA3DEngine->GetA3DWireCollector(), vCamPos, vCamPos + NxVec3(1.0f, 0, 0), 0xffff0000);
		APhysXUtilLib::DebugDrawArraw(*pA3DEngine->GetA3DWireCollector(), vCamPos, vCamPos + NxVec3(0, 1.0f, 0), 0xff00ff00);
		APhysXUtilLib::DebugDrawArraw(*pA3DEngine->GetA3DWireCollector(), vCamPos, vCamPos + NxVec3(0, 0, 1.0f), 0xff0000ff);
	}
*/
	CPoint pt;
	AString strName;
	if (m_Properties.GetHitObjectFileName(strName, &pt))
	{
		af_GetFileTitle(strName, strName);
		m_pG3DEngine->TextOut(pt.x, pt.y - 15, _TAS2WC(strName), strName.GetLength(), 0xff00ff00);
	}
	return true;
}


void CGame::OnEntryEmptyMode()
{
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EMPTY))
		return;

	//Note: we must call EndSimulate at first, if we are in the NON-Edit game mode.
	//       The reason is that the PhysX stuff maybe was released while delete/release member objects.
	if (!m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT))
	{
		if (0 != m_pPhysXScene)
			m_pPhysXScene->EndSimulate();
	}

	m_Properties.m_status.ClearFlag(CGameProperty::GAME_IS_PICK_PHYSX_STH);
	m_Properties.SetPickedObject(0);
	m_PhysXPicker.OnMouseButtonUp(0, 0);
	m_pG3DEngine->EnableDrawCameraDir(false);
	if (m_pCameraCtrl->QueryMode(CCameraController::MODE_BINDING))
		m_pCameraCtrl->EntryFreeFlyMode(true);
	m_objMgr.LeaveRuntime();
	m_Properties.GetSelGroup().LeaveRuntime();
	if (m_Properties.m_status.ReadFlag(CGameProperty::GAME_INIT_ENDOK))
		m_Properties.GetSelGroup().ReplaceWithObject(0);

	ReleaseAllObject();
	ReleaseMainActor();
	if (0 != m_pPhysXScene)
		m_pPhysXScene->Reset();

	IAPWalk::gpTerrain = 0;
	A3DRELEASE(m_pTerrain);
	m_pG3DEngine->UnregesiterTerrain();

	ChangeCursor(true);
	RawObjMgr::GetInstance()->ClearUniqueSequenceID();
}

void CGame::OnEntryEditMode()
{
	assert(0 != m_pPhysXScene);	
	if (0 == m_pPhysXScene)
		return;

	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		return;

/*	if (gTestRiveFF)
	{
		pActor = 0;
		pMoveActor = 0;
	}*/

	// Note: The following calling sequence is the most safety way.
	// No matter the type of APhysXScene is synchronism or asynchronism.
	// EndSimulate() must be called before release any PhysX objects. 
	// In addition, ReleasePhysXObj() must be called before APhysXScene.Reset();
	m_pPhysXScene->EndSimulate();
//	m_MMP.RemoveAll();
//	if (0 != m_pScenarioMgr)
//		m_pScenarioMgr->ReleaseScenario();
	m_Properties.m_status.ClearFlag(CGameProperty::GAME_IS_PICK_PHYSX_STH);
	m_Properties.SetPickedObject(0);
	m_PhysXPicker.OnMouseButtonUp(0, 0);
	m_pG3DEngine->EnableDrawCameraDir(false);
	if (m_pCameraCtrl->QueryMode(CCameraController::MODE_BINDING))
		m_pCameraCtrl->EntryFreeFlyMode(true);
//	delete m_pBkGrndTask;
//	m_pBkGrndTask = 0;
	m_objMgr.LeaveRuntime();
	m_Properties.GetSelGroup().LeaveRuntime();
	m_Trigger.LeaveRuntime();
	ReleaseMainActor();
	m_pPhysXScene->Reset(false);
	m_objMgr.WakeUp(m_LoadPackage);
//	m_objMgr.ResetPose();
//	m_bIsCloseModelClothes = false;
//	m_bIsEnableWeather = false;
//	m_bIsEnableSurrounding = false;

	// Clear any other native dynamic NxActors
/*	NxScene* pScene = m_pPhysXScene->GetNxScene();
	for (NxU32 i = 0; i < pScene->getNbActors();)
	{
		if (pScene->getActors()[i]->isDynamic())
			pScene->releaseActor(*pScene->getActors()[i]);
		else
			++i;
	}*/
}

void CGame::OnEntrySimMode()
{
	assert(0 != m_pPhysXScene);	
	if (0 == m_pPhysXScene)
		return;

	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
		return;

	m_pCameraCtrl->EntryFreeFlyMode();
	StartSimulate();
}

void CGame::OnEntryPlayMode()
{
	assert(0 != m_pPhysXScene);	
	if (0 == m_pPhysXScene)
		return;

	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
		return;

//	m_pBkGrndTask = new BackgroundTask(*m_pG3DEngine, *m_pPhysXScene);
//	if (1 > gTotalMACount)
//		gTotalMACount = 1;
	m_pCurrentMA = CreateMainActor();
	if (0 != m_pCurrentMA)
		m_CMAIdx = 0;
	m_Trigger.EnterRuntime(*m_pPhysXScene);
//	for (int i = 1; i < gTotalMACount; ++i)
//		CreateMainActor();
//	m_objMgr.UpdateNeighborObjects(m_pCurrentMA->GetPos(), 10, *m_pG3DEngine, *m_pPhysXScene, true, m_pBkGrndTask);
	m_BrushesObjMgr.PrepareStart();
	StartSimulate();
}

void CGame::StartSimulate()
{
	IAPWalk::gpTerrain = m_pTerrain;

	m_pG3DEngine->EnableDrawCameraDir(true);
	m_Properties.SetPickedObject(0);
	m_Properties.SetRayHitObject(0);
	m_Properties.GetSelGroup().EnterRuntime(*m_pPhysXScene);
	m_LoadPackage.GetGFXPhysXSceneShell()->PhysXSceneSwitchOn(*m_pPhysXScene);
	m_objMgr.EnterRuntime(*m_pPhysXScene);
//	m_objMgr.AddCMM(m_MMP);
	m_pPhysXScene->StartSimulate();

//	const int nSize = m_MainActors.GetSize();
//	for (int i = 0; i < nSize; ++i)
//		m_MainActors[i]->UpdateAura();

	// We must update terrain firstly for creating terrain in PhysX.
	// The character controller depend on the terrain.
	APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
	if ((0 != pPhysXTerrain) && (0 != m_pCameraCtrl))
		pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(m_pCameraCtrl->GetPos()));
}

IPhysXObjBase* CGame::CreateObjectAndLoad(const ObjID objType, const char* szFile)
{
	IPhysXObjBase* pObject = m_objMgr.CreateLabPhysXObject(objType);
	if (0 == pObject)
		return 0;

	IPropPhysXObjBase* pObjProps = pObject->GetProperties();
	pObjProps->SetDrivenMode(m_Properties.GetDrivenMode());
	bool IsEditMode = m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT);
	if (!pObject->LoadModel(m_LoadPackage, szFile, m_pPhysXScene, IsEditMode))
	{
		m_objMgr.ReleaseLabPhysXObject(pObject);
		return 0;
	}

	if (!IsEditMode)
		pObjProps->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	return pObject;
}

IPhysXObjBase* CGame::CreateObject(const ObjID objType, const char* szFile, const POINT& pos)
{
	IPhysXObjBase* pObject = CreateObjectAndLoad(objType, szFile);
	if (0 == pObject)
		return 0;

	A3DVECTOR3 vHitPos;
	if (GetTerrainPosByCursor(pos.x, pos.y, vHitPos))
		pObject->GetProperties()->SetPos(vHitPos);

	m_Properties.GetCmdQueue().SubmitBeforeExecution(CmdCreateObj(m_LoadPackage));
	m_Properties.m_status.RaiseFlag(CGameProperty::GAME_CREATE_AND_PICK_STH);
	m_Properties.SetRayHitObject(pObject);
	m_Properties.SetPickedObject(pObject);
	return pObject;
}

IPhysXObjBase* CGame::CreateObjectRuntime(const ObjID objType, const char* szFile, const A3DVECTOR3& pos, const bool OnGround, const bool isEnterRuntime)
{
	IPhysXObjBase* pObj = CreateObjectAndLoad(objType, szFile);
	if (0 == pObj)
		return 0;

	A3DVECTOR3 aPos = pos;
	if (OnGround)
		m_pTerrain->GetPosHeight(aPos, aPos.y);

	IPropPhysXObjBase* pProp = pObj->GetProperties();
	pProp->SetPos(aPos);
	pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
	pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	if (isEnterRuntime)
		pObj->EnterRuntime(*m_pPhysXScene);

	return pObj;
}

void CGame::ReleaseLabPhysXObject(IPhysXObjBase* pObject)
{
	if (pObject == m_Properties.GetPickedObject()) m_Properties.SetPickedObject(0);
	if (pObject == m_Properties.GetRayHitObject()) m_Properties.SetRayHitObject(0);
	if (0 != pObject)
		m_Properties.GetSelGroup().RemoveObject(*pObject);
	m_objMgr.ReleaseLabPhysXObject(pObject); 
}

void CGame::ReleaseAllObject()
{
	m_Properties.SetPickedObject(0);
	m_Properties.SetRayHitObject(0);
	m_objMgr.ReleaseObjectToEnd(0);
}

void CGame::ReleaseAllSelectedObjects()
{
	CPhysXObjSelGroup& selGroup = m_Properties.GetSelGroup();
	int nCount = selGroup.Size();
	if (0 == nCount)
		return;

	std::vector<IObjBase*>  selObjs;
	selObjs.reserve(nCount);
	IObjBase* pObject = 0;
	for (int i = 0; i < nCount; ++i)
	{
		pObject = selGroup.GetObject(i);
		assert(0 != pObject);
		selObjs.push_back(pObject);
	}

	bool bAddToCmdRecorder = true;
	if (0 != m_Properties.GetPickedObject())
	{
		if (m_Properties.m_status.ReadFlag(CGameProperty::GAME_CREATE_AND_PICK_STH))
		{
			m_Properties.m_status.ClearFlag(CGameProperty::GAME_CREATE_AND_PICK_STH);
			bAddToCmdRecorder = false;
		}
	}

	if (bAddToCmdRecorder)
	{
		m_Properties.GetCmdQueue().SubmitBeforeExecution(CmdReleaseObj(m_LoadPackage));
	}
	else
	{
		m_Properties.GetCmdQueue().CancelLastSubmit();
		m_Properties.GetCmdQueue().SubmitBeforeExecution(CmdSelReplace());
	}

	m_Properties.SetPickedObject(0);
	m_Properties.SetRayHitObject(0);
	for (int i = 0; i < nCount; ++i)
		selObjs[i]->ToDie();
	selGroup.ReplaceWithObject(0);
}

Region*	CGame::CreateRegion(const POINT& pos)
{
	Region*	pObj = m_RegionMgr.CreateRegionObject();
	if (0 == pObj)
		return 0;

	A3DVECTOR3 vHitPos;
	if (GetTerrainPosByCursor(pos.x, pos.y, vHitPos))
		pObj->GetProperties()->SetPos(vHitPos);

	m_Properties.GetCmdQueue().SubmitBeforeExecution(CmdCreateObj(m_LoadPackage));
	m_Properties.m_status.RaiseFlag(CGameProperty::GAME_CREATE_AND_PICK_STH);
	m_Properties.SetRayHitObject(pObj);
	m_Properties.SetPickedObject(pObj);
	return pObj;
}

void CGame::ReleaseRegion(Region* pObject)
{
	if (pObject == m_Properties.GetPickedObject()) m_Properties.SetPickedObject(0);
	if (pObject == m_Properties.GetRayHitObject()) m_Properties.SetRayHitObject(0);
	if (0 != pObject)
		m_Properties.GetSelGroup().RemoveObject(*pObject);
	m_RegionMgr.ReleaseRegionObject(pObject); 
}

void CGame::OnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EMPTY))
		return;
//	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
//	{
//		m_PhysXPicker.OnMouseButtonDown(x, y);
//		return;
//	}
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
	{
		if (m_PhysXPicker.OnMouseButtonDown(x, y))
			m_Properties.m_status.RaiseFlag(CGameProperty::GAME_IS_PICK_PHYSX_STH);
		else
		{
			m_Properties.m_status.ClearFlag(CGameProperty::GAME_IS_PICK_PHYSX_STH);
			A3DVECTOR3 vHitPos;
			if (GetTerrainPosByCursor(x, y, vHitPos))
			{
				if (!m_pCurrentMA->IsInAir())
				{
					m_pCurrentMA->WalkTo(vHitPos);
					m_pCurrentMA->UpdateTargetPT(x, y);
					m_pCurrentMA->SetLButtonDown();
				}
			}
		}
		return;
	}
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT))
	{
		CmdQueue& cmdRecorder = m_Properties.GetCmdQueue();
		if (0 != m_Properties.GetPickedObject())
		{
			if (m_Properties.m_status.ReadFlag(CGameProperty::GAME_CREATE_AND_PICK_STH))
				m_Properties.m_status.ClearFlag(CGameProperty::GAME_CREATE_AND_PICK_STH);
			else
				cmdRecorder.SubmitBeforeExecution(CmdPropChange());
			m_Properties.SetPickedObject(0);
			if (MK_SHIFT & nFlags)
				CloneObjects();
		}
		else
		{
			CPhysXObjSelGroup& selGroup = m_Properties.GetSelGroup();
			if (selGroup.UpdateRefFrameOnLButtonDown(x, y, nFlags))
			{
				cmdRecorder.SubmitBeforeExecution(CmdPropChange());
				return;
			}

			IObjBase* pObjRayHit = m_Properties.GetRayHitObject();
			if (0 != pObjRayHit)
			{
				if (MK_CONTROL & nFlags)
				{
					cmdRecorder.SubmitBeforeExecution(CmdSelAppend());
					selGroup.AppendObject(*pObjRayHit);
					return;
				}
				if (MK_SHIFT & nFlags)
				{
					m_Properties.SetPickedObject(pObjRayHit);
					return;
				}
			}

			if (selGroup.GetFocusObject() != pObjRayHit)
				cmdRecorder.SubmitBeforeExecution(CmdSelReplace());
			selGroup.ReplaceWithObject(pObjRayHit);
		}
	}
}

void CGame::OnLButtonUp(const int x, const int y, const unsigned int nFlags)
{
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EMPTY))
		return;
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		return;

//	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
//	{
//		m_PhysXPicker.OnMouseButtonUp(x, y);
//		return;
//	}
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
	{
		m_Properties.m_status.ClearFlag(CGameProperty::GAME_IS_PICK_PHYSX_STH);
		m_PhysXPicker.OnMouseButtonUp(x, y);
		m_pCurrentMA->SetLButtonUp();
	}
}
/*
void CGame::OnMButtonUp(const int x, const int y, const unsigned int nFlags)
{
	switch(GetGameMode())
	{
	case CGame::GAME_MODE_SIMULATE:
	case CGame::GAME_MODE_PLAY:	
		AddForce(x, y, false);
		m_pG3DEngine->ClearForceBar();
		break;
	default:
		break;
	}
}
*/

void CGame::OnMouseMove(const int x, const int y, const unsigned int nFlags)
{
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EMPTY))
		return;
//	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
//	{
//		m_PhysXPicker.OnMouseMove(x, y);
//		return;
//	}
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
	{
		if (m_Properties.m_status.ReadFlag(CGameProperty::GAME_IS_PICK_PHYSX_STH))
			m_PhysXPicker.OnMouseMove(x, y);
		else
		{
			if (MK_LBUTTON & nFlags)
				m_pCurrentMA->UpdateTargetPT(x, y);
		}
		return;
	}
	if (m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT))
	{
		CPhysXObjSelGroup& selGroup = m_Properties.GetSelGroup();
		// drag and drop the object's reference frame
		if (MK_LBUTTON & nFlags)
		{
			selGroup.UpdateRefFrameOnMouseMove(x, y, nFlags, false);
			return;
		}
		// move the picked object position 
		IObjBase* pObjPicked = m_Properties.GetPickedObject();
		if (0 != pObjPicked)
		{
			A3DVECTOR3 vHitPos;
			if (GetTerrainPosByCursor(x, y, vHitPos))
			{
				assert(0 != pObjPicked->GetProperties()->GetSelGroup());
				selGroup.SetGroupPos(vHitPos);
			}
			return;
		}
		// hit object detection 
		PhysRayTraceHit hitInfo;
		PhysRay ray = ScreenPosToRay(*m_pG3DEngine->GetViewport(), x, y);
		if (RayTraceObject(CPoint(x, y), ray, hitInfo))
			selGroup.UpdateRefFrameOnMouseMove(x, y, nFlags, true);
	}
}

bool CGame::OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags)
{
	if (!m_Properties.QueryGameMode(CGameProperty::GAME_MODE_PLAY))
		return false;

	if (!m_pCameraCtrl->QueryMode(CCameraController::MODE_BINDING))
		return false;

	m_pCameraCtrl->UpdateFocusFactor(zDelta, GetTerrain());
	return true;
}

bool CGame::RayTraceObject(const CPoint& pt, const PhysRay& ray, PhysRayTraceHit& hitInfo)
{
	bool rtnObj = m_objMgr.RayTraceObj(ray, hitInfo);
	bool rtnRegion = m_RegionMgr.RayTraceObj(ray, hitInfo);

	IObjBase* pNewHit = static_cast<IObjBase*>(hitInfo.UserData);
	m_Properties.SetRayHitObject(pNewHit, pt);
	return (rtnObj || rtnRegion);
}

void CGame::CloneObjects()
{
	if (!m_Properties.QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		return;

	CPoint pt;
	IObjBase* pObjRayHit = m_Properties.GetRayHitObject(&pt);
	if (0 == pObjRayHit)
		return;

	IPropObjBase* pHitObjProp = pObjRayHit->GetProperties();
	CPhysXObjSelGroup* pSelGroup = pHitObjProp->GetSelGroup();
	m_Properties.GetCmdQueue().SubmitBeforeExecution(CmdCreateObj(m_LoadPackage));

	std::vector<IObjBase*> sampleObjs;
	if (0 != pSelGroup)
		pSelGroup->GetAllObjects(sampleObjs);
	else
		sampleObjs.push_back(pObjRayHit);

	IPhysXObjBase* pCloneObj = 0;
	IPropObjBase* pSampleProp = 0;
	IPropObjBase* pCloneProp = 0;
	std::vector<IObjBase*> cloneObjs;
	const size_t nCount = sampleObjs.size();
	for (unsigned int i = 0; i < nCount; ++i)
	{
		pSampleProp = sampleObjs[i]->GetProperties();
		pCloneObj = m_objMgr.CreateLabPhysXObject(ObjID(pSampleProp->GetObjType()));
		if (0 == pCloneObj)
		{
			a_LogOutput(1, "CGame::CloneObjects: Create raw object fail!");
			continue;
		}

		pCloneProp = pCloneObj->GetProperties();
		*pCloneProp = *pSampleProp;
		if (!pCloneObj->WakeUp(m_LoadPackage))
		{
			a_LogOutput(1, "CGame::CloneObjects: Wake up the clone object fail!");
			m_objMgr.ReleaseLabPhysXObject(pCloneObj); 
			continue;
		}
		cloneObjs.push_back(pCloneObj);
	}

	if (0 < cloneObjs.size())
	{
		IObjBase* pNewObj = cloneObjs[0];
		m_Properties.m_status.RaiseFlag(CGameProperty::GAME_CREATE_AND_PICK_STH);
		m_Properties.SetPickedObject(pNewObj, false);
		m_Properties.SetRayHitObject(pNewObj, pt);
	}
	m_Properties.GetSelGroup().ReplaceWithObjects(cloneObjs);
}

void CGame::SavePhysXDemoScene(const char* szFilePath)
{
	A3DCamera* pCamera = m_pG3DEngine->GetCamera();
	assert(0 != pCamera);
	if (0 == pCamera)
		return;

	APhysXUserStream outStream(szFilePath, false);
	m_Properties.SaveToFile(outStream, *pCamera);
}

bool CGame::LoadPhysXDemoScene(const char* szFilePath)
{
	if (!m_Properties.IsReady())
	{
		AfxMessageBox(_T("请先初始化后再读取文件！"), MB_OK | MB_ICONERROR);
		return false;
	}
	m_Properties.SetGameMode(CGameProperty::GAME_MODE_EMPTY);

	A3DCamera* pCamera = m_pG3DEngine->GetCamera();
	assert(0 != pCamera);

	bool bIsLowVersion = false;
	APhysXUserStream inStream(szFilePath, true);
	if (!m_Properties.LoadFromFile(inStream, *pCamera, bIsLowVersion))
	{
		AfxMessageBox(_T("无法识别的文件版本！"), MB_OK | MB_ICONERROR);
		return false;
	}

	m_Properties.SetActiveSceneFile(szFilePath);
	bool bCheckUpdated = m_objMgr.CheckDefPropsUpdate(m_LoadPackage, *m_pPhysXScene);
	if (bIsLowVersion || bCheckUpdated)
		SavePhysXDemoScene(szFilePath);

	m_objMgr.WakeUp(m_LoadPackage);

//	m_bIsCloseModelClothes = false;
//	m_bIsEnableWeather = false;
//	m_bIsEnableSurrounding = false;
	return true;
}

void CGame::SetUserEvent(IUserEvent* pUserEvent)
{
	m_Trigger.InitUserEvent(pUserEvent);
}

void CGame::ChangeCameraMode()
{
	if (m_pCameraCtrl->QueryMode(CCameraController::MODE_BINDING))
	{
		m_pCameraCtrl->EntryFreeFlyMode();
		return;
	}

	if (0 != m_pCurrentMA)
		m_pCameraCtrl->EntryBindingMode(m_pCurrentMA->GetPos(), GetTerrain());
}

void CGame::JumpMainActor() const
{
	if (0 != m_pCurrentMA)
	{
		if (GetAsyncKeyState(VK_SHIFT))
		{
			APhysXDynCharCtrler* pDCC = m_pCurrentMA->GetAPhysXDynCC();
			if (0 != pDCC)
			{
				pDCC->HangEnd();
				return;
			}
		}
		m_pCurrentMA->Jump(6.0f);
	}
}

void CGame::SwitchMainActor()
{
	const int nCount = m_MainActors.GetSize(); 
	if (1 > nCount)
		return;

	++m_CMAIdx;
	if (nCount <= m_CMAIdx)
		m_CMAIdx = 0;

	CMainActor* pNewMA = m_MainActors[m_CMAIdx];
	if (0 != pNewMA)
	{
		m_pCurrentMA = pNewMA;
		m_pCameraCtrl->EntryBindingMode(m_pCurrentMA->GetPos(), GetTerrain(), true);
	}
}

void CGame::SwitchMainActorWalkRun() const
{
	if (0 == m_pCurrentMA)
		return;

	bool isRuning = m_pCurrentMA->IsRunEnable();
	isRuning = !isRuning; 
	m_pCurrentMA->EnableRun(isRuning);
}

CMainActor*	CGame::CreateMainActor()
{
	bool bIsFirst = false;
	if (0 == m_MainActors.GetSize())
		bIsFirst = true;

	A3DVECTOR3 pos;
	if (bIsFirst && !m_pCameraCtrl->EntryDefBindingMode(pos, *GetTerrain()))
		return 0;

	if (!bIsFirst)
		pos = IAPWalk::GetRandomPos(5, m_pCameraCtrl->GetPos());

	IPhysXObjBase* pObj = CreateObjectRuntime(RawObjMgr::OBJ_TYPEID_ECMODEL, "Models\\Players\\形象\\女\\躯干\\人族女.ecm3", pos, true, false);
	if (0 == pObj)
	{
		if (bIsFirst)
			m_pCameraCtrl->EntryFreeFlyMode(true);
		return 0;
	}
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
	if (0 == pDynObj)
		return 0;
	
	pDynObj->GetProperties()->SetRoleType(CDynProperty::ROLE_ACTOR_AVATAR);
//	if (gEnableDynamicAPhysXCC)
//	{
//		pObj->RaisePeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC);
//	}
//	if (!gDefaultAPhysXCCIsOpen)
//	{
//		pObj->ClearPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_CC);
//		pObj->ClearPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC);
//	}
	pObj->GetProperties()->SetDirAndUp(m_pCameraCtrl->GetCamera().GetDirH(), g_vAxisY);
//	pDynObj->ApplyPhysXBody(gEnableMainActorPhysXBody);

/*	if (!m_strFFBindToMA.IsEmpty())
	{
		pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD, m_strFFBindToMA, pos, false, false);
		if (0 != pObj)
		{
			NxMat34 localpose;
			localpose.id();
			localpose.t.set(0, pDynObj->GetHeight() * 0.5f, 0);
			pDynObj->SetAssociateObj(pObj, &localpose);
		}
	}
*/
	CMainActor* pMainActor = new CMainActor(*pDynObj);
	m_MainActors.Add(pMainActor);
	return pMainActor;
}

void CGame::ReleaseMainActor()
{
	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
		delete m_MainActors[i];

	m_MainActors.RemoveAll();
	m_pCurrentMA = 0;
	m_CMAIdx	 = -1;
}

const TCHAR* CGame::GetCurrentMainActorCCType() const
{
	if (0 == m_pCurrentMA)
		return 0;

	return m_pCurrentMA->GetCCTypeText();
}

void CGame::SwitchMainActorCCType(CAPhysXCCMgr::CCType ct)
{
	if (0 != m_pCurrentMA)
		m_pCurrentMA->SwitchCCType(ct);
}

/*
bool CGame::RayTraceObject(const PhysRay& ray, APhysXRayHitInfo& hitInfo)
{
	NxRay nxRay;
	nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
	nxRay.dir  = APhysXConverter::A2N_Vector3(ray.vDelta);
	nxRay.dir.normalize();
	if (!APhysXPicker::Raycast(m_pPhysXScene, nxRay, hitInfo, APX_RFTYPE_ALL))
	{
		m_Properties.SetRayHitObject(0);
		return false;
	}

	IPhysXObjBase* pObj = m_objMgr.GetObject(*hitInfo.mActor);
	m_Properties.SetRayHitObject(pObj);
	return (0 == pObj)? false : true;
}

float CGame::GetForceValue(const float mass, const bool bIsRandom) const
{
	static const int nMin = 0;
	static const int nMax = 600;

	if (bIsRandom)
		return a_Random(nMin, nMin + nMax) * mass;

	float perF = m_pG3DEngine->GetForcePercent();
	a_ClampFloor(perF, 0.0f);
	a_ClampRoof(perF, 1.0f);
	return (nMin + nMax * perF) *  mass;
	// return (nMin + nMax * perF);
}

bool CGame::AddForce(const int x, const int y, const bool bIsRandomForce)
{
	static const float fMaxDist = 50;
	if (0 == m_pPhysXScene)
		return false;
	NxScene* pScene = m_pPhysXScene->GetNxScene();
	if (0 == pScene)
		return false;

	NxRay nxRay;
	PhysRay ray = ScreenPosToRay(*m_pG3DEngine->GetViewport(), x, y);
	nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
	nxRay.dir  = APhysXConverter::A2N_Vector3(ray.vDelta);
	nxRay.dir.normalize();
	
	NxRaycastHit nxHit;
	// we should filter out the Attacher-Type Object...
	APhysXU32 uAttacherGroup = ~((1 << APX_COLLISION_GROUP_ATTACHER) | (1 << APX_COLLISION_GROUP_CLOTH_ATTACHER ));
	NxGroupsMask gm = m_pPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
	gm.bits0 = ~gm.bits0;
	gm.bits1 = ~gm.bits1;
	gm.bits2 = ~gm.bits2;
	gm.bits3 = ~gm.bits3;
	NxShape* pHitShape = pScene->raycastClosestShape(nxRay, NX_DYNAMIC_SHAPES, nxHit, uAttacherGroup, fMaxDist, 0xffffffff, &gm);
	if (0 == pHitShape)
		return false;
	NxActor& hitActor = pHitShape->getActor();

	IPhysXObjBase* pObj = m_objMgr.GetObject(hitActor);
	if (0 == pObj)
		return false;
	if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_IS_MAIN_ACTOR))
		return false;

	A3DVECTOR3 hitPos = APhysXConverter::N2A_Vector3(nxHit.worldImpact);
	A3DVECTOR3 posForce = APhysXConverter::N2A_Vector3(nxHit.worldImpact - nxRay.dir * 0.2f);
	APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)(hitActor.userData); 
	APHYSX_ASSERT(pRBObj);
	float valForce = GetForceValue(pRBObj->GetMass(), bIsRandomForce);

	if(0 != m_pCurrentMA)
	{
		m_pCurrentMA->Attack(*pObj, hitPos, valForce, &hitActor);
	}
	else
	{
		ApxHitForceInfo apxHitForceInfo;
		apxHitForceInfo.mHitDir = APhysXConverter::N2A_Vector3(nxRay.dir);
		apxHitForceInfo.mHitForceMagnitude = valForce;
		apxHitForceInfo.mHitPos = hitPos;
		pObj->OnHitbyForce(apxHitForceInfo, &hitActor);
	}

	ACString str;
	str.Format(_T("Force: %.0f"), valForce);
	m_pG3DEngine->TextOutAnimation(x, y, str, 0xff00ff00);
	return true;

	// Here is another way to call AddForce().
	// DrivenMode dmOldVal = CPhysXObjECModel->GetDrivenMode();
	// CPhysXObjECModel->SetDrivenMode(IPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
	// CPhysXObjECModel->AddForce(......);
	// CPhysXObjECModel->SetDrivenMode(dmOldVal);

	// In this way, we can't get a proper force base the mass. And further more, 
	// the visual effect are also not good. It appears a slight pause if AddForce() return false. 
	// Due to StopAllActions() was called in the internal of EC Model. 
	// So, we didn't add force with this method.
}

void CGame::AddForceInTheArea()
{
	int xCenter = 0;
	int yCenter = 0;
	int HalfSideLen = 0;
	if (!m_pG3DEngine->GetFrontSight(xCenter, yCenter, HalfSideLen))
		return;

	int x = a_Random(xCenter - HalfSideLen, xCenter + HalfSideLen);
	int y = a_Random(yCenter - HalfSideLen, yCenter + HalfSideLen);
	if (AddForce(x, y, true))
		m_pG3DEngine->ClearFrontSight();
}

void CGame::ChangeECModelPhysXState() const
{
	static bool state = false;
	
	int nMax = m_objMgr.SizeModels();
	for (int i = 0; i < nMax; ++i)
	{
		IPhysXObjBase* pObj = m_objMgr.GetObject(i);
		if (CPhysXObjMgr::OBJ_TYPEID_ECMODEL != pObj->GetObjType())
			continue;
		
		CPhysXObjECModel* pObjAim = static_cast<CPhysXObjECModel*>(pObj);
		pObjAim->SetPhysSystemState(state);
	}
	
	state = !state;
}

CPhysXObjECModel* CGame::GetNextECModel_NonShot() const 
{
	static int currentIdx = -1;

	const int nMax = m_objMgr.SizeModels();
	if (0 == nMax)
		return 0;

	++currentIdx;
    int start = currentIdx;
	if (nMax <= start)
		start = 0;

	CPhysXObjECModel* pObjEC = 0; 
	do 
	{
		if (nMax <= currentIdx)
			currentIdx = 0;
		
		IPhysXObjBase* pObj = m_objMgr.GetObject(currentIdx);
		if (CPhysXObjMgr::OBJ_TYPEID_ECMODEL == pObj->GetObjType())
			return static_cast<CPhysXObjECModel*>(pObj);

		++currentIdx;
	} while (currentIdx != start);
	return 0;
}


void CGame::SwitchCCCollision() const
{
	if (0 == m_pPhysXEngine)
		return;

	APhysXCharacterControllerManager* pCCM = m_pPhysXEngine->GetAPhysXCCManager();
	if (0 == pCCM)
		return;

	APhysXCCCollisionFlag flag = pCCM->GetInteractFlag();
	if (APX_CCCF_COLLIDE_ALL_CHANNEL == flag)
	{
		pCCM->SetInteractFlag(APX_CCCF_FILTERING_REG_CHANNEL);
		OutputDebugString(_T("---CC filter register channel---\n"));
	}
	else
	{
		pCCM->SetInteractFlag(APX_CCCF_COLLIDE_ALL_CHANNEL);
		OutputDebugString(_T("---CC collide with all channel---\n"));
	}
}

void CGame::OnKeyDownScenarioMgr(const unsigned int nChar)
{
	if ((0 == m_pCurrentMA) || (0 == m_pScenarioMgr))
		return;
	
	APhysXCollisionChannelManager* pCCM = m_pPhysXScene->GetCollisionChannelManager();
	if (0 == pCCM)
		return;
	
	NxGroupsMask gm = pCCM->GetChannelGroupMask(m_pPhysXScene->GetDefaultCollisionChannel());
	NxVec3 pos = APhysXConverter::A2N_Vector3(m_pCurrentMA->GetPos());
	pos.y += 1;

	switch (nChar)
	{
	case 'K':
		m_pScenarioMgr->ReleaseScenario();
		m_pScenarioMgr->SelectNext();
		m_pScenarioMgr->CreateScenario(*this, pos, &gm);
		break;
	case 'L':
		m_pScenarioMgr->ReleaseScenario();
		break;
	default:
		m_pScenarioMgr->ControlScenario(nChar);
		m_MMP.ControlScenario(nChar);
	}
}

void CGame::ControlModelClothes()
{
	m_bIsCloseModelClothes = !m_bIsCloseModelClothes;
	m_objMgr.ControlClothes(m_bIsCloseModelClothes);
}
*/
/*void CGame::OnKeyDownEditScenario()
{
	A3DCamera* pCamera = m_pG3DEngine->GetCamera();
	A3DVECTOR3 centerPos = pCamera->GetPos() + 5 * pCamera->GetDir();
	A3DVECTOR3 pos;
	IPhysXObjBase* pObj = 0;
*/
	// Create CC demo stuff
//}
/*
void CGame::EnterTopView()
{
	APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
	if (0 != pPhysXTerrain)
	{
		A3DCamera& Camera = m_pCameraCtrl->GetCamera();
		A3DVECTOR3 vCamProjPos = Camera.GetPos();
		float h = m_pTerrain->GetPosHeight(vCamProjPos);
		vCamProjPos.y = h + 80.0f;

		Camera.SetPos(vCamProjPos);
		Camera.SetDeg(-90);
		Camera.SetPitch(-90);
	}
}

void CGame::ControlWeatherSurrounding()
{
	if (GetAsyncKeyState(VK_SHIFT))
	{
		m_bIsEnableSurrounding = !m_bIsEnableSurrounding;
	//	m_objMgr.ControlSurrounding(m_bIsEnableSurrounding);
	}
	else
	{
		m_bIsEnableWeather = !m_bIsEnableWeather;
		m_objMgr.ControlWeather(m_bIsEnableWeather);
	}
}

void CGame::TestAndDebug(HWND hWnd, bool fixed)
{
	POINT pt;
	if (!GetCursorPos(&pt))
		return;
	if (!ScreenToClient(hWnd, &pt))
		return;

	NxVec3 pos(-328.4f, 35, 211.6f);
	if (!fixed)
	{
		PhysRay ray;
		APhysXRayHitInfo hitInfo;
		ray = ScreenPosToRay(*m_pG3DEngine->GetViewport(), pt.x, pt.y);
		NxRay nxRay;
		nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
		nxRay.dir  = APhysXConverter::A2N_Vector3(ray.vDelta);
		nxRay.dir.normalize();
		if (!APhysXPicker::Raycast(m_pPhysXScene, nxRay, hitInfo, APX_RFTYPE_ALL))
		{
			A3DVECTOR3 mouseHitPos;
			if (!GetTerrainPosByCursor(pt.x, pt.y, mouseHitPos))
				return;
			hitInfo.mHitPos = APhysXConverter::A2N_Vector3(mouseHitPos);
		}
		pos.set(hitInfo.mHitPos.x, hitInfo.mHitPos.y + 4, hitInfo.mHitPos.z);
	}

	if (0 == pMoveActor)
	{
		NxMaterial* defaultMaterial = m_pPhysXScene->GetNxScene()->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(0);
		
		NxBodyDesc descBody;
		descBody.flags |= NX_BF_DISABLE_GRAVITY;
		NxActorDesc actorDesc;
		actorDesc.body = &descBody;
		actorDesc.density = 1;
		actorDesc.globalPose.t.set(pos);
		NxCapsuleShapeDesc capDesc;
		capDesc.radius = 0.05f;
		capDesc.height = 0.05f;
		//	APhysXCollisionChannelManager* pCCM = m_pPhysXScene->GetCollisionChannelManager();
		//	capDesc.groupsMask = pCCM->GetChannelGroupMask(APX_COLLISION_CHANNEL_COMMON);
		actorDesc.shapes.pushBack(&capDesc);
		pMoveActor = m_pPhysXScene->GetNxScene()->createActor(actorDesc);
		
		IPhysXObjBase* pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD, "ForceField\\Rive.xff", NxVec3(0.0f), false, true);
		if (0 != pObj)
		{
			pObj->RaiseFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
			CPhysXObjForceField* pFF = static_cast<CPhysXObjForceField*>(pObj);
			pFF->AttachTo(pMoveActor);
		}
	}

	if (0 == pActor)
	{
		NxMaterial* defaultMaterial = m_pPhysXScene->GetNxScene()->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(0);
		
		NxBodyDesc descBody;
		descBody.flags |= NX_BF_FROZEN_ROT;
		NxActorDesc actorDesc;
		actorDesc.body = &descBody;
		actorDesc.density = 1;
		NxQuat rot(90, NxVec3(1, 0, 0));
		actorDesc.globalPose.M.fromQuat(rot);
		actorDesc.globalPose.t.set(pos);
		NxCapsuleShapeDesc capDesc;
		capDesc.radius = 0.05f;
		capDesc.height = 0.05f;
		
	//	APhysXCollisionChannelManager* pCCM = m_pPhysXScene->GetCollisionChannelManager();
	//	capDesc.groupsMask = pCCM->GetChannelGroupMask(APX_COLLISION_CHANNEL_COMMON);
		actorDesc.shapes.pushBack(&capDesc);
		pActor = m_pPhysXScene->GetNxScene()->createActor(actorDesc);

		IPhysXObjBase* pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD, "ForceField\\Rive.xff", NxVec3(0.0f), false, true);
		if (0 != pObj)
		{
			pObj->RaiseFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
			CPhysXObjForceField* pFF = static_cast<CPhysXObjForceField*>(pObj);
			pFF->AttachTo(pActor);
		}
	}
	else
	{
		pActor->setLinearVelocity(NxVec3(0.0f));
		pActor->setAngularVelocity(NxVec3(0.0f));
		NxQuat rot(90, NxVec3(1, 0, 0));
		NxMat34 mat;
		mat.id();
		mat.M.fromQuat(rot);
		mat.t.set(pos);
		pActor->setGlobalPose(mat);
	}
	*/
	/*	// debug for bridge in xajh
	if (0 == m_pCurrentMA)
		return;

	static bool IsFirst = true;
	static NxVec3 p0, p1;
	NxVec3 t;

	if (IsFirst)
	{
		NxScene* pScene = m_pPhysXScene->GetNxScene();
		NxActor** ppActors = pScene->getActors();
		NxU32 nb = pScene->getNbActors();
		for (NxU32 i = 0; i < nb; ++i)
		{
			NxActor* pActor = ppActors[i];
			t= pActor->getGlobalPosition();
			if ((243 > t.z) || (247 < t.z))
				continue;
			
			const char* pName = pActor->getName();
			if (!pName) continue;
//			int r0 = strcmp("Bone07Actor", pName);
//			int r1 = strcmp("Bone08Actor", pName);
			int r0 = strcmp("KActorB9", pName);
			int r1 = strcmp("KActorB8", pName);
			if (0 == r0) p0 = t;
			if (0 == r1) p1 = t;
		}
		IsFirst = false;
	}

	t = m_pCurrentMA->GetAPhysXCC()->GetFootPosition();
	bool IsToP1 = true;
	if (NxMath::equals(t.x - p1.x, 0.0f, 0.1f) && NxMath::equals(t.z - p1.z, 0.0f, 0.1f))
	{
		IsToP1 = false;
	}
	else
	{
		NxVec3 pos = p0;
		pos.y += 0.4;
		m_pCurrentMA->GetAPhysXCC()->SetFootPosition(pos);
	}

	if (IsToP1)
		m_pCurrentMA->WalkTo(APhysXConverter::N2A_Vector3(p1));
	else
		m_pCurrentMA->WalkTo(APhysXConverter::N2A_Vector3(p0));
	*/
//}
/*
void CGame::ShowFFShapes(bool show)
{
	if (m_bShowFFShapes == show)
		return;

	m_bShowFFShapes = show;
	IPhysXObjBase* pObjRayHit = m_Properties.GetRayHitObject();

	int nCount = m_objMgr.SizeModels();
	for (int i = 0; i < nCount; ++i)
	{
		IPhysXObjBase* pObj = m_objMgr.GetObject(i);
		if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType())
			continue;

		if (pObjRayHit == pObj)
			continue;

		if (m_bShowFFShapes)
			pObj->RaiseFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
		else
			pObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
	}
}

void CGame::ReloadFFFile(const AString& theFile)
{
	if (theFile.IsEmpty())
		return;

	if (GAME_MODE_EDIT != GetGameMode())
		return;

	int nCount = m_objMgr.SizeModels();
	for (int i = 0; i < nCount; ++i)
	{
		IPhysXObjBase* pObj = m_objMgr.GetObject(i);
		if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType())
			continue;
		
		AString objFile = pObj->GetFilePathName();
		if (objFile != theFile)
			continue;

		CPhysXObjForceField* pFF = static_cast<CPhysXObjForceField*>(pObj);
		pFF->ReloadModel();
	}
}

void CGame::SetMABindingFF(const AString& ff)
{
	m_strFFBindToMA = "ForceField\\";
	m_strFFBindToMA += ff;
	m_strFFBindToMA += ".xff";
}

void CGame::GetMABindingFF(AString& outFF) const
{
	outFF.Empty();
	outFF = m_strFFBindToMA;
	int right = outFF.Find(_T('.'));
	if (0 < right) outFF = outFF.Left(right);
	int left = outFF.Find(_T('\\'));
	if (0 < left) outFF = outFF.Right(outFF.GetLength() - left - 1);
}

void CGame::TrytoHang()
{
	if (0 == m_pCurrentMA)
		return;

	APhysXDynCharCtrler* pDynCC = m_pCurrentMA->GetAPhysXDynCC();
	if (0 == pDynCC)
		return;

	m_pCurrentMA->WalkTo(m_pCurrentMA->GetPos());
	NxVec3 ccDir = pDynCC->GetHorizonDir();
	NxVec3 vNewDir = APhysXUtilLib::Slerp(ccDir, NxVec3(0, 1, 0), 0.6666667f);
	pDynCC->HangStart(vNewDir);
}
*/

void CGame::ShootAPhysXObject(HWND hWnd, const float scale, const float fVelocity)
{
	if (!m_Properties.IsRuntime())
		return;

	if (0 == m_pPhysXScene)
		return;

	CGameProperty::ShootMode iSMode = m_Properties.GetShootMode();
	CGameProperty::ShootStuffID iSSID = m_Properties.GetShootType();

	PhysRay ray;
	APhysXRayHitInfo hitInfo;
	if ((CGameProperty::SM_MOUSE_FALLING == iSMode) || (CGameProperty::SM_EYE_TO_MOUSE == iSMode))
	{
		POINT pt;
		if (!GetCursorPos(&pt))
			return;
		if (!ScreenToClient(hWnd, &pt))
			return;

		ray = ScreenPosToRay(*m_pG3DEngine->GetViewport(), pt.x, pt.y);
		if (CGameProperty::SM_EYE_TO_MOUSE == iSMode)
			ray.vDelta.Normalize();
		else
		{
			NxRay nxRay;
			nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
			nxRay.dir  = APhysXConverter::A2N_Vector3(ray.vDelta);
			nxRay.dir.normalize();
			if (!APhysXPicker::Raycast(m_pPhysXScene, nxRay, hitInfo, APX_RFTYPE_ALL))
			{
				A3DVECTOR3 mouseHitPos;
				if (!GetTerrainPosByCursor(pt.x, pt.y, mouseHitPos))
					return;
				hitInfo.mHitPos = APhysXConverter::A2N_Vector3(mouseHitPos);
			}
		}
	}

	IPhysXObjBase* pObj = m_objMgr.CreateLabPhysXObject(GetObjType(iSSID), true);
	if (0 == pObj)
		return;

	if (!pObj->LoadModel(m_LoadPackage, GetObjName(iSSID), 0, false))
	{
		m_objMgr.ReleaseLabPhysXObject(pObj);
		return;
	}

	A3DCamera* pA3DCamera = m_pG3DEngine->GetCamera();
	A3DVECTOR3 vEyeDir = pA3DCamera->GetDir();
	A3DVECTOR3 vEyePos = pA3DCamera->GetPos();

	A3DVECTOR3 pos = vEyePos;
	NxVec3 vel = fVelocity * APhysXConverter::A2N_Vector3(vEyeDir);
	if (CGameProperty::SM_EYE_TO_MOUSE == iSMode)
		vel = fVelocity * APhysXConverter::A2N_Vector3(ray.vDelta);
//	else if (SM_KINEMATIC == iSMode)
//		pos += pA3DCamera->GetDir() * 5;
	else if (CGameProperty::SM_FRONT_FALLING == iSMode)
	{
		vel.zero();
		pos += pA3DCamera->GetDir() * 10;
		float y = pos.y;
		m_pTerrain->GetPosHeight(pos, y);
		if (y > pos.y) pos.y = y + 2; 
	}
	else if (CGameProperty::SM_MOUSE_FALLING == iSMode)
	{
		vel.zero();
		pos = APhysXConverter::N2A_Vector3(hitInfo.mHitPos);
		pos.y += 4;
	}

	IPropPhysXObjBase* pProp = pObj->GetProperties();
	pProp->SetPos(pos);
	pProp->SetScale(scale);
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	//	pObj->TickAnimation(0);  // must update internal pose data if using EC_Model with hardLink PhysX 
	pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);
	pObj->EnterRuntime(*m_pPhysXScene);

	if (RawObjMgr::OBJ_TYPEID_FORCEFIELD != pProp->GetObjType())
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		APhysXSkeletonRBObject* pPSRB =  (0 == pDynObj)? 0 : pDynObj->GetSkeletonRBObject();
		if(0 != pPSRB)
		{
			pPSRB->SetLinearVelocity(vel);
		/*	if (SM_KINEMATIC == m_iSMode)
			{
				const int nCount = pPSRB->GetNxActorNum();
				NxActor* pActor = 0;
				for (int j = 0; j < nCount; ++j)
				{
					pActor = pPSRB->GetNxActor(j);
					if (pActor->isDynamic())
						pActor->raiseBodyFlag(NX_BF_KINEMATIC);
				}
			}*/
		}
	}
}

void CGame::MoveLight(DWORD dwTime)
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		return;

	float fDeltaTime = dwTime * 0.001f;
	float fRotSpeed = 90.0f * fDeltaTime;	//	90 degree / second
	float fRot1 = 0.0f, fRot2 = 0.0f;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		fRot1 = -fRotSpeed;
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		fRot1 = fRotSpeed;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		fRot2 = fRotSpeed;
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		fRot2 = -fRotSpeed;

	if (fRot1 != 0.0f || fRot2 != 0.0f)
	{
		A3DLIGHTPARAM lp = m_pG3DEngine->GetDirectionalLight()->GetLightparam();
		A3DVECTOR3 vLightPos = -lp.Direction;

		if (fRot1 != 0.0f)
		{
			vLightPos = a3d_RotatePosAroundY(vLightPos, DEG2RAD(fRot1));
		}

		if (fRot2 != 0.0f)
		{
			A3DVECTOR3 vDir = a3d_Normalize(vLightPos);
			float fCurPitch = (float)atan(vDir.y / vDir.MagnitudeH());
			fCurPitch = RAD2DEG(fCurPitch);

			fCurPitch += fRot2;
			a_Clamp(fCurPitch, -89.0f, 89.0f);
			fCurPitch = DEG2RAD(fCurPitch);

			A3DVECTOR3 vPos;
			vPos.y = (float)sin(fCurPitch);
			float fProjLen = (float)cos(fCurPitch);
			vPos.x = vDir.x * fProjLen / vDir.MagnitudeH();
			vPos.z = vDir.z * fProjLen / vDir.MagnitudeH();
			vLightPos = vPos * vLightPos.Magnitude();
		}

		lp.Direction = a3d_Normalize(-vLightPos);
		m_pG3DEngine->GetDirectionalLight()->SetLightParam(lp);
	}
}
