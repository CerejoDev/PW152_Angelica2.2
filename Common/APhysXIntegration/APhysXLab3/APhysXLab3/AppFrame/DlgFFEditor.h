#pragma once

// CDlgFFEditor dialog
class CDlgEdit : public CBCGPEdit
{
public:
	CDlgEdit()
	{
		m_bIsOpen = true;
	}

	virtual void OnBrowse()
	{
		strRelPath.Empty();
		strFullPath.Empty();
		const ObjManager::ObjTypeInfo& objInfo = ObjManager::GetObjTypeInfo(ObjManager::OBJ_TYPEID_FORCEFIELD);

		DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
		if (!m_bIsOpen)
			dwFlags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

		BOOL bIsOpen = m_bIsOpen? TRUE : FALSE;
		CFileDialog FileDlg(bIsOpen, objInfo.strWCExt, NULL, dwFlags, objInfo.strFilter);
		TString dirBase(_TAS2TSting(af_GetBaseDir()));
		dirBase += _T("\\ForceField\\");
		FileDlg.m_ofn.lpstrInitialDir = dirBase;
		if (IDOK != FileDlg.DoModal())
			return;

		strFullPath = FileDlg.GetPathName();
		AString strTempRelPath;
		af_GetRelativePath(_TWC2AS(strFullPath), strTempRelPath);
		strRelPath = _TAS2WC(strTempRelPath);
		AString strName;
		af_GetFileTitle(strTempRelPath, strName);
		int right = strName.Find(".");
		strName = strName.Left(right);
		SetWindowText(_TAS2TSting(strName));
	}

	bool m_bIsOpen;
	TString strRelPath;
	TString strFullPath;
};

class CDlgFFEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgFFEditor)

public:
	CDlgFFEditor(CWnd* pParent = NULL, bool isImport = true);   // standard constructor
	virtual ~CDlgFFEditor();

// Dialog Data
	enum { IDD = IDD_DLG_FFEDIT };

public:
	virtual BOOL OnInitDialog();

	const TString& GetFullPath() const { return m_edtFileName.strFullPath; }
	const TString& GetRelPath() const { return m_edtFileName.strRelPath; }

private:
	CDlgEdit m_edtFileName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};
