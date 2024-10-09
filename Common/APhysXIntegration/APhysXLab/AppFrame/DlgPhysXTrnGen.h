#if !defined(AFX_DLGPHYSXTRNGEN_H__03B2628A_153D_4746_AC65_D1D31B43CA6D__INCLUDED_)
#define AFX_DLGPHYSXTRNGEN_H__03B2628A_153D_4746_AC65_D1D31B43CA6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPhysXTrnGen.h : header file
//

#include <AString.h>

class A3DTerrain2;
class A3DEngine;
class A3DTerrain2Block;

/////////////////////////////////////////////////////////////////////////////
// CDlgPhysXTrnGen dialog

class CDlgPhysXTrnGen : public CDialog
{
// Construction
public:
	CDlgPhysXTrnGen(const char* pCurrentTrnName, A3DEngine& eng, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPhysXTrnGen)
	enum { IDD = IDD_TRNBUILDER };
	CProgressCtrl	m_Progress;
	CEdit	m_TrnInfoEdit;
	CString	m_strTrnFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPhysXTrnGen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPhysXTrnGen)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnBtnBrowse();
	//}}AFX_MSG
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
	
	A3DTerrain2*	m_pTerrain;		//	Terrain object
	float*			m_pHeightMap;	//	Height map buffer
	
	// for hole support...
	// APhysXU32*		m_ExtraPatchData;
	// APhysXU32*		m_ExtraCellData;

	APhysXTerrainBlockData* m_pAPhysXTBData;

	A3DEngine& m_A3DEngine;
	AString m_CurTrnFilename;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPHYSXTRNGEN_H__03B2628A_153D_4746_AC65_D1D31B43CA6D__INCLUDED_)
