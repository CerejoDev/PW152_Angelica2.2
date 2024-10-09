/*
* FILE: AResAtt.cpp
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
#include "AResPreRequireHeader.h"
#include "AResAtt.h"

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
//	Implement AResAtt
//	
///////////////////////////////////////////////////////////////////////////

AResAtt::AResAtt(AResManager* pResMan)
: base_class(pResMan)
{
}

AResAtt::~AResAtt(void)
{
}

bool AResAtt::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	IResAtt* pResAtt = m_pResMan->GetResInterface()->LoadAtt(szPath);
	AResWrapper _ResWrapper(pResAtt, m_pResMan->GetResInterface());
	if (!pResAtt)
	{
		g_pResLog->Log("%s, Failed to load Att file %s", __FUNCTION__, szPath);
		return false;
	}

	for (int nIdx = 0; nIdx < pResAtt->GetRefGfxNum(); ++nIdx)
	{
		AResPtr pRefGfx = m_pResMan->LoadRes(pResAtt->GetRefGfxPath(nIdx), true);
		if (pRefGfx == ARes::NullObject())
			continue;

		m_aRefRes.push_back(pRefGfx);
	}

	return true;
}

void AResAtt::Release()
{
	m_aRefRes.clear();
}

int AResAtt::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResAtt::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}
