// ChildView.h : interface of the CChildView class
//


#pragma once
#include <vcclr.h>


// CChildView window

class CChildView : public CWnd
{
	// Construction
public:
	CChildView();

	// Attributes
public:
	//Scene m_scene;
	gcroot<APhysXCommonDNet::MCameraController^> m_pMCamCtrl;
	gcroot<APhysXCommonDNet::MScene^> m_MScene; 

	DWORD m_dwRenderStart;
	DWORD m_dwLastRender;
	bool  m_bInit;
	CPoint m_LastPt;

	// Operations
public:
	bool Init();
	bool Tick();
	bool Render();
	static void RenderExtra(APhysXCommonDNet::MRender^ render);

	void FileLoad(CString filename);
	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLoad();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

