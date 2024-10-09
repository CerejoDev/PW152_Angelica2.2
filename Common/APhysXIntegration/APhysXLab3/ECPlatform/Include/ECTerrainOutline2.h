/*
 * FILE: ELTerrainOutline2.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Wangkuiwu, 2005/6/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef   _EL_TERRAINOULINE2_H_
#define   _EL_TERRAINOULINE2_H_

#include <A3DTypes.h>
#include <A3DTerrain2Env.h>
#include <ARect.h>
#include <A3DTerrain2.h>

class A3DEngine;
class A3DDevice;
class A3DCameraBase;
class A3DTerrain2;
class A3DStream;
class A3DViewport;
class A3DFXParam;
class A3DVertexDecl;
class A3DLPPRender;
class TerrainBlockInfo;

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTrnOutlineMtl
//	
///////////////////////////////////////////////////////////////////////////

class ECTrnOutlineMtl
{
public:		//	Types

public:		//	Constructor and Destructor

	ECTrnOutlineMtl();
	virtual ~ECTrnOutlineMtl();

public:		//	Attributes

	A3DFXParam*		paramVertColor;		//	Vertex color
	A3DFXParam*		paramDayTex;
	A3DFXParam*		paramNightTex;
	A3DFXParam*		paramDetailTex;
	A3DFXParam*		paramDNFactor;
	A3DFXParam*		paramSamDay;
	A3DFXParam*		paramSamNight;
	A3DFXParam*		paramDetailScale;	//	Detail uv scale
	A3DFXParam*		paramTexTrans;
	A3DFXParam*		paramLPPBufTex;		//	LPP light properties buffer texture
	A3DFXParam*		paramDiffuse;		//	Diffuse property

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Get FX material
	A3DFXMaterial* GetFXMaterial() { return m_pFXMtl; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;
	A3DFXMaterial*	m_pFXMtl;			//	FX material object

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTerrainOutline2
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECTerrainOutline2
{
	friend class ECOutlineTexture;
	friend class ECOutlineTextureCache;
	
public:

	enum
	{
		OUTLINE_MAX_LEVEL = 5,
	};

	ECTerrainOutline2();
	virtual ~ECTerrainOutline2();
	/*
	 *	init the outline
	 *  @param  pA3DDevice	the a3d device
	 *  @param  szTexDir	the texture dir
	 *  @param  szMap		the height map file path
	 *  @param  pTerrain2	the terrain to be stitched
	 *  @return             true if init succeed
	 *  @author kuiwu
	 */
	bool Init(A3DEngine* pA3DEngine, int iTileGridW,int iTileGridH,int iGridLen,unsigned int nWTileCount, unsigned int nHTileCount);
	bool Load(const char * szTileDir, A3DTerrain2* pTerrain2,const char* szDetaileTexFile=NULL,float cx = 0.0f,float cz = 0.0f);
	void Release();

	void Render2(A3DViewport * pViewport);
	//	Render terrain onto G-buffer
	bool RenderOntoGBuf(A3DViewport* pViewport, A3DLPPRender* pLPPRender);
	//	LPP forward rendering
	bool RenderLPPForward(A3DViewport* pViewport, A3DLPPRender* pLPPRender);

	void SetReplaceTerrain(bool bVal);
	bool GetReplaceTerrain() const {
		return m_bReplaceTerrain;
	}

	/*
	 *
	 * @desc : set the view distance scheme
	 * @param iScheme : 0 ~ 4 , near ~ far     
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [22/9/2005]
	 * @ref:
	 */
	void SetViewScheme(int iScheme)
	{
		 a_Clamp(iScheme, 0, 4);
		 m_iCurViewScheme = iScheme;
	}
	void SetEndDist(int iLevel, float fDist){
		if ((iLevel < 0) || (iLevel > OUTLINE_MAX_LEVEL-1)) {
			return;
		}
		m_bNeedUpdate = m_bNeedUpdate || (m_fLevelEndDist[iLevel] - fDist > 1.0f) 
						|| (m_fLevelEndDist[iLevel] - fDist < -1.0f);
		if (!m_bNeedUpdate) {
			return;
		}
		m_fLevelEndDist[iLevel] = fDist;
	}

	/*
	 *	update the outline
	 *  @param pCamera  the 3d camera
	 *  @param vecCenter the  center pos of the terrain
	 *  @param bReplaceTerrain   indicates if the outline replaces the terrain
	 *  @author kuiwu
	 */
	void Update(const A3DVECTOR3& vecCenter);

	int GetTileWidth() { return m_OutlineTileWidth; }
	int GetTileHeight() { return m_OutlineTileHeight; }
	int GetGridLen() { return m_OutlineTileGridLen; }

	//	Get material
	ECTrnOutlineMtl* GetMaterialInst() { return m_pMtlInst; }
	//	Set/Get detail texture uv scale
	void SetDetailUVScale(float fScale) { m_fDetailScale = fScale; }
	float GetDetailUVScale() const { return m_fDetailScale; }

	float GetOffsetX() const {return m_fWorldOffsetX;}
	float GetOffsetZ() const {return m_fWorldOffsetZ;}

	int GetGridCountPerBlk() const { return m_iGridCountPerBlk;}

	A3DTerrain2* GetTerrain2() const {return m_pTerrain2;}

private:

	A3DEngine*			m_pA3DEngine;
	A3DDevice*			m_pA3DDevice;		// device pointer
	A3DTerrain2*		m_pTerrain2;        // the terrain
	ECTrnOutlineMtl*	m_pMtlInst;			//	Material instance	

	int             m_nWorldWidth;          // width in logic unit (meters)
	int             m_nWorldHeight;         // height in logic unit (meters)
	float			m_x;					// coordinates of left top corner in world space
	float			m_z;					// coordinates of left top corner in world space
	float			m_fDetailScale;			// Detail texture uv scale

	A3DVertexDecl*	m_pvdOLVerts;			//	Outline vertex declaration
	
	ARectI          m_BlockVisibleRect;   //active block visible rect;

	float           m_OutlineTexSize[OUTLINE_MAX_LEVEL*2];  //texture occupied size in world space, w and h
	
	ECOutlineTextureCache * m_pOutlineTexCache;
 
	DWORD             m_dwFrame;
	A3DVECTOR3        m_vCameraDir;
	AString           m_szTexDir;
	
	TerrainBlockInfo* m_pTerrain2BlkInfo;
	
	int               m_iMinLevel;	
	bool              m_bReplaceTerrain;	
	A3DTexture*		  m_pDetailTexture;	
	float			  m_fLevelEndDist[OUTLINE_MAX_LEVEL];

	bool              m_bNeedUpdate;
	
	AString           m_szTileDir;
	AString			  m_strDetailTexturePath; // texture for detail pass.

	int               m_nExpectCacheSize[5];  
	int               m_iCurViewScheme;

	int				  m_OutlineTileWidth;
	int				  m_OutlineTileHeight;
	int				  m_OutlineTileGridLen;

	float			  m_fWorldOffsetX;
	float			  m_fWorldOffsetZ;

	int				  m_iGridCountPerBlk;

private:
	
	inline int  _GetSign(float a){
		return (a >0)? (+1):(-1);
	}

	void _CheckBlkInfo(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, bool& bBlkChange);
	
	void _RenderReplaceTerrain(A3DViewport* pViewport, bool bDoCull);

	void UploadDNFactor();
};

#endif 
