// AFileDialogDevTesterDoc.h : interface of the CAFileDialogDevTesterDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFILEDIALOGDEVTESTERDOC_H__75E7993C_6D49_41E9_B278_CF9715A55CA8__INCLUDED_)
#define AFX_AFILEDIALOGDEVTESTERDOC_H__75E7993C_6D49_41E9_B278_CF9715A55CA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAFileDialogDevTesterDoc : public CDocument
{
protected: // create from serialization only
	CAFileDialogDevTesterDoc();
	DECLARE_DYNCREATE(CAFileDialogDevTesterDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAFileDialogDevTesterDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAFileDialogDevTesterDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAFileDialogDevTesterDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFILEDIALOGDEVTESTERDOC_H__75E7993C_6D49_41E9_B278_CF9715A55CA8__INCLUDED_)
