/*
* FILE: AResFactory.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include <AFI.h>
#include "AResFactory.h"
#include "AResAfm.h"
#include "AResBsp.h"
#include "AResChf.h"
#include "AResEcm.h"
#include "AResGfx.h"
#include "AResLmd.h"
#include "AResSingle.h"
#include "AResSki.h"
#include "AResSmd.h"
#include "AResSpt.h"
#include "AResUmd.h"
#include "AResUmds.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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


struct FileExt2ResType
{
	AResString strExt;
	AResType resType;
};

static FileExt2ResType _TableExt2ResType[] = 
{
	{ (".afm"),	ARES_AFM },
	{ (".ecm3"),	ARES_ECM },
	{ (".gfx3"),	ARES_GFX },
	{ (".att3"),	ARES_ATT },
	{ (".lmd"),	ARES_LMD },
	{ (".umd"),	ARES_UMD },
	{ (".umds"),	ARES_UMDS },
	{ (".chf"),	ARES_CHF },
	{ (".bsp"),	ARES_BSP },
	{ (".smd"),	ARES_SMD },
	{ (".ski"),	ARES_SKI },
	{ (".spt"),	ARES_SPT },	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResFactory
//	
///////////////////////////////////////////////////////////////////////////

AResFactory::AResFactory(AResManager* pResMan)
: m_pResMan(pResMan)
{
}

AResFactory::~AResFactory(void)
{
}

AResType AResFactory::GetTypeFromPath(const char* szPath)
{
	const char* szExt = strrchr(szPath, '.');
	if (!szExt)
		return ARES_SINGLE;

	for (int nIdx = 0; nIdx < _countof(_TableExt2ResType); ++nIdx)
	{
		if (_stricmp(szExt, str(_TableExt2ResType[nIdx].strExt)) == 0)
			return _TableExt2ResType[nIdx].resType;
	}

	return ARES_SINGLE;
}

AResPtr AResFactory::MakeResource(const char* szPath, bool bRequireExist)
{
	AResPathInstanceMap::iterator itr = m_mapRes.find(szPath);
	if (itr != m_mapRes.end())
		return itr->second;

	AResPtr pRes = MakeResourceImpl(szPath);
	if (pRes == ARes::NullObject())
		return pRes;

	if (!pRes->Init(szPath) && !bRequireExist)
		return ARes::NullObject();

	m_mapRes[szPath] = pRes;
	return pRes;
}

AResPtr AResFactory::MakeResourceImpl(const char* szPath)
{
	AResType resType = GetTypeFromPath(szPath);
	switch (resType)
	{
	case ARES_AFM:
		return MakeSharedPtr<AResAfm>(m_pResMan);
	case ARES_ECM:
		return MakeSharedPtr<AResEcm>(m_pResMan);
	case ARES_GFX:
		return MakeSharedPtr<AResGfx>(m_pResMan);
	case ARES_ATT:
		return MakeSharedPtr<AResAtt>(m_pResMan);
	case ARES_LMD:		// .lmd
		return MakeSharedPtr<AResLmd>(m_pResMan);
	case ARES_UMD:		// .umd
		return MakeSharedPtr<AResUmd>(m_pResMan);
	case ARES_UMDS:		// .umds
		return MakeSharedPtr<AResUmds>(m_pResMan);
	case ARES_CHF:		// .chf  convex hull file
		return MakeSharedPtr<AResChf>(m_pResMan);
	case ARES_BSP:		// .bsp  binary space partition for umd / umds
		return MakeSharedPtr<AResBsp>(m_pResMan);
	case ARES_SMD:		// .SMD
		return MakeSharedPtr<AResSmd>(m_pResMan);
	case ARES_SKI:		// .ski
		return MakeSharedPtr<AResSki>(m_pResMan);
	case ARES_SPT:		// .spt  speedtree
		return MakeSharedPtr<AResSpt>(m_pResMan);
	case ARES_SINGLE:
		return MakeSharedPtr<AResSingle>(m_pResMan);
	default:
		return ARes::NullObject();
	}
}

void AResFactory::ReleaseRes(AResPtr& pRes)
{
	if (pRes == ARes::NullObject())
		return;

	AResPathInstanceMap::iterator itr = m_mapRes.find(pRes->GetFile());
	if (itr != m_mapRes.end())
	{
		m_mapRes.erase(itr);
	}
}

void AResFactory::Release()
{
	m_mapRes.clear();
}