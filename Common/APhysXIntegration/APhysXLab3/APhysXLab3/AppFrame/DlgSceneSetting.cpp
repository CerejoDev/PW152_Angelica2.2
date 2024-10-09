// DlgPhysXTrnBuilder.cpp : implementation file
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "DlgSceneSetting.h"

// CDlgSceneSetting dialog
using namespace APhysXCommonDNet;
using namespace System;

IMPLEMENT_DYNAMIC(CDlgSceneSetting, CDialog)

CDlgSceneSetting::CDlgSceneSetting(Scene* pScene, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSceneSetting::IDD, pParent)
	, m_bEnableFrmLimit(false)
	, m_nbFrame(0)
{
	m_scene = pScene;
	int tt = m_scene->GetMinTickTime();
	if (0 < tt)
	{
		m_bEnableFrmLimit = true;
		float t = 1000.0f / tt;
		m_nbFrame = int(t);
	}
}

CDlgSceneSetting::~CDlgSceneSetting()
{
}

void CDlgSceneSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCENESCRIPT, m_ctrSceneScripts);
	DDX_Control(pDX, IDC_DEFSCRIPT, m_ctrDefScript);

	BOOL val = m_bEnableFrmLimit? 1 : 0;
	DDX_Check(pDX, IDC_CKB_FRAMELIMIT, val);
	if (val)
		m_bEnableFrmLimit = true;
	else
		m_bEnableFrmLimit = false;
	DDX_Text(pDX, IDC_EDT_FRAMEMAX, m_nbFrame);
	if (!m_bEnableFrmLimit)
		m_nbFrame = 0;
}


BEGIN_MESSAGE_MAP(CDlgSceneSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSceneSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CKB_FRAMELIMIT, &CDlgSceneSetting::OnBnClickedCkbFramelimit)
END_MESSAGE_MAP()


BOOL CDlgSceneSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_bEnableFrmLimit)
		GetDlgItem(IDC_EDT_FRAMEMAX)->EnableWindow(false);
	if (m_scene->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
	{
		GetDlgItem(IDC_SCENESCRIPT)->EnableWindow(false);
		GetDlgItem(IDC_DEFSCRIPT)->EnableWindow(false);
	}

	m_ctrSceneScripts.EnableBrowseButton();
	m_ctrSceneScripts.m_bMultiFiles = true;
	m_ctrDefScript.EnableBrowseButton();

	CString str = _T("");
	array<String^>^ scripts = MScene::Instance->GetScriptNames();
	for (int i = 0; i < scripts->Length; i++)
	{
		str += CString(scripts[i]) + CString(_T(";"));
	}

	m_ctrSceneScripts.SetWindowText(str);

	m_ctrDefScript.SetWindowText(CString(MScene::Instance->DefScriptFile));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSceneSetting::OnBnClickedOk()
{
	OnOK();
	if (m_bEnableFrmLimit)
	{
		if (0 < m_nbFrame)
		{
			float t = 1000.0f / m_nbFrame;
			m_scene->SetMinTickTime(int(t));
		}
		else
		{
			m_scene->SetMinTickTime(-1);
		}
	}
	else
	{
		m_scene->SetMinTickTime(-1);
	}

	if (m_scene->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
		return;

	CString scriptNames;
	m_ctrSceneScripts.GetWindowText(scriptNames);
	scriptNames.Trim();

	System::Collections::Generic::List<String^>^ scripts = gcnew System::Collections::Generic::List<String^>();

	int start = 0;
	int nIndex = scriptNames.Find(';');
	while(nIndex != -1)
	{
		if (nIndex <= start)
			break;

		CString str = scriptNames.Mid(start, nIndex - start);
		if (!str.IsEmpty() && str != ";")
		{
			scripts->Add(gcnew String(str));
		}
		start = nIndex + 1;
		nIndex = scriptNames.Find(';', start);
	}
	if (start < scriptNames.GetLength())
	{
		CString str = scriptNames.Mid(start);
		if (!str.IsEmpty() && str != ";")
		{
			scripts->Add(gcnew String(str));
		}
	}
	MScene::Instance->SetScriptNames(scripts->ToArray());
	
	CString strDefScript;
	m_ctrDefScript.GetWindowText(strDefScript);
	
	int index = strDefScript.Find(';');
	String^ DefScript = nullptr;
	if (index != -1)
		DefScript = gcnew String(strDefScript.Left(index));
	else
		DefScript = gcnew String(strDefScript);
	
	MScene::Instance->DefScriptFile = DefScript;
}

void CDlgSceneSetting::OnBnClickedCkbFramelimit()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CKB_FRAMELIMIT))
		GetDlgItem(IDC_EDT_FRAMEMAX)->EnableWindow(true);
	else
		GetDlgItem(IDC_EDT_FRAMEMAX)->EnableWindow(false);
}
