// DlgSceneBuilder.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSceneBuilder.h"

using namespace System;
using namespace APhysXCommonDNet;

// CDlgSceneBuilder dialog

IMPLEMENT_DYNAMIC(CDlgSceneBuilder, CDialog)

CDlgSceneBuilder::CDlgSceneBuilder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSceneBuilder::IDD, pParent)
	, m_strTrnName(_T(""))
	, m_SceneName(_T(""))
{
	m_strTrnName = "x1";
}

CDlgSceneBuilder::~CDlgSceneBuilder()
{
}

void CDlgSceneBuilder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TRNNAME, m_strTrnName);
	DDX_Text(pDX, IDC_EDIT_SCENENAME, m_SceneName);
	DDX_Control(pDX, IDC_EDIT_MODELPATH, m_wndModelPath);
}


BEGIN_MESSAGE_MAP(CDlgSceneBuilder, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSceneBuilder::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSceneBuilder message handlers

void CDlgSceneBuilder::OnBnClickedOk()
{
	OnOK();

	// TODO: Add your control notification handler code here
	m_strModels.clear();
//	AString strBase = "D:\\¡¶Ð¦°Á½­ºþ¡·\\package\\models\\npcs\\npcs\\Õ½¶·npc";
//	AString strBase = "D:\\¡¶Ð¦°Á½­ºþ¡·\\package\\models\\npcs\\npcs\\ÆÆËé";
	CString strFolder;
	m_wndModelPath.GetWindowText(strFolder);
	FindECMPathName(strFolder.GetString());

	TString str;
	str.Format(_T("find n = %d models\n"), m_strModels.size());
	OutputDebugString(str);
	strList::iterator it = m_strModels.begin();
	strList::iterator itEnd = m_strModels.end();
	for (; it != itEnd; ++it)
	{
		OutputDebugString(*it);
		OutputDebugString(_T("\n"));
	}
}

void CDlgSceneBuilder::FindECMPathName(const TString& path)
{
	if (path.IsEmpty())
		return;

	WIN32_FIND_DATA fdFindData;
	TString strAll = path;
	strAll += _T("\\*");
	HANDLE hRtn = FindFirstFile(strAll, &fdFindData);
	if (INVALID_HANDLE_VALUE == hRtn)
		return;

	int left1 = path.ReverseFind(_T('/'));
	int left2 = path.ReverseFind(_T('\\'));
	int left = (left1 > left2)? left1 : left2;
	TString strFolderName = path.Right(path.GetLength() - left - 1);

	const TString strFD = _T(".");
	const TString strFDD = _T("..");
	do 
	{
		if (fdFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((0 != strFD.CompareNoCase(fdFindData.cFileName)) && (0 != strFDD.CompareNoCase(fdFindData.cFileName)))
			{
				TString strPath = path;
				strPath += _T("\\");
				strPath += fdFindData.cFileName;
				FindECMPathName(strPath);
			}
		}
		else
		{
			TString strFullName = fdFindData.cFileName;
			int dotIdx = strFullName.Find(_T("."));
			if (0 < dotIdx)
			{
				TString strName = strFullName.Left(dotIdx);
				TString strExt = strFullName.Right(strFullName.GetLength() - dotIdx - 1);
				if (0 == strExt.CompareNoCase(_T("ecm")))
				{
					if (0 == strFolderName.CompareNoCase(strName))
					{
						TString strPhysX = path;
						strPhysX += _T("\\");
						strPhysX += strName;
						strPhysX += _T(".mphy");
#ifdef _ANGELICA3
						if (af_IsFileExist(_TWC2AS(strPhysX), false))
#else
						if (af_IsFileExist(_TWC2AS(strPhysX)))
#endif
						{
							TString strPathName = path;
							strPathName += _T("\\");
							strPathName += fdFindData.cFileName;
							m_strModels.push_back(strPathName);
						}
					}
				}
			}
		}
	} while (FindNextFile(hRtn, &fdFindData) != 0);

	FindClose(hRtn);
}

BOOL CDlgSceneBuilder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_wndModelPath.EnableFolderBrowseButton();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
