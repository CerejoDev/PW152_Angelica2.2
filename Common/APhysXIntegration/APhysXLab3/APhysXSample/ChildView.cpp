// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "APhysXSample.h"
#include "ChildView.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace System;
using namespace System::Collections::Generic;
using namespace APhysXCommonDNet;
// CChildView

CChildView::CChildView() : /*m_pCamCtrl(NULL),*/ m_dwRenderStart(0), m_dwLastRender(0)
{
	m_bInit = false;
	//m_MScene = gcnew APhysXCommonDNet::MScene(&m_scene);
	m_MScene = gcnew APhysXCommonDNet::MScene();
	m_pMCamCtrl = nullptr;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, &CChildView::OnFileLoad)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()




// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::RenderExtra(APhysXCommonDNet::MRender^ render)
{
	System::String^ str = gcnew System::String("event handler test");
	render->RenderText(20, 50, str, 0x80ff0000);
}

bool CChildView::Init()
{
	//return true;

	System::IntPtr  handle(AfxGetInstanceHandle());
	System::IntPtr hwnd(m_hWnd);

	//APhysXEngine* pEngine = APhysXEngine::GetPtrInstance();
	//if (!m_scene.Init(AfxGetInstanceHandle(), m_hWnd, false))
	if (!m_MScene->Init(handle, hwnd, false))
	{
		//a_LogOutput(1, "CChildView::Init: Failed to create scene!");
		return false;
	}
	m_MScene->renderExtraEvent += gcnew RenderExtraHandler(&CChildView::RenderExtra);
	m_pMCamCtrl = m_MScene->GetMCameraController();

	return true;

}
void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (0 == cx || 0 == cy)
		return;

	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	if (m_MScene->GetMRender() != nullptr)
	{
		m_MScene->GetMRender()->ResizeDevice(cx, cy);
	}

	if (static_cast<MCameraController^>(m_pMCamCtrl) != nullptr)
		m_pMCamCtrl->SetScreenSize(cx, cy);

}

bool CChildView::Tick()
{
	if (!m_bInit)
		return false;

	return m_MScene->Tick();

}
bool CChildView::Render()
{
	if (!m_bInit)
		return false;

	return m_MScene->Render();
	//return m_scene.Render();
}
void CChildView::OnFileLoad()
{
	DWORD dwFlags =  OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;

	TCHAR szFilter[] = _T("PLS нд╪Ч(*.pls)|*.pls||");
	CFileDialog	FileDia(TRUE, _T("*.pls"), NULL, dwFlags, szFilter, NULL);
	char szCurPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurPath);
	CString strPath(szCurPath);
	strPath += _T("\\Scenes\\");
	FileDia.m_ofn.lpstrInitialDir = strPath;

	if ( static_cast<MCameraController^>(m_pMCamCtrl) != nullptr)
		m_pMCamCtrl->EnableMoveCamera(false);
	INT_PTR rtn = FileDia.DoModal();
	if (static_cast<MCameraController^>(m_pMCamCtrl) != nullptr)
		m_pMCamCtrl->EnableMoveCamera(true);

	if (IDOK != rtn)
		return;

	FileLoad(FileDia.GetPathName());
}

void CChildView::FileLoad(CString filename)
{
	m_MScene->ResetScene();
	m_bInit = false;

	std::string strtmp = filename.GetBuffer(0);
	System::String^ strFile = gcnew System::String(strtmp.c_str());
	if (m_MScene->LoadPhysXDemoScene(strFile))
	{
		CRect rect;
		GetClientRect(&rect);
		m_pMCamCtrl->SetScreenSize(rect.Width(), rect.Height());

		m_MScene->EntryPlayMode();

		m_bInit = true;
		return;
	}
	else
	{
		MessageBox(_T("failed to load file"), _T("error"));
	}
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();

	m_MScene->Release();
	m_MScene = nullptr;
	//GC::Collect();
	//m_scene.Release();
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bInit)
		m_MScene->OnLButtonDown(point.x, point.y, nFlags);

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bInit)
		m_MScene->OnLButtonUp(point.x, point.y, nFlags);

	CWnd::OnLButtonUp(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	m_LastPt = point;

	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{

	if (m_bInit)
	{
		if (MK_RBUTTON & nFlags)
		{
			// Camera view rotation
			if (static_cast<MCameraController^>(m_pMCamCtrl) != nullptr)
			{
				CSize dis = point - m_LastPt;
				m_pMCamCtrl->RotateCamera(dis.cx, dis.cy);
			}

			m_LastPt = point;

			CWnd::OnMouseMove(nFlags, point);
			return;
		}
		m_MScene->OnMouseMove(point.x, point.y, nFlags);
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_bInit)
		m_MScene->OnMouseWheel(pt.x, pt.y, zDelta, nFlags);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bInit)
	{
		switch (nChar)
		{
		case 'C':
			m_MScene->ChangeCameraMode();
			break;
		case 'P':
			m_MScene->ChangePhysXPauseState();
			break;
		case 'O':
			m_MScene->PhysXSimASingleStep();
			break;
		case 'R':
			//m_scene.SwitchMainActorWalkRun();
			m_MScene->SwitchMainActorWalkRun();
			break;
		case 'J':
			m_MScene->JumpMainActor();
			break;		
		case VK_TAB:
			m_MScene->SwitchMainActor();
			break;
		case VK_OEM_1:  // ';:' for US
			//		m_game.ControlModelClothes();
			break;
		case VK_ADD:
			break;
		case VK_SUBTRACT:
			break;

		case VK_F5:
			m_MScene->EnablePhysXProfile(!m_MScene->IsPhysXProfileEnabled());
			break;

		case VK_F6:
			m_MScene->EnablePhysXDebugRender(!m_MScene->IsPhysXDebugRenderEnabled());
			break;

		case 'I':
			m_MScene->SwitchMainActorCCType(MCCType::CC_UNKNOWN_TYPE);
			break;
		case VK_NUMPAD5:
			m_MScene->SwitchMainActorCCType(MCCType::CC_TOTAL_EMPTY);
			break;
		case VK_NUMPAD6:
			m_MScene->SwitchMainActorCCType(MCCType::CC_APHYSX_EXTCC);
			break;
		case VK_NUMPAD7:
			m_MScene->SwitchMainActorCCType(MCCType::CC_APHYSX_NXCC);
			break;
		case VK_NUMPAD8:
			m_MScene->SwitchMainActorCCType(MCCType::CC_APHYSX_DYNCC);
			break;
		case VK_NUMPAD9:
			m_MScene->SwitchMainActorCCType(MCCType::CC_BRUSH_CDRCC);
			break;
			//	case 'M':
			//		m_game.MoveObj();
			break;
		}

		m_MScene->OnKeyDown(nChar, nRepCnt, nFlags);

	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bInit)
	{
		m_MScene->OnKeyUp(nChar, nRepCnt, nFlags);
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
