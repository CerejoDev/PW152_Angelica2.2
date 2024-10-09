// AFileDialogDevTester.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AFileDialogDevTester.h"

#include "MainFrm.h"
#include "AFileDialogDevTesterDoc.h"
#include "AFileDialogDevTesterView.h"

#include "Render.h"
#include "Global.h"

#include "AFileDialogWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterApp

BEGIN_MESSAGE_MAP(CAFileDialogDevTesterApp, CWinApp)
	//{{AFX_MSG_MAP(CAFileDialogDevTesterApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterApp construction

CAFileDialogDevTesterApp::CAFileDialogDevTesterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAFileDialogDevTesterApp object

CAFileDialogDevTesterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterApp initialization

BOOL CAFileDialogDevTesterApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else


	Enable3dControlsStatic();	// Call this when linking to MFC statically


#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	if (!AFD_DllInitialize())
	{
		g_Log.Log("Init afiledialog module failed.");
		return FALSE;
	}

	AFD_DllInitialize();

	af_Initialize();
	char szCurDir[1024];
	GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);
	af_SetBaseDir(szCurDir);

	g_Log.Init("AFileDialogTester.log", "Angelica FileDialog Tester log");
	
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAFileDialogDevTesterDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAFileDialogDevTesterView));
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	if (!g_Render.Init(m_hInstance, ((CFrameWnd*)m_pMainWnd)->GetActiveView()->GetSafeHwnd()))
	{
		g_Log.Log("CPoolApp::InitInstance, Failed to create render");
		return FALSE;
	}
	
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAFileDialogDevTesterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterApp message handlers


int CAFileDialogDevTesterApp::ExitInstance() 
{
	//	Release A3DEngine
	g_Render.Release();

	//	Close app's log file
	g_Log.Release();
	
	//	Finalize AF system
	af_Finalize();

	AFD_DllFinalize();
	
	return CWinApp::ExitInstance();
}

BOOL CAFileDialogDevTesterApp::OnIdle(LONG lCount) 
{
	DWORD dwTickTime;
	DWORD tick = ::GetTickCount();

	if (m_dwLastFrame)
		dwTickTime = tick - m_dwLastFrame;
	else
	{
		m_dwLastFrame = tick;
		dwTickTime = 0;
	}

	CAFileDialogDevTesterView* pView = ((CAFileDialogDevTesterView *)((CFrameWnd*)m_pMainWnd)->GetActiveView());
	
	//	Do frame move
	pView->Tick(dwTickTime);
	pView->Render();
	
	Sleep(100);
	
	//	Let window update menu item states
	CWinApp::OnIdle(lCount);

	return TRUE;
}
