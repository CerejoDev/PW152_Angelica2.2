// workspacebar.h : interface of the CWorkSpaceBar class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TreeCtrlExt.h"

class CEditor;
class CmdQueue;
struct CItemData;

class CWorkSpaceBar : public CBCGPDockingControlBar, public IObjStateReport
{
public:
	CWorkSpaceBar();

// Attributes
protected:
#ifdef ENABLE_MULLITHREAD_TEST
	static APhysXMutex m_Locker;
#endif

	static const UINT tvID;
	HTREEITEM		   m_hRoot;
	CTreeCtrlExt	   m_wndTree;
	CEditor*		   m_pGame;
	CmdQueue*		   m_pCmdRecorder;
	CPhysXObjSelGroup* m_pObjSelGroup;
	bool			   m_IsEventActive;

// Operations
public:
	void SetSelGroup(CPhysXObjSelGroup* pSelGroup);
	void SetCmdQueue(CmdQueue& cmdQueue);
	void SetGame(CEditor& game);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkSpaceBar();

	virtual void OnNotifyBorn(const IObjBase& obj);
	virtual void OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn);
	virtual void OnNotifyRename(const IObjBase& obj);
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

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	HTREEITEM GetHTIData(IPropObjBase& prop, const CWorkSpaceBar& key);
	typedef		std::vector<HTREEITEM>	arrHTI;
	HTREEITEM RemoveAllHTIData(const IObjBase& obj, arrHTI* pOutDelHTIs = 0);

	void AddItemItself(const IObjBase& obj, HTREEITEM htiParent, bool isFocus);
	void DelTreeItems(HTREEITEM hti); 

private:
	struct HTIInfo
	{
		HTIInfo() { pKey = 0; hti = 0; pNext = 0; }
		const CWorkSpaceBar* pKey;
		HTREEITEM	hti;
		HTIInfo*	pNext;
	};

private:
	virtual void SetupTreeContent() = 0;
	virtual HTREEITEM GetParentItem(const IObjBase& obj) = 0;

private:
	bool TreeSelChangedProcesser(const NMTREEVIEW& nmtv);
	bool UpdateContent(CItemData& itemData);

	void RemoveHTIData(IPropObjBase& prop, const CWorkSpaceBar& key);
	void AddHTIData(IPropObjBase& prop, const CWorkSpaceBar& key, HTREEITEM value);

private:
	enum MSGDIRECT
	{
		MD_IDLE,
		MD_TREE_TO_CONTENT,
		MD_CONTENT_TO_TREE,
	};

	bool m_EnableTree;
	MSGDIRECT m_MsgDirect;
	CRect	  m_rcDirty;

};

/////////////////////////////////////////////////////////////////////////////
