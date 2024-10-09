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

bool CALogWrapper::Init(const AString& logFileBasicName, IOutputWnd* pOP)
{
	if (m_bIsInited)
		Release();

	char szCurPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurPath);
	
	if (!af_Initialize())
		return false;
	af_SetBaseDir(szCurPath);
	
	AString fn;
	fn.Format("%s.log", logFileBasicName);
	AString msg;
	msg.Format("======%s Log======", logFileBasicName);
	if (!m_Log.Init(fn, msg))
	{
		af_Finalize();
		return false;
	}
	
	a_RedirectDefLogOutput(_LogOutput);
	SetOutputWnd(pOP);
	m_bIsInited = true;
	return true;
}

void CALogWrapper::Release()
{
	a_RedirectDefLogOutput(NULL);

	m_Log.Release();
	af_Finalize();
	m_bIsInited = false;
}
