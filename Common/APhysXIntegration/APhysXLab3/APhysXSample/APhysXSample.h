// APhysXSample.h : main header file for the APhysXSample application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAPhysXSampleApp:
// See APhysXSample.cpp for the implementation of this class
//

class CAPhysXSampleApp : public CWinApp
{
public:
	CAPhysXSampleApp();
	~CAPhysXSampleApp()
	{

	}


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
};

extern CAPhysXSampleApp theApp;