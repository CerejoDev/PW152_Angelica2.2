// MainFrm.h : interface of the CMainFrame class
//



#pragma once

class CAPhysXDebugRenderTestView;
//#include "APhysXDebugRenderTestView.h"


class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	CAPhysXDebugRenderTestView* GetView(){ return (CAPhysXDebugRenderTestView*)GetActiveView(); }
	//CAPhysXDebugRenderTestView* GetView(){ return &m_wndView; }

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	//CAPhysXDebugRenderTestView m_wndView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


