/*
 * FILE: ALogWrapper.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "ALogWrapper.h"
#include <AFI.h>

bool CALogWrapper::Init()
{
	char szCurPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurPath);
	
	if (!af_Initialize())
		return false;
	af_SetBaseDir(szCurPath);
	
	if (!m_Log.Init("APhysXLab.log", "======APhysXLab Log======"))
	{
		af_Finalize();
		return false;
	}
	
	a_RedirectDefLogOutput(_LogOutput);
	return true;
}

void CALogWrapper::Release()
{
	a_RedirectDefLogOutput(NULL);

	m_Log.Release();
	af_Finalize();
}
