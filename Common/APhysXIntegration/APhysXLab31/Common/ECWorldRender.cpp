/*
 * FILE: ECWorldRender.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2010/3/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */
#include "stdafx.h"

#include "ECWorldRender.h"
//#include "Game.h"
//#include "Render.h"
//#include "NPC.h"
//#include "Building.h"
//#include "Settings.h"
//#include "Light.h"
//#include "ShadowAdjuster.h"

//#include "SpeedTree4_1\\ECTreeForest.h"
//#include "SpeedTree4_1\\ECTreeInst.h"
#include <A3DShadowPSSM.h>
#include <A3DShadowPSSM2.h>
#include <A3DShadowSSM.h>
#include <A3DShadowTSM.h>
#include <A3DShadowCube.h>
#include <A3DShadowTerrain.h>
#include <A3DSkinRender.h>
#include <A3DLPPRender.h>
#include <A3DLPPLight.h>
#include <A3DLPPShadowMask.h>
/*
#include <A3DViewport.h>
#include <A3DRenderTarget.h>
#include <A3DRenderTargetMan.h>
#include <A3DCubeRenderTarget.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DLitModelRender.h>
#include <A3DMacros.h>
#include <A3DSkinMan.h>
#include <A3DFXMaterialMan.h>
#include <A3DFXParam.h>
#include <A3DTerrain2.h>
#include <A3DTerrainWater.h>
#include <A3DCamera.h>
#include <A3DSkySphere.h>
#include <A3DLight.h>

#include <A3DUnlitModel.h>
#include <A3DConfig.h>
#include <A3DMtlWater.h>
#include <A3DSimpleQuad.h>
#include <A3DFuncs.h>
#include <ACounter.h>
#include <ALog.h>

#include <SpeedTree4_1\ECTree.h>
*/
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

	#define _OUTPUT_RTS_

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static void _CreateShadowMapCallback(A3DShadowMap* pShadowMap, A3DViewport* pViewport, int iSlice, a_uiptr user_data)
{
	ECWorldRender* pWorldRender = (ECWorldRender*)user_data;
	pWorldRender->CreateShadowMapCallback(pShadowMap, pViewport, iSlice);
}

static void _RenderReflect(A3DViewport* pViewport, a_uiptr* pArg)
{
	ECWorldRender* pWorldRender = (ECWorldRender*)pArg;
	pWorldRender->RenderReflect(pViewport);
}

static void _RenderRefract(A3DViewport* pViewport, a_uiptr* pArg)
{
	ECWorldRender* pWorldRender = (ECWorldRender*)pArg;
	pWorldRender->RenderRefract(pViewport);
}
/*
void BuildLitMeshVertLit(const BUILD_ST_VERTLIT& pBuildContent, A3DUnlitMesh* pLitMesh, DWORD** ppVertLit, 
						const float fLightScale, BOOL bFade, BOOL bLeaf)
{
	A3DCOLORVALUE color;
	int i, n, nLight = pBuildContent.aLightParam.GetSize();
	int nNumVert = pLitMesh->GetNumVerts();
	const A3DUNLMVERTEX* pVert = pLitMesh->GetVerts();
	const A3DAABB aabb = pLitMesh->GetAABB();

	*ppVertLit = new DWORD[nNumVert];
	const A3DVECTOR3 vLightDir = -pBuildContent.vMainLightPos;
	A3DVECTOR3 vMin(
		vLightDir.x > 0 ? aabb.Mins.x : aabb.Maxs.x,
		vLightDir.y > 0 ? aabb.Mins.y : aabb.Maxs.y,
		vLightDir.z > 0 ? aabb.Mins.z : aabb.Maxs.z);
	A3DVECTOR3 vMax(
		vLightDir.x < 0 ? aabb.Mins.x : aabb.Maxs.x,
		vLightDir.y < 0 ? aabb.Mins.y : aabb.Maxs.y,
		vLightDir.z < 0 ? aabb.Mins.z : aabb.Maxs.z);

	float fMin = DotProduct(vMin, vLightDir);
	float fMax = DotProduct(vMax, vLightDir);

	for(i = 0; i < nNumVert; i++)
	{
		const float fNdotL = DotProduct(pVert[i].normal, vLightDir);
		const float fDistFactor = bFade == TRUE ? (DotProduct(pVert[i].pos, vLightDir) - fMin) / (fMax - fMin) : 1.0f;
		A3DCOLORVALUE Minor = 0.0f;
		for(n = 0; n < nLight; n++)
		{
			A3DLIGHTPARAM& LightParam = pBuildContent.aLightParam[n];
			A3DVECTOR3 vDist(pVert[i].pos - LightParam.Position);
			float fDist = vDist.Magnitude();
			float fAttend = 1.0f / (LightParam.Attenuation0 + LightParam.Attenuation1 * fDist + LightParam.Attenuation2 * fDist * fDist);

			A3DVECTOR3 vLightToVert = LightParam.Position - pVert[i].pos;
			vLightToVert.Normalize();

			if(bLeaf == false)
			{
				float fDiffuse = DotProduct(vLightToVert, pVert[i].normal);
				if(fDiffuse < 0)
					fDiffuse = 0;
				fAttend *= fDiffuse;
			}

			Minor.r += LightParam.Diffuse.r * fAttend;
			Minor.g += LightParam.Diffuse.g * fAttend;
			Minor.b += LightParam.Diffuse.b * fAttend;
		}
		color.r = Minor.r;
		color.g = Minor.g;
		color.b = Minor.b;
		color.a = ((fNdotL + 1.0f) * 0.5f) * fLightScale * fDistFactor;
		color.Clamp();
		(*ppVertLit)[i] = color.ToRGBAColor();
	}
}

void BuildSpeedTreeVertLit(const BUILD_ST_VERTLIT& pBuildContent)
{
	char buffer[MAX_PATH];
	sprintf(buffer, "%s\\0x%08x.umd", pBuildContent.pTempWorkingDir, pBuildContent.pTreeInst);
	pBuildContent.pTreeInst->SaveToUnlitModel(buffer, -pBuildContent.vMainLightPos);

	A3DUnlitModel* pLitModel = new A3DUnlitModel;
	if(pLitModel->Load(pBuildContent.pA3DDevice, buffer) != false)
	{
		ASSERT(pLitModel->GetNumMeshes() == 3);
		DWORD* pVertLit;
		A3DUnlitMesh* pLitMesh;

		pLitMesh = pLitModel->GetMesh(0);
		BuildLitMeshVertLit(pBuildContent, pLitMesh, &pVertLit, 0.3f, FALSE, FALSE);
		pBuildContent.pTreeInst->UpdateBranchVertLit(pVertLit, pLitMesh->GetNumVerts());
		delete pVertLit;

		pLitMesh = pLitModel->GetMesh(1);
		BuildLitMeshVertLit(pBuildContent, pLitMesh, &pVertLit, 0.3f, TRUE, FALSE);
		pBuildContent.pTreeInst->UpdateFrondVertLit(pVertLit, pLitMesh->GetNumVerts());
		delete pVertLit;

		pLitMesh = pLitModel->GetMesh(2);
		BuildLitMeshVertLit(pBuildContent, pLitMesh, &pVertLit, 0.5f, TRUE, TRUE);
		if(pBuildContent.pTreeInst->GetCoreTree()->GetRenderData()->pLeafCardStream != NULL)
			pBuildContent.pTreeInst->UpdateLeafCardVertLit(pVertLit, pLitMesh->GetNumVerts());
		else
			pBuildContent.pTreeInst->UpdateLeafMeshVertLit(pVertLit, pLitMesh->GetNumVerts());
		delete pVertLit;
	}
	A3DRELEASE(pLitModel);
}
*/
///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement ECWorldRender
//	
///////////////////////////////////////////////////////////////////////////


ECWorldRender::ECWorldRender(CRender& render) :
m_Render(render),
m_pA3DEngine(NULL),
m_pA3DDevice(NULL),
m_pPSSM(NULL),
m_pPSSM2(NULL),
m_pSSM(NULL),
m_pTSM(NULL),
m_pCubeSM(NULL),
m_pTrnSM(NULL),
m_pLPPRender(NULL),
m_psrShadow(NULL),
m_plmrShadow(NULL),
m_psrLPP(NULL),
m_plmrLPP(NULL),
m_pMainShadow(NULL),
m_pShadowAdj(NULL),
m_aShadowCasters(0, 512)
{
}

ECWorldRender::~ECWorldRender()
{
}

void ECWorldRender::OnTerrainInit(A3DTerrain2& trn)
{
	if (0 != m_pTrnSM)
		m_pTrnSM->Release();
	else
	{
		m_pTrnSM = new A3DShadowTerrain;
		if (0 == m_pTrnSM)
		{
			a_LogOutput(1, "ECWorldRender::OnTerrainInit, failed to create A3DShadowTerrain.");
			return; 
		}
	}

	if (!m_pTrnSM->Init(&trn, 300.0f, 1024))
		a_LogOutput(1, "ECWorldRender::OnTerrainInit, failed to initialize A3DShadowTerrain.");
}

void ECWorldRender::OnTerrainRelease()
{
	if (0 == m_pTrnSM)
		return;
	A3DRELEASE(m_pTrnSM);
}

void ECWorldRender::AddLight(A3DLPPLight& light)
{
	if (0 != m_pLPPRender)
	{
		m_pLPPRender->GetLightMan()->AddLight(&light);
		m_aLPPLights.Add(&light);
	}
}

void ECWorldRender::RemoveLight(A3DLPPLight* pLight)
{
	if (0 == pLight)
		return;
	if (0 == m_pLPPRender)
		return;

	m_pLPPRender->GetLightMan()->RemoveLight(pLight);
	int MaxCount = m_aLPPLights.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		if(pLight != m_aLPPLights[i])
			continue;
		m_aLPPLights.RemoveAtQuickly(i);
		break;
	}
	return;
}

//	Initialize object
bool ECWorldRender::Init(A3DEngine* pA3DEngine)
{
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();

	//	Create PSSM builder
	m_pPSSM = new A3DShadowPSSM;
	if (!m_pPSSM || !m_pPSSM->Init(m_pA3DEngine, _CreateShadowMapCallback, 1024, true))
	{
		a_LogOutput(1, "ECWorldRender::Init, failed to initialize PSSM");
		return false;
	}
	m_pPSSM->SetMaxFarDist(300);
	m_pPSSM->SetSplitLambda(0.9f);
	m_pPSSM->SetCallbackData(a_uiptr(this));

	//	Create PSSM2 builder
	m_pPSSM2 = new A3DShadowPSSM2;
	if (!m_pPSSM2 || !m_pPSSM2->Init(m_pA3DEngine, _CreateShadowMapCallback, 1))
	{
		a_LogOutput(1, "ECWorldRender::Init, failed to initialize PSSM");
		return false;
	}
	m_pPSSM2->SetCallbackData(a_uiptr(this));

	//	Create SSM builder
#define MAX_CSL	4

	// const int aMapSize[MAX_CSL] = {0, 512, 1024, 2048};
	const int aMapSize[MAX_CSL] = {0, 512, 1024, 2048};
	const float aDepthBias[MAX_CSL] = {0.0f, 0.000001f, 0.000001f, 0.000001f};
	const float aDist[MAX_CSL] = {0.0f, 5.0f, 30.0f, 45.0f};
	const float aKernelRad[MAX_CSL] = {0.0f, 0.5f, 1.0f, 2.0f};
	const int aKernelNum[MAX_CSL] = {4, 4, 8, 8};

	// int iLevel = MAX_CSL -1;
	int iLevel = 3;

	m_pSSM = new A3DShadowSSM;
	if (!m_pSSM || !m_pSSM->Init(m_pA3DEngine, _CreateShadowMapCallback, aMapSize[iLevel]))
	{
		a_LogOutput(1, "ECWorldRender::Init, failed to initialize SSM");
		return false;
	}
	m_pSSM->SetMaxCasterDist(200.0f);
	m_pSSM->SetCallbackData(a_uiptr(this));

	m_pSSM->Set2DKernelRadius(aKernelRad[iLevel]);
	m_pSSM->SetDepthBias(aDepthBias[iLevel]);
	m_pSSM->SetSampleType(A3DShadowMap::SAMPLE_PCF4);
	m_pSSM->Set2DKernelNum(aKernelNum[iLevel]);


	//	Create TSM builder
	m_pTSM = new A3DShadowTSM;
	if (!m_pTSM || !m_pTSM->Init(m_pA3DEngine, _CreateShadowMapCallback, aMapSize[iLevel]))
	{
		a_LogOutput(1, "ECWorldRender::Init, failed to initialize TSM");
		return false;
	}
	m_pTSM->SetMaxCasterDist(200.0f);
	m_pTSM->SetCallbackData(a_uiptr(this));

	m_pTSM->Set2DKernelRadius(aKernelRad[iLevel]);
	m_pTSM->SetDepthBias(aDepthBias[iLevel]);
	m_pTSM->SetSampleType(A3DShadowMap::SAMPLE_PCF4);
	m_pTSM->Set2DKernelNum(aKernelNum[iLevel]);


	//	Create cube SM builder
	m_pCubeSM = new A3DShadowCube;
	if (!m_pCubeSM || !m_pCubeSM->Init(m_pA3DEngine, _CreateShadowMapCallback, 256, true))
	{
		a_LogOutput(1, "ECWorldRender::Init, failed to initialize Cube SM");
		return false;
	}
	m_pCubeSM->SetCallbackData(a_uiptr(this));

	//	Create LPP render
	m_pLPPRender = new A3DLPPRender;
	if (!m_pLPPRender || !m_pLPPRender->Init(m_pA3DEngine))
	{
		a_LogOutput(1, "ECWorldRender::Init, failed to initialize LPP render");
		return false;
	}
//	m_pLPPRender->EnableDynamicShadow(true);

	//	Create temporary skin render
	m_psrShadow = new A3DSkinRender;
	m_psrShadow->Init(m_pA3DEngine);
	m_psrLPP = new A3DSkinRender;
	m_psrLPP->Init(m_pA3DEngine);

	//	Create temporary litmodel render
	m_plmrShadow = new A3DLitModelRender;
	// m_plmrShadow->Init(m_pA3DEngine, A3DLitModelRender::INIT_MTL_SHADOW);
	m_plmrShadow->Init(m_pA3DEngine);
	m_plmrLPP = new A3DLitModelRender;
	// m_plmrLPP->Init(m_pA3DEngine, A3DLitModelRender::INIT_MTL_BASE | A3DLitModelRender::INIT_MTL_GBUF);
	m_plmrLPP->Init(m_pA3DEngine);

	//	Set default shadow type
	// SetMainShadow(A3DShadowMap::TYPE_PSSM2);
	SetMainShadow(A3DShadowMap::TYPE_TSM);

	//m_pShadowAdj = new CShadowAdjuster;
	//m_pShadowAdj->Init(m_pPSSM2);
	return true;
}

//	Release object
void ECWorldRender::Release()
{
//	A3DRELEASE(m_pShadowAdj);
	if (0 != m_pLPPRender)
	{
		A3DLPPLightMan* pLightMan = m_pLPPRender->GetLightMan();
		int MaxCount = m_aLPPLights.GetSize();
		for (int i = 0; i < MaxCount; ++i)
			pLightMan->RemoveLight(m_aLPPLights[i]);
		m_aLPPLights.RemoveAll(true);
	}

	A3DRELEASE(m_plmrLPP);
	A3DRELEASE(m_plmrShadow);
	A3DRELEASE(m_psrLPP);
	A3DRELEASE(m_psrShadow);
	A3DRELEASE(m_pLPPRender);
	A3DRELEASE(m_pTrnSM);
	A3DRELEASE(m_pCubeSM);
	A3DRELEASE(m_pTSM);
	A3DRELEASE(m_pSSM);
	A3DRELEASE(m_pPSSM2);
	A3DRELEASE(m_pPSSM);

	m_aShadowCasters.RemoveAll();
}

void ECWorldRender::SetMainShadow(int type)
{
	switch (type)
	{
	case A3DShadowMap::TYPE_PSSM:	m_pMainShadow = m_pPSSM;	break;
	case A3DShadowMap::TYPE_PSSM2:	m_pMainShadow = m_pPSSM2;	break;
	case A3DShadowMap::TYPE_SSM:	m_pMainShadow = m_pSSM;		break;
	case A3DShadowMap::TYPE_TSM:	m_pMainShadow = m_pTSM;		break;
	default:
		ASSERT(0);
		break;
	}
}

//	Render routine
bool ECWorldRender::Render(A3DViewport* pViewport, A3DLPPLight* pLppLight)
{
	if (m_pLPPRender->IsDynamicShadowEnabled())
	{
		//	Generate terrain shadow map
		const A3DLIGHTPARAM& lp = m_Render.GetDirectionalLight()->GetLightparam();
		//  if (0 != m_pTrnSM)
		if(0)			// do not create the terrain shadow map at runtime...
		{
			m_pTrnSM->CreateShadowMap(pViewport->GetCamera()->GetPos(), lp.Direction);
			m_pLPPRender->GetShadowMaskMan()->SetTerrainShadow(m_pTrnSM);
		}
		else
			m_pLPPRender->GetShadowMaskMan()->SetTerrainShadow(NULL);

		//	Generate main light shadow map
		if (0 != pLppLight)
		{
			// pLppLight->EnableShadow(true);
			pLppLight->SetShadowMap(GetMainShadow());
			if (pLppLight->IsShadowEnabled())
			{
				// force to rebuild all split shadowmap...
				if(m_pPSSM2) m_pPSSM2->SetRebuildFlag(true);

				BuildShadowMap(pLppLight);
				m_pLPPRender->GetShadowMaskMan()->SetShadowMapR(GetMainShadow());
			}
			else
			{
				m_pLPPRender->GetShadowMaskMan()->SetShadowMapR(NULL);
			}
		}
	}
	else
	{
		m_pLPPRender->GetShadowMaskMan()->SetTerrainShadow(NULL);
		if (0 != pLppLight)
			pLppLight->EnableShadow(false);
	}

	//	LPP render for all opaque objects
	if (!LPPRender(pViewport))
		return false;

	//	Render all other things by normal forward rendering
	if (!RenderOthers(pViewport))
		return false;

	return true;
}

//	Build shadow map for light
A3DShadowMap* ECWorldRender::BuildShadowMap(A3DLPPLight* pLight)
{
	//	Use our own skin render and litmodel render
	m_pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(m_psrShadow);
	m_pA3DEngine->SetA3DLitModelRender(m_plmrShadow);

	//	Register shadow casters
	m_aShadowCasters.RemoveAll(false);

	if (m_pLPPRender->IsDynamicShadowEnabled())
	{
		m_Render.RegisterShadowCasters(m_aShadowCasters);
	}
	
	//CLight* pDirLight = g_Game.GetLightMan()->GetGlobalDirLight();
	//CLight* pFlashlight = g_Game.GetLightMan()->GetFlashLight();
	//CLight* pCandleLight = g_Game.GetLightMan()->GetCandleLight();

	A3DShadowMap* pShadowMap = 0;
	if (A3DLIGHT_DIRECTIONAL == pLight->GetType())
		pShadowMap = BuildGlobalShadowMap(m_Render.GetCamera(), pLight);
	else
		pShadowMap = BuildLightShadowMap(pLight);

	//	Restore renders
	m_pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(NULL);
	m_pA3DEngine->SetA3DLitModelRender(NULL);

	return pShadowMap;
}

//	Build global dynamic shadow map
A3DShadowMap* ECWorldRender::BuildGlobalShadowMap(A3DCameraBase* pEyeCamera, A3DLPPLight* pLight)
{
	ASSERT(pLight->GetType() == A3DLIGHT_DIRECTIONAL);
	A3DLPPLightDir* pDirLight = (A3DLPPLightDir*)pLight;

	//	Get light direction
	const A3DVECTOR3& vLightDir = pDirLight->GetDir();

	A3DShadowMap* pShadowMap = NULL;

	if (m_pMainShadow->GetType() == A3DShadowMap::TYPE_PSSM)
	{
		//	Create shadow map
		m_pPSSM->CreateShadowMap(pEyeCamera, vLightDir, m_aShadowCasters.GetData(), m_aShadowCasters.GetSize(), NULL);
		pShadowMap = m_pPSSM;

	#ifdef _OUTPUT_RTS_
		if (GetAsyncKeyState(VK_F6) & 0x8000)
			D3DXSaveTextureToFileA("PSSM.tga", D3DXIFF_TGA, m_pPSSM->GetShadowMap()->GetTargetTexture(), NULL);
	#endif	
	}
	if (m_pMainShadow->GetType() == A3DShadowMap::TYPE_PSSM2)
	{
		//	Create shadow map
	//	m_pPSSM2->SetRebuildFlag(true);
		m_pPSSM2->CreateShadowMap(pEyeCamera, vLightDir, m_aShadowCasters.GetData(), m_aShadowCasters.GetSize());
		pShadowMap = m_pPSSM2;

	#ifdef _OUTPUT_RTS_
		if (!m_pPSSM2->UseHWShadowPCF() && (GetAsyncKeyState(VK_F6) & 0x8000))
		{
			D3DXSaveTextureToFileA("PSSM0.tga", D3DXIFF_TGA, m_pPSSM2->GetShadowMapTexture(0), NULL);
			D3DXSaveTextureToFileA("PSSM1.tga", D3DXIFF_TGA, m_pPSSM2->GetShadowMapTexture(1), NULL);
			D3DXSaveTextureToFileA("PSSM2.tga", D3DXIFF_TGA, m_pPSSM2->GetShadowMapTexture(2), NULL);
			D3DXSaveTextureToFileA("PSSM3.tga", D3DXIFF_TGA, m_pPSSM2->GetShadowMapTexture(3), NULL);
		}
	#endif	
	}
	else if (m_pMainShadow->GetType() == A3DShadowMap::TYPE_SSM)
	{
		m_pSSM->CreateShadowMap_DL(pEyeCamera, 100.0f, vLightDir, m_aShadowCasters.GetData(), m_aShadowCasters.GetSize());
		pShadowMap = m_pSSM;

	#ifdef _OUTPUT_RTS_
		if (GetAsyncKeyState(VK_F6) & 0x8000)
			D3DXSaveTextureToFileA("SSM.tga", D3DXIFF_TGA, m_pSSM->GetShadowMapTexture(), NULL);
	#endif
	}
	else if (m_pMainShadow->GetType() == A3DShadowMap::TYPE_TSM)
	{
		m_pTSM->CreateShadowMap_DL(pEyeCamera, 80.0f, vLightDir, m_aShadowCasters.GetData(), m_aShadowCasters.GetSize());
		pShadowMap = m_pTSM;

	#ifdef _OUTPUT_RTS_
		if (GetAsyncKeyState(VK_F6) & 0x8000)
			D3DXSaveTextureToFileA("TSM.tga", D3DXIFF_TGA, m_pTSM->GetShadowMapTexture(), NULL);
	#endif
	}

	return pShadowMap;
}

//	Build shadow map for lights
A3DShadowMap* ECWorldRender::BuildLightShadowMap(A3DLPPLight* pLight)
{
	A3DShadowMap* pShadowMap = NULL;

	if (pLight->GetType() == A3DLIGHT_SPOT)
	{
		A3DLPPLightSpot* pSpotLight = (A3DLPPLightSpot*)pLight;

		m_pSSM->CreateShadowMap_SL(pSpotLight->GetPos(), pSpotLight->GetDir(), pSpotLight->GetOuterAngle(),
						pSpotLight->GetRange(), m_aShadowCasters.GetData(), m_aShadowCasters.GetSize());

		pShadowMap = m_pSSM;
	}
	else if (pLight->GetType() == A3DLIGHT_POINT)
	{
		A3DLPPLightPoint* pPointLight = (A3DLPPLightPoint*)pLight;

		m_pCubeSM->CreateShadowMap(pPointLight->GetPos(), pPointLight->GetRange(), m_aShadowCasters.GetData(), 
						m_aShadowCasters.GetSize(), NULL);

		pShadowMap = m_pCubeSM;

	}
	else
	{
		ASSERT(0);
		return NULL;
	}

	return pShadowMap;
}

//	Create PSSM callback
void ECWorldRender::CreateShadowMapCallback(A3DShadowMap* pShadowMap, A3DViewport* pViewport, int iSlice)
{
	//	Reset renders
	m_psrShadow->ResetRenderInfo();
	m_plmrShadow->ResetAllMeshes();

	if (m_pLPPRender->IsDynamicShadowEnabled())
	{
		//	Render buildings
		m_Render.RenderShadowMap(pViewport, iSlice);
		m_plmrShadow->RenderOntoShadowMap(pViewport, pShadowMap, iSlice);
	}

	//	Render NPCs
	m_psrShadow->RenderOntoShadowMap(pViewport, pShadowMap, iSlice);

}

//	LPP render routine
bool ECWorldRender::LPPRender(A3DViewport* pViewport)
{
	m_pLPPRender->EnableStencilBuf(true);
	m_psrLPP->ResetRenderInfo();
	m_plmrLPP->ResetAllMeshes();

	m_pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(m_psrLPP);
	m_pA3DEngine->SetA3DLitModelRender(m_plmrLPP);
	
	// call back, real render functions...
	m_Render.RenderLPP(pViewport);

	m_pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(NULL);
	m_pA3DEngine->SetA3DLitModelRender(NULL);

	if (!BuildGBuffer(pViewport))
		return false;
	m_pLPPRender->DoIBL(pViewport);

	if (GetAsyncKeyState(VK_F8) & 0x8000)
	{
		D3DXSaveTextureToFileA("LightBuffer.dds", D3DXIFF_DDS, m_pLPPRender->GetLightBuffer()->GetTargetTexture(), NULL);
		D3DXSaveTextureToFileA("ShadowMask.dds", D3DXIFF_DDS, m_pLPPRender->GetShadowMaskTexture(), NULL);
	}


	LPPForwardRender(pViewport);
	return true;
}

//	Build G-Buffer
bool ECWorldRender::BuildGBuffer(A3DViewport* pViewport)
{
	if (!m_pLPPRender->BeginRenderGBuffer(pViewport))
		return false;

	m_plmrLPP->RenderOntoGBuf(pViewport);
	m_psrLPP->RenderOntoGBuf(pViewport);

	//	Render forest
/*	if (g_Game.GetTreeForest())
	{
		ECTreeForest* pForest = g_Game.GetTreeForest();
		pForest->SetFXTechUsage(A3DFXMaterial::TECH_USAGE_GBUF);

		A3DDevice* pA3DDevice = m_Render.GetA3DDevice();
		pForest->SetFogColor(m_Render.GetA3DDevice()->GetFogColor());
		pForest->SetFogDist(m_Render.GetA3DDevice()->GetFogStart(), m_Render.GetA3DDevice()->GetFogEnd());
		pForest->Render(pViewport, pViewport->GetCamera()->GetPos(), true, ECTreeForest::TREEFOREST_MESH);
	}*/

	//if (!g_Game.GetProperties()->IsShowTerrianGridEnabled())
	
	m_Render.GetTerrain()->RenderOntoGBuf(pViewport);

	m_pLPPRender->EndRenderGBuffer();
	return true;
}

//	LPP forward rendering
bool ECWorldRender::LPPForwardRender(A3DViewport* pViewport)
{
	m_pLPPRender->BeginForwardRender(pViewport);
	
	bool fog_enable = m_Render.GetA3DDevice()->GetFogEnable();
	
	// disable the fog here...
	m_Render.GetA3DDevice()->SetFogEnable(false);

	m_plmrLPP->RenderLPPForward(pViewport, m_pLPPRender);
	m_psrLPP->RenderLPPForward(pViewport, m_pLPPRender);

	//	Render forest
/*	if (g_Game.GetTreeForest())
	{
		ECTreeForest* pForest = g_Game.GetTreeForest();
		pForest->SetFXTechUsage(A3DFXMaterial::TECH_USAGE_LPPFWD);
		pForest->SetForestLPPBufTex(m_pLPPRender->GetLightBuffer()->GetTargetTexture());
		pForest->SetShadowMask(m_pLPPRender->GetShadowMask()->GetTargetTexture());

		A3DDevice* pA3DDevice = m_Render.GetA3DDevice();
		pForest->SetFogColor(m_Render.GetA3DDevice()->GetFogColor());
		pForest->SetFogDist(m_Render.GetA3DDevice()->GetFogStart(), m_Render.GetA3DDevice()->GetFogEnd());
		pForest->Render(pViewport, pViewport->GetCamera()->GetPos(), false, ECTreeForest::TREEFOREST_MESH);
	}*/

	//if (!g_Game.GetProperties()->IsShowTerrianGridEnabled())
	m_Render.GetTerrain()->RenderLPPForward(pViewport, m_pLPPRender);

	m_Render.GetA3DDevice()->SetFogEnable(fog_enable);
	m_pLPPRender->EndForwardRender();
	return true;
}

//	Render all other things by normal forward rendering
bool ECWorldRender::RenderOthers(A3DViewport* pViewport)
{
	pViewport->Active();
	A3DCameraBase* pCamera = pViewport->GetCamera();
//	CSettings* pSettings = &g_Game.GetSettings();
	//A3DTerrainWater* pWater = g_Game.GetTerrainWater();

//	bool bUnderWater = g_Game.IsUnderWater();

	//	Render sky after opaque meshes but before alpha meshes
	m_Render.GetSky()->Render(pViewport);

	//	Render water
/*	if (pSettings->GetRenderWaterFlag())
	{
		A3DTerrainWaterRender::RenderParam rp;
		rp.pCurViewport	= pViewport;
		rp.pTerrain = g_Game.GetTerrain();
		rp.pGBuffer = m_pLPPRender->GetGBuffer();
		rp.pFrameRT = g_Game.GetFrameBuffer();
		rp.pfnReflectCallBack = _RenderReflect;
		rp.pfnRefractCallBack = _RenderRefract;
		rp.pArg = (a_uiptr*)this;

		g_Game.GetTerrainWater()->Render(&rp);
	}*/

	m_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	m_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	m_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	m_Render.GetA3DDevice()->SetAlphaRef(84);
//	m_plmrLPP->RenderLeaf(pViewport);
	m_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
	m_Render.GetA3DDevice()->SetAlphaTestEnable(false);

	// to revise...
	//if (g_Game.GetProperties()->IsShowTerrianGridEnabled())
	//{
	//	m_Render.GetA3DDevice()->SetFillMode(A3DFILL_WIREFRAME);
	//	m_Render.GetTerrain()->Render(pViewport);
	//	m_Render.GetA3DDevice()->SetFillMode(A3DFILL_SOLID);
	//}

	m_plmrLPP->RenderAlpha(pViewport);
	return true;
}

//	Render reflection
bool ECWorldRender::RenderReflect(A3DViewport* pViewport)
{
	A3DSkinRender* psr = m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	A3DLitModelRender* plmr = m_pA3DEngine->GetA3DLitModelRender();

	psr->ResetRenderInfo();
	plmr->ResetAllMeshes();

//	g_Game.GetNPCMan()->RenderLPP(pViewport);
//	g_Game.GetBuildingMan()->RenderLPP(pViewport);

	plmr->RenderReflect(pViewport);
	psr->RenderReflect(pViewport);

//	g_Game.GetSky()->Render(pViewport);

	return true;
}

//	Render refraction
bool ECWorldRender::RenderRefract(A3DViewport* pViewport)
{
/*	A3DSkinRender* psr = m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	A3DLitModelRender* plmr = m_pA3DEngine->GetA3DLitModelRender();

	psr->ResetRenderInfo();
	plmr->ResetAllMeshes();

	g_Game.GetNPCMan()->RenderLPP(pViewport);
	g_Game.GetBuildingMan()->RenderLPP(pViewport);

	plmr->RenderReflect(pViewport);
	psr->RenderReflect(pViewport);
*/
	return true;
}

