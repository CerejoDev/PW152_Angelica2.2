/*
* FILE: AResSpt.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/2
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include "AResSpt.h"

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
//	Implement AResSpt
//	
///////////////////////////////////////////////////////////////////////////

AResSpt::AResSpt(AResManager* pResMan)
: base_class(pResMan)
{
}

AResSpt::~AResSpt(void)
{
}

bool AResSpt::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	IResSpeedTree* pResSpt = m_pResMan->GetResInterface()->LoadSpt(szPath);
	if (!pResSpt)
	{
		g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, szPath);
		return false;
	}

	for (int nTexIdx = 0; nTexIdx < pResSpt->GetTexNum(); ++nTexIdx)
	{
		const char* szTexPath = pResSpt->GetTexFileName(nTexIdx);
		AResPtr pRefTex = m_pResMan->LoadRes(szTexPath, true);
		if (pRefTex != ARes::NullObject())
		{
			m_aRefRes.push_back(pRefTex);
		}
		else
		{
			g_pResLog->Log("%s, Failed to load speedtree texture res %s", __FUNCTION__, szTexPath);
		}
	}

	return true;
}

void AResSpt::Release()
{
	base_class::Release();

	m_aRefRes.clear();
}

int AResSpt::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResSpt::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}