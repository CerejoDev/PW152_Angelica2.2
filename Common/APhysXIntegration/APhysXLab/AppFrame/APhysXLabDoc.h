// APhysXLabDoc.h : interface of the CAPhysXLabDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_APHYSXLABDOC_H__C46E8F75_B737_4FA1_890B_70F5D726A4E7__INCLUDED_)
#define AFX_APHYSXLABDOC_H__C46E8F75_B737_4FA1_890B_70F5D726A4E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAPhysXLabDoc : public CDocument
{
protected: // create from serialization only
	CAPhysXLabDoc();
	DECLARE_DYNCREATE(CAPhysXLabDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAPhysXLabDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAPhysXLabDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAPhysXLabDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APHYSXLABDOC_H__C46E8F75_B737_4FA1_890B_70F5D726A4E7__INCLUDED_)
