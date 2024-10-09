/*
 * FILE: ECMiniDump.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECMINIDUMP_H_
#define _ECMINIDUMP_H_

#include <ACPlatform.h>
#include <DbgHelp.h>
#include <AAssist.h>


class _AEXP_DECL ECMiniDump
{

private:
	typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

	//	A couple of private smart types
	struct CSmartModule
	{
		HMODULE m_hModule;

		CSmartModule(HMODULE h) { m_hModule = h; }
		~CSmartModule() { if (m_hModule) ::FreeLibrary(m_hModule); }
	};

	struct CSmartHandle
	{
		HANDLE m_hHandle;

		CSmartHandle(HANDLE h) { m_hHandle = h; }
		~CSmartHandle() { if (m_hHandle && m_hHandle != INVALID_HANDLE_VALUE) ::CloseHandle(m_hHandle); }
	};
	
protected:

	ECMiniDump() 
	{
		m_bExceptionOccured = false;
		m_pCSException = new CRITICAL_SECTION;

		InitializeCriticalSection(m_pCSException);
		
	};
	~ECMiniDump() 
	{
		DeleteCriticalSection(m_pCSException);
		delete m_pCSException;
	}

public:

	static bool CreateInstance(int buildNum)
	{
		if (!m_pMiniDumpInst)
		{
			m_pMiniDumpInst = new ECMiniDump;
			if(!m_pMiniDumpInst) return false;

		//	m_pOldFilter = ::SetUnhandledExceptionFilter( HandleExceptionByMiniDump );
		}
		m_gameBuildNum = buildNum;

		return true;
	}
	static bool Release()
	{
		if (m_pMiniDumpInst)
		{
			delete m_pMiniDumpInst;
	//		::SetUnhandledExceptionFilter(m_pOldFilter);
		}
		return true;
	}
		
	bool GetExceptionOccur() const{ return m_bExceptionOccured;}
	static DWORD HandleExceptionByMiniDump(LPEXCEPTION_POINTERS pExceptionPointers);
protected:	
	
	static bool Create(HMODULE hModule, PEXCEPTION_POINTERS pExceptionInfo, const TCHAR* szDumpFile);
	static bool GetImpersonationToken(HANDLE* phToken);
	static bool EnablePriv(const TCHAR* pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
	static bool RestorePriv(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
	static HMODULE LoadDbgHelpDLL(HMODULE hModule);

private:
	static ECMiniDump* m_pMiniDumpInst;
	static bool  m_bExceptionOccured;		
	static CRITICAL_SECTION*	m_pCSException;
	static DWORD m_gameBuildNum;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Macros
//	
///////////////////////////////////////////////////////////////////////////

#define BEGIN_MINIDUMP(buildNum) \
	ECMiniDump::CreateInstance((buildNum));\
	__try{

#define END_MINIDUMP }\
	__except(ECMiniDump::HandleExceptionByMiniDump(GetExceptionInformation())){	\
	::OutputDebugString(_AL("Exception occurred...\n"));\
	a_LogOutput(1, "Exception occurred... mini dumped!");\
	::ExitProcess(-1);}\
	ECMiniDump::Release();


#endif
