// AFileDialogDevTesterDoc.cpp : implementation of the CAFileDialogDevTesterDoc class
//

#include "stdafx.h"
#include "AFileDialogDevTester.h"

#include "AFileDialogDevTesterDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterDoc

IMPLEMENT_DYNCREATE(CAFileDialogDevTesterDoc, CDocument)

BEGIN_MESSAGE_MAP(CAFileDialogDevTesterDoc, CDocument)
	//{{AFX_MSG_MAP(CAFileDialogDevTesterDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterDoc construction/destruction

CAFileDialogDevTesterDoc::CAFileDialogDevTesterDoc()
{
	// TODO: add one-time construction code here

}

CAFileDialogDevTesterDoc::~CAFileDialogDevTesterDoc()
{
}

BOOL CAFileDialogDevTesterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterDoc serialization

void CAFileDialogDevTesterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterDoc diagnostics

#ifdef _DEBUG
void CAFileDialogDevTesterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAFileDialogDevTesterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDevTesterDoc commands
