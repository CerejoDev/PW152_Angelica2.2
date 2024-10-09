//----------------------------------------------------------
// Filename	: CoordinateDirection.h
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 坐标指示类。该类显示物体坐标信息，同时返回选中的
//			  坐标轴或是坐标平面
//-----------------------------------------------------------
// Revised by He Wenfeng, Oct. 21, 2008
//
// Change Descriptions:
//		1. make some clean up work...
//		2. add the moving and rotating operations to the class
//-----------------------------------------------------------
//-----------------------------------------------------------
// Revised by Liu Yang, May. 20, 2009
//
// Change Descriptions:
//		1. make some clean up work...
//		2. remove the global render dependency
//      3. calculate the matrix according to the moving and rotating operations
//      4. add the scale operation
//-----------------------------------------------------------

#pragma once
#ifndef _COORDINATEDIRECTION_H_
#define _COORDINATEDIRECTION_H_

#include <windows.h>
#include "A3DVECTOR.h"
#include "A3DMatrix.h"

class CRender;
class A3DWireCollector;

class CCoordinateDirection  
{
public:
	enum
	{
		AXIS_PLANE_NO = 0,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		PLANE_XY,
		PLANE_XZ,
		PLANE_YZ,
	};
	
	enum EditType
	{
		EDIT_TYPE_NONE   = 0,
		EDIT_TYPE_MOVE   = 1,
		EDIT_TYPE_ROTATE = 2,
		EDIT_TYPE_SCALE  = 3,
	};

public:
	CCoordinateDirection();

	const A3DVECTOR3 GetPos() const { return m_vPos; }
	void SetPos(const A3DVECTOR3& vPos);

	void GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const;
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

	float GetScale() const { return m_scale; }
	void  SetScale(const float& fScale) { m_scale = fScale; }

	A3DMATRIX4 GetRotate() const { return m_matRotate; }
	void SetRotate(const A3DMATRIX4& matR) { m_matRotate = matR;  Update(); };

	EditType GetEditType() const { return m_enumET; }
	void SetEditType(const EditType enumET);

	int  RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd);
	bool RayTrace(int x, int y);

	int  GetSelectedFlags() const { return m_nSelectedAxis; };
	void SetSelectedFlags(const int nFlag) { m_nSelectedAxis = nFlag; };

	bool OnMouseMove(int x, int y, DWORD dwFlags);
	bool OnLButtonDown(int x, int y, DWORD dwFlags);

	void Draw(A3DWireCollector& A3dWC) const;
	void SetCursor(HCURSOR hc){ m_hCursor = hc; }; 
	A3DVECTOR3 GetAxisVec(const int iAxis) const;

private:
	void Update();
	void DrawArrow(A3DWireCollector& A3dWC) const;
	void DrawPlane(A3DWireCollector& A3dWC) const;
	void DrawAxis(A3DWireCollector& A3dWC) const;
	void ReEdge(A3DVECTOR3& vMaxEdge, A3DVECTOR3& vMinEdge);
	
	bool TraceAxis(A3DVECTOR3 vAxis, A3DVECTOR3 vStart, A3DVECTOR3 vEnd, float& dis);
	bool TracePlane(A3DVECTOR3* pVertices, A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis);

	A3DVECTOR3 GetDeltaVector(const A3DVECTOR3& pos, int x, int y);
	A3DVECTOR3 GetSelectedAxis();

private:
	int  m_nOldX;
	int  m_nOldY;

	int  m_nSelectedAxis;
	int  m_nSelectedTemp;

	EditType m_enumET;
	A3DVECTOR3 m_vXY;
	A3DVECTOR3 m_vXZ;
	A3DVECTOR3 m_vYZ;

	A3DVECTOR3 m_vPos;
	A3DMATRIX4 m_matTranslate;

	// matrix used for rendering...
	A3DMATRIX4 m_matABS;
	A3DMATRIX4 m_matRotate;

	float m_scale;
	
	float m_fDelta;
	HCURSOR m_hCursor;

public:
	static CRender* g_Render;
};

#endif
