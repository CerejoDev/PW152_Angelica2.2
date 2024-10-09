// APhysXSample.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "APhysXSample.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace APhysXCommonDNet;

// CAPhysXSampleApp

BEGIN_MESSAGE_MAP(CAPhysXSampleApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAPhysXSampleApp::OnAppAbout)
END_MESSAGE_MAP()


// CAPhysXSampleApp construction

CAPhysXSampleApp::CAPhysXSampleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAPhysXSampleApp object

CAPhysXSampleApp theApp;


// CAPhysXSampleApp initialization

BOOL CAPhysXSampleApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 1665;


	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	MLogWrapper::Instance->Init(gcnew System::String("APhysXSample"), nullptr);

	pFrame->GetView()->Init();

	CString strFile = m_lpCmdLine;
	if (strFile != "")
	{
		pFrame->GetView()->FileLoad(strFile);
	}

	return TRUE;
}


// CAPhysXSampleApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CAPhysXSampleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CAPhysXSampleApp message handlers


int CAPhysXSampleApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	MLogWrapper::Instance->Release();
	
	return CWinApp::ExitInstance();
}

BOOL CAPhysXSampleApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(m_pMainWnd);
	ASSERT(pMainFrame);

	pMainFrame->Tick();
	if (0 < lCount)
		return TRUE;

	return CWinApp::OnIdle(lCount);
}
