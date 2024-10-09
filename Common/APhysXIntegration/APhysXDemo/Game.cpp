//	Game.cpp

#include "Global.h"
#include "Game.h"
#include "Render.h"
#include "PhysTerrain.h"
#include "PhysObjectMan.h"
#include "PhysObject.h"

#include <A3DSkySphere.h>
#include <A3DViewport.h>
#include <A3DCamera.h>
#include <A3DSkinMan.h>
#include <A3DEngine.h>
#include <A3DFuncs.h>
#include <A3DSkinRender.h>
#include <A3DDevice.h>
#include <A3DCursor.h>
#include <A3DLitModelRender.h>

#include "A3DGFXExMan.h"


static bool s_bPhysXClothTurnOn = true;

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define TIME_TICKANIMATION	30		//	ms

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

// to do...
// implementation of some external function...
APhysXScene* gGetAPhysXScene()
{
	return g_Game.GetPhysXScene();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

CGame	g_Game;

//const char szDefaultTerrainPath[] = "E:\\Developement\\Source_ Depot\\APhysXIntegration\\bin\\Maps\\1\\1Render.trn2";

const char szDefaultTerrainPath[] = "Maps\\1\\1Render.trn2";

// static APhysXRegistryHack gPhysXHacker;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static WORD _RadianToChar(float rad)
{
	int a = (int)((rad / A3D_2PI) * 65536.0f + 0.5f);
	return (WORD)a;
}

WORD glb_CompressDirH2(float x, float z)
{
	static const float fInvInter = 65536.0f / 360.0f;

	if (fabs(x) < 0.00001)
	{
		if (z > 0.0f)
			return 16384;
		else
			return 49152;
	}
	else
	{
		float fDeg = RAD2DEG((float)atan2(z, x));
		return (WORD)(fDeg * fInvInter);
	}
}

A3DVECTOR3 glb_DecompressDirH2(WORD wDir)
{
	static const float fInter = 360.0f / 65536.0f;

	float fRad = DEG2RAD(wDir * fInter);
	A3DVECTOR3 v;
	v.x = (float)cos(fRad);
	v.z = (float)sin(fRad);
	v.y = 0.0f;

	return v;
}

void glb_CompressDir2(const A3DVECTOR3& v, WORD& dir0, WORD& dir1)
{
	static const float fInvInter = 65536.0f / 360.0f;

	A3DVECTOR3 vh(v.x, 0.0f, v.z);
	vh.Normalize();
	dir0 = glb_CompressDirH2(vh.x, vh.z);
	
	float fDeg = RAD2DEG((float)acos(v.y));
	dir1 = (WORD)(fDeg * fInvInter);
}

A3DVECTOR3 glb_DecompressDir2(WORD dir0, WORD dir1)
{
	static const float fInter0 = 360.0f / 65536.0f;
	static const float fInter1 = 360.0f / 65536.0f;

	float fRad = DEG2RAD(dir0 * fInter0);
	float fHei = DEG2RAD(dir1 * fInter1);

	A3DVECTOR3 v;
	float p = (float)sin(fHei);
	v.x = p * (float)cos(fRad);
	v.z = p * (float)sin(fRad);
	v.y = (float)cos(fHei);

	return v;
}

WORD _CompressDirH2(float x, float z)
{
	static const float fInvInter = 65536.0f / 360.0f;

	if (fabs(x) < 0.00001)
	{
		if (z > 0.0f)
			return 16384;
		else
			return 49152;
	}
	else
	{
		float fDeg = RAD2DEG((float)atan2(z, x));
		return (WORD)(fDeg * fInvInter);
	}
}

void _CompressDir2(const A3DVECTOR3& v, WORD& dir0, WORD& dir1)
{
	static const float fInvInter = 65536.0f / 360.0f;

	A3DVECTOR3 vh(v.x, 0.0f, v.z);
	vh.Normalize();
	dir0 = _CompressDirH2(vh.x, vh.z);
	
	float fDeg = RAD2DEG((float)acos(v.y));
	dir1 = (WORD)(fDeg * fInvInter + 0.5f);
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement GAMESETTING
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CGame
//	
///////////////////////////////////////////////////////////////////////////

CGame::CGame()
{
	m_pTerrain		= NULL;
	m_pObjectMan	= NULL;
	m_pA3DSky		= NULL;
	m_pCursor		= NULL;
	m_pPhysXScene	= NULL;
	m_pPickedObject	= NULL;

	m_pHitObject = NULL;

	m_dwTickTime	= 0;
	m_bWireMode		= false;
	// m_iPhysState	= CPhysObject::PHY_STATE_PURESIMULATE;
	m_iPhysState	= CPhysObject::PHY_STATE_ANIMATION;

	m_iGameMode = GAME_MODE_EDIT;

	m_bEnablePhysXProfile = false;
	m_bEnablePhysXDebugRender = false;

	m_bPickNewObject = true;

}

CGame::~CGame()
{
}

//	Initialize game
bool CGame::Init()
{
	//	Initialize physical engine

 	if (!InitPhysX())
 	{
 		a_LogOutput(1, "CGame::Init, failed to initialzie physical engine !");
 		return false;
 	}

	// turn off the render of physx joints and joint limits...
	gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 0);
	gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 0);

	//gPhysXEngine->SetNxPhysicsSDKParameter(NX_SKIN_WIDTH, 0.005f);
	

	//	Load vertex shaders which are needed by skin models
	A3DSkinMan* pA3DSkinMan = g_Render.GetA3DEngine()->GetA3DSkinMan();
	pA3DSkinMan->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");

	//	Load sky
	if (!LoadSky())
		return false;

	//	Initialize physical object manager
	m_pObjectMan = new CPhysObjectMan;

	if (!m_pObjectMan->Init())
		return false;

	//	Initialize cursor
	m_pCursor = new A3DSysCursor;
	if (!m_pCursor->Load(g_Render.GetA3DDevice(), "Cursors\\normal.ani"))
	{
		a_LogOutput(1, "CGame::Init, failed to cursor !");
		delete m_pCursor;
		m_pCursor = NULL;
	}
	else
	{
		g_Render.GetA3DDevice()->SetCursor(m_pCursor);
		g_Render.GetA3DDevice()->ShowCursor(true);
	}

	// init the GfxEx manager...
	AfxGetGFXExMan()->Init(g_Render.GetA3DDevice());

	// load the default terrain...
	// LoadTerrainAndPhysXTerrain(szDefaultTerrainPath);

/*	//	Test code..
	if (1)
	{
		A3DMATRIX4 mat = a3d_RotateY(DEG2RAD(10.0f));
		mat *= a3d_RotateX(DEG2RAD(10.0f));
		A3DVECTOR3 vAxis;
		float fAngle;
		WORD dir0, rad;
		WORD dir1;

		A3DQUATERNION quat(mat);
		quat.ConvertToAxisAngle(vAxis, fAngle);
		vAxis.Normalize();
	//	glb_CompressDir2(vAxis, dir0, dir1);
		_CompressDir2(vAxis, dir0, dir1);

		rad = _RadianToChar(fAngle);
		
		vAxis = glb_DecompressDir2(dir0, dir1);
		A3DQUATERNION quat2(vAxis, rad / 65536.0f * A3D_2PI);
		A3DMATRIX4 mat2;
		quat2.ConvertToMatrix(mat2);

		int bb = 0;
	}
*/
	return true;
}

//	Release game
void CGame::Release()
{
	A3DRELEASE(m_pPickedObject);

	if (m_pCursor)
	{
		g_Render.GetA3DDevice()->SetCursor(NULL);
		delete m_pCursor;
		m_pCursor = NULL;
	}

	A3DRELEASE(m_pObjectMan);

	A3DRELEASE(m_pA3DSky);
	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();

	for(int i=0; i<(int)m_arrGfxExes.size(); i++)
	{
		m_arrGfxExes[i]->Release();
		delete m_arrGfxExes[i];
	}

	AfxGetGFXExMan()->Release();

	//	Release physical engine
	ReleasePhysX();
}

//	Reset game
bool CGame::Reset()
{
	A3DRELEASE(m_pPickedObject);

	//	Reset object manager
	if (m_pObjectMan)
		m_pObjectMan->Reset();

	//	Release dummy actors
	if (m_pPhysXScene)
		// m_pPhysXScene->ReleaseDummyActors();
		m_pPhysXScene->Reset();


	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();

	return true;
}

//	Load terrain
bool CGame::LoadTerrain(const char* szFile)
{
	//	Load terrain
	CPhysTerrain* pNewTerrain = new CPhysTerrain;
	if (!pNewTerrain)
	{
		a_LogOutput(1, "CGame::LoadTerrain", "Not enough memory");
		return false;
	}

	if (!pNewTerrain->Init(g_Render.GetA3DEngine(), 100.0f, 100.0f))
	{
		a_LogOutput(1, "CGame::LoadTerrain", "Failed to initialize terrain");
		pNewTerrain->Release();
		return false;
	}

	if (!pNewTerrain->Load(szFile, 0.0f, 0.0f, false))
	{
		a_LogOutput(1, "CGame::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	pNewTerrain->SetLODType(A3DTerrain2::LOD_NOHEIGHT);
	pNewTerrain->SetActRadius(512.0f);
	pNewTerrain->SetViewRadius(512.0f);

	//	Release old terrain
	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();

	m_pTerrain = pNewTerrain;

	//	Reset camera's position
	A3DVECTOR3 vPos = g_vOrigin;
	vPos.y = m_pTerrain->GetPosHeight(vPos) + 80.0f;
	g_Render.GetCamera()->SetPos(vPos);

	//	Load terrain blocks around camera
	m_pTerrain->SetLoadCenter(vPos);

	//	Record terrain file
	m_strTrnFile = szFile;

	return true;
}

//	Load sky
bool CGame::LoadSky()
{
	//	Sphere type sky
	A3DSkySphere* pSky = new A3DSkySphere;
	if (!pSky)
		return false;

	//	Sky texture file name shouldn't be set here
	if (!pSky->Init(g_Render.GetA3DDevice(), NULL, "Cloudy\\Roof.bmp",
			"Cloudy\\01.bmp", "Cloudy\\02.bmp"))
	{
		a_LogOutput(1, "CGame::LoadSky, Failed to create sphere sky !");
		return false;
	}

	pSky->SetFlySpeedU(0.03f);
	pSky->SetFlySpeedV(0.03f);

	m_pA3DSky = pSky;
	
	//	This enable sky can do animation when A3DEngine::TickAnimation is called
	if (m_pA3DSky)
		g_Render.GetA3DEngine()->SetSky(m_pA3DSky);

	return true;
}

bool CGame::InitPhysX()
{
	// try to enable the local dll usage...
 
	// DoPhysXRegistryHack();
	//gPhysXHacker.HackPhysXPath(af_GetBaseDir());

	//	Initialize physical engine
	APhysXEngineDesc apxEngineDesc;
	apxEngineDesc.mPhysXCorePath = af_GetBaseDir();

	apxEngineDesc.mA3DEngine = g_Render.GetA3DEngine();
	// apxEngineDesc.mEnableVRD = true;
	bool bPhysXInitRes = gPhysXEngine->Init(apxEngineDesc);

	//gPhysXHacker.CleanupHack();

	if (!bPhysXInitRes)
		return false;

	

	//	Create physical scene
	APhysXSceneDesc apxSceneDesc;
	apxSceneDesc.mEnableDebugRender = true;
	apxSceneDesc.mNxSceneDesc.groundPlane = true;
	apxSceneDesc.mNxSceneDesc.gravity = APHYSX_STD_GRAVITY;
	apxSceneDesc.mDefaultMaterial.mRestitution = 0.3f;
	apxSceneDesc.mDefaultMaterial.mStaticFriction = 0.5f;
	apxSceneDesc.mDefaultMaterial.mDynamicFriction = 0.5f;

	m_pPhysXScene = gPhysXEngine->CreatePhysXScene(apxSceneDesc);

	m_pPhysXScene->EnablePerfStatsRender(m_bEnablePhysXProfile);
	m_pPhysXScene->EnableDebugRender(m_bEnablePhysXDebugRender);

	// do not call this...
	// m_pPhysXScene->StartSimulate();

	m_PhysXPicker.Init(m_pPhysXScene, g_Render.GetViewport());

	return true;
}

void CGame::ReleasePhysX()
{
	if (m_pPhysXScene)
	{
		m_pPhysXScene->EndSimulate();
		gPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
		m_pPhysXScene = NULL;
	}

	gPhysXEngine->Release();

	// CleanUpPhysXRegistryHack();
}

//	Load physX terrain data
bool CGame::LoadPhysXTerrain(const char* szFile)
{
	if (!m_pPhysXScene)
		return false;

	//	TODO: Release current PhysX terrain data
	m_pPhysXScene->ReleaseTerrain();

	m_pPhysXScene->InitTerrain(szFile);

	return true;
}

const float c_fMoveSpeed = 3.0f;

//	Tick routine
bool CGame::Tick(DWORD dwDeltaTime)
{
	if (!IsTerrainInit())
		return true;

	m_dwTickTime = dwDeltaTime;

	if(::GetAsyncKeyState('8') & 0x8000)
	{
		TurnOnPhysX();
	}

	//--------------------------------------------------------------------------
	// moving control...
	if(::GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pObjectMan->Move(A3DVECTOR3(c_fMoveSpeed * dwDeltaTime * 0.001f, 0, 0));
	}
	if(::GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pObjectMan->Move(A3DVECTOR3(-c_fMoveSpeed * dwDeltaTime * 0.001f, 0, 0));
	}
	
	if(::GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pObjectMan->Move(A3DVECTOR3(0, 0, c_fMoveSpeed * dwDeltaTime * 0.001f));
	}
	if(::GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pObjectMan->Move(A3DVECTOR3(0, 0, -c_fMoveSpeed * dwDeltaTime * 0.001f));
	}

	//--------------------------------------------------------------------------




	if(s_bPhysXClothTurnOn)
	{
		DWORD dwElapsedTime = dwDeltaTime;
	}

	//	Tick terrain
	A3DVECTOR3 vCamPos = g_Render.GetCamera()->GetPos();
	m_pTerrain->Update(dwDeltaTime, g_Render.GetCamera(), vCamPos);

	//	Tick physical objects
	m_pObjectMan->Tick(dwDeltaTime);

	if (m_pPickedObject)
		m_pPickedObject->Tick(dwDeltaTime);

	//	Tick physical system
	float fTime = dwDeltaTime * 0.001f;
	
	// gPhysXEngine->Simulate(fTime);
	gPhysXEngine->Simulate(APHYSX_STD_TICKTIME);

	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
	{
		APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
		if (pPhysXTerrain)
			pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(vCamPos));

		//	Sync physical data to graphics data
		m_pObjectMan->Sync();

		if (m_pPickedObject)
			m_pPickedObject->Sync();


	}


	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	if ((dwAnimTime += dwDeltaTime) >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_Render.GetA3DEngine()->TickAnimation();

		//	Update ear position so that all 3D sounds' positions are correct
		g_Render.GetCamera()->UpdateEar();
	}


	for(int i=0; i<(int)m_arrGfxExes.size(); i++)
	{
		m_arrGfxExes[i]->TickAnimation(dwDeltaTime);
	}


	AfxGetGFXExMan()->Tick(dwDeltaTime);

	return true;
}

//	Render routine
bool CGame::Render()
{
	//	Clear viewport
	g_Render.ClearViewport();

	if (!IsTerrainInit())
		return true;

	A3DEngine* pA3DEngine = g_Render.GetA3DEngine();
	A3DDevice* pA3DDevice = pA3DEngine->GetA3DDevice();
	A3DViewport* pA3DViewport = g_Render.GetViewport();
	A3DCamera* pA3DCamera = g_Render.GetCamera();

	//	Render sky first
	m_pA3DSky->SetCamera(pA3DCamera);
	m_pA3DSky->Render();

	//	Render terrain
	if (m_bWireMode)
		pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);

	m_pTerrain->Render(pA3DViewport);
	pA3DDevice->SetFillMode(A3DFILL_SOLID);

	//	Render objects
	m_pObjectMan->Render(pA3DViewport);

	if (m_pPickedObject)
		m_pPickedObject->RenderProxy(pA3DViewport);

	if(m_pHitObject)
		m_pHitObject->RenderProxy(pA3DViewport);

	//	Flush skin models
	pA3DEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(pA3DViewport);

	//	Flush GFXs
	pA3DEngine->RenderGFX(pA3DViewport, 0xffffffff);

	//	Flush litmodels
	pA3DEngine->GetA3DLitModelRender()->Render(pA3DViewport, false);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(false);
	pA3DEngine->GetA3DLitModelRender()->RenderAlpha(pA3DViewport);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(true);

	if (m_pPhysXScene && m_pPhysXScene->OnSimulate())
		m_pPhysXScene->DebugRender();
	
	// render GfxEx...
	for(int i=0; i<(int)m_arrGfxExes.size(); i++)
	{
		AfxGetGFXExMan()->RegisterGfx(m_arrGfxExes[i]);
	}

	AfxGetGFXExMan()->RenderAllGfx(pA3DViewport);

	return true;
}

//	Show or hide cursor
void CGame::ShowCursor(bool bShow)
{
	g_Render.GetA3DDevice()->ShowCursor(bShow);
}

//	Set picked object
void CGame::SetPickedObject(CPhysObject* pObject, bool bNewObject)
{
	
	//	Release old object
	if (m_pPickedObject && m_bPickNewObject)
	{
		m_pPickedObject->Release();
		delete m_pPickedObject;
	}

	m_pPickedObject = pObject;
	m_bPickNewObject = bNewObject;
}

//	Set up picked object
bool CGame::SetupPickedObject(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (!m_pPickedObject)
		return false;

	m_pPickedObject->Setup(vPos, vDir, vUp);
	
	if(m_bPickNewObject)
		m_pObjectMan->AddPhysObject(m_pPickedObject);


	m_pPickedObject = NULL;

	return true;
}

void CGame::SetPhysState(int iState)
{
	a_Clamp(iState, 0, CPhysObject::NUM_PHY_STATE-1);
	m_iPhysState = iState;
	m_pObjectMan->ChangePhysicalState(iState);
}

void CGame::SetGameMode(int iGameMode)
{ 
	if(iGameMode == m_iGameMode)
		return;

	switch(iGameMode)
	{
	
	case GAME_MODE_EDIT:
		{

			m_pObjectMan->ReleasePhysX();
			m_pObjectMan->ResetPose();

			// reset physX scene...
			if(m_pPhysXScene)
			{
				// clear all physX objects...
				m_pPhysXScene->Reset(false);

				// end simulate...
				m_pPhysXScene->EndSimulate();

			}

		}
		
		
		break;

	
	case GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case GAME_MODE_PLAY:

		{
			ASSERT(m_pPhysXScene);

			m_pObjectMan->ReleasePhysX();
			m_pObjectMan->ResetPose();

			if(m_pPhysXScene)
			{

				m_pObjectMan->InstantializePhysX();
				m_pPhysXScene->StartSimulate();


			}
		}



	    break;
	
	
	default:
		return;
	    break;
	}
	


	m_iGameMode = iGameMode;

}


void CGame::EnablePhysXProfile(bool bEnable)
{
	if(bEnable == m_bEnablePhysXProfile)
		return;

	if(m_pPhysXScene)
	{
		m_pPhysXScene->EnablePerfStatsRender(bEnable);
	}

	m_bEnablePhysXProfile = bEnable;

}

void CGame::EnablePhysXDebugRender(bool bEnable)
{
	if(bEnable == m_bEnablePhysXDebugRender)
		return;

	if(m_pPhysXScene)
	{
		m_pPhysXScene->EnableDebugRender(bEnable);
	}

	m_bEnablePhysXDebugRender = bEnable;

}

void CGame::LoadTerrainAndPhysXTerrain(const char* szFile)
{

	AString strFile(szFile);

	if (!LoadTerrain(strFile))
	{
		AfxMessageBox("加载地形数据失败！");
		return;
	}

	//	Load PhysX terrain
	af_ChangeFileExt(strFile, "_ptb.cfg");
	if (af_IsFileExist(strFile))
	{
		// do not load terrain directly...
		if(!LoadPhysXTerrain(strFile))
		{
			AfxMessageBox("加载PhysX地形数据失败！");
			return;
		}

	}

	m_strTerrainFileName = szFile;

}


//-------------------------------------------------------------

// current version number
static DWORD s_dwDemoSceneVersionNum = 0xCC000001;

// version history:



//-------------------------------------------------------------
bool CGame::LoadPhysXDemoScene(const char* szFilePath)
{
	Reset();

	APhysXUserStream inStream(szFilePath, true);

	DWORD dwDemoSceneVersion = inStream.readDword();


	if(dwDemoSceneVersion >= 0xCC000001)
	{
		m_strTerrainFileName = APhysXSerializerBase::ReadString(&inStream);

		LoadTerrainAndPhysXTerrain(m_strTerrainFileName);

		LoadCameraPose(&inStream);

		m_pObjectMan->Load(&inStream);

	}

	if(dwDemoSceneVersion >= 0xCC000002)
	{
		// to add...
	}


	return true;


}

bool CGame::SavePhysXDemoScene(const char* szFilePath)
{

	APhysXUserStream outStream(szFilePath, false);

	// save version number...
	outStream.storeDword(s_dwDemoSceneVersionNum);

	// save the terrain file name...
	ASSERT(! m_strTerrainFileName.IsEmpty());
	APhysXSerializerBase::StoreString(&outStream, m_strTerrainFileName);


	// save camera pose
	SaveCameraPose(&outStream);
		

	// save the CPhysObjMan...
	m_pObjectMan->Save(&outStream);

	return true;

}

bool CGame::LoadCameraPose(NxStream* pNxStream)
{
	A3DVECTOR3 vPos, vDir, vUp;
	
	pNxStream->readBuffer(&vPos, sizeof(A3DVECTOR3));
	pNxStream->readBuffer(&vDir, sizeof(A3DVECTOR3));
	pNxStream->readBuffer(&vUp, sizeof(A3DVECTOR3));

	if(g_Render.GetCamera())
	{
		g_Render.GetCamera()->SetPos(vPos);
		g_Render.GetCamera()->SetDirAndUp(vDir, vUp);
	}


	return true;
	

}

bool CGame::SaveCameraPose(NxStream* pNxStream)
{
	
	pNxStream->storeBuffer(&g_Render.GetCamera()->GetPos(), sizeof(A3DVECTOR3));
	pNxStream->storeBuffer(&g_Render.GetCamera()->GetDir(), sizeof(A3DVECTOR3));
	pNxStream->storeBuffer(&g_Render.GetCamera()->GetUp(), sizeof(A3DVECTOR3));

	return true;
}

bool CGame::LoadGfxEx(const char* szFileName, const A3DVECTOR3& vPos)
{
	AString strFileName(szFileName);
	A3DGFXEx* pGfxEx = AfxGetGFXExMan()->LoadGfx(g_Render.GetA3DDevice(), strFileName);
	if(pGfxEx)
	{
		pGfxEx->SetPos(vPos);
		pGfxEx->Start();

		m_arrGfxExes.push_back(pGfxEx);

		return true;
	}
	else
		return false;

}

bool CGame::RayTracePhysObject(const PhysRay& ray)
{
	PhysRayTraceHit hitInfo;

	for(int i=0; i<m_pObjectMan->GetNum(); i++)
	{
		m_pObjectMan->GetPhysObject(i)->RayTrace(ray, hitInfo);
	}

	m_pHitObject = (CPhysObject*)hitInfo.UserData;

	return (m_pHitObject != NULL);


}


void CGame::TurnOnPhysX()
{

	for(int i=0; i<m_pObjectMan->GetNum(); i++)
	{
		m_pObjectMan->GetPhysObject(i)->TurnOnPhysX();
	}

	if(m_pPhysXScene)
	{
		// m_pPhysXScene->PauseSimulate(false);
	}

	s_bPhysXClothTurnOn = true;
	
}

void CGame::TurnOffPhysX()
{

	for(int i=0; i<m_pObjectMan->GetNum(); i++)
	{
		m_pObjectMan->GetPhysObject(i)->TurnOffPhysX();
	}

	if(m_pPhysXScene)
	{
		// m_pPhysXScene->PauseSimulate();
	}
	
	s_bPhysXClothTurnOn = false;
}

void CGame::RemovePickedObject()
{
	if(m_pPickedObject)
	{
		if(m_pHitObject == m_pPickedObject)
		{
			m_pHitObject = NULL;
		}

		m_pObjectMan->RemovePhysObject(m_pPickedObject);
		m_pPickedObject = NULL;

	}
}

void CGame::TestWindFF()
{

	//---------------------------------------------------------------------------
	// create fluid emitter...

#if 0

	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3((float)5.0f, (float)1.0f, (float)1.0f);
	boxDesc.UseDefaultMaterial();

	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	APhysXDynamicRBObjectDesc rbDesc;
	rbDesc.AddActorComponenet(apxActorDesc, apxMaterialDesc, Shapes);		
	
	// all parameters use the default values...
	APhysXFluidObjectDesc fluidObjDesc;
	fluidObjDesc.mFluidParameters.mRestDensity = 50;
	fluidObjDesc.mFluidParameters.mRestParticlesPerMeter = 10;
	fluidObjDesc.mFluidParameters.mMotionLimitMultiplier = 0.8f;
	fluidObjDesc.mFluidParameters.mExternalAcceleration.set(0, 5, 0);
	// fluidObjDesc.mFluidParameters.mStiffness = 1.0f;
	//fluidObjDesc.mFluidParameters.mViscosity = 60.0f;
	
	// this is the key reason why the fluid emitter blocks if it is created in
	// force fields.
	fluidObjDesc.mFluidParameters.mFlags &= ~NX_FF_HARDWARE;

	APhysXFluidEmitterAttacherDesc apxFEAttacherDesc;
	apxFEAttacherDesc.AddFluidComponent(&fluidObjDesc);
	APhysXFluidEmitterParameters feParas;
	// feParas.mRelPose.t.set(0.0f, 0, 1.1f);
	feParas.mType = NX_FE_CONSTANT_FLOW_RATE;
	feParas.mShape = NX_FE_ELLIPSE;
	// feParas.mRate = 15;
	feParas.mParticleLifetime = 10.0f;
	feParas.mFluidVelocityMagnitude = 5.0f;
	feParas.mRelPose.t.set(0.0f, 0, 1.1f);
	feParas.mRandomPos.set(0.05f);
	feParas.mRandomAngle = 0.5f;

	apxFEAttacherDesc.AddAttacherComponent(&rbDesc, feParas);

	APhysXObjectInstanceDesc apxObjInstanceDesc;

	apxObjInstanceDesc.mPhysXObjectDesc = &apxFEAttacherDesc;
	apxObjInstanceDesc.mGlobalPose.t.set(123, 43, -19);

	APhysXObject* pPhysXObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);
	APhysXFluidEmitterAttacher* pFEAttacher = (APhysXFluidEmitterAttacher*)pPhysXObj;

#endif

	//-----------------------------------------------------------------------------------------
	// create the forcefield...

#if 1

	APhysXWindForceFieldObjectDesc ffObjDesc;
	APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters apxWindFFPara;
	apxWindFFPara.mWindForce.set(0, 0, 1.f);
	
	ffObjDesc.SetWindFFParameters(apxWindFFPara);

#else

	APhysXVacuumForceFieldObjectDesc ffObjDesc;

#endif

	APhysXBoxShapeDesc ffboxDesc;
	ffboxDesc.mDimensions.set(20.0f);
	ffboxDesc.mLocalPose.t.set(0, 10.0f, 0);

	ffObjDesc.mIncludeShapes.CreateAndPushShapeDesc(&ffboxDesc);

	ffObjDesc.mClothForceScale = 0.05f;

	APhysXObjectInstanceDesc objInstanceDesc;
	objInstanceDesc.mPhysXObjectDesc = &ffObjDesc;
	objInstanceDesc.mGlobalPose.t.set(123, 42, -19);


	APhysXForceFieldObject* pFFObject = (APhysXForceFieldObject*)m_pPhysXScene->CreatePhysXObject(objInstanceDesc);

	


}

void CGame::TestCloth()
{
	//-----------------------------------------------------------------------
	// generate a planar patch cloth mesh
	NxClothMeshDesc desc;

	float w = 10.0f;			// width
	float h = 6.0f;			// height
	float d = 0.3f;		// cell size

	int numX = (int)(w / d) + 1;
	int numY = (int)(h / d) + 1;

	desc.numVertices				= (numX+1) * (numY+1);
	desc.numTriangles				= numX*numY*2;
	desc.pointStrideBytes			= sizeof(NxVec3);
	desc.triangleStrideBytes		= 3*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.points						= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.triangles					= (NxU32*)malloc(sizeof(NxU32)*desc.numTriangles*3);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;


	int i,j;
	NxVec3 *p = (NxVec3*)desc.points;
	for (i = 0; i <= numY; i++) {
		for (j = 0; j <= numX; j++) {
			p->set(d*j, 0.0f, d*i); 
			p++;
		}
	}


	NxU32 *id = (NxU32*)desc.triangles;
	for (i = 0; i < numY; i++) {
		for (j = 0; j < numX; j++) {
			NxU32 i0 = i * (numX+1) + j;
			NxU32 i1 = i0 + 1;
			NxU32 i2 = i0 + (numX+1);
			NxU32 i3 = i2 + 1;
			if ((j+i)%2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}


	// init APhysXClothObjectDesc object via the NxClothMeshDesc object
	APhysXClothObjectDesc clothDesc;
	static int s_iClothMaterial = 0;

	// clothDesc.mClothParameters.LoadPresetMaterial(APhysXClothParameters::APX_CLOTH_MATERIAL_SILK);
	// clothDesc.mClothParameters.LoadPresetMaterial(APhysXClothParameters::APX_CLOTH_MATERIAL_FLAX);
	// clothDesc.mClothParameters.LoadPresetMaterial(APhysXClothParameters::APX_CLOTH_MATERIAL_FELT);
	clothDesc.mClothParameters.LoadPresetMaterial(s_iClothMaterial);
	if(++s_iClothMaterial == 3)
	{	
		s_iClothMaterial = 0;
	}

	clothDesc.InitClothMesh(desc);
	
	
	//clothDesc.SetName("cloth");

	// release allocated memory in NxClothMeshDesc
	free((void *)desc.points);
	free((void *)desc.triangles);

	//-----------------------------------------------------------------------
	// init static rigid body object desc
	//*
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3((float)5.0f, (float)1.0f, (float)1.0f);
	boxDesc.UseDefaultMaterial();

	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	APhysXDynamicRBObjectDesc rbDesc;
	rbDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	//rbDesc.SetName("rb");

	//-----------------------------------------------------------------------
	
	
	// cloth attacher desc...

	APhysXClothAttacherDesc compoundObjDesc;
	compoundObjDesc.AddMultiClothComponent(&clothDesc, "cloth1");
	compoundObjDesc.AddAttacherComponent(&rbDesc);
	compoundObjDesc.CompleteInnerConnectorInfo();



	// create compound object
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = &compoundObjDesc;

	
	apxObjInstanceDesc.mGlobalPose.t.set(123, 43, -19);
	m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);




}

void CGame::PhysXCoreDump(const char* szFileName)
{

	if(szFileName)
		gPhysXEngine->CoreDumpPhysX(szFileName, APhysXEngine::APX_COREDUMP_FT_XML);

}