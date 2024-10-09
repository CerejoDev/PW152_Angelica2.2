/*
* FILE: AResAttImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/8
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResEnginePreRequireHeader.h"
#include "AResAttImpl.h"
#include "AResInterfaceImpl.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResAttImpl
//	
///////////////////////////////////////////////////////////////////////////

AResAttImpl::AResAttImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
	 
}

AResAttImpl::~AResAttImpl(void)
{
}

bool AResAttImpl::Init(const char* szPath)
{
	A3DSkillGfxComposer composer(m_pResImpl->GetSkillGfxEventMan());
	if (!composer.Load(szPath))
		return false;
	
	if (composer.m_szFlyGfx[0])
	{
		m_aRefGfxs.push_back(AString("Gfx\\") + composer.m_szFlyGfx);
	}

	if (composer.m_szHitGfx[0])
	{
		m_aRefGfxs.push_back(AString("Gfx\\") + composer.m_szHitGfx);
	}

	if (composer.m_szHitGrndGfx[0])
	{
		m_aRefGfxs.push_back(AString("Gfx\\") + composer.m_szHitGrndGfx);
	}

	return true;
}

int AResAttImpl::GetRefGfxNum() const
{
	return static_cast<int>(m_aRefGfxs.size());
}

const char* AResAttImpl::GetRefGfxPath(int nIdx) const
{
	return m_aRefGfxs[nIdx];
}