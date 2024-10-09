/*
* FILE: AResLmd.cpp
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
#include "AResLmd.h"

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
//	Implement AResLmd
//	
///////////////////////////////////////////////////////////////////////////

AResLmd::AResLmd(AResManager* pResMan)
: base_class(pResMan)
{
}

AResLmd::~AResLmd(void)
{
}

bool AResLmd::Init(const char* szPath)
{
	g_pResLog->Log("%s, Error! .lmd file is not a kind of resource you may mean (.umd/.umds)?", __FUNCTION__);
	return false;
	/*
	if (!base_class::Init(szPath))
		return false;

	A3DLitModel* pLitModel = new A3DLitModel();
	if (!pLitModel->Init(m_pResMan->GetA3DDevice()) || !pLitModel->Load(m_pResMan->GetA3DDevice(), szPath))
	{
		g_pResLog->Log("%s, Failed to init or load litmodel %s", __FUNCTION__, szPath);
		return false;
	}
	*/

	return true;
}

void AResLmd::Release()
{
	base_class::Release();
}

//	Files that this resource ref to
int AResLmd::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResLmd::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}