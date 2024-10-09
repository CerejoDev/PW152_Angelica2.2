// PropsWindow.h : interface of the CPropsWindow class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CmdQueue;

class CMultiValueProp : public CBCGPProp
{
public:

	CMultiValueProp(const CString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

	virtual void SetValue(const _variant_t& varValue);

private:
	void SetNewValue(const _variant_t& val);

private:
	bool m_bIsMultiValue;
	VARTYPE m_OriginType;
};

class CFilePropPicker : public CBCGPFileProp
{
public:
	CFilePropPicker(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, 
		LPCTSTR lpszDefExt = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		LPCTSTR lpszFilter = NULL,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0)
	:CBCGPFileProp(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFlags, lpszFilter, lpszDescr, dwData)
	{
	}

	virtual void OnClickButton (CPoint point);
};

class MyPropItem : public PropItem
{
public:
	struct DataImp 
	{
		DataImp() { pItem = 0; dwData = 0; }
		MyPropItem* pItem;
		DWORD_PTR   dwData;
	};

public:
	MyPropItem(const std_TString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE)
	{
		m_bIsResOwner = true;
		m_myData.dwData = dwData;
		m_myData.pItem = this;
		m_propItem = new CBCGPProp(CString(strGroupName.c_str()), (DWORD_PTR)&m_myData, bIsValueList);
	}

	MyPropItem(const std_TString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL, BOOL bFilePicker = FALSE)
	{ 
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
		static ACHAR szFilter[] = _AL("Script Files (*.cs)|*.cs|All Files (*.*)|*.*||");
		m_bIsResOwner = true;
		m_myData.dwData = dwData;
		m_myData.pItem = this;
		if (bFilePicker)
			m_propItem = new CFilePropPicker(CString(strName.c_str()), TRUE, CString(varValue.bstrVal), NULL, dwFlags, szFilter, lpszDescr, (DWORD_PTR)&m_myData);
		else
			m_propItem = new CMultiValueProp(CString(strName.c_str()), varValue, lpszDescr, (DWORD_PTR)&m_myData, lpszEditMask, lpszEditTemplate, lpszValidChars);
	}
	~MyPropItem();
	void ResourceDetach() { m_bIsResOwner = false; }

	virtual void SetValue(const _variant_t& varValue) { m_propItem->SetValue(varValue); }
	virtual const _variant_t& GetValue() const { return m_propItem->GetValue(); }

	virtual BOOL AddSubItem(PropItem* pProp);
	virtual int GetSubItemsCount () const { return m_propItem->GetSubItemsCount(); }
	virtual void SetSubItemValue(int nIndex, const _variant_t& varValue) { m_propItem->GetSubItem(nIndex)->SetValue(varValue); };
	virtual const _variant_t& GetSubItemValue(int nIndex) const { return m_propItem->GetSubItem(nIndex)->GetValue(); } 

	virtual BOOL AddOption(LPCTSTR lpszOption, BOOL bInsertUnique = TRUE) { return m_propItem->AddOption(lpszOption, bInsertUnique); }
	virtual void RemoveAllOptions() { m_propItem->RemoveAllOptions(); }
	virtual int GetOptionCount() const { return m_propItem->GetOptionCount(); }

	virtual BOOL IsEnabled() const { return m_propItem->IsEnabled(); }
	virtual void Enable(BOOL bEnable = TRUE) { m_propItem->Enable(bEnable); }
	virtual void Expand (BOOL bExpand = TRUE) { m_propItem->Expand(bExpand); }

	virtual void AllowEdit(BOOL bAllow = TRUE) { m_propItem->AllowEdit(bAllow); }
	virtual DWORD_PTR GetData() const;

private:
	friend class MyPropList;

	bool m_bIsResOwner;
	DataImp	m_myData;
	CBCGPProp* m_propItem;
	APtrArray<MyPropItem*> m_subItem;
};

class MyPropList : public IPropList
{
public:
	MyPropList() { m_bIsShowCmds = false; m_pPropList = 0; }
	~MyPropList();
	void SetPropListInstance(CBCGPPropList& propList) { m_pPropList = &propList; }

	virtual int AddProperty(PropItem* pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	virtual int GetPropertyCount() const;

	virtual void RemoveAll();
	virtual BOOL RedrawWindow();
	virtual void AdjustLayout();
	virtual void EnableCommands(const std::vector<TString>& cmds); 
	virtual void DisableCommands(); 

	virtual PropItem* CreatePropItem(const std_TString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE) 
	{
		return new MyPropItem(strGroupName, dwData, bIsValueList); 
	}
	virtual PropItem* CreatePropItem(const std_TString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, BOOL bFilePicker = FALSE,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL)
	{
		return new MyPropItem(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars, bFilePicker); 
	}
	virtual void ReleasePropItem(PropItem* pItem, bool* outInList);

private:
	void AddProperties(MyPropItem* pProp);

private:
	bool m_bIsShowCmds;
	CBCGPPropList* m_pPropList;
	APtrArray<MyPropItem*> m_props;
};

class CPropsWindow : public CBCGPDockingControlBar
{
public:
	CPropsWindow();

// Attributes
protected:
	CmdQueue*		   m_pCmdRecorder;
	CPhysXObjSelGroup* m_pObjSelGroup;
	CBCGPPropList	   m_wndPropList;
	MyPropList		   m_myPropList;

	bool m_bIsEditFF;
	AString m_strFullPathName;
	AString m_strRelPathName;
	AString m_strTitleName;

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
	afx_msg LRESULT OnCommandClicked(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
