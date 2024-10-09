// APhysXDebugRenderTest.h : main header file for the APhysXDebugRenderTest application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAPhysXDebugRenderTestApp:
// See APhysXDebugRenderTest.cpp for the implementation of this class
//

class CAPhysXDebugRenderTestApp : public CWinApp
{
public:
	CAPhysXDebugRenderTestApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()


protected:

	DWORD	m_dwLastFrame;

	//	Do some initial work
	bool InitApp();
public:
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
};

extern CAPhysXDebugRenderTestApp theApp;