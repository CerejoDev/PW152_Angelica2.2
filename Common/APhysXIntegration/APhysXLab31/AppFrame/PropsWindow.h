// PropsWindow.h : interface of the CPropsWindow class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPropsWindow : public CBCGPDockingControlBar
{
public:
	CPropsWindow();

// Attributes
protected:
	CmdQueue*		   m_pCmdRecorder;
	CPhysXObjSelGroup* m_pObjSelGroup;
	CBCGPPropList m_wndPropList;

// Operations
public:
	void SetSelGroup(CPhysXObjSelGroup& selGroup);
	void SetCmdQueue(CmdQueue& cmdQueue);
	void UpdateProp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropsWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropsWindow();

// Generated message map functions
protected:
	//{{AFX_MSG(CPropsWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);  
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
