#include "stdafx.h"
#include "Scene.h"
#include <A3DGFXExMan.h>
#include "ECWorldRender.h"
#include "Collision.h"
#include <LuaWrapper/LuaState.h>
#include "AppInterface.h"
#include "BrushesObj.h"
#include "SubThread.h"
#include <time.h>

#ifdef _ANGELICA3
	#include <A3DLPPLight.h>
	#include <A3DGfxEngine.h>
	#include <ECPlatform.h>
	#include <ECSceneModule.h>
	#include "ECScene.h"
	#include "ECBuilding.h"
#else
	#include "terrain.h"
	#include "EC_TriangleMan.h"
	#include "EC_ShadowRender.h"
	#include <A3DSceneRenderConfig.h>
namespace _SGC
{
	static A3DSkillGfxEventMan s_apxAttackSkillGfxMan;
	A3DSkillGfxMan* AfxGetSkillGfxEventMan()
	{
		return &s_apxAttackSkillGfxMan;
	}
}

struct TerrainCfg
{
	TerrainCfg()
	{
		nGridWidth = 256;
		nGridHeight = 256;
		fGridSize = 1.f;
		fHeightMin = 0.f;
		fHeightMax = 800.f;
	}

	int nGridWidth;
	int nGridHeight;
	float fGridSize;
	float fHeightMin;
	float fHeightMax;
};

bool ReadLinePreFromAF(const char* szName, AScriptFile& sf)
{
	if (!sf.GetNextToken(true))
		return false;

	AString str = sf.m_szToken;
	str.TrimLeft("\t ");
	str.TrimRight("\t ");
	if (str != szName)
		return false;

	if (!sf.GetNextToken(true) || sf.m_szToken[0] != '=')
		return false;

	return true;
}
#endif

// some config variable
NxActor* pActor[1000];
bool gTest1KActors = false;
bool gHas1KActors = false;
bool gEnableAllPhysX = true;

bool gToEndSim = false;
bool gToStartSim = false;

// void InitDlgApi(CLuaState * pState){}
class AUILuaManager;
AUILuaManager* GetAuiManager() { return NULL;}
DWORD g_GfxGetLightUniqueId()
{
	static volatile LONG _light_id = 1;
	LONG Id = InterlockedExchangeAdd(&_light_id, 1);
	return Id;
}


class UserEventsTest : public IUserEvent
{
public:
	virtual void RuntimeBegin(const CRegion& place)
	{
		a_LogOutput(1, "RuntimeBegin: %s", place.GetShapeTypeText(place.GetShapeType()));
	}
	virtual void RuntimeEnd(const CRegion& place)
	{
		a_LogOutput(1, "RuntimeEnd: %s", place.GetShapeTypeText(place.GetShapeType()));
	}

	virtual void EnterRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities)
	{
		a_LogOutput(1, "EnterRegion: %s, %d, %d", place.GetShapeTypeText(place.GetShapeType()), nbEntities, entities);
		IObjBase* p0 = 0;
		for (unsigned int i = 0; i < nbEntities; ++i)
		{
			p0 = *entities;
			++entities;
		}

	}
	virtual void LeaveRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities)
	{
		a_LogOutput(1, "LeaveRegion: %s, %d, %d", place.GetShapeTypeText(place.GetShapeType()), nbEntities, entities);
	}
} gTestUE;

#ifdef _ANGELICA3
void ECPLoaderCB::LoadBuliding(int id, ECBuilding* pBuilding)
{
	////to do 
	//ItemData itemInfo;
	//itemInfo.m_ID = id;
	//itemInfo.m_pBrush = new CBrushesObj;
	//itemInfo.m_pAPhysXDesc = new APhysXObjectInstanceDesc;

	//A3DMATRIX4 matPose = pBuilding->GetRotAndTrans();
	//float fScale = pBuilding->GetScale();
	//if (0 != itemInfo.m_pBrush)
	//{
	//	itemInfo.m_pBrush->SetStrFile(pBuilding->GetChbFile());
	//	if (m_bIsRuntime)
	//	{
	//		itemInfo.m_pBrush->Load(matPose, fScale);
	//	}
	//	else
	//	{
	//		itemInfo.m_pBrush->SetRotTrans(matPose);
	//		itemInfo.m_pBrush->SetScale(fScale);
	//	}
	//}

	//if (0 != itemInfo.m_pAPhysXDesc)
	//{
	//	itemInfo.m_pAPhysXDesc->mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(pBuilding->GetPxdFile());

	//	APhysXConverter::A2N_Matrix44(matPose, itemInfo.m_pAPhysXDesc->mGlobalPose);
	//	itemInfo.m_pAPhysXDesc->mScale3D = NxVec3(fScale);
	//	if (m_bIsRuntime)
	//	{
	//		assert(0 != m_pPhysXScene);
	//		itemInfo.m_pAPhysXObj = m_pPhysXScene->CreatePhysXObject(*(itemInfo.m_pAPhysXDesc));
	//	}
	//}
	//m_arrItemData.push_back(itemInfo);
}

void ECPLoaderCB::ReleaseBuilding(int id, ECBuilding* pBuilding)
{
	ItemData* pItem = 0;
	size_t nCount = m_arrItemData.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		pItem = &m_arrItemData[i];
		if (id == pItem->m_ID)
		{
			if (0 != pItem->m_pBrush)
			{
				pItem->m_pBrush->Release();
				delete pItem->m_pBrush;
			}
			if (0 != pItem->m_pAPhysXObj)
			{
				pItem->m_pAPhysXObj->GetPhysXScene()->ReleasePhysXObject(pItem->m_pAPhysXObj);
			}
			delete pItem->m_pAPhysXDesc;

			m_arrItemData[i] = m_arrItemData[nCount - 1];
			m_arrItemData.pop_back();
			return;
		}
	}
}

void ECPLoaderCB::EnterRuntime(APhysXScene& aPhysXScene)
{
	if (m_bIsRuntime)
		return;
	m_bIsRuntime = true;
	m_pPhysXScene = &aPhysXScene;

	ItemData* pItem = 0;
	size_t nCount = m_arrItemData.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		pItem = &m_arrItemData[i];
		if (0 != pItem->m_pBrush)
			pItem->m_pBrush->Load(pItem->m_pBrush->GetTrans(), pItem->m_pBrush->GetScale());
		if (0 != pItem->m_pAPhysXDesc)
			pItem->m_pAPhysXObj = m_pPhysXScene->CreatePhysXObject(*(pItem->m_pAPhysXDesc));
	}
}

void ECPLoaderCB::LeaveRuntime()
{
	if (!m_bIsRuntime)
		return;
	m_bIsRuntime = false;

	ItemData* pItem = 0;
	size_t nCount = m_arrItemData.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		pItem = &m_arrItemData[i];
		if (0 != pItem->m_pBrush)
			pItem->m_pBrush->Release();
		if (0 != pItem->m_pAPhysXObj)
		{
			pItem->m_pAPhysXObj->GetPhysXScene()->ReleasePhysXObject(pItem->m_pAPhysXObj);
			pItem->m_pAPhysXObj = 0;
		}
	}
}
#else

// an interface required by EC_Model/GfxCommon
A3DDevice*	  g_pDevice = 0;
APhysXScene*  gPhysXScene = 0;
APhysXScene* gGetAPhysXScene() { return gPhysXScene; }

IKInterfaceImp s_IKInterface;

#endif
const Scene::SceneModeDesc Scene::gGMDesc[] =
{
	SCENE_MODE_EMPTY,	 _T("空闲"), _T("当前状态: 空闲"),
	SCENE_MODE_PLAY_OFF, _T("编辑"), _T("当前状态: 编辑"),
	SCENE_MODE_PLAY_ON,	 _T("游戏"), _T("当前状态: 游戏"),
};

const A3DVECTOR3 Scene::g_vAxisX(1.0f, 0.0f, 0.0f);
const A3DVECTOR3 Scene::g_vAxisY(0.0f, 1.0f, 0.0f);
const A3DVECTOR3 Scene::g_vAxisZ(0.0f, 0.0f, 1.0f);

const char* Scene::GetObjName(const ShootStuffID id)
{
	static bool bInited = false;
	static char* theInfo[SSI_END_BOUND];

	assert(id < SSI_END_BOUND);
	if (bInited)
		return (id == SSI_END_BOUND)? theInfo[0] : theInfo[id];

	bInited = true;
#ifdef _ANGELICA3
	theInfo[SSI_SPHERE] = "Models\\PhysXComponents\\球\\球.ecm3";
	theInfo[SSI_BOX]    = "Models\\PhysXComponents\\箱子\\箱子.ecm3";
	theInfo[SSI_BARREL] = "Models\\PhysXComponents\\桶\\桶.ecm3";
#else
	theInfo[SSI_SPHERE] = "Models\\PhysXComponents\\球\\球.ecm";
	theInfo[SSI_BOX]    = "Models\\PhysXComponents\\箱子\\箱子.ecm";
	theInfo[SSI_BARREL] = "Models\\PhysXComponents\\桶\\桶.ecm";
#endif
	return GetObjName(id);
}

Scene::ObjID Scene::GetObjType(const ShootStuffID id)
{
	//	if (SSI_BOMB == id)
	//		return RawObjMgr::OBJ_TYPEID_FORCEFIELD;
	//	else
	return ObjManager::OBJ_TYPEID_ECMODEL;
}

Scene::Scene()
{
	m_pG3DEngine  = 0;
	m_pCamCtrlAgent = 0;
	m_pCameraCtrl = 0;
	m_pA3DSky	  = 0;
	m_pCursor	  = 0;
	m_pBackMusic  = 0;
	m_pTerrain	  = 0;
	_renderExtra  = 0;

	m_pECPlatform = 0;
	m_pSceneModule = 0;

	m_pPhysXEngine = 0;
	m_pPhysXScene  = 0;
	m_pRecording = 0;
#ifndef _ANGELICA3
	m_pTriangleMan = 0;
	m_pShadowRender = 0;
#endif

	m_pBkGrndTask = 0;

	m_pGMD = &gGMDesc[SCENE_MODE_EMPTY];
	m_iDrivenMode = IPropPhysXObjBase::DRIVEN_BY_ANIMATION;
	m_iSMode = SM_EYE_TO_CENTER;
	m_iSSID	 = SSI_SPHERE;	

	m_RPTOutputTickNum = 0;
	m_RPTMode = RPT_ALL_LOAD;
	m_fDynLoadRadius = 10.0f;
	m_MAPushForce = 10;
	m_CMAIdx	  = -1;
	m_pCurrentMA  = 0;

	m_minTickTime = -1;
	m_strTrnFile.Empty();
	m_strActiveScene.Empty();

	ApxObjBase::SetOperationReport(&m_AOORMgr);
	IPropObjBase::SetStateReport(&m_OSRMgr);
	m_SelGroup.SetStateReport(&m_OSRMgr);
	m_SelGroup.SetRayHitOwner(m_RHOwner);
	m_OSRMgr.RegisterReceiver(m_RHOwner);
	m_OSRMgr.RegisterReceiver(m_Trigger);
	m_OSRMgr.RegisterReceiver(m_TEventMgr);
	m_OSRMgr.RegisterReceiver(m_SelGroup);
	m_AOORMgr.RegisterReceiver(m_SelGroup);
	m_AOORMgr.RegisterReceiver(*(ObjManager::GetInstance()));
}

Scene::~Scene()
{
	if (m_status.ReadFlag(GAME_INIT_START))
		Release();
	if (m_pPhysXEngine)
		m_pPhysXEngine->Release();
}

int Scene::GetRandSeed() const
{
	return m_pRecording->GetRandSeed();
}

void Scene::SetRandSeed(int rs)
{
	m_pRecording->SetRandSeed(rs);
}

bool Scene::Replay(const char* szFile)
{
	return m_pRecording->Replay(szFile);
}

void Scene::MakeRecordingFileName(bool bIsEpisode, AString& outFullPathName)
{
	int left1 = m_strActiveScene.ReverseFind('/');
	int left2 = m_strActiveScene.ReverseFind('\\');
	int left = (left1 > left2)? left1 : left2;
	int right = m_strActiveScene.GetLength() - left - 1;
	AString strTitle = m_strActiveScene.Right(right);
	int dotpos = strTitle.ReverseFind('.');
	right = strTitle.GetLength() - dotpos;
	strTitle.CutRight(right);

	time_t long_time;
	time(&long_time);					// Get time as long integer.
	tm* today = localtime(&long_time);	// Convert to local time.
	char tbuf[1024];
	strftime(tbuf, 100, "%Y%m%d_%H%M%S", today);

	AString strBasePath = af_GetBaseDir();
	if (bIsEpisode)
	{
		int idx = m_pRecording->GetNextRecordingEpisodeIndex();
		assert(0 < idx);
		outFullPathName.Format("%s\\Rec\\%s_%s_epi%d.rpi", strBasePath, strTitle, tbuf, idx);
	}
	else
	{
		outFullPathName.Format("%s\\Rec\\%s_%s.rec", strBasePath, strTitle, tbuf);
	}
}

bool Scene::Init(const HINSTANCE hInst, const HWND hWnd, const bool bFullScreen)
{
	if (m_status.ReadFlag(GAME_INIT_START))
		Release();
	m_status.RaiseFlag(GAME_INIT_START);
	a_InitRandom();

#ifdef _ANGELICA3
	if (!ECPlatform::CreatePlatform())
	{
		a_LogOutput(1, "Error in %s, Failed to create platform", __FUNCTION__);
		return false;
	}
#else
	g_LuaStateMan.Init();
	InitECMApi(g_LuaStateMan.GetConfigState());
	InitECMApi(g_LuaStateMan.GetAIState());
#endif

	m_pG3DEngine = new CRender;
	if (0 == m_pG3DEngine)
	{
		a_LogOutput(1, "Error in %s: Failed to create CRender!", __FUNCTION__);
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
#endif
		return false;
	}

	if (!m_pG3DEngine->Init(hInst, hWnd, bFullScreen))
	{
		a_LogOutput(1, "Error in %s: Failed to init CRender!", __FUNCTION__);
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
#endif
		return false;
	}
#ifdef _ANGELICA3
	m_pECPlatform = ECPlatform::GetPlatform();
#endif
	m_SelGroup.SetRenderUtility(m_pG3DEngine);

	m_pCameraCtrl = new CCameraController(*m_pG3DEngine->GetCamera());
	if (0 == m_pCameraCtrl)
	{
		a_LogOutput(1, "Error in %s: Failed to create CCameraController!", __FUNCTION__);
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
		m_pECPlatform = 0;
#endif
		return false;
	}

	m_pRecording = new CRecording;
	if (0 == m_pRecording)
	{
		a_LogOutput(1, "Error in %s: Failed to create CRecording!", __FUNCTION__);
		delete m_pCameraCtrl;
		m_pCameraCtrl = 0;
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
		m_pECPlatform = 0;
#endif
		return false;
	}
	m_pRecording->InitScene(*this);
	m_pRecording->EnableDebugFile(true);
	m_pCamCtrlAgent = new CCamCtrlAgent(*m_pCameraCtrl, *m_pRecording);
	if (0 == m_pCamCtrlAgent)
	{
		a_LogOutput(1, "Error in %s: Failed to create CCamCtrlAgent!", __FUNCTION__);
		delete m_pRecording;
		m_pRecording = 0;
		delete m_pCameraCtrl;
		m_pCameraCtrl = 0;
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
		m_pECPlatform = 0;
#endif
		return false;
	}

	if (!InitPhysX())
	{
		delete m_pCamCtrlAgent;
		m_pCamCtrlAgent = 0;
		delete m_pRecording;
		m_pRecording = 0;
		delete m_pCameraCtrl;
		m_pCameraCtrl = 0;
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
		m_pECPlatform = 0;
#endif
		return false;
	}

	if (!LoadSky())
	{
		ReleasePhysX();
		delete m_pCamCtrlAgent;
		m_pCamCtrlAgent = 0;
		delete m_pRecording;
		m_pRecording = 0;
		delete m_pCameraCtrl;
		m_pCameraCtrl = 0;
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
#ifdef _ANGELICA3
		ECPlatform::DestroyPlatform();
		m_pECPlatform = 0;
#endif
		return false;
	}

	m_pCursor = new A3DSysCursor;
	if (0 == m_pCursor)
		a_LogOutput(1, "Warning in %s: Failed to create A3DSysCursor!", __FUNCTION__);
	else
	{
		if (!m_pCursor->Load(m_pG3DEngine->GetA3DDevice(), "Cursors\\normal.ani"))
		{
			a_LogOutput(1, "Warning in %s: Failed to load cursor!", __FUNCTION__);
			delete m_pCursor;
			m_pCursor = 0;
		}
	}

	CDynProperty::gRoleCallback = this;
#ifdef _ANGELICA3
	m_pG3DEngine->GetA3DEngine()->SetA3DAppInterface(AppInterface::GetInstance());

#else
	A3DIKInterface::SetA3DIKInterface(&s_IKInterface);
#endif

	m_SelGroup.SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE);
	IPropObjBase::SetRenderManager(m_pG3DEngine);
	m_LoadPackage.SetRender(*m_pG3DEngine);
	m_status.RaiseFlag(GAME_INIT_ENDOK);
#ifdef _DEBUG
	RegisterRegionEvent(&gTestUE);
#endif

#ifdef _ANGELICA3
	A3DGFXExMan* pGfxExMan = m_pG3DEngine->GetGFXEngine()->GetA3DGfxExMan();
	if (0 != pGfxExMan)
	{
		if (m_generalAttMgr.InitAttackManager(*this, *pGfxExMan))
			m_pG3DEngine->SetAttackSkillGfxMan(m_generalAttMgr.GetAttackSkillGfxMan());
	}
#else
	g_pDevice = m_pG3DEngine->GetA3DDevice();
	if (!AfxGetGFXExMan()->Init(g_pDevice))
		return false;
	_SGC::AfxGetSkillGfxEventMan()->Init(m_pG3DEngine->GetA3DDevice());
	if (m_generalAttMgr.InitAttackManager(*this, *AfxGetGFXExMan()))
		m_pG3DEngine->SetAttackSkillGfxMan(m_generalAttMgr.GetAttackSkillGfxMan());

	m_pTriangleMan = new CECTriangleMan;
	if (0 == m_pTriangleMan)
		a_LogOutput(1, "Warning in %s: Failed to create CECTriangleMan!", __FUNCTION__);
	m_pShadowRender = new CECShadowRender;
	if (0 == m_pShadowRender)
		a_LogOutput(1, "Warning in %s: Failed to create CECShadowRender!", __FUNCTION__);
	else
	{
		if (!m_pShadowRender->Init(1024))
		{
			a_LogOutput(1, "Warning init %s: Failed to initalize CECShadowRender!", __FUNCTION__);
			m_pShadowRender->Release();
			delete m_pShadowRender;
			m_pShadowRender = 0;
		}
		m_pShadowRender->SetCastShadowFlag(true, false);
	}
#endif

	m_ContactReport.InitCR(m_generalAttMgr);
	return true;
}

void Scene::Release()
{
	m_ContactReport.ReleaseCR();
#ifndef _ANGELICA3
	A3DRELEASE(m_pShadowRender);
	delete m_pTriangleMan;
	m_pTriangleMan = 0;
#endif
	m_generalAttMgr.ReleaseAttackManager();
	m_pG3DEngine->SetAttackSkillGfxMan(0);

	SetSceneMode(SCENE_MODE_EMPTY);
	m_status.ClearFlag(GAME_INIT_START);
	m_status.ClearFlag(GAME_INIT_ENDOK);
	ReleasePhysX();

#ifdef _ANGELICA3
	if (0 != m_pG3DEngine)
		m_pG3DEngine->GetA3DEngine()->SetA3DAppInterface(0);
#endif
	CBrushesObjMgr::GetInstance()->Release();

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
	delete m_pCamCtrlAgent;
	m_pCamCtrlAgent = 0;
	delete m_pCameraCtrl;
	m_pCameraCtrl = 0;

#ifdef _ANGELICA3
	if (0 != m_pSceneModule)
	{
		m_pECPlatform->ReleaseSceneModule(m_pSceneModule);
		m_pSceneModule = 0;
		ECWorldRender::gpSceneModule = 0;
	}
#endif
	ReleaseTerrain();

	IPropObjBase::SetRenderManager(0);
	if (0 != m_pRecording)
		delete m_pRecording;

#ifdef _ANGELICA3
	if(m_pG3DEngine)
	{
		m_pG3DEngine->UnregisterSky();
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
	}

	m_pECPlatform = 0;
	ECPlatform::DestroyPlatform();
#else
	g_pDevice = 0;
	_SGC::AfxGetSkillGfxEventMan()->Release();
	AfxGetGFXExMan()->Release();
	g_LuaStateMan.Release();

	if(m_pG3DEngine)
	{
		m_pG3DEngine->UnregisterSky();
		delete m_pG3DEngine;
		m_pG3DEngine = 0;
	}
#endif
}

bool Scene::InitPhysX()
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
//	apxEngineDesc.mDisablePhysX = true;

	if (!m_pPhysXEngine->Init(apxEngineDesc))
	{
		a_LogOutput(1, "Scene::InitPhysX: Failed to initialzie physical engine!");
		m_pPhysXEngine = 0;
		return false;
	}

	EnablePhysXHW(true);
	// turn off the debug render of physX joints and joint limits...
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 0);
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 0);
	m_pPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 0);

	CreateAScene();
	gPhysXContactReport->Register(&m_ContactReport);
	gPhysXBreakReport->Register(&m_BreakReport);
	gPhysXCCHitReport->Register(&m_CCHitReceiver);
	gPhysXCreationDeletionReport->Register(this);
	return true;
}

bool Scene::CreateAScene()
{
	assert(0 != m_pPhysXEngine);
	assert(0 == m_pPhysXScene);
	APhysXSceneDesc apxSceneDesc;
	apxSceneDesc.mEnableDebugRender = true;
	apxSceneDesc.mNxSceneDesc.groundPlane = true;
#ifdef _ANGELICA3
	apxSceneDesc.mNxSceneDesc.gravity = APHYSX_STD_GRAVITY;
#else
	apxSceneDesc.mNxSceneDesc.gravity.set(0, -16.8f, 0);
#endif
	apxSceneDesc.mDefaultMaterial.mRestitution = 0.3f;
	apxSceneDesc.mDefaultMaterial.mStaticFriction = 0.5f;
	apxSceneDesc.mDefaultMaterial.mDynamicFriction = 0.5f;
	apxSceneDesc.mTimeStep = 0.02f;

	m_pPhysXScene = m_pPhysXEngine->CreatePhysXScene(apxSceneDesc);
	if (0 == m_pPhysXScene)
		return false;

	m_pPhysXScene->EnableTerrainDebugRender(IsShowTerrianGridEnabled());
	m_pPhysXScene->EnablePerfStatsRender(IsPhysXProfileEnabled());
	m_pPhysXScene->EnableDebugRender(IsPhysXDebugRenderEnabled());
#ifdef _ANGELICA3
	m_LoadPackage.GetGFXPhysXSceneShell()->PhysXSceneSwitchOn(*m_pPhysXScene);
#else
	gPhysXScene = m_pPhysXScene;
#endif
	m_PhysXPicker.Init(m_pPhysXScene, m_pG3DEngine->GetViewport());
	return true;
}

void Scene::ReleasePhysX()
{
	gPhysXCreationDeletionReport->UnRegister(this);
	gPhysXBreakReport->UnRegister(&m_BreakReport);
	gPhysXCCHitReport->UnRegister(&m_CCHitReceiver);
	gPhysXContactReport->UnRegister(&m_ContactReport);
	m_PhysXPicker.Release();
	if (0 != m_pPhysXScene)
	{
#ifdef _ANGELICA3
		m_LoadPackage.GetGFXPhysXSceneShell()->PhysXSceneSwitchOff(*m_pPhysXScene);
#else
		gPhysXScene = 0;
#endif
		m_pPhysXScene->EndSimulate();
		m_pPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
		m_pPhysXScene = 0;
	}
	if (0 != m_pPhysXEngine)
		m_pPhysXEngine->Release();
}

bool Scene::LoadTerrainAndPhysXTerrain(const char* szFile)
{
	if (!IsReady())
	{
		a_LogOutput(1, "Warning in %s: 请先初始化后再加载地形！", __FUNCTION__);
		return false;
	}

	AString strFile(szFile);
	if (!LoadPW2Map(strFile))
	{
		if (!LoadTerrain(strFile))
		{
			a_LogOutput(1, "Warning in %s: 加载地形数据失败！", __FUNCTION__);
			return false;
		}
	}

	af_ChangeFileExt(strFile, "_ptb.cfg");
	if (!IsFileExist(strFile, false))
		return false;

	if(!LoadPhysXTerrain(strFile))
	{
		a_LogOutput(1, "Warning in %s: 加载PhysX地形数据失败！", __FUNCTION__);
		return false;
	}
	ChangeCursor(false);
	return true;
}

bool Scene::LoadSky()
{
	assert(0 == m_pA3DSky);
	m_pA3DSky = new A3DSkySphere;
	if (0 == m_pA3DSky)
	{
		a_LogOutput(1, "Scene::LoadSky: Not enough memory!");
		return false;
	}

#ifdef _ANGELICA3
	if (!m_pA3DSky->Init(m_pG3DEngine->GetA3DEngine(), "Cloudy\\Roof.bmp", "Cloudy\\01.bmp", "Cloudy\\02.bmp"))
#else
	if (!m_pA3DSky->Init(m_pG3DEngine->GetA3DDevice(), NULL, "Cloudy\\Roof.dds", "Cloudy\\01.dds", "Cloudy\\02.dds"))
#endif
	{
		a_LogOutput(1, "Scene::LoadSky: Failed to create sphere sky!");
		delete m_pA3DSky;
		m_pA3DSky = 0;
		return false;
	}

	m_pA3DSky->SetFlySpeedU(0.01f);
	m_pA3DSky->SetFlySpeedV(0.01f);
	m_pG3DEngine->RegisterSky(m_pA3DSky);
	return true;
}

bool Scene::LoadMusic(const char* szFile)
{
	assert(0 == m_pBackMusic);
	m_pBackMusic = new AMSoundStream;
	if (0 == m_pBackMusic)
	{
		a_LogOutput(1, "Scene::LoadMusic: Not enough memory!");
		return false;
	}

	if (!m_pBackMusic->Init(m_pG3DEngine->GetA3DEngine()->GetAMEngine()->GetAMSoundEngine(), szFile))
	{
		a_LogOutput(1, "Scene::LoadMusic, Failed to load background music %s.", szFile);
		return false;
	}

	m_pBackMusic->Play();
	return true;
}

bool Scene::LoadPW2Map(const char* szFile)
{
#ifdef _ANGELICA3
	ECSceneModule* pSceneModule = m_pECPlatform->CreateSceneModule();
	if (0 == pSceneModule)
	{
		assert(true == false);
		return false;
	}

	AString strFile(szFile);
	int nEnd = strFile.ReverseFind('.');
	int nStart1 = strFile.ReverseFind('/');
	int nStart2 = strFile.ReverseFind('\\');
	int nStart = (nStart1 > nStart2)? nStart1 : nStart2;
	++nStart;
	int nLen = nEnd - nStart;
	AString strName;
	if (0 <= nLen)
		strName = strFile.Mid(nStart, nLen);

	ECSceneModule::SCENEMODULE_PARAM param;
	param.bMultiThreadLoad = true;
	param.fViewRadius = 100;
	param.szMapName = strName;
	param.iWaterRenderLevel = 2;
	param.iTrnRenderLevel = 2;
	param.fGrassLODLevel = 0.5f;
	param.pUserResLoadFunc = 0;
	param.pGFXPhysXSceneShell = m_LoadPackage.GetGFXPhysXSceneShell();
	if(!pSceneModule->Load(param))
	{
		a_LogOutput(1, "Error in %s, Failed to load scene model %s", __FUNCTION__, param.szMapName);
		m_pECPlatform->ReleaseSceneModule(pSceneModule);
		return false;
	}
	ECTerrain* pECTerrain = pSceneModule->GetECTerrain();
	if (0 == pECTerrain)
	{
		m_pECPlatform->ReleaseSceneModule(pSceneModule);
		return false;
	}
	pSceneModule->GetECScene()->SetResLoaderCallBack(&m_MapLoaderCB);
	pSceneModule->SetLoadCenter(APhysXConverter::N2A_Vector3(m_pCamCtrlAgent->GetActiveCamera()->GetGPos()));

	if (0 != m_pSceneModule)
	{
		m_pECPlatform->ReleaseSceneModule(m_pSceneModule);
		m_pSceneModule = 0;
	}
	m_pSceneModule = pSceneModule;
	ECWorldRender::gpSceneModule = m_pSceneModule;
	CommitTerrain(*pECTerrain->GetTerrain2(), szFile);
	return true;
#endif
	return false;
}

bool Scene::LoadTerrain(const char* szFile)
{
	A3DTerrain2* pNewTerrain = new A3DTerrain2;
	if (0 == pNewTerrain)
	{
		a_LogOutput(1, "Scene::LoadTerrain: Not enough memory!");
		return false;
	}

	if (!pNewTerrain->Init(m_pG3DEngine->GetA3DEngine(), 512.0f, 512.0f))
	{
		a_LogOutput(1, "Scene::LoadTerrain: Failed to initialize terrain!");
		A3DRELEASE(pNewTerrain);
		return false;
	}

#ifdef _ANGELICA3
	if (!pNewTerrain->Load(szFile, 0.0f, 0.0f, false))
#else
	if (!pNewTerrain->Load(szFile, 0.0f, 0.0f, false, true))
#endif
	{
		a_LogOutput(1, "Scene::LoadTerrain: Failed to load A3DTerrain2!");
		A3DRELEASE(pNewTerrain);
		return false;
	}

	pNewTerrain->SetLODType(A3DTerrain2::LOD_NOHEIGHT);
	CommitTerrain(*pNewTerrain, szFile);
	m_status.RaiseFlag(GAME_IS_OWNER_TERRAIN);
	return true;
}

bool Scene::LoadPhysXTerrain(const char* szFile)
{
	assert(0 != m_pPhysXScene);
	if (0 == m_pPhysXScene)
		return false;

	m_pPhysXScene->ReleaseTerrain();
	return m_pPhysXScene->InitTerrain(szFile);
}

void Scene::CommitTerrain(A3DTerrain2& terrain, const char* szFile)
{
	ReleaseTerrain();
	m_pTerrain = &terrain;
	m_strTrnFile = szFile;

	m_pTerrain->SetLoadCenter(APhysXConverter::N2A_Vector3(m_pCameraCtrl->GetGPos()));
	m_pCameraCtrl->SetTerrain(m_pTerrain);
	m_pCameraCtrl->AddTerrainHeight(*m_pTerrain);
	m_pG3DEngine->RegisterTerrain(m_pTerrain);
	GetCollisionSupplier()->Init(m_pTerrain, CBrushesObjMgr::GetInstance());
	SetSceneMode(SCENE_MODE_PLAY_OFF);
#ifdef _ANGELICA3
	AppInterface::GetInstance()->SetTerrain(m_pTerrain);
#else
	s_IKInterface.SetTerrain(m_pTerrain);

	AString strFile(szFile);
	int nEnd = strFile.ReverseFind('.');
	int nStart1 = strFile.ReverseFind('/');
	int nStart2 = strFile.ReverseFind('\\');
	int nStart = (nStart1 > nStart2)? nStart1 : nStart2;
	++nStart;
	int nLen = nEnd - nStart;
	AString strName;
	if (0 <= nLen)
		strName = strFile.Mid(nStart, nLen);
	AString strPath = strFile.Left(nStart);
	strPath += "terrain.cfg";

	TerrainCfg	terrainCfg;
	AScriptFile sf;
	if (!sf.Open(strPath))
		return;
	if (!ReadLinePreFromAF("nAreaWidth", sf))
	{
		sf.Close();
		return;
	}
	terrainCfg.nGridWidth = sf.GetNextTokenAsInt(false);
	if (!ReadLinePreFromAF("nAreaHeight", sf))
	{
		sf.Close();
		return;
	}
	terrainCfg.nGridHeight = sf.GetNextTokenAsInt(false);
	if (!ReadLinePreFromAF("vGridSize", sf))
	{
		sf.Close();
		return;
	}
	terrainCfg.fGridSize = sf.GetNextTokenAsFloat(false);
	if (!ReadLinePreFromAF("vHeightMin", sf))
	{
		sf.Close();
		return;
	}
	terrainCfg.fHeightMin = sf.GetNextTokenAsFloat(false);
	if (!ReadLinePreFromAF("vHeightMax", sf))
	{
		sf.Close();
		return;
	}
	terrainCfg.fHeightMax = sf.GetNextTokenAsFloat(false);
	sf.Close();

	m_pTriangleMan->Clear();
	float xoff = terrainCfg.fGridSize * terrainCfg.nGridWidth / -2.0f;
	float zoff = terrainCfg.fGridSize * terrainCfg.nGridHeight / -2.0f;
	m_pTriangleMan->SetMapOrgPos(xoff, zoff);

	TERRAINCONFIG config;
	config.nNumRows = 1;
	config.nNumCols = 1;
	config.nAreaWidth = terrainCfg.nGridWidth;
	config.nAreaHeight = terrainCfg.nGridHeight;
	config.vGridSize = terrainCfg.fGridSize;
	config.vHeightMin = terrainCfg.fHeightMin;
	config.vHeightMax = terrainCfg.fHeightMax;
	sprintf(config.szMapPath, "maps/%s/movemap", strName);

	float fW = config.nAreaWidth * config.vGridSize;
	float fH = config.nAreaHeight * config.vGridSize;
	float fXMin = -fW * 0.5f, fXMax = fW * 0.5f;
	float fZMin = -fH * 0.5f, fZMax = fH * 0.5f;
	CTerrain terrainData;
	if (terrainData.Init(config, fXMin, fZMin, fXMax, fZMax))
	{
		m_pTriangleMan->BuildFromTerrain(terrainData.GetHeights(), terrainData.GetNumVertX() - 1, terrainData.GetNumVertZ() - 1);
		m_pTriangleMan->Build(A3DVECTOR3(0, 0, 0), TMAN_BUILD_FORCE);
		m_pTriangleMan->SetReadToUse();
		if(0 != m_pShadowRender)
		{
			m_pShadowRender->SetA3DTerrain(m_pTerrain);
			m_pShadowRender->SetCECTriangleMan(m_pTriangleMan);
		}
	}
#endif
}

void Scene::ReleaseTerrain()
{
#ifdef _ANGELICA3
	AppInterface::GetInstance()->SetTerrain(0);
#endif
	if (0 != m_pG3DEngine)
		m_pG3DEngine->UnregesiterTerrain();
	if (0 != m_pCameraCtrl)
		m_pCameraCtrl->SetTerrain(0);
	IAPWalk::gpTerrain = 0;
	if (m_status.ReadFlag(GAME_IS_OWNER_TERRAIN))
	{
		m_status.ClearFlag(GAME_IS_OWNER_TERRAIN);
		A3DRELEASE(m_pTerrain);
	}
	m_pTerrain = 0;
	m_strTrnFile.Empty();
}

void Scene::ChangeCursor(const bool bIsSysDef)
{
	if (bIsSysDef)
		m_pG3DEngine->ShowCursor(0);
	else
		m_pG3DEngine->ShowCursor(m_pCursor);
}

void Scene::RegisterRegionEvent(IUserEvent* pUserEvent)
{
	m_Trigger.Register(pUserEvent);
}

void Scene::UnRegisterRegionEvent(IUserEvent* pUserEvent)
{
	m_Trigger.UnRegister(pUserEvent);
}

void Scene::RegisterTimerEvent(IEventNotify& notify, float delayTime, TimerEvent::DelayType type, int nbEvents)
{
	m_TEventMgr.RegisterEvent(notify, delayTime, type, nbEvents);
}

void Scene::RegisterTimerEvent(TimerEvent* pTEvent)
{
	m_TEventMgr.RegisterEvent(pTEvent);
}

void Scene::UnRegisterTimerEvent(TimerEvent* pTEvent)
{
	m_TEventMgr.RegisterEvent(pTEvent);
}

void Scene::RegisterPhysXContactDamage(ApxActorBase* pActor)
{
	m_ContactReport.RegisterActor(pActor);
}

void Scene::UnRegisterPhysXContactDamage(ApxActorBase* pActor)
{
	m_ContactReport.UnRegisterActor(pActor);
}

void Scene::SavePhysXDemoScene(const char* szFilePath)
{
	if (0 == szFilePath)
		return;

	APhysXUserStream outStream(szFilePath, false);
	SavePhysXDemoScene(outStream);
	if (m_strActiveScene != szFilePath)
		m_strActiveScene = szFilePath;
}

void Scene::SavePhysXDemoScene(APhysXUserStream& saveStream)
{
	A3DCamera* pCamera = m_pG3DEngine->GetCamera();
	assert(0 != pCamera);
	if (0 == pCamera)
		return;

	SaveVersion(&saveStream);
	APhysXSerializerBase::StoreString(&saveStream, m_strTrnFile);

	saveStream.storeBuffer(&pCamera->GetPos(), sizeof(A3DVECTOR3));
	saveStream.storeBuffer(&pCamera->GetDir(), sizeof(A3DVECTOR3));
	saveStream.storeBuffer(&pCamera->GetUp(), sizeof(A3DVECTOR3));

	//scene script
	APhysXSerializerBase::StoreString(&saveStream, m_defScript);
	saveStream.storeDword(m_strScripts.size());
	for (size_t i = 0; i < m_strScripts.size(); i++)
	{
		APhysXSerializerBase::StoreString(&saveStream, m_strScripts[i]);
	}

	ObjManager::GetInstance()->SaveSerialize(saveStream);
	m_pCameraCtrl->SaveSerialize(saveStream);
}

bool Scene::LoadPhysXDemoScene(APhysXUserStream& loadStream, bool& outIsLowVer)
{
	//assert(true == IsReady());

	LoadVersion(&loadStream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVer = true;

	ObjManager* pObjMgr = ObjManager::GetInstance();
	if(m_StreamObjVer < 0xAA000002)
		pObjMgr->SetArchiveStyle(true);
	else
		pObjMgr->SetArchiveStyle(false);

	if (m_StreamObjVer >= 0xAA000001)
	{
		const char* pTrnFile = APhysXSerializerBase::ReadString(&loadStream);
		A3DVECTOR3 vPos, vDir, vUp;	
		loadStream.readBuffer(&vPos, sizeof(A3DVECTOR3));
		loadStream.readBuffer(&vDir, sizeof(A3DVECTOR3));
		loadStream.readBuffer(&vUp, sizeof(A3DVECTOR3));
		m_pCameraCtrl->SetGPos(vPos);
		m_pCameraCtrl->SetGDirAndUp(vDir, vUp);
		LoadTerrainAndPhysXTerrain(pTrnFile);
	}
	if (m_StreamObjVer >= 0xAA000003)
	{
		m_defScript = APhysXSerializerBase::ReadString(&loadStream);
		int nScript = loadStream.readDword();
		for (int i = 0; i < nScript; i++)
		{
			m_strScripts.push_back(APhysXSerializerBase::ReadString(&loadStream));
		}
	}

	if (!pObjMgr->LoadDeserialize(*this, loadStream, outIsLowVer))
	{
		a_LogOutput(1, "无法识别的文件版本！");
		return false;
	}
	if (m_StreamObjVer >= 0xAA000004)
		m_pCameraCtrl->LoadDeserialize(loadStream, outIsLowVer);

	bool bCheckUpdated = pObjMgr->CheckDefPropsUpdate(m_LoadPackage, *m_pPhysXScene);
	//	if (bIsLowVersion || bCheckUpdated)
	//		SavePhysXDemoScene(szFilePath);

	pObjMgr->WakeUp(m_LoadPackage);
	SetSceneMode(SCENE_MODE_PLAY_OFF);
	return true;
}

bool Scene::LoadPhysXDemoScene(const char* szFilePath)
{
	if (szFilePath == 0)
		return false;

	if (!IsReady())
	{
		a_LogOutput(1, "请先初始化后再读取文件！"); 
		return false;
	}
	assert(0 != m_pCameraCtrl);
	SetSceneMode(SCENE_MODE_EMPTY);

	bool bIsLowVersion = false;
	APhysXUserStream inStream(szFilePath, true);
	if (LoadPhysXDemoScene(inStream, bIsLowVersion))
	{
		m_strActiveScene = szFilePath;
//		if (bIsLowVersion)
//			SavePhysXDemoScene(szFilePath);
		return true;
	}
	return false;
}

void Scene::SaveAnObject(const char* szFilePath)
{
	if (0 == szFilePath)
		return;

	IObjBase* pObject = m_SelGroup.GetFocusObject();
	if (0 == pObject)
		return;

	APhysXUserStream outStream(szFilePath, false);
	outStream.storeDword(GetObjDocVersion());

	ObjManager::SaveObject(outStream, pObject);
}

IObjBase* Scene::LoadAnObject(const char* szFilePath)
{
	if (szFilePath == 0)
		return 0;

	if (!IsReady())
	{
		a_LogOutput(1, "请先初始化后再读取文件！"); 
		return 0;
	}
	if (0 == m_pTerrain)
	{
		a_LogOutput(1, "请先加载一个场景！"); 
		return 0;
	}

	bool bIsLowVersion = false;
	APhysXUserStream inStream(szFilePath, true);
	APhysXU32 nObjVer = inStream.readDword();
	if ((nObjVer & 0xffffff00) != (GetObjDocVersion() & 0xffffff00))
		return 0;
	if(nObjVer < GetObjDocVersion())
		bIsLowVersion = true;

	bool bIsLowVerObj = false;
	IObjBase* pObject = ObjManager::LoadObject(*this, inStream, bIsLowVerObj, 0, false, true);
	if (0 != pObject)
	{
		bool bCheckUpdated = false;
		if (ObjManager::IsPhysXObjType(pObject->GetProperties()->GetObjType()))
		{
			IPhysXObjBase* pPhysXObj = dynamic_cast<IPhysXObjBase*>(pObject);
			bCheckUpdated = pPhysXObj->CheckDefPropsUpdate(m_LoadPackage, *m_pPhysXScene);
			//if (bIsLowVersion || bCheckUpdated)
			//	SaveAnObject(szFilePath);
		}
		pObject->WakeUp(m_LoadPackage);
	}
	return pObject;
}

bool Scene::GetCurrentMouseClientPoint(HWND hWnd, POINT& outPT)
{
	AutoData ad(0, 0);
	if (m_pRecording->IsReplaying())
	{
		if (m_pRecording->ReproItemData(RIT_CRNT_MOUSE_PT, &ad))
		{
			outPT.x = ad.szeVal.x;
			outPT.y = ad.szeVal.y;
			return true;
		}
	}

	if (!GetCursorPos(&outPT))
		return false;
	if (!ScreenToClient(hWnd, &outPT))
		return false;

	ad.szeVal.x = outPT.x;
	ad.szeVal.y = outPT.y;
	m_pRecording->LogItemData(RIT_CRNT_MOUSE_PT, ad);
	return true;
}

void Scene::ScreenPointToRay(const int x, const int y, PhysRay& outRay)
{
	if (m_pRecording->GetInstantReproEventScreenRay(x, y, outRay))
		return;
	outRay = ScreenPosToRay(*m_pG3DEngine->GetViewport(), x, y);
}

void Scene::SetSceneMode(const SceneMode& sm)
{
	if (!IsReady())
		return;
	if (QuerySceneMode(sm))
		return;

	switch(sm)
	{
	case SCENE_MODE_EMPTY:
		SetEmptyMode();
		break;
	case SCENE_MODE_PLAY_OFF:
		SetPlayOffMode();
		break;
	case SCENE_MODE_PLAY_ON:
		SetPlayOnMode();
		break;
	default:
		assert(!"Unknown Scene Mode!");
	}
	m_pGMD = &gGMDesc[sm];
}

bool Scene::QuerySceneMode(const SceneMode& sm) const
{
	return (sm == m_pGMD->iState);
}

const TCHAR* Scene::GetSceneModeDesc(bool bIsFullDesc) const
{
	if (bIsFullDesc)
		return m_pGMD->pFullDesc;

	return m_pGMD->pDesc;
}

void Scene::SetEmptyMode()
{
	assert(true == IsReady());
	assert(false == QuerySceneMode(SCENE_MODE_EMPTY));
	EndSimulate();
	ReleaseAllObjects();
	if (0 != m_pPhysXScene)
		m_pPhysXScene->Reset();

#ifdef _ANGELICA3
	m_LoadPackage.GetGFXPhysXSceneShell()->PhysXSceneSwitchOff(*m_pPhysXScene);
#endif
	m_pPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
	m_pPhysXScene = 0;
	CreateAScene();

#ifdef _ANGELICA3
	if (0 != m_pSceneModule)
	{
		m_pECPlatform->ReleaseSceneModule(m_pSceneModule);
		m_pSceneModule = 0;
		ECWorldRender::gpSceneModule = 0;
	}
#endif

	ReleaseTerrain();
	ChangeCursor(true);
	ObjManager::GetInstance()->ClearUniqueSequenceID();
	m_strTrnFile.Empty();
	m_strActiveScene.Empty();
	m_strScripts.clear();
	m_defScript.Empty();
	OnSceneModeChange(SCENE_MODE_EMPTY);
}

void Scene::SetPlayOffMode()
{
	assert(true == IsReady());
	assert(false == QuerySceneMode(SCENE_MODE_PLAY_OFF));
	OnSceneModeChange(SCENE_MODE_PLAY_OFF);
	EndSimulate();
	if (0 != m_pPhysXScene)
		m_pPhysXScene->Reset(false);
}

void Scene::SetPlayOnMode()
{
	assert(true == IsReady());
	assert(false == QuerySceneMode(SCENE_MODE_PLAY_ON));

	m_RHOwner.SetRayHit(0);
	StartSimulate();
	OnSceneModeChange(SCENE_MODE_PLAY_ON);
}

void Scene::StartSimulate()
{
	AString strName;
	MakeRecordingFileName(false, strName);
	m_pRecording->Start(strName);

	assert(0 != m_pPhysXScene);	
	m_TEventMgr.EnterRuntime(*m_pPhysXScene);
	m_Trigger.EnterRuntime(*m_pPhysXScene);
	m_generalAttMgr.ClearAttackQueue(false);

	IAPWalk::gpTerrain = m_pTerrain;
	ActorGroundMoveCtrlImp::gpTerrain = m_pTerrain;
	if (m_status.ReadFlag(GAME_IS_REPROEPI_LOADING))
		m_pG3DEngine->EnableDrawCameraDir(true);
	else
		m_pG3DEngine->EnableDrawCameraDir(true);
	CamOptTask_EntryFreeFlyMode ot(false);
	m_pCamCtrlAgent->ExecuteCameraOperation(ot);
#ifdef _ANGELICA3
	m_MapLoaderCB.EnterRuntime(*m_pPhysXScene);
#endif

	m_SelGroup.EnterRuntime(*m_pPhysXScene);
	ObjManager::GetInstance()->EnterRuntime(*m_pPhysXScene);

#ifdef ENABLE_MULLITHREAD_TEST
		m_pBkGrndTask = new BackgroundTask(m_LoadPackage, *this);
#endif

	m_pPhysXScene->StartSimulate();
	m_MAPushForce = 10;
	ApplyPushForce();

	// We must update terrain firstly for creating terrain in PhysX.
	// The character controller depend on the terrain.
	APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
	if (0 != pPhysXTerrain)
		pPhysXTerrain->UpdateSimCenter(m_pCamCtrlAgent->GetActiveCamera()->GetGPos());
	m_BreakReport.ClearAllBroken();

#ifdef ENABLE_MULLITHREAD_TEST
		m_BreakReport.DisableRecover();
#else
		m_BreakReport.EnableRecover();
#endif
}

void Scene::EndSimulate()
{
	m_pRecording->End();
	m_status.ClearFlag(GAME_IS_PHYSX_SIM_PAUSE);
	m_status.ClearFlag(GAME_IS_PHYSX_SIM_STEP);

	ObjManager::GetInstance()->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);

	m_status.ClearFlag(GAME_IS_PICK_PHYSX_STH);
	m_PhysXPicker.OnMouseButtonUp(0, 0);

	if (0 != m_pBkGrndTask)
	{
		delete m_pBkGrndTask;
		m_pBkGrndTask = 0;
	}

#ifdef _ANGELICA3
	m_MapLoaderCB.LeaveRuntime();
#endif
	m_pG3DEngine->EnableDrawCameraDir(false);
	CamOptTask_EntryFreeFlyMode ot(true);
	m_pCamCtrlAgent->ExecuteCameraOperation(ot);

	m_Trigger.LeaveRuntime();
	m_TEventMgr.LeaveRuntime();
	while (0 < m_MainActors.GetSize())
		ReleaseMainActor(m_MainActors[0]);
	while (0 < m_NPCActors.GetSize())
		ReleaseNPCActor(m_NPCActors[0]);
	ObjManager::GetInstance()->LeaveRuntime();
	m_SelGroup.LeaveRuntime();
	m_generalAttMgr.ClearAttackQueue();
	m_ContactReport.ClearAllRegisteredActors();
	m_BreakReport.ClearAllBroken();

	// have to execute sim to release PhysX objs 
	m_pPhysXEngine->Simulate(1/60.0f);
	assert(0 != m_pPhysXScene); 
	// Note: the type of asynchronism APhysXScene is NOT supported anymore.
	if (0 != m_pPhysXScene)
		m_pPhysXScene->EndSimulate();

}

void Scene::SetDrivenMode(const DrivenMode& dm)
{
	m_iDrivenMode = dm;
	if (IsRuntime())
		ObjManager::GetInstance()->SetDrivenMode(dm);
}

bool Scene::QueryDrivenMode(const DrivenMode& dm) const
{
	return (dm == m_iDrivenMode)? true : false;
}

void Scene::ChangeShootStuffType(int shootID)
{
	if ((0 <= shootID) && (shootID < SSI_END_BOUND))
	{
		m_iSSID = ShootStuffID(shootID);
		return;
	}

	int temp = m_iSSID;
	++temp;
	if (SSI_END_BOUND == temp)
		temp = 0;
	m_iSSID = ShootStuffID(temp);
}

void Scene::ChangeShootMode(int shootMode)
{
	if ((0 <= shootMode) && (shootMode < SM_END_FLAG))
	{
		m_iSMode = ShootMode(shootMode);
		return;
	}

	int temp = m_iSMode;
	++temp;
	if (SM_END_FLAG == temp)
		temp = 0;
	m_iSMode = ShootMode(temp);
}

void Scene::ShootAPhysXObject(HWND hWnd, const float scale, const float fVelocity)
{
	if (!IsRuntime())
		return;

	if (0 == m_pPhysXScene)
		return;

	PhysRay ray;
	APhysXRayHitInfo hitInfo;
	if ((SM_MOUSE_FALLING == m_iSMode) || (SM_EYE_TO_MOUSE == m_iSMode))
	{
		POINT pt;
		if (!GetCurrentMouseClientPoint(hWnd, pt))
			return;
		ScreenPointToRay(pt.x, pt.y, ray);

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

	ObjManager* pObjMgr = ObjManager::GetInstance();
	IObjBase* pObj = pObjMgr->CreateObject(GetObjType(m_iSSID), true, *this);
	if (0 == pObj)
		return;

	IPhysXObjBase* pPhysXObj = static_cast<IPhysXObjBase*>(pObj);
	if (!pPhysXObj->LoadModel(m_LoadPackage, GetObjName(m_iSSID), 0, false))
	{
		pObjMgr->ReleaseObject(pObj);
		return;
	}

	A3DCamera* pA3DCamera = m_pG3DEngine->GetCamera();
	A3DVECTOR3 vEyeDir = pA3DCamera->GetDir();
	A3DVECTOR3 vEyePos = pA3DCamera->GetPos();

	A3DVECTOR3 pos = vEyePos;
	NxVec3 vel = fVelocity * APhysXConverter::A2N_Vector3(vEyeDir);
	if (SM_EYE_TO_MOUSE == m_iSMode)
		vel = fVelocity * APhysXConverter::A2N_Vector3(ray.vDelta);
	//	else if (SM_KINEMATIC == iSMode)
	//		pos += pA3DCamera->GetDir() * 5;
	else if (SM_FRONT_FALLING == m_iSMode)
	{
		vel.zero();
		pos += pA3DCamera->GetDir() * 10;
		float y = pos.y;
		GetTerrainHeight(m_pTerrain, pos, y);
		if (y > pos.y) pos.y = y + 2; 
	}
	else if (SM_MOUSE_FALLING == m_iSMode)
	{
		vel.zero();
		pos = APhysXConverter::N2A_Vector3(hitInfo.mHitPos);
		pos.y += 4;
	}

	pPhysXObj->SetLPos(pos);
	pPhysXObj->SetLScale(scale);
	IPropPhysXObjBase* pProp = pPhysXObj->GetProperties();
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	pProp->GetFlags().RaiseFlag(OBF_IS_SHOT_STUFF);
	pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);
	pObj->EnterRuntime(*m_pPhysXScene);

	if (ObjManager::OBJ_TYPEID_FORCEFIELD != pProp->GetObjType())
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pPhysXObj);
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

void Scene::EnableShowTerrianGrid(const bool bShow)
{
	if (bShow)
		m_status.RaiseFlag(GAME_DRAW_TERRAIN_GRID);
	else
		m_status.ClearFlag(GAME_DRAW_TERRAIN_GRID);

	if (0 != m_pPhysXScene)
		m_pPhysXScene->EnableTerrainDebugRender(bShow);	
	if (0 != m_pG3DEngine)
		m_pG3DEngine->DrawPhysXTerrainRange(A3DVECTOR3(0, 0, 0), 0, 0);
}

void Scene::EnablePhysXProfile(const bool bEnable)
{
	if (bEnable)
		m_status.RaiseFlag(GAME_DRAW_PHYSX_PRIFILE);
	else
		m_status.ClearFlag(GAME_DRAW_PHYSX_PRIFILE);

	if (0 != m_pPhysXScene)
		m_pPhysXScene->EnablePerfStatsRender(bEnable);
}

void Scene::EnablePhysXDebugRender(const bool bEnable)
{
	if (bEnable)
	{
		m_pPhysXEngine->EnableSDKDebugRender(true);
		m_status.RaiseFlag(GAME_DRAW_PHYSX_DBGRENDER);
	}
	else
	{
		m_pPhysXEngine->EnableSDKDebugRender(false);
		m_status.ClearFlag(GAME_DRAW_PHYSX_DBGRENDER);
	}

	if (0 != m_pPhysXScene)
		m_pPhysXScene->EnableDebugRender(bEnable);
	if (0 != m_pG3DEngine)
		m_pG3DEngine->DrawPhysXTerrainRange(A3DVECTOR3(0, 0, 0), 0, 0);
	ApxAttackEvent::EnableDebugRender(bEnable);
}

void Scene::EnablePhysXHW(const bool bEnable)
{
	if (bEnable)
		m_status.RaiseFlag(GAME_ENABLE_PHYSX_HW);
	else
		m_status.ClearFlag(GAME_ENABLE_PHYSX_HW);

	if(0 != m_pPhysXEngine)
		m_pPhysXEngine->EnableHardwarePhysX(bEnable);
}

const char* Scene::GetActiveSceneFile() const
{
	if (m_strActiveScene.IsEmpty())
		return 0;

	return m_strActiveScene; 
}

bool Scene::PhysXSimASingleStep()
{
	if (IsPhysXSimPauseState())
	{
		m_status.RaiseFlag(GAME_IS_PHYSX_SIM_STEP);
		return true;
	}
	return false;
}

void Scene::ChangePhysXPauseState() 
{
	if (IsRuntime())
	{
		if (m_status.ReadFlag(GAME_IS_PHYSX_SIM_PAUSE))
			m_status.ClearFlag(GAME_IS_PHYSX_SIM_PAUSE);
		else
			m_status.RaiseFlag(GAME_IS_PHYSX_SIM_PAUSE);
	}
}

bool Scene::IsPhysXSimEnable() const
{
	if (!m_status.ReadFlag(GAME_IS_PHYSX_SIM_PAUSE))
		return true;
	if (m_status.ReadFlag(GAME_IS_PHYSX_SIM_STEP))
		return true;
	return false;
}

void Scene::GetAllAssailablePhysXObj(IPhysXObjBase* pCaster, APtrArray<IAssailable*>& outArray)
{
	IAssailable* pIA = 0;
	IPhysXObjBase* pObj = 0;
	APtrArray<IObjBase*> objArray;
	int nCount = ObjManager::GetInstance()->GetLiveObjects(objArray, ObjManager::OBJ_TYPEID_ECMODEL);
	for (int i = 0; i < nCount; ++i)
	{
		pIA = 0;
		pObj = dynamic_cast<IPhysXObjBase*>(objArray[i]);
		if (0 == pObj)
			continue;
		if (pObj == pCaster)
			continue;
		if (0 != pCaster->GetRelationshipCode(*pObj))
			continue;
		pIA = pObj->GetAssailableInterface();
		if (0 != pIA)
			outArray.Add(pIA);
	}
}

IPhysXObjBase* Scene::GetPhysXObj(const char* objName)
{
	IObjBase* pObj = ObjManager::GetInstance()->FindObj(objName);
	IPhysXObjBase* pPhysXObj = dynamic_cast<IPhysXObjBase*>(pObj);
	return pPhysXObj;
}

bool Scene::GetTerrainPosByCursor(const int x, const int y, A3DVECTOR3& vOutPos)
{
	if (!m_status.ReadFlag(GAME_INIT_ENDOK))
		return false;

	if (0 == m_pTerrain)
		return false;

	PhysRay ray;
	ScreenPointToRay(x, y, ray);

	RAYTRACERT rt;
	if (!m_pTerrain->RayTrace(ray.vStart, ray.vDelta, 1.0f, &rt))
	{
		A3DVECTOR3 vPos = ray.vStart + 50 * ray.vDelta;
		float fHeight = 0.0;
		if (GetTerrainHeight(m_pTerrain, vPos, fHeight))
			vPos.y = fHeight;
		rt.vHitPos = vPos;
	}

	vOutPos = rt.vHitPos;
	return true;
}

void Scene::ChangeCameraMode(bool bEnforceSyncReplayCam)
{
	if (bEnforceSyncReplayCam)
	{
		if (!m_pCamCtrlAgent->IsTheSameWithReplayCamera())
		{
			m_pCamCtrlAgent->EnforceSyncReplayCamera();
			return;
		}
	}

	const CCameraController* pActiveCam = m_pCamCtrlAgent->GetActiveCamera(true);
	if (pActiveCam->QueryMode(MODE_BINDING))
	{
		CamOptTask_EntryFreeFlyMode ot(false);
		m_pCamCtrlAgent->ExecuteCameraOperation(ot);
		return;
	}

	if (0 != m_pCurrentMA)
	{
		CamOptTask_EntryBindingMode ot(m_pCurrentMA->GetDynObj());
		m_pCamCtrlAgent->ExecuteCameraOperation(ot);
	}
}

bool Scene::Tick(float dtSec)
{
	if (gTest1KActors && gHas1KActors)
	{
		float s= 0.01f;
		float d= 0.0f;
		NxVec3 pos;
		for (int i =0; i<1000;++i)
		{
			pos = pActor[i]->getGlobalPosition();
			pos.y += s;
			d+=s;
			pActor[i]->moveGlobalPosition(pos);
			//		pActor[i]->setGlobalPosition(pos);
		}
		if (abs(d) > 2)
		{
			d = 0.0f;
			s*= -1;
		}
	}

	assert(0 != GetTerrain());
	unsigned int dtMSec = unsigned int(dtSec * 1000); // note: only used for angelica tick, don't used for PhysX tick/sim
	MoveLight(dtMSec);
	if (!IsPhysXSimEnable())
	{
		m_pCamCtrlAgent->Tick(dtSec, true);
		return false;
	}

	if (m_pRecording->IsReplaying())
	{
		float fPer = 0;
		if (m_pRecording->IsReplayLoadingEpisode(&fPer))
		{
			m_pG3DEngine->DrawProgressBar(fPer, m_pG3DEngine->GetXCenter(), m_pG3DEngine->GetYCenter(), A3DCOLORRGB(0, 192, 0), 100, 12);
			m_status.RaiseFlag(GAME_IS_REPROEPI_LOADING);
		}
		else
		{
			m_pG3DEngine->ClearProgressBar();
			m_status.ClearFlag(GAME_IS_REPROEPI_LOADING);
		}
	}

	if (IsPhysXSimPauseState())
		m_status.ClearFlag(GAME_IS_PHYSX_SIM_STEP);
	if (m_pRecording->IsReplayEnd())
	{
		m_pCamCtrlAgent->Tick(dtSec, true);
		if (!IsPhysXSimPauseState())
			ChangePhysXPauseState();
		m_pRecording->UpdateTickForRec(dtSec);
		m_status.ClearFlag(GAME_IS_REPROEPI_LOADING);
		return false;
	}

	if (0 != m_pCurrentMA && IsRuntime())
	{
		if (m_pCurrentMA->IsKeepWalking())
		{
	/*		int x, y;     // to update handle repro
			if (m_pCurrentMA->GetTargetPT(x, y))
			{
				A3DVECTOR3 vHitPos;
				if (GetTerrainPosByCursor(x, y, vHitPos))
					m_pCurrentMA->WalkTo(vHitPos);
			}*/
		}
	}

	GetAPhysXScene()->GetPerfStats()->StartPerfStat("Tick MA");
	for (int k = 0; k < m_MainActors.GetSize(); ++k)
		m_MainActors[k]->TickMA(dtSec);
	for (int k = 0; k < m_NPCActors.GetSize(); ++k)
		m_NPCActors[k]->TickMA(dtSec);
	GetAPhysXScene()->GetPerfStats()->EndPerfStat();

#ifdef _ANGELICA3
	A3DGFXExMan* pGfxExMan = m_LoadPackage.GetGFXEngine()->GetA3DGfxExMan();
	if (0 != pGfxExMan)
		pGfxExMan->Tick(dtMSec);
#else
	AfxGetGFXExMan()->Tick(dtMSec);
	_SGC::AfxGetSkillGfxEventMan()->Tick(dtMSec);
#endif
	
	GetAPhysXScene()->GetPerfStats()->StartPerfStat("Tick Attack Manager");
	m_generalAttMgr.Tick(dtSec);
	GetAPhysXScene()->GetPerfStats()->EndPerfStat();
	m_ContactReport.ClearCRHit();
	m_TEventMgr.TickTimerEvents(dtSec);
	m_BreakReport.TickBroken(dtSec);

	GetAPhysXScene()->GetPerfStats()->StartPerfStat("Tick Objs");
	ObjManager::GetInstance()->Tick(dtSec);
	GetAPhysXScene()->GetPerfStats()->EndPerfStat();

	if (0 != m_pBkGrndTask)
		m_pBkGrndTask->Tick(dtSec);

	m_pCamCtrlAgent->Tick(dtSec, false);
	
	APhysXCharacterControllerManager* pCCM = m_pPhysXEngine->GetAPhysXCCManager();
	if (0 != pCCM)
		pCCM->Tick();

	A3DCamera& Camera = m_pCamCtrlAgent->GetActiveCamera()->GetCamera();
	A3DVECTOR3 vCamPos = Camera.GetPos();
	A3DVECTOR3 vHostPos = vCamPos;
	if (0 != m_pCurrentMA)
		vHostPos = m_pCurrentMA->GetPos();
	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())  // "OnSimulate" means in the physX state instead of edit state 
	{
		APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
		if (0 != pPhysXTerrain)
			pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(vHostPos));
	}

	DWORD dwNowTime = 0;
	if (0 < m_RPTOutputTickNum)
		dwNowTime = a_GetTime();
	m_pPhysXEngine->Simulate(dtSec);
	if (0 < m_RPTOutputTickNum)
	{
		dwNowTime = a_GetTime() - dwNowTime;
		a_LogOutput(1, "index = %d, APhysX Engine Tick Time: %d ms, RPT Mode %s", 3 - m_RPTOutputTickNum, dwNowTime, GetRPTModeText());
		--m_RPTOutputTickNum;
	}

	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
		ObjManager::GetInstance()->SyncDataPhysXToGraphic();
	m_pRecording->UpdateTickForRec(dtSec);
	m_pRecording->ReproAllEvents();

	m_Trigger.RegionEvents();
#ifdef _ANGELICA3
	if (0 != m_pSceneModule)
	{
		m_pSceneModule->Tick(dtMSec, vHostPos, vCamPos);
	}
	else
	{
		m_pTerrain->Update(dtMSec, vCamPos);
		m_pG3DEngine->GetA3DEngine()->Tick(dtMSec);
	}
	m_pA3DSky->Tick(dtMSec);
#else
	m_pTerrain->Update(dtMSec, &Camera, vCamPos);
	m_pG3DEngine->GetA3DEngine()->TickAnimation();
	m_pA3DSky->TickAnimation();
#endif

	static const float TIME_TICKANIMATION =	0.03f;
	static float AnimTime = 0;
	if ((AnimTime += dtSec) >= TIME_TICKANIMATION)
	{
		AnimTime -= TIME_TICKANIMATION;
		Camera.UpdateEar();
	}

	if (gToStartSim)
	{
		m_pPhysXScene->StartSimulate();
		gToStartSim = false;
	}
	if (gToEndSim)
	{
		m_pPhysXScene->EndSimulate();
		gToEndSim = false;
	}

	int a = m_pPhysXScene->GetNxScene()->getNbActors();

	return true;
}

void Scene::MoveLight(DWORD dwTime)
{
	if (m_pRecording->QueryAsyncKeyDown(VK_RSHIFT, true))
		return;

	float fDeltaTime = dwTime * 0.001f;
	float fRotSpeed = 90.0f * fDeltaTime;	//	unit: 90 degree/second
	float fRot1 = 0.0f, fRot2 = 0.0f;

	if (m_pRecording->QueryAsyncKeyDown(VK_LEFT, true))
		fRot1 = -fRotSpeed;
	else if (m_pRecording->QueryAsyncKeyDown(VK_RIGHT, true))
		fRot1 = fRotSpeed;

	if (m_pRecording->QueryAsyncKeyDown(VK_UP, true))
		fRot2 = fRotSpeed;
	else if (m_pRecording->QueryAsyncKeyDown(VK_DOWN, true))
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
#ifndef _ANGELICA3
		A3DSceneRenderConfig* pRenderCfg = m_pG3DEngine->GetA3DEngine()->GetSceneRenderConfig();
		pRenderCfg->SetSunLightDir(A3DVECTOR4(lp.Direction.x, lp.Direction.y, lp.Direction.z, 0));
		pRenderCfg->SetSunLightColor(lp.Diffuse);
#endif
	}
}

bool Scene::GetHitObjectFileName(AString& outstrFilePath, POINT* poutPT)
{
	outstrFilePath.Empty();
	IObjBase* pObj = m_RHOwner.GetRayHit(poutPT);
	if (0 == pObj)
		return false;

	IPropObjBase* pProp = pObj->GetProperties();
	if (!ObjManager::IsPhysXObjType(pProp->GetObjType()))
		return false;

	IPhysXObjBase* pPhysXObj = static_cast<IPhysXObjBase*>(pObj);
	outstrFilePath = pPhysXObj->GetProperties()->GetFilePathName();
	return true;
}

bool Scene::Render()
{
	if (0 == m_pG3DEngine)
		return true;

	if (!m_pG3DEngine->BeginRender())
		return false;

	GetAPhysXScene()->GetPerfStats()->StartPerfStat("Render");
	if (!m_status.ReadFlag(GAME_IS_REPROEPI_LOADING))
	{
		if (0 != GetTerrain())
		{
			A3DEngine* pA3DEngine = m_pG3DEngine->GetA3DEngine();
			A3DDevice* pA3DDevice = m_pG3DEngine->GetA3DDevice();
			A3DViewport* pA3DViewport = m_pG3DEngine->GetViewport();
#ifndef _ANGELICA3
			A3DSkinRender* psr1 = pA3DEngine->GetA3DSkinMan()->GetDefSkinRender();

			A3DCOLOR fogColor = pA3DViewport->GetClearColor();
			pA3DDevice->Clear(D3DCLEAR_TARGET, fogColor | A3DCOLORRGBA(0, 0, 0, 255), 1.0f, 0);
			A3DLitModelRender* pLitRender = pA3DEngine->GetA3DLitModelRender();
			pLitRender->SetAlphaMeshesArray(0);
			pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

			A3DSky* pSky = m_pG3DEngine->GetSky();
			if (0 != pSky)
			{
				pSky->SetCamera(m_pG3DEngine->GetCamera());
				pSky->Render();
			}

			A3DSceneRenderConfig* pRenderCfg = m_pG3DEngine->GetA3DEngine()->GetSceneRenderConfig();
			const CCameraController* pCtrlCam = m_pCamCtrlAgent->GetActiveCamera();

			BYTE nRenderShadowMap = 1;
			bool bUpdateShadowMap = false;
			bool bRenderSkinModelShadow = false;
			bRenderSkinModelShadow = true;

			static A3DVECTOR3 gUpdatedShadowCenter(FLT_MIN, FLT_MIN, FLT_MIN);
			A3DVECTOR3 vShadowCenter;
			if (0 != m_pCurrentMA)
				vShadowCenter = m_pCurrentMA->GetPos();
			else
				vShadowCenter = APhysXConverter::N2A_Vector3(pCtrlCam->GetGPos());

			float fDist = (vShadowCenter - gUpdatedShadowCenter).Magnitude();
			if (fDist > 12)
			{
				gUpdatedShadowCenter = vShadowCenter;
				bUpdateShadowMap = true;
			}
			else
			{
				if (pRenderCfg->EnableStaticShadow(2048, 80, 60))
					bUpdateShadowMap = true;
			}
			pLitRender->SetCurShadowCenter(vShadowCenter);
			GetTerrain()->SetCurShadowCenter(vShadowCenter);
			const A3DVECTOR3& vSunLightDir = *(const A3DVECTOR3*)&pRenderCfg->GetSunLightDir();
			if (0 != m_pShadowRender)
			{
				if (!pRenderCfg->IsDynShadowEnabled() || !m_pShadowRender->GetCastShadowFlag() || m_pShadowRender->GetSimpleShadowFlag())
					bRenderSkinModelShadow = false;

				if (bRenderSkinModelShadow)
				{
					A3DShadowMapEx* pDynShadowMap = pRenderCfg->GetDynShadowMap();
					A3DViewport* pShadowViewport = pDynShadowMap->GetShadowViewport();
					m_pShadowRender->AdjustShadowRadius(pCtrlCam->GetFocusFactorPercent());
					pDynShadowMap->SetRadius(m_pShadowRender->GetViewportSize(), m_pShadowRender->GetFadeStart());
					pDynShadowMap->SetShadowCamera(vShadowCenter, vSunLightDir);
					pDynShadowMap->ApplyShadowParams();
					psr1->SetRenderForShadow(true);

					m_pG3DEngine->RenderForShadowMap(pShadowViewport);
					psr1->RenderShadowMap(pDynShadowMap);
					psr1->ResetRenderInfo(false);
					psr1->ResetRenderInfo(true);
					psr1->SetRenderForShadow(false);
				}
				pLitRender->SetDNFactor(1.0f);
				if (!pRenderCfg->IsStaticShadowEnabled())
					nRenderShadowMap = 0;
				if (nRenderShadowMap > 0)
				{
					if (bUpdateShadowMap)
					{
						m_pG3DEngine->RenderOrnaments(true, 0);
						pLitRender->RenderShadowMap(gUpdatedShadowCenter, vSunLightDir);
						GetTerrain()->RenderShadowMap();
						pLitRender->ResetRenderInfo(false);
						pLitRender->ResetRenderInfo(true);
					}
					pLitRender->SetUseShadowMap(true);
				}
				else
					pLitRender->SetUseShadowMap(false);
				if (bRenderSkinModelShadow)
					m_pG3DEngine->RenderOrnaments(false, 1);
				if (nRenderShadowMap > 0)
				{
					if (bRenderSkinModelShadow)
						m_pG3DEngine->RenderOrnaments(false, 2);
					m_pG3DEngine->RenderOrnaments(false, 3);
				}
				m_pG3DEngine->RenderOrnaments(false, 4);
				if (nRenderShadowMap > 0)
				{
					if (bRenderSkinModelShadow)
					{
						pLitRender->Render(pA3DViewport, false, true, true, 0);
						pLitRender->Render(pA3DViewport, false, true, true, 2);
					}
					else
					{
						pLitRender->Render(pA3DViewport, false, false, true, 0);
						pLitRender->Render(pA3DViewport, false, false, true, 2);
					}
				}
				else if(bRenderSkinModelShadow)
				{
					pLitRender->Render(pA3DViewport, false, true, false, 0);
					pLitRender->Render(pA3DViewport, false, true, false, 2);
				}
				else
				{
					pLitRender->Render(pA3DViewport, false, false, false, 0);
					pLitRender->Render(pA3DViewport, false, false, false, 2);
				}


				GetTerrain()->SetDNFactor(1.0f);
				GetTerrain()->Render(pA3DViewport, bRenderSkinModelShadow, nRenderShadowMap > 0);
				if (nRenderShadowMap > 0)
				{
					if (bRenderSkinModelShadow)
					{
						pLitRender->Render(pA3DViewport, false, true, false, 1);
						pLitRender->Render(pA3DViewport, false, true, false, 3);
					}
					else
					{
						pLitRender->Render(pA3DViewport, false, false, false, 1);
						pLitRender->Render(pA3DViewport, false, false, false, 3);
					}
				}
				else if(bRenderSkinModelShadow)
				{
					pLitRender->Render(pA3DViewport, false, true, false, 1);
					pLitRender->Render(pA3DViewport, false, true, false, 3);
				}
				else
				{
					pLitRender->Render(pA3DViewport, false, false, false, 1);
					pLitRender->Render(pA3DViewport, false, false, false, 3);
				}
				pLitRender->ResetRenderInfo(false);
			}
			pA3DDevice->SetAlphaBlendEnable(false);
			m_pG3DEngine->RenderStuff();
			pA3DDevice->SetAlphaBlendEnable(false);
			psr1->Render(pA3DViewport, false, nRenderShadowMap > 0, bRenderSkinModelShadow);
			psr1->ResetRenderInfo(false);

			pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
			pA3DEngine->FlushCachedAlphaMesh(pA3DViewport);
			pA3DDevice->SetAlphaBlendEnable(true);

			pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
			pA3DDevice->ClearTexture(1);

			psr1->RenderAlpha(pA3DViewport, nRenderShadowMap > 0, bRenderSkinModelShadow);
			pA3DDevice->SetZWriteEnable(false);
			pLitRender->RenderAlpha(pA3DViewport);
			pA3DDevice->SetAlphaBlendEnable(true);
			pA3DDevice->SetZWriteEnable(true);

			pLitRender->ResetRenderInfo(true);
			psr1->ResetRenderInfo(true);
#endif

#ifdef _ANGELICA3
			// key render routine...
			m_pG3DEngine->RenderStuff();

			// Render Gfx
			A3DGFXExMan* pGfxExMan = m_LoadPackage.GetGFXEngine()->GetA3DGfxExMan();
#else
			A3DGFXExMan* pGfxExMan = AfxGetGFXExMan();
#endif
			if (0 != pGfxExMan)
			{
#ifdef _ANGELICA3
				pGfxExMan->RenderAllGfxSkinModel(pA3DViewport);
				bool bAlphaBlend = pA3DDevice->GetAlphaBlendEnable();
				pA3DDevice->SetAlphaBlendEnable(true);
				A3DSkinRender* pSkinRender = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
				if (0 != pSkinRender)
					pSkinRender->Render(pA3DViewport, A3DSkinRender::RD_ALL);
				pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
#else
				A3DSkinRender* psr = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
				A3DLitModelRender* plmr = pA3DEngine->GetA3DLitModelRender();
				//	Flush non-alpha objects
				pA3DDevice->SetAlphaBlendEnable(false);
				pA3DDevice->SetAlphaTestEnable(true);
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
				pA3DEngine->RenderGFX(pA3DViewport, 0xffffffff);
#endif
				pGfxExMan->RenderAllGfx(pA3DViewport);
			}
			m_generalAttMgr.Render();

			const bool bIsDebugRender = IsPhysXDebugRenderEnabled();
			// render the current PhysX Terrain's range...
			if (IsShowTerrianGridEnabled() && (0 != m_pPhysXScene))
			{
				if (bIsDebugRender)
				{
					APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
					if ((0 != pPhysXTerrain) && (0 != m_pTerrain))
					{
						A3DVECTOR3 vProjPos = APhysXConverter::N2A_Vector3(m_pCamCtrlAgent->GetActiveCamera()->GetGPos());
						GetTerrainHeight(m_pTerrain, vProjPos, vProjPos.y);
						m_pG3DEngine->DrawPhysXTerrainRange(vProjPos, pPhysXTerrain, m_pTerrain);
					}	
				}
			}

			// render the reference frame of selected objects...
			A3DWireCollector* pWC = m_pG3DEngine->GetWCTopZBUf();
			if (0 == pWC)
				pWC = pA3DEngine->GetA3DWireCollector();
			m_SelGroup.DrawFrame(pWC);

			const int nSize = m_MainActors.GetSize();
			for (int i = 0; i < nSize; ++i)
				m_MainActors[i]->Render(*pA3DEngine->GetA3DWireCollector(), bIsDebugRender, m_pCurrentMA);
			const int nNPCSize = m_NPCActors.GetSize();
			for (int i = 0; i < nNPCSize; ++i)
				m_NPCActors[i]->Render(*pA3DEngine->GetA3DWireCollector(), bIsDebugRender);

			if (bIsDebugRender)
				CBrushesObjMgr::GetInstance()->Render(pA3DEngine->GetA3DFlatCollector());

//			if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
			if (m_pPhysXScene)
			{
				static APhysXDebugRenderDesc desc;
				desc.nSceneStatsTextStartX = 700;
				desc.nSceneStatsTextStartY = 10;
				m_pPhysXScene->DebugRender(&desc);
			}

			POINT pt;
			AString strName;
			if (GetHitObjectFileName(strName, &pt))
			{
				af_GetFileTitle(strName, strName);
				m_pG3DEngine->TextOut(pt.x, pt.y - 15, _TAS2WC(strName), strName.GetLength(), 0xff00ff00);
			}
		}

		bool bIsDrawRecText = false;
		AString strRecText;
		if (m_pRecording->IsRecording())
		{
			bIsDrawRecText = true;
			strRecText = "Recording";
		}
		if (m_pRecording->IsReplaying())
		{
			bIsDrawRecText = true;
			strRecText = "Replaying";
		}
		if (bIsDrawRecText)
		{
			int x = m_pG3DEngine->GetXCenter() * 2 - 75;
			int y = 30;
			m_pG3DEngine->TextOut(x, y, _TAS2WC(strRecText), strRecText.GetLength(), 0xff00ff00);
		}

		// render some ik info...
		if(m_pCurrentMA && m_pCurrentMA->GetActorBase())
		{
			static const TString strEnable = _T("IKYawCorrection: Enabled");
			static const TString strDiable = _T("IKYawCorrection: Disabled");

			ApxActorBase* pActor = m_pCurrentMA->GetActorBase();
			const TString* pIKStr = pActor->IsIKYawCorrectionEnabled()? &strEnable : &strDiable;
			
			int x = m_pG3DEngine->GetXCenter() * 2 - 200;
			int y = 50;
			m_pG3DEngine->TextOut(x, y, *pIKStr, pIKStr->GetLength(), 0xffffff00);

			TString strIKMax;
			strIKMax.Format(_T("Max IK Pitch Degree:%.2f"), pActor->mFootIKMaxPitchDeg);
			y+= 20;
			m_pG3DEngine->TextOut(x, y, strIKMax, strIKMax.GetLength(), 0xffffff00);
		}

		//render extra data
		if (_renderExtra)
			_renderExtra(m_pG3DEngine);

#ifdef _ANGELICA3
		AppInterface::GetInstance()->Render(m_pG3DEngine->GetA3DEngine()->GetA3DWireCollector());
#endif
	}
	GetAPhysXScene()->GetPerfStats()->EndPerfStat();

	//end render
	return m_pG3DEngine->EndRender();
}

IPhysXObjBase* Scene::CreateObjectAndLoad(const ObjID objType, const char* szFile)
{
	IObjBase* pObject = ObjManager::GetInstance()->CreateObject(objType, true, *this);
	if (0 == pObject)
		return 0;
	
	IPhysXObjBase* pPhysXObject = static_cast<IPhysXObjBase*>(pObject);
	IPropPhysXObjBase* pObjProps = pPhysXObject->GetProperties();
	pObjProps->SetDrivenMode(m_iDrivenMode);
	bool IsNonRuntime = !IsRuntime();
	if (!pPhysXObject->LoadModel(m_LoadPackage, szFile, m_pPhysXScene, IsNonRuntime))
	{
		ObjManager::GetInstance()->ReleaseObject(pObject);
		return 0;
	}

	if (!IsNonRuntime)
		pObjProps->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	return pPhysXObject;
}

bool Scene::CreateFFSubObject()
{
	if (IsRuntime())
		return false;
	IObjBase* pObj = m_SelGroup.GetFocusObject();
	if (0 == pObj)
		return false;
	if (ObjManager::OBJ_TYPEID_REGION != pObj->GetProperties()->GetObjType())
		return false;
	CRegion* pRgn = dynamic_cast<CRegion*>(pObj);

	pObj = CreateObjectAndLoad(ObjManager::OBJ_TYPEID_FORCEFIELD, AString());
	if (0 == pObj)
		return false;
	IPropObjBase* pProp = pObj->GetProperties();
	pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);

	CRegion::ShapeType st = pRgn->GetShapeType();
	A3DVECTOR3 vol = pRgn->GetVolume(true);
	CPhysXObjForceField* pFF = dynamic_cast<CPhysXObjForceField*>(pObj);
	if (!pFF->SetShapeInfo(st, vol))
	{
		ReleaseObject(pFF);
		return false;
	}

	pRgn->AddChild(pObj);
	A3DMATRIX4 pose(A3DMATRIX4::IDENTITY);
	pObj->SetLPose(pose);
	return true;
}

IPhysXObjBase* Scene::CreateObjectRuntime(const ObjID objType, const char* szFile, const A3DVECTOR3& pos, const bool OnGround, const bool isEnterRuntime)
{
	IPhysXObjBase* pObj = CreateObjectAndLoad(objType, szFile);
	if (0 == pObj)
		return 0;

	A3DVECTOR3 aPos = pos;
	if (OnGround)
		GetTerrainHeight(m_pTerrain, aPos, aPos.y);

	pObj->SetGPos(aPos);
	IPropObjBase* pProp = pObj->GetProperties();
	pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
	pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	if (isEnterRuntime)
		pObj->EnterRuntime(*m_pPhysXScene);

	return pObj;
}

IObjBase* Scene::CreateObjectRuntime(const ObjID objType, const A3DVECTOR3& pos, const bool isEnterRuntime)
{
	ObjManager* pObjMgr = ObjManager::GetInstance();
	if (pObjMgr->IsPhysXObjType(objType))
		return false;

	if (ObjManager::OBJ_TYPEID_EMPTY == objType)
		return false;

	IObjBase* pObj = pObjMgr->CreateObject(objType, true, *this);
	if (0 == pObj)
		return 0;

	pObj->SetGPos(pos);
	IPropObjBase* pProp = pObj->GetProperties();
	pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	if (isEnterRuntime)
		pObj->EnterRuntime(*m_pPhysXScene);
	return pObj;
}

void Scene::ReleaseObject(IObjBase* pObject)
{
	ObjManager::GetInstance()->ReleaseObject(pObject); 
}

void Scene::ReleaseAllObjects()
{
	ObjManager::GetInstance()->ReleaseAllRootObjects(); 
}

ApxActorBase* Scene::GetActorBase(IObjBase* pObj) const
{
	if (0 == pObj)
		return 0;

	int nCount = m_NPCActors.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_NPCActors[i]->GetDynObj() == pObj)
			return m_NPCActors[i]->GetActorBase();
	}
	nCount = m_MainActors.GetSize();
	for (int j = 0; j < nCount; ++j)
	{
		if (m_MainActors[j]->GetDynObj() == pObj)
			return m_MainActors[j]->GetActorBase();
	}
	return 0;
}

void Scene::OnObjectDeletionNotify(APhysXObject* pObject)
{
	if (0 == pObject)
		return;

	if (!pObject->IsObjectType(APX_OBJTYPE_FORCEFIELD_EXPLOSION))
		return;

	ObjManager::GetInstance()->OnDeletionNotify(*pObject);
}

IRoleActor* Scene::CreateNPCRole(CPhysXObjDynamic& dynObj)
{
	int idx = -1;
	const int nSize = m_NPCActors.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (&dynObj == m_NPCActors[i]->GetDynObj())
			return m_NPCActors[i];
	}

	CDynProperty* pProp = dynObj.GetProperties();
	assert(CDynProperty::ROLE_ACTOR_NPC == pProp->GetRoleType());
	ApxActorBase* pActorBase = new ApxActorBase();
	if (0 == pActorBase)
		return 0;

	NxVec3 ext = pProp->GetCCMgr().GetDefaultVolume();
	ext.y *= 0.5f;
	pActorBase->Init(&dynObj, ext);

	IRoleActor* pNPC = new IRoleActor(*pActorBase);
	if (0 == pNPC)
	{
		delete pActorBase;
		return 0;
	}

	m_NPCActors.Add(pNPC);
	return pNPC;
}

IRoleActor* Scene::CreateNPCActor(const char* szFile, const A3DVECTOR3* pPos)
{
	IPhysXObjBase* pObj = CreateObjectRuntime(ObjManager::OBJ_TYPEID_ECMODEL, szFile, *pPos, true, false);
	if (0 == pObj)
		return 0;

	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
	if (0 == pDynObj)
		return 0;

	CDynProperty* pProp = pDynObj->GetProperties();
	pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);
	pProp->SetScriptNames("Monster.cs");
//	pProp->SetScriptNames("PlayActionSequence.cs");
	pProp->SetRoleType(CDynProperty::ROLE_ACTOR_NPC);
	pProp->SetAPType(IActionPlayerBase::AP_NULL);
	pProp->GetCCMgr().SwtichCC(CAPhysXCCMgr::CC_BRUSH_CDRCC);
	pDynObj->EnterRuntime(*m_pPhysXScene);
	return CreateNPCRole(*pDynObj);
}

void Scene::ReleaseNPCActor(IRoleActor* pRA)
{
	if (0 == pRA)
		return;

	int idx = -1;
	const int nSize = m_NPCActors.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (pRA == m_NPCActors[i])
			idx = i;
	}

	assert(0 <= idx);
	if (0 <= idx)
	{
		ApxActorBase* pActorBase = m_NPCActors[idx]->GetActorBase();
		if (0 != pActorBase)
			pActorBase->Release();
		delete pActorBase;
		delete m_NPCActors[idx];
		m_NPCActors.RemoveAtQuickly(idx);
	}
}

CMainActor* Scene::CreateMainActor(const A3DVECTOR3& pos, const char* szFile)
{
	if (gTest1KActors)
	{
		AString str;
		str.Format("call CreateMainActor: Has 1k actors = %d\n", gHas1KActors);
		OutputDebugStringA(str);
		NxScene* pScene = m_pPhysXScene->GetNxScene();
		NxBodyDesc bodyDesc;
		bodyDesc.flags |= NX_BF_KINEMATIC;
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions = NxVec3(0.1f);
		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&boxDesc);
		actorDesc.body			= &bodyDesc;
		actorDesc.density		= 1.0f;
		actorDesc.globalPose.t  = GetCamCtrlAgent()->GetActiveCamera()->GetGPos();
		for (int i = 0; i < 1000; ++i)
		{
			actorDesc.globalPose.t.x += 0.6f;
			pActor[i] = pScene->createActor(actorDesc);
		}
		gHas1KActors =true;
		return 0;
	}

#ifdef _ANGELICA3
	static const char* pDefFile = "Models\\Players\\形象\\女\\躯干\\人族女.ecm3";
#else
	static const char* pDefFile = "Models\\Players\\形象\\男\\躯干\\男.ecm";
#endif
	const char* pFile = pDefFile;
	if (0 != szFile)
		pFile = szFile;

	IPhysXObjBase* pObj = CreateObjectRuntime(ObjManager::OBJ_TYPEID_ECMODEL, pFile, pos, false, false);
	if (0 == pObj)
		return 0;

	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
	if (0 == pDynObj)
		return 0;

	pObj->SetLDirAndUp(m_pCamCtrlAgent->GetActiveCamera()->GetCamera().GetDirH(), g_vAxisY);
	CDynProperty* pProp = pDynObj->GetProperties();
	pProp->SetRoleType(CDynProperty::ROLE_ACTOR_AVATAR);
	pProp->SetAPType(IActionPlayerBase::AP_NULL);
	pProp->GetCCMgr().SwtichCC(CAPhysXCCMgr::CC_BRUSH_CDRCC);
//	pProp->GetCCMgr().SwtichCC(CAPhysXCCMgr::CC_APHYSX_DYNCC);
//	pProp->GetCCMgr().SwtichCC(CAPhysXCCMgr::CC_APHYSX_EXTCC);
	pProp->SetScriptNames("CommonActorBehavior.cs");
	pDynObj->EnterRuntime(*m_pPhysXScene);

	ApxActorBase* pActorBase = new ApxActorBase();
	if (0 == pActorBase)
	{
		ReleaseObject(pObj);
		return 0;
	}
	NxVec3 ext = pProp->GetCCMgr().GetDefaultVolume();
	ext.y *= 0.5f;
	pActorBase->Init(pDynObj, ext);

	CMainActor* pMainActor = new CMainActor(*pActorBase);
	if (0 == pMainActor)
	{
		delete pActorBase;
		ReleaseObject(pObj);
		return 0;
	}
	m_MainActors.Add(pMainActor);
	if (0 == m_pCurrentMA)
	{
		m_pCurrentMA = pMainActor;
		m_CMAIdx = m_MainActors.GetSize() - 1;

		if (0 != m_pBkGrndTask)
		{
			A3DVECTOR3 pos = m_pCurrentMA->GetPos();
			float fR = m_fDynLoadRadius * 0.5f;
			pos.x += a_Random(-fR, fR);
			pos.z += a_Random(-fR, fR);
			m_pBkGrndTask->AddTask(pos);
		}

	}
	return pMainActor;
}

void Scene::ReleaseMainActor(CMainActor* pMA)
{
	if (0 == pMA)
		return;

	int idx = -1;
	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (pMA == m_MainActors[i])
			idx = i;
	}

	assert(0 <= idx);
	if (0 <= idx)
	{
		// note: the associate IPhysXObjBase obj wasn't released until the end of runtime
		ApxActorBase* pActorBase = m_MainActors[idx]->GetActorBase();
		if (0 != pActorBase)
			pActorBase->Release();
		delete pActorBase;
		delete m_MainActors[idx];
		m_MainActors.RemoveAtQuickly(idx);
	}

	if (m_pCurrentMA == pMA)
	{
		int nAll = m_MainActors.GetSize();
		if (0 < nAll)
		{
			m_pCurrentMA = m_MainActors[0];
			m_CMAIdx	 = 0;
		}
		else
		{
			m_pCurrentMA = 0;
			m_CMAIdx	 = -1;
		}
	}
}

void Scene::JumpMainActor() const
{
	if (0 != m_pCurrentMA)
	{
		m_pCurrentMA->HangEnd();
		m_pCurrentMA->Jump(6.0f);
	}
}

int Scene::SwitchMainActor()
{
	const int nCount = m_MainActors.GetSize(); 
	if (1 > nCount)
		return m_CMAIdx;

	++m_CMAIdx;
	if (nCount <= m_CMAIdx)
		m_CMAIdx = 0;

	CMainActor* pNewMA = m_MainActors[m_CMAIdx];
	if (0 != pNewMA)
	{
		m_pCurrentMA = pNewMA;

		CamOptTask_EntryBindingMode ot(m_pCurrentMA->GetDynObj());
		ot.m_bStorePose = false;
		ot.m_bIsKeepDir = true;
		m_pCamCtrlAgent->ExecuteCameraOperation(ot);
	}
	return m_CMAIdx;
}

void Scene::SwitchMainActorWalkRun() const
{
	if (0 == m_pCurrentMA)
		return;

	bool isRuning = m_pCurrentMA->IsRunEnable();
	isRuning = !isRuning; 
	m_pCurrentMA->EnableRun(isRuning);
}

const TCHAR* Scene::GetCurrentMainActorCCType() const
{
	if (0 != m_pCurrentMA)
		return m_pCurrentMA->GetCCTypeText();
	return 0;
}

void Scene::SwitchMainActorCCType(CAPhysXCCMgr::CCType ct)
{
	if (0 != m_pCurrentMA)
	{
		m_pCurrentMA->SwitchCCType(ct);

		CAPhysXCCMgr& ccm  = m_pCurrentMA->GetDynObj()->GetProperties()->GetCCMgr();
		APhysXCharacterController* pExtCC = ccm.GetAPhysXExtCC();
		if (0 != pExtCC)
			pExtCC->SetPushFactor(m_MAPushForce);
	}
}

void Scene::ChangeMainActorPushForce(float deltaValue)
{
	m_MAPushForce += deltaValue;
	ApplyPushForce();
}

void Scene::ApplyPushForce()
{
	const int nSize = m_MainActors.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		CAPhysXCCMgr& ccm  = m_MainActors[i]->GetDynObj()->GetProperties()->GetCCMgr();
		APhysXCharacterController* pExtCC = ccm.GetAPhysXExtCC();
		if (0 != pExtCC)
			pExtCC->SetPushFactor(m_MAPushForce);
	}
}

void Scene::TrytoSwing()
{
	if (0 == m_pCurrentMA)
		return;

	ApxActorBase* pAAB = m_pCurrentMA->GetActorBase();
	ApxActorGroundMoveController* pAAGMC = pAAB->GetGroundMoveController();
	NxVec3 ccDir = pAAB->GetHeadDir();
	NxVec3 vNewDir = APhysXUtilLib::Slerp(ccDir, NxVec3(0, 1, 0), 0.6666667f);
	pAAGMC->StartSwing(vNewDir);
	if (pAAGMC->IsInSwing())
		pAAB->ResetHitTarget();
}

void Scene::ControlModelClothes()
{
	if (m_status.ReadFlag(GAME_IS_MODELCLOTHES_ENABLE))
	{
		m_status.ClearFlag(GAME_IS_MODELCLOTHES_ENABLE);
		ObjManager::GetInstance()->ControlModelClothes(false);
	}
	else
	{
		m_status.RaiseFlag(GAME_IS_MODELCLOTHES_ENABLE);
		ObjManager::GetInstance()->ControlModelClothes(true);
	}
}

bool Scene::OnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	if (!IsRuntime())
		return false;

	if (MK_LBUTTON & nFlags)
	{
		bool HasCtrl = (0 == (MK_CONTROL & nFlags))? false : true;
		bool HasShift = (0 == (MK_SHIFT & nFlags))? false : true;
		if (HasCtrl)
			return false;
		if (HasShift)
		{
			if (IsRuntime())
			{
				m_generalAttMgr.DirectRayAttack(x, y);
				return true;
			}
			return false;
		}

		if (m_PhysXPicker.OnMouseButtonDown(x, y))
		{
			m_status.RaiseFlag(GAME_IS_PICK_PHYSX_STH);
			return true;
		}
		else
		{
			m_status.ClearFlag(GAME_IS_PICK_PHYSX_STH);
		}
	}
	return false;
}

void Scene::OnLButtonUp(const int x, const int y, const unsigned int nFlags)
{
	if (!IsRuntime())
		return;

	m_status.ClearFlag(GAME_IS_PICK_PHYSX_STH);
	m_PhysXPicker.OnMouseButtonUp(x, y);
}

void Scene::OnMouseMove(const int x, const int y, const unsigned int nFlags)
{
	if (IsRuntime())
	{
		if (m_status.ReadFlag(GAME_IS_PICK_PHYSX_STH))
			m_PhysXPicker.OnMouseMove(x, y);
	}
}

bool Scene::OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags)
{
	if (!IsRuntime())
		return false;

	CamOptTask_UpdateFocusFactor ot(zDelta);
	m_pCamCtrlAgent->ExecuteCameraOperation(ot);
	return true;
}

bool Scene::RayTraceObject(const POINT& pt, const PhysRay& ray, PhysRayTraceHit& outHitInfo, bool onlyPhysX, APhysXRaycastFilterType apxRFType, APhysXRayHitInfo* poutHitInfo)
{
	bool rtn = false;
	APhysXRayHitInfo nxHitInfo;
	NxRay nxRay;
	nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
	nxRay.dir  = APhysXConverter::A2N_Vector3(ray.vDelta);
	nxRay.dir.normalize();
	float dist = ray.vDelta.Magnitude();
	if (APhysXPicker::Raycast(m_pPhysXScene, nxRay, nxHitInfo, apxRFType, dist))
	{
		if (0 != nxHitInfo.mActor)
		{
			IPhysXObjBase* pHitObj = ObjManager::GetInstance()->GetPhysXObject(*nxHitInfo.mActor);
			if (0 != pHitObj)
			{
				rtn = true;
				outHitInfo.vHit.Set(nxHitInfo.mHitPos.x, nxHitInfo.mHitPos.y, nxHitInfo.mHitPos.z);
				outHitInfo.vNormal.Set(nxHitInfo.mHitNormal.x, nxHitInfo.mHitNormal.y, nxHitInfo.mHitNormal.z);
				outHitInfo.t = nxHitInfo.mDist / dist;
				outHitInfo.UserData = (void*)pHitObj;
			}
		}
	}
	if (0 != poutHitInfo)
		*poutHitInfo = nxHitInfo;

	if (!onlyPhysX)
	{
		if (!rtn)
			rtn = ObjManager::GetInstance()->RayTraceObj(ray, outHitInfo, false);
	}

	if (!IsRuntime())
	{
		IObjBase* pNewHit = static_cast<IObjBase*>(outHitInfo.UserData);
		m_RHOwner.SetRayHit(pNewHit, &pt);
	}
	return rtn;
}

void Scene::EnableActorIK(bool bEnable)
{
	if(m_pCurrentMA)
		m_pCurrentMA->GetActorBase()->EnableIK(bEnable);
}

bool Scene::IsActorIKEnabled() const
{
	if(m_pCurrentMA)
	{
		return m_pCurrentMA->GetActorBase()->IsIKEnabled();
	}
	else
		return false;
}

bool Scene::UpdatePropForBuildScene(IObjBase* pObj)
{
	CPhysXObjDynamic* pDynObj = dynamic_cast<CPhysXObjDynamic*>(pObj);
	if (0 == pDynObj)
		return false;

	CDynProperty* pProp = pDynObj->GetProperties();
	pProp->GetFlags().ClearFlag(OBF_RUNTIME_ONLY);


	CECModel* pECM = pDynObj->GetECModel();
	if (0 != pECM)
		pECM->EnablePhysSystem(false);

	APhysXSkeletonRBObject* pSRB = pDynObj->GetSkeletonRBObject();
	if (0 != pSRB)
	{
		if (0 == pSRB->IsBreakable())
		{
			pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);
			pProp->SetScriptNames("MonsterStand.cs");
			pProp->SetRoleType(CDynProperty::ROLE_ACTOR_NPC);
			pProp->SetAPType(IActionPlayerBase::AP_NULL);
			pProp->GetCCMgr().SwtichCC(CAPhysXCCMgr::CC_TOTAL_EMPTY);
		}
	}
	pECM->DisablePhysSystem();
	return true;
}

void Scene::ToBreakAndRagdoll()
{
	A3DVECTOR3 camDir, camUp;
	m_pCamCtrlAgent->GetActiveCamera()->GetGDirAndUp(camDir, camUp);
	A3DVECTOR3 dir = camDir;
	float x = DotProduct(camDir, A3DVECTOR3(0, 1, 0));
	if (0.9 < x)
		dir = -camUp;
	if (-0.9 > x)
		dir = camUp;
	dir.y = 0;
	dir.Normalize();
	A3DVECTOR3 force = dir * 1200; 
	A3DVECTOR3 forceUp(0, 50, 0);

	m_BreakReport.DisableRecover();

	NxVec3 posHit, posModel;
	NxBounds3 bnd;
	CPhysXObjDynamic* pDynObj = 0;
	CDynProperty* pProp = 0;

	APtrArray<IObjBase*> outObjs;
	int nObjs = ObjManager::GetInstance()->GetRootLiveObjects(outObjs, ObjManager::OBJ_TYPEID_ECMODEL);
	for (int i = 0; i < nObjs; ++i)
	{
		pDynObj = dynamic_cast<CPhysXObjDynamic*>(outObjs[i]);
		APhysXSkeletonRBObject* pSRB = pDynObj->GetSkeletonRBObject();
		if (0 == pSRB)
			continue;

		if (0 != pSRB->IsBreakable())
		{
			posModel = pDynObj->GetGPos();
			posModel.y += 0.8f;
			if (pDynObj->GetObjAABB(bnd))
				bnd.getCenter(posModel);
			posHit = posModel;
			posHit.x -= dir.x; 
			posHit.z -= dir.z; 
			pDynObj->AddForce(posHit, APhysXConverter::A2N_Vector3(force), CECModel::PFT_FORCE, 0.1f);
		}
		else
		{
			if (0 == pDynObj->GetActorBase())
			{
				pProp = pDynObj->GetProperties();
				pProp->GetCCMgr().SwtichCC(CAPhysXCCMgr::CC_TOTAL_EMPTY);
				bool bIsLock = pProp->GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED);
				if (bIsLock)
					pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);
				pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
				if (bIsLock)
					pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);

				posHit = pDynObj->GetGPos();
				posHit.y += 0.1f;
				pDynObj->AddForce(posHit, APhysXConverter::A2N_Vector3(forceUp), CECModel::PFT_FORCE, 0.1f);
			}
		}
	}
}

bool Scene::QueryRPTStateEnable(const NxVec3& pos, bool bIsLoadPhysX) const
{
	if (!gEnableAllPhysX)
		return false;

	if (RPT_NO_RAGDOLL == m_RPTMode)
		return false;

	bool bDoRangTest = false;
	if (bIsLoadPhysX)
	{
		if (RPT_DYN_LOAD == m_RPTMode)
			bDoRangTest = true;
	}
	else
	{
		if (RPT_DYN_SIM == m_RPTMode)
			bDoRangTest = true;
	}

	if (bDoRangTest)
	{
		if (0 != m_pCurrentMA)
		{
			NxVec3 d = pos - m_pCurrentMA->GetActorBase()->GetPos();
			if (d.magnitudeSquared() > m_fDynLoadRadius * m_fDynLoadRadius)
				return false;
		}
	}
	return true;
}

const TCHAR* Scene::GetRPTModeText() const
{
	switch (m_RPTMode)
	{
	case RPT_DISABLE_TEST:
		return _T("Disable RPT Test");
	case RPT_NO_RAGDOLL:
		return _T("No Ragdoll");
	case RPT_ALL_LOAD:
		return _T("All Ragdolls Loaded & Updated Pose");
	case RPT_DYN_LOAD:
		return _T("Dynamic Loading");
	case RPT_DYN_SIM:
		return _T("Dynamic Updating Pose");
	}

	assert(!"Unknown RPT Mode!");
	return _T("");
}

void Scene::SetRPTMode(RagdollPerfTest mode)
{
	m_RPTMode = mode;
	m_RPTOutputTickNum = 3;
}

void Scene::EnumNextRPTMode()
{
	if (RPT_DISABLE_TEST == m_RPTMode)
		return;

	if (RPT_NO_RAGDOLL == m_RPTMode)
		SetRPTMode(RPT_ALL_LOAD);
	else if (RPT_ALL_LOAD == m_RPTMode)
		SetRPTMode(RPT_DYN_LOAD);
	else if (RPT_DYN_LOAD == m_RPTMode)
		SetRPTMode(RPT_DYN_SIM);
	else if (RPT_DYN_SIM == m_RPTMode)
		SetRPTMode(RPT_NO_RAGDOLL);
}

bool Scene::QueryRPTMode(RagdollPerfTest mode) const
{
	if (mode == m_RPTMode)
		return true;
	return false;
}

void Scene::SwitchAllPhysXState()
{
	if (gEnableAllPhysX)
	{
		gEnableAllPhysX = false;
		ObjManager::GetInstance()->DisableAllPhysX();
	//	gToEndSim = true;
	}
	else
	{
		gEnableAllPhysX = true;
		ObjManager::GetInstance()->EnableAllPhysX(*m_pPhysXScene);
	//	gToStartSim = true;
	}
}
