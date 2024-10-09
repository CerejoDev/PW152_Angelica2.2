/*
* FILE: AResSkinModelImpl.cpp
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
#include "AResSkinModelImpl.h"
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

class AResSkinModelWrapper : public A3DSkinModel
{
public:
	AResSkinModelWrapper()
	{

	}

	~AResSkinModelWrapper()
	{
		Release();
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResSkinModelImpl
//	
///////////////////////////////////////////////////////////////////////////

AResSkinModelImpl::AResSkinModelImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
}

AResSkinModelImpl::~AResSkinModelImpl(void)
{
	Release();
}

bool AResSkinModelImpl::Init(const char* szPath)
{
	std::auto_ptr<AResSkinModelWrapper> pSkinModel(new AResSkinModelWrapper);
	if (!pSkinModel->Init(m_pResImpl->GetA3DEngine()) || !pSkinModel->Load(szPath))
		return false;
	
	return Init(pSkinModel.get());
}

bool AResSkinModelImpl::Init(A3DSkinModel* pSkinModel)
{
	assert( pSkinModel && pSkinModel->GetSkeleton() );

	m_strSkeletonFile = pSkinModel->GetSkeleton()->GetFileName();

	for (int nIdx = 0; nIdx < pSkinModel->GetSkinNum(); ++nIdx)
	{
		std::auto_ptr<AResSkinImpl> pResSkin(new AResSkinImpl(m_pResImpl));
		if (!pResSkin->Init(pSkinModel->GetA3DSkin(nIdx)))
			return false;

		m_aResSkins.push_back(pResSkin.release());
	}

	for (A3DSkinModelActionCore* pAction = pSkinModel->GetFirstAction()
		; pAction
		; pAction = pSkinModel->GetNextAction())
	{
		m_aTrackSetFiles.push_back(pAction->GetTrackSetFileName());
	}

	return true;
}

void AResSkinModelImpl::Release()
{
	for (ResSkinArray::iterator itr = m_aResSkins.begin()
		; itr != m_aResSkins.end()
		; ++itr)
	{
		A3DRELEASE(*itr);
	}

	m_aResSkins.clear();
}

const char* AResSkinModelImpl::GetSkeletonFileName() const
{
	return m_strSkeletonFile;
}

int AResSkinModelImpl::GetSkinNum() const
{
	return static_cast<int>(m_aResSkins.size());
}

const IResSkin* AResSkinModelImpl::GetSkin(int nIdx) const
{
	return m_aResSkins[nIdx];
}

int AResSkinModelImpl::GetTrackSetFileNum() const
{
	return static_cast<int>(m_aTrackSetFiles.size());
}

const char* AResSkinModelImpl::GetTrackSetFile(int nIdx) const
{
	return m_aTrackSetFiles[nIdx];
}