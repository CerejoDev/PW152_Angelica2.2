//----------------------------------------------------------
// Filename	: CoordinateDirection.cpp
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 坐标指示类。该类显示物体坐标信息，同时返回选中的
//			  坐标轴或是坐标平面
//-----------------------------------------------------------
#include "Global.h"
#include "CoordinateDirection.h"
#include "A3D.h"
#include "Render.h"

//---------------------------------------------------------------------------------------------
// global functions and variables...

const float c_fRotationScale = 0.01f;

static void ScreenTraceAxis(const A3DVECTOR3 &pos, const A3DMATRIX4& matRotation, CPoint pt, A3DVECTOR3 *pXY, A3DVECTOR3 *pXZ, A3DVECTOR3 *pYZ, float dot[3])
{
	A3DVECTOR3 s_start,s_end,w_start,w_end;
	s_start.x = (float)pt.x;
	s_start.y = (float)pt.y;
	s_start.z = 0.0f;

	s_end.x = (float)pt.x;
	s_end.y = (float)pt.y;
	s_end.z = 1.0f;

	g_Render.GetA3DEngine()->GetActiveViewport()->InvTransform(s_start,w_start);
	g_Render.GetA3DEngine()->GetActiveViewport()->InvTransform(s_end,w_end);
	
	D3DXPLANE xy,xz,yz;
	D3DXVECTOR3 normal[3];
	D3DXVECTOR3 vDelta1,vDelta2;

#define USE_OLD_COMPUTATION 	0

	{ 
		// 计算XY平面

#if USE_OLD_COMPUTATION 
		
		
		D3DXVECTOR3 pt1(pos.x - 10.0f, pos.y - 10.0f, pos.z),
					pt2(pos.x - 10.0f, pos.y, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&xy, &pt1, &pt2, &pt3);
		
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[0],&vDelta1, &vDelta2);

#else


		D3DXVECTOR3 dPos(pos.x, pos.y, pos.z);
		A3DVECTOR3 vAxis = matRotation.GetRow(2);
		D3DXVECTOR3 dNormal(vAxis.x, vAxis.y, vAxis.z);

		D3DXPlaneFromPointNormal(&xy, &dPos, &dNormal);

		normal[0] = dNormal;

#endif

	}

	{ 
		// 计算XZ平面

#if USE_OLD_COMPUTATION 
		
		D3DXVECTOR3 pt1(pos.x - 10.0f, pos.y, pos.z - 10.0f),
					pt2(pos.x - 10.0f, pos.y, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&xz, &pt1, &pt2, &pt3);
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[1],&vDelta1, &vDelta2);
#else

		D3DXVECTOR3 dPos(pos.x, pos.y, pos.z);
		A3DVECTOR3 vAxis = matRotation.GetRow(1);
		D3DXVECTOR3 dNormal(vAxis.x, vAxis.y, vAxis.z);

		D3DXPlaneFromPointNormal(&xz, &dPos, &dNormal);

		normal[1] = dNormal;

#endif

	}

	{ 
		// 计算YZ平面

#if USE_OLD_COMPUTATION 

		D3DXVECTOR3 pt1(pos.x, pos.y - 10.0f, pos.z - 10.0f),
					pt2(pos.x, pos.y - 10.0f, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&yz, &pt1, &pt2, &pt3);
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[2],&vDelta1, &vDelta2);

#else

		D3DXVECTOR3 dPos(pos.x, pos.y, pos.z);
		A3DVECTOR3 vAxis = matRotation.GetRow(0);
		D3DXVECTOR3 dNormal(vAxis.x, vAxis.y, vAxis.z);

		D3DXPlaneFromPointNormal(&yz, &dPos, &dNormal);

		normal[2] = dNormal;

#endif

	}

	D3DXVECTOR3 vecLine = D3DXVECTOR3(w_end.x,w_end.y,w_end.z) -D3DXVECTOR3(w_start.x,w_start.y,w_start.z);
	for( int i =0; i<3; i++)
		dot[i] = (float)abs(D3DXVec3Dot(&normal[i],&vecLine));

	D3DXVECTOR3 start(w_start.x, w_start.y, w_start.z), end(w_end.x, w_end.y, w_end.z);
	D3DXVECTOR3 iXY(pos.x,pos.y,pos.z),iXZ(pos.x,pos.y,pos.z),iYZ(pos.x,pos.y,pos.z);
	
	if(NULL==D3DXPlaneIntersectLine(&iXY, &xy, &start, &end))
		iXY = D3DXVECTOR3(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iXZ, &xz, &start, &end))
		iXZ = D3DXVECTOR3(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iYZ, &yz, &start, &end))
		iYZ = D3DXVECTOR3(pos.x,pos.y,pos.z);
		
	if(pXY) *pXY = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pXZ) *pXZ = A3DVECTOR3(iXZ.x, iXZ.y, iXZ.z);
	if(pYZ) *pYZ = A3DVECTOR3(iYZ.x, iYZ.y, iYZ.z);

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinateDirection::CCoordinateDirection()
{
	m_nSelectedAxis = 0;
	m_nSelectedTemp = 0;
	m_bLMouseDown = false;
	m_matRotate.Identity();
	m_matTranslate.Identity();;
	m_matABS.Identity();
	m_fDelta = 1.0f;

	m_nOldX = 0;
	m_nOldY = 0;

	m_iEditOperation = Edit_Operation_Rotate;
}

CCoordinateDirection::~CCoordinateDirection()
{

}

void CCoordinateDirection::SetPos(const A3DVECTOR3& vPos)
{
	m_matTranslate.Translate(vPos.x,vPos.y,vPos.z);
	m_vPos = vPos;

	Update();
}

void CCoordinateDirection::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	A3DVECTOR3 vRight = CrossProduct(vUp, vDir);

	m_matRotate.SetRow(0, vRight);
	m_matRotate.SetRow(1, vUp);
	m_matRotate.SetRow(2, vDir);

	Update();
}

void CCoordinateDirection::GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp)
{
	vDir = m_matRotate.GetRow(2);
	vUp = m_matRotate.GetRow(1);
}

A3DVECTOR3 CCoordinateDirection::GetAxisVec(int iAxis)
{

	switch(iAxis)
	{
	case AXIS_X:

		// return m_matRotate.GetRow(0);
		return A3DVECTOR3(1.0f, 0, 0);

		break;

	case AXIS_Y:
		
		// return m_matRotate.GetRow(1);
		return A3DVECTOR3(0, 1.0f, 0);

		break;

	case AXIS_Z:

		// return m_matRotate.GetRow(2);
		return A3DVECTOR3(0, 0, 1.0f);

	    break;

	default:

		return A3DVECTOR3(0);

	    break;
	}
}

void CCoordinateDirection::Update()
{
	A3DVECTOR3 camPos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
	A3DVECTOR3 mAxisGizmoCenter = m_vPos;
	// assumes a 90deg FOV
	A3DVECTOR3 dir = mAxisGizmoCenter - camPos;
	A3DVIEWPORTPARAM* param = g_Render.GetA3DEngine()->GetActiveViewport()->GetParam();
	
	A3DMATRIX4 matScale;
	float mAxisGizmoProjLen = (60.0f/(float)param->Height) * dir.Magnitude();
	matScale.Identity();
	matScale.Scale(mAxisGizmoProjLen,mAxisGizmoProjLen,mAxisGizmoProjLen);
	m_matABS = matScale*m_matRotate*m_matTranslate;
	
}

void CCoordinateDirection::Draw()
{

	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWireCollector->Flush();
	//临时关闭Z_buffer
	g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(false);
	DrawArrow();
	DrawAxis();
	DrawPlane();
	
	pWireCollector->Flush();
	g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(true);
}

void CCoordinateDirection::DrawArrow()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.05f;
	float fArrow = 0.1f;
	float fPlane = 0.25f;
	A3DVECTOR3 Vertices[6];
	WORD indices[10];

	indices[0] = 0; indices[1] = 1;
	indices[2] = 1; indices[3] = 2;
	indices[4] = 1; indices[5] = 3;
	indices[6] = 1; indices[7] = 4;
	indices[8] = 1; indices[9] = 5;

	//x 轴
	Vertices[0] =  A3DVECTOR3(m_fDelta*0.9f,0,0);
	Vertices[1] =  A3DVECTOR3(vExt.x,0,0);
	Vertices[2] =  A3DVECTOR3(vExt.x - fArrow,0 - fTemp,0 - fTemp);
	Vertices[3] =  A3DVECTOR3(vExt.x - fArrow,0 - fTemp,fTemp);
	Vertices[4] =  A3DVECTOR3(vExt.x - fArrow,fTemp,fTemp);
	Vertices[5] =  A3DVECTOR3(vExt.x - fArrow,fTemp,0 - fTemp);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	Vertices[2] = m_matABS*Vertices[2];
	Vertices[3] = m_matABS*Vertices[3];
	Vertices[4] = m_matABS*Vertices[4];
	Vertices[5] = m_matABS*Vertices[5];
	
	pWireCollector->AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(255,0,0));
	
	
	//y 轴
	Vertices[0] =  A3DVECTOR3(0,m_fDelta*0.9f,0);
	Vertices[1] =  A3DVECTOR3(0,vExt.y,0);
	Vertices[2] =  A3DVECTOR3(0 - fTemp,vExt.y - fArrow,0 - fTemp);
	Vertices[3] =  A3DVECTOR3(0 - fTemp,vExt.y - fArrow,fTemp);
	Vertices[4] =  A3DVECTOR3(fTemp,vExt.y - fArrow,fTemp);
	Vertices[5] =  A3DVECTOR3(fTemp,vExt.y - fArrow,0 - fTemp);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	Vertices[2] = m_matABS*Vertices[2];
	Vertices[3] = m_matABS*Vertices[3];
	Vertices[4] = m_matABS*Vertices[4];
	Vertices[5] = m_matABS*Vertices[5];
	pWireCollector->AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(0,255,0));

	//z 轴
	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*0.9f);
	Vertices[1] =  A3DVECTOR3(0,0,vExt.z);
	Vertices[2] =  A3DVECTOR3(0 - fTemp,0 - fTemp,vExt.z - fArrow);
	Vertices[3] =  A3DVECTOR3(0 - fTemp,fTemp,vExt.z - fArrow);
	Vertices[4] =  A3DVECTOR3(fTemp,fTemp,vExt.z - fArrow);
	Vertices[5] =  A3DVECTOR3(fTemp,0 - fTemp,vExt.z - fArrow);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	Vertices[2] = m_matABS*Vertices[2];
	Vertices[3] = m_matABS*Vertices[3];
	Vertices[4] = m_matABS*Vertices[4];
	Vertices[5] = m_matABS*Vertices[5];
	pWireCollector->AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(0,0,255));
	
}

void CCoordinateDirection::DrawPlane()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	float m_fDelta = 1.0f;
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.1f;
	float fArrow = 0.25f;
	float fPlane = 0.25f;
	A3DVECTOR3 Vertices[2];
	WORD indices[2];

	indices[0] = 0; indices[1] = 1;

	//xy plane
	Vertices[0] =  A3DVECTOR3(m_fDelta*fPlane,0,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,m_fDelta*fPlane,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XY)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));

	Vertices[0] =  A3DVECTOR3(0,m_fDelta*fPlane,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,m_fDelta*fPlane,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==PLANE_XY)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));
	
	//xz plane
	Vertices[0] =  A3DVECTOR3(m_fDelta*fPlane,0,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,0,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));
	
	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*fPlane);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,0,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
	
	//yz plane
	Vertices[0] =  A3DVECTOR3(0,m_fDelta*fPlane,0);;
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*fPlane,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_YZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));

	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*fPlane);;
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*fPlane,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==PLANE_YZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
}

void CCoordinateDirection::DrawAxis()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.1f;
	float fArrow = 0.25f;
	float fPlane = 0.25f;
	A3DVECTOR3 Vertices[2];
	A3DVECTOR3 vScreenPos;
	WORD indices[2];

	indices[0] = 0; indices[1] = 1;

	//x 轴
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(m_fDelta*0.9f,0,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==AXIS_X || m_nSelectedAxis==PLANE_XY || m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
		g_Render.TextOut(vScreenPos.x,vScreenPos.y,"X",A3DCOLORRGB(255,255,255));
	
	//y 轴
	Vertices[0] = A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*0.9f,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==AXIS_Y || m_nSelectedAxis==PLANE_XY || m_nSelectedAxis==PLANE_YZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
	g_Render.TextOut(vScreenPos.x,vScreenPos.y,"Y",A3DCOLORRGB(255,255,255));
	//z 轴
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(0,0,m_fDelta*0.9f);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==AXIS_Z || m_nSelectedAxis==PLANE_YZ || m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
	g_Render.TextOut(vScreenPos.x,vScreenPos.y,"Z",A3DCOLORRGB(255,255,255));
}

int CCoordinateDirection::RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd)
{
	
	float fDis,ftemp;
	fDis = 999999.0f;
	m_nSelectedAxis = AXIS_PLANE_NO;

	if(TraceAxis(A3DVECTOR3(m_fDelta*0.9f,0,0),vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = AXIS_X;
		}
	}
	
	if(TraceAxis(A3DVECTOR3(0,m_fDelta*0.9f,0),vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = AXIS_Y;
		}
	}
	
	if(TraceAxis(A3DVECTOR3(0,0,m_fDelta*0.9f),vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = AXIS_Z;
		}
	}

	A3DVECTOR3 Vertices[4];
	Vertices[0] = m_matABS*A3DVECTOR3(0,0,0);
	Vertices[1] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,0,0);
	Vertices[2] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,m_fDelta*0.25f,0);
	Vertices[3] = m_matABS*A3DVECTOR3(0,m_fDelta*0.25f,0);
	
	if(TracePlane(Vertices,vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = PLANE_XY;
		}
	}
	
	
	Vertices[0] = m_matABS*A3DVECTOR3(0,0,0);
	Vertices[1] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,0,0);
	Vertices[2] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,0,m_fDelta*0.25f);
	Vertices[3] = m_matABS*A3DVECTOR3(0,0,m_fDelta*0.25f);
	
	if(TracePlane(Vertices,vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = PLANE_XZ;
		}
	}
	
	Vertices[0] = m_matABS*A3DVECTOR3(0,0,0);
	Vertices[1] = m_matABS*A3DVECTOR3(0,m_fDelta*0.25f,0);
	Vertices[2] = m_matABS*A3DVECTOR3(0,m_fDelta*0.25f,m_fDelta*0.25f);
	Vertices[3] = m_matABS*A3DVECTOR3(0,0,m_fDelta*0.25f);
	
	if(TracePlane(Vertices,vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = PLANE_YZ;
		}
	}
	
	return m_nSelectedAxis;
}

bool CCoordinateDirection::TraceAxis(A3DVECTOR3 vAxis,A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis)
{
	A3DVECTOR3 Vertices[2];
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  vAxis;
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	A3DVECTOR3 vPlaneDir = Vertices[1] - Vertices[0];
	A3DVECTOR3 vPlaneUp  = CrossProduct(vPlaneDir,g_Render.GetA3DEngine()->GetActiveCamera()->GetDir());
	A3DVECTOR3 vPlaneNormal = CrossProduct(vPlaneDir,vPlaneUp);
	vPlaneNormal.Normalize();

	D3DXPLANE plane;
	D3DXVECTOR3 vOut,vV,vN;
	vV = D3DXVECTOR3(Vertices[0].x, Vertices[0].y, Vertices[0].z);
	vN = D3DXVECTOR3(vPlaneNormal.x,vPlaneNormal.y,vPlaneNormal.z);
	D3DXPlaneFromPointNormal(&plane,&vV,&vN);
	
	vV = D3DXVECTOR3(vStart.x, vStart.y, vStart.z);
	vN = D3DXVECTOR3(vEnd.x, vEnd.y, vEnd.z);
	if(NULL!=D3DXPlaneIntersectLine(&vOut,&plane,&vV,&vN))
	{
		A3DVECTOR3 vPlane[4], vIntersection;
		vIntersection = A3DVECTOR3(vOut.x, vOut.y, vOut.z);
		vPlane[0] = Vertices[0] + vPlaneUp*m_fDelta*0.1f;
		vPlane[1] = Vertices[0] - vPlaneUp*m_fDelta*0.1f;
		vPlane[2] = Vertices[1] - vPlaneUp*m_fDelta*0.1f;
		vPlane[3] = Vertices[1] + vPlaneUp*m_fDelta*0.1f;
		
		bool inside = true;
		for(int j = 0; inside && (j < 4); j++)
		{
            int k = (j+1) % 4;
            A3DVECTOR3 vec1 = vPlane[k] - vPlane[j];
            A3DVECTOR3 vec2 = vIntersection - vPlane[k];
			
            if(DotProduct(vec1, vec2) > 0.f)
				inside = false;   
		}
		
		if(inside)
		{
			vIntersection = vIntersection - vStart;
			dis = vIntersection.Magnitude();
			return true;
		}
	}
	return false;
}

bool CCoordinateDirection::TracePlane(A3DVECTOR3* pVertices, A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis)
{
	A3DVECTOR3 vPlaneNormal = CrossProduct(pVertices[1] - pVertices[0],pVertices[3] - pVertices[0]);
	vPlaneNormal.Normalize();

	D3DXPLANE plane;
	D3DXVECTOR3 vOut,vV,vN;
	vV = D3DXVECTOR3(pVertices[0].x, pVertices[0].y, pVertices[0].z);
	vN = D3DXVECTOR3(vPlaneNormal.x,vPlaneNormal.y,vPlaneNormal.z);
	D3DXPlaneFromPointNormal(&plane,&vV,&vN);
	
	vV = D3DXVECTOR3(vStart.x, vStart.y, vStart.z);
	vN = D3DXVECTOR3(vEnd.x, vEnd.y, vEnd.z);
	if(NULL!=D3DXPlaneIntersectLine(&vOut,&plane,&vV,&vN))
	{
		A3DVECTOR3 vIntersection;
		vIntersection = A3DVECTOR3(vOut.x, vOut.y, vOut.z);
		
		bool inside = true;
		for(int j = 0; inside && (j < 4); j++)
		{
            int k = (j+1) % 4;
            A3DVECTOR3 vec1 = pVertices[k] - pVertices[j];
            A3DVECTOR3 vec2 = vIntersection - pVertices[k];
			
            if(DotProduct(vec1, vec2) > 0.f)
				inside = false;   
		}
		
		if(inside)
		{
			vIntersection = vIntersection - vStart;
			dis = vIntersection.Magnitude();
			return true;
		}
	}
	return false;
}



void CCoordinateDirection::ReEdge(A3DVECTOR3 &vMaxEdge,A3DVECTOR3 &vMinEdge)
{
	float temp;
	if(vMaxEdge.x<vMinEdge.x) 
	{
		temp = vMaxEdge.x;
		vMaxEdge.x = vMinEdge.x;
		vMinEdge.x = temp;
	}
	if(vMaxEdge.y<vMinEdge.y) 
	{
		temp = vMaxEdge.y;
		vMaxEdge.y = vMinEdge.y;
		vMinEdge.y = temp;
	}
	if(vMaxEdge.z<vMinEdge.z) 
	{
		temp = vMaxEdge.z;
		vMaxEdge.z = vMinEdge.z;
		vMinEdge.z = temp;
	}
}

bool CCoordinateDirection::RayTrace(int x, int y)
{
	A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
	A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
	A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
	A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
	pViewport->InvTransform(vStart,vStart);
	pViewport->InvTransform(vPos,vPos);
	
	return (RayTrace(vStart,vPos)!=AXIS_PLANE_NO);

}

bool CCoordinateDirection::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if(m_bLMouseDown)
	{
		 m_nSelectedAxis = m_nSelectedTemp;
	}else
	{
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		if(RayTrace(vStart,vPos)!=AXIS_PLANE_NO)
		{
			SetCursor(m_hCursor);
		}
		//else 
		//	return false;
	}

	if(dwFlags & MK_LBUTTON)
	{
		// left button pressed, we will 

		switch(m_iEditOperation)
		{
		case Edit_Operation_None:
			break;

		case Edit_Operation_Move:
			
			{
				A3DVECTOR3 vDelta =  GetDeltaVector(m_vPos, x, y);
				SetPos(m_vPos + vDelta);
			}

			break;

		case Edit_Operation_Rotate:
			
			{
				float deltaX = (x - m_nOldX) * c_fRotationScale;
				float deltaY = (m_nOldY - y) * c_fRotationScale;

				A3DVECTOR3 vAxis = GetAxisVec(m_nSelectedAxis);
				A3DMATRIX4 mtTmp;
				mtTmp.Identity();
					
				if(!vAxis.IsZero())
				{

					mtTmp.RotateAxis(vAxis, deltaY);
					m_matRotate = mtTmp * m_matRotate;

					Update();
				}

			}

			break;

		default:
			break;

		};


	}

	m_nOldX = x;
	m_nOldY = y;

	return true;
}

bool CCoordinateDirection::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
	A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
	A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
	A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
	pViewport->InvTransform(vStart,vStart);
	pViewport->InvTransform(vPos,vPos);
	m_nSelectedTemp = RayTrace(vStart,vPos);

	if(m_nSelectedTemp!=AXIS_PLANE_NO)
	{
		m_bLMouseDown = true;

		CPoint pt(x,y);
		float dot[3];
		ScreenTraceAxis(m_vPos, m_matRotate, pt,&m_vXY,&m_vXZ,&m_vYZ,dot);
		m_vXY = m_vXY - m_vPos;
		m_vXZ = m_vXZ - m_vPos;
		m_vYZ = m_vYZ - m_vPos;
	}
	else 
	{
		m_vYZ = m_vXZ = m_vXY = A3DVECTOR3(0,0,0);

		//return false;
	}

	m_nOldX = x;
	m_nOldX = y;

	return true;
}

bool CCoordinateDirection::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	m_bLMouseDown = false;
	return true;
}

A3DVECTOR3 CCoordinateDirection::GetDeltaVector(const A3DVECTOR3& pos,int x,int y)
{
	A3DVECTOR3 vDelta = A3DVECTOR3(0.0f);
	CPoint pt(x,y);
	
	float dot[3];
	A3DVECTOR3 xy(pos),xz(pos),yz(pos);
	ScreenTraceAxis(pos, m_matRotate, pt,&xy,&xz,&yz,dot);

#if USE_OLD_COMPUTATION

	switch(m_nSelectedAxis)
	{
	case  CCoordinateDirection::AXIS_X: 
		if(dot[0] < dot[1])
		{
			vDelta.x = xz.x - pos.x - m_vXZ.x;
		}
		else 
		{
			vDelta.x = xy.x - pos.x - m_vXY.x;
		}
		break;
		
	case  CCoordinateDirection::AXIS_Y: 
		if(dot[0] < dot[2])
		{
			vDelta.y = yz.y - pos.y - m_vYZ.y;
		}
		else 
		{
			vDelta.y = xy.y - pos.y - m_vXY.y;
		}
		break;
		
	case  CCoordinateDirection::AXIS_Z: 
		if(dot[1] < dot[2])
		{
			vDelta.z = yz.z - pos.z - m_vYZ.z;
		}
		else 
		{
			vDelta.z = xz.z - pos.z - m_vXZ.z;
		}
		break;
		
	case  CCoordinateDirection::PLANE_XY: 
		vDelta.x = xy.x - pos.x - m_vXY.x;
		vDelta.y = xy.y - pos.y - m_vXY.y;
		break;
		
	case  CCoordinateDirection::PLANE_XZ: 
		vDelta.x = xz.x - pos.x - m_vXZ.x;
		vDelta.z = xz.z - pos.z - m_vXZ.z;
		break;
		
	case  CCoordinateDirection::PLANE_YZ: 
		vDelta.y = yz.y - pos.y - m_vYZ.y;
		vDelta.z = yz.z - pos.z - m_vYZ.z;
		break; 

	default:
		break;
	}

#else

	switch(m_nSelectedAxis)
	{
	case  CCoordinateDirection::AXIS_X: 
		if(dot[0] < dot[1])
		{
			// vDelta.x = xz.x - pos.x - m_vXZ.x;

			vDelta = DotProduct(m_matRotate.GetRow(0), xz - pos - m_vXZ) * m_matRotate.GetRow(0);
		}
		else 
		{
			// vDelta.x = xy.x - pos.x - m_vXY.x;

			vDelta = DotProduct(m_matRotate.GetRow(0), xy - pos - m_vXY) * m_matRotate.GetRow(0);
		}
		break;
		
	case  CCoordinateDirection::AXIS_Y: 
		if(dot[0] < dot[2])
		{
			// vDelta.y = yz.y - pos.y - m_vYZ.y;
			vDelta = DotProduct(m_matRotate.GetRow(1), yz - pos - m_vYZ) * m_matRotate.GetRow(1);

		}
		else 
		{
			// vDelta.y = xy.y - pos.y - m_vXY.y;
			vDelta = DotProduct(m_matRotate.GetRow(1), xy - pos - m_vXY) * m_matRotate.GetRow(1);
		}
		break;
		
	case  CCoordinateDirection::AXIS_Z: 
		if(dot[1] < dot[2])
		{
			// vDelta.z = yz.z - pos.z - m_vYZ.z;
			vDelta = DotProduct(m_matRotate.GetRow(2), yz - pos - m_vYZ) * m_matRotate.GetRow(2);
		}
		else 
		{
			//vDelta.z = xz.z - pos.z - m_vXZ.z;
			vDelta = DotProduct(m_matRotate.GetRow(2), xz - pos - m_vXZ) * m_matRotate.GetRow(2);
		}
		break;
		
	case  CCoordinateDirection::PLANE_XY: 
		//vDelta.x = xy.x - pos.x - m_vXY.x;
		//vDelta.y = xy.y - pos.y - m_vXY.y;

		vDelta = xy - pos - m_vXY;

		break;
		
	case  CCoordinateDirection::PLANE_XZ: 
		//vDelta.x = xz.x - pos.x - m_vXZ.x;
		//vDelta.z = xz.z - pos.z - m_vXZ.z;
		
		vDelta = xz - pos - m_vXZ;

		break;
		
	case  CCoordinateDirection::PLANE_YZ: 

		//vDelta.y = yz.y - pos.y - m_vYZ.y;
		//vDelta.z = yz.z - pos.z - m_vYZ.z;

		vDelta = yz - pos - m_vYZ;

		break; 

	default:
		break;
	}


#endif

	
	return vDelta;
	

}