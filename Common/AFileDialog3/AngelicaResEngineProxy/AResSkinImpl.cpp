/*
* FILE: AResSkinImpl.cpp
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
#include "AResSkinImpl.h"
#include "AResInterfaceImpl.h"
#include "AResFxMaterialImpl.h"
#include <A3DMtlObjBase.h>

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
//	Implement AResSkinImpl
//	
///////////////////////////////////////////////////////////////////////////

AResSkinImpl::AResSkinImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
	assert( pResImpl );
}

AResSkinImpl::~AResSkinImpl(void)
{
	Release();
}

bool AResSkinImpl::Init(const char* szPath)
{
	A3DSkinMan* pSkinMan = m_pResImpl->GetA3DEngine()->GetA3DSkinMan();
	A3DSkin* pSkin = pSkinMan->LoadSkinFile(szPath);
	if (!pSkin)
		return false;

	if (!Init(pSkin))
	{
		pSkinMan->ReleaseSkin(&pSkin);
		return false;
	}

	pSkinMan->ReleaseSkin(&pSkin);
	return true;
}

bool AResSkinImpl::Init(A3DSkin* pSkin)
{
	assert( pSkin );

	m_strSkinFile = pSkin->GetFileName();

	A3DSkinMtlPack* pMtlPack = pSkin->GetMaterialPack();
	for (int nIdx = 0; nIdx < pMtlPack->GetMaterialNum(); ++nIdx)
	{
		A3DMtlObjBase* pMtlSkinBase = pMtlPack->GetMaterial(nIdx);
		std::auto_ptr<AResFxMaterialImpl> pResFxMtl(new AResFxMaterialImpl(m_pResImpl));
		if (!pResFxMtl->Init(pMtlSkinBase->GetFXMaterial()))
			return false;

		m_aResFxMtls.push_back(pResFxMtl.release());
	}

	return true;
}

void AResSkinImpl::Release()
{
	for (ResFxMaterialArray::iterator itr = m_aResFxMtls.begin()
		; itr != m_aResFxMtls.end()
		; ++itr)
	{
		A3DRELEASE(*itr);
	}

	m_aResFxMtls.clear();
}

const char* AResSkinImpl::GetFileName() const
{
	return m_strSkinFile;
}

int AResSkinImpl::GetMaterialNum() const
{
	return static_cast<int>(m_aResFxMtls.size());
}

IResFxMaterial* AResSkinImpl::GetMaterial(int nIdx) const
{
	return m_aResFxMtls[nIdx];
}