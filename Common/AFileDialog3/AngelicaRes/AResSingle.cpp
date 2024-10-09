/*
* FILE: AResSingle.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include "AResSingle.h"
#include <AFI.h>
#include <ALog.h>

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
//	Implement AResSingle
//	
///////////////////////////////////////////////////////////////////////////

AResSingle::AResSingle(AResManager* pResMan)
: ARes(pResMan)
{
}

AResSingle::~AResSingle(void)
{
}

bool AResSingle::Init(const char* szPath)
{
	m_strRes = szPath;
	return AResIsFileExist(szPath);
}

void AResSingle::Release()
{
	
}

int AResSingle::GetRefResNum() const
{
	return 0;
}

AResPtr AResSingle::GetRefRes(int nIdx) const
{
	return ARes::NullObject();
}

AResString AResSingle::GetFile() const
{
	return m_strRes;
}

bool AResSingle::GetLastModifyTime(FILETIME* pLastWriteTime) const
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA(str(m_strRes), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		g_pResLog->Log("Find %s not find", m_strRes);
		return false;
	}

	if (pLastWriteTime)
		*pLastWriteTime = FindFileData.ftLastWriteTime;

	return true;
}