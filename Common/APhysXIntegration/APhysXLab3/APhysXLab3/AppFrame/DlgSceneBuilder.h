#pragma once
#include "afxwin.h"


// CDlgSceneBuilder dialog

class CDlgSceneBuilder : public CDialog
{
	DECLARE_DYNAMIC(CDlgSceneBuilder)

public:
	CDlgSceneBuilder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSceneBuilder();

	typedef std::list<TString>		strList;
// Dialog Data
	enum { IDD = IDD_DLGSCENEBUILDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	const strList& GetModels() const { return m_strModels; }
	const CString& GetTerrianName() const { return m_strTrnName; }
	const CString& GetSceneName() const { return m_SceneName; }

private:
	void FindECMPathName(const TString& path);

private:
	strList m_strModels;

public:
	CString m_strTrnName;
	CString m_SceneName;
	CBCGPEdit	m_wndModelPath;
};
