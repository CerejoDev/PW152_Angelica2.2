#if !defined(AFX_DLGFLAGS_H__60BF0486_0CE8_49E6_BCA1_109279E6EB37__INCLUDED_)
#define AFX_DLGFLAGS_H__60BF0486_0CE8_49E6_BCA1_109279E6EB37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFlags.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFlags dialog
class CPhysXObjDynamic;
class APhysXScene;

class CDlgFlags : public CDialog
{
// Construction
public:
	CDlgFlags(CPhysXObjDynamic& rObj, const bool IsEditMode, APhysXScene* pPhysXScene = 0, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFlags)
	enum { IDD = IDD_FLAGS_SETTING };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFlags)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFlags)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckHasCC();
	afx_msg void OnCheckHasLWCC();
	afx_msg void OnCheckIsNPC();
	afx_msg void OnRadioCNLCMN();
	afx_msg void OnRadioCNLIDP();
	afx_msg void OnRadioCNLIDPCMN();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void EnableGroupAPhysXObj(const bool bEnable);
	void EnableGroupPeople(const bool bEnable);
	void EnableGroupChannel(const bool bEnable, const bool forceRefresh = false);
	void EnableGroupNPCAction(const bool bEnable);
	void EnableGroupAPhysXCC(const bool bEnable, const bool SetDefVal = false);

private:
	bool IsEdit;
	CPhysXObjDynamic* pObj;
	APhysXScene* pScene;

	int UserSelectID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFLAGS_H__60BF0486_0CE8_49E6_BCA1_109279E6EB37__INCLUDED_)
