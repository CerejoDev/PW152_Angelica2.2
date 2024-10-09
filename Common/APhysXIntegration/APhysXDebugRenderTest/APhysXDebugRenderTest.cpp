// APhysXDebugRenderTest.cpp : Defines the class behaviors for the application.
//
//#define NOMINMAX

#include "stdafx.h"
#include "APhysXDebugRenderTest.h"

#include "MainFrm.h"

#include "APhysXDebugRenderTestDoc.h"
#include "APhysXDebugRenderTestView.h"


#include "Global.h"
#include "Render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPhysXDebugRenderTestApp

BEGIN_MESSAGE_MAP(CAPhysXDebugRenderTestApp, CWinApp)

#if MSVC_VER > 6

	ON_COMMAND(ID_APP_ABOUT, &CAPhysXDebugRenderTestApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)

#else

	ON_COMMAND(ID_APP_ABOUT,OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)

#endif

END_MESSAGE_MAP()


// CAPhysXDebugRenderTestApp construction

CAPhysXDebugRenderTestApp::CAPhysXDebugRenderTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAPhysXDebugRenderTestApp object

CAPhysXDebugRenderTestApp theApp;


// CAPhysXDebugRenderTestApp initialization

BOOL CAPhysXDebugRenderTestApp::InitInstance()
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
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAPhysXDebugRenderTestDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAPhysXDebugRenderTestView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	//	Do some initialization work
	if (!InitApp())
		return FALSE;

	CMainFrame* pFrame = (CMainFrame* )m_pMainWnd;

	// init angelica
	//	Create A3DEngine
	if (!g_Render.Init(m_hInstance, pFrame->GetView()->GetSafeHwnd()))
	{
		g_Log.Log("CTutorialApp::InitInstance, Failed to create render");
		return FALSE;
	}

	//	Initlaize game
	if (!pFrame->GetView()->InitGame())
	{
		g_Log.Log("CTutorialApp::InitInstance, Failed to initialize game");
		return FALSE;
	}

	return TRUE;
}



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
void CAPhysXDebugRenderTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CAPhysXDebugRenderTestApp message handlers


int CAPhysXDebugRenderTestApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	
	//	Release A3DEngine
	g_Render.Release();

	//	Close app's log file
	g_Log.Release();

	//	Finalize AF system
	af_Finalize();

	return CWinApp::ExitInstance();
}


BOOL CAPhysXDebugRenderTestApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	ASSERT(pMainFrame);

	// test code...
#if 0

	while(a_GetTime() - m_dwLastFrame < 10)
	{
		;
	}

#endif

	DWORD dwTickTime = m_dwLastFrame ? a_GetTime() - m_dwLastFrame : 0;
	m_dwLastFrame = a_GetTime();


	if (dwTickTime)
	{
		//	Clamp tick time in 80 ms, too large tick time isn't a good thing
		a_ClampRoof(dwTickTime, (DWORD)80);

		//	Do frame move
		pMainFrame->GetView()->Tick(dwTickTime);
	}

	//	Render
	pMainFrame->GetView()->Render(dwTickTime);

	// return CWinApp::OnIdle(lCount);

	return TRUE;
}

//	Do some initial work
bool CAPhysXDebugRenderTestApp::InitApp()
{
	//	Set app's path as Angelica work directory
	GetModuleFileNameA(NULL, g_szWorkDir, MAX_PATH);
	char* pTemp = strrchr(g_szWorkDir, '\\');
	*(pTemp+1) = '\0';

	//	Set Angelica work path
	af_Initialize();
	
	af_SetBaseDir(g_szWorkDir);
	// af_SetBaseDir("E:\\Developement\\Source_ Depot\\APhysXIntegration\\bin");

	//	Open log file
	g_Log.Init("Tutorial.log", "Angelica tutorial log");

	return true;
}
int CAPhysXDebugRenderTestApp::Run()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::Run();
/*
	MSG	msg;
	DWORD dwLastTick = a_GetTime();
	CMainFrame *pMainFrame = (CMainFrame *)m_pMainWnd;
	CAPhysXDebugRenderTestView *pView = (CAPhysXDebugRenderTestView *)pMainFrame->GetView();
	HACCEL hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

	while (1)
	{
		//	Message tick which will prevent the application hanging up;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0); 
			if( msg.message == WM_QUIT ||
				msg.message == WM_CLOSE ||
				msg.message == WM_DESTROY )
			{
				goto EXIT;
			}

			TranslateAccelerator(m_pMainWnd->GetSafeHwnd(), hAccel, &msg);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD dwTime = a_GetTime();
		DWORD dwTickTime = dwTime - dwLastTick;
		dwLastTick = dwTime;

		while(dwTime - dwLastTick < 20)
		{
			dwTime = a_GetTime();
		}

		pView->Tick(dwTickTime);
		pView->Render(dwTickTime);
	}

EXIT:
	return ExitInstance();
*/

}
