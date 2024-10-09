//----------------------------------------------------------
// Filename	: CoordinateDirection.cpp
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 坐标指示类。该类显示物体坐标信息，同时返回选中的
//			  坐标轴或是坐标平面
//-----------------------------------------------------------

#include "CoordinateDirection.h"
#include "A3D.h"
#include "RenderInterface.h"

const float c_fRotationScale = 0.01f;

IRenderUtility* CCoordinateDirection::g_Render = 0;

static void ScreenTraceAxis(A3DViewport& viewPort, const POINT& pt, const A3DVECTOR3& pos,
							const A3DMATRIX4& matRotation, A3DVECTOR3* pXY, A3DVECTOR3* pXZ,
							A3DVECTOR3*pYZ, float dot[3])
{
	D3DXPLANE xy, xz, yz;
	D3DXVECTOR3 normal[3];
	const D3DXVECTOR3 dPos(pos.x, pos.y, pos.z);

	// 计算XY平面
	A3DVECTOR3 vAxisZ = matRotation.GetRow(2);
	normal[0].x = vAxisZ.x;
	normal[0].y = vAxisZ.y;
	normal[0].z = vAxisZ.z;
	D3DXPlaneFromPointNormal(&xy, &dPos, &normal[0]);

	// 计算XZ平面
	A3DVECTOR3 vAxisY = matRotation.GetRow(1);
	normal[1].x = vAxisY.x;
	normal[1].y = vAxisY.y;
	normal[1].z = vAxisY.z;
	D3DXPlaneFromPointNormal(&xz, &dPos, &normal[1]);

	// 计算YZ平面
	A3DVECTOR3 vAxisX = matRotation.GetRow(0);
	normal[2].x = vAxisX.x;
	normal[2].y = vAxisX.y;
	normal[2].z = vAxisX.z;
	D3DXPlaneFromPointNormal(&yz, &dPos, &normal[2]);

	A3DVECTOR3 start(float(pt.x), float(pt.y), 0);
	A3DVECTOR3 end(float(pt.x), float(pt.y), 1);
	viewPort.InvTransform(start, start);
	viewPort.InvTransform(end, end);

	D3DXVECTOR3 d3dStart(start.x, start.y, start.z), d3dEnd(end.x, end.y, end.z);
	D3DXVECTOR3 vecLine = d3dEnd - d3dStart;
	for(int i = 0; i < 3; ++i)
		dot[i] = (float)abs(D3DXVec3Dot(&normal[i], &vecLine));

	D3DXVECTOR3 iXY(dPos), iXZ(dPos), iYZ(dPos);
	
	if(NULL==D3DXPlaneIntersectLine(&iXY, &xy, &d3dStart, &d3dEnd)) iXY = dPos;
	if(NULL==D3DXPlaneIntersectLine(&iXZ, &xz, &d3dStart, &d3dEnd)) iXZ = dPos;
	if(NULL==D3DXPlaneIntersectLine(&iYZ, &yz, &d3dStart, &d3dEnd)) iYZ = dPos;
		
	if(pXY) *pXY = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pXZ) *pXZ = A3DVECTOR3(iXZ.x, iXZ.y, iXZ.z);
	if(pYZ) *pYZ = A3DVECTOR3(iYZ.x, iYZ.y, iYZ.z);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinateDirection::CCoordinateDirection()
{
	m_nOldX = 0;
	m_nOldY = 0;

	m_nSelectedAxis = 0;
	m_nSelectedTemp = 0;

	m_enumET = EDIT_TYPE_ROTATE;

	m_matTranslate.Identity();;
	m_matABS.Identity();
	m_matRotate.Identity();
	m_scale = 1.0f;
	m_fDelta = 1.0f;
	m_LastRotateAngleRadians = 0.0f;
	m_LastScaleChange = 0.0f;
}

void CCoordinateDirection::SetPos(const A3DVECTOR3& vPos)
{
	m_vPos = vPos;
	m_matTranslate.Translate(vPos.x, vPos.y, vPos.z);
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

void CCoordinateDirection::GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const
{
	vDir = m_matRotate.GetRow(2);
	vUp = m_matRotate.GetRow(1);
}

void CCoordinateDirection::SetEditType(const EditType enumET)
{
	if (enumET == m_enumET)
		return;

	m_enumET = enumET; 
	Update();
}

A3DVECTOR3 CCoordinateDirection::GetSelectedAxis() const
{
	switch(m_nSelectedAxis)
	{
	case AXIS_X:
		return A3DVECTOR3(1.0f, 0, 0);
		break;
	case AXIS_Y:
		return A3DVECTOR3(0, 1.0f, 0);
		break;
	case AXIS_Z:
		return A3DVECTOR3(0, 0, 1.0f);
	    break;
	default:
		return A3DVECTOR3(0);
	    break;
	}
}

void CCoordinateDirection::Update()
{
	if (0 == g_Render)
		return;

	A3DVECTOR3 camPos = g_Render->GetCamera()->GetPos();
	// assumes a 90deg FOV
	A3DVECTOR3 dir = m_vPos - camPos;
	A3DVIEWPORTPARAM* param = g_Render->GetViewport()->GetParam();

	A3DMATRIX4 matScale;
	matScale.Identity();
	float mAxisGizmoProjLen = (60.0f / float(param->Height)) * dir.Magnitude();
	matScale.Scale(mAxisGizmoProjLen, mAxisGizmoProjLen, mAxisGizmoProjLen);

	if (EDIT_TYPE_ROTATE == m_enumET)
	{
		m_matABS = matScale * m_matRotate * m_matTranslate;
		return;
	}

	A3DMATRIX4 mat;
	mat.Identity();
	m_matABS = matScale * mat * m_matTranslate;
}

void CCoordinateDirection::Draw(A3DWireCollector& A3dWC) const
{
	DrawArrow(A3dWC);
	DrawAxis(A3dWC);
	DrawPlane(A3dWC);
}

void CCoordinateDirection::DrawArrow(A3DWireCollector& A3dWC) const
{
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
	A3dWC.AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(255,0,0));
	
	
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
	A3dWC.AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(0,255,0));

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
	A3dWC.AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(0,0,255));
	
}

void CCoordinateDirection::DrawPlane(A3DWireCollector& A3dWC) const
{
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
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));

	Vertices[0] =  A3DVECTOR3(0,m_fDelta*fPlane,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,m_fDelta*fPlane,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==PLANE_XY)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));
	
	//xz plane
	Vertices[0] =  A3DVECTOR3(m_fDelta*fPlane,0,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,0,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XZ)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));
	
	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*fPlane);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,0,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XZ)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
	
	//yz plane
	Vertices[0] =  A3DVECTOR3(0,m_fDelta*fPlane,0);;
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*fPlane,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_YZ)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));

	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*fPlane);;
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*fPlane,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==PLANE_YZ)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
}

A3DVECTOR3 CCoordinateDirection::GetAxisVec(const int iAxis) const
{
	A3DVECTOR3 dir;
	switch(iAxis)
	{
	case AXIS_X:
		dir.Set(1.0f, 0, 0);
		break;
	case AXIS_Y:
		dir.Set(0, 1.0f, 0);
		break;
	case AXIS_Z:
		dir.Set(0, 0, 1.0f);
		break;
	default:
		dir.Set(0, 0 ,0);
		break;
	}

	dir = dir * m_matRotate;
	dir.Normalize();
	return dir;
}

void CCoordinateDirection::DrawAxis(A3DWireCollector& A3dWC) const
{
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
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));
	g_Render->GetViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render->GetCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
		g_Render->TextOut(int(vScreenPos.x),int(vScreenPos.y),_AL("X"),1,A3DCOLORRGB(255,255,255));
	
	//y 轴
	Vertices[0] = A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*0.9f,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==AXIS_Y || m_nSelectedAxis==PLANE_XY || m_nSelectedAxis==PLANE_YZ)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));
	g_Render->GetViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render->GetCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
		g_Render->TextOut(int(vScreenPos.x),int(vScreenPos.y),_AL("Y"),1,A3DCOLORRGB(255,255,255));

	//z 轴
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(0,0,m_fDelta*0.9f);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==AXIS_Z || m_nSelectedAxis==PLANE_YZ || m_nSelectedAxis==PLANE_XZ)
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else
		A3dWC.AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
	g_Render->GetViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render->GetCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
		g_Render->TextOut(int(vScreenPos.x),int(vScreenPos.y),_AL("Z"),1,A3DCOLORRGB(255,255,255));
}

int CCoordinateDirection::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd) 
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
	A3DVECTOR3 vPlaneUp  = CrossProduct(vPlaneDir,g_Render->GetCamera()->GetDir());
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
	A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
	A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
	A3DViewport* pViewport =g_Render->GetViewport();
	pViewport->InvTransform(vStart,vStart);
	pViewport->InvTransform(vPos,vPos);
	
	return (RayTrace(vStart,vPos)!=AXIS_PLANE_NO);

}

bool CCoordinateDirection::GetLastRotateInfo(float& outAngleRadians, A3DVECTOR3& outAxis) const
{
	if (EDIT_TYPE_ROTATE != m_enumET)
		return false;

	outAngleRadians = m_LastRotateAngleRadians;
	outAxis = GetSelectedAxis();
	outAxis = m_matRotate * outAxis;
	outAxis.Normalize();
	return true;
}

bool CCoordinateDirection::GetLastScaleChange(float& outScaleChange) const
{
	if (EDIT_TYPE_SCALE	!= m_enumET)
		return false;

	outScaleChange = m_LastScaleChange;
	return true;
}

bool CCoordinateDirection::OnMouseMove(int x, int y, DWORD dwFlags)
{
	m_LastRotateAngleRadians = 0.0f;
	m_LastScaleChange = 0.0f;
	if (!(MK_LBUTTON & dwFlags))
	{
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DViewport* pViewport =g_Render->GetViewport();
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		if(AXIS_PLANE_NO != RayTrace(vStart,vPos))
			SetCursor(m_hCursor);
	}
	else
	{
		m_nSelectedAxis = m_nSelectedTemp;
		switch(m_enumET)
		{
		case EDIT_TYPE_SCALE:
			{
				A3DVECTOR3 vAxis = GetSelectedAxis();
				if(!vAxis.IsZero())
				{
					m_LastScaleChange =	(x - m_nOldX) * c_fRotationScale;
					m_scale += m_LastScaleChange;
				}
			}
			break;
		case EDIT_TYPE_MOVE:
			{
				A3DVECTOR3 vDelta = GetDeltaVector(m_vPos, x, y);
				m_vPos += vDelta;
				m_matTranslate.Translate(m_vPos.x, m_vPos.y, m_vPos.z);
			}
			break;
		case EDIT_TYPE_ROTATE:
			{
				A3DVECTOR3 vAxis = GetSelectedAxis();
				if(!vAxis.IsZero())
				{
					m_LastRotateAngleRadians = (m_nOldY - y) * c_fRotationScale;
					A3DMATRIX4 mtTmp;
					mtTmp.Identity();
					mtTmp.RotateAxis(vAxis, m_LastRotateAngleRadians);
					m_matRotate = mtTmp * m_matRotate;
				}
			}
			break;
		default:
			break;
		};
	}

	Update();
	m_nOldX = x;
	m_nOldY = y;
	return true;
}

bool CCoordinateDirection::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
	A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
	A3DViewport* pViewport =g_Render->GetViewport();
	pViewport->InvTransform(vStart,vStart);
	pViewport->InvTransform(vPos,vPos);
	m_nSelectedTemp = RayTrace(vStart,vPos);

	A3DMATRIX4 mat;
	mat.Identity();
	if (EDIT_TYPE_ROTATE == m_enumET)
		mat = m_matRotate;

	if(m_nSelectedTemp!=AXIS_PLANE_NO)
	{
		POINT pt; pt.x = x; pt.y = y;
		float dot[3];
		ScreenTraceAxis(*pViewport, pt, m_vPos, mat, &m_vXY,&m_vXZ,&m_vYZ,dot);
		m_vXY = m_vXY - m_vPos;
		m_vXZ = m_vXZ - m_vPos;
		m_vYZ = m_vYZ - m_vPos;
	}
	else 
	{
		m_vYZ = m_vXZ = m_vXY = A3DVECTOR3(0,0,0);
	}

	m_nOldX = x;
	m_nOldY = y;

	return (AXIS_PLANE_NO == m_nSelectedTemp)? false : true;
}

A3DVECTOR3 CCoordinateDirection::GetDeltaVector(const A3DVECTOR3& pos,int x,int y)
{
	A3DVECTOR3 vDelta = A3DVECTOR3(0.0f);
	POINT pt; pt.x = x; pt.y = y;

	A3DMATRIX4 mat;
	mat.Identity();
	if (EDIT_TYPE_ROTATE == m_enumET)
		mat = m_matRotate;
	
	float dot[3];
	A3DVECTOR3 xy(pos),xz(pos),yz(pos);
	ScreenTraceAxis(*g_Render->GetViewport(), pt, pos, mat, &xy,&xz,&yz,dot);

	switch(m_nSelectedAxis)
	{
	case  CCoordinateDirection::AXIS_X: 
		if(dot[0] < dot[1])
			vDelta = DotProduct(mat.GetRow(0), xz - pos - m_vXZ) * mat.GetRow(0);
		else 
			vDelta = DotProduct(mat.GetRow(0), xy - pos - m_vXY) * mat.GetRow(0);
		break;
		
	case  CCoordinateDirection::AXIS_Y: 
		if(dot[0] < dot[2])
			vDelta = DotProduct(mat.GetRow(1), yz - pos - m_vYZ) * mat.GetRow(1);
		else 
			vDelta = DotProduct(mat.GetRow(1), xy - pos - m_vXY) * mat.GetRow(1);
		break;
		
	case  CCoordinateDirection::AXIS_Z: 
		if(dot[1] < dot[2])
			vDelta = DotProduct(mat.GetRow(2), yz - pos - m_vYZ) * mat.GetRow(2);
		else 
			vDelta = DotProduct(mat.GetRow(2), xz - pos - m_vXZ) * mat.GetRow(2);
		break;
		
	case  CCoordinateDirection::PLANE_XY: 
		vDelta = xy - pos - m_vXY;
		break;
		
	case  CCoordinateDirection::PLANE_XZ: 
		vDelta = xz - pos - m_vXZ;
		break;
		
	case  CCoordinateDirection::PLANE_YZ: 
		vDelta = yz - pos - m_vYZ;
		break; 

	default:
		break;
	}
	return vDelta;
}