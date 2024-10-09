/*
 * FILE: EC_ShadowRender.cpp
 *
 * DESCRIPTION: a class for shadow rendering
 *
 * CREATED BY: Hedi, 2004/9/21
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */
#include "stdafx.h"

#ifndef _ANGELICA3

#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DFuncs.h>
#include <A3DCamera.h>
#include <A3DOrthoCamera.h>
#include <A3DStream.h>
#include <A3DTexture.h>
#include <A3DViewport.h>
#include <A3DTerrain2.h>
#include <A3DPixelShader.h>
#include <A3DVertexShader.h>
#include <A3DRenderTarget.h>
#include <A3DShaderMan.h>
#include <A3DLitModel.h>
#include <A3DSurface.h>
#include <A3DSurfaceMan.h>
#include <A3DShadowMapEx.h>
#include <A3DSceneRenderConfig.h>
#include "EC_Model.h"
#include "EC_TriangleMan.h"
#include "EC_ShadowRender.h"

#ifdef _ELEMENTCLIENT

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_CDS.h"
#define new A_DEBUG_NEW

#endif

A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

static const D3DVERTEXELEMENT9 aVertDecl[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	D3DDECL_END()
};

static const float s_fViewRadiusMax			= 12.f;
static const float s_fViewportSizeMax		= 11.f;
static const float s_fFadeStartMax			= 6.0f;
static const float s_fViewRadiusMin			= 5.f;
static const float s_fViewportSizeMin		= 6.f;
static const float s_fFadeStartMin			= 3.0f;

const int _max_terrain_surface_cell_count = 1000;

static TMan_Overlap_Info s_info;
static abase::hash_map<DWORD, int> s_terr_cells;

static bool GetWorldBounds(const A3DAABB& aabb, const A3DMATRIX4& viewTM, float& l, float& r, float& b, float& t, float& n, float& f)
{
	A3DVECTOR3 verts[8];
	A3DVECTOR3 vecX = A3DVECTOR3(2 * aabb.Extents.x, 0.0f, 0.0f);
	A3DVECTOR3 vecY = A3DVECTOR3(0.0f, 2 * aabb.Extents.y, 0.0f);
	A3DVECTOR3 vecZ = A3DVECTOR3(0.0f, 0.0f, 2 * aabb.Extents.z);

	verts[0] = aabb.Mins;
	verts[1] = aabb.Mins + vecX;
	verts[2] = aabb.Mins + vecY;
	verts[3] = aabb.Mins + vecZ;
	verts[4] = aabb.Maxs;
	verts[5] = aabb.Maxs - vecX;
	verts[6] = aabb.Maxs - vecY;
	verts[7] = aabb.Maxs - vecZ;

	A3DAABB aabbWorld;
	aabbWorld.Clear();

	A3DVECTOR3 vert;
	int i;
	for(i=0; i<8; i++)
	{
		vert = verts[i]	* viewTM;
		aabbWorld.AddVertex(vert);
	}

	l = aabbWorld.Mins.x;
	r = aabbWorld.Maxs.x;
	b = aabbWorld.Mins.y;
	t = aabbWorld.Maxs.y;
	n = aabbWorld.Mins.z;
	f = aabbWorld.Maxs.z;
	return true;
}

CECShadowRender::CECShadowRender() : m_TerrSurfaceCellCachePool(_max_terrain_surface_cell_count)
{
	m_pA3DTerrain				= NULL;
	m_pTriangleMan				= 0;

	m_pShadowMapViewport		= NULL;
	m_pShadowMapCamera			= NULL;
	m_pShadowMapTarget			= NULL;
	m_pShadowStream				= NULL;
	m_pSimpleShadowStream		= NULL;

	m_pVertexShader				= NULL;
	m_pShadowShader				= NULL;

	m_pSimpleShadowMap			= NULL;

	m_bCastShadow				= false;
	m_bSimple					= false;

	m_fViewRadius				= s_fViewRadiusMax;
	m_fViewportSize				= s_fViewportSizeMax;
	m_fFadeStart				= s_fFadeStartMax;

	for (int i = 0; i < _max_terrain_surface_cell_count; i++)
	{
		TERRAIN_SURFACE_CELL* p = new TERRAIN_SURFACE_CELL;
		memset(p, 0, sizeof(*p));
		m_TerrSurfaceCellCachePool.AddTail(p);
	}

	s_info.pVert.reserve(5000);
	s_info.pIndex.reserve(5000);
}

CECShadowRender::~CECShadowRender()
{
	Release();

	ALISTPOSITION pos = m_TerrSurfaceCellCachePool.GetHeadPosition();

	while (pos)
	{
		TERRAIN_SURFACE_CELL* p = m_TerrSurfaceCellCachePool.GetNext(pos);
		delete p;
	}

	m_TerrSurfaceCellCachePool.RemoveAll();
}

void CECShadowRender::Reset()
{
	ALISTPOSITION pos = m_TerrSurfaceCellCachePool.GetHeadPosition();

	while (pos)
	{
		TERRAIN_SURFACE_CELL* p = m_TerrSurfaceCellCachePool.GetNext(pos);
		memset(p, 0, sizeof(*p));
	}

	m_TerrSurfaceCellCacheMap.clear();
}

bool CECShadowRender::AllocResource()
{
	ReleaseResource();
	A3DDevice* pA3DDevice = AfxGetA3DDevice();

	if (m_bSimple)
	{
		// now create render targets for shadow rendering
		m_pShadowMapCamera = new A3DOrthoCamera();

		if (!m_pShadowMapCamera->Init(pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f))
		{
			a_LogOutput(1, "CECShadowRender::Init(), failed to create the shadow map camera!");
			return false;
		}

		m_pShadowMapCamera->SetProjectionParam(-m_fViewportSize, m_fViewportSize, -m_fViewportSize, m_fViewportSize, -100.f, 100.0f);

		//	Create shadow map Viewport
		RECT rc;
		rc.left = 0;
		rc.right = m_nShadowMapSize;
		rc.top = 0;
		rc.bottom = m_nShadowMapSize;

		if (!pA3DDevice->CreateViewport(&m_pShadowMapViewport, 0, 0, rc.right, rc.bottom, 0.0f, 1.0f, true, true, 0x00000000))
		{
			a_LogOutput(1, "CECShadowRender::Create shadow map viewport fail");
			return false;
		}

		m_pShadowMapViewport->SetCamera(m_pShadowMapCamera);

		A3DSurface * pSurface = NULL;

		if (!pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->LoadSurfaceFromFile("ingame\\simpleshadow.tga", 0, &pSurface))
		{
			a_LogOutput(1, "CECShadowRender::Create load simple shadow map failed!");
			return false;
		}

		A3DRECT rect(0, 0, pSurface->GetWidth(), pSurface->GetHeight());
		int nWidth, nHeight;
		m_pSimpleShadowMap = new A3DTexture();

		if (!m_pSimpleShadowMap->CreateFromSurface(pA3DDevice, pSurface, true, rect, &nWidth, &nHeight))
		{
			a_LogOutput(1, "CECShadowRender::Create create simple shadow map texture failed!");
			pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->ReleaseSurface(pSurface);
			return false;
		}

		pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->ReleaseSurface(pSurface);

		// create light's shadow map render target
		A3DRenderTarget::RTFMT devFmt;
		devFmt.iWidth		= m_nShadowMapSize;
		devFmt.iHeight		= m_nShadowMapSize;
		devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
		devFmt.fmtDepth		= A3DFMT_D24X8;

		m_pShadowMapTarget = new A3DRenderTarget();

		if (!m_pShadowMapTarget->Init(pA3DDevice, devFmt, true, true))
			return false;

		// now load vertex shader for shadow render
		m_pVertexShader = pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\vs\\dx9\\shadow_vs.txt", false);

		if (NULL == m_pVertexShader)
		{
			a_LogOutput(1, "CECShadowRender::Init(), failed to load shadow_vs.txt");
			return false;
		}

		m_pVertexShader->SetDecl(aVertDecl);

		// now load pixel shader for render shadow
		m_pShadowShader = pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\ps\\dx9\\shadowrenderblur.txt", false);

		if (NULL == m_pShadowShader)
		{
			a_LogOutput(1, "CECShadowRender::Init(), failed to load shadowrenderblur.txt");
			return false;
		}

		// now create streams
		if (!CreateStream(32000, 32000))
		{
			a_LogOutput(1, "CECShadowRender::Init(), failed to create stream!");
			return false;
		}

		return true;
	}
	else
	{
		return pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->EnableDynShadow(m_nShadowMapSize, m_fViewportSize, m_fFadeStart);
	}
}

bool CECShadowRender::ReleaseResource()
{
	A3DDevice* pA3DDevice = AfxGetA3DDevice();
	pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->DisableDynShadow();

	ReleaseStream();

	if (m_pSimpleShadowMap)
	{
		m_pSimpleShadowMap->Release();
		delete m_pSimpleShadowMap;
		m_pSimpleShadowMap = NULL;
	}

	if (m_pShadowMapTarget)
	{
		m_pShadowMapTarget->Release();
		delete m_pShadowMapTarget;
		m_pShadowMapTarget = NULL;
	}

	if (m_pShadowMapViewport)
	{
		m_pShadowMapViewport->Release();
		delete m_pShadowMapViewport;
		m_pShadowMapViewport = NULL;
	}

	if (m_pShadowMapCamera)
	{
		m_pShadowMapCamera->Release();
		delete m_pShadowMapCamera;
		m_pShadowMapCamera = NULL;
	}

	if (m_pVertexShader)
	{
		pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pVertexShader);
		m_pVertexShader = NULL;
	}

	if (m_pShadowShader)
	{
		pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pShadowShader);
		m_pShadowShader = NULL;
	}

	return true;
}

bool CECShadowRender::Init(int nShadowMapSize)
{
	m_nShadowMapSize = nShadowMapSize;
	m_bCastShadow = false;
	return true;
}

bool CECShadowRender::Release()
{
	m_arrayShadowers.RemoveAll();

	ReleaseResource();
	Reset();
	return true;
}

bool CECShadowRender::CreateStream(int nMaxVerts, int nMaxFaces)
{
	// try to release old ones.
	ReleaseStream();

	A3DDevice * pA3DDevice = AfxGetA3DDevice();

	m_nMaxVerts = nMaxVerts;
	m_nMaxFaces = nMaxFaces;

	m_pShadowStream = new A3DStream();

	if (!m_pShadowStream->Init(pA3DDevice, sizeof(A3DSHADOWVERTEX), A3DSHADOWVERT_FVF, m_nMaxVerts, m_nMaxFaces * 3, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		a_LogOutput(1, "CECShadowRender::CreateStream(), fail to create stream!");
		return false;
	}

	m_pSimpleShadowStream = new A3DStream();

	if (!m_pSimpleShadowStream->Init(pA3DDevice, sizeof(A3DSIMPLESHADOWVERTEX), A3DSIMPLESHADOWVERT_FVF, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		a_LogOutput(1, "CECShadowRender::CreateStream(), fail to create stream!");
		return false;
	}

	WORD* pIndices;
	m_pSimpleShadowStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0);

	pIndices[0]		= 0;
	pIndices[1]		= 1;
	pIndices[2]		= 2;
	pIndices[3]		= 1;
	pIndices[4]		= 3;
	pIndices[5]		= 2;
	m_pSimpleShadowStream->UnlockIndexBuffer();

	m_nVertCount = 0;
	m_nFaceCount = 0;

	m_nOccludeVertCount = 0;
	m_nOccludeFaceCount = 0;

	return true;
}

bool CECShadowRender::ReleaseStream()
{
	A3DRELEASE(m_pShadowStream);
	A3DRELEASE(m_pSimpleShadowStream);
	return true;
}

bool CECShadowRender::AddShadower(const A3DVECTOR3& vecCenter, const A3DVECTOR3& vecExts)
{
	SHADOWER s;
	s.vecCenter = vecCenter;
	s.vecExts = vecExts;
	m_arrayShadowers.Add(s);
	return true;
}

bool CECShadowRender::Render(A3DViewport * pViewport, const A3DVECTOR3& vCenterPos, const A3DVECTOR3& vecLightDir)
{
	if (!m_bSimple)
	{
		ClearShadowers();
		return true;
	}

	if (!m_bCastShadow)
	{
		ClearShadowers();
		return true;
	}

	int nNumShadow = m_arrayShadowers.GetSize();

	if (nNumShadow == 0)
		return true;

	A3DVECTOR3 vDirLight = vecLightDir;
	vDirLight.Normalize();

	A3DVECTOR3 vDirUp = g_vAxisY;

	if (Magnitude(vDirLight + g_vAxisY) < 1e-3)
		vDirUp = g_vAxisZ;

	const bool bSimpleShadow = true;

	if (!SetupStreamForShadower(pViewport, vCenterPos, vDirLight, bSimpleShadow))
		return true;

	if (m_nVertCount == 0 || m_nFaceCount == 0)
		return true;

	A3DMATRIX4 matScale = IdentityMatrix();
	matScale._11 = 0.5f;
	matScale._22 = -0.5f;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;
	A3DMATRIX4 matProjectedView;

	A3DCameraBase* pHostCamera = pViewport->GetCamera();
	A3DDevice* pA3DDevice = AfxGetA3DDevice();
	pA3DDevice->SetLightingEnable(false);

	m_pShadowMapCamera->SetDirAndUp(vDirLight, vDirUp);

	A3DMATRIX4 matOffsetLT, matOffsetRT, matOffsetLB, matOffsetRB;
	matOffsetLT = IdentityMatrix();
	matOffsetLT._41 = -1.0f / m_nShadowMapSize;
	matOffsetRT = IdentityMatrix();
	matOffsetRT._41 = 1.0f / m_nShadowMapSize;
	matOffsetLB = IdentityMatrix();
	matOffsetLB._42 = -1.0f / m_nShadowMapSize;
	matOffsetRB = IdentityMatrix();
	matOffsetRB._42 = 1.0f / m_nShadowMapSize;

	m_pShadowMapCamera->SetPos(vCenterPos - vecLightDir * 30.f);

	// first render into light's shadow map
	//pA3DDevice->SetRenderTarget(m_pShadowMapTarget);
	m_pShadowMapTarget->ApplyToDevice();
	
	m_pShadowMapViewport->Active();
	m_pShadowMapViewport->ClearDevice();

	// first render the ground for clip, use z buffer to do clip
	A3DCULLTYPE cullType = pA3DDevice->GetFaceCull();
	pA3DDevice->SetFaceCull(A3DCULL_CCW);

	if (bSimpleShadow)
	{
		pA3DDevice->SetZWriteEnable(false);
	}
	else
	{
		m_pShadowStream->Appear(0);
		pA3DDevice->ClearTexture(0);
		pA3DDevice->SetWorldMatrix(IdentityMatrix());
		pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0);
		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nFaceCount);
		pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
	}

	for (int i = 0; i < nNumShadow; i++)
	{
		SHADOWER& s = m_arrayShadowers[i];

		if (bSimpleShadow)
		{
			A3DSIMPLESHADOWVERTEX* pVerts;

			if (m_pSimpleShadowStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0))
			{
				const A3DVECTOR3& center = s.vecCenter;
				float radius = a_Max(s.vecExts.x, s.vecExts.z) * 2.0f + 0.2f;
				pVerts[0].Set(A3DVECTOR3(center.x - radius, center.y, center.z + radius), 0xffffffff, 0.0f, 0.0f);
				pVerts[1].Set(A3DVECTOR3(center.x + radius, center.y, center.z + radius), 0xffffffff, 1.0f, 0.0f);
				pVerts[2].Set(A3DVECTOR3(center.x - radius, center.y, center.z - radius), 0xffffffff, 0.0f, 1.0f);
				pVerts[3].Set(A3DVECTOR3(center.x + radius, center.y, center.z - radius), 0xffffffff, 1.0f, 1.0f);
				m_pSimpleShadowStream->UnlockVertexBuffer();
				m_pSimpleShadowStream->Appear(0);
				m_pSimpleShadowMap->Appear(0);
				pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
				m_pSimpleShadowMap->Disappear(0);
			}
		}
	}

	/*
	if (g_AppCmdParams.iRenderDebug > 0)
	{
		if (GetKeyState(VK_F10) & 0x8000)
			D3DXSaveSurfaceToFileA("shadowtexture.dds", D3DXIFF_DDS, m_pShadowMapTarget->GetTargetSurface(), NULL, NULL);
	}
	*/

	//pA3DDevice->RestoreRenderTarget();
	m_pShadowMapTarget->WithdrawFromDevice();

	pViewport->Active();

	IDirect3DTexture9 * pShadowMap = m_pShadowMapTarget->GetTargetTexture(); 

	// 2. render shadow in the world
	m_pShadowMapTarget->AppearAsTexture(0);
	m_pShadowMapTarget->AppearAsTexture(1);
	m_pShadowMapTarget->AppearAsTexture(2);
	m_pShadowMapTarget->AppearAsTexture(3);
	m_pShadowMapTarget->AppearAsTexture(4);
	pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);

	pA3DDevice->SetTextureAddress(0, A3DTADDR_BORDER, A3DTADDR_BORDER);
	pA3DDevice->SetTextureAddress(1, A3DTADDR_BORDER, A3DTADDR_BORDER);
	pA3DDevice->SetTextureAddress(2, A3DTADDR_BORDER, A3DTADDR_BORDER);
	pA3DDevice->SetTextureAddress(3, A3DTADDR_BORDER, A3DTADDR_BORDER);
	pA3DDevice->SetTextureAddress(4, A3DTADDR_BORDER, A3DTADDR_BORDER);

	pA3DDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
	pA3DDevice->SetSamplerState(1, D3DSAMP_BORDERCOLOR, 0);
	pA3DDevice->SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0);
	pA3DDevice->SetSamplerState(3, D3DSAMP_BORDERCOLOR, 0);
	pA3DDevice->SetSamplerState(4, D3DSAMP_BORDERCOLOR, 0);

	m_pShadowShader->Appear();
	m_pShadowStream->Appear(0);
	m_pVertexShader->Appear();
	

	pA3DDevice->SetWorldMatrix(IdentityMatrix());
	matProjectedView = m_pShadowMapCamera->GetVPTM() * matScale;

	//pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	//pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	//pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	//pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	//pA3DDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	// set vertex shader constants here
	A3DMATRIX4 matVP = pA3DDevice->GetViewMatrix() * pA3DDevice->GetProjectionMatrix();
	matVP.Transpose();
	pA3DDevice->SetVertexShaderConstants(0, &matVP, 4);

	A3DVECTOR4 c4(pHostCamera->GetPos());
	pA3DDevice->SetVertexShaderConstants(4, &c4, 1);

	A3DVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
	pA3DDevice->SetVertexShaderConstants(5, &c5, 1);

	A3DMATRIX4 matTex = IdentityMatrix();
	matTex = matProjectedView;
	matTex.Transpose();
	pA3DDevice->SetVertexShaderConstants(6, &matTex, 4);

	matTex = matProjectedView * matOffsetLT;
	matTex.Transpose();
	pA3DDevice->SetVertexShaderConstants(10, &matTex, 4);

	matTex = matProjectedView * matOffsetRT;
	matTex.Transpose();
	pA3DDevice->SetVertexShaderConstants(14, &matTex, 4);

	matTex = matProjectedView * matOffsetLB;
	matTex.Transpose();
	pA3DDevice->SetVertexShaderConstants(18, &matTex, 4);
	
	matTex = matProjectedView * matOffsetRB;
	matTex.Transpose();
	pA3DDevice->SetVertexShaderConstants(22, &matTex, 4);

	A3DVECTOR4 c2(0.2f, 0.2f, 0.2f, bSimpleShadow ? 0.1f : 0.1f);
	pA3DDevice->SetPixelShaderConstants(2, &c2, 1);

	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetZBias(2);

	pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nFaceCount);

	pA3DDevice->SetZBias(0);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->ClearTexture(0);
	pA3DDevice->ClearTexture(1);
	pA3DDevice->ClearTexture(2);
	pA3DDevice->ClearTexture(3);
	pA3DDevice->ClearTexture(4);
	pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureCoordIndex(0, 0);
	pA3DDevice->SetTextureCoordIndex(1, 1);
	pA3DDevice->SetTextureCoordIndex(2, 2);
	pA3DDevice->SetTextureCoordIndex(3, 3);
	pA3DDevice->SetTextureCoordIndex(4, 4);

	
	m_pVertexShader->Disappear();
	m_pShadowShader->Disappear();
	pA3DDevice->SetFaceCull(cullType);

#ifdef _DEBUG

#ifdef _ELEMENTCLIENT
	if (g_AppCmdParams.iRenderDebug == 3)
	{
		cullType = pA3DDevice->GetFaceCull();
		m_pShadowStream->Appear(0);
		pA3DDevice->SetWorldMatrix(IdentityMatrix());
		pA3DDevice->SetFaceCull(A3DCULL_NONE);
		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nFaceCount);
		pA3DDevice->SetFaceCull(cullType);
	}
#endif

#endif

	ClearShadowers();
	return true;
}

bool CECShadowRender::ClearShadowers()
{
	if (m_arrayShadowers.GetSize())
		m_arrayShadowers.RemoveAll();

	return true;
}

void CECShadowRender::AdjustShadowRadius(float fRatio)
{
	float f = 1.0f - fRatio;
	m_fViewRadius				= s_fViewRadiusMax * fRatio + s_fViewRadiusMin * f;
	m_fViewportSize				= s_fViewportSizeMax * fRatio + s_fViewportSizeMin * f;
	m_fFadeStart				= s_fFadeStartMax * fRatio + s_fFadeStartMin * f;
	a_Clamp(m_fViewRadius, s_fViewRadiusMin, s_fViewRadiusMax);
	a_Clamp(m_fViewportSize, s_fViewportSizeMin, s_fViewportSizeMax);
	a_Clamp(m_fFadeStart, s_fFadeStartMin, s_fFadeStartMax);
}

bool CECShadowRender::IsShadowVisble(const A3DAABB& aabb, float fYOffset, const A3DVECTOR3& vCenterPos, A3DCameraBase* pCamera, const A3DVECTOR3& vLightHDir)
{
	float fRadius = a_Max(aabb.Extents.x, aabb.Extents.z) * 1.42f;
	float fHalfExt = aabb.Extents.y;
	A3DVECTOR3 vShadowCenter = aabb.Center + vLightHDir * (fHalfExt + fYOffset);
	fHalfExt += fRadius;

	float fDist = MagnitudeH(vShadowCenter - vCenterPos);

	if (fDist + fHalfExt > m_fViewportSize)
		return false;

	A3DAABB aabbShadow;
	aabbShadow.Center = vShadowCenter;
	aabbShadow.Center.y -= aabb.Extents.y + fYOffset;
	aabbShadow.Extents.Set(fHalfExt, 1.0f, fHalfExt);
	aabbShadow.CompleteMinsMaxs();

	return pCamera->AABBInViewFrustum(aabbShadow);
}

bool CECShadowRender::SetupStreamForShadower(A3DViewport* pViewport, const A3DVECTOR3& vCenterPos, const A3DVECTOR3& vecLightDir, bool bSimpleShadow)
{
	m_nVertCount = 0;
	m_nFaceCount = 0;

	m_nOccludeVertCount = 0;
	m_nOccludeFaceCount = 0;

	A3DSHADOWVERTEX* pVerts;
	WORD* pIndices;

	if (!m_pShadowStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0))
		return false;

	if (!m_pShadowStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0))
	{
		m_pShadowStream->UnlockVertexBuffer();
		return false;
	}

	A3DVECTOR3 vLightHDir;
	vLightHDir.Set(vecLightDir.x, 0, vecLightDir.z);
	vLightHDir.Normalize();
	float fCos = DotProduct(vecLightDir, vLightHDir);
	float fSin = sqrtf(1.0f - fCos * fCos);
	float co;

	if (fSin > .1f)
	{
		co = fCos / fSin;
		a_ClampRoof(co, 3.0f);
	}
	else
		co = 3.0f;

#define	 MAKE_CELL_ID(r, c)		((DWORD)(((WORD)((c) & 0xffff)) | ((DWORD)((WORD)((r) & 0xffff))) << 16))
#define  GET_CELL_COL(dw)		((WORD)((DWORD)(dw) & 0xffff))	
#define	 GET_CELL_ROW(dw)		((WORD)((DWORD)(dw) >> 16))

	A3DVECTOR3* vTerrVerts;
	WORD* aTerrIndices;
	A3DCameraBase* pCamera = pViewport->GetCamera();
	int nBool = 0;
	const ARectF& rcTerr = m_pA3DTerrain->GetTerrainArea();
	int nNumShadow = m_arrayShadowers.GetSize();
	A3DAABB aabbShadow;
	s_terr_cells.clear();

	for (int i = 0; i < nNumShadow; i++)
	{
		const SHADOWER& s = m_arrayShadowers[i];
		float fHalfExt;
		A3DVECTOR3 vShadowCenter;
		float yOffset;

		fHalfExt = a_Max(s.vecExts.x, s.vecExts.z) * 2.5f + 0.2f;
		vShadowCenter = s.vecCenter;
		yOffset = 0;
		a_ClampRoof(fHalfExt, 6.f);

		s_info.pVert.clear();
		s_info.pIndex.clear();
		s_info.vStart = vShadowCenter;
		s_info.vStart.y += s.vecExts.y;// 1.0f - s.vecExts.y - s.fYOffset;
		float fMinY = vShadowCenter.y - s.vecExts.y - yOffset - 2.f;
		s_info.vExtent.Set(fHalfExt, fMinY, fHalfExt);
		s_info.vDelta = vecLightDir;
		s_info.bIncCellStamp = (nBool == 0);
		s_info.bCheckNormal = true;
		m_pTriangleMan->GetCell(s_info);
		nBool++;

		int nTriVertCount = s_info.pVert.size();
		int nTriIndexCount = s_info.pIndex.size();

		if (nTriVertCount && nTriIndexCount && nTriVertCount + m_nVertCount <= m_nMaxVerts && nTriIndexCount / 3 + m_nFaceCount <= m_nMaxFaces)
		{
			// now fill these verts into shadow buffer.
			int c;

			for (c = 0; c < nTriVertCount; c++)
			{
				const A3DVECTOR3& vPos = s_info.pVert[c];
				float dx = vPos.x - vCenterPos.x;
				float dz = vPos.z - vCenterPos.z;
				float fDistH = sqrtf(dx * dx + dz * dz);
				float fAlpha = (m_fViewRadius - fDistH) / (m_fViewportSize - m_fFadeStart);
				a_Clamp(fAlpha, 0.0f, 1.0f);
				A3DCOLOR cl = A3DCOLORRGBA(255, 255, 255, (DWORD)(fAlpha * 255));
				pVerts[c].Set(vPos, cl);
			}

			for (c = 0; c < nTriIndexCount; c++)
				pIndices[c] = m_nVertCount + s_info.pIndex[c];

			m_nVertCount += nTriVertCount;
			m_nFaceCount += nTriIndexCount / 3;

			pVerts += nTriVertCount;
			pIndices += nTriIndexCount;
		}

		int nLeft	= int(floorf(vShadowCenter.x - fHalfExt - rcTerr.left));
		int nRight	= int(ceilf (vShadowCenter.x + fHalfExt - rcTerr.left));
		int nTop	= int(floorf(rcTerr.top - (vShadowCenter.z + fHalfExt)));
		int nBottom	= int(ceilf (rcTerr.top - (vShadowCenter.z - fHalfExt)));
		int nWidth = nRight - nLeft;
		int nHeight = nBottom - nTop;
		int nSquare = nWidth * nHeight;
		int nTerrVertCount = nSquare * 4;
		int nTerrFaceCount = nSquare * 2;

		if (nTerrVertCount && nTerrFaceCount && nTerrVertCount + m_nVertCount <= m_nMaxVerts && nTerrFaceCount + m_nFaceCount <= m_nMaxFaces)
		{
			for (int j = nLeft; j < nRight; j++)
			{
				for (int k = nTop; k < nBottom; k++)
				{
					DWORD cell = MAKE_CELL_ID(k, j);

					if (s_terr_cells.find(cell) != s_terr_cells.end())
						continue;

					s_terr_cells[cell] = 1;
					TERRAIN_SURFACE_CELL* pCellData = FindTerrainSurfaceCell(cell);

					if (pCellData)
					{
						if (!pCellData->bValid)
							continue;

						vTerrVerts = pCellData->aVerts;
						aTerrIndices = pCellData->aIndices;

						if (vTerrVerts[0].y > fMinY || vTerrVerts[1].y > fMinY || vTerrVerts[2].y > fMinY || vTerrVerts[3].y > fMinY)
						{
							int n;
							for (n = 0; n < 4; n++)
							{
								const A3DVECTOR3& vPos = vTerrVerts[n];
								float dx = vPos.x - vCenterPos.x;
								float dz = vPos.z - vCenterPos.z;
								float fDistH = sqrtf(dx * dx + dz * dz);
								float fAlpha = (m_fViewRadius - fDistH) / (m_fViewportSize - m_fFadeStart);
								a_Clamp(fAlpha, 0.0f, 1.0f);
								A3DCOLOR cl = A3DCOLORRGBA(255, 255, 255, (DWORD)(fAlpha * 255));
								pVerts[n].Set(vPos, cl);
							}

							for (n = 0; n < 6; n++)
								pIndices[n] = aTerrIndices[n] + m_nVertCount;

							pVerts += 4;
							m_nVertCount += 4;
							pIndices += 6;
							m_nFaceCount += 2;
						}

						continue;
					}

					ARectI rc(j, k, j+1, k+1);

					pCellData = AddSurfaceCell(cell);
					vTerrVerts = pCellData->aVerts;
					aTerrIndices = pCellData->aIndices;

					if (m_pA3DTerrain->GetFacesOfArea(rc, vTerrVerts, aTerrIndices))
					{
						if (vTerrVerts[0].y > fMinY || vTerrVerts[1].y > fMinY || vTerrVerts[2].y > fMinY || vTerrVerts[3].y > fMinY)
						{
							int n;
							for (n = 0; n < 4; n++)
							{
								const A3DVECTOR3& vPos = vTerrVerts[n];
								float dx = vPos.x - vCenterPos.x;
								float dz = vPos.z - vCenterPos.z;
								float fDistH = sqrtf(dx * dx + dz * dz);
								float fAlpha = (m_fViewRadius - fDistH) / (m_fViewportSize - m_fFadeStart);
								a_Clamp(fAlpha, 0.0f, 1.0f);
								A3DCOLOR cl = A3DCOLORRGBA(255, 255, 255, (DWORD)(fAlpha * 255));
								pVerts[n].Set(vPos, cl);
							}

							for (n = 0; n < 6; n++)
								pIndices[n] = aTerrIndices[n] + m_nVertCount;

							pVerts += 4;
							m_nVertCount += 4;
							pIndices += 6;
							m_nFaceCount += 2;
						}

						pCellData->bValid = true;
					}
					else
						pCellData->bValid = false;
				}
			}
		}
	}

	m_pShadowStream->UnlockVertexBuffer();
	m_pShadowStream->UnlockIndexBuffer();
	return true;
}

bool CECShadowRender::SetCastShadowFlag(bool bFlag, bool bSimple)
{
	if (bFlag == m_bCastShadow)
	{
		if (!bFlag)
			return true;
		else if (bSimple == m_bSimple)
			return true;
	}

	m_bCastShadow = bFlag;
	m_bSimple = bSimple;

	if (m_bCastShadow)
	{
		// we should create resource for shadow
		if (!AllocResource())
		{
			ReleaseResource();
			m_bCastShadow = false;
			return false;
		}
	}
	else
	{
		// resource for shadow are not needed, so just release them
		ReleaseResource();
	}

	return true;
}

#endif  //_ANGELICA3