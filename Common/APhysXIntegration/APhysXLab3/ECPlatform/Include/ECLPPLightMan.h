/*
 * FILE: ECLPPLightMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2010/6/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECLPPLIGHTMAN_H_
#define _ECLPPLIGHTMAN_H_

#include <AArray.h>
#include <hashtab.h>
#include <A3DTypes.h>

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

class A3DLPPLight;
class A3DLPPLightDir;
class A3DLPPLightPoint;
class A3DLPPLightSpot;
class A3DLPPRender;
class A3DLPPLightMan;
class ECScnLight;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECLPPLightMan
//	
///////////////////////////////////////////////////////////////////////////

class ECLPPLightMan
{
public:		//	Types

	//	Lights laid out in scene
	struct SCNLIGHTENTITY
	{
		A3DLPPLight*	pLight;
		ECScnLight*		pScnLight;		//	Scene light object
		bool			bDynamic;		//	true, dynamic light
	};

	typedef abase::hashtab<SCNLIGHTENTITY*, DWORD, abase::_hash_function> SceneLightTable;

	friend class ECShadowRender;
		
public:		//	Constructor and Destructor

	ECLPPLightMan();
	virtual ~ECLPPLightMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DLPPRender* pLPPRender);
	//	Release object
	void Release();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Enable/Disable sun
	void EnableSun(bool bEnable);
	//	Is sun enabled ?
	bool IsSunEnabled() const { return m_bEnableSun; }

	//	Add scene light
	void AddSceneLight(DWORD id, ECScnLight* pScnLight);
	//	Remove scene light
	void RemoveSceneLight(DWORD id);
	//	Remove all scene lights
	void RemoveAllSceneLights();
	//	Add gfx light
	void AddGfxLight(const A3DLIGHTPARAM& lp);
	//	Remove all gfx lights
	void RemoveAllGfxLights(bool bRelease=false);

protected:	//	Attributes

	A3DLPPRender*		m_pA3DLPPRender;
	A3DLPPLightMan*		m_pA3DLightMan;
	A3DLPPLightDir*		m_pSunLight;		//	The directional light represents sun
	bool				m_bEnableSun;		//	true, enable sun
	int					m_iNumGfxPL;		//	Current number of gfx point lights
	int					m_iNumGfxSL;		//	Current number of gfx spotlights

	SceneLightTable					m_SceneLightTab;	//	Scene light table
	APtrArray<A3DLPPLightPoint*>	m_aGfxPLs;			//	Gfx point lights
	APtrArray<A3DLPPLightSpot*>		m_aGfxSLs;			//	Gfx spotlights

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECLPPLIGHTMAN_H_
