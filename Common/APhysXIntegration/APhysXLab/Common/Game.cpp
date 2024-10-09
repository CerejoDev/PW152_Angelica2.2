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

#include <A3DGFXExMan.h>
#include <A3DSkillGfxEvent.h>
#include "LuaState.h"
#include "ShadowRender.h"

CGame g_Game;

// an interface required by EC_Model/GfxCommon
A3DDevice*	  g_pDevice = 0;
APhysXScene*  gPhysXScene = 0;
APhysXScene* gGetAPhysXScene() { return gPhysXScene; }

// some config variable
bool gTestConvexMesh = false;
bool gChangeECMScale = false;
bool gTestECMBounceDamping = false;
bool gEnableDynamicAPhysXCC = false;
bool gDefaultAPhysXCCIsOpen = true;
bool gEnableMainActorPhysXBody = true;
bool gShowJointDebugAxis = false;
bool gEnableTestCCPerformance = false;

int gTotalMACount = 1;
int gTotalNPCCount = 0;

bool bIsPause = false;
bool bIsStep = false;

// for test & debug Rive FF
bool gTestRiveFF = false;
NxVec3 TargetPos;
NxActor* pActor = 0;
NxActor* pMoveActor = 0;

const unsigned int CGame::g_gameSceneVersion = 0xCC00000F;
const A3DVECTOR3 CGame::g_vAxisX(1.0f, 0.0f, 0.0f);
const A3DVECTOR3 CGame::g_vAxisY(0.0f, 1.0f, 0.0f);
const A3DVECTOR3 CGame::g_vAxisZ(0.0f, 0.0f, 1.0f);

CGame::CGame()
{
	m_bInitedStart = false;
	m_bInitedEndOK = false;
	
	m_bIsPhysXProfile   = false;
	m_bIsPhysXDbgRender = false;
	m_bShowTerrianGrid  = false;
	m_bShowFFShapes		= false;

	m_bIsPhysXPause = false;
	m_bIsPhysXStep  = false; 
	m_bIsPickPhysXSomething = false;
	m_bIsCloseModelClothes = false;
	m_bIsEnableWeather = false;
	m_bIsEnableSurrounding = false;

	m_iGameMode   = GAME_MODE_EDIT;
	m_iDrivenMode = IPhysXObjBase::DRIVEN_BY_ANIMATION;
	m_iSMode	  = SM_EYE_TO_CENTER;
	m_iSSID		  = SSI_SPHERE;	

	m_pPhysXEngine = 0;
	m_pPhysXScene  = 0;

	m_pG3DEngine    = 0;
	m_pShadowRender = 0;
	m_pCursor	  = 0;
	m_pA3DSky	  = 0;
	m_pTerrain	  = 0;
	m_pCameraCtrl = 0;

	m_ptRayHit.x = m_ptRayHit.y = 0;
	m_pObjPicked  = 0;
	m_pObjRayHit  = 0;
	m_CMAIdx	  = -1;
	m_pCurrentMA  = 0;
	m_pBkGrndTask = 0;

	m_pGraphicSync = 0;
	// m_pTConvexMesh = 0;
	a_InitRandom();
/*	m_pCMOBDamping = 0;*/

	m_pScenarioMgr = 0;
}

CGame::~CGame()
{
	if (m_bInitedStart)
		Release();
}

const char* CGame::GetObjName(const ShootStuffID id)
{
	static bool bInited = false;
	static char* theInfo[SSI_END_BOUND];
	
	assert(id < SSI_END_BOUND);
	if (bInited)
		return (id == SSI_END_BOUND)? theInfo[0] : theInfo[id];
	
	bInited = true;
	theInfo[SSI_SPHERE] = "Models\\PhysXComponents\\球\\球.ecm";
	theInfo[SSI_BOX]    = "Models\\PhysXComponents\\箱子\\箱子.ecm";
	theInfo[SSI_BARREL] = "Models\\PhysXComponents\\桶\\桶.ecm";
	theInfo[SSI_BREAKABLEBOX] = "Models\\PhysXComponents\\箱子破损\\箱子破损.ecm";
	theInfo[SSI_BOMB] = "ForceField\\explo.xff";

	return GetObjName(id);
}

CGame::ObjID CGame::GetObjType(const ShootStuffID id)
{
	if (SSI_BOMB == id)
		return CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD;
	else
		return CPhysXObjMgr::OBJ_TYPEID_ECMODEL;
}

bool CGame::Init(CRender& g3dEngine)
{
	assert(g3dEngine.EngineIsReady());
	if (!g3dEngine.EngineIsReady())
		return false;

	if (m_bInitedStart)
		Release();

	m_bInitedStart = true;
	
	g_LuaStateMan.Init();
	InitECMApi(g_LuaStateMan.GetConfigState());
	InitECMApi(g_LuaStateMan.GetAIState());

	m_pG3DEngine = &g3dEngine;
	g_pDevice = g3dEngine.GetA3DDevice();

	if (!AfxGetGFXExMan()->Init(g3dEngine.GetA3DDevice()))
		return false;
	_SGC::AfxGetSkillGfxEventMan()->Init(g3dEngine.GetA3DDevice());
	
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

	m_pShadowRender = new CECShadowRender;
	if (0 == m_pShadowRender)
		a_LogOutput(1, "CGame::Init: Failed to create CECShadowRender!");
	else
	{
		if (!m_pShadowRender->Init(512))
		{
			a_LogOutput(1, "CGame::Init, failed to initalize CECShadowRender!");
			m_pShadowRender->Release();
			delete m_pShadowRender;
			m_pShadowRender = 0;
		}
	}
	
	m_pGraphicSync = new CGraphicsSyncRB(*(m_pG3DEngine->GetA3DEngine()->GetA3DWireCollector()));
// 	if (gTestECMBounceDamping)
// 		m_pCMOBDamping = new CMOBDamping;
	
	m_bInitedEndOK = true;
	return true;
}

void CGame::Release()
{
	m_bInitedStart = false;
	m_bInitedEndOK = false;

	// Note: we must call EndSimulate at first, if we are in the NON-Edit game mode.
	//       The reason is that the PhysX stuff maybe was released while delete/release member objects.  
	if (GAME_MODE_EDIT != GetGameMode())
	{
		if (0 != m_pPhysXScene)
			m_pPhysXScene->EndSimulate();
	}

	delete m_pScenarioMgr;
	m_pScenarioMgr = 0;
// 	delete m_pCMOBDamping;
// 	m_pCMOBDamping = 0;
// 	delete m_pTConvexMesh;
// 	m_pTConvexMesh = 0;
	delete m_pBkGrndTask;
	m_pBkGrndTask = 0;
	delete m_pGraphicSync;
	m_pGraphicSync = 0;

	A3DRELEASE(m_pShadowRender);
	if (0 != m_pCursor)
	{
		ChangeCursor(true);
		delete m_pCursor;
		m_pCursor = 0;
	}

	ReleaseAllObject();
	ReleaseMainActor();

	m_strTrnFile.Empty();
	A3DRELEASE(m_pTerrain);
	A3DRELEASE(m_pA3DSky);
	ReleasePhysX();

	delete m_pCameraCtrl;
	m_pCameraCtrl = 0;
	m_pG3DEngine = 0;
	g_pDevice = 0;

	_SGC::AfxGetSkillGfxEventMan()->Release();
	AfxGetGFXExMan()->Release();
	g_LuaStateMan.Release();
}

bool CGame::Reset()
{
	ReleaseAllObject();
	if (m_pPhysXScene)
		m_pPhysXScene->Reset();
	
	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();
	ChangeCursor(true);

	SetGameMode(GAME_MODE_EDIT);

	m_SceneConfig.Reset();
	m_BrushesObjMgr.Release();

	return true;
}

IPhysXObjBase* CGame::CreateObjectAndLoad(const ObjID objType, const char* szFile, const bool isNPC)
{
	bool IsEditMode = false;
	if (GAME_MODE_EDIT == GetGameMode())
		IsEditMode = true;

	IPhysXObjBase* pObject = CreateObject(objType, isNPC);
	if (0 == pObject)
		return 0;
	
	pObject->SetDrivenMode(m_iDrivenMode);
	if (!pObject->LoadModel(*m_pG3DEngine, szFile, m_pPhysXScene, IsEditMode))
	{
		ReleaseObject(pObject);
		return 0;
	}

	pObject->SetRefFrameEditMode(m_objMgr.m_refFrame.GetEditType());
	if (!IsEditMode)
		pObject->RaiseFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	if (!m_SceneConfig.IsLocked())
		m_BrushesObjMgr.AddBrushesObj(pObject);

	return pObject;
}

IPhysXObjBase* CGame::CreateObjectRuntime(const ObjID objType, const char* szFile, const NxVec3& pos, const bool OnGround, const bool isEnterRuntime)
{
	IPhysXObjBase* pObj = CreateObjectAndLoad(objType, szFile, false);
	if (0 == pObj)
		return 0;
	
	A3DVECTOR3 aPos = APhysXConverter::N2A_Vector3(pos);
	if (OnGround) 
		aPos.y = m_pTerrain->GetPosHeight(aPos);

	pObj->SetPos(aPos);
	pObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
	pObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
	//	pObj->TickAnimation(0);  // must update internal pose data if using EC_Model with hardLink PhysX 
	pObj->RaiseFlag(IPhysXObjBase::OBF_DRIVENMODE_ISLOCKED);
	pObj->RaiseFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
	if (isEnterRuntime)
		pObj->EnterRuntime(*m_pPhysXScene);
	
	return pObj;
}

IPhysXObjBase* CGame::CreateObject(const ObjID objType, const char* szFile, const POINT& pos)
{
	IPhysXObjBase* pObject = CreateObjectAndLoad(objType, szFile);
	if (0 == pObject)
		return 0;

	A3DVECTOR3 vHitPos;
	if (GetTerrainPosByCursor(pos.x, pos.y, vHitPos))
		pObject->SetPos(vHitPos);
	
	SetHitObject(pObject);
	SetPickedObject(pObject);
	return pObject;
}

void CGame::ReleaseObject(IPhysXObjBase* pObject)
{
	//yx，维护一个m_BrushesObjMgr与m_objMgr对应，包含brush信息及运动属性 
	m_BrushesObjMgr.ReleaseBrushesObj(pObject);

	m_objMgr.ReleaseObject(pObject); 
	if (pObject == m_pObjPicked) m_pObjPicked = 0;
	if (pObject == m_pObjRayHit) m_pObjRayHit = 0;
	if (0 == m_pObjRayHit) m_ptRayHit.x = m_ptRayHit.y = 0;
}

void CGame::ReleaseAllObject()
{
	m_objMgr.Release();	
	m_pObjPicked = 0;
	m_pObjRayHit = 0;
	m_ptRayHit.x = m_ptRayHit.y = 0;
}

bool CGame::InitPhysX()
{
	assert(0 != m_pG3DEngine);
	m_pPhysXEngine = APhysXEngine::GetPtrInstance();

	//	Initialize physical engine
	AString strPhysXPath = af_GetBaseDir();
	strPhysXPath += "\\PhysX";

	APhysXEngineDesc apxEngineDesc;
//	apxEngineDesc.mPhysXCorePath = strPhysXPath;
	apxEngineDesc.mA3DEngine = m_pG3DEngine->GetA3DEngine();
	apxEngineDesc.mEnableVRD = true;

	if (!m_pPhysXEngine->Init(apxEngineDesc))
	{
		a_LogOutput(1, "CGame::InitPhysX: Failed to initialzie physical engine!");
		m_pPhysXEngine = 0;
		return false;
	}
	m_pPhysXEngine->EnableHardwarePhysX(true);

	// turn off the debug render of physX joints and joint limits...
	int DJAV = 0;
	if (gShowJointDebugAxis)
		DJAV = 1;
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_WORLD_AXES, DJAV);
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, DJAV);
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 0);
	gPhysXCreationDeletionReport->Register(&m_objMgr);

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
	m_pPhysXScene->EnablePerfStatsRender(m_bIsPhysXProfile);
	m_pPhysXScene->EnableDebugRender(m_bIsPhysXDbgRender);
	gPhysXScene = m_pPhysXScene;
	
	m_PhysXPicker.Init(m_pPhysXScene, m_pG3DEngine->GetViewport());
	m_pScenarioMgr = new ScenarioMgr(*m_pPhysXScene->GetNxScene());
	return true;
}

void CGame::ReleasePhysX()
{
// 	if (0 != m_pCMOBDamping)
// 		m_pCMOBDamping->CloseCM();

	m_PhysXPicker.Release();
	if (0 != m_pPhysXScene)
	{
		m_pPhysXScene->EndSimulate();
		m_pPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
		m_pPhysXScene = 0;
		gPhysXScene = 0;
	}
}

void CGame::CreateNPCActor()
{
	if (0 >= gTotalNPCCount)
		return;

	IPhysXObjBase* pObj = 0;
	for (int i = 0; i < gTotalNPCCount; ++i)
	{
		NxVec3 pos = APhysXConverter::A2N_Vector3(ACNPCWalk::GetRandomPos(5, 0, m_pTerrain, m_pCameraCtrl));
		pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\Players\\形象\\女\\躯干\\女.ecm", pos, false, false);
		if (0 != pObj)
		{
			CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
			if (0 != pDynObj)
			{
				pDynObj->RaisePeopleInfo(IPhysXObjBase::OBPI_IS_NPC);
				pDynObj->RaisePeopleInfo(IPhysXObjBase::OBPI_ACTION_WALK_AROUND);
				pDynObj->ClearFlag(IPhysXObjBase::OBF_DRIVENMODE_ISLOCKED);
				if (!gDefaultAPhysXCCIsOpen)
				{
					pDynObj->ClearPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_CC);
					pDynObj->ClearPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC);
				}
			}
		}
	}
}

CMainActor*	CGame::CreateMainActor()
{
/*	int ii = 0;
	IPhysXObjBase* pNewObj = 0;
	A3DVECTOR3 posA;
	A3DVECTOR3 temp, temp2, temp3, temp4;
	A3DVECTOR3 tempP, tempP2, tempP3, tempP4;
	IPhysXObjBase* pObjRayHit = GetHitObject();
	if (0 != pObjRayHit)
	{
		temp = posA = pObjRayHit->GetPos();
		temp4 = temp3 = temp2 = temp;
		temp2.z = posA.z + 0.8;
		temp3.z = temp2.z + 0.8;
		temp4.z = temp3.z + 0.8;

		tempP = posA = pObjRayHit->GetPos();
		tempP4 = tempP3 = tempP2 = tempP;
		tempP2.z = posA.z + 0.8;
		tempP3.z = tempP2.z + 0.8;
		tempP4.z = tempP3.z + 0.8;

		tempP.x += 20; tempP2.x += 20;tempP3.x += 20;tempP4.x += 20;
//		for (ii = 1; ii <= 9; ++ii)
		for (ii = 1; ii <= 13; ++ii)
		{
			temp.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(temp);
		//		pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}

			temp2.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(temp2);
		//		pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}

			temp3.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(temp3);
		//		pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}

			temp4.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(temp4);
		//		pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}

			tempP.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(tempP);
		//		pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}
			
			tempP2.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(tempP2);
		//		pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}
			
			tempP3.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(tempP3);
	//			pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}
			
			tempP4.x += 1.5;
			pNewObj = CreateObject(ObjID(pObjRayHit->GetObjType()));
			if (0 != pNewObj)
			{
				pNewObj->CopyBasicProperty(*pObjRayHit);
				pNewObj->WakeUp(*m_pG3DEngine);
				pNewObj->SetPos(tempP4);
	//			pNewObj->ClearFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
				pNewObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}
		}
	}
*/

	bool bIsFirst = false;
	if (0 == m_MainActors.GetSize())
		bIsFirst = true;

	A3DVECTOR3 pos;
	if (bIsFirst && !m_pCameraCtrl->EntryDefBindingMode(pos, *GetTerrain()))
		return 0;

	if (!bIsFirst)
		pos = ACNPCWalk::GetRandomPos(5, 0, m_pTerrain, m_pCameraCtrl);

	NxVec3 nxpos = APhysXConverter::A2N_Vector3(pos);
	IPhysXObjBase* pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\Players\\形象\\女\\躯干\\女.ecm", nxpos, false, false);
	if (0 == pObj)
	{
		if (bIsFirst)
			m_pCameraCtrl->EntryFreeFlyMode(true);
		return 0;
	}
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
	if (0 == pDynObj)
		return 0;
	
	pObj->RaisePeopleInfo(IPhysXObjBase::OBPI_IS_MAIN_ACTOR);
	if (gEnableDynamicAPhysXCC)
	{
		pObj->RaisePeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC);
	}
	if (!gDefaultAPhysXCCIsOpen)
	{
		pObj->ClearPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_CC);
		pObj->ClearPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC);
	}
	pObj->SetDirAndUp(m_pCameraCtrl->GetCamera().GetDirH(), g_vAxisY);
	pDynObj->ApplyPhysXBody(gEnableMainActorPhysXBody);

	if (!m_strFFBindToMA.IsEmpty())
	{
		pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD, m_strFFBindToMA, nxpos, false, false);
		if (0 != pObj)
		{
			NxMat34 localpose;
			localpose.id();
			localpose.t.set(0, pDynObj->GetHeight() * 0.5f, 0);
			pDynObj->SetAssociateObj(pObj, &localpose);
		}
	}

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

void CGame::JumpMainActor() const
{
	if (0 != m_pCurrentMA)
	{
		if (GetAsyncKeyState(VK_SHIFT))
		{
			if (0 != m_pCurrentMA->GetAPhysXDynCC())
			{
				m_pCurrentMA->GetAPhysXDynCC()->HangEnd();
				return;
			}
		}
		m_pCurrentMA->Jump(6.0f);
	}
}

bool CGame::LoadSky()
{
	assert(0 == m_pA3DSky);
	m_pA3DSky = new A3DSkySphere;
	if (0 == m_pA3DSky)
	{
		a_LogOutput(1, "CRender::LoadSky: Not enough memory!");
		return false;
	}

	if (!m_pA3DSky->Init(m_pG3DEngine->GetA3DDevice(), NULL, "Cloudy\\Roof.bmp", 
							"Cloudy\\01.bmp", "Cloudy\\02.bmp"))
	{
		a_LogOutput(1, "CGame::LoadSky: Failed to create sphere sky!");
		delete m_pA3DSky;
		m_pA3DSky = 0;
		return false;
	}
	
	m_pA3DSky->SetFlySpeedU(0.03f);
	m_pA3DSky->SetFlySpeedV(0.03f);
	return m_pG3DEngine->SetSky(*m_pA3DSky);
}

void CGame::LoadTerrainAndPhysXTerrain(const char* szFile)
{
	if (!m_bInitedEndOK)
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
	if (!af_IsFileExist(strFile))
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
	
	if (!pNewTerrain->Init(m_pG3DEngine->GetA3DEngine(), 100.0f, 100.0f))
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
	pNewTerrain->SetActRadius(512.0f);
	pNewTerrain->SetViewRadius(512.0f);

	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();
	m_pTerrain = pNewTerrain;
	m_strTrnFile = szFile;

	m_pTerrain->SetLoadCenter(m_pCameraCtrl->GetPos());
	m_pCameraCtrl->AddTerrainHeigt(*m_pTerrain);
	if(0 != m_pShadowRender)
		m_pShadowRender->SetA3DTerrain(m_pTerrain);

	return true;
}

bool CGame::LoadPhysXTerrain(const char* szFile)
{
	assert(0 != m_pPhysXScene);
	if (0 == m_pPhysXScene)
		return false;
	
	m_pPhysXScene->ReleaseTerrain();
	m_pPhysXScene->InitTerrain(szFile);
	return true;
}

bool CGame::GetTracePosByCursor(int x, int y, A3DVECTOR3& vHitPos)
{
	A3DVECTOR3 vDest((float)x, (float)y, 1.0f);
	A3DViewport* pView = g_Render.GetViewport();
	pView->InvTransform(vDest, vDest);
	
	A3DVECTOR3 vStart, vDelta, vNormal;
	float fraction;
	vStart = g_Render.GetCamera()->GetPos();
	vDelta = vDest - vStart;
	if (::RayTrace(vStart, vDelta, vHitPos, vNormal, fraction))
	{
		return true;
	}
	return false;
}

bool CGame::GetTerrainPosByCursor(const int x, const int y, A3DVECTOR3& vOutPos)
{
	if (!m_bInitedEndOK)
		return false;

	if (0 == m_pTerrain)
		return false;

	A3DVECTOR3 vDest = A3DVECTOR3(x, y, 1);
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

bool CGame::IsPhysXSimEnable() const
{
	if (!m_bIsPhysXPause)
		return true;

	if (m_bIsPhysXStep)
		return true;

	return false;
}

bool CGame::Tick(const unsigned int dwDeltaTime)
{
	float dtSec = dwDeltaTime * 0.001f;
	dtSec = m_pPhysXScene->GetExactSimulateTime(dtSec);
	if (dtSec < 0.0001f)
		return true;
	unsigned int dtMSec = dtSec * 1000; // note: only used for angelica tick, don't used for PhysX tick/sim

	if (0 == GetTerrain())
		return true;

	const bool bIsPaused = !IsPhysXSimEnable();
	if (bIsPaused)
	{
		m_pCameraCtrl->MoveCamera(dtSec, GetTerrain());
		return false;
	}

	A3DCamera& Camera = m_pCameraCtrl->GetCamera();
	A3DVECTOR3 vCamPos = Camera.GetPos();
	m_pTerrain->Update(dtMSec, &Camera, vCamPos);

	if ((0 != m_pCurrentMA) && (GAME_MODE_PLAY == GetGameMode()))
	{
		m_objMgr.UpdateNeighborObjects(m_pCurrentMA->GetPos(), 100, *m_pG3DEngine, *m_pPhysXScene, false, m_pBkGrndTask);
		m_BrushesObjMgr.Tick(dtMSec);
		for (int k = 0; k < m_MainActors.GetSize(); k++)
			m_MainActors[k]->UpdateDynDes();
		if (m_pCurrentMA->IsKeepWalking())
		{
			int x, y;
			if (m_pCurrentMA->GetTargetPT(x, y))
			{
				A3DVECTOR3 vHitPos;
				if (GetTracePosByCursor(x, y, vHitPos))
				{
					m_pCurrentMA->WalkTo(vHitPos);
					if (gTestRiveFF && (0 != pMoveActor))
					{
						NxVec3 pos = pMoveActor->getGlobalPosition();
						TargetPos = APhysXConverter::A2N_Vector3(vHitPos);
						NxVec3 d = TargetPos;
						d -= pos;
						d.normalize();
						d *= 5;
						pMoveActor->setLinearVelocity(d);
					}
				}
			}
		}

		if (CCameraController::MODE_BINDING == m_pCameraCtrl->GetMode())
			m_pCurrentMA->ScanControlKey(Camera.GetDirH());
	}

	if (gTestRiveFF && (0 != pMoveActor))
	{
		NxVec3 pos = pMoveActor->getGlobalPosition();
		NxVec3 d = TargetPos - pos;
		if (d.magnitude() < 0.1f)
			pMoveActor->setLinearVelocity(NxVec3(0.0f));
	}

	if (0 != m_pScenarioMgr)
		m_pScenarioMgr->TickScenario(dtSec);
	m_MMP.TickScenario(dtSec);

	//yx test
	if (m_pCurrentMA)
	{
		float angle, r;
		float pi2 = 2*3.1415926f;
		A3DVECTOR3 pos = m_pCurrentMA->GetPos();
		
		for (int i = 0; i < m_MainActors.GetSize(); i++)
		{
			if (m_pCurrentMA == m_MainActors[i])
				continue;
			//
			if (m_MainActors[i]->GetDeltaMove().SquaredMagnitude() < 1E-5f)
			{
				//generate new target
				angle = a_Random(0.f, pi2);
				r = a_Random(5, 90);
				//generate new target				
				pos.x += r*cosf(angle);
				pos.z += r*sinf(angle);
				pos.y = m_pTerrain->GetPosHeight(pos);
				
				m_MainActors[i]->WalkTo(pos);
			}			
		}
	}
	m_pPhysXScene->GetPerfStats()->StartPerfStat("TickMove Dt");
	m_objMgr.TickMove(dtSec);
	m_pPhysXScene->GetPerfStats()->EndPerfStat();

	APhysXCharacterControllerManager* pCCM = m_pPhysXEngine->GetAPhysXCCManager();
	if (0 != pCCM)
	{
		if (gEnableTestCCPerformance)
			m_pPhysXScene->GetPerfStats()->StartPerfStat("APhysX CCM Tick");
		pCCM->Tick();
		if (gEnableTestCCPerformance)
			m_pPhysXScene->GetPerfStats()->EndPerfStat();
	}
	m_objMgr.TickAnimation(dtMSec);
	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_MainActors[i]->TickAura();
	m_pPhysXEngine->Simulate(dtSec);

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
				
		m_objMgr.SyncDataPhysXToGraphic();
	}

// 	if (0 != m_pCMOBDamping)
// 		m_pCMOBDamping->UpdateTick();

	// A3DEngine::TickAnimation trigger animation of many objects.
	// For example: A3DSky objects, GFX objects etc.
	static const float TIME_TICKANIMATION =	0.03f;

	static float AnimTime = 0;
	if ((AnimTime += dtSec) >= TIME_TICKANIMATION)
	{
		AnimTime -= TIME_TICKANIMATION;
		m_pG3DEngine->GetA3DEngine()->TickAnimation();
		Camera.UpdateEar();
	}
	
	AfxGetGFXExMan()->Tick(dtMSec);
	_SGC::AfxGetSkillGfxEventMan()->Tick(dtMSec);
	
	if (0 == m_pCurrentMA)
		m_pCameraCtrl->MoveCamera(dtSec, GetTerrain());
	else
		m_pCameraCtrl->MoveCamera(dtSec, GetTerrain(), m_pCurrentMA->GetDeltaMove());

	m_bIsPhysXStep = false;
	return true;
}

bool CGame::Render()
{
	if (0 == GetTerrain())
		return true;
	
	A3DEngine*   pA3DEngine   = m_pG3DEngine->GetA3DEngine();
	A3DDevice*   pA3DDevice   = m_pG3DEngine->GetA3DDevice();
	A3DViewport* pA3DViewport = m_pG3DEngine->GetViewport();

	m_pA3DSky->SetCamera(&m_pCameraCtrl->GetCamera());
	m_pA3DSky->Render();
	
	if (m_bShowTerrianGrid) pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);
	m_pTerrain->Render(pA3DViewport);
	if (m_bShowTerrianGrid) pA3DDevice->SetFillMode(A3DFILL_SOLID);
	
	if (m_bShowTerrianGrid && m_bIsPhysXDbgRender && (0 != m_pPhysXScene))
	{
		APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
		if ((0 != pPhysXTerrain) && (0 != m_pTerrain))
		{
			A3DVECTOR3 vProjPos = m_pCameraCtrl->GetPos();
			vProjPos.y = m_pTerrain->GetPosHeight(vProjPos);
			if (0 != m_pCurrentMA)
				vProjPos = m_pCurrentMA->GetPos();
			m_pG3DEngine->DrawPhysXTerrainRange(vProjPos, pPhysXTerrain, m_pTerrain);
		}	
	}
	
	// render the shadows firstly, so that we can handle transparent ECModel correctly...
	if (0 != m_pShadowRender)
	{
		m_objMgr.RenderShadow(*pA3DViewport);
		A3DLIGHTPARAM param = g_Render.GetDirectionalLight()->GetLightparam();
		m_pShadowRender->Render(pA3DViewport, param.Direction);
	}

	static bool IsDrawModelAABBLast = false;
	static bool IsDrawModelAABBNow = false;
	if (GetAsyncKeyState(VK_F9) & 0x8000)
		IsDrawModelAABBNow = true;
	else
		IsDrawModelAABBNow = false;
	if (IsDrawModelAABBLast != IsDrawModelAABBNow)
	{
		m_objMgr.SetDrawModelAABB(IsDrawModelAABBNow);
		IsDrawModelAABBLast = IsDrawModelAABBNow;
	}
	m_objMgr.Render(*pA3DViewport, pA3DEngine->GetA3DWireCollector(), IsPhysXDebugRenderEnabled());

	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_MainActors[i]->Render(*pA3DEngine->GetA3DWireCollector(), m_pCurrentMA);

	A3DSkinRender* psr = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	A3DLitModelRender* plmr = pA3DEngine->GetA3DLitModelRender();

	//yx render convex hull
	if (m_bIsPhysXDbgRender && m_iGameMode == GAME_MODE_PLAY)
	{
		m_BrushesObjMgr.Render(pA3DEngine->GetA3DFlatCollector());
	}
	if (m_iGameMode == GAME_MODE_EDIT)
		m_SceneConfig.Render(pA3DEngine);

	//	Flush non-alpha objects
	pA3DDevice->SetAlphaBlendEnable(false);
	pA3DDevice->SetAlphaTestEnable(true);
//	pA3DDevice->SetAlphaFunction(A3DCMP_GREATER);
//	pA3DDevice->SetAlphaRef(84);
	psr->Render(pA3DViewport, false);
	plmr->Render(pA3DViewport, false);

	//	Flush alpha objects
	psr->ResetRenderInfo(false);
	plmr->ResetRenderInfo(false);
	psr->RenderAlpha(pA3DViewport);
	plmr->RenderAlpha(pA3DViewport);
	psr->ResetRenderInfo(true);
	plmr->ResetRenderInfo(true);

	pA3DDevice->SetAlphaBlendEnable(true);
	pA3DDevice->SetAlphaTestEnable(false);

	_SGC::AfxGetSkillGfxEventMan()->Render();

	//	Flush gfx
	pA3DEngine->RenderGFX(pA3DViewport, 0xffffffff);
	AfxGetGFXExMan()->RenderAllGfx(pA3DViewport);

	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
		m_pPhysXScene->DebugRender();

	// try to render the global axis...
	if(GetAsyncKeyState(VK_F9) & 0x8000)
	{
		NxVec3 vCamPos = APhysXConverter::A2N_Vector3(m_pCameraCtrl->GetCamera().GetPos() + 3.0f * m_pCameraCtrl->GetCamera().GetDir());
		APhysXUtilLib::DebugDrawArraw(*pA3DEngine->GetA3DWireCollector(), vCamPos, vCamPos + NxVec3(1.0f, 0, 0), 0xffff0000);
		APhysXUtilLib::DebugDrawArraw(*pA3DEngine->GetA3DWireCollector(), vCamPos, vCamPos + NxVec3(0, 1.0f, 0), 0xff00ff00);
		APhysXUtilLib::DebugDrawArraw(*pA3DEngine->GetA3DWireCollector(), vCamPos, vCamPos + NxVec3(0, 0, 1.0f), 0xff0000ff);
	}

	IPhysXObjBase* pObj = GetHitObject();
	if (0 != pObj)
	{
		AString strName = pObj->GetFilePathName();
		af_GetFileTitle(strName, strName);
		m_pG3DEngine->TextOut(m_ptRayHit.x, m_ptRayHit.y - 15, _TAS2WC(strName), 0xff00ff00);
	}

	return true;
}

void CGame::SetGameMode(const GameMode iGameMode)
{ 
	assert(0 != m_pPhysXScene);	
	if (0 == m_pPhysXScene)
		return;
	
	if(iGameMode == m_iGameMode)
		return;
	
	switch(iGameMode)
	{
	case GAME_MODE_EDIT:
		if (gTestRiveFF)
		{
			pActor = 0;
			pMoveActor = 0;
		}

		// Note: The following calling sequence is the most safety way.
		// No matter the type of APhysXScene is synchronism or asynchronism.
		// EndSimulate() must be called before release any PhysX objects. 
		// In addition, ReleasePhysXObj() must be called before APhysXScene.Reset();
		m_pPhysXScene->EndSimulate();
		m_MMP.RemoveAll();
		if (0 != m_pScenarioMgr)
			m_pScenarioMgr->ReleaseScenario();
		m_bIsPickPhysXSomething = false;
		m_PhysXPicker.OnMouseButtonUp(0, 0);
		m_pG3DEngine->EnableDrawCameraDir(false);
// 		if (0 != m_pCMOBDamping)
// 			m_pCMOBDamping->CloseCM();
		m_pCameraCtrl->EntryFreeFlyMode(true);
// 		delete m_pTConvexMesh;
// 		m_pTConvexMesh = 0;
		delete m_pBkGrndTask;
		m_pBkGrndTask = 0;
		m_objMgr.LeaveRuntime();
		ReleaseMainActor();
		m_pPhysXScene->Reset(false);
		m_objMgr.WakeUp(*m_pG3DEngine);
		m_objMgr.ResetPose();
		m_bIsPhysXPause = false;
		m_bIsPhysXStep = false;
		m_bIsCloseModelClothes = false;
		m_bIsEnableWeather = false;
		m_bIsEnableSurrounding = false;
		{
			NxScene* pScene = m_pPhysXScene->GetNxScene();
			for (NxU32 i = 0; i < pScene->getNbActors();)
			{
				if (pScene->getActors()[i]->isDynamic())
					pScene->releaseActor(*pScene->getActors()[i]);
				else
					++i;
			}
		}
		SetDrivenMode(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		break;
	case GAME_MODE_SIMULATE:
// 		if (gTestConvexMesh)
// 			m_pTConvexMesh = new TestConvexMesh(*m_pG3DEngine, *m_pPhysXScene, m_objMgr, m_pTerrain, 10);
		m_pCameraCtrl->EntryFreeFlyMode();
		StartSimulate();
		break;
	case GAME_MODE_PLAY:
		m_pBkGrndTask = new BackgroundTask(*m_pG3DEngine, *m_pPhysXScene);
		m_SceneConfig.Lock(true);
		if (1 > gTotalMACount)
			gTotalMACount = 1;
		m_pCurrentMA = CreateMainActor();
		if (0 == m_pCurrentMA)
			break;
		{
			m_CMAIdx = 0;
			for (int i = 1; i < gTotalMACount; ++i)
				CreateMainActor();
		}
		m_SceneConfig.CreateRegionActors();
		m_SceneConfig.Lock(false);
		m_objMgr.UpdateNeighborObjects(m_pCurrentMA->GetPos(), 10, *m_pG3DEngine, *m_pPhysXScene, true, m_pBkGrndTask);
		m_BrushesObjMgr.PrepareStart();
		StartSimulate();
		m_SceneConfig.ApplyInitCC();
		break;
	default:
		return;
	}
	m_iGameMode = iGameMode;
}

void CGame::ChangeECMScale()
{
	if (!gChangeECMScale)
		return;

	static float mass[] = {1.2f, 1.0f, 0.95f, 0.85f, 0.75f, 0.65f, 0.55f, 0.45f, 0.35f, 0.25f, 0.15f, 0.05f };
	static int nSize = sizeof(mass) / sizeof(float);
	static int nCount = 0;
	
	static const float val = 8.377581f / 29.889839f;

	const int nMax = m_objMgr.SizeModels();
	int aCur = 0;
	do
	{
		CPhysXObjECModel* pModel = GetNextECModel_NonShot();
		pModel->SetScale(pow(val / mass[nCount], 1.0f/3.0f));
		aCur++;
		++nCount;
		if (nSize <= nCount)
			nCount = 0;
	} while (aCur < nMax);
}

void CGame::StartSimulate()
{
	ACWalk::gpTerrain = m_pTerrain;
	ACWalk::gpCameraCtrl = m_pCameraCtrl;
	CreateNPCActor();
	ChangeECMScale();

	if (SM_EYE_TO_CENTER == m_iSMode)
		m_pG3DEngine->EnableDrawCameraDir(true);
	SetPickedObject(0);
	SetHitObject(0);
	SetDrivenMode(IPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
	m_objMgr.EnterRuntime(*m_pPhysXScene);
	m_objMgr.AddCMM(m_MMP);
	m_pPhysXScene->StartSimulate();

	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_MainActors[i]->UpdateAura();

	// We must update terrain firstly for creating terrain in PhysX.
	// The character controller depend on the terrain.
	APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
	if ((0 != pPhysXTerrain) && (0 != m_pCameraCtrl))
		pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(m_pCameraCtrl->GetPos()));
}

void CGame::SetPickedObject(IPhysXObjBase* pObject)
{
	m_pObjPicked = pObject;
}

bool CGame::LoadPhysXDemoScene(const char* szFilePath)
{
	if (!m_bInitedEndOK)
	{
		AfxMessageBox(_T("请先初始化后再读取文件！"), MB_OK | MB_ICONERROR);
		return false;
	}
	Reset();

	APhysXUserStream inStream(szFilePath, true);
	unsigned int version = inStream.readDword();

	if(version < 0xCC000001)
	{
		AfxMessageBox(_T("无法识别的文件版本！"), MB_OK | MB_ICONERROR);
		return false;
	}

	m_strTrnFile = APhysXSerializerBase::ReadString(&inStream);
	LoadTerrainAndPhysXTerrain(m_strTrnFile);
	LoadCameraPose(inStream);
	m_objMgr.LoadDeserialize(*m_pG3DEngine, inStream, version, *m_pPhysXScene);

	if (version >= 0xCC000010)
		m_SceneConfig.Load(inStream);
	else
		m_SceneConfig.InitDefault();
	
	if(version < 0xCC00000E)
		SavePhysXDemoScene(szFilePath);

	m_bIsCloseModelClothes = false;
	m_bIsEnableWeather = false;
	m_bIsEnableSurrounding = false;
	return true;
}

void CGame::SavePhysXDemoScene(const char* szFilePath)
{
	APhysXUserStream outStream(szFilePath, false);	
	outStream.storeDword(g_gameSceneVersion);
	
	APhysXSerializerBase::StoreString(&outStream, m_strTrnFile);
	SaveCameraPose(outStream);
	m_objMgr.SaveSerialize(outStream);

	//save scene config
	m_SceneConfig.Save(outStream);
}

void CGame::LoadCameraPose(const NxStream& nxStream)
{
	A3DVECTOR3 vPos, vDir, vUp;	
	nxStream.readBuffer(&vPos, sizeof(A3DVECTOR3));
	nxStream.readBuffer(&vDir, sizeof(A3DVECTOR3));
	nxStream.readBuffer(&vUp, sizeof(A3DVECTOR3));
	m_pG3DEngine->SetupCamera(vPos, vDir, vUp);
}

void CGame::SaveCameraPose(NxStream& nxStream)
{
	A3DCamera* pCamera = m_pG3DEngine->GetCamera();
	nxStream.storeBuffer(&pCamera->GetPos(), sizeof(A3DVECTOR3));
	nxStream.storeBuffer(&pCamera->GetDir(), sizeof(A3DVECTOR3));
	nxStream.storeBuffer(&pCamera->GetUp(), sizeof(A3DVECTOR3));
}

void CGame::ChangeCursor(const bool bIsSysDef)
{
	if (bIsSysDef)
		m_pG3DEngine->ShowCursor(0);
	else
		m_pG3DEngine->ShowCursor(m_pCursor);
}

void CGame::SetHitObject(IPhysXObjBase* pNewHit)
{
	if (m_pObjRayHit == pNewHit)
		return;

	if (0 != m_pObjRayHit)
	{
		m_pObjRayHit->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
		m_pObjRayHit->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
	}

	if (0 != pNewHit)
	{
		pNewHit->RaiseFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
		pNewHit->RaiseFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
	}
	m_pObjRayHit = pNewHit;

	if (0 == m_pObjRayHit)
		m_ptRayHit.x = m_ptRayHit.y = 0;
}

void CGame::OnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	switch(GetGameMode())
	{
	case GAME_MODE_SIMULATE:
		m_PhysXPicker.OnMouseButtonDown(x, y);
		break;
	case GAME_MODE_PLAY:
		{
			m_bIsPickPhysXSomething = m_PhysXPicker.OnMouseButtonDown(x, y);
			//switch current main actor(yx)
			int nIndex = -1;
			A3DVECTOR3 vDest((float)x, (float)y, 1.0f);
			A3DViewport* pView = g_Render.GetViewport();
			pView->InvTransform(vDest, vDest);
			A3DVECTOR3 vTempPos, vTempNormal;
			float fTempFrac;
			
			A3DVECTOR3 vStart = g_Render.GetCamera()->GetPos();
			A3DVECTOR3 vDelta = vDest - vStart;
			float fMinFraction = 1.1f;
			for (int i = 0; i < m_MainActors.GetSize(); i++)
			{
				const A3DAABB& aabb = m_MainActors[i]->GetModelAABB();
				if (CLS_RayToAABB3(vStart, vDelta, aabb.Mins, aabb.Maxs, vTempPos, &fTempFrac, vTempNormal) &&
					fTempFrac < fMinFraction)
				{
					fMinFraction	= fTempFrac;
					nIndex = i;
				}
				
			}
			if (fMinFraction < 1.1f && m_pCurrentMA != m_MainActors[nIndex])
			{
				m_pCurrentMA = m_MainActors[nIndex];
				
				if (m_pCameraCtrl->GetMode() == CCameraController::Mode::MODE_BINDING)
					m_pCameraCtrl->EntryBindingMode(m_pCurrentMA->GetPos(), GetTerrain(), true);
				
				return;
			}
			if (!m_bIsPickPhysXSomething)
			{
				A3DVECTOR3 vHitPos;
				if (GetTracePosByCursor(x, y, vHitPos))
				{
					if (!m_pCurrentMA->IsInAir())
					{
						m_pCurrentMA->WalkTo(vHitPos);
						m_pCurrentMA->UpdateTargetPT(x, y);
						m_pCurrentMA->SetLButtonDown();
					}
				}
			}
		}
		
		break;
	case GAME_MODE_EDIT:
		if (0 != GetPickedObject())
		{
			SetPickedObject(0);
			if (MK_SHIFT & nFlags)
				CloneHitObject();
		}
		else
		{
			if (0 != GetHitObject())
			{
				if (!m_pObjRayHit->m_refFrame.OnLButtonDown(x, y, nFlags))
					SetPickedObject(m_pObjRayHit);
			}
		}
		//yx
		m_SceneConfig.OnLButtonDown(x, y, nFlags);
		break;
	default:
		break;
	}
}

void CGame::OnLButtonUp(const int x, const int y, const unsigned int nFlags)
{
	switch(GetGameMode())
	{
	case CGame::GAME_MODE_SIMULATE:
		m_PhysXPicker.OnMouseButtonUp(x, y);
		break;
	case CGame::GAME_MODE_PLAY:
		m_bIsPickPhysXSomething = false;
		m_PhysXPicker.OnMouseButtonUp(x, y);
		m_pCurrentMA->SetLButtonUp();
		break;
	default:
		break;
	}
}

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

void CGame::OnMouseMove(const int x, const int y, const unsigned int nFlags)
{
	switch(GetGameMode())
	{
	case CGame::GAME_MODE_SIMULATE:
		m_PhysXPicker.OnMouseMove(x, y);
		break;
	case CGame::GAME_MODE_PLAY:
		if (m_bIsPickPhysXSomething)
			m_PhysXPicker.OnMouseMove(x, y);
		else
		{
			if (MK_LBUTTON & nFlags)
				m_pCurrentMA->UpdateTargetPT(x, y);
		}
		break;
	case CGame::GAME_MODE_EDIT:
		{
			if (MK_LBUTTON & nFlags)
			{
				// drag and drop the object's reference frame
				if (0 != GetHitObject())
				{
					m_pObjRayHit->m_refFrame.OnMouseMove(x, y, nFlags);
					m_pObjRayHit->UpdatePoseFromRefFrame();
				}
			}
			else
			{
				if (0 != GetPickedObject())   // move the picked object position 
				{
					A3DVECTOR3 vHitPos;
					if (GetTerrainPosByCursor(x, y, vHitPos))
						m_pObjPicked->SetPos(vHitPos);
				}
				else               // hit object detection 
				{
					PhysRayTraceHit hitInfo;
					PhysRay ray = ScreenPosToRay(*m_pG3DEngine->GetViewport(), x, y);
					if (RayTraceObject(ray, hitInfo))
					{
						m_ptRayHit.x = x;
						m_ptRayHit.y = y;
						GetHitObject()->m_refFrame.OnMouseMove(x, y, nFlags);
					}
				}
			}
			m_SceneConfig.OnMouseMove(x, y, nFlags);
			break;
		}
	default:
		break;
	}
}

bool CGame::OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags)
{
	if (CGame::GAME_MODE_PLAY != GetGameMode())
		return false;

	if (!m_pCameraCtrl->QueryMode(CCameraController::MODE_BINDING))
		return false;

	m_pCameraCtrl->UpdateFocusFactor(zDelta, GetTerrain());
	return true;
}

bool CGame::RayTraceObject(const PhysRay& ray, PhysRayTraceHit& hitInfo)
{
	bool rtn = m_objMgr.RayTraceObj(ray, hitInfo);

	IPhysXObjBase* pNewHit = static_cast<IPhysXObjBase*>(hitInfo.UserData);
	SetHitObject(pNewHit);
	return rtn;
}

bool CGame::RayTraceObject(const PhysRay& ray, APhysXRayHitInfo& hitInfo)
{
	NxRay nxRay;
	nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
	nxRay.dir  = APhysXConverter::A2N_Vector3(ray.vDelta);
	nxRay.dir.normalize();
	if (!APhysXPicker::Raycast(m_pPhysXScene, nxRay, hitInfo, APX_RFTYPE_ALL))
	{
		SetHitObject(0);
		return false;
	}

	IPhysXObjBase* pObj = m_objMgr.GetObject(*hitInfo.mActor);
	SetHitObject(pObj);
	return (0 == pObj)? false : true;
}

void CGame::CloneHitObject()
{
	if (0 == GetHitObject())
		return;

	IPhysXObjBase* pNewObj = CreateObject(ObjID(m_pObjRayHit->GetObjType()));
	if (0 == pNewObj)
		return;

	pNewObj->CopyBasicProperty(*m_pObjRayHit);
	if (!pNewObj->WakeUp(*m_pG3DEngine))
	{
		ReleaseObject(pNewObj);
		return;
	}

	SetHitObject(pNewObj);
	SetPickedObject(pNewObj);
}

bool CGame::GetHitObjectFileName(AString& outstrFilePath)
{
	outstrFilePath.Empty();
	if (0 == GetHitObject())
		return false;

	outstrFilePath = GetHitObject()->GetFilePathName();
	return true;
}

void CGame::EnablePhysXProfile(const bool bEnable)
{
	if(bEnable == m_bIsPhysXProfile)
		return;
	
	m_bIsPhysXProfile = bEnable;
	if(0 != m_pPhysXScene)
		m_pPhysXScene->EnablePerfStatsRender(m_bIsPhysXProfile);
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

void CGame::EnablePhysXDebugRender(const bool bEnable)
{
	if(bEnable == m_bIsPhysXDbgRender)
		return;
	
	m_bIsPhysXDbgRender = bEnable;
	if(0 != m_pPhysXScene)
		m_pPhysXScene->EnableDebugRender(m_bIsPhysXDbgRender);	
}

void CGame::SetTerrianGridShow(const bool bShow)
{
	m_bShowTerrianGrid = bShow;
	if(0 != m_pPhysXScene)
		m_pPhysXScene->EnableTerrainDebugRender(bShow);	
}

void CGame::ShootAPhysXObject(HWND hWnd, const float scale, const float fVelocity)
{
	if (GAME_MODE_EDIT == GetGameMode())
		return;
	
	if (0 == m_pPhysXScene)
		return;

	PhysRay ray;
	APhysXRayHitInfo hitInfo;
	if ((SM_MOUSE_FALLING == m_iSMode) || (SM_EYE_TO_MOUSE == m_iSMode))
	{
		POINT pt;
		if (!GetCursorPos(&pt))
			return;
		if (!ScreenToClient(hWnd, &pt))
			return;

		ray = ScreenPosToRay(*m_pG3DEngine->GetViewport(), pt.x, pt.y);
		if (SM_EYE_TO_MOUSE == m_iSMode)
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

	IPhysXObjBase* pObj = m_objMgr.CreateShotObject(GetObjType(m_iSSID));
	if (0 == pObj)
		return;
	
	if (!pObj->LoadModel(*m_pG3DEngine, GetObjName(m_iSSID), 0, false))
	{
		ReleaseObject(pObj);
		return;
	}

	A3DCamera* pA3DCamera = m_pG3DEngine->GetCamera();
	A3DVECTOR3 vEyeDir = pA3DCamera->GetDir();
	A3DVECTOR3 vEyePos = pA3DCamera->GetPos();

	A3DVECTOR3 pos = vEyePos;
	NxVec3 vel = fVelocity * APhysXConverter::A2N_Vector3(vEyeDir);
	if (SM_EYE_TO_MOUSE == m_iSMode)
		vel = fVelocity * APhysXConverter::A2N_Vector3(ray.vDelta);
	else if (SM_FIXED_POINT_1 == m_iSMode)
		pos.Set(13.3854f, 35.6637f, 19.8664f);
	else if (SM_FIXED_POINT_2 == m_iSMode)
		pos.Set(30.7532f, 35.6639f, 20.5214f);
	else if (SM_KINEMATIC == m_iSMode)
		pos += pA3DCamera->GetDir() * 5;
	else if (SM_FRONT_FALLING == m_iSMode)
	{
		vel.zero();
		pos += pA3DCamera->GetDir() * 10;
		float y = m_pTerrain->GetPosHeight(pos);
		if (y > pos.y) pos.y = y + 2; 
	}
	else if (SM_MOUSE_FALLING == m_iSMode)
	{
		vel.zero();
		pos = APhysXConverter::N2A_Vector3(hitInfo.mHitPos);
		pos.y += 4;
	}
	else if (SM_EACH_ECM == m_iSMode)
	{
		CPhysXObjECModel* pModel = GetNextECModel_NonShot();
		if (0 == pModel)
			return;
	
		pos = pModel->GetPos() - pA3DCamera->GetDirH() * 20;
		pos.y = m_pTerrain->GetPosHeight(pos);
		pos.y += 5;
		vel *= 2.5f;
	}

	pObj->SetPos(pos);
	pObj->SetScale(scale);
	pObj->RaiseFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
//	pObj->TickAnimation(0);  // must update internal pose data if using EC_Model with hardLink PhysX 
	pObj->RaiseFlag(IPhysXObjBase::OBF_DRIVENMODE_ISLOCKED);
	pObj->EnterRuntime(*m_pPhysXScene);

	if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType())
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		APhysXSkeletonRBObject* pPSRB =  (0 == pDynObj)? 0 : pDynObj->GetSkeletonRBObject();
		if(0 != pPSRB)
		{
			pPSRB->SetLinearVelocity(vel);
			if (SM_KINEMATIC == m_iSMode)
			{
				const int nCount = pPSRB->GetNxActorNum();
				NxActor* pActor = 0;
				for (int j = 0; j < nCount; ++j)
				{
					pActor = pPSRB->GetNxActor(j);
					if (pActor->isDynamic())
						pActor->raiseBodyFlag(NX_BF_KINEMATIC);
				}
			}
		}
	}
}

void CGame::ChangeShootMode()
{
	int temp = m_iSMode;
	++temp;

	// temporary close some modes
	if (SM_EACH_ECM == temp)
		temp = 0;
//	if (SM_END_FLAG == temp)
//		temp = 0;

	m_iSMode = ShootMode(temp);
}

void CGame::ChangeShootStuffType()
{
	int temp = m_iSSID;
	++temp;
	if (SSI_END_BOUND == temp)
		temp = 0;
	
	m_iSSID = ShootStuffID(temp);
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

void CGame::ChangeTestCMState() const
{
// 	if ((0 != m_pCMOBDamping) && !m_pCMOBDamping->IsWorking())
// 		gTestECMBounceDamping = !gTestECMBounceDamping;
}

void CGame::SetDrivenMode(const DrivenMode dm)
{
	// following codes used to test ECMBounceDamping, have nothing to do with driven mode
	if (gTestECMBounceDamping && (dm == IPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
	{
		int nMax = m_objMgr.SizeModels();
// 		if ((0 < nMax)/* && (0 != m_pCMOBDamping)*/)
// 		{
// 			IPhysXObjBase* pObj = m_objMgr.GetObject(nMax - 1);
// 			CPhysXObjECModel* pObjAim = static_cast<CPhysXObjECModel*>(pObj);
// 			APhysXSkeletonRBObject* pPSRB =  (0 == pObjAim)? 0 : pObjAim->GetSkeletonRBObject();
// 			if (0 != pPSRB)
// 				m_pCMOBDamping->OpenCM(*pPSRB);
// 			if (6 == nMax)
// 				AppendModel(4, nMax);
// 			else if (50 < nMax)
// 				AppendModel(50, nMax);
// 		}
	}

	m_iDrivenMode = dm;
	m_objMgr.SetDrivenMode(dm); 
}

void CGame::AppendModel(const int n, const int size)
{
	if (n > size)
		return;

	for (int i = 1; i < n; ++i)
	{
		IPhysXObjBase* pObj = m_objMgr.GetObject(size - 1 - i);
		CPhysXObjECModel* pObjAim = static_cast<CPhysXObjECModel*>(pObj);
		APhysXSkeletonRBObject* pPSRB =  (0 == pObjAim)? 0 : pObjAim->GetSkeletonRBObject();
// 		if (0 != pPSRB)
// 			m_pCMOBDamping->AppendModel(*pPSRB);
	}
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

void CGame::IncreaseMainActorCCVolume() const
{
	if (0 == m_pCurrentMA)
		return;

	m_pCurrentMA->ChangeCCVolume(true);
}

void CGame::DecreaseMainActorCCVolume() const
{
	if (0 == m_pCurrentMA)
		return;

	m_pCurrentMA->ChangeCCVolume(false);
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

void CGame::OnKeyDownEditScenario()
{
	A3DCamera* pCamera = m_pG3DEngine->GetCamera();
	A3DVECTOR3 centerPos = pCamera->GetPos() + 5 * pCamera->GetDir();
	A3DVECTOR3 pos;
	IPhysXObjBase* pObj = 0;

	// Create CC demo stuff
/*
	pos = centerPos;
	pos.x -= 5;
	pos.z += 10;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}

	pos = centerPos;
	pos.x -= 5;
	pos.z += 17;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}

	pos = centerPos;
	pos.x += 5;
	pos.z += 10;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}
	
	pos = centerPos;
	pos.x += 5;
	pos.z += 17;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}

	pos = centerPos;
	pos.x += 15;
	pos.z += 10;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}
	
	pos = centerPos;
	pos.x += 15;
	pos.z += 17;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}

	pos = centerPos;
	pos.x += 25;
	pos.z += 10;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}
	
	pos = centerPos;
	pos.x += 25;
	pos.z += 17;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\大木板\\大木板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
	{
		pObj->SetDefaultDMOnSim(IPhysXObjBase::DRIVEN_BY_ANIMATION);
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
		pObj->RaiseFlag(IPhysXObjBase::OBF_TO_KINEMATIC);
	}

	pos = centerPos;
	pos.y += 0.5f;
	pos.z -= 6;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\吊桥\\吊桥.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	pos = centerPos;
	pos.y += 0.5f;
	pos.z -= 13;
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\吊桥\\吊桥.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	pos = centerPos;
	pos.x += 20;
	pos.z -= 6;
	pos.y = m_pTerrain->GetPosHeight(pos);
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\跷跷板\\跷跷板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	pos = centerPos;
	pos.x += 20;
	pos.z -= 13;
	pos.y = m_pTerrain->GetPosHeight(pos);
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\跷跷板\\跷跷板.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	pos = centerPos;
	pos.x -= 3;
	pos.z -= 20;
	pos.y = m_pTerrain->GetPosHeight(pos);
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\箱子\\箱子.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	pos = centerPos;
	pos.x += 2;
	pos.z -= 20;
	pos.y = m_pTerrain->GetPosHeight(pos);
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\箱子\\箱子.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);

	pos = centerPos;
	pos.x += 7;
	pos.z -= 20;
	pos.y = m_pTerrain->GetPosHeight(pos);
	pObj = CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\箱子\\箱子.ecm", APhysXConverter::A2N_Vector3(pos), false, false);
	if (0 != pObj)
		pObj->ClearFlag(IPhysXObjBase::OBF_RUNTIME_ONLY);
	*/
}

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
}

void CGame::ShowFFShapes(bool show)
{
	if (m_bShowFFShapes == show)
		return;

	m_bShowFFShapes = show;

	int nCount = m_objMgr.SizeModels();
	for (int i = 0; i < nCount; ++i)
	{
		IPhysXObjBase* pObj = m_objMgr.GetObject(i);
		if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType())
			continue;

		if (m_pObjRayHit == pObj)
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

void CGame::TrytoHang(const PhysRay& ray)
{
	if (0 == m_pCurrentMA)
		return;

	APhysXDynCharCtrler* pDynCC = m_pCurrentMA->GetAPhysXDynCC();
	if (0 == pDynCC)
		return;

	m_pCurrentMA->WalkTo(m_pCurrentMA->GetPos());
	NxVec3 ccDir = pDynCC->GetHorizonDir();
	NxVec3 vNewDir = APhysXUtilLib::Slerp(ccDir, NxVec3(0, 1, 0), 0.6666667f);

	NxVec3 p0 = pDynCC->GetCenterPosition();
	NxVec3 p1 = p0 + ccDir * 2;
	NxVec3 p2 = p0 + vNewDir * 2;
	NxPlane plane(p0, p1, p2);

	NxRay nxRay;
	nxRay.orig.set(ray.vStart.x, ray.vStart.y, ray.vStart.z);
	nxRay.dir.set(ray.vDelta.x, ray.vDelta.y, ray.vDelta.z);
	nxRay.dir.normalize();

	NxReal dist = 0.0f;
	NxVec3 ptHit(0.0f);
	if (!NxRayPlaneIntersect(nxRay, plane,  dist, ptHit))
		return;

	vNewDir = ptHit - p0;
	vNewDir.normalize();
	pDynCC->HangStart(vNewDir);
}

void CGame::SwitchCC(int index)
{
	if (!m_SceneConfig.IsCCSwitchAble())
		return;
	
	for (int i = 0; i < m_MainActors.GetSize(); i++)
	{
		m_MainActors[i]->SwtichCC(index);
	}			
}

ACString CGame::GetCCName() const
{
	if (NULL == m_pCurrentMA)
		return ACString(_T(""));
	if (NULL != m_pCurrentMA->GetAPhysXCC())
		return ACString(_T("APhysXCC"));
	if (NULL != m_pCurrentMA->GetAPhysXDynCC())
		return ACString(_T("APhysXDynCC"));
	if (NULL != m_pCurrentMA->GetBrushCC())
		return ACString(_T("BrushCC"));
	if (NULL != m_pCurrentMA->GetAPhysXNxCC())
		return ACString(_T("NxCC"));
	return ACString(_T(""));
}
