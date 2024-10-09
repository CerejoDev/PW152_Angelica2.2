#if !defined(AFX_DLGFFEDIT_H__EDE5D386_1620_4913_889B_E4A0D663C51B__INCLUDED_)
#define AFX_DLGFFEDIT_H__EDE5D386_1620_4913_889B_E4A0D663C51B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFFEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgFFEdit dialog

class DlgFFEdit : public CDialog
{
// Construction
public:
	DlgFFEdit(AString& strBindingFF, CWnd* pParent = NULL);   // standard constructor
	DlgFFEdit(AString& strBindingFF, AString& strInitFilePath, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(DlgFFEdit)
	enum { IDD = IDD_FFEDIT };
	CComboBox	m_cmbSB;
	CComboBox	m_cmbRB;
	CComboBox	m_cmbFluid;
	CComboBox	m_cmbCloth;
	CComboBox	m_comboFFT;
	float		m_volX;
	float		m_volY;
	float		m_volZ;
	CString	m_strFileName;
	float	m_scaleFluid;
	float	m_scaleRB;
	float	m_scaleSB;
	float	m_scaleCloth;
	float	m_WindX;
	float	m_WindY;
	float	m_WindZ;
	float	m_WindCT;
	float	m_WindRT;
	float	m_WindWT;
	float	m_WindCD;
	float	m_VortexTV;
	float	m_VortexTVM;
	float	m_VortexTR;
	float	m_VortexTRM;
	float	m_VortexCF;
	float	m_VortexEF;
	float	m_VortexEFS;
	float	m_VortexN;
	float	m_ExploCF;
	float	m_ExploR;
	float	m_ExploLT;
	float	m_ExploQFO;
	float	m_GravityX;
	float	m_GravityY;
	float	m_GravityZ;
	float	m_BuoyancyD;
	float	m_BossFV;
	float	m_BossR;
	CString	m_CurBindingMA;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgFFEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgFFEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioStbox();
	afx_msg void OnRadioStcapsule();
	afx_msg void OnRadioStsphere();
	virtual void OnOK();
	afx_msg void OnBtnLoad();
	afx_msg void OnSelchangeComboFftype();
	afx_msg void OnSelchangeComboRb();
	afx_msg void OnSelchangeComboCloth();
	afx_msg void OnSelchangeComboFluid();
	afx_msg void OnSelchangeComboSb();
	afx_msg void OnKillfocusEditRb();
	afx_msg void OnKillfocusEditCloth();
	afx_msg void OnKillfocusEditFluid();
	afx_msg void OnKillfocusEditSb();
	afx_msg void OnKillfocusEditWfdx();
	afx_msg void OnKillfocusEditWfdy();
	afx_msg void OnKillfocusEditWfdz();
	afx_msg void OnKillfocusEditWct();
	afx_msg void OnKillfocusEditWrt();
	afx_msg void OnKillfocusEditWwt();
	afx_msg void OnKillfocusEditWcd();
	afx_msg void OnKillfocusEditVtv();
	afx_msg void OnKillfocusEditVtvm();
	afx_msg void OnKillfocusEditVtr();
	afx_msg void OnKillfocusEditVtrm();
	afx_msg void OnKillfocusEditVcf();
	afx_msg void OnKillfocusEditVef();
	afx_msg void OnKillfocusEditVes();
	afx_msg void OnKillfocusEditVn();
	afx_msg void OnKillfocusEditEcf();
	afx_msg void OnKillfocusEditEr();
	afx_msg void OnKillfocusEditElt();
	afx_msg void OnKillfocusEditEqfo();
	afx_msg void OnKillfocusEditGx();
	afx_msg void OnKillfocusEditGy();
	afx_msg void OnKillfocusEditGz();
	afx_msg void OnKillfocusEditBd();
	afx_msg void OnKillfocusEditSsx();
	afx_msg void OnKillfocusEditSsy();
	afx_msg void OnKillfocusEditSsz();
	afx_msg void OnKillfocusEditBfv();
	afx_msg void OnKillfocusEditBr();
	afx_msg void OnBtnBindma();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	AString GetSavedFile() const { return m_SavedFile; }
	bool GetBindingFF(AString& outFF) const;

private:
	bool SetComboSelected(CComboBox& cb, const DWORD valItemData);
	bool UpdateFFShape(const APhysXShapeDescManager& sdm);

	void SetWindParaValue(const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters* pPara = 0);
	void SetVortexParaValue(const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters* pPara = 0);
	void SetExplosionParaValue(const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters* pPara = 0);
	void SetGravityParaValue(const NxVec3* pPara = 0);
	void SetBuoyancyParaValue(const float* pPara = 0);
	void SetBossAuraParaValue(const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters* pPara = 0);

	void EnableGroupWind(const bool bEnable);
	void EnableGroupVortex(const bool bEnable);
	void EnableGroupExplosion(const bool bEnable);
	void EnableGroupGravity(const bool bEnable);
	void EnableGroupBuoyancy(const bool bEnable);
	void EnableGroupVacuum(const bool bEnable);
	void EnableGroupBoss(const bool bEnable);

	void AppointCoordinate(const bool bEnable, int valCoor);
	void SetSpecialVortex(const bool bEnable);
	void SetSpecialExplosion(const bool bEnable);

	void OpenFileToEdit(const AString& toOpen);

private:
	enum Indicator
	{
		DDX_ID_BOSSAURA	 = (1<<23),
		DDX_ID_WIND		 = (1<<24),
		DDX_ID_VORTEX	 = (1<<25),
		DDX_ID_EXPLOSION = (1<<26),
		DDX_ID_GRAVITY	 = (1<<27),
		DDX_ID_BUOYANCY  = (1<<28),
		DDX_ID_SHAPEYZ   = (1<<29),
		DDX_ID_FILENAME  = (1<<30),
		DDX_ID_ONLY  = (1<<31),
	};

private:
	static float gDefScale;

	bool m_UpdateBindingFF;
	bool m_DoNotUpdateCTRLs;
	int m_ToUpdateID;
	float* m_pToUpdateVal;
	int nIDLastCoordinates;
	int nIDLastShape;
	
	AString m_InitToOpen;
	AString m_SavedFile;
	APhysXObjectDescManager m_objDescMgr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFFEDIT_H__EDE5D386_1620_4913_889B_E4A0D663C51B__INCLUDED_)
