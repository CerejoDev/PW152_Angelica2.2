// APhysXLabView.cpp : implementation of the CAPhysXLabView class
//

#include "stdafx.h"
#include "APhysXLab.h"

#include "MainFrm.h"
#include "APhysXLabDoc.h"
#include "APhysXLabView.h"
#include "DlgPhysXTrnGen.h"

#include <CommonFileName.h>
#include "DlgFlags.h"
#include "DlgFFEdit.h"

#include "ChbEntityManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool gTestRiveFF;

/////////////////////////////////////////////////////////////////////////////
// CShotScale dialog used for dialog edit scale

class CShotScale : public CDialog
{
public:
	CShotScale(const float scale, const float speed, CWnd* pParent = NULL);    // standard constructor
	float GetScale() const { return m_scale; }
	float GetSpeed() const { return m_speed; }
	
	// Dialog Data
	//{{AFX_DATA(CShotScale)
	enum { IDD = IDD_SHOOTSCALE };
	CEdit	m_ScaleEdit;
	CEdit	m_SpeedEdit;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShotScale)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CShotScale)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	float m_scale;
	float m_speed;
};

CShotScale::CShotScale(const float scale, const float speed, CWnd* pParent /*=NULL*/)
	: CDialog(CShotScale::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShotScale)
	m_scale = scale;
	m_speed = speed;
	//}}AFX_DATA_INIT
}

void CShotScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShotScale)
	DDX_Text(pDX, IDC_SCALE, m_scale);
	DDX_Text(pDX, IDC_SPEED, m_speed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShotScale, CDialog)
//{{AFX_MSG_MAP(CShotScale)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CShotScale::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();

	if (0 > m_scale)
		m_scale *= -1;
	if (0.001 > m_scale)
		m_scale = 0.001f;
	if (0 > m_speed)
		m_speed *= -1;
}

/////////////////////////////////////////////////////////////////////////////
// CAPhysXLabView

IMPLEMENT_DYNCREATE(CAPhysXLabView, CView)

BEGIN_MESSAGE_MAP(CAPhysXLabView, CView)
	ON_COMMAND_RANGE(ID_FILE_RECENT0, ID_FILE_RECENT4, OnFileRecent)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_RECENT0, ID_FILE_RECENT4, OnUpdateFileRecent)
	ON_COMMAND(ID_FILE_CHF2CHB, OnFileChf2Chb)
	ON_COMMAND(ID_EDIT_ADDCHB, OnEditAddChb)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDCHB, OnUpdateEditAddChb)
	//{{AFX_MSG_MAP(CAPhysXLabView)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DRAGOPT, OnUpdateDragOpt)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DRVMODE, OnUpdateDrvMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CURSTATE, OnUpdateCurrentState)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SHOOTMODE, OnUpdateShootMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SHOOTTYPE, OnUpdateShootType)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_EDIT_ADDTERRAIN, OnEditAddterrain)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDTERRAIN, OnUpdateEditAddterrain)
	ON_COMMAND(ID_EDIT_PHYSXTRNGEN, OnEditPhysxtrngen)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHYSXTRNGEN, OnUpdateEditPhysxtrngen)
	ON_COMMAND(ID_EDIT_PHYSXFFMAKER, OnEditPhysxffmaker)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHYSXFFMAKER, OnUpdateEditPhysxffmaker)
	ON_COMMAND(ID_EDIT_ADDSTATICOBJ, OnEditAddstaticobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSTATICOBJ, OnUpdateEditAddstaticobj)
	ON_COMMAND(ID_EDIT_ADDSMD, OnEditAddsmd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSMD, OnUpdateEditAddsmd)
	ON_COMMAND(ID_EDIT_ADDECM, OnEditAddsmdEcm)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDECM, OnUpdateEditAddsmdEcm)
	ON_COMMAND(ID_EDIT_ADDPHYSXGFX, OnEditAddphysxgfx)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPHYSXGFX, OnUpdateEditAddphysxgfx)
	ON_COMMAND(ID_EDIT_ADDPHYSXFF, OnEditAddphysxff)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPHYSXFF, OnUpdateEditAddphysxff)
	ON_COMMAND(ID_EDIT_ADDLASTOBJ, OnEditAddlastobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDLASTOBJ, OnUpdateEditAddlastobj)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_EDIT_DRAGANDMOVE, OnEditDragAndMove)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DRAGANDMOVE, OnUpdateEditDragAndMove)
	ON_COMMAND(ID_EDIT_DRAGANDROTATE, OnEditDragAndRotate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DRAGANDROTATE, OnUpdateEditDragAndRotate)
	ON_COMMAND(ID_EDIT_DRAGANDSCALE, OnEditDragAndScale)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DRAGANDSCALE, OnUpdateEditDragAndScale)
	ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
	ON_COMMAND(ID_EDIT_SCALE, OnEditScale)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SCALE, OnUpdateEditScale)
	ON_COMMAND(ID_MODE_SIMULATE, OnModeSimulate)
	ON_UPDATE_COMMAND_UI(ID_MODE_SIMULATE, OnUpdateModeSimulate)
	ON_COMMAND(ID_MODE_PLAY, OnModePlay)
	ON_UPDATE_COMMAND_UI(ID_MODE_PLAY, OnUpdateModePlay)
	ON_COMMAND(ID_MODE_PX_PROFILE, OnModePxProfile)
	ON_UPDATE_COMMAND_UI(ID_MODE_PX_PROFILE, OnUpdateModePxProfile)
	ON_COMMAND(ID_MODE_PX_DEBUG, OnModePxDebug)
	ON_UPDATE_COMMAND_UI(ID_MODE_PX_DEBUG, OnUpdateModePxDebug)
	ON_COMMAND(ID_MODE_PHYSXHW, OnModePhysxhw)
	ON_UPDATE_COMMAND_UI(ID_MODE_PHYSXHW, OnUpdateModePhysxhw)
	ON_COMMAND(ID_VIEW_WIRE, OnViewWire)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIRE, OnUpdateViewWire)
	ON_COMMAND(ID_MODE_DVN_ANIMATION, OnModeDvnAnimation)
	ON_UPDATE_COMMAND_UI(ID_MODE_DVN_ANIMATION, OnUpdateModeDvnAnimation)
	ON_COMMAND(ID_MODE_DVN_PARTPHYSX, OnModeDvnPartphysx)
	ON_UPDATE_COMMAND_UI(ID_MODE_DVN_PARTPHYSX, OnUpdateModeDvnPartphysx)
	ON_COMMAND(ID_MODE_DVN_PUREPHYSX, OnModeDvnPurephysx)
	ON_UPDATE_COMMAND_UI(ID_MODE_DVN_PUREPHYSX, OnUpdateModeDvnPurephysx)
	ON_COMMAND(ID_MODE_C_FOLLOWING, OnModeCFollowing)
	ON_UPDATE_COMMAND_UI(ID_MODE_C_FOLLOWING, OnUpdateModeCFollowing)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_MENU_EDITMOD, OnMenuEditmod)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAPhysXLabView construction/destruction
const CString CAPhysXLabView::NoName = "无标题";

CAPhysXLabView::CAPhysXLabView() : m_render(g_Render), m_game(g_Game)
{
	// TODO: add construction code here
	m_bIsAccumulating = false;
	m_bIsRotation = false;
	m_bIsMovement = false;
	m_bIsDragging = false;
	m_bIsRBClick = false;
	m_ptMouseLast.x = 0;
	m_ptMouseLast.y = 0;
	m_ShotScale = 1.0;
	m_ShotSpeed = 20.0;

	m_dwRenderStart	= 0;
	m_dwLastRender	= 0;
}

CAPhysXLabView::~CAPhysXLabView()
{
}

BOOL CAPhysXLabView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAPhysXLabView drawing

void CAPhysXLabView::OnDraw(CDC* pDC)
{
	CAPhysXLabDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAPhysXLabView diagnostics

#ifdef _DEBUG
void CAPhysXLabView::AssertValid() const
{
	CView::AssertValid();
}

void CAPhysXLabView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAPhysXLabDoc* CAPhysXLabView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAPhysXLabDoc)));
	return (CAPhysXLabDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAPhysXLabView message handlers

void CAPhysXLabView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	if (m_render.EngineIsReady())
		return;

	if (!m_render.Init(AfxGetInstanceHandle(), m_hWnd, false))
	{
		a_LogOutput(1, "CAPhysXLabView::OnInitialUpdate: Failed to create render!");
		return;
	}

	CCoordinateDirection::g_Render = &m_render;
	IPhysXObjBase::pWCTopZBuf = m_render.GetWCTopZBUf();
	if (!m_game.Init(m_render))
		return;

	m_bIsRotation = true;
	m_bIsMovement = false;
	m_game.SetDragAndDropRotate();
	m_game.EnablePhysXProfile(false);

	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (0 != pFrame)
		m_game.SetMABindingFF(AString(_TWC2AS(pFrame->m_FFbindingToMA)));
}

void CAPhysXLabView::OnDestroy() 
{
	CView::OnDestroy();
	// TODO: Add your message handler code here
	m_game.Release();
	m_render.Release();
}

void CAPhysXLabView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (0 == cx || 0 == cy)
		return;

	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;
	
	m_render.ResizeDevice(cx, cy);
	CCameraController* pCam = m_game.GetCameraController();
	if (0 != pCam)
		pCam->SetScreenCenterPos(cx / 2, cy / 2);
}

bool CAPhysXLabView::Tick(const DWORD dwTime)
{
	if (0 == m_game.GetTerrain())
		return true;
	
	return	m_game.Tick(dwTime);
}

bool CAPhysXLabView::Render()
{
	m_dwRenderStart = a_GetTime();
	
	if (!m_render.BeginRender())
	{
		m_dwLastRender = a_GetTime() - m_dwRenderStart;
		return false;
	}
	
	m_render.ClearViewport();
	m_game.Render();
	DrawPrompts();
	
	m_render.EndRender();
	
	m_dwLastRender = a_GetTime() - m_dwRenderStart;
	return true;
}

void CAPhysXLabView::OnUpdateDragOpt(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(); 
    pCmdUI->SetText(GetDragOptText()); 
}

void CAPhysXLabView::OnUpdateDrvMode(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(); 
    pCmdUI->SetText(GetDrvModeText()); 
}

void CAPhysXLabView::OnUpdateCurrentState(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(); 
    pCmdUI->SetText(GetCurrentStateText()); 
}

void CAPhysXLabView::OnUpdateShootMode(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(); 
    pCmdUI->SetText(GetShootModeText()); 
}

void CAPhysXLabView::OnUpdateShootType(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(); 
    pCmdUI->SetText(GetShootTypeText()); 
}

LPCTSTR CAPhysXLabView::GetDragOptText() const
{
	if (m_bIsMovement)
		return _T("拖拽效果(F8): 平移");

	if (m_bIsRotation)
		return _T("拖拽效果(F8): 旋转");

	return _T("拖拽效果(F8): 缩放");
}

LPCTSTR CAPhysXLabView::GetDrvModeText() const
{
	CGame::DrivenMode dm = m_game.GetDrivenMode();

	if (IPhysXObjBase::DRIVEN_BY_ANIMATION == dm)
		return _T("驱动模式(F4): 动画");
	
	if (IPhysXObjBase::DRIVEN_BY_PURE_PHYSX == dm)
		return _T("驱动模式(F4): 物理");
	
	return _T("驱动模式(F4): 混合");
}

LPCTSTR CAPhysXLabView::GetCurrentStateText() const
{
	if (0 == m_game.GetTerrain())
		return _T("当前状态: 空闲");

	switch(m_game.GetGameMode())
	{
	case CGame::GAME_MODE_EDIT:
		return _T("当前状态: 编辑");
	case CGame::GAME_MODE_SIMULATE:
		return _T("当前状态: 模拟");
	case CGame::GAME_MODE_PLAY:
		return _T("当前状态: 游戏");
	}

	return _T("当前状态: 未知");
}

LPCTSTR CAPhysXLabView::GetShootModeText() const
{
	CGame::ShootMode ssm = m_game.GetShootMode();
	
	switch(ssm)
	{
	case CGame::SM_EYE_TO_CENTER:
		return _T("弹物模式(9): 随摄像机");
	case CGame::SM_EYE_TO_MOUSE:
		return _T("弹物模式(9): 弹向鼠标");
	case CGame::SM_MOUSE_FALLING:
		return _T("弹物模式(9): 鼠标落体");
	case CGame::SM_FRONT_FALLING:
		return _T("弹物模式(9): 前方落体");
	case CGame::SM_EACH_ECM:
		return _T("弹物模式(9): ECM轮询");
	case CGame::SM_FIXED_POINT_1:
		return _T("弹物模式(9): 定点1");
	case CGame::SM_FIXED_POINT_2:
		return _T("弹物模式(9): 定点2");
	case CGame::SM_KINEMATIC:
		return _T("弹物模式(9): Kinematic");
	}
	
	return _T("弹物模式(9): 未知");
}

LPCTSTR CAPhysXLabView::GetShootTypeText() const
{
	CGame::ShootStuffID ssi = m_game.GetShootType();
	
	switch(ssi)
	{
	case CGame::SSI_SPHERE:
		return _T("弹物类别(8): 小球");
	case CGame::SSI_BOX:
		return _T("弹物类别(8): 箱子");
	case CGame::SSI_BARREL:
		return _T("弹物类别(8): 木桶");
	case CGame::SSI_BREAKABLEBOX:
		return _T("弹物类别(8): 破箱");
	case CGame::SSI_BOMB:
		return _T("弹物类别(8): 炸弹");
	}
	
	return _T("弹物类别(8): 未知");
}

void CAPhysXLabView::ChangeNextDragOperation()
{
	if (m_bIsDragging)
		return;

	if (m_bIsMovement)
		OnEditDragAndScale();
	else if (m_bIsRotation)
		OnEditDragAndMove();
	else
		OnEditDragAndRotate();
}

void CAPhysXLabView::ChangeNextDriveMode()
{
	CGame::DrivenMode dm = m_game.GetDrivenMode();

	if (IPhysXObjBase::DRIVEN_BY_ANIMATION == dm)
		OnModeDvnPartphysx();
	else if (IPhysXObjBase::DRIVEN_BY_PART_PHYSX == dm)
		OnModeDvnPurephysx();
	else
		OnModeDvnAnimation();
}

void CAPhysXLabView::DrawPrompts()
{
	RECT rcClient;
	GetClientRect(&rcClient);
	
	int x = rcClient.left + 10;
	int y = rcClient.bottom - 24;
	DWORD dwColor = 0xff00ff00;
	ACString str;
	
	//	Camera position
	CCameraController* pCam = m_game.GetCameraController();
	if (0 != pCam)
	{
		A3DVECTOR3 vPos = pCam->GetPos();
		if (pCam->QueryMode(CCameraController::MODE_BINDING))
			str.Format(_T("Camera(binding mode): %.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		else
			str.Format(_T("Camera(free fly mode): %.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		m_render.TextOut(x, y, str, dwColor);
	}
	y -= 16;
	
	//	Render time of last frame
	str.Format(_T("Render time: %d"), m_dwLastRender);
	m_render.TextOut(x, y, str, dwColor);
	y -= 16;

	//collision model
	str.Format(_T("collision model: %s"), m_game.GetCCName());
	m_render.TextOut(x, y, str, dwColor);
	y -= 16;

	//	Physical state
	IPhysXObjBase::DrivenMode iState = m_game.GetDrivenMode();
	if (iState == IPhysXObjBase::DRIVEN_BY_PURE_PHYSX)
		str = _T("Pure PhysX Simulation");
	else if (iState == IPhysXObjBase::DRIVEN_BY_ANIMATION)
		str = _T("Pure Animation");
	else if (iState = IPhysXObjBase::DRIVEN_BY_PART_PHYSX)
		str = _T("Partial PhysX Simulation");
	else
		assert(!"Unknown Driven Mode!");
	
	m_render.TextOut(x, y, str, dwColor);
	y -= 16;
}

void CAPhysXLabView::OnAddOperation(const ObjID objType)
{
	const CPhysXObjMgr::ObjInfo& objInfo = CPhysXObjMgr::GetObjInfo(objType);
		
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, objInfo.strWCExt, NULL, dwFlags, objInfo.strFilter, NULL);
	FileDia.m_ofn.lpstrInitialDir = _TAS2WC(af_GetBaseDir());
	if (FileDia.DoModal() != IDOK)
		return;
	
	// we use the united relative path to avoid the same model has different path name.
	AString strFilePath;
	af_GetRelativePath(_TWC2AS(FileDia.GetPathName()), strFilePath);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	IPhysXObjBase* pObject = m_game.CreateObject(objType, strFilePath, pt);
	if (0 == pObject)
	{
		MessageBox(_T("加载数据失败!"));
		return;
	}

	m_LastAddedItem.nTypeID = pObject->GetObjType();
	m_LastAddedItem.strPathName = strFilePath;
}

void CAPhysXLabView::SceneOpenOrSaveAs(const bool bIsOpen)
{
	BOOL bISOpen = bIsOpen? TRUE : FALSE;
	DWORD dwFlags =  OFN_EXPLORER | OFN_NOCHANGEDIR;
	if (bIsOpen)
		dwFlags |= OFN_FILEMUSTEXIST;

	TCHAR szFilter[] = _T("PDS 文件(*.pds)|*.pds||");
	CFileDialog	FileDia(bISOpen, _T("*.pds"), NULL, dwFlags, szFilter, NULL);
	ACString strPath = _TAS2WC(af_GetBaseDir());
	strPath += _T("\\Scenes\\");
	FileDia.m_ofn.lpstrInitialDir = strPath;
	
	m_game.GetCameraController()->EnableMoveCamera(false);
	if (IDOK != FileDia.DoModal())
	{
		m_game.GetCameraController()->EnableMoveCamera(true);
		return;
	}
	m_game.GetCameraController()->EnableMoveCamera(true);

	if (bIsOpen)
		SceneOpen(FileDia.GetPathName(), FileDia.GetFileTitle());
	else
	{
		m_game.SavePhysXDemoScene(_TWC2AS(FileDia.GetPathName()));
		GetDocument()->SetTitle(FileDia.GetFileTitle());
		CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
		if (0 != pFrame)
			pFrame->AddToRecent(FileDia.GetPathName());
	}
}

void CAPhysXLabView::SceneOpen(const CString& path, const CString& title)
{
	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (!CFile::GetStatus(path, CFileStatus()))
	{
		if (0 != pFrame)
			pFrame->DelFromRecent(path);
		return;
	}
	
	if(m_game.LoadPhysXDemoScene(_TWC2AS(path)))
	{
		m_bIsRotation = true;
		m_bIsMovement = false;
		GetDocument()->SetTitle(title);
		if (0 != pFrame)
			pFrame->AddToRecent(path);
	}
	else
	{
		if (0 != pFrame)
			pFrame->DelFromRecent(path);
	}
}

bool CAPhysXLabView::EnableEditState() const
{
	if (0 == m_game.GetTerrain())
		return false;
	
	if (CGame::GAME_MODE_EDIT != m_game.GetGameMode())
		return false;

	return true;
}

bool CAPhysXLabView::EnableAddObject() const
{
	if (!EnableEditState())
		return false;

	if (0 != m_game.GetPickedObject())
		return false;

	return true;
}

void CAPhysXLabView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_game.OnLButtonDown(point.x, point.y, nFlags);
	CView::OnLButtonDown(nFlags, point);
}

void CAPhysXLabView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (CGame::GAME_MODE_EDIT == m_game.GetGameMode())
		m_bIsDragging = false;

	m_game.OnLButtonUp(point.x, point.y, nFlags);
	CView::OnLButtonUp(nFlags, point);
}

void CAPhysXLabView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (CGame::GAME_MODE_SIMULATE == m_game.GetGameMode() ||
		CGame::GAME_MODE_PLAY == m_game.GetGameMode()
		)
	{
		SetCapture();
		if (MK_CONTROL & nFlags)
		{
			m_render.SetFrontSight(point.x, point.y, 8, 0xff00ff00);
		}
		else if (MK_SHIFT & nFlags)
		{
			m_render.ClearFrontSight();
		}
		else
		{
			m_render.DrawForceBar(point.x, point.y, 0xff00ff00, true);
			m_bIsAccumulating = true;
		}
	}
	CView::OnMButtonDown(nFlags, point);
}

void CAPhysXLabView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (CGame::GAME_MODE_SIMULATE == m_game.GetGameMode() || 
		CGame::GAME_MODE_PLAY == m_game.GetGameMode()   
		)
	{
		if (m_bIsAccumulating)
		{
			m_bIsAccumulating = false;
			m_game.OnMButtonUp(point.x, point.y, nFlags);
		}
		ReleaseCapture();
	}
	CView::OnMButtonUp(nFlags, point);
}

void CAPhysXLabView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_ptMouseLast = point;
	m_bIsRBClick = true;
	SetCapture();
	CView::OnRButtonDown(nFlags, point);
}

void CAPhysXLabView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	if (CGame::GAME_MODE_EDIT == m_game.GetGameMode())
	{
		IPhysXObjBase* pObj = m_game.GetHitObject();
		if (0 != pObj)
		{
			if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD == pObj->GetObjType())
			{
				CMenu menu;
				menu.LoadMenu(IDR_MENU_POP);
				CMenu* pSM = menu.GetSubMenu(0);
				if ( 0 != pSM)
				{
					CPoint pt;
					GetCursorPos(&pt);
					pSM->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
				}
			}
		}
	}
	else if (CGame::GAME_MODE_PLAY == m_game.GetGameMode())
	{
		if (m_bIsRBClick && (MK_SHIFT & nFlags))
		{
			PhysRay ray = ScreenPosToRay(*m_render.GetViewport(), point.x, point.y);
			m_game.TrytoHang(ray);
		}
	}
	m_bIsRBClick = false;
	CView::OnRButtonUp(nFlags, point);
}

void CAPhysXLabView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bIsRBClick = false;
	bool IsRunTime = false;
	if (CGame::GAME_MODE_EDIT != m_game.GetGameMode())
		IsRunTime = true;

	IPhysXObjBase* pObjBackUp = m_game.GetHitObject();
 
	PhysRay ray = ScreenPosToRay(*m_render.GetViewport(), point.x, point.y);
	PhysRayTraceHit hitInfo;
	m_game.RayTraceObject(ray, hitInfo);

	IPhysXObjBase* pObj = m_game.GetHitObject();
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = dynamic_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			bool isEditMode = false;
			if (CGame::GAME_MODE_EDIT == m_game.GetGameMode())
				isEditMode = true;

			bool refBackUp = false;
			if (IsRunTime)
			{
				refBackUp = pDynObj->ReadFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
				pDynObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
			}
			
			bool IsPause = m_game.GetPhysXPauseState();
			if (!IsPause)
				m_game.ChangePhysXPauseState();
			CDlgFlags flagsDlg(*pDynObj, isEditMode, m_game.GetAPhysXScene());
			flagsDlg.DoModal();
			if (!IsPause)
				m_game.ChangePhysXPauseState();
			if (refBackUp)
				pDynObj->RaiseFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
		}
	}

	m_game.SetHitObject(pObjBackUp);
	CView::OnRButtonDblClk(nFlags, point);
}

void CAPhysXLabView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	// Camera view rotation
	if (MK_RBUTTON & nFlags)
	{
		CSize dis = point - m_ptMouseLast;
		m_game.GetCameraController()->RotateCamera(dis.cx, dis.cy, m_game.GetTerrain());
		m_ptMouseLast = point;

		m_bIsRBClick = false;
		CView::OnMouseMove(nFlags, point);
		return;
	}

	if (MK_MBUTTON & nFlags)
	{
		if (m_bIsAccumulating)
			m_render.DrawForceBar(point.x, point.y, 0xff00ff00);
		else if (MK_CONTROL & nFlags)
			m_render.SetFrontSight(point.x, point.y, 8, 0xff00ff00);

		CView::OnMouseMove(nFlags, point);
		return;
	}

	if ((MK_LBUTTON & nFlags) && (0 != m_game.GetHitObject()))
	{
		if (CGame::GAME_MODE_EDIT == m_game.GetGameMode())
			m_bIsDragging = true;
	}

	m_game.OnMouseMove(point.x, point.y, nFlags);
	CView::OnMouseMove(nFlags, point);
}

BOOL CAPhysXLabView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	m_game.OnMouseWheel(pt.x, pt.y, zDelta, nFlags);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CAPhysXLabView::OnFileReset() 
{
	// TODO: Add your command handler code here
	if (IDNO == MessageBox(_T("确定要重置系统么？"), _T("提示"), MB_YESNO))
		return;

	m_game.Reset();	
	GetDocument()->SetTitle(NoName);
}

static AString CStr2AStr(CString str)
{
#ifdef _UNICODE
	int bfsize = a_WideCharToMultiByte(str.GetBuffer(0), str.GetLength(), 0, 0);
	char *pstr;
	pstr = new char[bfsize + 1];
	a_WideCharToMultiByte(str.GetBuffer(0), str.GetLength(), pstr, bfsize);
	pstr[bfsize] = 0;
	AString astr = pstr;
	delete[] pstr;
	return astr;
#else
	return AString(str);
#endif

}


void CAPhysXLabView::OnFileChf2Chb()
{
	const int BufferSize = 1000;
	ACHAR Buffer[BufferSize];
	if (GetCurrentDirectory(BufferSize, Buffer) == 0)
		Buffer[0] = 0;

	static ACHAR szFilter[] = _AL("Convex Hull Files (*.chf)|*.chf|All Files (*.*)|*.*||");
	CFileDialog fd(TRUE, NULL, NULL, OFN_HIDEREADONLY
		| OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilter, this);
	
	//delete []fd.m_ofn.lpstrFile;
	const int BufSize = 12800;
	fd.m_ofn.lpstrFile = new ACHAR[BufSize];
	memset(fd.m_ofn.lpstrFile, 0, BufSize);
	fd.m_ofn.nMaxFile = BufSize;
	
	if( IDCANCEL == fd.DoModal()) 
	{
		delete []fd.m_ofn.lpstrFile;
		return;
	}
	
	POSITION pos = fd.GetStartPosition();
	CString strFile = fd.GetNextPathName(pos);
	AString folder;
	
	AString astrFile = CStr2AStr(strFile);
	af_GetFilePath(astrFile, folder);	

	ALog log;
	log.SetLogDir(folder);
	log.Init("chf2chb.log", "chf to chb");
	
	int total = 0, succeed = 0;
	
	AString re = CHBasedCD::CChbEntityManager::Chf2Chb(astrFile)?AString(" ok"):AString(" fail");
	if (re == AString(" ok"))
		succeed++;
	total++;
	
	log.LogString(astrFile + re);
	
	while(pos)
	{
		strFile = fd.GetNextPathName(pos);

		astrFile = CStr2AStr(strFile);
		re = CHBasedCD::CChbEntityManager::Chf2Chb(astrFile)?AString(" ok"):AString(" fail");
		log.LogString(astrFile + re);
		if (re == AString(" ok"))
			succeed++;
		total++;
	}
	log.Release();
	delete []fd.m_ofn.lpstrFile;
	fd.m_ofn.lpstrFile = NULL;
	
	SetCurrentDirectory(Buffer);

	CString str;
	str.Format(_T("chf文件转换已转换为同目录的同名chb文件,%d/%d个文件完成转换，详见chf2chb.log"), succeed, total);
	MessageBox(str, _T("文件转换完成"));
}

void CAPhysXLabView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	SceneOpenOrSaveAs(true);
}

void CAPhysXLabView::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (CGame::GAME_MODE_SIMULATE == m_game.GetGameMode())
		pCmdUI->Enable(FALSE);
	else if (CGame::GAME_MODE_PLAY == m_game.GetGameMode())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CAPhysXLabView::OnFileSave() 
{
	// TODO: Add your command handler code here
	if (IDYES == AfxMessageBox(_T("确定覆盖当前文档？"), MB_YESNO | MB_ICONQUESTION))
	{
		CString path;
		CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		if (0 != pFrame)
		{
			if (pFrame->GetCurrentString(path))
				m_game.SavePhysXDemoScene(_TWC2AS(path));
		}
	}
}

void CAPhysXLabView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableEditState()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	SceneOpenOrSaveAs(false);
}

void CAPhysXLabView::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableEditState()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnFileRecent(UINT nID) 
{
	// TODO: Add your command handler code here
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	if (0 != pFrame)
	{
		if (pFrame->IsCurrentID(nID))
		{
			if (NoName != GetDocument()->GetTitle())
				return;
		}

		CString path, title;
		if (pFrame->GetRecentString(nID, path, &title))
			SceneOpen(path, title);
	}
}

void CAPhysXLabView::OnUpdateFileRecent(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	OnUpdateFileOpen(pCmdUI);
}

void CAPhysXLabView::OnEditAddterrain() 
{
	// TODO: Add your command handler code here
	CCommonFileName FileDlg(m_render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("trn2");
	if (FileDlg.DoModal() != IDOK)
		return;
	
	AString strFile = FileDlg.GetRelativeFileName();
	m_game.LoadTerrainAndPhysXTerrain(strFile);
}

void CAPhysXLabView::OnUpdateEditAddterrain(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(true);
}

void CAPhysXLabView::OnEditPhysxtrngen() 
{
	// TODO: Add your command handler code here
	CDlgPhysXTrnGen dlg(m_game.GetTerrainFile(), *m_render.GetA3DEngine());
	dlg.DoModal();
}

void CAPhysXLabView::OnUpdateEditPhysxtrngen(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}


void CAPhysXLabView::OnEditPhysxffmaker() 
{
	// TODO: Add your command handler code here
	AString ff;
	m_game.GetMABindingFF(ff);
	DlgFFEdit dlg(ff);
	dlg.DoModal();

	m_game.ReloadFFFile(dlg.GetSavedFile());
	if (dlg.GetBindingFF(ff))
	{
		m_game.SetMABindingFF(ff);
		CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
		if (0 != pFrame)
			pFrame->m_FFbindingToMA = ff;
	}
}

void CAPhysXLabView::OnUpdateEditPhysxffmaker(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CAPhysXLabView::OnEditAddstaticobj() 
{
	// TODO: Add your command handler code here
	OnAddOperation(CPhysXObjMgr::OBJ_TYPEID_STATIC);
}

void CAPhysXLabView::OnUpdateEditAddstaticobj(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnEditAddsmd() 
{
	// TODO: Add your command handler code here
	OnAddOperation(CPhysXObjMgr::OBJ_TYPEID_SKINMODEL);
}

void CAPhysXLabView::OnUpdateEditAddsmd(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnEditAddsmdEcm() 
{
	// TODO: Add your command handler code here
	OnAddOperation(CPhysXObjMgr::OBJ_TYPEID_ECMODEL);
}

void CAPhysXLabView::OnUpdateEditAddsmdEcm(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnEditAddphysxgfx()
{
	// TODO: Add your command handler code here
	OnAddOperation(CPhysXObjMgr::OBJ_TYPEID_PARTICLE);
}

void CAPhysXLabView::OnUpdateEditAddphysxgfx(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnEditAddphysxff() 
{
	// TODO: Add your command handler code here
	OnAddOperation(CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD);
}

void CAPhysXLabView::OnUpdateEditAddphysxff(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnEditAddlastobj() 
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	m_game.CreateObject(ObjID(m_LastAddedItem.nTypeID), m_LastAddedItem.strPathName, pt);
}

void CAPhysXLabView::OnUpdateEditAddlastobj(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (0 >= m_LastAddedItem.nTypeID)
		pCmdUI->Enable(FALSE);
	else
		EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

BOOL CAPhysXLabView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if ((HTCLIENT != nHitTest) || (!m_render.EngineIsReady()))
		return CView::OnSetCursor(pWnd, nHitTest, message);

	if (m_render.OnSetCursor())
		return TRUE;

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CAPhysXLabView::OnEditDragAndMove() 
{
	// TODO: Add your command handler code here
	m_bIsMovement = true;
	m_bIsRotation = false;
	m_game.SetDragAndDropMove();
}

void CAPhysXLabView::OnUpdateEditDragAndMove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableEditState()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bIsMovement);
}

void CAPhysXLabView::OnEditDragAndRotate() 
{
	// TODO: Add your command handler code here
	m_bIsRotation = true;
	m_bIsMovement = false;
	m_game.SetDragAndDropRotate();
}

void CAPhysXLabView::OnUpdateEditDragAndRotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableEditState()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bIsRotation);
}

void CAPhysXLabView::OnEditDragAndScale() 
{
	// TODO: Add your command handler code here
	m_bIsRotation = false;
	m_bIsMovement = false;
	m_game.SetDragAndDropScale();
}

void CAPhysXLabView::OnUpdateEditDragAndScale(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	EnableEditState()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bIsRotation == m_bIsMovement);
}

void CAPhysXLabView::OnModeEdit() 
{
	// TODO: Add your command handler code here
	m_game.SetGameMode(CGame::GAME_MODE_EDIT);
}

void CAPhysXLabView::OnUpdateModeEdit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (0 == m_game.GetTerrain())
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}

	bool bIsEdit(CGame::GAME_MODE_EDIT == m_game.GetGameMode());
	pCmdUI->SetCheck(bIsEdit);
	pCmdUI->Enable(!bIsEdit);
}

void CAPhysXLabView::OnEditScale() 
{
	// TODO: Add your command handler code here
	bool NeedRecover = false;
	if (!m_game.GetPhysXPauseState())
	{
		NeedRecover = true;
		m_game.ChangePhysXPauseState();
	}

	CShotScale scaleDlg(m_ShotScale, m_ShotSpeed);
	scaleDlg.DoModal();

	if (NeedRecover)
		m_game.ChangePhysXPauseState();

	m_ShotScale = scaleDlg.GetScale();
	m_ShotSpeed = scaleDlg.GetSpeed();
}

void CAPhysXLabView::OnUpdateEditScale(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(true);
}

void CAPhysXLabView::OnModeSimulate() 
{
	// TODO: Add your command handler code here
	m_game.SetGameMode(CGame::GAME_MODE_SIMULATE);
}

void CAPhysXLabView::OnUpdateModeSimulate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CGame::GameMode mode = m_game.GetGameMode();
	if ((0 == m_game.GetTerrain()) || (CGame::GAME_MODE_PLAY == mode)) 
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}
	else if (CGame::GAME_MODE_SIMULATE == mode)
	{
		pCmdUI->SetCheck(TRUE);
		pCmdUI->Enable(FALSE);
	}
	else if (CGame::GAME_MODE_EDIT == mode)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(TRUE);
	}
	else
		assert(!"Oops! Unknown state.");
}

void CAPhysXLabView::OnModePlay() 
{
	// TODO: Add your command handler code here
	m_game.SetGameMode(CGame::GAME_MODE_PLAY);
}

void CAPhysXLabView::OnUpdateModePlay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CGame::GameMode mode = m_game.GetGameMode();
	if ((0 == m_game.GetTerrain()) || (CGame::GAME_MODE_SIMULATE == mode)) 
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}
	else if (CGame::GAME_MODE_PLAY == mode)
	{
		pCmdUI->SetCheck(TRUE);
		pCmdUI->Enable(FALSE);
	}
	else if (CGame::GAME_MODE_EDIT == mode)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(TRUE);
	}
	else
		assert(!"Oops! Unknown state.");
}

void CAPhysXLabView::OnModePxProfile() 
{
	// TODO: Add your command handler code here
	m_game.EnablePhysXProfile(!m_game.IsPhysXProfileEnabled());
}

void CAPhysXLabView::OnUpdateModePxProfile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_game.IsPhysXProfileEnabled());
}

void CAPhysXLabView::OnModePxDebug() 
{
	// TODO: Add your command handler code here
	m_game.EnablePhysXDebugRender(!m_game.IsPhysXDebugRenderEnabled());
}

void CAPhysXLabView::OnUpdateModePxDebug(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_game.IsPhysXDebugRenderEnabled());
}

void CAPhysXLabView::OnModePhysxhw() 
{
	// TODO: Add your command handler code here
	m_game.EnablePhysXHW(!m_game.IsPhysXHWEnable());
}

void CAPhysXLabView::OnUpdateModePhysxhw(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_game.IsPhysXHWEnable());
	CGame::GameMode mode = m_game.GetGameMode();
	if (CGame::GAME_MODE_EDIT == mode)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLabView::OnViewWire() 
{
	// TODO: Add your command handler code here
	m_game.SetTerrianGridShow(!m_game.GetTerrianGridShow());
}

void CAPhysXLabView::OnUpdateViewWire(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	(0 == m_game.GetTerrain())? pCmdUI->Enable(FALSE) : pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_game.GetTerrianGridShow());
}

void CAPhysXLabView::OnModeDvnAnimation() 
{
	// TODO: Add your command handler code here
	m_game.SetDrivenMode(IPhysXObjBase::DRIVEN_BY_ANIMATION);
}

void CAPhysXLabView::OnUpdateModeDvnAnimation(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	(0 == m_game.GetTerrain())? pCmdUI->Enable(FALSE) : pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_game.QueryDrivenMode(IPhysXObjBase::DRIVEN_BY_ANIMATION));
}

void CAPhysXLabView::OnModeDvnPartphysx() 
{
	// TODO: Add your command handler code here
	m_game.SetDrivenMode(IPhysXObjBase::DRIVEN_BY_PART_PHYSX);
}

void CAPhysXLabView::OnUpdateModeDvnPartphysx(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	(0 == m_game.GetTerrain())? pCmdUI->Enable(FALSE) : pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_game.QueryDrivenMode(IPhysXObjBase::DRIVEN_BY_PART_PHYSX));
}

void CAPhysXLabView::OnModeDvnPurephysx() 
{
	// TODO: Add your command handler code here
	m_game.SetDrivenMode(IPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
}

void CAPhysXLabView::OnUpdateModeDvnPurephysx(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	(0 == m_game.GetTerrain())? pCmdUI->Enable(FALSE) : pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_game.QueryDrivenMode(IPhysXObjBase::DRIVEN_BY_PURE_PHYSX));
}

void CAPhysXLabView::OnModeCFollowing() 
{
	// TODO: Add your command handler code here
	m_game.ChangeCameraMode();
}

void CAPhysXLabView::OnUpdateModeCFollowing(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool bIsPlay(CGame::GAME_MODE_PLAY == m_game.GetGameMode());
	pCmdUI->Enable(bIsPlay);

	bool bIsBinding = m_game.GetCameraController()->QueryMode(CCameraController::MODE_BINDING);
	pCmdUI->SetCheck(bIsBinding);
}

void CAPhysXLabView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (0 == m_game.GetTerrain())
		return; 

	switch(m_game.GetGameMode())
	{
	case CGame::GAME_MODE_EDIT:
		OnKeyDownModeEdit(nChar, nRepCnt, nFlags);
		break;
	case CGame::GAME_MODE_SIMULATE:
		OnKeyDownModeSimulate(nChar, nRepCnt, nFlags);
		break;
	case CGame::GAME_MODE_PLAY:
		OnKeyDownModePlay(nChar, nRepCnt, nFlags);
		break;
	default:
		break;
	}

	if (VK_F4 == nChar)
		ChangeNextDriveMode();
	else if ('X' == nChar)
		m_game.EnterTopView();
	else if ('8' == nChar)
		m_game.ChangeShootStuffType();
	else if ('9' == nChar)
		m_game.ChangeShootMode();
	
#if defined(_DEBUG)
	// just for experiment and debugging
	if ('K' == nChar)
	{
		A3DCamera& a= m_game.GetCameraController()->GetCamera();
		//a.SetDirAndUp(A3DVECTOR3(1,0,0), A3DVECTOR3(0,1,0));

		A3DVECTOR3 aa;
		//m_game.GetCameraController()->EntryDefBindingMode(aa, *m_render.GetViewport(), *m_game.GetTerrain());
	}
	if (VK_OEM_COMMA == nChar) // 0xBC == 188 == ',' 
	{
		A3DCamera& a= m_game.GetCameraController()->GetCamera();
		A3DVECTOR3 dir = a.GetDir();
		dir.y = -0.1f;
		a.SetDirAndUp(dir, A3DVECTOR3(0,1,0));

		dir.Normalize();
		A3DVECTOR3 dir2 = a.GetDir();
		assert(dir == dir2);
	}
#endif
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXLabView::OnKeyDownModeEdit(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_DELETE:
		m_game.DelPickedObject();
		break;
	case VK_F8:
		ChangeNextDragOperation();
		break;
	case 'C':
		if (GetKeyState(VK_CONTROL) < 0)
			m_game.CloneHitObject();
		break;
	case 'F':
		m_game.ShowFFShapes(true);
		break;
	case 'K':
		m_game.OnKeyDownEditScenario();
		break;
	}
	m_game.GetSceneExtraConfig().OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXLabView::OnKeyDownModeSimulate(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	static char* szStandActs[] = 
	{
		"Models\\运兵车\\军用运兵车.mox",
		"Models\\人族新手村\\人族新手村杂货铺.mox",
		"Models\\人族新手村\\人族新手村药铺.mox",
	};

	static const int nMax = sizeof(szStandActs) / sizeof(char*);
	static int index = -1;

// 	TestConvexMesh* pTCM = m_game.GetTestConvexMesh();
// 	if (0 != pTCM)
// 	{
// 		switch (nChar)
// 		{
// 		case 'T':
// 			index = (++index) % nMax;
// 			pTCM->CreateObjectGroup(szStandActs[index]);
// 			break;
// 		case 'Y':
// 			pTCM->ReleaseObject();
// 			break;
// 		}
// 	}


	if ('B' == nChar)
		m_game.AddForceInTheArea();
	if ('N' == nChar)
		m_game.ChangeECModelPhysXState();
	if ('H' == nChar)
		m_game.ChangeTestCMState();

	OnKeyDownModePlay(nChar, nRepCnt, nFlags);
}

void CAPhysXLabView::OnKeyDownModePlay(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_SPACE:
		m_game.ShootAPhysXObject(GetSafeHwnd(), m_ShotScale, m_ShotSpeed);
		break;
	case 'C':
		OnModeCFollowing();
		break;
	case 'P':
		m_game.ChangePhysXPauseState();
		break;
	case 'O':
		m_game.SingleStepPhysXSim();
		break;
	case 'R':
		m_game.SwitchMainActorWalkRun();
		break;
	case 'U':
		if (gTestRiveFF)
			m_game.TestAndDebug(GetSafeHwnd());
		else
			m_game.ControlWeatherSurrounding();
		break;
	case 'J':
		if (gTestRiveFF)
			m_game.TestAndDebug(GetSafeHwnd(), true);
		else
			m_game.JumpMainActor();
		break;
	case 'L':
		m_game.ReleaseAllShotStuff();
		break;
	case VK_TAB:
		m_game.SwitchMainActor();
		break;
	case VK_OEM_1:  // ';:' for US
		m_game.ControlModelClothes();
		break;
	case '0':
		m_game.SwitchCCCollision();
		break;
	case VK_ADD:
		m_game.IncreaseMainActorCCVolume();
		break;
	case VK_SUBTRACT:
		m_game.DecreaseMainActorCCVolume();
		break;
	case VK_F7:
		{
			// to Coredump PhysX data to the file...
			char szFile[1024],tbuf[1024];
			strcpy(szFile,af_GetBaseDir());
			struct tm *today;
			time_t long_time;
			time( &long_time );					/* Get time as long integer. */
			today = localtime( &long_time );	/* Convert to local time. */
			strftime(tbuf,100,"\\PhysX\\%y-%m-%d-%H-%M-%S.xml",today);
			strcat(szFile,tbuf);
			gPhysXEngine->CoreDumpPhysX(szFile, APhysXEngine::APX_COREDUMP_FT_XML);
			break;
		}
	case 'I':
		m_game.SwitchCC();
		break;
	case VK_NUMPAD6:
		m_game.SwitchCC(3);
		break;
	case VK_NUMPAD7:
		m_game.SwitchCC(0);
		break;
	case VK_NUMPAD8:
		m_game.SwitchCC(1);
		break;
	case VK_NUMPAD9:
		m_game.SwitchCC(2);
		break;
	case 'M':
		m_game.MoveObj();
		break;
	}

	m_game.OnKeyDownScenarioMgr(nChar);
}

void CAPhysXLabView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if ('F' == nChar)
		m_game.ShowFFShapes(false);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CAPhysXLabView::OnMenuEditmod() 
{
	// TODO: Add your command handler code here
	AString strFilePath;
	if (m_game.GetHitObjectFileName(strFilePath))
	{
		AString ff;
		m_game.GetMABindingFF(ff);
		DlgFFEdit dlg(ff, strFilePath);
		dlg.DoModal();
		m_game.ReloadFFFile(dlg.GetSavedFile());
		if (dlg.GetBindingFF(ff))
		{
			CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
			if (0 != pFrame)
				pFrame->m_FFbindingToMA = ff;
		}
	}
}

void CAPhysXLabView::OnEditAddChb() 
{
	OnAddOperation(CPhysXObjMgr::OBJ_TYPEID_CHB);
}

void CAPhysXLabView::OnUpdateEditAddChb(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(EnableAddObject());
}
