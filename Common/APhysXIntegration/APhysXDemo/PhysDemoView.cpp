// PhysDemoView.cpp : implementation of the CPhysDemoView class
//

#include "Global.h"
#include "PhysDemo.h"
#include "PhysDemoDoc.h"
#include "PhysDemoView.h"
#include "Render.h"
#include "Game.h"
#include "PhysObjectMan.h"
#include "PhysStaticObject.h"
#include "PhysTerrain.h"
#include "PhysModelObject.h"
#include "PhysParticleObject.h"

#include "MainFrm.h"

#include "DlgPhysXTrnGen.h"

#include "APhysX.h"

#include <ALog.h>
#include <A3DDevice.h>
#include <A3DCursor.h>
#include <A3DCamera.h>
#include <A3DFuncs.h>
#include <A3DViewport.h>
#include <CommonFileName.h>

#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"


enum
{
	APHYSX_DEMO_RUNMODE_NORMAL,
	APHYSX_DEMO_RUNMODE_SINGLESTEP,

};

static void ScreenPosToRay(A3DViewport* pViewPort, int x, int y, PhysRay& ray)
{
	
	ASSERT(pViewPort);

	A3DVECTOR3 avNearPos((float)x, (float)y, 0.0f), avFarPos((float)x, (float)y, 1.0f);
	pViewPort->InvTransform(avNearPos, avNearPos);
	pViewPort->InvTransform(avFarPos, avFarPos);

	ray.vStart = avNearPos;
	ray.vDelta = avFarPos - avNearPos;
}

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView

IMPLEMENT_DYNCREATE(CPhysDemoView, CView)

BEGIN_MESSAGE_MAP(CPhysDemoView, CView)
	//{{AFX_MSG_MAP(CPhysDemoView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_EDIT_ADDSTATICOBJ, OnEditAddstaticobj)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_EDIT_PHYSXTRNGEN, OnEditPhysxtrngen)
	ON_COMMAND(ID_EDIT_ADDTERRAIN, OnEditAddterrain)
	ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
	ON_COMMAND(ID_MODE_SIMULATE, OnModeSimulate)
	ON_COMMAND(ID_MODE_PLAY, OnModePlay)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
	ON_UPDATE_COMMAND_UI(ID_MODE_SIMULATE, OnUpdateModeSimulate)
	ON_UPDATE_COMMAND_UI(ID_MODE_PLAY, OnUpdateModePlay)
	ON_COMMAND(ID_MODE_PHYSXPROFILE, OnModePhysxprofile)
	ON_UPDATE_COMMAND_UI(ID_MODE_PHYSXPROFILE, OnUpdateModePhysxprofile)
	ON_COMMAND(ID_MODE_PHYSXDEBUGRENDER, OnModePhysxdebugrender)
	ON_UPDATE_COMMAND_UI(ID_MODE_PHYSXDEBUGRENDER, OnUpdateModePhysxdebugrender)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDTERRAIN, OnUpdateEditAddterrain)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSTATICOBJ, OnUpdateEditAddstaticobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHYSXTRNGEN, OnUpdateEditPhysxtrngen)
	ON_COMMAND(ID_EDIT_ADDSMD, OnEditAddsmd)
	ON_COMMAND(ID_EDIT_ADDLASTOBJ, OnEditAddlastobj)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDSMD, OnUpdateEditAddsmd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDLASTOBJ, OnUpdateEditAddlastobj)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_EDIT_ADDPHYSXGFX, OnEditAddphysxgfx)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPHYSXGFX, OnUpdateEditAddphysxgfx)
	ON_COMMAND(ID_EDIT_PICKANDROTATE, OnEditPickandrotate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PICKANDROTATE, OnUpdateEditPickandrotate)
	ON_COMMAND(ID_EDIT_PICKANDMOVE, OnEditPickandmove)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PICKANDMOVE, OnUpdateEditPickandmove)
	
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView construction/destruction

CPhysDemoView::CPhysDemoView()
{
	m_bRightPressed	= false;
	m_ptMouseLock.x = 0;
	m_ptMouseLock.y = 0;
	m_ptMouseLast.x = 0;
	m_ptMouseLast.y = 0;

	m_dwRenderStart	= 0;
	m_dwLastRender	= 0;

	m_bHitTerrainEditMode = true;
}

CPhysDemoView::~CPhysDemoView()
{
}

BOOL CPhysDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView drawing

void CPhysDemoView::OnDraw(CDC* pDC)
{
	CPhysDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView diagnostics

#ifdef _DEBUG
void CPhysDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CPhysDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPhysDemoDoc* CPhysDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhysDemoDoc)));
	return (CPhysDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView message handlers

void CPhysDemoView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (!cx || !cy)
		return;
	
	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	g_Render.ResizeDevice(cx, cy);
}

void CPhysDemoView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if (!g_Render.EngineIsReady())
	{
		//	Create render
		if (!g_Render.Init(AfxGetInstanceHandle(), m_hWnd, false))
		{
			a_LogOutput(1, "CSkeletonEditorView::OnInitialUpdate, Failed to create render");
			return;
		}
	}
}

//	Logic tick
bool CPhysDemoView::Tick(DWORD dwTime)
{
	if (!g_Game.IsTerrainInit())
		return true;

	//	Restore mouse to original position
	if (m_bRightPressed)
	{
		::SetCursorPos(m_ptMouseLock.x, m_ptMouseLock.y);
		m_ptMouseLast = m_ptMouseLock;
	}

	//	Move camera
	MoveCamera(dwTime);

	//	Do logic tick here.
	g_Game.Tick(dwTime);

	return true;
}

//	Render scene
bool CPhysDemoView::Render()
{
	m_dwRenderStart = a_GetTime();

	if (!g_Render.BeginRender())
		return false;

	//	Clear viewport
	g_Render.ClearViewport();

	//	Do render
	g_Game.Render();

	//	Draw prompts
	DrawPrompts();

	// draw the coordinate direction...
	if(g_Game.GetPickedObject() || g_Game.GetHitObject())
	{
		m_CoordinateDirection.Draw();
	}

	g_Render.EndRender();

	m_dwLastRender = a_GetTime() - m_dwRenderStart;

	return true;
}

//	Move camera
void CPhysDemoView::MoveCamera(DWORD dwTime)
{
	A3DCamera* pA3DCamera = g_Render.GetCamera();

	// A3DVECTOR3 vDirH = pA3DCamera->GetDirH();
	// A3DVECTOR3 vRightH = pA3DCamera->GetRightH();
	
	A3DVECTOR3 vDirH = pA3DCamera->GetDir();
	A3DVECTOR3 vRightH = pA3DCamera->GetRight();
	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDirH;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDirH;
	
	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRightH;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRightH;
	
	if (GetAsyncKeyState('Q') & 0x8000)
		vDelta = vDelta + g_vAxisY;
	else if (GetAsyncKeyState('Z') & 0x8000)
		vDelta = vDelta - g_vAxisY;

	float fSpeed = 20.0f;
	if (GetAsyncKeyState(VK_SHIFT))
		fSpeed *= 2.5f;

	vDelta = a3d_Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);
	
	pA3DCamera->Move(vDelta);

	//	Adjust camera position so that it won't move into some wrong place
	A3DVECTOR3 vPos = pA3DCamera->GetPos();
	CPhysTerrain* pTerrain = g_Game.GetTerrain();
	if (pTerrain)
	{
		float fHeight = pTerrain->GetPosHeight(vPos);
		a_ClampFloor(vPos.y, fHeight + 1.0f);
	}

	pA3DCamera->SetPos(vPos);
}

//	Render prompts
void CPhysDemoView::DrawPrompts()
{
	HWND hWnd = g_Render.GetWndHandler();

	RECT rcClient;
	::GetClientRect(hWnd, &rcClient);

	int x = rcClient.left + 10;
	int y = rcClient.bottom - 24;
	DWORD dwColor = 0xff00ff00;
	AString str;

	//	Camera position
	A3DVECTOR3 vPos = g_Render.GetCamera()->GetPos();
	str.Format("Camera: %.3f, %.3f, %.3f", vPos.x, vPos.y, vPos.z);
	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;

	//	Physical state
	int iState = g_Game.GetPhysState();
	if (iState == CPhysObject::PHY_STATE_PURESIMULATE)
		str = "Pure simulation";
	else if (iState == CPhysObject::PHY_STATE_ANIMATION)
		str = "Pure animation";
	else if (iState = CPhysObject::PHY_STATE_PARTSIMULATE)
		str = "Partial simulation";

	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;

	//	Render time of last frame
	str.Format("Render time: %d", m_dwLastRender);
	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;
}

//	Get the terrain position pointed by cursor
bool CPhysDemoView::GetCurTerrainPos(int x, int y, A3DVECTOR3& vOutPos)
{
	A3DVECTOR3 vDest = A3DVECTOR3((float)x, (float)y, 1.0f);
	if (!g_Render.GetViewport()->InvTransform(vDest, vDest))
		return false;
	
	A3DVECTOR3 vStart = g_Render.GetCamera()->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;
	RAYTRACERT TraceRt;

	CPhysTerrain* pTerrain = g_Game.GetTerrain();
	if (!pTerrain)
		return false;

	RAYTRACERT rt;
	if (!pTerrain->RayTrace(vStart, vDelta, 1.0f, &rt))
		return false;

	vOutPos = rt.vHitPos;
	return true;
}

void CPhysDemoView::OnLButtonDown(UINT nFlags, CPoint point) 
{

	switch(g_Game.GetGameMode())
	{
	
	case CGame::GAME_MODE_EDIT:

		{
			// edit mode

			if(g_Game.GetPickedObject())
			{
				// if I have picked one phys object...
				// set up the picked object to the terrain hit pos...

				A3DVECTOR3 vHitPos;
				if (GetCurTerrainPos(point.x, point.y, vHitPos))
				{
					A3DVECTOR3 vDir, vUp;
					g_Game.GetPickedObject()->GetDirAndUp(vDir, vUp);
					g_Game.SetupPickedObject(vHitPos, vDir, vUp);
				}

			}
			else if(g_Game.GetHitObject())
			{
				// if I just hit the object...
				g_Game.PickHitObject();
				
				if(m_CoordinateDirection.RayTrace(point.x, point.y))
				{
					CPhysObject* pObject = g_Game.GetPickedObject();
					
					A3DVECTOR3 vDir, vUp;
					m_CoordinateDirection.SetPos(pObject->GetPos());
					if(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move)
					{
						m_CoordinateDirection.SetDirAndUp(g_vAxisZ, g_vAxisY);
					}
					else if (m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Rotate)
					{
						pObject->GetDirAndUp(vDir, vUp);
						m_CoordinateDirection.SetDirAndUp(vDir, vUp);
					}
					
					m_CoordinateDirection.Update();
					m_CoordinateDirection.OnLButtonDown(point.x, point.y, nFlags);

					m_bHitTerrainEditMode = false;

				}

				

			}

		}
		
		break;

	
	case CGame::GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case CGame::GAME_MODE_PLAY:

		{
			// physX picking ...

			g_Game.GetPhysXPicker()->OnMouseButtonDown(point.x, point.y);

		}

	    break;
	
	default:
	    break;
	}

	CView::OnLButtonDown(nFlags, point);
}

void CPhysDemoView::OnLButtonUp(UINT nFlags, CPoint point) 
{

	switch(g_Game.GetGameMode())
	{
	
	case CGame::GAME_MODE_EDIT:
		{

			// do nothing...

			m_CoordinateDirection.OnLButtonUp(point.x, point.y, nFlags);

			CPhysObject* pObject = g_Game.GetPickedObject();
			if (pObject && !m_bHitTerrainEditMode)
			{
				A3DVECTOR3 vDir, vUp;
				if(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move)
				{
					pObject->GetDirAndUp(vDir, vUp);
					g_Game.SetupPickedObject(m_CoordinateDirection.GetPos(), vDir, vUp);

				}
				else if (m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Rotate)
				{
					m_CoordinateDirection.GetDirAndUp(vDir, vUp);
					g_Game.SetupPickedObject(m_CoordinateDirection.GetPos(), vDir, vUp);
				}

			}

			m_bHitTerrainEditMode = true;
		}
		
		
		break;

	
	case CGame::GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case CGame::GAME_MODE_PLAY:

		{
			// physX picking ...

			g_Game.GetPhysXPicker()->OnMouseButtonUp(point.x, point.y);

		}

	    break;
	
	default:
	    break;
	}

	CView::OnLButtonUp(nFlags, point);
}

void CPhysDemoView::OnMouseMove(UINT nFlags, CPoint point) 
{

	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);

	switch(g_Game.GetGameMode())
	{
	
	case CGame::GAME_MODE_EDIT:
		{

			if (m_bRightPressed)
			{
				// right button pressed, we rotate the camera...

				int dx = pt.x - m_ptMouseLast.x;
				int dy = pt.y - m_ptMouseLast.y;

				//	Rotate camera
				float f1 = 0.03f;
				A3DCamera* pCamera = g_Render.GetCamera();

				if (dx)
				{
					float fYaw = dx * f1;
					pCamera->DegDelta(fYaw);
				}
				
				if (dy)
				{
					float fPitch = -dy * f1;
					pCamera->PitchDelta(fPitch);
				}

				m_ptMouseLast = pt;
			}
			else if(nFlags & MK_LBUTTON)
			{
				// left button pressed...
				CPhysObject* pObject = g_Game.GetPickedObject();
				if (pObject)
				{
					m_CoordinateDirection.OnMouseMove(point.x, point.y, nFlags);
					if(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move)
						pObject->SetPos(m_CoordinateDirection.GetPos());
					else if (m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Rotate)
					{
						A3DVECTOR3 vDir, vUp;
						m_CoordinateDirection.GetDirAndUp(vDir, vUp);
						pObject->SetDirAndUp(vDir, vUp);
					}
				}
				
			}
			else
			{

				CPhysObject* pObject = g_Game.GetPickedObject();
				if (pObject)
				{
					A3DVECTOR3 vHitPos;
					if (GetCurTerrainPos(point.x, point.y, vHitPos))
					{
						pObject->SetPos(vHitPos);
					}

					A3DVECTOR3 vDir, vUp;
					m_CoordinateDirection.SetPos(pObject->GetPos());
					
					if(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move)
					{
						m_CoordinateDirection.SetDirAndUp(g_vAxisZ, g_vAxisY);
					}
					else if (m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Rotate)
					{
						A3DVECTOR3 vDir, vUp;	
						pObject->GetDirAndUp(vDir, vUp);
						m_CoordinateDirection.SetDirAndUp(vDir, vUp);
					}
	
					m_CoordinateDirection.Update();

				}
				else
				{
					// try to ray trace the object...
					PhysRay ray;

					ScreenPosToRay(g_Render.GetViewport(), point.x, point.y, ray);
					
					g_Game.RayTracePhysObject(ray);

					if(g_Game.GetHitObject())
					{
						m_CoordinateDirection.SetPos(g_Game.GetHitObject()->GetPos());

						if(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move)
						{
							m_CoordinateDirection.SetDirAndUp(g_vAxisZ, g_vAxisY);
						}
						else if (m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Rotate)
						{
							A3DVECTOR3 vDir, vUp;	
							g_Game.GetHitObject()->GetDirAndUp(vDir, vUp);
							m_CoordinateDirection.SetDirAndUp(vDir, vUp);
						}
					

						m_CoordinateDirection.Update();
						m_CoordinateDirection.OnMouseMove(point.x, point.y, nFlags);


					}

				}


			}

		}
	
		break;

	
	case CGame::GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case CGame::GAME_MODE_PLAY:

		{
			if (m_bRightPressed)
			{
				// right button pressed, we rotate the camera...

				int dx = pt.x - m_ptMouseLast.x;
				int dy = pt.y - m_ptMouseLast.y;

				//	Rotate camera
				float f1 = 0.03f;
				A3DCamera* pCamera = g_Render.GetCamera();

				if (dx)
				{
					float fYaw = dx * f1;
					pCamera->DegDelta(fYaw);
				}
				
				if (dy)
				{
					float fPitch = -dy * f1;
					pCamera->PitchDelta(fPitch);
				}

				m_ptMouseLast = pt;
			}
			else
			{
				// physX picking
				g_Game.GetPhysXPicker()->OnMouseMove(point.x, point.y);
			}
		}

	    break;
	
	default:
	    break;
	}

	CView::OnMouseMove(nFlags, point);
}

void CPhysDemoView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bRightPressed)
		return;
	m_bRightPressed	= true;


	switch(g_Game.GetGameMode())
	{
	
	case CGame::GAME_MODE_EDIT:
		{

		}
		
		
		break;

	
	case CGame::GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case CGame::GAME_MODE_PLAY:

		{

		}

	    break;
	
	default:
	    break;
	}
	
	m_ptMouseLock.x	= point.x;
	m_ptMouseLock.y = point.y;
	ClientToScreen(&m_ptMouseLock);

	m_ptMouseLast = m_ptMouseLock;

	SetCapture();
	ShowCursor(false);
	
	CView::OnRButtonDown(nFlags, point);
}

void CPhysDemoView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bRightPressed)
		return;
	m_bRightPressed = false;

	switch(g_Game.GetGameMode())
	{
	
	case CGame::GAME_MODE_EDIT:
		{

		}
		
		
		break;

	
	case CGame::GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case CGame::GAME_MODE_PLAY:

		{

		}

	    break;
	
	default:
	    break;
	}

	ShowCursor(true);
	ReleaseCapture();
	
	CView::OnRButtonUp(nFlags, point);
}

BOOL CPhysDemoView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	if (pA3DDevice && pA3DDevice->GetShowCursor())
	{
		if (nHitTest != HTCLIENT)
			return FALSE;
		else
		{
			A3DCursor* pCursor = pA3DDevice->GetCursor();
			if (pCursor)
				pCursor->OnSysSetCursor();
		}
	}
	else
		SetCursor(NULL);

	return TRUE;
//	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CPhysDemoView::OnEditAddstaticobj() 
{
	//	Let user select a mox file
	char szFilter[] = "MOX 文件(*.mox)|*.mox||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.mox", NULL, dwFlags, szFilter, NULL);
	FileDia.m_ofn.lpstrInitialDir = g_szWorkDir;
	
	if (FileDia.DoModal() != IDOK)
		return;

	//	Create static object
	CPhysStaticObject* pObject = new CPhysStaticObject;

	if (!pObject->LoadModel(FileDia.GetPathName()))
	{
		MessageBox("加载数据失败!");

		delete pObject;

		return;
	}

	g_Game.SetPickedObject(pObject);

	m_LastAddedObjItem.mObjType = PHYS_OBJTYPE_STATIC;
	m_LastAddedObjItem.mFilePath = FileDia.GetPathName();
		
}

void CPhysDemoView::OnEditAddsmd() 
{
	// TODO: Add your command handler code here
	char szFilter[] = "SMD 文件(*.smd)|*.smd||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.smd", NULL, dwFlags, szFilter, NULL);
	FileDia.m_ofn.lpstrInitialDir = g_szWorkDir;
	
	if (FileDia.DoModal() != IDOK)
		return;

	//	Create static object
	CPhysModelObject* pObject = new CPhysModelObject;

	if (!pObject->LoadModel(FileDia.GetPathName()))
	{
		MessageBox("加载数据失败!");

		delete pObject;

		return;
	}

	g_Game.SetPickedObject(pObject);	

	m_LastAddedObjItem.mObjType = PHYS_OBJTYPE_SKELETON;
	m_LastAddedObjItem.mFilePath = FileDia.GetPathName();

}


void CPhysDemoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPhysDemoView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(g_Game.GetGameMode())
	{
	
	case CGame::GAME_MODE_EDIT:
		{

			
			switch (nChar)
			{

			/*				
			case '0':

				if(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move)
					m_CoordinateDirection.SetEditOperation(CCoordinateDirection::Edit_Operation_Rotate);
				else
					m_CoordinateDirection.SetEditOperation(CCoordinateDirection::Edit_Operation_Move);


				break;
			*/
				
			case VK_DELETE:

				{

					// remove the picked object...
					g_Game.RemovePickedObject();

				}

				break;
				
			default:
				break;
			};

			
		}
		
		
		break;

	
	case CGame::GAME_MODE_SIMULATE:
		
		// currently, we deal with GAME_MODE_SIMULATE case and GAME_MODE_PLAY case identically.
		// break;
	
	case CGame::GAME_MODE_PLAY:

		{
			switch (nChar)
			{
			case VK_SPACE:

				ShootDummySphereFromEye();
				break;

			case '1':
			
				g_Game.SetPhysState(CPhysObject::PHY_STATE_PURESIMULATE);
				break;
			
			case '2':
			
				g_Game.SetPhysState(CPhysObject::PHY_STATE_ANIMATION);
				break;

			case '3':
			
				g_Game.SetPhysState(CPhysObject::PHY_STATE_PARTSIMULATE);
				break;

			case '9':


				g_Game.TurnOnPhysX();

				break;

			case '0':
			
				g_Game.TurnOffPhysX();
				break;


			case 'F':

				g_Game.TestWindFF();
				break;

			case 'C':

				g_Game.TestCloth();
				break;

			case 'U':

				gPhysXEngine->CoreDumpPhysX("APhysXDemoCoreDump.xml", APhysXEngine::APX_COREDUMP_FT_XML);
				
				break;

/*
			// obsolete code...
			// test code...
			case '0':

				{
					//A3DVECTOR3 vPos(45,35,51);
					A3DVECTOR3 vPos(35.7f,34.6f,32.6f);
					g_Game.LoadGfxEx("SmokeEmitter.gfx", vPos);
				}

				break;

			case '9':

				{
					//A3DVECTOR3 vPos(45,35,51);
					A3DVECTOR3 vPos(35.7f,34.6f,32.6f);
					g_Game.LoadGfxEx("SmokeEmitter2.gfx", vPos);
				}

				break;

*/
			}


		}

	    break;
	
	default:
	    break;
	}


	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

//	Drop a dummy sphere with specified height
void CPhysDemoView::DropDummySphere(float fHeight)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	A3DVECTOR3 vHitPos;
	if (GetCurTerrainPos(pt.x, pt.y, vHitPos))
	{
		vHitPos.y += fHeight;
		APhysXScene* pPhysXScene = g_Game.GetPhysXScene();
		if(pPhysXScene)
		{
			pPhysXScene->CreateDummySphereActor(APhysXConverter::A2N_Vector3(vHitPos), 1.0f);
		}
	}
}

void CPhysDemoView::ShootDummySphereFromEye(float fVelocity)
{
	A3DCamera* pA3DCamera = g_Render.GetCamera();

	A3DVECTOR3 vEyeDir = pA3DCamera->GetDir();
	A3DVECTOR3 vEyePos = pA3DCamera->GetPos();

	APhysXScene* pPhysXScene = g_Game.GetPhysXScene();
	if(pPhysXScene)
	{
		pPhysXScene->CreateDummySphereActor(APhysXConverter::A2N_Vector3(vEyePos), 0.5f, fVelocity * APhysXConverter::A2N_Vector3(vEyeDir));
	}


}

void CPhysDemoView::OnEditPhysxtrngen() 
{
	CDlgPhysXTrnGen dlg;
	dlg.DoModal();
}

void CPhysDemoView::OnEditAddterrain() 
{
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("trn2");
	if (FileDlg.DoModal() != IDOK)
		return;

	AString strFile = FileDlg.GetRelativeFileName();

#if 0

	if (!g_Game.LoadTerrain(strFile))
	{
		MessageBox("加载地形数据失败！");
		return;
	}



	//	Load PhysX terrain
	af_ChangeFileExt(strFile, "_ptb.cfg");
	if (af_IsFileExist(strFile))
	{
		// do not load terrain directly...
		if(!g_Game.LoadPhysXTerrain(strFile))
			MessageBox("加载PhysX地形数据失败！");

		return;
	}

#else
	
	g_Game.LoadTerrainAndPhysXTerrain(strFile);


#endif

}




void CPhysDemoView::OnModeEdit() 
{
	// TODO: Add your command handler code here

	g_Game.SetGameMode(CGame::GAME_MODE_EDIT);

	
}

void CPhysDemoView::OnModeSimulate() 
{
	// TODO: Add your command handler code here
	g_Game.SetGameMode(CGame::GAME_MODE_SIMULATE);
}

void CPhysDemoView::OnModePlay() 
{
	// TODO: Add your command handler code here
	g_Game.SetGameMode(CGame::GAME_MODE_PLAY);

}

void CPhysDemoView::OnUpdateModeEdit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);

}

void CPhysDemoView::OnUpdateModeSimulate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Game.GetGameMode() == CGame::GAME_MODE_SIMULATE);
}

void CPhysDemoView::OnUpdateModePlay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Game.GetGameMode() == CGame::GAME_MODE_PLAY);
}

void CPhysDemoView::OnModePhysxprofile() 
{
	// TODO: Add your command handler code here
	g_Game.EnablePhysXProfile(!g_Game.IsPhysXProfileEnabled());
	
}

void CPhysDemoView::OnUpdateModePhysxprofile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Game.IsPhysXProfileEnabled());
}

void CPhysDemoView::OnModePhysxdebugrender() 
{
	// TODO: Add your command handler code here
	g_Game.EnablePhysXDebugRender(!g_Game.IsPhysXDebugRenderEnabled());
	
}

void CPhysDemoView::OnUpdateModePhysxdebugrender(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_Game.IsPhysXDebugRenderEnabled());
}

void CPhysDemoView::OnUpdateEditAddterrain(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);
}

void CPhysDemoView::OnUpdateEditAddstaticobj(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);
}

void CPhysDemoView::OnUpdateEditPhysxtrngen(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);	
}


void CPhysDemoView::OnEditAddlastobj() 
{
	// TODO: Add your command handler code here
	CPhysObject* pPhysObject = CPhysObjectMan::CreatePhysObject(m_LastAddedObjItem.mObjType);
	if(pPhysObject && pPhysObject->LoadModel(m_LastAddedObjItem.mFilePath))
	{
		g_Game.SetPickedObject(pPhysObject);
	}

}

void CPhysDemoView::OnUpdateEditAddsmd(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);	
}

void CPhysDemoView::OnUpdateEditAddlastobj(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT && m_LastAddedObjItem.mObjType != -1);	
}

void CPhysDemoView::OnFileSave() 
{
	// TODO: Add your command handler code here

	char szFilter[] = "PDS 文件(*.pds)|*.pds||";
	DWORD dwFlags =  OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.pds", NULL, dwFlags, szFilter, NULL);

	AString strGfxPath = g_szWorkDir;
	strGfxPath += "\\Scenes\\";

	FileDia.m_ofn.lpstrInitialDir = strGfxPath;
	
	if (FileDia.DoModal() != IDOK)
		return;

	if(g_Game.SavePhysXDemoScene(FileDia.GetPathName()))
	{
		AUX_GetMainFrame()->SetWindowText(FileDia.GetFileTitle());
	}

	
}

void CPhysDemoView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	char szFilter[] = "PDS 文件(*.pds)|*.pds||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.pds", NULL, dwFlags, szFilter, NULL);
	AString strPath = g_szWorkDir;
	strPath += "\\Scenes\\";

	FileDia.m_ofn.lpstrInitialDir = strPath;
	
	if (FileDia.DoModal() != IDOK)
		return;

	if(g_Game.LoadPhysXDemoScene(FileDia.GetPathName()))
	{
		AUX_GetMainFrame()->SetWindowText(FileDia.GetFileTitle());
	}

	
}

void CPhysDemoView::OnEditAddphysxgfx() 
{
	// TODO: Add your command handler code here

	//	Let user select a mox file
	char szFilter[] = "GFX 文件(*.gfx)|*.gfx||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.gfx", NULL, dwFlags, szFilter, NULL);
	
	AString strGfxPath = g_szWorkDir;
	strGfxPath += "\\GFX\\";

	FileDia.m_ofn.lpstrInitialDir = strGfxPath;
	
	if (FileDia.DoModal() != IDOK)
		return;

	//	Create static object
	CPhysParticleObject* pObject = new CPhysParticleObject;

	if (!pObject->LoadModel(FileDia.GetPathName()))
	{
		MessageBox("加载数据失败!");

		delete pObject;

		return;
	}

	g_Game.SetPickedObject(pObject);

	m_LastAddedObjItem.mObjType = PHYS_OBJTYPE_PARTICLE;
	m_LastAddedObjItem.mFilePath = FileDia.GetPathName();
	
}

void CPhysDemoView::OnUpdateEditAddphysxgfx(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);	
	
}

void CPhysDemoView::OnEditPickandrotate() 
{
	// TODO: Add your command handler code here
	m_CoordinateDirection.SetEditOperation(CCoordinateDirection::Edit_Operation_Rotate);
}

void CPhysDemoView::OnUpdateEditPickandrotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);
	
	pCmdUI->SetCheck(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Rotate);	
}

void CPhysDemoView::OnEditPickandmove() 
{
	// TODO: Add your command handler code here
	m_CoordinateDirection.SetEditOperation(CCoordinateDirection::Edit_Operation_Move);
}

void CPhysDemoView::OnUpdateEditPickandmove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);	
	pCmdUI->SetCheck(m_CoordinateDirection.GetEditOperation() == CCoordinateDirection::Edit_Operation_Move);
}

void CPhysDemoView::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);

}

void CPhysDemoView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(g_Game.GetGameMode() == CGame::GAME_MODE_EDIT);

}

