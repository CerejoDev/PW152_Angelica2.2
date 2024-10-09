#ifndef _ECWORLDRENDER_H_
#define _ECWORLDRENDER_H_

#include <AArray.h>
#include <A3DVector.h>
#include <A3DColorValue.h>
#include <A3DTypes.h>

class A3DViewport;
class A3DShadowPSSM;
class A3DShadowPSSM2;
class A3DSkinRender;
class A3DLitModelRender;
class A3DEngine;
class A3DDevice;
class A3DCameraBase;
class A3DShadowCaster;
class A3DLPPRender;
class A3DShadowSSM;
class A3DShadowTSM;
class A3DShadowMap;
class A3DShadowCube;
class A3DLPPLight;
class A3DShadowTerrain;
class CShadowAdjuster;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////
/*
class ECTreeInst;
struct BUILD_ST_VERTLIT
{
	A3DDevice*		pA3DDevice;
	ECTreeInst*		pTreeInst;
	const char*		pTempWorkingDir;
	A3DVECTOR3		vMainLightPos;
	A3DCOLORVALUE	crLight;
	A3DCOLORVALUE	crAmbient;
	AArray<A3DLIGHTPARAM>	aLightParam;
};

void BuildSpeedTreeVertLit(const BUILD_ST_VERTLIT& pBuildContent);
*/
///////////////////////////////////////////////////////////////////////////
//	
//	Class ECWorldRender
//	
///////////////////////////////////////////////////////////////////////////

class ECWorldRender
{
public:		//	Types

	friend class CGame;

public:		//	Constructor and Destructor

	ECWorldRender(CRender& render);
	virtual ~ECWorldRender();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Render routine
	bool Render(A3DViewport* pViewport, A3DLPPLight* pLppLight);
	//	Create PSSM callback
	void CreateShadowMapCallback(A3DShadowMap* pShadowMap, A3DViewport* pViewport, int iSlice);
	//	Render reflection
	bool RenderReflect(A3DViewport* pViewport);
	//	Render refraction
	bool RenderRefract(A3DViewport* pViewport);

	//	Build shadow map for light
	A3DShadowMap* BuildShadowMap(A3DLPPLight* pLight);
	//	Set/Get main shadow's type
	void SetMainShadow(int type);
	A3DShadowMap* GetMainShadow() const { return m_pMainShadow; }

	//	Get LPP render
	A3DLPPRender* GetLPPRender() { return m_pLPPRender; }
	//	Get shadow map interfaces
	A3DShadowPSSM* GetPSSM() { return m_pPSSM; }
	A3DShadowPSSM2* GetPSSM2() { return m_pPSSM2; }
	A3DShadowSSM* GetSSM() { return m_pSSM; }
	A3DShadowTSM* GetTSM() { return m_pTSM; }
	A3DShadowCube* GetCubeSM() { return m_pCubeSM; }

	//	Get shadow adjuster
	CShadowAdjuster* GetShadowAdjuster() { return m_pShadowAdj; }

	void OnTerrainInit(A3DTerrain2& trn);
	void OnTerrainRelease();

	void AddLight(A3DLPPLight& light);
	void RemoveLight(A3DLPPLight* pLight);

protected:	//	Attributes

	CRender& m_Render;

	A3DEngine*			m_pA3DEngine;		//	A3DEngine object
	A3DDevice*			m_pA3DDevice;		//	A3DDevice object
	A3DShadowPSSM*		m_pPSSM;			//	Parallel Split Shadow Map
	A3DShadowPSSM2*		m_pPSSM2;			//	Parallel Split Shadow Map
	A3DShadowSSM*		m_pSSM;				//	Standard Shadow Map
	A3DShadowTSM*		m_pTSM;				//	Trapezoid Shadow Map
	A3DShadowCube*		m_pCubeSM;			//	Cube shadow map
	A3DShadowTerrain*	m_pTrnSM;			//	Terrain shadow map
	A3DLPPRender*		m_pLPPRender;		//	LPP render
	A3DSkinRender*		m_psrShadow;		//	Temporary skin render for shadow
	A3DLitModelRender*	m_plmrShadow;		//	Temporary litmodel render for shadow
	A3DSkinRender*		m_psrLPP;			//	Temporary skin render for LPP
	A3DLitModelRender*	m_plmrLPP;			//	Temporary litmodel render for LPP

	A3DShadowMap*		m_pMainShadow;		//	Current main shadow
	CShadowAdjuster*	m_pShadowAdj;		//	Shadow adjuster

	APtrArray<A3DShadowCaster*>	m_aShadowCasters;	//	Shadow casters
	APtrArray<A3DLPPLight*>	m_aLPPLights;

protected:	//	Operations

	//	LPP render routine
	bool LPPRender(A3DViewport* pViewport);
	//	Build global dynamic shadow map
	A3DShadowMap* BuildGlobalShadowMap(A3DCameraBase* pEyeCamera, A3DLPPLight* pLight);
	//	Build shadow map for lights
	A3DShadowMap* BuildLightShadowMap(A3DLPPLight* pLight);
	//	Build G-Bbuffer
	bool BuildGBuffer(A3DViewport* pViewport);
	//	LPP forward rendering
	bool LPPForwardRender(A3DViewport* pViewport);
	//	Render all other things by normal forward rendering
	bool RenderOthers(A3DViewport* pViewport);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECWORLDRENDER_H_
