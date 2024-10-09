// DlgFlags.cpp : implementation file
//

#include "stdafx.h"
#include "aphysxlab.h"
#include "DlgFlags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFlags dialog


CDlgFlags::CDlgFlags(CPhysXObjDynamic& rObj, const bool IsEditMode, APhysXScene* pPhysXScene, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFlags::IDD, pParent)
{
	assert(0 != pPhysXScene);
	IsEdit = IsEditMode;
	pObj = &rObj;
	pScene = pPhysXScene;
	UserSelectID = -1;
	//{{AFX_DATA_INIT(CDlgFlags)
	//}}AFX_DATA_INIT
}


void CDlgFlags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFlags)
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
		return;

	// Note: must set channel information at first!
	CPhysXObjDynamic::CollisionChannel cnl = CPhysXObjDynamic::CNL_UNKNOWN;
	const int nID = GetCheckedRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN);
	if (nID == IDC_RADIO_CNL_CMN)
		cnl = CPhysXObjDynamic::CNL_COMMON;
	else if (nID == IDC_RADIO_CNL_IDP)
		cnl = CPhysXObjDynamic::CNL_INDEPENDENT;
	else if (nID == IDC_RADIO_CNL_IDPCMN)
		cnl = CPhysXObjDynamic::CNL_INDEPENDENT_CCHOST;
	pObj->SetChannel(cnl);
	if (!IsEdit)
	{
		IActionPlayerBase* pAC = pObj->GetPlayAction();
		CPhysXObjForceField* pFF = pObj->GetAssociateFF();
		if ((0 != pFF) && (0 != pAC))
		{
			APhysXCCBase* pCCBase = pAC->GetAPhysXCCBase();
			if (0 != pCCBase)
			{
				assert(!"To update");
//				APhysXCollisionChannel hostCNL = pCCBase->GetHostModelChannel();
//				pFF->SetHostChannel(&hostCNL);
			}
		}
	}

	BOOL val = false;
	DDX_Check(pDX, IDC_CHECK_HASPB, val);
	pObj->ApplyPhysXBody(val? true : false, pScene);

	int PeopleFlags = 0;
	DDX_Check(pDX, IDC_CHECK_ISMA, val);
	if (val)
		PeopleFlags = IPhysXObjBase::OBPI_IS_MAIN_ACTOR;
	DDX_Check(pDX, IDC_CHECK_ISNPC, val);
	if (val)
		PeopleFlags = IPhysXObjBase::OBPI_IS_NPC;
	if (0 == PeopleFlags)
		PeopleFlags = IPhysXObjBase::OBPI_IS_NONE_PEOPLE;
	DDX_Check(pDX, IDC_CHECK_HASCC, val);
	if (val)
		PeopleFlags |= IPhysXObjBase::OBPI_HAS_APHYSX_CC;
	else
	{
		DDX_Check(pDX, IDC_CHECK_HASLWCC, val);
		if (val)
			PeopleFlags |= IPhysXObjBase::OBPI_HAS_APHYSX_LWCC;
	}

	if (IPhysXObjBase::OBPI_IS_NPC & PeopleFlags)
	{
		const int nID = GetCheckedRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS);
		if (nID == IDC_RADIO_DANCE)
			PeopleFlags |= IPhysXObjBase::OBPI_ACTION_DANCE;
		else if (nID == IDC_RADIO_WALKAROUND)
			PeopleFlags |= IPhysXObjBase::OBPI_ACTION_WALK_AROUND;
		else if (nID == IDC_RADIO_WALKCAMERA)
			PeopleFlags |= IPhysXObjBase::OBPI_ACTION_WALK;
		else if (nID == IDC_RADIO_WALKXAXIS)
			PeopleFlags |= IPhysXObjBase::OBPI_ACTION_WALK_XAXIS;
	}
	pObj->SetPeopleFlags(PeopleFlags, *pScene);

	int flags = 0;
	if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_IS_NONE_PEOPLE))
	{
		DDX_Check(pDX, IDC_CHECK_PF, val);
		if (val)	flags |= APX_OBJFLAG_CC_ADD_PUSHFORCE;
		DDX_Check(pDX, IDC_CHECK_SF, val);
		if (val)	flags |= APX_OBJFLAG_CC_ADD_STANDFORCE;
		DDX_Check(pDX, IDC_CHECK_CAA, val);
		if (val)	flags |= APX_OBJFLAG_CC_CAN_ATTACH;
		pObj->SetPMFlags(flags);
	}

	if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_CC))
	{
		flags = 0;
		DDX_Check(pDX, IDC_CHECK_CCSLIDE, val);
		if (val)	flags |= APhysXCharacterController::APX_CCF_SLIDE_ENABLE;
		DDX_Check(pDX, IDC_CHECK_CCPF, val);
		if (val)	flags |= APhysXCharacterController::APX_CCF_ADD_PUSHFORCE;
		DDX_Check(pDX, IDC_CHECK_CCSF, val);
		if (val)	flags |= APhysXCharacterController::APX_CCF_ADD_STANDFORCE;
		DDX_Check(pDX, IDC_CHECK_CCFO, val);
		if (val)	flags |= APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN;
		DDX_Check(pDX, IDC_CHECK_CCFC, val);
		if (val)	flags |= APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE;
		pObj->SetCCFlags(flags);
	}

}

BEGIN_MESSAGE_MAP(CDlgFlags, CDialog)
	//{{AFX_MSG_MAP(CDlgFlags)
	ON_BN_CLICKED(IDC_CHECK_HASCC, OnCheckHasCC)
	ON_BN_CLICKED(IDC_CHECK_HASLWCC, OnCheckHasLWCC)
	ON_BN_CLICKED(IDC_CHECK_ISNPC, OnCheckIsNPC)
	ON_BN_CLICKED(IDC_RADIO_CNL_CMN, OnRadioCNLCMN)
	ON_BN_CLICKED(IDC_RADIO_CNL_IDP, OnRadioCNLIDP)
	ON_BN_CLICKED(IDC_RADIO_CNL_IDPCMN, OnRadioCNLIDPCMN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFlags message handlers

BOOL CDlgFlags::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_IS_NONE_PEOPLE))
	{
		EnableGroupAPhysXObj(true);
		EnableGroupPeople(false);
		EnableGroupChannel(false, true);
		EnableGroupNPCAction(false);
		EnableGroupAPhysXCC(false);
	}
	else
	{
		EnableGroupAPhysXObj(false);
		EnableGroupPeople(true);
		const bool IsNPC = pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_IS_NPC);
		if (IsNPC)
			EnableGroupNPCAction(true);
		else
			EnableGroupNPCAction(false);
		EnableGroupChannel(true, true);
		if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_HAS_APHYSX_CC))
		{
			if (IsNPC)
				EnableGroupAPhysXCC(false, true);
			else
				EnableGroupAPhysXCC(true);
		}
		else
		{
			EnableGroupAPhysXCC(false);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFlags::EnableGroupAPhysXObj(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_MODEL)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_PF)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_SF)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_CAA)->EnableWindow(bEnable);

	int flags = 0;
	if (bEnable)
	{
		flags = pObj->GetPMFlags();
	}
	CheckDlgButton(IDC_CHECK_PF,  APX_OBJFLAG_CC_ADD_PUSHFORCE	& flags);
	CheckDlgButton(IDC_CHECK_SF,  APX_OBJFLAG_CC_ADD_STANDFORCE	& flags);
	CheckDlgButton(IDC_CHECK_CAA, APX_OBJFLAG_CC_CAN_ATTACH	& flags);
}

void CDlgFlags::EnableGroupPeople(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_PEOPLE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_ISMA)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_ISNPC)->EnableWindow(IsEdit);
	GetDlgItem(IDC_CHECK_HASCC)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_HASLWCC)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_HASPB)->EnableWindow(bEnable);

	if (bEnable)
	{
		int flags = pObj->GetPeopleInfo();
		CheckDlgButton(IDC_CHECK_ISMA,  IPhysXObjBase::OBPI_IS_MAIN_ACTOR & flags);
		CheckDlgButton(IDC_CHECK_ISNPC, IPhysXObjBase::OBPI_IS_NPC	 	  & flags);
		CheckDlgButton(IDC_CHECK_HASCC, IPhysXObjBase::OBPI_HAS_APHYSX_CC & flags);
		CheckDlgButton(IDC_CHECK_HASLWCC, IPhysXObjBase::OBPI_HAS_APHYSX_LWCC & flags);
	}
	CheckDlgButton(IDC_CHECK_HASPB, pObj->ReadFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY));
}

void CDlgFlags::EnableGroupChannel(const bool bEnable, const bool forceRefresh)
{
	GetDlgItem(IDC_STATIC_CHANNEL)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_CNL_CMN)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_CNL_IDP)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_CNL_IDPCMN)->EnableWindow(bEnable);

	const CPhysXObjDynamic::CollisionChannel cnl = pObj->GetChannel(forceRefresh);
	switch(cnl)
	{
	case CPhysXObjDynamic::CNL_UNKNOWN:
		CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, 0);
		break;
	case CPhysXObjDynamic::CNL_COMMON:
		CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, IDC_RADIO_CNL_CMN);
		break;
	case CPhysXObjDynamic::CNL_INDEPENDENT:
		CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, IDC_RADIO_CNL_IDP);
		break;
	case CPhysXObjDynamic::CNL_INDEPENDENT_CCHOST:
		CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, IDC_RADIO_CNL_IDPCMN);
		break;
	default:
		CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, 0);
		break;
	}

	if (forceRefresh)
		return;

	if (!bEnable)
		CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, IDC_RADIO_CNL_CMN);
	else
	{
		if (IDC_RADIO_CNL_CMN == GetCheckedRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN))
		{
			if (0 < UserSelectID)
				CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, UserSelectID);
			else
				CheckRadioButton(IDC_RADIO_CNL_CMN, IDC_RADIO_CNL_IDPCMN, IDC_RADIO_CNL_IDPCMN);
		}
	}
}

void CDlgFlags::EnableGroupNPCAction(const bool bEnable)
{
	GetDlgItem(IDC_STATIC_NPCACTION)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_DANCE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_WALKAROUND)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_WALKCAMERA)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_WALKXAXIS)->EnableWindow(bEnable);

	if (bEnable)
	{
		if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_ACTION_DANCE))
			CheckRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS, IDC_RADIO_DANCE);
		else if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_ACTION_WALK_AROUND))
			CheckRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS, IDC_RADIO_WALKAROUND);
		else if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_ACTION_WALK))
			CheckRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS, IDC_RADIO_WALKCAMERA);
		else if (pObj->ReadPeopleInfo(IPhysXObjBase::OBPI_ACTION_WALK_XAXIS))
			CheckRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS, IDC_RADIO_WALKXAXIS);
		else
			CheckRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS, IDC_RADIO_DANCE);
	}
	else
	{
		CheckRadioButton(IDC_RADIO_DANCE, IDC_RADIO_WALKXAXIS, 0);
	}
}

void CDlgFlags::EnableGroupAPhysXCC(const bool bEnable, const bool SetDefVal)
{
	GetDlgItem(IDC_STATIC_CC)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_CCSLIDE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_CCPF)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_CCSF)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_CCFO)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_CCFC)->EnableWindow(bEnable);

	if (bEnable)
	{
		int flags = pObj->GetCCFlags();
		CheckDlgButton(IDC_CHECK_CCSLIDE, APhysXCharacterController::APX_CCF_SLIDE_ENABLE & flags);
		CheckDlgButton(IDC_CHECK_CCPF, APhysXCharacterController::APX_CCF_ADD_PUSHFORCE	& flags);
		CheckDlgButton(IDC_CHECK_CCSF, APhysXCharacterController::APX_CCF_ADD_STANDFORCE & flags);
		CheckDlgButton(IDC_CHECK_CCFO, APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN & flags);
		CheckDlgButton(IDC_CHECK_CCFC, APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE & flags);
	}
	else
	{
		if (SetDefVal)
		{
			CheckDlgButton(IDC_CHECK_CCSLIDE, true);
			CheckDlgButton(IDC_CHECK_CCPF, true);
			CheckDlgButton(IDC_CHECK_CCSF, true);
		}
		else
		{
			CheckDlgButton(IDC_CHECK_CCSLIDE, false);
			CheckDlgButton(IDC_CHECK_CCPF, false);
			CheckDlgButton(IDC_CHECK_CCSF, false);
		}
		CheckDlgButton(IDC_CHECK_CCFO, false);
		CheckDlgButton(IDC_CHECK_CCFC, false);
	}
}

void CDlgFlags::OnCheckHasCC() 
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_HASCC))
	{
		CheckDlgButton(IDC_CHECK_HASLWCC, false);
		if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ISNPC))
			EnableGroupAPhysXCC(false, true);
		else
			EnableGroupAPhysXCC(true, false);
	}
	else
		EnableGroupAPhysXCC(false);
}

void CDlgFlags::OnCheckHasLWCC() 
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_HASLWCC))
	{
		CheckDlgButton(IDC_CHECK_HASCC, false);
		EnableGroupAPhysXCC(false);
	}
}

void CDlgFlags::OnCheckIsNPC() 
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ISNPC))
	{
		EnableGroupAPhysXObj(false);
		EnableGroupNPCAction(true);
		EnableGroupChannel(true);
		GetDlgItem(IDC_CHECK_HASCC)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_HASLWCC)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_HASPB)->EnableWindow(true);
	}
	else
	{
		EnableGroupAPhysXObj(true);
		EnableGroupNPCAction(false);
		const bool MAIsChecked = (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ISMA))? true : false;
		if (MAIsChecked)
			EnableGroupChannel(true);
		else
			EnableGroupChannel(false);
		GetDlgItem(IDC_CHECK_HASCC)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_HASLWCC)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_HASPB)->EnableWindow(false);

		CheckDlgButton(IDC_CHECK_HASCC, false);
		CheckDlgButton(IDC_CHECK_HASLWCC, false);
		EnableGroupAPhysXCC(false);
		CheckDlgButton(IDC_CHECK_HASPB, pObj->ReadFlag(IPhysXObjBase::OBF_ENABLE_PHYSX_BODY));
	}
}

void CDlgFlags::OnRadioCNLCMN() 
{
	// TODO: Add your control notification handler code here
	UserSelectID = IDC_RADIO_CNL_CMN;
}

void CDlgFlags::OnRadioCNLIDP() 
{
	// TODO: Add your control notification handler code here
	UserSelectID = IDC_RADIO_CNL_IDP;
}

void CDlgFlags::OnRadioCNLIDPCMN() 
{
	// TODO: Add your control notification handler code here
	UserSelectID = IDC_RADIO_CNL_IDPCMN;
}
