// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "WSBTreeClassified.h"
#include "WSBTreeParentChild.h"
#include "PropsWindow.h"
#include "OutputBar.h"

class CAPhysXLab31View;

class CMenuIconMgr
{
public:
	bool InitImage(UINT uiResID);
	bool AddItemMap(UINT menuID, int imgIndex);
	HICON GetIcon(UINT menuID, bool isGray);

private:
	struct IconPair 
	{
		HICON m_normal;
		HICON m_gray;
	};
private:
	CBCGPToolBarImages	m_images;
	CBCGPToolBarImages	m_imagesGray;
	std::map<UINT, IconPair> m_idToicon;
};

class CMainFrame : public CBCGPFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	static const int RECENT_FILE_ID_BEGIN = 60000;
	static const int RECENT_FILE_ID_COUNT = 5;

// Operations
public:
	COutputBar* GetOutputBar() {return &m_wndOutput; }
	CPropsWindow* GetPropsWindow() {return &m_wndPropsWnd; }
	CWSBTreeClassified* GetWSBClassified() {return &m_wndWSBCL; }
	CWSBTreeParentChild* GetWSBParentChild() {return &m_wndWSBPC; }

	void Tick();

	void AddToRecent(const CString& strName);
	void DelFromRecent(const CString& strName);

	bool IsCurrentID(const UINT nID) const { return 0 == (nID - RECENT_FILE_ID_BEGIN); }
	bool GetCurrentString(CString& outPath, CString* poutTitle = 0) const { return GetRecentString(RECENT_FILE_ID_BEGIN, outPath, poutTitle); }
	bool GetRecentString(const UINT nID, CString& outPath, CString* poutTitle = 0) const;

private:
	CMenu* GetFileMenuGroup();
	CMenu* GetRecentMenu();
	CString GetRecentlyFileText(int index);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnDrawMenuImage (	CDC* pDC, 
		const CBCGPToolbarMenuButton* pMenuButton, 
		const CRect& rectImage);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
	CWSBTreeClassified		m_wndWSBCL;
	CWSBTreeParentChild		m_wndWSBPC;
	CPropsWindow			m_wndPropsWnd;
	COutputBar				m_wndOutput;

	CMenuIconMgr		m_miiMgr;
	CBCGPToolBarImages	m_UserImages;

private:
	CFont m_font;
	const int m_PopMenuIndex;
	const CString m_PopMenuName;
	const CString m_ConfigName;

	bool m_bActive;
	CAPhysXLab31View* m_pView;
	CString m_Names[RECENT_FILE_ID_COUNT];

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnDestroy();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	void OnToolsViewUserToolbar (UINT id);
	void OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
};


