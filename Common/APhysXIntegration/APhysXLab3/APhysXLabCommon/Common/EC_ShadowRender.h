/*
 * FILE: EC_ShadowRender.h
 *
 * DESCRIPTION: a class for shadow rendering
 *
 * CREATED BY: Hedi, 2004/9/21
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _EC_SHADOWRENDER_H_
#define _EC_SHADOWRENDER_H_

#ifndef _ANGELICA3

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "AArray.h"

class A3DDevice;
class A3DCamera;
class A3DOrthoCamera;
class A3DStream;
class A3DTexture;
class A3DViewport;
class A3DTerrain2;
class A3DRenderTarget;
class A3DPixelShader;
class A3DVertexShader;
class A3DLitModel;
struct SHADOW_VERTEX_INFO;

// Vertex for shadow area
#define A3DSHADOWVERT_FVF			D3DFVF_XYZ | D3DFVF_DIFFUSE
#define A3DSIMPLESHADOWVERT_FVF		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1

struct A3DSHADOWVERTEX
{
	A3DSHADOWVERTEX() {}
	void Set(const A3DVECTOR3& _pos, A3DCOLOR _diffuse)
	{
		vPos = _pos;
		diffuse = _diffuse;
	}

	A3DVECTOR3	vPos;			//	Position
	A3DCOLOR	diffuse;		//	Color
};

struct A3DSIMPLESHADOWVERTEX
{
	A3DSIMPLESHADOWVERTEX() {}
	void Set(const A3DVECTOR3& _pos, A3DCOLOR _diffuse, float _u, float _v)
	{
		vPos = _pos;
		diffuse = _diffuse;
		u = _u;
		v = _v;
	}

	A3DVECTOR3	vPos;			//	Position
	A3DCOLOR	diffuse;		//	Color
	float		u;
	float		v;
};

// shadow object rendering callback
typedef bool (*LPRENDERFORSHADOW)(A3DViewport *, void * pArg);

// what receive the shadows
enum SHADOW_RECEIVER
{
	SHADOW_RECEIVER_TERRAIN = 0x1,
	SHADOW_RECEIVER_LITMODEL = 0x2,
	SHADOW_RECEIVER_ZEROPLANE = 0x4,
};

struct TERRAIN_SURFACE_CELL
{
	DWORD dwId;
	A3DVECTOR3 aVerts[4];
	WORD aIndices[6];
	bool bUsed;
	bool bValid;
};

typedef abase::hash_map<DWORD, TERRAIN_SURFACE_CELL*> TerrainSurfaceCellMap;

// class represent shadow renderer
class CECShadowRender
{
private:
	struct SHADOWER
	{
		A3DVECTOR3			vecCenter;
		A3DVECTOR3			vecExts;
	};

	bool				m_bCastShadow;				// flag indicates if cast shadow now.
	bool				m_bSimple;
	
	A3DTerrain2 *		m_pA3DTerrain;				// terrain shadow receiver
	CECTriangleMan*		m_pTriangleMan;

	int					m_nShadowMapSize;			// shadow map's size for the light
	A3DViewport *		m_pShadowMapViewport;		// viewport to render light's shadow map
	A3DOrthoCamera *	m_pShadowMapCamera;			// camera to do rendering of light's shadow map
	A3DRenderTarget *	m_pShadowMapTarget;			// render target to render light's shadow map
	float				m_fViewRadius;
	float				m_fViewportSize;
	float				m_fFadeStart;

	int					m_nMaxVerts;
	int					m_nMaxFaces;				// max vertex and face used to show the shadow
	int					m_nVertCount;				// shadow stream vertex count used to show the shadow
	int					m_nFaceCount;				// shadow stream face count used to show the shadow
	int					m_nOccludeVertCount;		// verts that can create occlude things.
	int					m_nOccludeFaceCount;		// faces that can create occlude things.
	A3DStream*			m_pShadowStream;			// shadow stream
	A3DStream*			m_pSimpleShadowStream;
	

	A3DVertexShader *	m_pVertexShader;			// shadow vertex shader
	A3DPixelShader *	m_pShadowShader;			// shadow rendering shader

	A3DTexture *		m_pSimpleShadowMap;			// Texture used to do simple shadow map
	
	AArray<SHADOWER, SHADOWER&> m_arrayShadowers;	// shadower array

	TerrainSurfaceCellMap m_TerrSurfaceCellCacheMap;
	APtrList<TERRAIN_SURFACE_CELL*> m_TerrSurfaceCellCachePool;

protected:
	bool AllocResource();
	bool ReleaseResource();

	bool CreateStream(int nMaxVerts, int nMaxFaces);
	bool ReleaseStream();

	bool SetupStreamForShadower(A3DViewport * pViewport, const A3DVECTOR3& vCenterPos, const A3DVECTOR3& vecLightDir, bool bSimpleShadow);

public:
	CECShadowRender();
	~CECShadowRender();

	bool Init(int nShadowMapSize);
	bool Release();
	void Reset();

	bool AddShadower(const A3DVECTOR3& vecCenter, const A3DVECTOR3& vecExts);
	bool Render(A3DViewport * pViewport, const A3DVECTOR3& vCenterPos, const A3DVECTOR3& vecLightDir);
	bool ClearShadowers();

	bool SetCastShadowFlag(bool bFlag, bool bSimple);
	inline bool GetCastShadowFlag() const { return m_bCastShadow; }
	bool GetSimpleShadowFlag() const { return m_bSimple; }

	float GetViewRadius() const { return m_fViewRadius; }
	float GetViewportSize() const { return m_fViewportSize; }
	float GetFadeStart() const { return m_fFadeStart; }

	inline int GetVertCount() { return m_nVertCount; }
	inline int GetFaceCount() { return m_nFaceCount; }

	inline void SetA3DTerrain(A3DTerrain2 * pTerrain)	{ m_pA3DTerrain = pTerrain; }
	inline void SetCECTriangleMan(CECTriangleMan* pTriMan) { m_pTriangleMan = pTriMan; }

	TERRAIN_SURFACE_CELL* FindTerrainSurfaceCell(DWORD cell)
	{
		TerrainSurfaceCellMap::iterator itCell = m_TerrSurfaceCellCacheMap.find(cell);

		if (itCell != m_TerrSurfaceCellCacheMap.end())
			return itCell->second;

		return NULL;
	}

	TERRAIN_SURFACE_CELL* AddSurfaceCell(DWORD cell)
	{
		TERRAIN_SURFACE_CELL* pCellData = m_TerrSurfaceCellCachePool.RemoveTail();
		m_TerrSurfaceCellCachePool.AddHead(pCellData);

		if (pCellData->bUsed)
			m_TerrSurfaceCellCacheMap.erase(pCellData->dwId);

		pCellData->dwId = cell;
		pCellData->bUsed = true;
		m_TerrSurfaceCellCacheMap[cell] = pCellData;
		return pCellData;
	}

	bool IsShadowVisble(const A3DAABB& aabb, float fYOffset, const A3DVECTOR3& vCenterPos, A3DCameraBase* pCamera, const A3DVECTOR3& vLightHDir);
	void AdjustShadowRadius(float fRatio);
};

#endif  //_ANGELICA3

#endif//_EC_SHADOWRENDER_H_
