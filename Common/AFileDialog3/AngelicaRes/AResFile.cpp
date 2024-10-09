/*
* FILE: AResFile.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/8
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include "AResFile.h"

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

static char g_szBaseDir[MAX_PATH];

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

bool AResInitialize()
{
	GetCurrentDirectoryA(sizeof(g_szBaseDir), g_szBaseDir);
	return true;
}

bool AResIsFileExist(const char* szPath)
{
	char szFullPath[MAX_PATH];
	AResGetFullPath(szFullPath, sizeof(szFullPath), szPath);
	DWORD dwAttr = GetFileAttributesA(szFullPath);
	if (dwAttr != INVALID_FILE_ATTRIBUTES && dwAttr != FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

bool AResChangeFileExt(char* szFileName, int iBufLen, const char* szExt)
{
	char szFile[MAX_PATH];
	strcpy_s(szFile, szFileName);

	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
		strcpy(pTemp, szExt);
	else
		strcat_s(szFile, szExt);

	int iLen = static_cast<int>(strlen(szFile));
	if (iLen >= iBufLen)
	{
		assert(iLen < iBufLen);
		return false;
	}

	strcpy_s(szFileName, iBufLen, szFile);
	return true;
}

void AResGetFullPath(char* szFullPath, int iBufLen, const char* szRelPath)
{
	_snprintf_s(szFullPath, iBufLen, iBufLen, "%s\\%s", g_szBaseDir, szRelPath);
}