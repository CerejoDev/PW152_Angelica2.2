// DlgFFEdit.cpp : implementation file
//

#include "stdafx.h"
#include "aphysxlab.h"
#include "DlgFFEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgFFEdit dialog

float DlgFFEdit::gDefScale = 1.0f;
DlgFFEdit::DlgFFEdit(AString& strBindingFF, CWnd* pParent /*=NULL*/)
	: CDialog(DlgFFEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgFFEdit)
	m_volX = 0.0f;
	m_volY = 0.0f;
	m_volZ = 0.0f;
	m_strFileName = _T("");
	m_scaleFluid = 1.0f;
	m_scaleRB = 1.0f;
	m_scaleSB = 1.0f;
	m_scaleCloth = 1.0f;
	m_WindCD = 0.0f;
	m_CurBindingMA = _T("");
	//}}AFX_DATA_INIT

	SetWindParaValue();
	SetVortexParaValue();
	SetExplosionParaValue();
	SetGravityParaValue();
	SetBuoyancyParaValue();
	SetBossAuraParaValue();
	m_DoNotUpdateCTRLs = false;
	m_ToUpdateID = -1;
	m_pToUpdateVal = 0;
	nIDLastCoordinates = IDC_RADIO_CORCAR;
	nIDLastShape = IDC_RADIO_STBOX;
	m_UpdateBindingFF = false;
	m_CurBindingMA = strBindingFF;
}

DlgFFEdit::DlgFFEdit(AString& strBindingFF, AString& strInitFilePath, CWnd* pParent) : CDialog(DlgFFEdit::IDD, pParent)
{
	m_InitToOpen = strInitFilePath;

	m_volX = 0.0f;
	m_volY = 0.0f;
	m_volZ = 0.0f;
	m_strFileName = _T("");
	m_scaleFluid = 1.0f;
	m_scaleRB = 1.0f;
	m_scaleSB = 1.0f;
	m_scaleCloth = 1.0f;
	m_WindCD = 0.0f;
	
	SetWindParaValue();
	SetVortexParaValue();
	SetExplosionParaValue();
	SetGravityParaValue();
	SetBuoyancyParaValue();
	SetBossAuraParaValue();
	m_DoNotUpdateCTRLs = false;
	m_ToUpdateID = -1;
	m_pToUpdateVal = 0;
	nIDLastCoordinates = IDC_RADIO_CORCAR;
	nIDLastShape = IDC_RADIO_STBOX;
	m_UpdateBindingFF = false;
	m_CurBindingMA = strBindingFF;
}

void DlgFFEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (-1 != m_ToUpdateID)
	{
		const bool willRtn = (DDX_ID_ONLY & m_ToUpdateID)? true : false;
		m_ToUpdateID &= ~DDX_ID_ONLY; 
		if (DDX_ID_WIND & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_WFDX, m_WindX);
			DDX_Text(pDX, IDC_EDIT_WFDY, m_WindY);
			DDX_Text(pDX, IDC_EDIT_WFDZ, m_WindZ);
			DDX_Text(pDX, IDC_EDIT_WCT, m_WindCT);
			DDX_Text(pDX, IDC_EDIT_WRT, m_WindRT);
			DDX_Text(pDX, IDC_EDIT_WWT, m_WindWT);
			DDX_Text(pDX, IDC_EDIT_WCD, m_WindCD);
		}
		else if (DDX_ID_VORTEX & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_VTV, m_VortexTV);
			DDX_Text(pDX, IDC_EDIT_VTVM, m_VortexTVM);
			DDX_Text(pDX, IDC_EDIT_VTR, m_VortexTR);
			DDX_Text(pDX, IDC_EDIT_VTRM, m_VortexTRM);
			DDX_Text(pDX, IDC_EDIT_VCF, m_VortexCF);
			DDX_Text(pDX, IDC_EDIT_VEF, m_VortexEF);
			DDX_Text(pDX, IDC_EDIT_VES, m_VortexEFS);
			DDX_Text(pDX, IDC_EDIT_VN, m_VortexN);
		}
		else if (DDX_ID_EXPLOSION & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_ECF, m_ExploCF);
			DDX_Text(pDX, IDC_EDIT_ER, m_ExploR);
			DDX_Text(pDX, IDC_EDIT_ELT, m_ExploLT);
			DDX_Text(pDX, IDC_EDIT_EQFO, m_ExploQFO);
		}
		else if (DDX_ID_GRAVITY & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_GX, m_GravityX);
			DDX_Text(pDX, IDC_EDIT_GY, m_GravityY);
			DDX_Text(pDX, IDC_EDIT_GZ, m_GravityZ);
		}
		else if (DDX_ID_BUOYANCY & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_BD, m_BuoyancyD);
		}
		else if (DDX_ID_BOSSAURA & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_BAFV, m_BossFV);
			DDX_Text(pDX, IDC_EDIT_BAR, m_BossR);
		}
		else if (DDX_ID_SHAPEYZ & m_ToUpdateID)
		{
			DDX_Text(pDX, IDC_EDIT_SSY, m_volY);
			DDX_Text(pDX, IDC_EDIT_SSZ, m_volZ);
		}
		else if (DDX_ID_FILENAME & m_ToUpdateID)
		{
			if (0 == m_pToUpdateVal)
				DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
			else
				DDX_Text(pDX, IDC_STATIC_BINDMA, m_CurBindingMA);
		}
		else
		{
			assert(0 != m_pToUpdateVal);
			if (0 != m_pToUpdateVal)
				DDX_Text(pDX, m_ToUpdateID, *m_pToUpdateVal);
		}
		m_ToUpdateID = -1;
		m_pToUpdateVal = 0;
		if (willRtn)
			return;
	}

	if (GetDlgItem(IDC_EDIT_SSY)->IsWindowEnabled())
	{
		DDX_Text(pDX, IDC_EDIT_SSY, m_volY);
		DDV_MinMaxFloat(pDX, m_volY, 0.f, 1000.f);
	}
	if (GetDlgItem(IDC_EDIT_SSZ)->IsWindowEnabled())
	{
		DDX_Text(pDX, IDC_EDIT_SSZ, m_volZ);
		DDV_MinMaxFloat(pDX, m_volZ, 0.f, 1000.f);
	}
	if (GetDlgItem(IDC_EDIT_WCD)->IsWindowEnabled())
	{
		DDX_Text(pDX, IDC_EDIT_WCD, m_WindCD);
	}
	//{{AFX_DATA_MAP(DlgFFEdit)
	DDX_Control(pDX, IDC_COMBO_SB, m_cmbSB);
	DDX_Control(pDX, IDC_COMBO_RB, m_cmbRB);
	DDX_Control(pDX, IDC_COMBO_FLUID, m_cmbFluid);
	DDX_Control(pDX, IDC_COMBO_CLOTH, m_cmbCloth);
	DDX_Control(pDX, IDC_COMBO_FFTYPE, m_comboFFT);
	DDX_Text(pDX, IDC_EDIT_SSX, m_volX);
	DDV_MinMaxFloat(pDX, m_volX, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_FLUID, m_scaleFluid);
	DDV_MinMaxFloat(pDX, m_scaleFluid, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_RB, m_scaleRB);
	DDV_MinMaxFloat(pDX, m_scaleRB, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_SB, m_scaleSB);
	DDV_MinMaxFloat(pDX, m_scaleSB, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_CLOTH, m_scaleCloth);
	DDV_MinMaxFloat(pDX, m_scaleCloth, 0.f, 1000.f);
	DDX_Text(pDX, IDC_STATIC_BINDMA, m_CurBindingMA);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgFFEdit, CDialog)
	//{{AFX_MSG_MAP(DlgFFEdit)
	ON_BN_CLICKED(IDC_RADIO_STBOX, OnRadioStbox)
	ON_BN_CLICKED(IDC_RADIO_STCAPSULE, OnRadioStcapsule)
	ON_BN_CLICKED(IDC_RADIO_STSPHERE, OnRadioStsphere)
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBtnLoad)
	ON_CBN_SELCHANGE(IDC_COMBO_FFTYPE, OnSelchangeComboFftype)
	ON_CBN_SELCHANGE(IDC_COMBO_RB, OnSelchangeComboRb)
	ON_CBN_SELCHANGE(IDC_COMBO_CLOTH, OnSelchangeComboCloth)
	ON_CBN_SELCHANGE(IDC_COMBO_FLUID, OnSelchangeComboFluid)
	ON_CBN_SELCHANGE(IDC_COMBO_SB, OnSelchangeComboSb)
	ON_EN_KILLFOCUS(IDC_EDIT_RB, OnKillfocusEditRb)
	ON_EN_KILLFOCUS(IDC_EDIT_CLOTH, OnKillfocusEditCloth)
	ON_EN_KILLFOCUS(IDC_EDIT_FLUID, OnKillfocusEditFluid)
	ON_EN_KILLFOCUS(IDC_EDIT_SB, OnKillfocusEditSb)
	ON_EN_KILLFOCUS(IDC_EDIT_WFDX, OnKillfocusEditWfdx)
	ON_EN_KILLFOCUS(IDC_EDIT_WFDY, OnKillfocusEditWfdy)
	ON_EN_KILLFOCUS(IDC_EDIT_WFDZ, OnKillfocusEditWfdz)
	ON_EN_KILLFOCUS(IDC_EDIT_WCT, OnKillfocusEditWct)
	ON_EN_KILLFOCUS(IDC_EDIT_WRT, OnKillfocusEditWrt)
	ON_EN_KILLFOCUS(IDC_EDIT_WWT, OnKillfocusEditWwt)
	ON_EN_KILLFOCUS(IDC_EDIT_WCD, OnKillfocusEditWcd)
	ON_EN_KILLFOCUS(IDC_EDIT_VTV, OnKillfocusEditVtv)
	ON_EN_KILLFOCUS(IDC_EDIT_VTVM, OnKillfocusEditVtvm)
	ON_EN_KILLFOCUS(IDC_EDIT_VTR, OnKillfocusEditVtr)
	ON_EN_KILLFOCUS(IDC_EDIT_VTRM, OnKillfocusEditVtrm)
	ON_EN_KILLFOCUS(IDC_EDIT_VCF, OnKillfocusEditVcf)
	ON_EN_KILLFOCUS(IDC_EDIT_VEF, OnKillfocusEditVef)
	ON_EN_KILLFOCUS(IDC_EDIT_VES, OnKillfocusEditVes)
	ON_EN_KILLFOCUS(IDC_EDIT_VN, OnKillfocusEditVn)
	ON_EN_KILLFOCUS(IDC_EDIT_ECF, OnKillfocusEditEcf)
	ON_EN_KILLFOCUS(IDC_EDIT_ER, OnKillfocusEditEr)
	ON_EN_KILLFOCUS(IDC_EDIT_ELT, OnKillfocusEditElt)
	ON_EN_KILLFOCUS(IDC_EDIT_EQFO, OnKillfocusEditEqfo)
	ON_EN_KILLFOCUS(IDC_EDIT_GX, OnKillfocusEditGx)
	ON_EN_KILLFOCUS(IDC_EDIT_GY, OnKillfocusEditGy)
	ON_EN_KILLFOCUS(IDC_EDIT_GZ, OnKillfocusEditGz)
	ON_EN_KILLFOCUS(IDC_EDIT_BD, OnKillfocusEditBd)
	ON_EN_KILLFOCUS(IDC_EDIT_SSX, OnKillfocusEditSsx)
	ON_EN_KILLFOCUS(IDC_EDIT_SSY, OnKillfocusEditSsy)
	ON_EN_KILLFOCUS(IDC_EDIT_SSZ, OnKillfocusEditSsz)
	ON_EN_KILLFOCUS(IDC_EDIT_BAFV, OnKillfocusEditBfv)
	ON_EN_KILLFOCUS(IDC_EDIT_BAR, OnKillfocusEditBr)
	ON_BN_CLICKED(IDC_BTN_BINDMA, OnBtnBindma)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgFFEdit message handlers

BOOL DlgFFEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_comboFFT.InsertString(-1, _T("Wind"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_WIND);
	m_comboFFT.InsertString(-1, _T("Vortex"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_VORTEX);
	m_comboFFT.InsertString(-1, _T("Explosion"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_EXPLOSION);
	m_comboFFT.InsertString(-1, _T("Gravitation"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_GRAVITATION);
	m_comboFFT.InsertString(-1, _T("Buyancy"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_BUOYANCY);
	m_comboFFT.InsertString(-1, _T("Vacuum"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_VACUUM);
	m_comboFFT.InsertString(-1, _T("BossAura"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD_BOSS);
	m_comboFFT.InsertString(-1, _T("Custom"));
	m_comboFFT.SetItemData(m_comboFFT.GetCount() - 1, APX_OBJTYPE_FORCEFIELD);
	m_comboFFT.SetCurSel(0);

	CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, IDC_RADIO_CORCAR);
	CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, IDC_RADIO_STBOX);
	GetDlgItem(IDC_RADIO_STCONVEX)->EnableWindow(false);
	GetDlgItem(IDC_STATIC_R)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_H)->ShowWindow(SW_HIDE);

	m_cmbRB.InsertString(-1, _T("OTHER"));
	m_cmbRB.SetItemData(m_cmbRB.GetCount() - 1, NX_FF_TYPE_OTHER);
	m_cmbRB.InsertString(-1, _T("GRAVITATIONAL"));
	m_cmbRB.SetItemData(m_cmbRB.GetCount() - 1, NX_FF_TYPE_GRAVITATIONAL);
	m_cmbRB.InsertString(-1, _T("NO_INTERACTION"));
	m_cmbRB.SetItemData(m_cmbRB.GetCount() - 1, NX_FF_TYPE_NO_INTERACTION);
	m_cmbRB.SetCurSel(0);
	EnableGroupWind(true);
	EnableGroupVortex(false);
	EnableGroupExplosion(false);
	EnableGroupGravity(false);
	EnableGroupBuoyancy(false);
	EnableGroupVacuum(false);
	EnableGroupBoss(false);

	m_cmbSB.InsertString(-1, _T("OTHER"));
	m_cmbSB.SetItemData(m_cmbSB.GetCount() - 1, NX_FF_TYPE_OTHER);
	m_cmbSB.InsertString(-1, _T("GRAVITATIONAL"));
	m_cmbSB.SetItemData(m_cmbSB.GetCount() - 1, NX_FF_TYPE_GRAVITATIONAL);
	m_cmbSB.InsertString(-1, _T("NO_INTERACTION"));
	m_cmbSB.SetItemData(m_cmbSB.GetCount() - 1, NX_FF_TYPE_NO_INTERACTION);
	m_cmbSB.SetCurSel(0);

	m_cmbCloth.InsertString(-1, _T("OTHER"));
	m_cmbCloth.SetItemData(m_cmbCloth.GetCount() - 1, NX_FF_TYPE_OTHER);
	m_cmbCloth.InsertString(-1, _T("GRAVITATIONAL"));
	m_cmbCloth.SetItemData(m_cmbCloth.GetCount() - 1, NX_FF_TYPE_GRAVITATIONAL);
	m_cmbCloth.InsertString(-1, _T("NO_INTERACTION"));
	m_cmbCloth.SetItemData(m_cmbCloth.GetCount() - 1, NX_FF_TYPE_NO_INTERACTION);
	m_cmbCloth.SetCurSel(0);

	m_cmbFluid.InsertString(-1, _T("OTHER"));
	m_cmbFluid.SetItemData(m_cmbFluid.GetCount() - 1, NX_FF_TYPE_OTHER);
	m_cmbFluid.InsertString(-1, _T("GRAVITATIONAL"));
	m_cmbFluid.SetItemData(m_cmbFluid.GetCount() - 1, NX_FF_TYPE_GRAVITATIONAL);
	m_cmbFluid.InsertString(-1, _T("NO_INTERACTION"));
	m_cmbFluid.SetItemData(m_cmbFluid.GetCount() - 1, NX_FF_TYPE_NO_INTERACTION);
	m_cmbFluid.SetCurSel(0);

	if (!m_InitToOpen.IsEmpty())
		OpenFileToEdit(m_InitToOpen);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgFFEdit::OnRadioStbox() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_R)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_H)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_SSY)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_SSZ)->EnableWindow(true);
	m_ToUpdateID = DDX_ID_SHAPEYZ | DDX_ID_ONLY;
	UpdateData(false);
	UpdateWindow();
}

void DlgFFEdit::OnRadioStcapsule() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_R)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_H)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_SSY)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_SSZ)->EnableWindow(false);
	m_ToUpdateID = IDC_EDIT_SSY | DDX_ID_ONLY;
	m_pToUpdateVal = &m_volY;
	UpdateData(false);
	UpdateWindow();
	GetDlgItem(IDC_EDIT_SSZ)->SetWindowText(_T(""));
}

void DlgFFEdit::OnRadioStsphere() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_R)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_H)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_SSY)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SSZ)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SSY)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_SSZ)->SetWindowText(_T(""));
}

void DlgFFEdit::OnOK() 
{
	// TODO: Add extra validation here
	if (!UpdateData(true))
	{
		assert(true == false);
		return;
	}

	APhysXShapeDesc* pShapeDesc = 0;
	APhysXBoxShapeDesc boxDesc;
	APhysXSphereShapeDesc sphereDesc;
	APhysXCapsuleShapeDesc capsuleDesc;
	const int nID = GetCheckedRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCAPSULE);
	if (nID == IDC_RADIO_STBOX)
	{
		if (0 >= m_volX) { GetDlgItem(IDC_EDIT_SSX)->SetFocus(); AfxMessageBox(_T("Shape大小不能为空！"), MB_OK | MB_ICONERROR); return; }
		if (0 >= m_volY) { GetDlgItem(IDC_EDIT_SSY)->SetFocus(); AfxMessageBox(_T("Shape大小不能为空！"), MB_OK | MB_ICONERROR); return; }
		if (0 >= m_volZ) { GetDlgItem(IDC_EDIT_SSZ)->SetFocus(); AfxMessageBox(_T("Shape大小不能为空！"), MB_OK | MB_ICONERROR); return; }
		boxDesc.mDimensions.set(m_volX, m_volY, m_volZ);
		pShapeDesc = &boxDesc;
	}
	else if (nID == IDC_RADIO_STSPHERE)
	{
		if (0 >= m_volX) { GetDlgItem(IDC_EDIT_SSX)->SetFocus(); AfxMessageBox(_T("Shape大小不能为空！"), MB_OK | MB_ICONERROR); return; }
		m_volY = m_volZ = 0;
		sphereDesc.mRadius = m_volX;
		pShapeDesc = &sphereDesc;
	}
	else if (nID == IDC_RADIO_STCAPSULE)
	{
		if (0 >= m_volX) { GetDlgItem(IDC_EDIT_SSX)->SetFocus(); AfxMessageBox(_T("Shape大小不能为空！"), MB_OK | MB_ICONERROR); return; }
		if (0 >= m_volY) { GetDlgItem(IDC_EDIT_SSY)->SetFocus(); AfxMessageBox(_T("Shape大小不能为空！"), MB_OK | MB_ICONERROR); return; }
		m_volZ = 0;
		capsuleDesc.mRadius = m_volX;
		capsuleDesc.mHeight = m_volY;
		pShapeDesc = &capsuleDesc;
	}
	else if (nID == IDC_RADIO_STCONVEX)
	{
		assert(!"Oops! Unsupport convex shape type.");
		return;
	}
	else
	{
		assert(!"Oops! Unknown shape type.");
		return;
	}

	if (m_strFileName.IsEmpty())
	{
		AfxMessageBox(_T("文件名不能为空！"), MB_OK | MB_ICONERROR);
		return;
	}

	m_SavedFile = "ForceField\\";
	m_SavedFile += _TWC2AS(m_strFileName);
	m_SavedFile += ".xff";
	AString strPath = af_GetBaseDir();
	strPath += "\\";
	strPath += m_SavedFile;
	if (af_IsFileExist(strPath))
	{
		if (IDNO == AfxMessageBox(_T("该文件已存在，是否覆盖？"), MB_YESNO | MB_ICONQUESTION))
			return;
	}

	if (0 < m_objDescMgr.GetNum())
		m_objDescMgr.Release();

	APhysXObjectType objType = static_cast<APhysXObjectType>(m_comboFFT.GetItemData(m_comboFFT.GetCurSel()));
	APhysXObjectDesc* pFFOD = m_objDescMgr.CreateAndPushObjectDesc(objType);
	assert(0 != pFFOD);
	APhysXForceFieldObjectDesc* pFF = static_cast<APhysXForceFieldObjectDesc*>(pFFOD);
	if (0 != pFF)
	{
		pFF->mRBType = static_cast<NxForceFieldType>(m_cmbRB.GetItemData(m_cmbRB.GetCurSel()));
		pFF->mClothType = static_cast<NxForceFieldType>(m_cmbCloth.GetItemData(m_cmbCloth.GetCurSel()));
		pFF->mFluidType = static_cast<NxForceFieldType>(m_cmbFluid.GetItemData(m_cmbFluid.GetCurSel()));
		pFF->mSBType = static_cast<NxForceFieldType>(m_cmbSB.GetItemData(m_cmbSB.GetCurSel()));
		pFF->mRBForceScale = m_scaleRB;
		pFF->mClothForceScale = m_scaleCloth;
		pFF->mFluidForceScale = m_scaleFluid;
		pFF->mSBForceScale = m_scaleSB;
		const int nIDCor = GetCheckedRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR);
		if (nIDCor == IDC_RADIO_CORCAR)
			pFF->mFFCoordinates = NX_FFC_CARTESIAN;
		else if (nIDCor == IDC_RADIO_CORSPH)
			pFF->mFFCoordinates = NX_FFC_SPHERICAL;
		else if (nIDCor == IDC_RADIO_CORCYL)
			pFF->mFFCoordinates = NX_FFC_CYLINDRICAL;
		else if (nIDCor == IDC_RADIO_CORTOR)
			pFF->mFFCoordinates = NX_FFC_TOROIDAL;
		nIDLastCoordinates = nIDCor;
		nIDLastShape = nID;
	}
	
	switch(objType)
	{
	case APX_OBJTYPE_FORCEFIELD_WIND:
		{
			APhysXWindForceFieldObjectDesc* pDesc = static_cast<APhysXWindForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
			APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters apxWindFFPara;
			apxWindFFPara.mCycleTime = m_WindCT;
			apxWindFFPara.mRestTime = m_WindRT;
			apxWindFFPara.mWindTime = m_WindWT;
			apxWindFFPara.mWindForce.x = m_WindX;
			apxWindFFPara.mWindForce.y = m_WindY;
			apxWindFFPara.mWindForce.z = m_WindZ;
			pDesc->SetWindFFParameters(apxWindFFPara);
			pDesc->mCycleDistance = m_WindCD;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_VORTEX:
		{
			APhysXVortexForceFieldObjectDesc* pDesc = static_cast<APhysXVortexForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
			APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters apxVFFPara;
			apxVFFPara.mTargetVelocity			= m_VortexTV;
			apxVFFPara.mTargetVelocityScaleTerm = m_VortexTVM;
			apxVFFPara.mTargetRadius			= m_VortexTR;
			apxVFFPara.mTargetRadiusScaleTerm	= m_VortexTRM;
			apxVFFPara.mConstantCentripetalForce = m_VortexCF;
			apxVFFPara.mConstantElevationForce	= m_VortexEF;
			apxVFFPara.mElevationExpandScaleTerm = m_VortexEFS;
			apxVFFPara.mNoise = m_VortexN;
			pDesc->SetVortexFFParameters(apxVFFPara);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
		{
			APhysXGravitationForceFieldObjectDesc* pDesc = static_cast<APhysXGravitationForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
			pDesc->SetGravity(NxVec3(m_GravityX, m_GravityY, m_GravityZ));
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
		{
			APhysXExplosionForceFieldObjectDesc* pDesc = static_cast<APhysXExplosionForceFieldObjectDesc*>(pFFOD);
			assert(0 != pDesc->mIncludeShapes.GetNum());
			APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters apxEFFPara;
			apxEFFPara.mCenterForce = m_ExploCF;
			apxEFFPara.mRadius		= m_ExploR;
			apxEFFPara.mLifeTime	= m_ExploLT;
			apxEFFPara.mQuadricFallOff = m_ExploQFO;
			pDesc->SetExplosionFFParameters(apxEFFPara);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
		{
			APhysXBuoyancyForceFieldObjectDesc* pDesc = static_cast<APhysXBuoyancyForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
			pDesc->SetDensity(m_BuoyancyD);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
		{
			APhysXVacuumForceFieldObjectDesc* pDesc = static_cast<APhysXVacuumForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_BOSS:
		{
			APhysXBossForceFieldObjectDesc* pDesc = static_cast<APhysXBossForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
			APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters apxBossFFPara;
			apxBossFFPara.mForceValue = m_BossFV;
			apxBossFFPara.mRadius = m_BossR;
			pDesc->SetBossFFParameters(apxBossFFPara);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD:
		{
			APhysXForceFieldObjectDesc* pDesc = static_cast<APhysXForceFieldObjectDesc*>(pFFOD);
			pDesc->mIncludeShapes.CreateAndPushShapeDesc(pShapeDesc);
		}
		break;
	}

	m_objDescMgr.Save(strPath);
	CDialog::OnOK();
}

void DlgFFEdit::OnBtnLoad() 
{
	// TODO: Add your control notification handler code here
	const CPhysXObjMgr::ObjInfo& objInfo = CPhysXObjMgr::GetObjInfo(CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD);
	
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, objInfo.strWCExt, NULL, dwFlags, objInfo.strFilter, NULL);
	AString strPath = af_GetBaseDir();
	strPath += "\\ForceField\\";
	FileDia.m_ofn.lpstrInitialDir = _TAS2WC(strPath);
	if (FileDia.DoModal() != IDOK)
		return;
	
	AString strFilePath;
	af_GetRelativePath(_TWC2AS(FileDia.GetPathName()), strFilePath);
	OpenFileToEdit(strFilePath);
}

void DlgFFEdit::OpenFileToEdit(const AString& toOpen)
{
	APhysXObjectDesc* pDesc = m_objDescMgr.GetPhysXObjectDesc(toOpen);
	if (0 == pDesc)
	{
		AfxMessageBox(_T("打开文件失败！"), MB_OK | MB_ICONERROR);
		return;
	}

	const APhysXObjectType ot = pDesc->GetObjType();
	SetComboSelected(m_comboFFT, ot);
	m_DoNotUpdateCTRLs = true;
	OnSelchangeComboFftype();

	m_strFileName = toOpen;
	int right = m_strFileName.Find(_T("."));
	m_strFileName = m_strFileName.Left(right);
	int left = m_strFileName.Find(_T("\\"));
	m_strFileName = m_strFileName.Right(m_strFileName.GetLength() - left - 1);

	APhysXForceFieldObjectDesc* pFF = static_cast<APhysXForceFieldObjectDesc*>(pDesc);
	if (0 != pFF)
	{
		SetComboSelected(m_cmbRB, pFF->mRBType);
		SetComboSelected(m_cmbCloth, pFF->mClothType);
		SetComboSelected(m_cmbFluid, pFF->mFluidType);
		SetComboSelected(m_cmbSB, pFF->mSBType);
		OnSelchangeComboRb();
		OnSelchangeComboCloth();
		OnSelchangeComboFluid();
		OnSelchangeComboSb();
		m_scaleRB = pFF->mRBForceScale;
		m_scaleCloth = pFF->mClothForceScale;
		m_scaleFluid = pFF->mFluidForceScale;
		m_scaleSB = pFF->mSBForceScale;
		switch(pFF->mFFCoordinates)
		{
		case NX_FFC_CARTESIAN:
			CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, IDC_RADIO_CORCAR);
			nIDLastCoordinates = IDC_RADIO_CORCAR;
			break;
		case NX_FFC_SPHERICAL:
			CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, IDC_RADIO_CORSPH);
			nIDLastCoordinates = IDC_RADIO_CORSPH;
			break;
		case NX_FFC_CYLINDRICAL:
			CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, IDC_RADIO_CORCYL);
			nIDLastCoordinates = IDC_RADIO_CORCYL;
			break;
		case NX_FFC_TOROIDAL:
			CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, IDC_RADIO_CORTOR);
			nIDLastCoordinates = IDC_RADIO_CORTOR;
			break;
		}
	}

	SetWindParaValue();
	SetVortexParaValue();
	SetExplosionParaValue();
	SetGravityParaValue();
	SetBuoyancyParaValue();
	SetBossAuraParaValue();
	switch(ot)
	{
	case APX_OBJTYPE_FORCEFIELD_WIND:
		{
			APhysXWindForceFieldObjectDesc* pFFOD = static_cast<APhysXWindForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = pFFOD->GetWindFFParameters();
			SetWindParaValue(&para);
			m_WindCD = pFFOD->mCycleDistance;
			m_ToUpdateID = DDX_ID_WIND;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_VORTEX:
		{
			APhysXVortexForceFieldObjectDesc* pFFOD = static_cast<APhysXVortexForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = pFFOD->GetVortexFFParameters();
			SetVortexParaValue(&para);
			SetSpecialVortex(true);
			m_ToUpdateID = DDX_ID_VORTEX;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
		{
			APhysXGravitationForceFieldObjectDesc* pFFOD = static_cast<APhysXGravitationForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			SetGravityParaValue(&pFFOD->GetGravity());
			m_ToUpdateID = DDX_ID_GRAVITY;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
		{
			APhysXExplosionForceFieldObjectDesc* pFFOD = static_cast<APhysXExplosionForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters& para = pFFOD->GetExplosionFFParameters();
			SetExplosionParaValue(&para);
			SetSpecialExplosion(true);
			m_ToUpdateID = DDX_ID_EXPLOSION;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
		{
			APhysXBuoyancyForceFieldObjectDesc* pFFOD = static_cast<APhysXBuoyancyForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			float d = pFFOD->GetDensity();
			SetBuoyancyParaValue(&d);
			m_ToUpdateID = DDX_ID_BUOYANCY;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
		{
			APhysXVacuumForceFieldObjectDesc* pFFOD = static_cast<APhysXVacuumForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			m_ToUpdateID = DDX_ID_VORTEX;
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_BOSS:
		{
			APhysXBossForceFieldObjectDesc* pFFOD = static_cast<APhysXBossForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
			const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters& para = pFFOD->GetBossFFParameters();
			SetBossAuraParaValue(&para);
			m_ToUpdateID = DDX_ID_BOSSAURA;
			AppointCoordinate(true, IDC_RADIO_CORCAR);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD:
		{
			APhysXForceFieldObjectDesc* pFFOD = static_cast<APhysXForceFieldObjectDesc*>(pDesc);
			UpdateFFShape(pFFOD->mIncludeShapes);
		}
		break;
	}
	UpdateData(false);
}

bool DlgFFEdit::SetComboSelected(CComboBox& cb, const DWORD valItemData)
{
	int nCount = cb.GetCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (valItemData == cb.GetItemData(i))
		{
			cb.SetCurSel(i);
			return true;
		}
	}
	return false;
}

bool DlgFFEdit::UpdateFFShape(const APhysXShapeDescManager& sdm)
{
	int nCount = sdm.GetNum();
	if (0 >= nCount)
		return false;

	bool rtn = false;

	APhysXShapeDesc* pDesc = sdm.GetPhysXShapeDesc(0);
	switch(pDesc->GetShapeType())
	{
	case APX_SHAPETYPE_BOX:
		{
			CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, IDC_RADIO_STBOX);
			OnRadioStbox();
			rtn = true;
			APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
			m_volX = pBox->mDimensions.x;
			m_volY = pBox->mDimensions.y;
			m_volZ = pBox->mDimensions.z;
		}
		break;
	case APX_SHAPETYPE_SPHERE:
		{
			CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, IDC_RADIO_STSPHERE);
			OnRadioStsphere();
			rtn = true;
			APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
			m_volX = pSphere->mRadius;
			m_volY = m_volZ = 0;
		}
		break;
	case APX_SHAPETYPE_CAPSULE:
		{
			CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, IDC_RADIO_STCAPSULE);
			OnRadioStcapsule();
			rtn = true;
			APhysXCapsuleShapeDesc* pCapsule = static_cast<APhysXCapsuleShapeDesc*>(pDesc);
			m_volX = pCapsule->mRadius;
			m_volY = pCapsule->mHeight;
			m_volZ = 0;
		}
		break;
	case APX_SHAPETYPE_CONVEX:
		CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, IDC_RADIO_STCONVEX);
		break;
	}

	return rtn;
}

void DlgFFEdit::OnSelchangeComboFftype() 
{
	// TODO: Add your control notification handler code here
	APhysXObjectType objType = static_cast<APhysXObjectType>(m_comboFFT.GetItemData(m_comboFFT.GetCurSel()));
	switch(objType)
	{
	case APX_OBJTYPE_FORCEFIELD_WIND:
		EnableGroupWind(true);
		EnableGroupVortex(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(false);
		EnableGroupBoss(false);
		break;
	case APX_OBJTYPE_FORCEFIELD_VORTEX:
		EnableGroupWind(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(false);
		EnableGroupBoss(false);
		EnableGroupVortex(true);
		break;
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
		EnableGroupWind(false);
		EnableGroupVortex(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(true);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(false);
		EnableGroupBoss(false);
		break;
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
		EnableGroupWind(false);
		EnableGroupVortex(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(false);
		EnableGroupBoss(false);
		EnableGroupExplosion(true);
		break;
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
		EnableGroupWind(false);
		EnableGroupVortex(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(true);
		EnableGroupVacuum(false);
		EnableGroupBoss(false);
		break;
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
		EnableGroupWind(false);
		EnableGroupVortex(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(true);
		EnableGroupBoss(false);
		break;
	case APX_OBJTYPE_FORCEFIELD_BOSS:
		EnableGroupWind(false);
		EnableGroupVortex(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(false);
		EnableGroupBoss(true);
		break;
	case APX_OBJTYPE_FORCEFIELD:
		EnableGroupWind(false);
		EnableGroupVortex(false);
		EnableGroupExplosion(false);
		EnableGroupGravity(false);
		EnableGroupBuoyancy(false);
		EnableGroupVacuum(false);
		EnableGroupBoss(false);
		break;
	}
}

void DlgFFEdit::OnSelchangeComboRb() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_RB | DDX_ID_ONLY;
	NxForceFieldType fft = static_cast<NxForceFieldType>(m_cmbRB.GetItemData(m_cmbRB.GetCurSel()));
	if (NX_FF_TYPE_OTHER == fft)
	{
		m_pToUpdateVal = &m_scaleRB;
		GetDlgItem(IDC_EDIT_RB)->EnableWindow(true);
	}
	else
	{
		m_pToUpdateVal = &gDefScale;
		GetDlgItem(IDC_EDIT_RB)->EnableWindow(false);
	}
	UpdateData(false);
	UpdateWindow();
}

void DlgFFEdit::OnSelchangeComboCloth() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_CLOTH | DDX_ID_ONLY;
	NxForceFieldType fft = static_cast<NxForceFieldType>(m_cmbCloth.GetItemData(m_cmbCloth.GetCurSel()));
	if (NX_FF_TYPE_OTHER == fft)
	{
		m_pToUpdateVal = &m_scaleCloth;
		GetDlgItem(IDC_EDIT_CLOTH)->EnableWindow(true);
	}
	else
	{
		m_pToUpdateVal = &gDefScale;
		GetDlgItem(IDC_EDIT_CLOTH)->EnableWindow(false);
	}
	UpdateData(false);
	UpdateWindow();
}

void DlgFFEdit::OnSelchangeComboFluid() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_FLUID | DDX_ID_ONLY;
	NxForceFieldType fft = static_cast<NxForceFieldType>(m_cmbFluid.GetItemData(m_cmbFluid.GetCurSel()));
	if (NX_FF_TYPE_OTHER == fft)
	{
		m_pToUpdateVal = &m_scaleFluid;
		GetDlgItem(IDC_EDIT_FLUID)->EnableWindow(true);
	}
	else
	{
		m_pToUpdateVal = &gDefScale;
		GetDlgItem(IDC_EDIT_FLUID)->EnableWindow(false);
	}
	UpdateData(false);
	UpdateWindow();
}

void DlgFFEdit::OnSelchangeComboSb() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_SB | DDX_ID_ONLY;
	NxForceFieldType fft = static_cast<NxForceFieldType>(m_cmbSB.GetItemData(m_cmbSB.GetCurSel()));
	if (NX_FF_TYPE_OTHER == fft)
	{
		m_pToUpdateVal = &m_scaleSB;
		GetDlgItem(IDC_EDIT_SB)->EnableWindow(true);
	}
	else
	{
		m_pToUpdateVal = &gDefScale;
		GetDlgItem(IDC_EDIT_SB)->EnableWindow(false);
	}
	UpdateData(false);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditSsx() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_SSX | DDX_ID_ONLY;
	m_pToUpdateVal = &m_volX;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditSsy() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_SSY | DDX_ID_ONLY;
	m_pToUpdateVal = &m_volY;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditSsz() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_SSZ | DDX_ID_ONLY;
	m_pToUpdateVal = &m_volZ;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditRb() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_RB | DDX_ID_ONLY;
	m_pToUpdateVal = &m_scaleRB;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditCloth() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_CLOTH | DDX_ID_ONLY;
	m_pToUpdateVal = &m_scaleCloth;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditFluid() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_FLUID | DDX_ID_ONLY;
	m_pToUpdateVal = &m_scaleFluid;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditSb() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_SB | DDX_ID_ONLY;
	m_pToUpdateVal = &m_scaleSB;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWfdx() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WFDX | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindX;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWfdy() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WFDY | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindY;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWfdz() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WFDZ | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindZ;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWct() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WCT | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindCT;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWrt() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WRT | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindRT;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWwt() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WWT | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindWT;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditWcd() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_WCD | DDX_ID_ONLY;
	m_pToUpdateVal = &m_WindCD;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVtv() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VTV | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexTV;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVtvm() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VTVM | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexTVM;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVtr() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VTR | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexTR;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVtrm() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VTRM | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexTRM;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVcf() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VCF | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexCF;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVef() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VEF | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexEF;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVes() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VES | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexEFS;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditVn() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_VN | DDX_ID_ONLY;
	m_pToUpdateVal = &m_VortexN;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditEcf() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_ECF | DDX_ID_ONLY;
	m_pToUpdateVal = &m_ExploCF;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditEr() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_ER | DDX_ID_ONLY;
	m_pToUpdateVal = &m_ExploR;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditElt() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_ELT | DDX_ID_ONLY;
	m_pToUpdateVal = &m_ExploLT;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditEqfo() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_EQFO | DDX_ID_ONLY;
	m_pToUpdateVal = &m_ExploQFO;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditGx() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_GX | DDX_ID_ONLY;
	m_pToUpdateVal = &m_GravityX;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditGy() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_GY | DDX_ID_ONLY;
	m_pToUpdateVal = &m_GravityY;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditGz() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_GZ | DDX_ID_ONLY;
	m_pToUpdateVal = &m_GravityZ;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditBd() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_BD | DDX_ID_ONLY;
	m_pToUpdateVal = &m_BuoyancyD;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditBfv() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_BAFV | DDX_ID_ONLY;
	m_pToUpdateVal = &m_BossFV;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::OnKillfocusEditBr() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = IDC_EDIT_BAR | DDX_ID_ONLY;
	m_pToUpdateVal = &m_BossR;
	UpdateData(true);
	UpdateWindow();
}

void DlgFFEdit::SetWindParaValue(const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters* pPara)
{
	APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters Wind;
	if (0 == pPara)
		pPara = &Wind;
	m_WindCT = pPara->mCycleTime;
	m_WindRT = pPara->mRestTime;
	m_WindWT = pPara->mWindTime;
	m_WindX = pPara->mWindForce.x;
	m_WindY = pPara->mWindForce.y;
	m_WindZ = pPara->mWindForce.z;
	m_WindCD = 100;
}

void DlgFFEdit::SetVortexParaValue(const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters* pPara)
{
	APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters Vortex;
	if (0 == pPara)
		pPara = &Vortex;
	m_VortexTV	= pPara->mTargetVelocity;
	m_VortexTVM = pPara->mTargetVelocityScaleTerm;
	m_VortexTR	= pPara->mTargetRadius;
	m_VortexTRM	= pPara->mTargetRadiusScaleTerm;
	m_VortexCF	= pPara->mConstantCentripetalForce;
	m_VortexEF	= pPara->mConstantElevationForce;
	m_VortexEFS = pPara->mElevationExpandScaleTerm;
	m_VortexN	= pPara->mNoise;
}

void DlgFFEdit::SetExplosionParaValue(const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters* pPara)
{
	APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters Explo;
	if (0 == pPara)
		pPara = &Explo;
	m_ExploCF = pPara->mCenterForce;
	m_ExploR  = pPara->mRadius;
	m_ExploLT = pPara->mLifeTime;
	m_ExploQFO = pPara->mQuadricFallOff;
}

void DlgFFEdit::SetGravityParaValue(const NxVec3* pPara)
{
	NxVec3 zero(0.0f);
	if (0 == pPara)
		pPara = &zero;
	m_GravityX = pPara->x;
	m_GravityY = pPara->y;
	m_GravityZ = pPara->z;
}

void DlgFFEdit::SetBuoyancyParaValue(const float* pPara)
{
	if (0 == pPara)
		m_BuoyancyD = 1.0f;
	else
		m_BuoyancyD = *pPara;
}

void DlgFFEdit::SetBossAuraParaValue(const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters* pPara)
{
	APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters Boss;
	if (0 == pPara)
		pPara = &Boss;
	m_BossFV = pPara->mForceValue;
	m_BossR = pPara->mRadius;
}

void DlgFFEdit::EnableGroupWind(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_WIND)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WFD)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WX)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WY)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WZ)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WWT)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WRT)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WCT)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_WCD)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WFDX)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WFDY)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WFDZ)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WWT)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WRT)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WCT)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_WCD)->EnableWindow(bEnable);

	if(bEnable)
	{
		if (m_DoNotUpdateCTRLs)
			m_DoNotUpdateCTRLs = false;
		else
		{
			m_ToUpdateID = DDX_ID_WIND | DDX_ID_ONLY;
			UpdateData(false);
			UpdateWindow();
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_WFDX)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_WFDY)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_WFDZ)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_WWT)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_WRT)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_WCT)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_WCD)->SetWindowText(_T(""));
	}
}

void DlgFFEdit::EnableGroupVortex(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_VORTEX)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VTV)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VTVM)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VTR)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VTRM)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VCF)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VEF)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VES)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VN)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VTV)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VTVM)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VTR)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VTRM)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VCF)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VEF)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VES)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VN)->EnableWindow(bEnable);

	SetSpecialVortex(bEnable);
	if(bEnable)
	{
		if (m_DoNotUpdateCTRLs)
			m_DoNotUpdateCTRLs = false;
		else
		{
			m_ToUpdateID = DDX_ID_VORTEX | DDX_ID_ONLY;
			UpdateData(false);
			UpdateWindow();
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_VTV)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VTVM)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VTR)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VTRM)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VCF)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VEF)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VES)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_VN)->SetWindowText(_T(""));
	}
}

void DlgFFEdit::EnableGroupExplosion(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_EXPLOSION)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ECF)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ER)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ELT)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_EQFO)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ECF)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ER)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ELT)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_EQFO)->EnableWindow(bEnable);

	SetSpecialExplosion(bEnable);
	if(bEnable)
	{
		if (m_DoNotUpdateCTRLs)
			m_DoNotUpdateCTRLs = false;
		else
		{
			m_ToUpdateID = DDX_ID_EXPLOSION | DDX_ID_ONLY;
			UpdateData(false);
			UpdateWindow();
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_ECF)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ER)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ELT)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_EQFO)->SetWindowText(_T(""));
	}
}

void DlgFFEdit::EnableGroupGravity(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_GRAVITATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_GG)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_GX)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_GY)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_GZ)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_GX)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_GY)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_GZ)->EnableWindow(bEnable);
	if(bEnable)
	{
		if (m_DoNotUpdateCTRLs)
			m_DoNotUpdateCTRLs = false;
		else
		{
			m_ToUpdateID = DDX_ID_GRAVITY | DDX_ID_ONLY;
			UpdateData(false);
			UpdateWindow();
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_GX)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_GY)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_GZ)->SetWindowText(_T(""));
	}
}

void DlgFFEdit::EnableGroupBuoyancy(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_BUOYANCY)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_BD)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_BD)->EnableWindow(bEnable);
	if(bEnable)
	{
		if (m_DoNotUpdateCTRLs)
			m_DoNotUpdateCTRLs = false;
		else
		{
			m_ToUpdateID = DDX_ID_BUOYANCY | DDX_ID_ONLY;
			UpdateData(false);
			UpdateWindow();
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_BD)->SetWindowText(_T(""));
	}
}

void DlgFFEdit::EnableGroupVacuum(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_VACUUM)->EnableWindow(bEnable);
}

void DlgFFEdit::EnableGroupBoss(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_BOSSAURA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_BAFV)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_BAR)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_BAFV)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_BAR)->EnableWindow(bEnable);
	AppointCoordinate(bEnable, IDC_RADIO_CORCAR);
	if(bEnable)
	{
		if (m_DoNotUpdateCTRLs)
			m_DoNotUpdateCTRLs = false;
		else
		{
			m_ToUpdateID = DDX_ID_BOSSAURA | DDX_ID_ONLY;
			UpdateData(false);
			UpdateWindow();
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_BAFV)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_BAR)->SetWindowText(_T(""));
	}
}

void DlgFFEdit::AppointCoordinate(const bool bEnable, int valCoor)
{
	assert(IDC_RADIO_CORCAR <= valCoor);
	assert(IDC_RADIO_CORTOR >= valCoor);
	if (bEnable)
	{
		nIDLastCoordinates = GetCheckedRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR);
		CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, valCoor);
		GetDlgItem(IDC_RADIO_CORCAR)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_CORSPH)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_CORCYL)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_CORTOR)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_RADIO_CORCAR)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_CORSPH)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_CORCYL)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_CORTOR)->EnableWindow(true);
		CheckRadioButton(IDC_RADIO_CORCAR, IDC_RADIO_CORTOR, nIDLastCoordinates);
	}
}

void DlgFFEdit::SetSpecialVortex(const bool bEnable)
{
	AppointCoordinate(bEnable, IDC_RADIO_CORCYL);
}

void DlgFFEdit::SetSpecialExplosion(const bool bEnable)
{
	AppointCoordinate(bEnable, IDC_RADIO_CORSPH);
	if (bEnable)
	{
		nIDLastShape = GetCheckedRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX);
		CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, IDC_RADIO_STSPHERE);
		GetDlgItem(IDC_RADIO_STBOX)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_STSPHERE)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_STCAPSULE)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_STCONVEX)->EnableWindow(false);
		OnRadioStsphere();
		GetDlgItem(IDC_EDIT_SSX)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_RADIO_STBOX)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_STSPHERE)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_STCAPSULE)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_STCONVEX)->EnableWindow(true);
		CheckRadioButton(IDC_RADIO_STBOX, IDC_RADIO_STCONVEX, nIDLastShape);
		switch (nIDLastShape)
		{
		case IDC_RADIO_STBOX:
			OnRadioStbox();
			break;
		case IDC_RADIO_STSPHERE:
			OnRadioStsphere();
			break;
		case IDC_RADIO_STCAPSULE:
			OnRadioStcapsule();
			break;
		}
		GetDlgItem(IDC_EDIT_SSX)->EnableWindow(true);
	}
}

void DlgFFEdit::OnBtnBindma() 
{
	// TODO: Add your control notification handler code here
	m_ToUpdateID = DDX_ID_FILENAME | DDX_ID_ONLY;
	m_pToUpdateVal = 0;
	UpdateData(true);
	UpdateWindow();

	m_CurBindingMA = m_strFileName;
	m_UpdateBindingFF = true;

	m_ToUpdateID = DDX_ID_FILENAME | DDX_ID_ONLY;
	m_pToUpdateVal = (float*)1;
	UpdateData(false);
	UpdateWindow();
}

bool DlgFFEdit::GetBindingFF(AString& outFF) const
{
	outFF = _TWC2AS(m_CurBindingMA);
	return m_UpdateBindingFF;
}
