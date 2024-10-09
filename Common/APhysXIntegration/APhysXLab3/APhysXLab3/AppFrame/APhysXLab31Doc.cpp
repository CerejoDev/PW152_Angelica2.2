// APhysXLab31Doc.cpp : implementation of the CAPhysXLab31Doc class
//

#include "stdafx.h"
#include "APhysXLab31.h"

#include "APhysXLab31Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPhysXLab31Doc

IMPLEMENT_DYNCREATE(CAPhysXLab31Doc, CDocument)

BEGIN_MESSAGE_MAP(CAPhysXLab31Doc, CDocument)
END_MESSAGE_MAP()


// CAPhysXLab31Doc construction/destruction

CAPhysXLab31Doc::CAPhysXLab31Doc()
{
	// TODO: add one-time construction code here

}

CAPhysXLab31Doc::~CAPhysXLab31Doc()
{
}

BOOL CAPhysXLab31Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CAPhysXLab31Doc serialization

void CAPhysXLab31Doc::Serialize(CArchive& ar)
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


// CAPhysXLab31Doc diagnostics

#ifdef _DEBUG
void CAPhysXLab31Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAPhysXLab31Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAPhysXLab31Doc commands
