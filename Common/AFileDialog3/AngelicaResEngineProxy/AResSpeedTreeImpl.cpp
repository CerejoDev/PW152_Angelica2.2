/*
* FILE: AResSpeedTreeImpl.cpp
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
#include <SpeedTree4_1/ECTree.h>
#include <SpeedTree4_1/ECTreeInst.h>
#include <SpeedTree4_1/ECTreeForest.h>
#include <SpeedTree4_1/SpeedTreeRT4_1.h>
#include <A3DTexture.h>
#include "AResInterfaceImpl.h"
#include "AResSpeedTreeImpl.h"

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
//	Implement AResSpeedTreeImpl
//	
///////////////////////////////////////////////////////////////////////////

AResSpeedTreeImpl::AResSpeedTreeImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{

}

AResSpeedTreeImpl::~AResSpeedTreeImpl(void)
{
	Release();
}

bool AResSpeedTreeImpl::Init(const char* szPath)
{
	AArray<AString> aTexFiles;
	ECTree::GetSptTextureFiles(szPath, aTexFiles);

	for (int nIdx = 0; nIdx < aTexFiles.GetSize(); ++nIdx)
	{
		m_aTexFiles.push_back(aTexFiles[nIdx]);
	}

	return true;
}

void AResSpeedTreeImpl::Release()
{
	m_aTexFiles.clear();
}

int AResSpeedTreeImpl::GetTexNum() const
{
	return static_cast<int>(m_aTexFiles.size());
}

const char* AResSpeedTreeImpl::GetTexFileName(int nIdx) const
{
	return m_aTexFiles[nIdx];
}
