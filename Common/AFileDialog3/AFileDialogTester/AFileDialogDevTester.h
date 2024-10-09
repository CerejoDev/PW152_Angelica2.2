// AFileDialogDevTester.h : main header file for the AFILEDIALOGDEVTESTER application
//

#if !defined(AFX_AFILEDIALOGDEVTESTER_H__FFD888E4_5AB5_4678_AB39_9488D98BD880__INCLUDED_)
#define AFX_AFILEDIALOGDEVTESTER_H__FFD888E4_5AB5_4678_AB39_9488D98BD880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterApp:
// See AFileDialogDevTester.cpp for the implementation of this class
//

class CAFileDialogDevTesterApp : public CWinApp
{
public:
	CAFileDialogDevTesterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAFileDialogDevTesterApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	DWORD m_dwLastFrame;

// Implementation
	//{{AFX_MSG(CAFileDialogDevTesterApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFILEDIALOGDEVTESTER_H__FFD888E4_5AB5_4678_AB39_9488D98BD880__INCLUDED_)
