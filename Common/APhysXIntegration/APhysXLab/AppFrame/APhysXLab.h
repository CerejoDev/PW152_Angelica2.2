// APhysXLab.h : main header file for the APHYSXLAB application
//

#if !defined(AFX_APHYSXLAB_H__9D55385A_8D81_434E_B764_6266896DF599__INCLUDED_)
#define AFX_APHYSXLAB_H__9D55385A_8D81_434E_B764_6266896DF599__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAPhysXLabApp:
// See APhysXLab.cpp for the implementation of this class
//

class CAPhysXLabApp : public CWinApp
{
public:
	CAPhysXLabApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAPhysXLabApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CAPhysXLabApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APHYSXLAB_H__9D55385A_8D81_434E_B764_6266896DF599__INCLUDED_)
