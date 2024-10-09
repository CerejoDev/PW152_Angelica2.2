// AFileDialogDevTesterView.h : interface of the CAFileDialogDevTesterView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFILEDIALOGDEVTESTERVIEW_H__DC1F4D3B_78CD_4377_88C4_0821851BAD9A__INCLUDED_)
#define AFX_AFILEDIALOGDEVTESTERVIEW_H__DC1F4D3B_78CD_4377_88C4_0821851BAD9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAFileDialogDevTesterView : public CView
{
protected: // create from serialization only
	CAFileDialogDevTesterView();
	DECLARE_DYNCREATE(CAFileDialogDevTesterView)

// Attributes
public:
	CAFileDialogDevTesterDoc* GetDocument();

// Operations
public:

	void Tick(DWORD dwTick);
	void Render();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAFileDialogDevTesterView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAFileDialogDevTesterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAFileDialogDevTesterView)
	afx_msg void OnMenuitem32771();
	afx_msg void OnMenuitem32772();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On32773();
	afx_msg void OnTestARes();
};

#ifndef _DEBUG  // debug version in AFileDialogDevTesterView.cpp
inline CAFileDialogDevTesterDoc* CAFileDialogDevTesterView::GetDocument()
   { return (CAFileDialogDevTesterDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFILEDIALOGDEVTESTERVIEW_H__DC1F4D3B_78CD_4377_88C4_0821851BAD9A__INCLUDED_)
