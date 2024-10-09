/*
 * FILE: ECShadowRender.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2010/7/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECSHADOWRENDER_H_
#define _ECSHADOWRENDER_H_

#include <A3DShadowMap.h>
#include <AArray.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class ECGraphicsModule;
class A3DShadowSSM;
class A3DShadowPSSM2;
class A3DShadowFake;
class A3DCameraBase;
class A3DTerrain2;
class A3DShadowTerrain;
class A3DSSAO;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECShadowRender
//	
///////////////////////////////////////////////////////////////////////////

class ECShadowRender
{
public:		//	Types

	//	Dynamic Scene Shadow Level
	enum
	{
		DSSL_0,			//	PSSM, 8 samples
		DSSL_1,			//	PSSM, 16 samples + terrain shadow
		DSSL_2,			//	PSSM, 16 samples + terrain shadow + SSAO
		MAX_DSSL,
	};

	//	Character Shadow Level
	enum
	{
		CSL_0 = 0,		//	No character shadow
		CSL_1,			//	Character shadow (low)
		CSL_2,			//	character shadow (mid)
		CSL_3,			//	character shadow (high)
		MAX_CSL,
	};

	//	Shadow maps generating step
	enum
	{
		STEP_IDLE = 0,
		STEP_BUILDING = 1,
		STEP_TERRAIN,
		STEP_CHARACTER,
	};

public:		//	Constructor and Destructor

	ECShadowRender();
	virtual ~ECShadowRender();

public:		//	Attributes

public:		//	Operations

	/*	Initialize object

		pGraphicsModule: graphics module
		lpfnCreateShadowCB: shadow map creating callback
		dataCB: user's callback data
	*/
	bool Init(ECGraphicsModule* pGraphicsModule, LPFNCREATESHADOWMAPCALLBACK lpfnCreateShadowCB, a_uiptr dataCB);
	//	Release object
	void Release();

	//	Set rebuild flag
	void SetRebuildFlag(bool bRebuild) { m_bRebuild = bRebuild; }
	//	Generate shadow maps, called every frame
	bool GenerateShadowMaps(A3DCameraBase* pEyeCamera, const A3DVECTOR3& vHostPos);
	//	Get current shadow generating step
	int GetGenStep() const { return m_iGenStep; }
	//	Generate SSAO, called after IBL and before LPP forward rendering
	bool GenerateSSAO();

	//	Clear all shadow casters
	void ClearSceneShadowCasters();
	void ClearCharShadowCasters();
	//	Add shadow casters
	void AddSceneShadowCaster(A3DShadowCaster* pCaster) { m_aScnCasters.Add(pCaster); }
	void AddCharShadowCaster(A3DShadowCaster* pCaster) { m_aCharCasters.Add(pCaster); }

	//	Enable dynamic scene shadow
	bool EnableDynSceneShadow(A3DTerrain2* pTerrain);
	//	Disable dynamic scene shadow
	//	bReleaseRes: this flag indicate to release sun shadow resources
	void DisableDynSceneShadow();
	//	Is dynamic scene shadow enabled ?
	bool IsDynSceneShadowEnabled() const { return m_bDynScnShadow; }
	//	Change dynamic scene shadow level
	bool ChangeDynSceneShadowLevel(int iLevel);
	//	Get dynamic scene shadow level
	int GetDynSceneShadowLevel() const { return m_iDSSLevel; }
	//	Rebind terrain object
	//	pTerrain: new terrain object. can be NULL
	void RebindTerrain(A3DTerrain2* pTerrain);
	//	Is dynamic terrain shadow enabled ?
	bool IsDynTerrainShadowEnabled() const { return IsDynTerrainShadowEnabled(m_iDSSLevel); }
	//	Is SSAO enabled ?
	bool IsSSAOEnabled() const { return IsSSAOEnabled(m_iDSSLevel); }

	//	Change character shadow level
	bool ChangeCharShadowLevel(int iLevel);
	//	Get character shadow level
	int GetCharShadowLevel() const { return m_iCSLevel; }

protected:	//	Attributes

	ECGraphicsModule*				m_pGraphicsModule;		//	Graphics module
	LPFNCREATESHADOWMAPCALLBACK		m_lpfnCreateShadow;		//	shadow map creating callback
	a_uiptr							m_dataCB;				//	user's callback data

	bool		m_bRebuild;				//	rebuild flag
	bool		m_bDynScnShadow;		//	Enable/Disable dynamic scene shadow (building + terrain)
	int			m_iDSSLevel;			//	Dyanmic Scene Shadow level
	int			m_iCSLevel;				//	Character Chadow level
	float		m_fCSDist;				//	Character Shadow distance
	int			m_iGenStep;				//	Generating step

	A3DShadowSSM*		m_pSSM;			//	SSM/TSM for high level characters' sun shadow
	A3DShadowFake*		m_pFakeSM;		//	Fake SM for low level characters' sun shadow
	A3DShadowPSSM2*		m_pPSSM2;		//	PSSM2 for buildings' sun shadow
	A3DShadowTerrain*	m_pTrnSM;		//	Terrain shadow map
	A3DTerrain2*		m_pTerrain;		//	Terrain object that bound with m_pTrnSM
	A3DSSAO*			m_pSSAO;		//	SSAO generator

	APtrArray<A3DShadowCaster*>		m_aScnCasters;		//	Scene shadow casters
	APtrArray<A3DShadowCaster*>		m_aCharCasters;		//	Character shadow casters

protected:	//	Operations

	//	Create terrain shadow object
	bool CreateTerrainShadow();
	//	Is dynamic terrain shadow enabled ?
	bool IsDynTerrainShadowEnabled(int iDSSLevel) const { return iDSSLevel >= DSSL_1; }
	//	Create SSAO
	bool CreateSSAO();
	//	Is SSAO enabled ?
	bool IsSSAOEnabled(int iDSSLevel) const { return iDSSLevel >= DSSL_2; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECSHADOWRENDER_H_
