// APhysXLab31View.cpp : implementation of the CAPhysXLab31View class
//

#include "stdafx.h"
#include "APhysXLab31.h"

#include "MainFrm.h"
#include "APhysXLab31Doc.h"
#include "APhysXLab31View.h"
#include "DlgPhysXTrnBuilder.h"
#include <LuaWrapper/LuaState.h>

/*
#include <A3DMacros.h>
#include <A3DTerrain2.h>
#include <A3DTerrain2LOD.h>
#include <A3DTerrain2Blk.h>
#include <A3DTerrain2Loader.h>*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPhysXLab31View

IMPLEMENT_DYNCREATE(CAPhysXLab31View, CView)

BEGIN_MESSAGE_MAP(CAPhysXLab31View, CView)
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
	ON_COMMAND(ID_FILE_NEW, &CAPhysXLab31View::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CAPhysXLab31View::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CAPhysXLab31View::OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CAPhysXLab31View::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CAPhysXLab31View::OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CAPhysXLab31View::OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CAPhysXLab31View::OnUpdateFileSaveAs)
	ON_COMMAND_RANGE(CMainFrame::RECENT_FILE_ID_BEGIN, CMainFrame::RECENT_FILE_ID_BEGIN + CMainFrame::RECENT_FILE_ID_COUNT, OnFileRecent)
	ON_UPDATE_COMMAND_UI_RANGE(CMainFrame::RECENT_FILE_ID_BEGIN, CMainFrame::RECENT_FILE_ID_BEGIN + CMainFrame::RECENT_FILE_ID_COUNT, OnUpdateFileRecent)
	ON_COMMAND(ID_EDIT_UNDO, &CAPhysXLab31View::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CAPhysXLab31View::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CAPhysXLab31View::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CAPhysXLab31View::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_ADDTERRAIN, &CAPhysXLab31View::OnEditAddterrain)
	ON_COMMAND(ID_EDIT_PHYSXTRNGEN, &CAPhysXLab31View::OnEditPhysxtrngen)
	ON_COMMAND(ID_EDIT_ADDSTATICOBJ, &CAPhysXLab31View::OnEditAddstaticobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSTATICOBJ, &CAPhysXLab31View::OnUpdateEditAddstaticobj)
	ON_COMMAND(ID_EDIT_ADDSMD, &CAPhysXLab31View::OnEditAddsmd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSMD, &CAPhysXLab31View::OnUpdateEditAddsmd)
	ON_COMMAND(ID_EDIT_ADDECM, &CAPhysXLab31View::OnEditAddecm)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDECM, &CAPhysXLab31View::OnUpdateEditAddecm)
	ON_COMMAND(ID_EDIT_ADDPHYSXGFX, &CAPhysXLab31View::OnEditAddphysxgfx)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPHYSXGFX, &CAPhysXLab31View::OnUpdateEditAddphysxgfx)
	ON_COMMAND(ID_EDIT_ADDPHYSXFF, &CAPhysXLab31View::OnEditAddphysxff)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPHYSXFF, &CAPhysXLab31View::OnUpdateEditAddphysxff)
	ON_COMMAND(ID_EDIT_ADDLASTOBJ, &CAPhysXLab31View::OnEditAddlastobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDLASTOBJ, &CAPhysXLab31View::OnUpdateEditAddlastobj)
	ON_COMMAND(ID_MODE_EDIT, &CAPhysXLab31View::OnModeEdit)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, &CAPhysXLab31View::OnUpdateModeEdit)
	ON_COMMAND(ID_MODE_SIMULATE, &CAPhysXLab31View::OnModeSimulate)
	ON_UPDATE_COMMAND_UI(ID_MODE_SIMULATE, &CAPhysXLab31View::OnUpdateModeSimulate)
	ON_COMMAND(ID_MODE_PLAY, &CAPhysXLab31View::OnModePlay)
	ON_UPDATE_COMMAND_UI(ID_MODE_PLAY, &CAPhysXLab31View::OnUpdateModePlay)
	ON_COMMAND(ID_MODE_PX_PROFILE, &CAPhysXLab31View::OnModePxProfile)
	ON_UPDATE_COMMAND_UI(ID_MODE_PX_PROFILE, &CAPhysXLab31View::OnUpdateModePxProfile)
	ON_COMMAND(ID_MODE_PX_DEBUG, &CAPhysXLab31View::OnModePxDebug)
	ON_UPDATE_COMMAND_UI(ID_MODE_PX_DEBUG, &CAPhysXLab31View::OnUpdateModePxDebug)
	ON_COMMAND(ID_MODE_DRAG_MOVE, &CAPhysXLab31View::OnModeDragMove)
	ON_UPDATE_COMMAND_UI(ID_MODE_DRAG_MOVE, &CAPhysXLab31View::OnUpdateModeDragMove)
	ON_COMMAND(ID_MODE_DRAG_ROTATE, &CAPhysXLab31View::OnModeDragRotate)
	ON_UPDATE_COMMAND_UI(ID_MODE_DRAG_ROTATE, &CAPhysXLab31View::OnUpdateModeDragRotate)
	ON_COMMAND(ID_MODE_DRAG_SCALE, &CAPhysXLab31View::OnModeDragScale)
	ON_UPDATE_COMMAND_UI(ID_MODE_DRAG_SCALE, &CAPhysXLab31View::OnUpdateModeDragScale)
	ON_COMMAND(ID_MODE_PHYSXHW, &CAPhysXLab31View::OnModePhysxhw)
	ON_UPDATE_COMMAND_UI(ID_MODE_PHYSXHW, &CAPhysXLab31View::OnUpdateModePhysxhw)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_MODE_DVN_ANIMATION, &CAPhysXLab31View::OnModeDvnAnimation)
	ON_UPDATE_COMMAND_UI(ID_MODE_DVN_ANIMATION, &CAPhysXLab31View::OnUpdateModeDvnAnimation)
	ON_COMMAND(ID_MODE_DVN_PARTPHYSX, &CAPhysXLab31View::OnModeDvnPartphysx)
	ON_UPDATE_COMMAND_UI(ID_MODE_DVN_PARTPHYSX, &CAPhysXLab31View::OnUpdateModeDvnPartphysx)
	ON_COMMAND(ID_MODE_DVN_PUREPHYSX, &CAPhysXLab31View::OnModeDvnPurephysx)
	ON_UPDATE_COMMAND_UI(ID_MODE_DVN_PUREPHYSX, &CAPhysXLab31View::OnUpdateModeDvnPurephysx)
	ON_COMMAND(ID_MODE_C_FOLLOWING, &CAPhysXLab31View::OnModeCFollowing)
	ON_UPDATE_COMMAND_UI(ID_MODE_C_FOLLOWING, &CAPhysXLab31View::OnUpdateModeCFollowing)
END_MESSAGE_MAP()

// CAPhysXLab31View construction/destruction
const CString CAPhysXLab31View::NoName = _T("无标题");

CAPhysXLab31View::CAPhysXLab31View()
{
	// TODO: add construction code here
	m_ViewSize.cx	= 0;
	m_ViewSize.cy	= 0;
	m_dwRenderStart = 0;
	m_dwLastRender	= 0;
	m_pGameProps	= 0;
	m_pCamCtrl		= 0;
	m_pSelGroup		= 0;
}

CAPhysXLab31View::~CAPhysXLab31View()
{
}

BOOL CAPhysXLab31View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CAPhysXLab31View drawing

void CAPhysXLab31View::OnDraw(CDC* /*pDC*/)
{
	CAPhysXLab31Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	if (m_render.IsReady())
		Render();
}


// CAPhysXLab31View diagnostics

#ifdef _DEBUG
void CAPhysXLab31View::AssertValid() const
{
	CView::AssertValid();
}

void CAPhysXLab31View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAPhysXLab31Doc* CAPhysXLab31View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAPhysXLab31Doc)));
	return (CAPhysXLab31Doc*)m_pDocument;
}
#endif //_DEBUG


// CAPhysXLab31View message handlers

void CAPhysXLab31View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	g_LuaStateMan.Init();
	InitECMApi(g_LuaStateMan.GetConfigState());
	InitECMApi(g_LuaStateMan.GetAIState());

	if (m_render.IsReady())
		return;

	if (!m_render.Init(AfxGetInstanceHandle(), m_hWnd, false))
	{
		a_LogOutput(1, "CAPhysXLabView::OnInitialUpdate: Failed to create render!");
		return;
	}

	CCoordinateDirection::g_Render = &m_render;
	CPhysXObjSelGroup::pWCTopZBuf = m_render.GetWCTopZBUf();
	if (!m_game.Init(m_render))
		return;

	m_pGameProps = m_game.GetProperties();
	m_pCamCtrl	 = m_game.GetCameraController();
	if (0 != m_pCamCtrl)
		m_pCamCtrl->SetScreenCenterPos(m_ViewSize.cx / 2, m_ViewSize.cy / 2);

	OnModeDragRotate();
	m_pGameProps->EnablePhysXProfile(false);
	m_pGameProps->EnablePhysXDebugRender(false);
//	m_pGameProps->EnableShowTerrianGrid(true);

	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (0 != pFrame)
	{
		CmdQueue& cmdQueue = m_pGameProps->GetCmdQueue();
		m_pSelGroup = &(m_pGameProps->GetSelGroup());
		CPropsWindow* pPW = pFrame->GetPropsWindow();
		pPW->SetSelGroup(*m_pSelGroup);
		pPW->SetCmdQueue(cmdQueue);
		CWorkSpaceBar* pWSB = pFrame->GetWorkSpaceBar();
		pWSB->SetSelGroup(m_pSelGroup);
		pWSB->SetCmdQueue(cmdQueue);
		pWSB->SetGame(m_game);
		m_pGameProps->GetOSRMgr().RegisterReceiver(*pWSB);
//		m_game.SetMABindingFF(AString(_TWC2AS(pFrame->m_FFbindingToMA)));
	}
}

void CAPhysXLab31View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	m_game.Release();
	m_render.Release();
	g_LuaStateMan.Release();

	m_dwRenderStart  = 0;
	m_dwLastRender	 = 0;
	m_pGameProps	 = 0;
	m_pCamCtrl		 = 0;
}

void CAPhysXLab31View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (0 == cx || 0 == cy)
		return;

	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	m_ViewSize.cx = cx;
	m_ViewSize.cy = cy;
	m_render.ResizeDevice(cx, cy);
	if (0 != m_pCamCtrl)
		m_pCamCtrl->SetScreenCenterPos(cx / 2, cy / 2);
}
bool CAPhysXLab31View::Tick(const DWORD dwTime)
{
	if (!m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EMPTY))
		return m_game.Tick(dwTime);
	return true;
}

bool CAPhysXLab31View::Render()
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

LPCTSTR CAPhysXLab31View::GetDragOptText() const
{
	if (m_status.ReadFlag(STATE_IS_MOVEMENT))
		return _T("拖拽效果(F7): 平移");

	if (m_status.ReadFlag(STATE_IS_ROTATION))
		return _T("拖拽效果(F7): 旋转");

	if (m_status.ReadFlag(STATE_IS_SCALE))
		return _T("拖拽效果(F7): 缩放");

	assert(!"Unknown Drag Operation!");
	return 0;
}

LPCTSTR CAPhysXLab31View::GetDrvModeText() const
{
	const TCHAR* pStr = IPropPhysXObjBase::GetDrivenModeTextChinese(m_pGameProps->GetDrivenMode());
	m_strBuffer.Format(_T("驱动模式(F4): %s"), pStr);
	return m_strBuffer;
}

LPCTSTR CAPhysXLab31View::GetShootModeText() const
{
	CGameProperty::ShootMode ssm = m_pGameProps->GetShootMode();

	switch(ssm)
	{
	case CGameProperty::SM_EYE_TO_CENTER:
		return _T("弹物模式(9): 随摄像机");
	case CGameProperty::SM_EYE_TO_MOUSE:
		return _T("弹物模式(9): 弹向鼠标");
	case CGameProperty::SM_MOUSE_FALLING:
		return _T("弹物模式(9): 鼠标落体");
	case CGameProperty::SM_FRONT_FALLING:
		return _T("弹物模式(9): 前方落体");
//	case CGameProperty::SM_KINEMATIC:
//		return _T("弹物模式(9): Kinematic");
	}
	return _T("弹物模式(9): 未知");
}

LPCTSTR CAPhysXLab31View::GetShootTypeText() const
{
	CGameProperty::ShootStuffID ssi = m_pGameProps->GetShootType();

	switch(ssi)
	{
	case CGameProperty::SSI_SPHERE:
		return _T("弹物类别(8): 小球");
	case CGameProperty::SSI_BOX:
		return _T("弹物类别(8): 箱子");
	case CGameProperty::SSI_BARREL:
		return _T("弹物类别(8): 木桶");
//	case CGameProperty::SSI_BREAKABLEBOX:
//		return _T("弹物类别(8): 破箱");
//	case CGameProperty::SSI_BOMB:
//		return _T("弹物类别(8): 炸弹");
	}

	return _T("弹物类别(8): 未知");
}

void CAPhysXLab31View::OnUpdateDragOpt(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(); 
	pCmdUI->SetText(GetDragOptText()); 
}

void CAPhysXLab31View::OnUpdateDrvMode(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(); 
	pCmdUI->SetText(GetDrvModeText()); 
}

void CAPhysXLab31View::OnUpdateCurrentState(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(); 
	pCmdUI->SetText(m_pGameProps->GetGameModeDesc(true)); 
}

void CAPhysXLab31View::OnUpdateShootMode(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(); 
	pCmdUI->SetText(GetShootModeText()); 
}

void CAPhysXLab31View::OnUpdateShootType(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(); 
	pCmdUI->SetText(GetShootTypeText()); 
}

void CAPhysXLab31View::DrawPrompts()
{
	RECT rcClient;
	GetClientRect(&rcClient);

	int x = rcClient.left + 10;
	int y = rcClient.bottom - 24;
	DWORD dwColor = 0xff00ff00;
	ACString str;

	//	Camera position
	if (0 != m_pCamCtrl)
	{
		A3DVECTOR3 vPos = m_pCamCtrl->GetPos();
		if (m_pCamCtrl->QueryMode(CCameraController::MODE_BINDING))
			str.Format(_T("Camera(binding mode): %.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		else
			str.Format(_T("Camera(free fly mode): %.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		m_render.TextOut(x, y, str, str.GetLength(), dwColor);
	}
	y -= 16;

	//	Render time of last frame
	str.Format(_T("Render time: %d"), m_dwLastRender);
	m_render.TextOut(x, y, str, str.GetLength(), dwColor);
	y -= 16;

	// Current main actor CC type
	const TCHAR* pStr = m_game.GetCurrentMainActorCCType();
	if (0 != pStr)
	{
		str = pStr;
		str.Format(_T("Current MainActor CC: %s"), pStr);
		m_render.TextOut(x, y, str, str.GetLength(), dwColor);
		y -= 16;
	}

	//	Physical state
	IPropPhysXObjBase::DrivenMode iState = m_pGameProps->GetDrivenMode();
	if (iState == IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX)
		str = _T("Pure PhysX Simulation");
	else if (iState == IPropPhysXObjBase::DRIVEN_BY_ANIMATION)
		str = _T("Pure Animation");
	else if (iState = IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX)
		str = _T("Partial PhysX Simulation");
	else
		assert(!"Unknown Driven Mode!");

	m_render.TextOut(x, y, str, str.GetLength(), dwColor);
	y -= 16;
}

void CAPhysXLab31View::SceneOpenOrSaveAs(const bool bIsOpen)
{
	BOOL bISOpen = bIsOpen? TRUE : FALSE;
	DWORD dwFlags =  OFN_EXPLORER | OFN_NOCHANGEDIR;
	if (bIsOpen)
		dwFlags |= OFN_FILEMUSTEXIST;

	TCHAR szFilter[] = _T("PLS 文件(*.pls)|*.pls||");
	CFileDialog	FileDia(bISOpen, _T("*.pls"), NULL, dwFlags, szFilter, NULL);
	ACString strPath = _TAS2WC(af_GetBaseDir());
	strPath += _T("\\Scenes\\");
	FileDia.m_ofn.lpstrInitialDir = strPath;

	if (0 != m_pCamCtrl)
		m_pCamCtrl->EnableMoveCamera(false);
	INT_PTR rtn = FileDia.DoModal();
	if (0 != m_pCamCtrl)
		m_pCamCtrl->EnableMoveCamera(true);

	if (IDOK != rtn)
		return;

	if (bIsOpen)
		SceneOpen(FileDia.GetPathName(), FileDia.GetFileTitle());
	else
	{
		m_game.SavePhysXDemoScene(_TWC2AS(FileDia.GetPathName()));
		GetDocument()->SetTitle(FileDia.GetFileTitle());
		CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		if (0 != pFrame)
			pFrame->AddToRecent(FileDia.GetPathName());
	}
}

void CAPhysXLab31View::SceneOpen(const CString& path, const CString& title)
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	if (!CFile::GetStatus(path, CFileStatus()))
	{
		if (0 != pFrame)
			pFrame->DelFromRecent(path);
		return;
	}

	if(m_game.LoadPhysXDemoScene(_TWC2AS(path)))
	{
		OnModeDragRotate();
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

void CAPhysXLab31View::ChangeNextDragOperation()
{
	if (m_status.ReadFlag(STATE_IS_DRAGGING))
		return;

	if (m_status.ReadFlag(STATE_IS_MOVEMENT))
		OnModeDragScale();
	else if (m_status.ReadFlag(STATE_IS_ROTATION))
		OnModeDragMove();
	else if (m_status.ReadFlag(STATE_IS_SCALE))
		OnModeDragRotate();
}

void CAPhysXLab31View::ChangeNextDriveMode()
{
	CGameProperty::DrivenMode dm = m_pGameProps->GetDrivenMode();
	if (IPropPhysXObjBase::DRIVEN_BY_ANIMATION == dm)
		OnModeDvnPartphysx();
	else if (IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX == dm)
		OnModeDvnPurephysx();
	else if (IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX == dm)
		OnModeDvnAnimation();
}

bool CAPhysXLab31View::EnableAddObject() const
{
	if (!m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		return false;

	if (0 != m_pGameProps->GetPickedObject())
		return false;

	return true;
}

void CAPhysXLab31View::OnAddOperation(const ObjID objType)
{
	const RawObjMgr::ObjTypeInfo& objInfo = RawObjMgr::GetObjTypeInfo(objType);

	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	CFileDialog	FileDlg(TRUE, objInfo.strWCExt, NULL, dwFlags, objInfo.strFilter, NULL);
	FileDlg.m_ofn.lpstrInitialDir = _TAS2WC(af_GetBaseDir());
	if (IDOK != FileDlg.DoModal())
		return;

	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	AString strFullFile = _TWC2AS(FileDlg.GetPathName());
	AString strFile;
	af_GetRelativePath(strFullFile, strFile);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	IPhysXObjBase* pObject = m_game.CreateObject(objType, strFile, pt);
	if (0 == pObject)
	{
		MessageBox(_T("加载数据失败!"));
		return;
	}

	m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
	m_LastAddedItem.strPathName = strFile;
}

void CAPhysXLab31View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_game.OnLButtonDown(point.x, point.y, nFlags);
	CView::OnLButtonDown(nFlags, point);
}

void CAPhysXLab31View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		m_status.ClearFlag(STATE_IS_DRAGGING);

	m_game.OnLButtonUp(point.x, point.y, nFlags);
	CView::OnLButtonUp(nFlags, point);
}

void CAPhysXLab31View::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnMButtonDown(nFlags, point);
}

void CAPhysXLab31View::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnMButtonUp(nFlags, point);
}

void CAPhysXLab31View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_pGameProps->SetLastMousePos(point);
	m_status.RaiseFlag(STATE_IS_RBCLICK);
	SetCapture();
	CView::OnRButtonDown(nFlags, point);
}

void CAPhysXLab31View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
	{
		IObjBase* pObj = m_pGameProps->GetRayHitObject();
		if (0 != pObj)
		{
			if (MK_CONTROL & nFlags)
			{
				m_pGameProps->GetCmdQueue().SubmitBeforeExecution(CmdSelRemove());
				m_pGameProps->GetSelGroup().RemoveObject(*pObj);
			}
			else
			{
			}
			/*if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD == pObj->GetObjType())
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
			}*/
		}
	}
	else if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_PLAY))
	{
//		if (m_status.ReadFlag(STATE_IS_RBCLICK) && (MK_SHIFT & nFlags))
//			m_game.TrytoHang();
	}
	m_status.ClearFlag(STATE_IS_RBCLICK);
	CView::OnRButtonUp(nFlags, point);
}

void CAPhysXLab31View::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_status.ClearFlag(STATE_IS_RBCLICK);
	IObjBase* pObj = m_pGameProps->GetPickedObject();
	if ((0 != pObj) || (0 == m_pSelGroup))
	{
		CView::OnRButtonDblClk(nFlags, point);
		return;
	}

	PhysRay ray = ScreenPosToRay(*m_render.GetViewport(), point.x, point.y);
	PhysRayTraceHit hitInfo;
	m_game.RayTraceObject(point, ray, hitInfo);
	IObjBase* pHitObj = static_cast<IObjBase*>(hitInfo.UserData);
	if (MK_CONTROL & nFlags)
	{
		if (0 != pHitObj)
			m_pSelGroup->AppendObject(*pHitObj);
	}
	else
	{
		m_pSelGroup->ReplaceWithObject(pHitObj);
	}

	CView::OnRButtonDblClk(nFlags, point);
}

void CAPhysXLab31View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (MK_RBUTTON & nFlags)
	{
		// Camera view rotation
		if (0 != m_pCamCtrl)
		{
			CSize dis = point - m_pGameProps->GetLastMousePos();
			m_pCamCtrl->RotateCamera(dis.cx, dis.cy, m_game.GetTerrain());
		}

		m_status.ClearFlag(STATE_IS_RBCLICK);
		m_pGameProps->SetLastMousePos(point);
		CView::OnMouseMove(nFlags, point);
		return;
	}

/*	if (MK_MBUTTON & nFlags)
	{
		if (m_bIsAccumulating)
			m_render.DrawForceBar(point.x, point.y, 0xff00ff00);
		else if (MK_CONTROL & nFlags)
			m_render.SetFrontSight(point.x, point.y, 8, 0xff00ff00);

		m_pGameProps->SetLastMousePos(point);
		CView::OnMouseMove(nFlags, point);
		return;
	}
*/
//	if ((MK_LBUTTON & nFlags) && (0 != m_pGameProps.GetRayHitObject()))
	{
//		if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
//			m_status.RaiseFlag(STATE_IS_DRAGGING);
	}

	m_game.OnMouseMove(point.x, point.y, nFlags);
	m_pGameProps->SetLastMousePos(point);
	CView::OnMouseMove(nFlags, point);
}

BOOL CAPhysXLab31View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	m_game.OnMouseWheel(pt.x, pt.y, zDelta, nFlags);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CAPhysXLab31View::OnFileNew()
{
	// TODO: Add your command handler code here
	if (IDNO == MessageBox(_T("确定要重置系统么？"), _T("提示"), MB_YESNO))
		return;

	m_pGameProps->SetGameMode(CGameProperty::GAME_MODE_EMPTY);
	GetDocument()->SetTitle(NoName);
}

void CAPhysXLab31View::OnFileOpen()
{
	// TODO: Add your command handler code here
	SceneOpenOrSaveAs(true);
}

void CAPhysXLab31View::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
		pCmdUI->Enable(FALSE);
	else if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_PLAY))
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CAPhysXLab31View::OnFileSave()
{
	// TODO: Add your command handler code here
	const char* pPath = m_pGameProps->GetActiveSceneFile();
	if (0 == pPath)
	{
		OnFileSaveAs();
		return;
	}

	if (IDYES == AfxMessageBox(_T("确定覆盖当前文档？"), MB_YESNO | MB_ICONQUESTION))
	{
/*		CString path;
		CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		if (0 != pFrame)
		{
			if (pFrame->GetCurrentString(path))
				m_game.SavePhysXDemoScene(_TWC2AS(path));
		}
		*/
		m_game.SavePhysXDemoScene(pPath);
	}
}

void CAPhysXLab31View::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnFileSaveAs()
{
	// TODO: Add your command handler code here
	SceneOpenOrSaveAs(false);
}

void CAPhysXLab31View::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnFileRecent(UINT nID) 
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

void CAPhysXLab31View::OnUpdateFileRecent(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	OnUpdateFileOpen(pCmdUI);
}

void CAPhysXLab31View::OnEditUndo()
{
	// TODO: Add your command handler code here
	m_pGameProps->GetCmdQueue().Undo();
}

void CAPhysXLab31View::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->GetCmdQueue().UndoEnable())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditRedo()
{
	// TODO: Add your command handler code here
	m_pGameProps->GetCmdQueue().Redo();
}

void CAPhysXLab31View::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->GetCmdQueue().RedoEnable())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddterrain()
{
	// TODO: Add your command handler code here
	CString strInitPath = _TAS2WC(af_GetBaseDir());
	strInitPath += _T("\\Maps\\");
	CFileDialog FileDlg(TRUE, _T("trn2"), NULL, OFN_FILEMUSTEXIST |	OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST, _T("地形文件 (*.trn2)|*.trn2||"), this);
	FileDlg.GetOFN().lpstrInitialDir = strInitPath;
	if (IDOK != FileDlg.DoModal())
		return;
	
	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	AString strFullFile = _TWC2AS(FileDlg.GetPathName());
	AString strFile;
	af_GetRelativePath(strFullFile, strFile);
	m_game.LoadTerrainAndPhysXTerrain(strFile);
}

void CAPhysXLab31View::OnEditPhysxtrngen()
{
	// TODO: Add your command handler code here
	CDlgPhysXTrnBuilder dlg(m_pGameProps->GetTerrainFile(), *m_render.GetA3DEngine());
	dlg.DoModal();
}

void CAPhysXLab31View::OnEditAddstaticobj()
{
	// TODO: Add your command handler code here
	OnAddOperation(RawObjMgr::OBJ_TYPEID_STATIC);
}

void CAPhysXLab31View::OnUpdateEditAddstaticobj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddsmd()
{
	// TODO: Add your command handler code here
	OnAddOperation(RawObjMgr::OBJ_TYPEID_SKINMODEL);
}

void CAPhysXLab31View::OnUpdateEditAddsmd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddecm()
{
	// TODO: Add your command handler code here
	OnAddOperation(RawObjMgr::OBJ_TYPEID_ECMODEL);
//	for region test
/*	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	static int a =0;
	++a;
	Region* p =m_game.CreateRegion(pt);
	if (a%2 == 0)
		p->SetShapeType(Region::ST_CAPSULE);*/
}

void CAPhysXLab31View::OnUpdateEditAddecm(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddphysxgfx()
{
	// TODO: Add your command handler code here
	OnAddOperation(RawObjMgr::OBJ_TYPEID_PARTICLE);
}

void CAPhysXLab31View::OnUpdateEditAddphysxgfx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddphysxff()
{
	// TODO: Add your command handler code here
}

void CAPhysXLab31View::OnUpdateEditAddphysxff(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CAPhysXLab31View::OnEditAddlastobj()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	m_game.CreateObject(ObjID(m_LastAddedItem.nTypeID), m_LastAddedItem.strPathName, pt);

}

void CAPhysXLab31View::OnUpdateEditAddlastobj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (0 >= m_LastAddedItem.nTypeID)
		pCmdUI->Enable(FALSE);
	else
		EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnModeEdit()
{
	// TODO: Add your command handler code here
	m_pGameProps->SetGameMode(CGameProperty::GAME_MODE_EDIT);

	const POINT& pt = m_pGameProps->GetLastMousePos();
	PhysRay ray = ScreenPosToRay(*m_render.GetViewport(), pt.x, pt.y);
	PhysRayTraceHit hitInfo;
	m_game.RayTraceObject(pt, ray, hitInfo);
}

void CAPhysXLab31View::OnUpdateModeEdit(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EMPTY))
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}
	
	bool bIsEdit = m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT);
	pCmdUI->SetCheck(bIsEdit);
	pCmdUI->Enable(!bIsEdit);
}

void CAPhysXLab31View::OnModeSimulate()
{
	// TODO: Add your command handler code here
	m_pGameProps->SetGameMode(CGameProperty::GAME_MODE_SIMULATE);
}

void CAPhysXLab31View::OnUpdateModeSimulate(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
	{
		pCmdUI->SetCheck(TRUE);
		pCmdUI->Enable(FALSE);
	}
	else if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
}

void CAPhysXLab31View::OnModePlay()
{
	// TODO: Add your command handler code here
	m_pGameProps->SetGameMode(CGameProperty::GAME_MODE_PLAY);
}

void CAPhysXLab31View::OnUpdateModePlay(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_PLAY))
	{
		pCmdUI->SetCheck(TRUE);
		pCmdUI->Enable(FALSE);
	}
	else if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
}

void CAPhysXLab31View::OnModePxProfile()
{
	// TODO: Add your command handler code here
	m_pGameProps->EnablePhysXProfile(!m_pGameProps->IsPhysXProfileEnabled());
}

void CAPhysXLab31View::OnUpdateModePxProfile(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGameProps->IsPhysXProfileEnabled());
}

void CAPhysXLab31View::OnModePxDebug()
{
	// TODO: Add your command handler code here
	m_pGameProps->EnablePhysXDebugRender(!m_pGameProps->IsPhysXDebugRenderEnabled());
}

void CAPhysXLab31View::OnUpdateModePxDebug(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGameProps->IsPhysXDebugRenderEnabled());
}

void CAPhysXLab31View::OnModeDragMove()
{
	// TODO: Add your command handler code here
	m_status.RaiseFlag(STATE_IS_MOVEMENT);
	m_status.ClearFlag(STATE_IS_ROTATION);
	m_status.ClearFlag(STATE_IS_SCALE);
	m_pGameProps->SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE);
}

void CAPhysXLab31View::OnUpdateModeDragMove(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGameProps->QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE));
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnModeDragRotate()
{
	// TODO: Add your command handler code here
	m_status.RaiseFlag(STATE_IS_ROTATION);
	m_status.ClearFlag(STATE_IS_MOVEMENT);
	m_status.ClearFlag(STATE_IS_SCALE);
	m_pGameProps->SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_ROTATE);
}

void CAPhysXLab31View::OnUpdateModeDragRotate(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGameProps->QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_ROTATE));
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnModeDragScale()
{
	// TODO: Add your command handler code here
	m_status.RaiseFlag(STATE_IS_SCALE);
	m_status.ClearFlag(STATE_IS_ROTATION);
	m_status.ClearFlag(STATE_IS_MOVEMENT);
	m_pGameProps->SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_SCALE);
}

void CAPhysXLab31View::OnUpdateModeDragScale(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGameProps->QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_SCALE));
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnModePhysxhw()
{
	// TODO: Add your command handler code here
	m_pGameProps->EnablePhysXHW(!m_pGameProps->IsPhysHWEnabled());
}

void CAPhysXLab31View::OnUpdateModePhysxhw(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pGameProps->IsPhysHWEnabled());
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_EDIT))
		OnKeyDownModeEdit(nChar, nRepCnt, nFlags);
	else if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_SIMULATE))
	//	OnKeyDownModeSimulate(nChar, nRepCnt, nFlags);
		OnKeyDownModePlay(nChar, nRepCnt, nFlags);
	else if (m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_PLAY))
		OnKeyDownModePlay(nChar, nRepCnt, nFlags);

	if (VK_F4 == nChar)
	{
		if (m_pGameProps->IsRuntime())
			ChangeNextDriveMode();
	}
//	else if ('X' == nChar)
//		m_game.EnterTopView();
	else if ('8' == nChar)
		m_pGameProps->ChangeShootStuffType();
	else if ('9' == nChar)
		m_pGameProps->ChangeShootMode();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
//	if ('F' == nChar)
//		m_game.ShowFFShapes(false);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::OnKeyDownModeEdit(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_DELETE:
		m_game.ReleaseAllSelectedObjects();
		break;
	case VK_F7:
		ChangeNextDragOperation();
		break;
	case 'C':
		if (GetKeyState(VK_CONTROL) < 0)
			m_game.CloneObjects();
		break;
	case 'F':
//		m_game.ShowFFShapes(true);
		break;
	case 'K':
//		m_game.OnKeyDownEditScenario();
		break;
	}
}

void CAPhysXLab31View::OnKeyDownModeSimulate(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CAPhysXLab31View::OnKeyDownModePlay(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_SPACE:
		m_game.ShootAPhysXObject(GetSafeHwnd(), 0.3f, 10);
//		m_game.ShootAPhysXObject(GetSafeHwnd(), m_ShotScale, m_ShotSpeed);
		break;
	case 'C':
		OnModeCFollowing();
		break;
	case 'P':
		m_pGameProps->ChangePhysXPauseState();
		break;
	case 'O':
		m_pGameProps->PhysXSimASingleStep();
		break;
	case 'R':
		m_game.SwitchMainActorWalkRun();
		break;
	case 'J':
		m_game.JumpMainActor();
		break;
	case 'L':
		m_game.ReleaseAllShotStuff();
		break;
	case VK_TAB:
		m_game.SwitchMainActor();
		break;
	case VK_OEM_1:  // ';:' for US
//		m_game.ControlModelClothes();
		break;
	case VK_ADD:
		break;
	case VK_SUBTRACT:
		break;
	case VK_F8:
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
		m_game.SwitchMainActorCCType(CAPhysXCCMgr::CC_UNKNOWN_TYPE);
		break;
	case VK_NUMPAD5:
		m_game.SwitchMainActorCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY);
		break;
	case VK_NUMPAD6:
		m_game.SwitchMainActorCCType(CAPhysXCCMgr::CC_APHYSX_EXTCC);
		break;
	case VK_NUMPAD7:
		m_game.SwitchMainActorCCType(CAPhysXCCMgr::CC_APHYSX_NXCC);
		break;
	case VK_NUMPAD8:
		m_game.SwitchMainActorCCType(CAPhysXCCMgr::CC_APHYSX_DYNCC);
		break;
	case VK_NUMPAD9:
		m_game.SwitchMainActorCCType(CAPhysXCCMgr::CC_BRUSH_CDRCC);
		break;
//	case 'M':
//		m_game.MoveObj();
		break;
	}
}

void CAPhysXLab31View::OnModeDvnAnimation()
{
	// TODO: Add your command handler code here
	m_pGameProps->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);
}

void CAPhysXLab31View::OnUpdateModeDvnAnimation(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->IsRuntime())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_pGameProps->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION));
}

void CAPhysXLab31View::OnModeDvnPartphysx()
{
	// TODO: Add your command handler code here
	m_pGameProps->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX);
}

void CAPhysXLab31View::OnUpdateModeDvnPartphysx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->IsRuntime())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_pGameProps->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX));
}

void CAPhysXLab31View::OnModeDvnPurephysx()
{
	// TODO: Add your command handler code here
	m_pGameProps->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
}

void CAPhysXLab31View::OnUpdateModeDvnPurephysx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGameProps->IsRuntime())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_pGameProps->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX));
}

void CAPhysXLab31View::OnModeCFollowing()
{
	// TODO: Add your command handler code here
	m_game.ChangeCameraMode();
}

void CAPhysXLab31View::OnUpdateModeCFollowing(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	bool bIsPlay(m_pGameProps->QueryGameMode(CGameProperty::GAME_MODE_PLAY));
	pCmdUI->Enable(bIsPlay);

	bool bIsBinding = m_pCamCtrl->QueryMode(CCameraController::MODE_BINDING);
	pCmdUI->SetCheck(bIsBinding);
}
