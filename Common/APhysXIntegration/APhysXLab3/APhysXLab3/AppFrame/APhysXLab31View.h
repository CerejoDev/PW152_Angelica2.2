// APhysXLab31View.h : interface of the CAPhysXLab31View class
//
#pragma once

#include <vcclr.h>

#include "Game.h"
#include "Render.h"

class CAPhysXLab31Doc;

class CAPhysXLab31View : public CView, public IUserInputEvent
{
protected: // create from serialization only
	CAPhysXLab31View();
	DECLARE_DYNCREATE(CAPhysXLab31View)

// Attributes
public:
	CAPhysXLab31Doc* GetDocument() const;
	bool EnableAddObject() const;

// Operations
public:
	bool Tick();
	bool Render();

private:
	typedef ObjManager::ObjTypeID		ObjID;
	static const CString NoName;
	friend static void _renderExtraCallBack(APhysXCommonDNet::MRender^ pRender);

	void DrawPrompts(APhysXCommonDNet::MRender^ pRender);
	void ObjectOpenOrSaveAs(const bool bIsOpen, CString& outPath);
	void SceneOpenOrSaveAs(const bool bIsOpen);
	void SceneOpen(const CString& path, const CString& title);

	void ChangeNextDragOperation();
	void ChangeNextDriveMode();

	LPCTSTR GetDragOptText() const;
	LPCTSTR GetDrvModeText() const;
	LPCTSTR GetShootModeText() const;
	LPCTSTR GetShootTypeText() const;

	void OnAddOperation(const ObjID objType);
	void OnKeyDownModeEdit(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyDownModeSimulate(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyDownModePlay(UINT nChar, UINT nRepCnt, UINT nFlags);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CAPhysXLab31View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void EntryPlayMode();
	virtual void eventOnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
	virtual void eventOnKeyUp(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
	virtual void eventOnLButtonDown(int x, int y, unsigned int nFlags);
	virtual void eventOnLButtonUp(int x, int y, unsigned int nFlags);
	virtual void eventOnMButtonDown(int x, int y, unsigned int nFlags);
	virtual void eventOnMButtonUp(int x, int y, unsigned int nFlags);
	virtual void eventOnRButtonDblClk(int x, int y, unsigned int nFlags);
	virtual void eventOnRButtonDown(int x, int y, unsigned int nFlags);
	virtual void eventOnRButtonUp(int x, int y, unsigned int nFlags);
	virtual void eventOnMouseMove(int x, int y, unsigned int nFlags);
	virtual void eventOnMouseWheel(int x, int y, int zDelta, unsigned int nFlags);

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()

private:
	enum StateFlag
	{
		STATE_IS_ACCUMULATING	= (1<<0),
		STATE_IS_ROTATION		= (1<<1),
		STATE_IS_MOVEMENT		= (1<<2),
		STATE_IS_SCALE			= (1<<3),
		STATE_IS_DRAGGING		= (1<<4),
		STATE_IS_RBCLICK		= (1<<5),
		STATE_IS_RBDBLCLK		= (1<<6),
	};

private:
	bool m_HasCameraTreeItem;
	SIZE m_ViewSize;
	CFlagMgr<StateFlag> m_status;
	DWORD m_dwRenderStart;	//	Render starting time
	DWORD m_dwLastRender;	//	Render time of last frame
	CPoint m_LastPT;
	CPoint m_LastReproPT;

	gcroot<APhysXCommonDNet::MEditor^> m_MEditor; 
	CEditor* m_pEditor;
	CCameraController*	m_pCamCtrl;
	CPhysXObjSelGroup*  m_pSelGroup;
	CRecording* m_pRecording;

	struct ObjItem
	{
		ObjItem() : nTypeID(0) {}

		int nTypeID;
		AString strPathName;
		CRegion::ShapeType rgnST;
	};
	ObjItem m_LastAddedItem;

public:
	virtual void OnInitialUpdate();
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
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnFileReplay();
	afx_msg void OnUpdateFileReplay(CCmdUI *pCmdUI);
	afx_msg void OnFileRecent(UINT nID);
	afx_msg void OnUpdateFileRecent(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditAddterrain();
	afx_msg void OnEditPhysxtrngen();
	afx_msg void OnEditTscenegen();
	afx_msg void OnEditSaveobj();
	afx_msg void OnUpdateEditSaveobj(CCmdUI *pCmdUI);
	afx_msg void OnEditLoadobj();
	afx_msg void OnUpdateEditLoadobj(CCmdUI *pCmdUI);
	afx_msg void OnEditAddstaticobj();
	afx_msg void OnUpdateEditAddstaticobj(CCmdUI *pCmdUI);
	afx_msg void OnEditAddsmd();
	afx_msg void OnUpdateEditAddsmd(CCmdUI *pCmdUI);
	afx_msg void OnEditAddecm();
	afx_msg void OnUpdateEditAddecm(CCmdUI *pCmdUI);
	afx_msg void OnEditAddphysxgfx();
	afx_msg void OnUpdateEditAddphysxgfx(CCmdUI *pCmdUI);
	afx_msg void OnEditAddphysxff();
	afx_msg void OnUpdateEditAddphysxff(CCmdUI *pCmdUI);
	afx_msg void OnEditAddrgnbox();
	afx_msg void OnUpdateEditAddrgnbox(CCmdUI *pCmdUI);
	afx_msg void OnEditAddrgnsphere();
	afx_msg void OnUpdateEditAddrgnsphere(CCmdUI *pCmdUI);
	afx_msg void OnEditAddrgncapsule();
	afx_msg void OnUpdateEditAddrgncapsule(CCmdUI *pCmdUI);
	afx_msg void OnEditAdddummy();
	afx_msg void OnUpdateEditAdddummy(CCmdUI *pCmdUI);
	afx_msg void OnEditAddsmsocket();
	afx_msg void OnUpdateEditAddsmsocket(CCmdUI *pCmdUI);
	afx_msg void OnEditAddsocketsm();
	afx_msg void OnUpdateEditAddsocketsm(CCmdUI *pCmdUI);
	afx_msg void OnEditAddspotlight();
	afx_msg void OnUpdateEditAddspotlight(CCmdUI *pCmdUI);
	afx_msg void OnEditAddlastobj();
	afx_msg void OnUpdateEditAddlastobj(CCmdUI *pCmdUI);
	afx_msg void OnModeEdit();
	afx_msg void OnUpdateModeEdit(CCmdUI *pCmdUI);
	afx_msg void OnModeSimulate();
	afx_msg void OnUpdateModeSimulate(CCmdUI *pCmdUI);
	afx_msg void OnModePlay();
	afx_msg void OnUpdateModePlay(CCmdUI *pCmdUI);
	afx_msg void OnModePxProfile();
	afx_msg void OnUpdateModePxProfile(CCmdUI *pCmdUI);
	afx_msg void OnModePxDebug();
	afx_msg void OnUpdateModePxDebug(CCmdUI *pCmdUI);
	afx_msg void OnModeDragMove();
	afx_msg void OnUpdateModeDragMove(CCmdUI *pCmdUI);
	afx_msg void OnModeDragRotate();
	afx_msg void OnUpdateModeDragRotate(CCmdUI *pCmdUI);
	afx_msg void OnModeDragScale();
	afx_msg void OnUpdateModeDragScale(CCmdUI *pCmdUI);
	afx_msg void OnModePhysxhw();
	afx_msg void OnUpdateModePhysxhw(CCmdUI *pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnModeDvnAnimation();
	afx_msg void OnUpdateModeDvnAnimation(CCmdUI *pCmdUI);
	afx_msg void OnModeDvnPartphysx();
	afx_msg void OnUpdateModeDvnPartphysx(CCmdUI *pCmdUI);
	afx_msg void OnModeDvnPurephysx();
	afx_msg void OnUpdateModeDvnPurephysx(CCmdUI *pCmdUI);
	afx_msg void OnModeCFollowing();
	afx_msg void OnUpdateModeCFollowing(CCmdUI *pCmdUI);
	afx_msg void OnScenesetting();
	afx_msg void OnUpdateScenesetting(CCmdUI *pCmdUI);
	afx_msg void OnModeAphysxsample();
	afx_msg void OnUpdateModeAphysxsample(CCmdUI *pCmdUI);
	afx_msg void OnViewWire();
	afx_msg void OnUpdateViewWire(CCmdUI *pCmdUI);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

#ifndef _DEBUG  // debug version in APhysXLab31View.cpp
inline CAPhysXLab31Doc* CAPhysXLab31View::GetDocument() const
   { return reinterpret_cast<CAPhysXLab31Doc*>(m_pDocument); }
#endif
