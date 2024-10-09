// AFileDialogDevTesterView.cpp : implementation of the CAFileDialogDevTesterView class
//

#include "stdafx.h"
#include "AFileDialogDevTester.h"

#include "AFileDialogDevTesterDoc.h"
#include "AFileDialogDevTesterView.h"
#include "AFileImageWrapper.h"
#include "ASoundPlayerWrapper.h"
#include "AImagePreviewWrapper.h"
#include "AResFuncs.h"

#include "Render.h"
#include <A3D.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterView

IMPLEMENT_DYNCREATE(CAFileDialogDevTesterView, CView)

BEGIN_MESSAGE_MAP(CAFileDialogDevTesterView, CView)
	//{{AFX_MSG_MAP(CAFileDialogDevTesterView)
	ON_COMMAND(ID_MENUITEM32771, OnMenuitem32771)
	ON_COMMAND(ID_MENUITEM32772, OnMenuitem32772)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_32773, On32773)
	ON_COMMAND(ID_32774, &CAFileDialogDevTesterView::OnTestARes)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterView construction/destruction

CAFileDialogDevTesterView::CAFileDialogDevTesterView()
{
	// TODO: add construction code here

}

CAFileDialogDevTesterView::~CAFileDialogDevTesterView()
{
}

BOOL CAFileDialogDevTesterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterView drawing

void CAFileDialogDevTesterView::OnDraw(CDC* pDC)
{
	CAFileDialogDevTesterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterView diagnostics

#ifdef _DEBUG
void CAFileDialogDevTesterView::AssertValid() const
{
	CView::AssertValid();
}

void CAFileDialogDevTesterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAFileDialogDevTesterDoc* CAFileDialogDevTesterView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAFileDialogDevTesterDoc)));
	return (CAFileDialogDevTesterDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterView message handlers

void CAFileDialogDevTesterView::Tick(DWORD dwTick)
{
	
}

void CAFileDialogDevTesterView::Render()
{
	g_Render.BeginRender(false);
	g_Render.GetA3DViewport()->ClearDevice();


	g_Render.EndRender(false);
}

#include "AFileDialogWrapper.h"
void CAFileDialogDevTesterView::OnMenuitem32771() 
{
#ifdef _PROJ_IN_ANGELICA_3
	const TCHAR * szExt = _T(".txt");
#else
	const TCHAR * szExt = _T(".dds");
#endif

	AFileDialogWrapper wrapper(g_Render.GetA3DDevice(), TRUE, _T("\\Models\\"), _T("AFile Dialog"), szExt, AFileDialogWrapper::AFD_LIST, this);
	if (wrapper.DoModal() == IDOK) {
		AfxMessageBox(wrapper.GetFullPathName());
		AfxMessageBox(wrapper.GetRelativePathName());

		int nCount = wrapper.GetSelectedCount();
		if (nCount > 1)
		{
			AFileImageWrapper img;
			for (int i = 0 ; i < nCount; ++i)
			{
				AfxMessageBox(wrapper.GetSelectedFileByIdx(i));
				img.ReadFromFile(g_Render.GetA3DDevice(), wrapper.GetFullPathName());
			}
		}
#ifdef _PROJ_IN_ANGELICA_3

		AImagePreviewWrapper::GetInstance()->SetPreviewFile(wrapper.GetFullPathName());

#endif

	}
}

void CAFileDialogDevTesterView::OnMenuitem32772() 
{
	//int s = AFileDialogWrapper::s_testExpStatic;
	//int g = g_testExpGlobal;

	AFileDialogWrapper wrapper(g_Render.GetA3DDevice(), FALSE, _T(""), _T("AFile Dialog"), _T(""), this);
	if (wrapper.DoModal() == IDOK) {
		AfxMessageBox(wrapper.GetFullPathName());

		AFileImageWrapper img;
		img.ReadFromFile(g_Render.GetA3DDevice(), wrapper.GetFullPathName());

#ifdef _PROJ_IN_ANGELICA_3
		AImagePreviewWrapper::GetInstance()->SetPreviewFile(wrapper.GetFullPathName());
#endif
	}

}

void CAFileDialogDevTesterView::On32773()
{
	ASoundPlayerWrapper* pSoundPlayer = ASoundPlayerWrapper::CreateSoundPlayer();
	pSoundPlayer->Init(g_Render.GetA3DEngine(), _T("¡¶Ð¡³ØÕò¡·.ogg"));
	pSoundPlayer->Play();

	Sleep(5000);
	ASoundPlayerWrapper::ReleaseSoundPlayer(pSoundPlayer);


}

void CAFileDialogDevTesterView::OnTestARes()
{
	// TODO: Add your command handler code here

	AFileDialogWrapper dialog(g_Render.GetA3DDevice(), TRUE, "", "get file tested", "", this);
	if (dialog.DoModal() != IDOK)
		return;

	AResHandle* pResHandle;
	if (!AResCreateHandle(dialog.GetRelativePathName(), &pResHandle))
		return;

	char szPath[MAX_PATH];
	int iFileNum = AResGetFileNum(pResHandle);
	for (int i = 0; i < iFileNum; ++i)
	{
		AResGetFilePath(pResHandle, i, szPath, sizeof(szPath));
		a_LogOutput(1, "res reference: %s", szPath);
	}

}
