
#include "APhysX.h"

#include <stdio.h>
#include <conio.h>



//-------------------------------------------------------
// for core dump...
#include <windows.h>
#include <dbghelp.h>

#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI MyUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;
	
	char szFileName[128];
	SYSTEMTIME st;
	::GetLocalTime(&st);
	sprintf(szFileName, "PhysXBenchmark_%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d.dmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand()%100);
	
	HANDLE hFile = ::CreateFileA(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = false;
		
		// write the dump
		
		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
		
		if (bOK)
		{
			printf("Create Dump File Success!\n");
		}
		else
		{
			printf("MiniDumpWriteDump Failed: %d\n", GetLastError());
		}
		
		::CloseHandle(hFile);
	}
	else
	{
		printf("Create File %s Failed %d\n", szFileName, GetLastError());
	}
	return ret;
}

//-------------------------------------------------------


#define MAX_PATH_LENGTH 1024
char g_szPhysXPath[MAX_PATH_LENGTH];


void RetrievePhysXPath()
{

	GetModuleFileNameA(NULL, g_szPhysXPath, MAX_PATH_LENGTH);
	char* szFinalSlash = g_szPhysXPath, *szTmp = NULL;
	while(szTmp=strstr(szFinalSlash, "\\"))
	{
		szFinalSlash = szTmp+1;
	}

	szFinalSlash--;
	*szFinalSlash = '\0';

	strcat(g_szPhysXPath, "\\PhysX");
}


void InitPhysX()
{

	APhysXEngineDesc apxEngineDesc;
	apxEngineDesc.mEnableVRD = true;
	// Load PhysXCore.dll locally...
	apxEngineDesc.mPhysXCorePath = g_szPhysXPath;

	if(gPhysXEngine->Init(apxEngineDesc))
	{
		printf("PhysX Initialization OK!\n");
	}
	else
	{
		printf("PhysX Initialization Failed!\n");
	}
		
}

void RleasePhysX()
{
	gPhysXEngine->Release();
}



int main(int argc, char* argv[])
{

	::SetUnhandledExceptionFilter(MyUnhandledFilter);

	RetrievePhysXPath();

	printf("------ APhysX Lite Benchmark ------\n");
	printf("Local PhysX Path: %s\n", g_szPhysXPath);

	InitPhysX();

	float fHWBenchmarckFPS = APhysXUtilLib::PhysXBenchmark();
	float fSWBenchmarckFPS = APhysXUtilLib::PhysXBenchmark(false);	
	
	printf("PhysX Benchmark HW FPS: %.1f\n", fHWBenchmarckFPS);
	printf("PhysX Benchmark SW FPS: %.1f\n", fSWBenchmarckFPS);
	
	RleasePhysX();

	printf("PhysX Benchmark is done successfully. Press any key to exit! \n");
	_getch();

	return 0;

}