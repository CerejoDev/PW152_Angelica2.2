// APhysXLabView.h : interface of the CAPhysXLabView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_APHYSXLABVIEW_H__DD9B01A6_7820_468C_8417_176FC0C6DE9E__INCLUDED_)
#define AFX_APHYSXLABVIEW_H__DD9B01A6_7820_468C_8417_176FC0C6DE9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Game.h"
#include "Render.h"

class CAPhysXLabDoc;

class CAPhysXLabView : public CView
{
protected: // create from serialization only
	CAPhysXLabView();
	DECLARE_DYNCREATE(CAPhysXLabView)

// Attributes
public:
	CAPhysXLabDoc* GetDocument();

// Operations
public:
	bool Tick(const DWORD dwTime);
	bool Render();

	// CRender& GetRender() { return m_render; }

private:
	typedef CGame::ObjID ObjID;
	static const CString NoName;

	void DrawPrompts();
	void OnAddOperation(const ObjID objType);
	void SceneOpenOrSaveAs(const bool bIsOpen);
	void SceneOpen(const CString& path, const CString& title);

	bool EnableEditState() const;
	bool EnableAddObject() const;

	LPCTSTR GetDragOptText() const;
	LPCTSTR GetDrvModeText() const;
	LPCTSTR GetCurrentStateText() const;
	LPCTSTR GetShootModeText() const;
	LPCTSTR GetShootTypeText() const;

	void ChangeNextDragOperation();
	void ChangeNextDriveMode();

	void OnKeyDownModeEdit(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyDownModeSimulate(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyDownModePlay(UINT nChar, UINT nRepCnt, UINT nFlags);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAPhysXLabView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAPhysXLabView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFileChf2Chb();
	afx_msg void OnEditAddChb();
	afx_msg void OnUpdateEditAddChb(CCmdUI* pCmdUI);
	afx_msg void OnFileRecent(UINT nID);
	afx_msg void OnUpdateFileRecent(CCmdUI* pCmdUI);
	//{{AFX_MSG(CAPhysXLabView)
	afx_msg void OnUpdateDragOpt(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDrvMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCurrentState(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShootMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShootType(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFileReset();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnEditAddterrain();
	afx_msg void OnUpdateEditAddterrain(CCmdUI* pCmdUI);
	afx_msg void OnEditPhysxtrngen();
	afx_msg void OnUpdateEditPhysxtrngen(CCmdUI* pCmdUI);
	afx_msg void OnEditPhysxffmaker();
	afx_msg void OnUpdateEditPhysxffmaker(CCmdUI* pCmdUI);
	afx_msg void OnEditAddstaticobj();
	afx_msg void OnUpdateEditAddstaticobj(CCmdUI* pCmdUI);
	afx_msg void OnEditAddsmd();
	afx_msg void OnUpdateEditAddsmd(CCmdUI* pCmdUI);
	afx_msg void OnEditAddsmdEcm();
	afx_msg void OnUpdateEditAddsmdEcm(CCmdUI* pCmdUI);
	afx_msg void OnEditAddphysxgfx();
	afx_msg void OnUpdateEditAddphysxgfx(CCmdUI* pCmdUI);
	afx_msg void OnEditAddphysxff();
	afx_msg void OnUpdateEditAddphysxff(CCmdUI* pCmdUI);
	afx_msg void OnEditAddlastobj();
	afx_msg void OnUpdateEditAddlastobj(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditDragAndMove();
	afx_msg void OnUpdateEditDragAndMove(CCmdUI* pCmdUI);
	afx_msg void OnEditDragAndRotate();
	afx_msg void OnUpdateEditDragAndRotate(CCmdUI* pCmdUI);
	afx_msg void OnEditDragAndScale();
	afx_msg void OnUpdateEditDragAndScale(CCmdUI* pCmdUI);
	afx_msg void OnModeEdit();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnEditScale();
	afx_msg void OnUpdateEditScale(CCmdUI* pCmdUI);
	afx_msg void OnModeSimulate();
	afx_msg void OnUpdateModeSimulate(CCmdUI* pCmdUI);
	afx_msg void OnModePlay();
	afx_msg void OnUpdateModePlay(CCmdUI* pCmdUI);
	afx_msg void OnModePxProfile();
	afx_msg void OnUpdateModePxProfile(CCmdUI* pCmdUI);
	afx_msg void OnModePxDebug();
	afx_msg void OnUpdateModePxDebug(CCmdUI* pCmdUI);
	afx_msg void OnModePhysxhw();
	afx_msg void OnUpdateModePhysxhw(CCmdUI* pCmdUI);
	afx_msg void OnViewWire();
	afx_msg void OnUpdateViewWire(CCmdUI* pCmdUI);
	afx_msg void OnModeDvnAnimation();
	afx_msg void OnUpdateModeDvnAnimation(CCmdUI* pCmdUI);
	afx_msg void OnModeDvnPartphysx();
	afx_msg void OnUpdateModeDvnPartphysx(CCmdUI* pCmdUI);
	afx_msg void OnModeDvnPurephysx();
	afx_msg void OnUpdateModeDvnPurephysx(CCmdUI* pCmdUI);
	afx_msg void OnModeCFollowing();
	afx_msg void OnUpdateModeCFollowing(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMenuEditmod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool m_bIsAccumulating;
	bool m_bIsRotation;
	bool m_bIsMovement;
	bool m_bIsDragging;
	bool m_bIsRBClick;
	CPoint m_ptMouseLast;	//	Mouse last position
	float m_ShotScale;
	float m_ShotSpeed;

	DWORD m_dwRenderStart;	//	Render starting time
	DWORD m_dwLastRender;	//	Render time of last frame

	CRender& m_render;
	CGame& m_game;

	struct ObjItem
	{
		ObjItem() : nTypeID(0) {}

		int nTypeID;
		AString strPathName;
	};
	
	ObjItem m_LastAddedItem;
};

#ifndef _DEBUG  // debug version in APhysXLabView.cpp
inline CAPhysXLabDoc* CAPhysXLabView::GetDocument()
   { return (CAPhysXLabDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APHYSXLABVIEW_H__DD9B01A6_7820_468C_8417_176FC0C6DE9E__INCLUDED_)
