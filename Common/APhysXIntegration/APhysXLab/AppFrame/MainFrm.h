// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__DF3AFE0E_088D_45AE_B906_27BB2974320C__INCLUDED_)
#define AFX_MAINFRM_H__DF3AFE0E_088D_45AE_B906_27BB2974320C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAPhysXLabView;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CString m_FFbindingToMA;

// Operations
public:
	void Tick();
	bool AppIsActive() const { return m_bActive; }

	void AddToRecent(const CString& strName);
	void DelFromRecent(const CString& strName);

	bool IsCurrentID(const UINT nID) const { return 0 == (nID - m_IDBase); }
	bool GetCurrentString(CString& outPath, CString* poutTitle = 0) const { return GetRecentString(m_IDBase, outPath, poutTitle); }
	bool GetRecentString(const UINT nID, CString& outPath, CString* poutTitle = 0) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CMenu* GetRecentMenu();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

private:
	static const int RecentMenuIndex;

	const int m_IDBase;
	const int m_IDCount;
	const CString m_MenuName;
	const CString m_ConfigName;

	bool m_bActive;
	CAPhysXLabView* m_pView;
	CString m_Names[5];

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__DF3AFE0E_088D_45AE_B906_27BB2974320C__INCLUDED_)
