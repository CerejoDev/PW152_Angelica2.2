/*
* FILE: AResUnlitModelSetImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResEnginePreRequireHeader.h"
#include "AResUnlitModelImpl.h"
#include "AResUnlitModelSetImpl.h"
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
//	Implement AResUnlitModelSetImpl
//	
///////////////////////////////////////////////////////////////////////////

AResUnlitModelSetImpl::AResUnlitModelSetImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
	assert( pResImpl );
}

AResUnlitModelSetImpl::~AResUnlitModelSetImpl(void)
{
	Release();
}

bool AResUnlitModelSetImpl::Init(const char* szPath)
{
	std::auto_ptr<A3DUnlitModelSet> pUmds(new A3DUnlitModelSet);
	if (!pUmds->Load(m_pResImpl->GetA3DDevice(), szPath))
		return false;

	for (int nIdx = 0; nIdx < pUmds->GetNumModels(); ++nIdx)
	{
		A3DUnlitModel* pUmd = pUmds->GetModel(nIdx);
		std::auto_ptr<AResUnlitModelImpl> pResUmd(new AResUnlitModelImpl(m_pResImpl));
		if (!pResUmd->Init(pUmd))
			continue;

		m_aResUmds.push_back(pResUmd.release());
	}

	return true;
}

void AResUnlitModelSetImpl::Release()
{
	for (ResUmdArray::iterator itr = m_aResUmds.begin()
		; itr != m_aResUmds.end()
		; ++itr)
	{
		A3DRELEASE(*itr);
	}
	m_aResUmds.clear();
}

int AResUnlitModelSetImpl::GetUnlitModelNum() const
{
	return static_cast<int>(m_aResUmds.size());
}

IResUnlitModel* AResUnlitModelSetImpl::GetUnlitModel(int nIdx) const
{
	return m_aResUmds[nIdx];
}