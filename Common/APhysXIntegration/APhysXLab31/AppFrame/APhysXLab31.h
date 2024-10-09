// APhysXLab31.h : main header file for the APhysXLab31 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CAPhysXLab31App:
// See APhysXLab31.cpp for the implementation of this class
//

class CAPhysXLab31App : public CWinApp,
							public CBCGPWorkspace
{
public:
	CAPhysXLab31App();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CAPhysXLab31App theApp;