#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgPhysXTrnBuilder dialog

class CDlgPhysXTrnBuilder : public CDialog
{
	DECLARE_DYNAMIC(CDlgPhysXTrnBuilder)

public:
	CDlgPhysXTrnBuilder(const char* pCurrentTrnName, A3DEngine& eng, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPhysXTrnBuilder();

// Dialog Data
	enum { IDD = IDD_DLGPHYSXTRNBUILDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Print terrain info.
	void PrintTerrainInfo();
	//	Insert info text
	void InsertInfoText(const char* szText);

	//	Build terrain PhysX data
	bool BuildTerrainPhysXData();
	//	Build physX terrain data for specified sub-terrain
	bool BuildSubTerrainPhysXData(int r, int c, int iSubIndex);
	//	Fill block height to height map buffer
	void FillBlockHeight(A3DTerrain2Block* pBlock, int br, int bc);
	//	Save physX terrain configs
	bool SavePhysXDataConfigs();
	//	Save physX terrain data for specified sub-terrain
	bool SaveSubTerrainPhysXData(int iSubIndex, float fMinHei, float fMaxHei);

private:
	CProgressCtrl m_Progress;
	CEdit m_TrnInfoEdit;
	CString m_strTrnFile;

	A3DTerrain2*	m_pTerrain;
	float*			m_pHeightMap;
	APhysXTerrainBlockData* m_pAPhysXTBData;

	A3DEngine& m_A3DEngine;
	AString m_CurTrnFilename;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnBrowse();
};
