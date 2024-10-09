// DlgFFEditor.cpp : implementation file
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "DlgFFEditor.h"


// CDlgFFEditor dialog

IMPLEMENT_DYNAMIC(CDlgFFEditor, CDialog)

CDlgFFEditor::CDlgFFEditor(CWnd* pParent /*=NULL*/, bool isImport)
	: CDialog(CDlgFFEditor::IDD, pParent)
{
	m_edtFileName.m_bIsOpen = isImport;
}

CDlgFFEditor::~CDlgFFEditor()
{
}

void CDlgFFEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_edtFileName);
}

BEGIN_MESSAGE_MAP(CDlgFFEditor, CDialog)
END_MESSAGE_MAP()

// CDlgFFEditor message handlers

BOOL CDlgFFEditor::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_edtFileName.EnableBrowseButton();
	if (m_edtFileName.m_bIsOpen)
		SetWindowText(_T("导入力场数据"));
	else
		SetWindowText(_T("导出力场数据"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFFEditor::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::OnOK();

	CString strName;
	m_edtFileName.GetWindowText(strName);
	TString sTN = strName;
	int right = sTN.Find(_T("."));
	if (0 < right)
		sTN = sTN.Left(right);
	sTN += _T(".xff");
	if (m_edtFileName.strFullPath.IsEmpty())
	{
		m_edtFileName.strFullPath = _TAS2WC(af_GetBaseDir());
		m_edtFileName.strFullPath += _T("\\ForceField\\");
		m_edtFileName.strFullPath += sTN;
		m_edtFileName.strRelPath = _T("\\ForceField\\");
		m_edtFileName.strRelPath += sTN;
	}
	else
	{
		int left1 = m_edtFileName.strFullPath.ReverseFind(_T('/'));
		int left2 = m_edtFileName.strFullPath.ReverseFind(_T('\\'));
		int left = (left1 > left2)? left1 : left2;
		if (0 < left)
			m_edtFileName.strFullPath = m_edtFileName.strFullPath.Left(left + 1);
		m_edtFileName.strFullPath += sTN;

		left1 = m_edtFileName.strRelPath.ReverseFind(_T('/'));
		left2 = m_edtFileName.strRelPath.ReverseFind(_T('\\'));
		left = (left1 > left2)? left1 : left2;
		if (0 < left)
			m_edtFileName.strRelPath = m_edtFileName.strRelPath.Left(left + 1);
		m_edtFileName.strRelPath += sTN;
	}
}
