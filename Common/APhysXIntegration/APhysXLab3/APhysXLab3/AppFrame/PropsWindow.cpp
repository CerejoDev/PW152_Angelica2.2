// PropsWindow.cpp : implementation of the CPropsWindow class
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "PropsWindow.h"
#include "DlgFFEditor.h"
#include "Command.h"

using namespace APhysXCommonDNet;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// CPropsWindow

BEGIN_MESSAGE_MAP(CPropsWindow, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CPropsWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_COMMAND_CLICKED, OnCommandClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropsWindow construction/destruction

CPropsWindow::CPropsWindow()
{
	// TODO: add one-time construction code here
	m_pCmdRecorder = 0;
	m_pObjSelGroup = 0;
}

CPropsWindow::~CPropsWindow()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPropsWindow message handlers

int CPropsWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, 0xffffffff))
	{
		TRACE0("Failed to create property list\n");
		return -1;      // fail to create
	}

	m_wndPropList.EnableHeaderCtrl (FALSE);
	m_wndPropList.EnableDesciptionArea ();
	m_wndPropList.SetVSDotNetLook ();
	m_wndPropList.SetScrollBarsStyle (CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);
	m_wndPropList.SetDescriptionRows(4);
	m_myPropList.SetPropListInstance(m_wndPropList);
	return 0;
}

void CPropsWindow::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndPropList.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropsWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndPropList.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
		::GetSysColor (COLOR_3DSHADOW));
}

void CPropsWindow::SetCmdQueue(CmdQueue& cmdQueue)
{
	m_pCmdRecorder = &cmdQueue;
}

void CPropsWindow::SetSelGroup(CPhysXObjSelGroup& selGroup)
{
	m_pObjSelGroup = &selGroup;
	m_pObjSelGroup->SetPropList(&m_myPropList);
}

void CPropsWindow::UpdateProp()
{ 
	if (0 != m_pObjSelGroup)
		m_pObjSelGroup->UpdatePropItems();
}

LRESULT CPropsWindow::OnPropertyChanged(WPARAM wParam, LPARAM lParam) 
{ 
	CBCGPProp* pProp = reinterpret_cast<CBCGPProp*>(lParam);
	if (0 == pProp)
		return 0;

	assert(0 != m_pObjSelGroup);
	if (!m_bIsEditFF)
	{
		if ((0 != m_pCmdRecorder) && !m_pObjSelGroup->IsRunTime())
			m_pCmdRecorder->SubmitBeforeExecution(CmdPropChange());
	}
	MyPropItem::DataImp* pDI = reinterpret_cast<MyPropItem::DataImp*>(pProp->GetData());
	m_pObjSelGroup->PropertyChanged(*(pDI->pItem));
	return 0;
} 

LRESULT CPropsWindow::OnCommandClicked(WPARAM wParam, LPARAM lParam)
{
	if (m_pObjSelGroup->IsOnlyFocusOn(ObjManager::OBJ_TYPEID_REGION))
	{
		MScene::Instance->CreateFFSubObject();
	}
	else if (m_pObjSelGroup->IsOnlyFocusOn(ObjManager::OBJ_TYPEID_FORCEFIELD))
	{
		bool bIsImport = true;
		if (1 == (int)lParam)
			bIsImport = false;

		CDlgFFEditor dlg(0, bIsImport);
		INT_PTR rtn = dlg.DoModal();
		if (IDOK == rtn)
			MScene::Instance->ImportAndExportFFData(bIsImport, gcnew System::String(dlg.GetFullPath()));
	}
	return 0;
}

CMultiValueProp::CMultiValueProp(const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,
								 LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars)
								 : CBCGPProp(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	m_OriginType = varValue.vt;
	m_bIsMultiValue = false;

	if (varValue == PropItem::varMultiFlag)
		m_bIsMultiValue = true;
}

void CMultiValueProp::SetValue(const _variant_t& varValue)
{
	if (m_bIsMultiValue)
	{
		if (varValue.vt == m_OriginType)
		{
			m_bIsMultiValue = false;
			SetNewValue(varValue);
		}
		return;
	}

	if ((PropItem::varMultiFlag == varValue) || (varValue.vt != m_OriginType))
	{
		m_bIsMultiValue = true;
		SetNewValue(PropItem::varMultiFlag);
		return;
	}

	m_bIsMultiValue = false;
	CBCGPProp::SetValue(varValue);
}

void CMultiValueProp::SetNewValue(const _variant_t& val)
{
	ASSERT_VALID(this);
	BOOL bInPlaceEdit = m_bInPlaceEdit;
	if (bInPlaceEdit)
		OnEndEdit();

	m_varValue = val;
	Redraw();

	if (bInPlaceEdit)
	{
		ASSERT_VALID(m_pWndList);
		m_pWndList->EditItem(this);
	}
}

void CFilePropPicker::OnClickButton(CPoint point)
{
	CFileDialog dlg(m_bOpenFileDialog, m_strDefExt, NULL, m_dwFileOpenFlags, m_strFilter);
	CString dirBase(_TAS2WC(af_GetBaseDir()));
	CString dirScript = dirBase + _T("\\ApxUserScripts");
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)dirScript;
	CString str = _T("");
	if (dlg.DoModal() == IDOK)
	{
		POSITION pos = dlg.GetStartPosition();
		while (pos)
		{
			CString path = dlg.GetNextPathName(pos);
			int index = path.ReverseFind(_T('\\'));
			if (-1 != index)
				path = path.Right(path.GetLength() - index - 1);
			if (path != _T(""))
				str += path + _T(";");
		}
		SetValue(_variant_t((LPCTSTR)str));
	}
	SetCurrentDirectory(dirBase);
}

MyPropItem::~MyPropItem()
{
	if (m_bIsResOwner)
		delete m_propItem;
	m_bIsResOwner = false;
	m_propItem = 0;
}

BOOL MyPropItem::AddSubItem(PropItem* pProp)
{
	MyPropItem* pMyItem = dynamic_cast<MyPropItem*>(pProp);
	assert(0 != pMyItem);
	BOOL bRtn = m_propItem->AddSubItem(pMyItem->m_propItem);
	if (bRtn)
	{
		pMyItem->ResourceDetach();
		m_subItem.Add(pMyItem);
	}
	return bRtn;
}

DWORD_PTR MyPropItem::GetData() const
{
	DataImp* pDI = (DataImp*)m_propItem->GetData();
	assert(0 != pDI);
	return pDI->dwData;
}

MyPropList::~MyPropList()
{
	RemoveAll();
}

int MyPropList::AddProperty(PropItem* pProp, BOOL bRedraw, BOOL bAdjustLayout)
{
	assert(0 != m_pPropList);
	MyPropItem* pMyItem = dynamic_cast<MyPropItem*>(pProp);
	assert(0 != pMyItem);

	pMyItem->ResourceDetach();
	AddProperties(pMyItem);
	return m_pPropList->AddProperty(pMyItem->m_propItem, bRedraw, bAdjustLayout);
}

int MyPropList::GetPropertyCount() const
{
	assert(0 != m_pPropList);
	return m_pPropList->GetPropertyCount();
}

void MyPropList::RemoveAll()
{
	assert(0 != m_pPropList);
	if (0 != m_pPropList->m_hWnd)
	{
		m_pPropList->RemoveAll();
		m_bIsShowCmds = false;
	}

	int nCount = m_props.GetSize();
	for (int i = 0; i < nCount; ++i)
		delete m_props[i];
	m_props.RemoveAll(false);
}

BOOL MyPropList::RedrawWindow()
{ 
	assert(0 != m_pPropList);
	if (0 != m_pPropList->m_hWnd)
		return m_pPropList->RedrawWindow(); 
	return FALSE;
}

void MyPropList::AdjustLayout()
{
	assert(0 != m_pPropList);
	m_pPropList->AdjustLayout();
}

void MyPropList::EnableCommands(const std::vector<TString>& cmds) 
{
	size_t nCount = cmds.size();
	if (0 < nCount)
	{
		assert(0 != m_pPropList);
		CStringList lstCommands;
		for (size_t i = 0; i < nCount; ++i)
			lstCommands.AddTail(cmds[i]);
		m_pPropList->SetCommands(lstCommands);
		m_bIsShowCmds = true;
	}
}

void MyPropList::DisableCommands()
{
	if (m_bIsShowCmds)
	{
		assert(0 != m_pPropList);
		m_pPropList->ClearCommands();
		m_bIsShowCmds = false;
	}
}

void MyPropList::ReleasePropItem(PropItem* pItem, bool* outInList) 
{
	MyPropItem* pMyItem = dynamic_cast<MyPropItem*>(pItem);
	assert(0 != pMyItem);
	int nSubCount = pMyItem->GetSubItemsCount();
	for (int i = 0; i < nSubCount; ++i)
		ReleasePropItem(pMyItem->m_subItem[i], outInList);

	assert(0 != m_pPropList);
	int index = m_props.Find(pMyItem);
	if (-1 != index)
		m_props.RemoveAtQuickly(index);
	BOOL bRtn = m_pPropList->DeleteProperty(pMyItem->m_propItem, 0, 0);
	if (0 != outInList)
	{
		if (bRtn)
			*outInList = true;
	}
	delete pMyItem;
}

void MyPropList::AddProperties(MyPropItem* pProp)
{
	m_props.Add(pProp);
	for (int i = 0; i < pProp->m_subItem.GetSize(); ++i)
		AddProperties(pProp->m_subItem[i]);
}
