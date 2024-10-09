/*
* FILE: ECTerrain.h
*
* DESCRIPTION: 
*
* CREATED BY: Yongdong Wang, 2009/9/1
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#pragma once 

class A3DViewport;
class ECPlatform;
class ECTerrainOutline2;
class A3DTerrain2;
class A3DLPPRender;

class _AEXP_DECL ECTerrain
{
public:

	enum RENDER_TYPE
	{
	//	TERN_NONE,			//	No terrain.
		RT_TERRAIN2_CT,		//	Only A3DTerrain2, combo texture mode
		RT_TERRAIN2,		//	Only A3DTerrain2, normal mode
		RT_MIX_CT,			//	A3DTerrain2 + Outline, combo texture mode
		RT_MIX,				//	A3DTerrain2 + Outline, normal mode
	};

	struct EnvMaterial 
	{
		int id;
		AString name;
		AString texPath;
		AString gfxPath;
		AString sfxPath;
	};
	
	typedef abase::hashtab<EnvMaterial*,DWORD,abase::_hash_function> EnvMaterialTable;

public:
	ECTerrain(A3DDevice* device);
	virtual ~ECTerrain();

public:

	bool Load(float fViewRadius,const char* szFile,const char* szTilePath,const char* szDetailedTex,bool bMultiThread, RENDER_TYPE rt,float cx,float cz);
	virtual void Release();
	
	//	Tick routine
	void Tick(DWORD dwDeltaTime, const A3DVECTOR3& vCenter);
	//	Render routine
	bool Render(A3DViewport* pViewport);
	//	Render for water reflection
	bool RenderForReflect(A3DViewport* pViewport);
	//	Render terrain onto G-buffer
	bool RenderOntoGBuf(A3DViewport* pViewport, A3DLPPRender* pLPPRender);
	//	LPP forward rendering
	bool RenderLPPForward(A3DViewport* pViewport, A3DLPPRender* pLPPRender);

	//	Get render mesh number
	virtual int GetRenderMeshNum();
	//	Get render mesh data
	virtual bool GetRenderMesh(int iMesh, BYTE* pVertData, int iVertStride, WORD* pIdxData, int* piVertNum, int* piIdxNum);
	//	Set / Get water height that is used when render reflection, refraction and alpha water edge
	virtual void SetWaterHeight(float fHeight);
	virtual float GetWaterHeight() const;

	void SetTerrainActRadius(float fRadius);
	void SetTerrainViewRadius(float fRadius);
	void SetOutlineRenderLevel(bool high);	
	void SetTerrainLoadCenter(const A3DVECTOR3& vCenter);
	void SetOutlineEndDist(int iLevel, float fDist);
	void SetOutlineViewScheme(int iScheme);

	void SetRenderType(RENDER_TYPE type);
	RENDER_TYPE GetRenderType() { return m_typeRender; }

	int GetOutlineTileWidth(); // grid count along the tile width
	int GetOutlineTileHeight(); // grid count along the tile height
	int GetOutlineGridLen(); // length of each grid
	float GetHeightAtPos(const A3DVECTOR3& pos,A3DVECTOR3* vNormal=NULL);
	A3DTerrain2* GetTerrain2() { return m_pA3DTerrain;}
	ECTerrainOutline2* GetTerrainOutline2() {return m_pTerrainOutline;}
	EnvMaterial* GetEnvMaterialAt(const A3DVECTOR3& pos);

	int GetTileColumnCount() { return m_uiTileCountW;}
	int GetTileRowCount() {return m_uiTileCountH;}

	//	Set terrain materials
	void SetMtlDiffuse(const A3DCOLORVALUE& vDiffuse);
	void SetMtlSpecular(const A3DCOLORVALUE& vSpecular);
	void SetMtlSpecularPower(float fPower);

	// set detail texture
	void SetDetailTexture(const char* szFile);

protected:

	bool LoadTerrain(const char* szFile,bool bMultiThread,float cx,float cz);
	bool LoadTerrainOutline(const char* szTilePath,const char* szDetailedTex,float cx,float cz);
	bool LoadEnvMaterial(const char* szFile);

	//	Check if terrain outline should be rendered
	bool GetOutlineRenderFlag();

private:

	A3DDevice* m_pA3DDevice;

	A3DTerrain2* m_pA3DTerrain;
	ECTerrainOutline2* m_pTerrainOutline;

	RENDER_TYPE m_typeRender;

	int m_iTileGridCountW;
	int m_iTileGridCountH;
	int m_iGridLen;
	unsigned int m_uiTileCountW;
	unsigned int m_uiTileCountH;

	int m_iSightScope;

	EnvMaterialTable m_envMaterialTable;
};