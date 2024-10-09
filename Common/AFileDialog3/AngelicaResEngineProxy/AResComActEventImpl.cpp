/*
* FILE: AResComActEventImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResEnginePreRequireHeader.h"
#include "AResComActEventImpl.h"

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
//	Implement AResComActEventImpl
//	
///////////////////////////////////////////////////////////////////////////

AResComActEventImpl::AResComActEventImpl(EVENT_INFO* pInfo)
: m_pInfo(pInfo)
{
	assert( m_pInfo );
}

AResComActEventImpl::~AResComActEventImpl(void)
{
}

bool AResComActEventImpl::Init()
{
	if ( m_pInfo->GetType() == EVENT_TYPE_GFX
		|| m_pInfo->GetType() == EVENT_TYPE_SFX)
	{
		AString strPrefix = (m_pInfo->GetType() == EVENT_TYPE_GFX) ? "Gfx\\" : "Sfx\\";
		FX_BASE_INFO* pFxInfo = dynamic_cast<FX_BASE_INFO*>(m_pInfo);
		for (int nRandIdx = 0; nRandIdx < pFxInfo->GetFilePathCount(); ++nRandIdx)
		{
			const char* szFxFile = pFxInfo->GetFilePathByIndex(nRandIdx);
			AString strFxFilePath = strPrefix + szFxFile;
			m_aPaths.push_back(strFxFilePath);
		}
	}

	if (m_pInfo->GetType() == EVENT_TYPE_ATT_PT)
	{
		SGCAttackPointMark* pSgcInfo = dynamic_cast<SGCAttackPointMark*>(m_pInfo);
		if (!pSgcInfo)
		{
			return false;
		}

		char szPath[MAX_PATH];
		_snprintf_s(szPath, sizeof(szPath), "Gfx\\SkillAttack\\%s", pSgcInfo->GetAtkFile());
		m_aPaths.push_back(szPath);
	}

	return true;
}

int AResComActEventImpl::GetPathCount() const
{
	return static_cast<int>(m_aPaths.size());
}

const char* AResComActEventImpl::GetPath(int nIdx, char* szPath, int iBufLen) const
{
	_snprintf_s(szPath, iBufLen, iBufLen, m_aPaths[nIdx]);
	return szPath;
}