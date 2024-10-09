// APhysXLab31View.cpp : implementation of the CAPhysXLab31View class
//

#include "stdafx.h"
#include "APhysXLab31.h"

#include "MainFrm.h"
#include "APhysXLab31Doc.h"
#include "APhysXLab31View.h"
#include "DlgPhysXTrnBuilder.h"
#include "DlgSceneSetting.h"
#include "DlgSceneBuilder.h"
#include "DlgFFEditor.h"
#include <LuaWrapper/LuaState.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace APhysXCommonDNet;
using namespace System;
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
	ON_COMMAND(ID_FILE_REPLAY, &CAPhysXLab31View::OnFileReplay)
	ON_UPDATE_COMMAND_UI(ID_FILE_REPLAY, &CAPhysXLab31View::OnUpdateFileReplay)
	ON_COMMAND_RANGE(CMainFrame::RECENT_FILE_ID_BEGIN, CMainFrame::RECENT_FILE_ID_BEGIN + CMainFrame::RECENT_FILE_ID_COUNT, OnFileRecent)
	ON_UPDATE_COMMAND_UI_RANGE(CMainFrame::RECENT_FILE_ID_BEGIN, CMainFrame::RECENT_FILE_ID_BEGIN + CMainFrame::RECENT_FILE_ID_COUNT, OnUpdateFileRecent)
	ON_COMMAND(ID_EDIT_UNDO, &CAPhysXLab31View::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CAPhysXLab31View::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CAPhysXLab31View::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CAPhysXLab31View::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_ADDTERRAIN, &CAPhysXLab31View::OnEditAddterrain)
	ON_COMMAND(ID_EDIT_PHYSXTRNGEN, &CAPhysXLab31View::OnEditPhysxtrngen)
	ON_COMMAND(ID_EDIT_TSCENEGEN, &CAPhysXLab31View::OnEditTscenegen)
	ON_COMMAND(ID_EDIT_SAVEOBJ, &CAPhysXLab31View::OnEditSaveobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SAVEOBJ, &CAPhysXLab31View::OnUpdateEditSaveobj)
	ON_COMMAND(ID_EDIT_LOADOBJ, &CAPhysXLab31View::OnEditLoadobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LOADOBJ, &CAPhysXLab31View::OnUpdateEditLoadobj)
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
	ON_COMMAND(ID_EDIT_ADDDUMMY, &CAPhysXLab31View::OnEditAdddummy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDDUMMY, &CAPhysXLab31View::OnUpdateEditAdddummy)
	ON_COMMAND(ID_EDIT_ADDSMSOCKET, &CAPhysXLab31View::OnEditAddsmsocket)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSMSOCKET, &CAPhysXLab31View::OnUpdateEditAddsmsocket)
	ON_COMMAND(ID_EDIT_ADDSOCKETSM, &CAPhysXLab31View::OnEditAddsocketsm)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSOCKETSM, &CAPhysXLab31View::OnUpdateEditAddsocketsm)
	ON_COMMAND(ID_EDIT_ADDSPOTLIGHT, &CAPhysXLab31View::OnEditAddspotlight)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSPOTLIGHT, &CAPhysXLab31View::OnUpdateEditAddspotlight)
	ON_COMMAND(ID_EDIT_ADDRGNBOX, &CAPhysXLab31View::OnEditAddrgnbox)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDRGNBOX, &CAPhysXLab31View::OnUpdateEditAddrgnbox)
	ON_COMMAND(ID_EDIT_ADDRGNSPHERE, &CAPhysXLab31View::OnEditAddrgnsphere)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDRGNSPHERE, &CAPhysXLab31View::OnUpdateEditAddrgnsphere)
	ON_COMMAND(ID_EDIT_ADDRGNCAPSULE, &CAPhysXLab31View::OnEditAddrgncapsule)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDRGNCAPSULE, &CAPhysXLab31View::OnUpdateEditAddrgncapsule)
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
	ON_COMMAND(ID_SCENESETTING, &CAPhysXLab31View::OnScenesetting)
	ON_UPDATE_COMMAND_UI(ID_SCENESETTING, &CAPhysXLab31View::OnUpdateScenesetting)
	ON_COMMAND(ID_MODE_APHYSXSAMPLE, &CAPhysXLab31View::OnModeAphysxsample)
	ON_UPDATE_COMMAND_UI(ID_MODE_APHYSXSAMPLE, &CAPhysXLab31View::OnUpdateModeAphysxsample)
	ON_COMMAND(ID_VIEW_WIRE, &CAPhysXLab31View::OnViewWire)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIRE, &CAPhysXLab31View::OnUpdateViewWire)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CAPhysXLab31View construction/destruction
const CString CAPhysXLab31View::NoName = _T("无标题");

static void _renderExtraCallBack(MRender^ pRender)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CAPhysXLab31View* pView = (CAPhysXLab31View*)pFrame->GetActiveView();
	if (pView)
		pView->DrawPrompts(pRender);
}

CAPhysXLab31View::CAPhysXLab31View()
{
	// TODO: add construction code here
	m_HasCameraTreeItem = false;
	m_ViewSize.cx	= 0;
	m_ViewSize.cy	= 0;
	m_dwRenderStart = 0;
	m_dwLastRender	= 0;
	m_LastPT.x		= 0;
	m_LastPT.y		= 0;
	m_LastReproPT.x = 0;
	m_LastReproPT.y = 0;
	m_pCamCtrl		= 0;
	m_pSelGroup		= 0;
	m_pRecording	= 0;

	m_MEditor = gcnew APhysXCommonDNet::MEditor();
	m_pEditor = m_MEditor->GetEditor();
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
	m_MEditor->Render();
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

	System::IntPtr  handle(AfxGetInstanceHandle());
	System::IntPtr hwnd(m_hWnd);

	if (!m_MEditor->Init(handle, hwnd, false))
	{
		a_LogOutput(1, "CAPhysXLabView::OnInitialUpdate: Failed to create render!");
		return;
	}

	m_pCamCtrl = m_pEditor->GetNormalCamCtrl();
	if (0 != m_pCamCtrl)
		m_pCamCtrl->SetScreenSize(m_ViewSize.cx, m_ViewSize.cy);

	m_pSelGroup = &(m_pEditor->GetSelGroup());
	m_pEditor->EnablePhysXProfile(false);
	m_pEditor->EnablePhysXDebugRender(false);
	m_MEditor->renderExtraEvent += gcnew RenderExtraHandler(_renderExtraCallBack);
	m_pRecording = &(m_pEditor->GetRecording());
	m_pRecording->RegisterUIE(this);

	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (0 != pFrame)
	{
		CmdQueue& cmdQueue = m_pEditor->GetCmdQueue();
		CPropsWindow* pPW = pFrame->GetPropsWindow();
		pPW->SetSelGroup(*m_pSelGroup);
		pPW->SetCmdQueue(cmdQueue);
		CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
		CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
		pWSB1->SetSelGroup(m_pSelGroup);
		pWSB2->SetSelGroup(m_pSelGroup);
		pWSB1->SetCmdQueue(cmdQueue);
		pWSB2->SetCmdQueue(cmdQueue);
		pWSB1->SetGame(*m_pEditor);
		pWSB2->SetGame(*m_pEditor);
		m_pEditor->GetCloneOpt().Register(pWSB2);
		m_pEditor->GetOSRMgr().RegisterReceiver(*pWSB1);
		m_pEditor->GetOSRMgr().RegisterReceiver(*pWSB2);
		m_pEditor->GetAOORMgr().RegisterReceiver(*pWSB2);
		// workaround code to solve the "global static variable" issue. 
		// since the parent-children relationship was changed in the CmdQueue that is not wrapped by M-layer yet
		ApxObjBase::SetOperationReport(&m_pEditor->GetAOORMgr());
		OnModeDragMove();
	}
}

void CAPhysXLab31View::OnDestroy()
{
	// TODO: Add your message handler code here
	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (0 != pFrame)
	{
		if (m_HasCameraTreeItem)
		{
			CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
			CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
			pWSB1->OnNotifyDie(*m_pCamCtrl);
			pWSB2->OnNotifyDie(*m_pCamCtrl);
			m_HasCameraTreeItem = false;
		}
	}

	m_MEditor->Release();
	m_dwRenderStart  = 0;
	m_dwLastRender	 = 0;
	m_pCamCtrl		 = 0;

	CView::OnDestroy();
}

void CAPhysXLab31View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (0 == cx || 0 == cy)
		return;

	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	if (0 != m_pRecording)
		m_pRecording->LogEvent(OptSize(nType, cx, cy));

	m_ViewSize.cx = cx;
	m_ViewSize.cy = cy;
	if (m_pEditor->GetRender())
		m_pEditor->GetRender()->ResizeDevice(cx, cy);

	if (0 != m_pCamCtrl)
	{
		if (!m_pRecording->IsEventReproducing())
			m_pCamCtrl->SetScreenSize(cx, cy);
	}
}
bool CAPhysXLab31View::Tick()
{
	if (!m_pEditor->QuerySceneMode(Scene::SCENE_MODE_EMPTY))
		return m_MEditor->Tick();
	return true;
}

bool CAPhysXLab31View::Render()
{
	m_dwRenderStart = a_GetTime();
	m_MEditor->Render();
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
	const TCHAR* pStr = IPropPhysXObjBase::GetDrivenModeTextChinese(m_pEditor->GetDrivenMode());

	static CString str;
	str.Format(_T("驱动模式(F4): %s"), pStr);
	return str;
}

LPCTSTR CAPhysXLab31View::GetShootModeText() const
{
	CEditor::ShootMode ssm = m_pEditor->GetShootMode();

	switch(ssm)
	{
	case CEditor::SM_EYE_TO_CENTER:
		return _T("弹物模式(9): 随摄像机");
	case CEditor::SM_EYE_TO_MOUSE:
		return _T("弹物模式(9): 弹向鼠标");
	case CEditor::SM_MOUSE_FALLING:
		return _T("弹物模式(9): 鼠标落体");
	case CEditor::SM_FRONT_FALLING:
		return _T("弹物模式(9): 前方落体");
//	case CEditor::SM_KINEMATIC:
//		return _T("弹物模式(9): Kinematic");
	}
	return _T("弹物模式(9): 未知");
}

LPCTSTR CAPhysXLab31View::GetShootTypeText() const
{
	CEditor::ShootStuffID ssi = m_pEditor->GetShootType();

	switch(ssi)
	{
	case CEditor::SSI_SPHERE:
		return _T("弹物类别(8): 小球");
	case CEditor::SSI_BOX:
		return _T("弹物类别(8): 箱子");
	case CEditor::SSI_BARREL:
		return _T("弹物类别(8): 木桶");
//	case CEditor::SSI_BREAKABLEBOX:
//		return _T("弹物类别(8): 破箱");
//	case CEditor::SSI_BOMB:
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
	pCmdUI->SetText(m_pEditor->GetSceneModeDesc(true)); 
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

void CAPhysXLab31View::DrawPrompts(APhysXCommonDNet::MRender^ pRender)
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
		NxVec3 vPos = m_pCamCtrl->GetGPos();
		if (m_pCamCtrl->QueryMode(MODE_BINDING))
			str.Format(_T("Camera(binding mode): %.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		else
			str.Format(_T("Camera(free fly mode): %.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		pRender->RenderText(x, y, gcnew String(str), dwColor);
	}
	y -= 16;

	//	Render time of last frame
	str.Format(_T("Render time: %d"), m_dwLastRender);
	pRender->RenderText(x, y, gcnew String(str), dwColor);
	y -= 16;

	// Current main actor CC type
	const TCHAR* pStr = m_pEditor->GetCurrentMainActorCCType();
	if (0 != pStr)
	{
		str = pStr;
		str.Format(_T("Current MainActor CC: %s"), pStr);
		pRender->RenderText(x, y, gcnew String(str), dwColor);
		y -= 16;
	}

	//	Physical state
	IPropPhysXObjBase::DrivenMode iState = m_pEditor->GetDrivenMode();
	if (iState == IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX)
		str = _T("Pure PhysX Simulation");
	else if (iState == IPropPhysXObjBase::DRIVEN_BY_ANIMATION)
		str = _T("Pure Animation");
	else if (iState = IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX)
		str = _T("Partial PhysX Simulation");
	else
		assert(!"Unknown Driven Mode!");

	pRender->RenderText(x, y, gcnew String(str), dwColor);
	y -= 16;

#ifdef _ANGELICA3
	str = _T("SDK version: Angelica 31");
#else
	str = _T("SDK version: Angelica 21");
#endif
	pRender->RenderText(x, y, gcnew String(str), dwColor);
	y -= 16;

	str.Format(_T("Ragdoll Mode: %s"), m_pEditor->GetRPTModeText());
	pRender->RenderText(x, y, gcnew String(str), dwColor);
	y -= 16;
}

void CAPhysXLab31View::ObjectOpenOrSaveAs(const bool bIsOpen, CString& outPath)
{
	BOOL bISOpen = bIsOpen? TRUE : FALSE;
	DWORD dwFlags =  OFN_EXPLORER | OFN_NOCHANGEDIR;
	if (bIsOpen)
		dwFlags |= OFN_FILEMUSTEXIST;

	TCHAR szFilter[] = _T("PLO 文件(*.plo)|*.plo||");
	CFileDialog	FileDia(bISOpen, _T("*.plo"), NULL, dwFlags, szFilter, NULL);
	ACString strPath = _TAS2WC(af_GetBaseDir());
	strPath += _T("\\Prefab\\");
	FileDia.m_ofn.lpstrInitialDir = strPath;

	INT_PTR rtn = FileDia.DoModal();
	if (IDOK != rtn)
		return;
	outPath = FileDia.GetPathName();
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

	INT_PTR rtn = FileDia.DoModal();
	if (IDOK != rtn)
		return;

	if (bIsOpen)
		SceneOpen(FileDia.GetPathName(), FileDia.GetFileTitle());
	else
	{
		m_MEditor->SavePhysXDemoScene(gcnew String(_TWC2AS(FileDia.GetPathName())));
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

	m_MEditor->ResetScene();
	GetDocument()->SetTitle(NoName);

	CString filepath = path;
	std_TString strtmp = filepath.GetBuffer(0);
	System::String^ strFile = gcnew System::String(strtmp.c_str());
	if (0 != pFrame)
	{
		if (!m_HasCameraTreeItem)
		{
			CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
			CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
			pWSB1->OnNotifyBorn(*m_pCamCtrl);
			pWSB2->OnNotifyBorn(*m_pCamCtrl);
			m_HasCameraTreeItem = true;
		}
	}

	if (m_MEditor->LoadPhysXDemoScene(strFile))
	{
		OnModeDragMove();
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
		OnModeDragRotate();
	else if (m_status.ReadFlag(STATE_IS_ROTATION))
		OnModeDragScale();
	else if (m_status.ReadFlag(STATE_IS_SCALE))
		OnModeDragMove();
}

void CAPhysXLab31View::ChangeNextDriveMode()
{
	CEditor::DrivenMode dm = m_pEditor->GetDrivenMode();
	if (IPropPhysXObjBase::DRIVEN_BY_ANIMATION == dm)
		OnModeDvnPartphysx();
	else if (IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX == dm)
		OnModeDvnPurephysx();
	else if (IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX == dm)
		OnModeDvnAnimation();
}

bool CAPhysXLab31View::EnableAddObject() const
{
	if (!m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
		return false;

	if (0 != m_pEditor->GetPickedObject())
		return false;

	return true;
}

void CAPhysXLab31View::OnAddOperation(const ObjID objType)
{
	const ObjManager::ObjTypeInfo& objInfo = ObjManager::GetObjTypeInfo(objType);

	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	CFileDialog	FileDlg(TRUE, objInfo.strWCExt, NULL, dwFlags, objInfo.strFilter, NULL);

	ACString strPath = _TAS2WC(af_GetBaseDir());
	if (ObjManager::OBJ_TYPEID_PARTICLE == objType)
		strPath += _T("\\gfx\\");
	else
		strPath += _T("\\Models\\");
	FileDlg.m_ofn.lpstrInitialDir = strPath;
	if (IDOK != FileDlg.DoModal())
		return;

	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	AString strFullFile = _TWC2AS(FileDlg.GetPathName());
	AString strFile;
	af_GetRelativePath(strFullFile, strFile);
	if (ObjManager::OBJ_TYPEID_PARTICLE == objType)
	{
		AString strGFX = strFile.Left(4);
		if (0 == strGFX.CompareNoCase("gfx\\"))
			strFile.CutLeft(4);
	}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	
	MIPhysXObjBase^ mphysx = m_MEditor->CreateObject(static_cast<MObjTypeID>(objType), gcnew String(strFile), System::Drawing::Point(pt.x, pt.y));
	if (mphysx == nullptr)
	{
		MessageBox(_T("加载数据失败!"));
		return;
	}

	IPhysXObjBase* pObject = mphysx->GetIPhysXObjBase();	
	m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
	m_LastAddedItem.strPathName = strFile;
}

void CAPhysXLab31View::EntryPlayMode()
{
	OnModePlay();
}

void CAPhysXLab31View::eventOnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags)
{
	OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::eventOnKeyUp(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags)
{
	OnKeyUp(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::eventOnLButtonDown(int x, int y, unsigned int nFlags)
{
	OnLButtonDown(nFlags, CPoint(x, y));
}

void CAPhysXLab31View::eventOnLButtonUp(int x, int y, unsigned int nFlags)
{
	OnLButtonUp(nFlags, CPoint(x, y));
}

void CAPhysXLab31View::eventOnMButtonDown(int x, int y, unsigned int nFlags)
{
	OnMButtonDown(nFlags, CPoint(x, y));
}

void CAPhysXLab31View::eventOnMButtonUp(int x, int y, unsigned int nFlags)
{
	OnMButtonUp(nFlags, CPoint(x, y));
}

void CAPhysXLab31View::eventOnRButtonDblClk(int x, int y, unsigned int nFlags)
{
	OnRButtonDblClk(nFlags, CPoint(x, y));
}

void CAPhysXLab31View::eventOnRButtonDown(int x, int y, unsigned int nFlags)
{
	OnRButtonDown(nFlags, CPoint(x, y));
	m_LastReproPT.x = x;
	m_LastReproPT.y = y;
}

void CAPhysXLab31View::eventOnRButtonUp(int x, int y, unsigned int nFlags)
{
	OnRButtonUp(nFlags, CPoint(x, y));
}

void CAPhysXLab31View::eventOnMouseMove(int x, int y, unsigned int nFlags)
{
	OnMouseMove(nFlags, CPoint(x, y));
	m_LastReproPT.x = x;
	m_LastReproPT.y = y;
}

void CAPhysXLab31View::eventOnMouseWheel(int x, int y, int zDelta, unsigned int nFlags)
{
	OnMouseWheel(nFlags, zDelta, CPoint(x, y));
}

void CAPhysXLab31View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			return;
	}
	m_pRecording->LogEvent(OptMouse(OMT_LBTN_DOWN, point.x, point.y, nFlags));

	m_MEditor->OnLButtonDown(point.x, point.y, nFlags);
	CView::OnLButtonDown(nFlags, point);
}

void CAPhysXLab31View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			return;
	}
	m_pRecording->LogEvent(OptMouse(OMT_LBTN_UP, point.x, point.y, nFlags));

	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
		m_status.ClearFlag(STATE_IS_DRAGGING);

	m_MEditor->OnLButtonUp(point.x, point.y, nFlags);
	CView::OnLButtonUp(nFlags, point);
}

void CAPhysXLab31View::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			return;
	}
	CView::OnMButtonDown(nFlags, point);
}

void CAPhysXLab31View::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			return;
	}
	m_pRecording->LogEvent(OptMouse(OMT_MBTN_UP, point.x, point.y, nFlags));
	m_MEditor->OnMButtonUp(point.x, point.y, nFlags);
	CView::OnMButtonUp(nFlags, point);
}

void CAPhysXLab31View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
//	m_pGameProps->SetLastMousePos(point);
	m_pRecording->LogEvent(OptMouse(OMT_RBTN_DOWN, point.x, point.y, nFlags));
	m_status.RaiseFlag(STATE_IS_RBCLICK);
	if (!m_pRecording->IsReplaying())
		SetCapture();
	CView::OnRButtonDown(nFlags, point);
}

void CAPhysXLab31View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_pRecording->LogEvent(OptMouse(OMT_RBTN_UP, point.x, point.y, nFlags));
	if (!m_pRecording->IsReplaying())
		ReleaseCapture();
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
	{
		IObjBase* pObj = m_pEditor->GetRayHitObject();
		if (0 != pObj)
		{
			if (MK_CONTROL & nFlags)
			{
				m_pEditor->GetCmdQueue().SubmitBeforeExecution(CmdSelRemove());
				m_pSelGroup->RemoveObject(*pObj);
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
	else if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
	{
		if (m_status.ReadFlag(STATE_IS_RBCLICK))
		{
			if (m_pRecording->IsReplaying())
			{
				if (!m_pRecording->IsEventReproducing())
					return;
			}
			if (MK_SHIFT & nFlags)
			{
				m_pEditor->TrytoSwing();
			}
			m_MEditor->OnRButtonUp(point.x, point.y, nFlags);
		}
	}
	m_status.ClearFlag(STATE_IS_RBCLICK);
	m_status.ClearFlag(STATE_IS_RBDBLCLK);
	CView::OnRButtonUp(nFlags, point);
}

void CAPhysXLab31View::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			return;
	}
	m_status.RaiseFlag(STATE_IS_RBCLICK);
	m_status.RaiseFlag(STATE_IS_RBDBLCLK);
	IObjBase* pObj = m_pEditor->GetPickedObject();
	if ((0 != pObj) || (0 == m_pSelGroup))
	{
		CView::OnRButtonDblClk(nFlags, point);
		return;
	}
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
	{
		m_pRecording->LogEvent(OptMouse(OMT_RBTN_DBLCLK, point.x, point.y, nFlags));
		return;
	}

	MPhysRay physxRay;
	System::Drawing::Point pt(point.x, point.y);
	if (m_MEditor->ScreenPosToRay(pt, physxRay))
	{
		MPhysRayTraceHit hitInfo;
		m_MEditor->RayTraceObject(pt, physxRay, hitInfo, false);
		IObjBase* pHitObj = NULL;
		if (hitInfo.obj != nullptr)
			pHitObj = hitInfo.obj->GetIObjBase();

		if (MK_CONTROL & nFlags)
		{
			if (0 != pHitObj)
				m_pSelGroup->AppendObject(*pHitObj);
		}
		else
		{
			m_pSelGroup->ReplaceWithObject(pHitObj);
		}
	}

	CView::OnRButtonDblClk(nFlags, point);
}

void CAPhysXLab31View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	bool bSetFocus = false;
	bool bRotCamera = false;
	if (GetSafeHwnd() != ::GetFocus())
		bSetFocus = true;

	if (MK_RBUTTON & nFlags)
	{
		bRotCamera = true;
		m_status.ClearFlag(STATE_IS_RBCLICK);
	}
	else
	{
		m_MEditor->OnMouseMove(point.x, point.y, nFlags);
	}

	if (bSetFocus)
		SetFocus();

	bool bIsReproEvent = false;
	if (0 != m_pRecording)
	{
		if (bSetFocus || bRotCamera)
			m_pRecording->LogEvent(OptMouse(OMT_MOVE, point.x, point.y, nFlags));

		bIsReproEvent = m_pRecording->IsEventReproducing();
		if (bRotCamera)
		{
			CSize dis = point - m_LastPT;
			if (bIsReproEvent)
				dis = point - m_LastReproPT;

			CamOptTask_RotateCamera ot(dis.cx, dis.cy);
			m_pEditor->GetCamCtrlAgent()->ExecuteCameraOperation(ot);
		}
	}

	if (!bIsReproEvent)
		m_LastPT = point;
	CView::OnMouseMove(nFlags, point);
}

BOOL CAPhysXLab31View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	m_pRecording->LogEvent(OptMouse(pt.x, pt.y, zDelta, nFlags));
	m_MEditor->OnMouseWheel(pt.x, pt.y, zDelta, nFlags);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CAPhysXLab31View::OnFileNew()
{
	// TODO: Add your command handler code here
	if (IDNO == MessageBox(_T("确定要重置系统么？"), _T("提示"), MB_YESNO))
		return;

	m_MEditor->ResetScene();
	GetDocument()->SetTitle(NoName);
	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (0 != pFrame)
	{
		if (m_HasCameraTreeItem)
		{
			CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
			CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
			pWSB1->OnNotifyDie(*m_pCamCtrl);
			pWSB2->OnNotifyDie(*m_pCamCtrl);
			m_HasCameraTreeItem = false;
		}
	}
}

void CAPhysXLab31View::OnFileOpen()
{
	// TODO: Add your command handler code here
	SceneOpenOrSaveAs(true);
}

void CAPhysXLab31View::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->IsRuntime())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CAPhysXLab31View::OnFileSave()
{
	// TODO: Add your command handler code here
	const char* pPath = m_pEditor->GetActiveSceneFile();
	if (0 == pPath)
	{
		OnFileSaveAs();
		return;
	}

	if (IDYES == AfxMessageBox(_T("确定覆盖当前文档？"), MB_YESNO | MB_ICONQUESTION))
		m_MEditor->SavePhysXDemoScene(gcnew String(pPath));
}

void CAPhysXLab31View::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
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
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnFileReplay()
{
	// TODO: Add your command handler code here
	DWORD dwFlags =  OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;

	TCHAR szFilter[] = _T("所有记录文件(*.rec *.rpi)|*.r*|REC 文件(*.rec)|*.rec|RPI 文件(*.rpi)|*.rpi||");
	CFileDialog	FileDia(TRUE, _T("*.rec"), NULL, dwFlags, szFilter, NULL);
	ACString strPath = _TAS2WC(af_GetBaseDir());
	strPath += _T("\\Rec\\");
	FileDia.m_ofn.lpstrInitialDir = strPath;

	INT_PTR rtn = FileDia.DoModal();
	if (IDOK != rtn)
		return;

	CString path = FileDia.GetPathName();
	CString title = FileDia.GetFileTitle();
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	if (!CFile::GetStatus(path, CFileStatus()))
		return;

	m_MEditor->ResetScene();
	GetDocument()->SetTitle(NoName);

	std_TString strtmp = path.GetBuffer(0);
	System::String^ strFile = gcnew System::String(strtmp.c_str());
	if (0 != pFrame)
	{
		if (!m_HasCameraTreeItem)
		{
			CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
			CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
			pWSB1->OnNotifyBorn(*m_pCamCtrl);
			pWSB2->OnNotifyBorn(*m_pCamCtrl);
			m_HasCameraTreeItem = true;
		}
	}

	if (m_MEditor->Replay(strFile))
		GetDocument()->SetTitle(title);
}

void CAPhysXLab31View::OnUpdateFileReplay(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
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
	m_pEditor->GetCmdQueue().Undo();
}

void CAPhysXLab31View::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->GetCmdQueue().UndoEnable())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditRedo()
{
	// TODO: Add your command handler code here
	m_pEditor->GetCmdQueue().Redo();
}

void CAPhysXLab31View::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->GetCmdQueue().RedoEnable())
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
	if (m_MEditor->LoadTerrainAndPhysXTerrain(gcnew String(strFile)))
	{
		if (!m_HasCameraTreeItem)
		{
			CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
			if (0 != pFrame)
			{
				CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
				CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
				pWSB1->OnNotifyBorn(*m_pCamCtrl);
				pWSB2->OnNotifyBorn(*m_pCamCtrl);
				m_HasCameraTreeItem = true;
			}
		}
	}
}

void CAPhysXLab31View::OnEditPhysxtrngen()
{
	// TODO: Add your command handler code here
	CDlgPhysXTrnBuilder dlg(m_pEditor->GetTerrainFile(), *m_pEditor->GetRender()->GetA3DEngine());
	dlg.DoModal();
}

void CAPhysXLab31View::OnEditTscenegen()
{
	// TODO: Add your command handler code here
	CDlgSceneBuilder dlg;
	INT_PTR rtn = dlg.DoModal();
	if (IDOK != rtn)
		return;

	AString strTN = _TWC2AS(dlg.GetTerrianName());
	if (strTN.IsEmpty())
		return;
	TString strSN = dlg.GetSceneName();
	if (strSN.IsEmpty())
		return;

	const CDlgSceneBuilder::strList& strModels = dlg.GetModels();
	size_t nModels = strModels.size();
	if (0 == nModels)
		return;

	OnFileNew();
	AString strTRNName;
	strTRNName.Format("Maps\\%s\\%s.trn2", strTN, strTN);
	if (m_MEditor->LoadTerrainAndPhysXTerrain(gcnew String(strTRNName)))
	{
		if (!m_HasCameraTreeItem)
		{
			CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
			if (0 != pFrame)
			{
				CWSBTreeClassified* pWSB1 = pFrame->GetWSBClassified();
				CWSBTreeParentChild* pWSB2 = pFrame->GetWSBParentChild();
				pWSB1->OnNotifyBorn(*m_pCamCtrl);
				pWSB2->OnNotifyBorn(*m_pCamCtrl);
				m_HasCameraTreeItem = true;
			}
		}
		GetDocument()->SetTitle(strSN);

		const float fStep = 5.0f;
		int n = int(float(NxMath::sqrt(float(nModels))) + 0.5f); 
		float offset = fStep * n * 0.5f;
		Vector3 pos(-offset, 0, offset);

		bool bIsNPC = true;
		IObjBase* pObj = 0; 
		CPhysXObjDynamic* pDynObj = 0;
		APhysXCommonDNet::MIObjBase^ pMObj = nullptr;

		CDlgSceneBuilder::strList::const_iterator it = strModels.begin();
		CDlgSceneBuilder::strList::const_iterator itEnd = strModels.end();
		for (int i = 0; (i < n) && (it != itEnd); ++i)
		{
			pos.X = -offset;
			for (int j = 0; (j < n) && (it != itEnd); ++j)
			{
				pMObj = m_MEditor->CreateObjectRuntime(MObjTypeID::OBJ_TYPEID_ECMODEL, gcnew String(*it), pos, true, false, nullptr, nullptr, false);
				m_MEditor->UpdatePropForBuildScene(pMObj);
				++it;
				pos.X += fStep;
			}
			pos.Z -= fStep;
		}

		TString strSavePath;
		strSavePath.Format(_T("Scenes\\%s.pls"), strSN);
		m_MEditor->SavePhysXDemoScene(gcnew String(strSavePath));
	}
}

void CAPhysXLab31View::OnEditSaveobj()
{
	// TODO: Add your command handler code here
	CString strPathName;
	ObjectOpenOrSaveAs(false, strPathName);
	if (!strPathName.IsEmpty())
	{
		String^ pathName = gcnew String(_TWC2AS(strPathName));
		if (nullptr != pathName)
			m_MEditor->SaveAnObject(pathName);
	}
}

void CAPhysXLab31View::OnUpdateEditSaveobj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditLoadobj()
{
	// TODO: Add your command handler code here
	CString strPathName;
	ObjectOpenOrSaveAs(true, strPathName);
	if (!strPathName.IsEmpty())
	{
		String^ pathName = gcnew String(_TWC2AS(strPathName));
		if (nullptr != pathName)
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			IObjBase* pObject = m_MEditor->LoadAnObject(pathName, Drawing::Point(pt.x, pt.y));
			if (0 != pObject)
				m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
		}
	}
}

void CAPhysXLab31View::OnUpdateEditLoadobj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddstaticobj()
{
	// TODO: Add your command handler code here
	OnAddOperation(ObjManager::OBJ_TYPEID_STATIC);
}

void CAPhysXLab31View::OnUpdateEditAddstaticobj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddsmd()
{
	// TODO: Add your command handler code here
	OnAddOperation(ObjManager::OBJ_TYPEID_SKINMODEL);
}

void CAPhysXLab31View::OnUpdateEditAddsmd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddecm()
{
	// TODO: Add your command handler code here
	OnAddOperation(ObjManager::OBJ_TYPEID_ECMODEL);
}

void CAPhysXLab31View::OnUpdateEditAddecm(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddphysxgfx()
{
	// TODO: Add your command handler code here
	OnAddOperation(ObjManager::OBJ_TYPEID_PARTICLE);
}

void CAPhysXLab31View::OnUpdateEditAddphysxgfx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddphysxff()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	AString strFile("");
	MIPhysXObjBase^ mphysx = m_MEditor->CreateObject(static_cast<MObjTypeID>(ObjManager::OBJ_TYPEID_FORCEFIELD), gcnew String(strFile), System::Drawing::Point(pt.x, pt.y));
	if (mphysx == nullptr)
	{
		MessageBox(_T("加载数据失败!"));
		return;
	}

	IPhysXObjBase* pObject = mphysx->GetIPhysXObjBase();	
	m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
	m_LastAddedItem.strPathName = strFile;
}

void CAPhysXLab31View::OnUpdateEditAddphysxff(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddrgnbox()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MRegion^ region = m_MEditor->CreateRegion(Drawing::Point(pt.x, pt.y));
	region->SetShapeType(MShapeType::ST_BOX);
	IObjBase* pObject = region->GetIObjBase();
	if (0 != pObject)
	{
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
		m_LastAddedItem.rgnST = CRegion::ST_BOX;
	}
}

void CAPhysXLab31View::OnUpdateEditAddrgnbox(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddrgnsphere()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MRegion^ region = m_MEditor->CreateRegion(Drawing::Point(pt.x, pt.y));
	region->SetShapeType(MShapeType::ST_SPHERE);
	IObjBase* pObject = region->GetIObjBase();
	if (0 != pObject)
	{
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
		m_LastAddedItem.rgnST = CRegion::ST_SPHERE;
	}
}

void CAPhysXLab31View::OnUpdateEditAddrgnsphere(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddrgncapsule()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MRegion^ region = m_MEditor->CreateRegion(Drawing::Point(pt.x, pt.y));
	region->SetShapeType(MShapeType::ST_CAPSULE);
	IObjBase* pObject = region->GetIObjBase();
	if (0 != pObject)
	{
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
		m_LastAddedItem.rgnST = CRegion::ST_CAPSULE;
	}
}

void CAPhysXLab31View::OnUpdateEditAddrgncapsule(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAdddummy()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MIObjBase^ dummy = m_MEditor->CreateObject(MObjTypeID::OBJ_TYPEID_DUMMY, Drawing::Point(pt.x, pt.y));
	IObjBase* pObject = dummy->GetIObjBase();
	if (0 != pObject)
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
}

void CAPhysXLab31View::OnUpdateEditAdddummy(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddsmsocket()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MIObjBase^ socket = m_MEditor->CreateObject(MObjTypeID::OBJ_TYPEID_SMSOCKET, Drawing::Point(pt.x, pt.y));
	IObjBase* pObject = socket->GetIObjBase();
	if (0 != pObject)
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
}

void CAPhysXLab31View::OnUpdateEditAddsmsocket(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddsocketsm()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MIObjBase^ socket = m_MEditor->CreateObject(MObjTypeID::OBJ_TYPEID_SOCKETSM, Drawing::Point(pt.x, pt.y));
	IObjBase* pObject = socket->GetIObjBase();
	if (0 != pObject)
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
}

void CAPhysXLab31View::OnUpdateEditAddsocketsm(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddspotlight()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MIObjBase^ objSpotLight = m_MEditor->CreateObject(MObjTypeID::OBJ_TYPEID_SPOTLIGHT, Drawing::Point(pt.x, pt.y));
	IObjBase* pObject = objSpotLight->GetIObjBase();
	if (0 != pObject)
		m_LastAddedItem.nTypeID = pObject->GetProperties()->GetObjType();
}

void CAPhysXLab31View::OnUpdateEditAddspotlight(CCmdUI *pCmdUI)
{
	EnableAddObject()? pCmdUI->Enable(TRUE) : pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnEditAddlastobj()
{
	// TODO: Add your command handler code here
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	System::Drawing::Point point(pt.x, pt.y);
	if (ObjManager::IsPhysXObjType(ObjManager::ObjTypeID(m_LastAddedItem.nTypeID)))
	{
		String^ strPath = gcnew String(m_LastAddedItem.strPathName.operator const char*());
		m_MEditor->CreateObject(MObjTypeID(m_LastAddedItem.nTypeID), strPath, point);
	}
	else if (ObjManager::OBJ_TYPEID_REGION == m_LastAddedItem.nTypeID)
	{
		MRegion^ rgn = m_MEditor->CreateRegion(point);
		rgn->SetShapeType(static_cast<MShapeType>(m_LastAddedItem.rgnST));
	}
	else
	{
		m_MEditor->CreateObject(MObjTypeID(m_LastAddedItem.nTypeID), point);
	}
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
	m_MEditor->EnterEditMode();
	m_pRecording->RecordEnable(true);

	const POINT& pt = m_LastPT;
	MPhysRay physxRay;
	System::Drawing::Point point(pt.x, pt.y);
	if (m_MEditor->ScreenPosToRay(point, physxRay))
	{
		MPhysRayTraceHit hitInfo;
		m_MEditor->RayTraceObject(point, physxRay, hitInfo, false);
	}
}

void CAPhysXLab31View::OnUpdateModeEdit(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_EMPTY))
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
		return;
	}
	
	bool bIsEdit = m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF);
	pCmdUI->SetCheck(bIsEdit);
	pCmdUI->Enable(!bIsEdit);
}

void CAPhysXLab31View::OnModeSimulate()
{
	// TODO: Add your command handler code here
	m_pRecording->RecordEnable(false);
	OnModePlay();
}

void CAPhysXLab31View::OnUpdateModeSimulate(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	OnUpdateModePlay(pCmdUI);
}

void CAPhysXLab31View::OnModePlay()
{
	m_MEditor->EntryPlayMode();
}

void CAPhysXLab31View::OnUpdateModePlay(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
	{
		pCmdUI->SetCheck(TRUE);
		pCmdUI->Enable(FALSE);
	}
	else if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
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
	m_pEditor->EnablePhysXProfile(!m_pEditor->IsPhysXProfileEnabled());
}

void CAPhysXLab31View::OnUpdateModePxProfile(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pEditor->IsPhysXProfileEnabled());
}

void CAPhysXLab31View::OnModePxDebug()
{
	// TODO: Add your command handler code here
	m_MEditor->EnablePhysXDebugRender(!m_MEditor->IsPhysXDebugRenderEnabled());
}

void CAPhysXLab31View::OnUpdateModePxDebug(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pEditor->IsPhysXDebugRenderEnabled());
}

void CAPhysXLab31View::OnModeDragMove()
{
	// TODO: Add your command handler code here
	m_status.RaiseFlag(STATE_IS_MOVEMENT);
	m_status.ClearFlag(STATE_IS_ROTATION);
	m_status.ClearFlag(STATE_IS_SCALE);
	m_pSelGroup->SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE);
}

void CAPhysXLab31View::OnUpdateModeDragMove(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pSelGroup->QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE));
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
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
	m_pSelGroup->SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_ROTATE);
}

void CAPhysXLab31View::OnUpdateModeDragRotate(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pSelGroup->QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_ROTATE));
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
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
	m_pSelGroup->SetRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_SCALE);
}

void CAPhysXLab31View::OnUpdateModeDragScale(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pSelGroup->QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_SCALE));
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnModePhysxhw()
{
	// TODO: Add your command handler code here
	m_pEditor->EnablePhysXHW(!m_pEditor->IsPhysHWEnabled());
}

void CAPhysXLab31View::OnUpdateModePhysxhw(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pEditor->IsPhysHWEnabled());
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CAPhysXLab31View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	bool bIsReplayNotReproEvent = false;
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			bIsReplayNotReproEvent = true;
	}
	if (!bIsReplayNotReproEvent)
	{
		if ('8' == nChar)
			m_pEditor->ChangeShootStuffType();
		else if ('9' == nChar)
			m_pEditor->ChangeShootMode();
		else if ('7' == nChar)
		{
			CMainActor* pMA = m_pEditor->GetCurrentActor();
			if (0 != pMA)
				pMA->GetActorBase()->SwitchFootIKYawCorrection();
		}
		else if ('5' == nChar)
		{
			CMainActor* pMA = m_pEditor->GetCurrentActor();
			if (0 != pMA)
				pMA->GetActorBase()->IncreFootIKMaxPitchDeg();
		}
		else if ('6' == nChar)
		{
			CMainActor* pMA = m_pEditor->GetCurrentActor();
			if (0 != pMA)
				pMA->GetActorBase()->DecreFootIKMaxPitchDeg();
		}
	}

	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_OFF))
		OnKeyDownModeEdit(nChar, nRepCnt, nFlags);
	else if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
	{
		m_pRecording->LogEvent(OptKeyBoard(true, nChar, nRepCnt, nFlags));
		OnKeyDownModePlay(nChar, nRepCnt, nFlags);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	bool bIsReplayNotReproEvent = false;
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			bIsReplayNotReproEvent = true;
	}
	if (!bIsReplayNotReproEvent)
	{
		if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON))
		{
			if (VK_F9 == nChar)
			{
				m_pRecording->LogEvent(OptKeyBoard(false, nChar, nRepCnt, nFlags));
				if (m_pRecording->IsRecording())
				{
					if (m_pRecording->IsRecordingEpisode())
						m_pRecording->EpisodeEnd();
					else
					{
						AString strFile;
						m_pEditor->MakeRecordingFileName(true, strFile);
						m_pRecording->EpisodeBegin(strFile);
					}
				}
			}
		}
	}

//	if ('F' == nChar)
//		m_game.ShowFFShapes(false);
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::OnKeyDownModeEdit(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_DELETE:
		m_MEditor->RemoveAllSelectedObjects();
		break;
	case VK_F7:
		ChangeNextDragOperation();
		break;
	case 'C':
		if (GetKeyState(VK_CONTROL) < 0)
			m_MEditor->CloneObjects();
		break;
	case 'F':
//		m_editor.ShowFFShapes(true);
		{
			//Test codes
/*			APtrArray<IObjBase*> arrayObj;
			ObjManager* pObjMgr = ObjManager::GetInstance();
			pObjMgr->GetRootLiveObjects(arrayObj, ObjManager::OBJ_TYPEID_STATIC);
			int nc = arrayObj.GetSize();
			if (2 < nc)
			{
				IObjBase* pP = arrayObj[0];
				pP->AddChild(arrayObj[1]);

				APtrArray<IObjBase*> arrayObj2;
				pObjMgr->GetRootLiveObjects(arrayObj2, ObjManager::OBJ_TYPEID_STATIC);
				int nc2 = arrayObj2.GetSize();
			}*/
		}
		break;
	case 'K':
//		m_editor.OnKeyDownEditScenario();
		break;
	}
}

void CAPhysXLab31View::OnKeyDownModePlay(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bIsReplayNotReproEvent = false;
	if (m_pRecording->IsReplaying())
	{
		if (!m_pRecording->IsEventReproducing())
			bIsReplayNotReproEvent = true;
	}
	switch (nChar)
	{
	case 'C':
		OnModeCFollowing();
		break;
	case 'P':
		m_MEditor->ChangePhysXPauseState();
		break;
	case 'O':
		m_MEditor->PhysXSimASingleStep();
		break;
	case 'R':
		if (!bIsReplayNotReproEvent)
			m_MEditor->SwitchMainActorWalkRun();
		break;
	case 'Z':
		if (!bIsReplayNotReproEvent)
			m_MEditor->RenderNextFrameInfo();
		break;
	case VK_TAB:
		if (!bIsReplayNotReproEvent)
			m_MEditor->SwitchMainActor();
		break;
	case VK_OEM_1:  // ';:' for US
		m_MEditor->ControlModelClothes();
		break;
	case VK_OEM_2: // '/?' for US
		{
			bool bIsReplayNotReproEvent = false;
			if (m_pRecording->IsReplaying())
			{
				if (!m_pRecording->IsEventReproducing())
					bIsReplayNotReproEvent = true;
			}
			m_MEditor->ChangeCameraMode(bIsReplayNotReproEvent);
		}
		break;
	case '1':
		m_MEditor->SetRPTMode(1);
		break;
	case '2':
		m_MEditor->SetRPTMode(2);
		break;
	case '3':
		m_MEditor->SetRPTMode(3);
		break;
	case '4':
		m_MEditor->SetRPTMode(4);
		break;
	case '0':
		m_MEditor->EnumNextRPTMode();
		break;
	case 'B':
//		m_MEditor->SwitchAllPhysXState();
		break;
	case VK_F4:
		if (!bIsReplayNotReproEvent)
			ChangeNextDriveMode();
		break;
	case VK_F8:
		{
			// to Coredump PhysX data to the file...
			m_MEditor->DumpPhysX();
			break;
		}
	default:
		
		break;
	}
	if (!bIsReplayNotReproEvent)
		m_MEditor->OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXLab31View::OnModeDvnAnimation()
{
	// TODO: Add your command handler code here
	m_MEditor->SetDrivenMode(MDrivenMode::DRIVEN_BY_ANIMATION);
}

void CAPhysXLab31View::OnUpdateModeDvnAnimation(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->IsRuntime())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_MEditor->QueryDrivenMode(MDrivenMode::DRIVEN_BY_ANIMATION));
}

void CAPhysXLab31View::OnModeDvnPartphysx()
{
	// TODO: Add your command handler code here
	m_MEditor->SetDrivenMode(MDrivenMode::DRIVEN_BY_PART_PHYSX);
}

void CAPhysXLab31View::OnUpdateModeDvnPartphysx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->IsRuntime())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_MEditor->QueryDrivenMode(MDrivenMode::DRIVEN_BY_PART_PHYSX));
}

void CAPhysXLab31View::OnModeDvnPurephysx()
{
	// TODO: Add your command handler code here
	m_MEditor->SetDrivenMode(MDrivenMode::DRIVEN_BY_PURE_PHYSX);
}

void CAPhysXLab31View::OnUpdateModeDvnPurephysx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pEditor->IsRuntime())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_MEditor->QueryDrivenMode(MDrivenMode::DRIVEN_BY_PURE_PHYSX));
}

void CAPhysXLab31View::OnModeCFollowing()
{
	// TODO: Add your command handler code here
	m_MEditor->ChangeCameraMode(false);
}

void CAPhysXLab31View::OnUpdateModeCFollowing(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	bool bIsPlay(m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_PLAY_ON));
	pCmdUI->Enable(bIsPlay);

	bool bIsBinding = m_pCamCtrl->QueryMode(MODE_BINDING);
	pCmdUI->SetCheck(bIsBinding);
}

void CAPhysXLab31View::OnScenesetting()
{
	CDlgSceneSetting dlg(m_pEditor);
	dlg.DoModal();
}

void CAPhysXLab31View::OnUpdateScenesetting(CCmdUI *pCmdUI)
{
	if (m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_EMPTY))
		pCmdUI->Enable(false);
	else
		pCmdUI->Enable(true);
}

void CAPhysXLab31View::OnModeAphysxsample()
{
	OnFileSave();
	TString dir = _TAS2WC(af_GetBaseDir());
#ifdef _DEBUG
	TString sampleprogram = _T("APhysXSample_d.exe");
#else
	TString sampleprogram = _T("APhysXSample.exe");
#endif

	TString asf;
	asf.Empty();
	const char* pStr = m_pEditor->GetActiveSceneFile();
	if (0 != pStr)
		asf = _TAS2WC(pStr);

	ShellExecute(NULL, _T("open"), sampleprogram, asf, dir, SW_SHOW);
}

void CAPhysXLab31View::OnUpdateModeAphysxsample(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_pEditor->QuerySceneMode(CEditor::SCENE_MODE_EMPTY));
}


void CAPhysXLab31View::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	if (0 != m_pCamCtrl)
		m_pCamCtrl->EnableMoveCamera(false);
}

void CAPhysXLab31View::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	if (0 != m_pCamCtrl)
		m_pCamCtrl->EnableMoveCamera(true);

	// TODO: Add your message handler code here
}

void CAPhysXLab31View::OnViewWire()
{
	// TODO: Add your command handler code here
	m_pEditor->EnableShowTerrianGrid(!m_pEditor->IsShowTerrianGridEnabled());
}

void CAPhysXLab31View::OnUpdateViewWire(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	(0 == m_pEditor->GetTerrain())? pCmdUI->Enable(FALSE) : pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_pEditor->IsShowTerrianGridEnabled());
}
