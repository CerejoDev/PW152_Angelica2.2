// workspacebar.h : interface of the CWorkSpaceBar class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TreeCtrlExt.h"

struct CItemData;

class CWorkSpaceBar : public CBCGPDockingControlBar, public IObjStateReport
{
public:
	CWorkSpaceBar();

// Attributes
protected:
	static const UINT tvID;
	CTreeCtrlExt	   m_wndTree;
	CGame*			   m_pGame;
	CmdQueue*		   m_pCmdRecorder;
	CPhysXObjSelGroup* m_pObjSelGroup;

// Operations
public:
	void SetSelGroup(CPhysXObjSelGroup* pSelGroup);
	void SetCmdQueue(CmdQueue& cmdQueue);
	void SetGame(CGame& game);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkSpaceBar();

	virtual void OnNotifyBorn(const IObjBase& obj);
	virtual void OnNotifyDie(const IObjBase& obj);
	virtual void OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn);
	virtual void OnUpdateStateRedraw();

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void CreateOTTN(const RawObjMgr::ObjTypeID& nID);
	HTREEITEM GetOTTN(const RawObjMgr::ObjTypeID& nID) const;
	void TreeSelChangedProcesser(const NMTREEVIEW& nmtv);
	void UpdateContent(CItemData& itemData);

private:
	struct ObjTypeTreeNode 
	{
		ObjTypeTreeNode(const RawObjMgr::ObjTypeID& nID, const HTREEITEM& nHTI)
		{
			tid = nID;
			hti = nHTI;
		}

		RawObjMgr::ObjTypeID tid;
		HTREEITEM hti;
	};

	typedef		std::vector<ObjTypeTreeNode>	OTTN;

	enum MSGDIRECT
	{
		MD_IDLE,
		MD_TREE_TO_CONTENT,
		MD_CONTENT_TO_TREE,
	};

	bool m_EnableTree;
	MSGDIRECT m_MsgDirect;
	HTREEITEM m_hRoot;
	OTTN m_ottn;
	CRect m_rcDirty;

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

/////////////////////////////////////////////////////////////////////////////
