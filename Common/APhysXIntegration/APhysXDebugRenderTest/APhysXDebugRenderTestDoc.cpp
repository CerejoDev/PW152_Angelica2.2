// APhysXDebugRenderTestDoc.cpp : implementation of the CAPhysXDebugRenderTestDoc class
//

#include "stdafx.h"
#include "APhysXDebugRenderTest.h"

#include "APhysXDebugRenderTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPhysXDebugRenderTestDoc

IMPLEMENT_DYNCREATE(CAPhysXDebugRenderTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CAPhysXDebugRenderTestDoc, CDocument)
END_MESSAGE_MAP()


// CAPhysXDebugRenderTestDoc construction/destruction

CAPhysXDebugRenderTestDoc::CAPhysXDebugRenderTestDoc()
{
	// TODO: add one-time construction code here

}

CAPhysXDebugRenderTestDoc::~CAPhysXDebugRenderTestDoc()
{
}

BOOL CAPhysXDebugRenderTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CAPhysXDebugRenderTestDoc serialization

void CAPhysXDebugRenderTestDoc::Serialize(CArchive& ar)
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


// CAPhysXDebugRenderTestDoc diagnostics

#ifdef _DEBUG
void CAPhysXDebugRenderTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAPhysXDebugRenderTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAPhysXDebugRenderTestDoc commands
