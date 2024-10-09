/*
 * FILE: ECGFXCaster.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Niuyadong, 2009/9/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

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

class A3DGFXEx;
class A3DGFXExMan;
class CECViewport;
class CECModel;
class FWTemplate;
class FWAssemblySet;
class ECGraphicsModule;
class A3DViewport;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECGFXCaster
//	
///////////////////////////////////////////////////////////////////////////

class ECGFXCaster
{
public:		//	Types

	//	Normal GFX node
	struct NORMALGFX
	{
		A3DGFXEx*	pGFX;			//	GFX object
		DWORD		dwVisTimeCnt;	//	Visible time counter
		bool		bVisible;		//	Visible flag
	};

	//	Auto GFX node
	struct AUTOGFX
	{
		A3DGFXEx*	pGFX;			//	GFX object
		DWORD		dwWaitTime;		//	Wait time
		DWORD		dwTimeCnt;		//	Life time counter
		DWORD		dwVisTimeCnt;	//	Stop time counter
		bool		bVisible;		//	Visible flag
	};

	//	FW node
	struct FIREWORKNODE
	{
		DWORD			dwTimeToFire;	//	Time to fire
		CECModel *		pFWBody;		//	The body of fire work on ground
		FWAssemblySet *	pFW;			//	The fire work pointer
		char			szFile[MAX_PATH];// file of fw
		A3DVECTOR3		vecPos;			//	Position of the fire work
	};

	struct AUTOGFXSLOT
	{
		DWORD		dwID;			//	GFX ID
		APtrArray<AUTOGFX*>	aGFXs;	//	GFX array
	};

	typedef abase::hashtab<AUTOGFXSLOT*, int, abase::_hash_function>	AutoGFXTable;
	typedef abase::hashtab<NORMALGFX*, void*, abase::_hash_function>		GFXTable;
	typedef abase::hashtab<FWTemplate*, const char *, abase::_hash_function>	FWTemplateTable;
	typedef abase::vector<FIREWORKNODE *>								FWTable;

public:		//	Constructor and Destructor

	ECGFXCaster(A3DGFXExMan* pGFXMan,ECGraphicsModule* pGraphics);
	virtual ~ECGFXCaster();

public:		//	Attributes

public:		//	Operations

	//	Reset
	bool Reset();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	void Render(A3DViewport* pViewport);

	//	Play a automatical GFX
	bool PlayAutoGFXEx(const char* szFile, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, float fScale=0.0f);

	//	Load normal gfx
	A3DGFXEx* LoadGFXEx(const char* szFile);
	//	Release normal gfx
	void ReleaseGFXEx(A3DGFXEx* pGFX, bool bCacheRelease=true);
	//	Play a normal gfx
	bool PlayGFXEx(A3DGFXEx* pGFX, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, float fScale);
	bool PlayGFXEx(A3DGFXEx* pGFX, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, float fScale);

	//	Load a fire work
	FWTemplate * LoadFWTemplate(const char * szFile);
	bool PlayFW(const char * szFile, const A3DVECTOR3& vecPos, const A3DVECTOR3& vecFaceToDir, const wchar_t* pszText);

	//	Get GFXEx Mananger
	inline A3DGFXExMan * GetGFXExMan()		{ return m_pGFXMan; }

protected:	//	Attributes

	A3DGFXExMan*		m_pGFXMan;		//	GFX manager
	AutoGFXTable		m_AutoGFXTab;	//	Auto gfx table
	GFXTable			m_GFXTab;		//	Normal gfx table
	FWTemplateTable		m_FWTemplateTab;//	Fireworks template table
	FWTable				m_FWTab;		//	Fireworks table

	ECGraphicsModule*	m_pGraphics;//	Graphics Module

protected:	//	Operations

	//	Release all gfx
	void ReleaseAllGFX();

	//	Release all fire works
	void ReleaseAllFW();

	//	Get auto GFX slot
	inline AUTOGFXSLOT* GetAutoGFXSlot(DWORD dwID);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

//	Get auto GFX slot
ECGFXCaster::AUTOGFXSLOT* ECGFXCaster::GetAutoGFXSlot(DWORD dwID)
{
	//	GFX slot with this ID exists ?
	AutoGFXTable::pair_type Pair = m_AutoGFXTab.get((int)dwID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this slot

	return *Pair.first;
}

