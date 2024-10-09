/*
* FILE: AResComActImpl.cpp
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
#include "AResComActImpl.h"
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
//	Implement AResComActImpl
//	
///////////////////////////////////////////////////////////////////////////

AResComActImpl::AResComActImpl(void)
: m_pComAct(NULL)
{
}

AResComActImpl::~AResComActImpl(void)
{
	Release();
}

bool AResComActImpl::Init(A3DCombinedAction* pComAct)
{
	Release();

	m_pComAct = pComAct;
	assert( m_pComAct );
	
	for (int nIdx = 0; nIdx < m_pComAct->GetEventCount(); ++nIdx)
	{
		std::auto_ptr<AResComActEventImpl> pEventImpl(new AResComActEventImpl(m_pComAct->GetEventInfo(nIdx)));
		if (!pEventImpl->Init())
			continue;

		m_aEvents.push_back(pEventImpl.release());
	}

	return true;
}

void AResComActImpl::Release()
{
	m_pComAct = NULL;
	for (ResEventArray::iterator itr = m_aEvents.begin()
		; itr != m_aEvents.end()
		; ++itr)
	{
		delete (*itr);
	}

	m_aEvents.clear();
}

int AResComActImpl::GetEventCount() const
{
	return static_cast<int>(m_aEvents.size());
}

IResComActEvent* AResComActImpl::GetEvent(int nIdx) const
{
	return m_aEvents[nIdx];
}