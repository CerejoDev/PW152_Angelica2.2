/*
* FILE: AResGfxImpl.cpp
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
#include "AResGfxImpl.h"
#include "AResInterfaceImpl.h"
#include "AResGfxElementImpl.h"

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
//	Implement AResGfxImpl
//	
///////////////////////////////////////////////////////////////////////////

AResGfxImpl::AResGfxImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
}

AResGfxImpl::~AResGfxImpl(void)
{
	Release();
}

bool AResGfxImpl::Init(const char* szPath)
{
	AString strPathRelGfx;
	af_GetRelativePathNoBase(szPath, "Gfx\\", strPathRelGfx);
	A3DGFXEx* pGfx = m_pResImpl->GetA3DGfxExMan()->LoadGfx(strPathRelGfx, m_pResImpl->GetPhysXShell());

	if (!Init(pGfx))
	{
		m_pResImpl->GetA3DGfxExMan()->CacheReleasedGfx(pGfx);
		return false;
	}

	m_pResImpl->GetA3DGfxExMan()->CacheReleasedGfx(pGfx);
	return true;
}

bool AResGfxImpl::Init(A3DGFXEx* pGfx)
{
	if (!pGfx)
		return false;

	for (int nIdx = 0; nIdx < pGfx->GetElementCount(); ++nIdx)
	{
		std::auto_ptr<AResGfxElementImpl> pResGfxEle(new AResGfxElementImpl);
		if (!pResGfxEle->Init(pGfx->GetElement(nIdx)))
			return false;

		m_aGfxEles.push_back(pResGfxEle.release());		
	}

	return true;
}

void AResGfxImpl::Release()
{
	for (ResGfxEleArray::iterator itr = m_aGfxEles.begin()
		; itr != m_aGfxEles.end()
		; ++itr)
	{
		A3DRELEASE(*itr);
	}

	m_aGfxEles.clear();
}

int AResGfxImpl::GetElementCount() const
{
	return static_cast<int>(m_aGfxEles.size());
}

IResGfxElement* AResGfxImpl::GetElement(int nIdx) const
{
	return m_aGfxEles[nIdx];
}
