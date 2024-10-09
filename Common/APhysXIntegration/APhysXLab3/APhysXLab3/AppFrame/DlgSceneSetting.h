#pragma once

#include "Scene.h"
// CDlgSceneSetting dialog

class MyEdit : public CBCGPEdit
{
public:
	MyEdit()
	{
		m_bMultiFiles = false;
	}
	virtual void OnBrowse()
	{
		DWORD flag = OFN_HIDEREADONLY
			| OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		if (m_bMultiFiles)
			flag |= OFN_ALLOWMULTISELECT;
		static ACHAR szFilter[] = _AL("Script Files (*.cs)|*.cs|All Files (*.*)|*.*||");

		CFileDialog dlg(TRUE, NULL, NULL, flag, szFilter);
		CString dirBase(_TAS2WC(af_GetBaseDir()));
		dirBase += _T("\\ApxUserScripts");
		dlg.m_ofn.lpstrInitialDir = (LPCTSTR)dirBase;
		CString str = _T("");
		if (dlg.DoModal() == IDOK)
		{
			POSITION pos = dlg.GetStartPosition();
			while (pos)
			{
				CString path = dlg.GetNextPathName(pos);
				int index = path.ReverseFind(_T('\\'));
				if (index != -1)
					path = path.Right(path.GetLength() - index - 1);
				if (path != _T(""))
					str += path + _T(";");
			}
			SetWindowText(str);
		}		
	}

	bool m_bMultiFiles;//enable select multi-files
};

class CDlgSceneSetting : public CDialog
{
	DECLARE_DYNAMIC(CDlgSceneSetting)

public:
	CDlgSceneSetting(Scene* pScene, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSceneSetting();

	bool IsFrameLimited() const { return m_bEnableFrmLimit; }
// Dialog Data
	enum { IDD = IDD_DLGSCENESETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	Scene*	m_scene;
	bool	m_bEnableFrmLimit;
	int		m_nbFrame;

	MyEdit m_ctrSceneScripts;
	MyEdit m_ctrDefScript;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCkbFramelimit();
};
