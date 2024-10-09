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
//		
//		1. make some clean up work...
//		
//		2. add the moving and rotating operations to the class
//
//-----------------------------------------------------------

#ifndef _COORDINATEDIRECTION_H_
#define _COORDINATEDIRECTION_H_


#include "A3DVECTOR.h"
#include "A3DMatrix.h"



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
	
	enum
	{
		Edit_Operation_None,
		Edit_Operation_Move,
		Edit_Operation_Rotate,
		
		// to do...
		// Edit_Operation_Scale,
	};

public:
	CCoordinateDirection();
	virtual ~CCoordinateDirection();

	void SetPos(const A3DVECTOR3& vPos);
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	void SetRotate(const A3DMATRIX4& matR){ m_matRotate = matR; };

	const A3DVECTOR3& GetPos() const 
	{
		return m_vPos;
	}

	void GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp);
	
	A3DVECTOR3 GetAxisVec(int iAxis);

	int  RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd);
	bool RayTrace(int x, int y);

	void Draw();

	int  GetSelectedFlags(){ return m_nSelectedAxis; };
	void SetSelectedFlags(int nFlag){ m_nSelectedAxis = nFlag; };
	void Update();
	
	bool OnMouseMove(int x, int y, DWORD dwFlags);
	bool OnLButtonDown(int x, int y, DWORD dwFlags);
	bool OnLButtonUp(int x, int y, DWORD dwFlags);

	void SetCursor(HCURSOR hc){ m_hCursor = hc; }; 

	void SetEditOperation(int iEditOperation) { m_iEditOperation = iEditOperation; }
	int GetEditOperation()	{ return m_iEditOperation; }

protected:

	void DrawArrow();
	void DrawPlane();
	void DrawAxis();
	void ReEdge(A3DVECTOR3 &vMaxEdge,A3DVECTOR3 &vMinEdge);
	
	bool TraceAxis(A3DVECTOR3 vAxis, A3DVECTOR3 vStart, A3DVECTOR3 vEnd, float& dis);
	bool TracePlane(A3DVECTOR3* pVertices, A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis);

	A3DVECTOR3 GetDeltaVector(const A3DVECTOR3& pos,int x,int y);

protected:

	int  m_nSelectedAxis;
	int  m_nSelectedTemp;

	A3DVECTOR3 m_vPos;
	A3DMATRIX4 m_matTranslate;
	
	A3DMATRIX4 m_matRotate;

	// matrix used for rendering...
	A3DMATRIX4 m_matABS;
	
	bool       m_bLMouseDown;
	HCURSOR    m_hCursor;

	float m_fDelta;

	// last mouse pos...
	int m_nOldX;
	int m_nOldY;

	//----------------------------------------------------
	int m_iEditOperation;
	// int m_nDirectFlag;
	A3DVECTOR3 m_vXY;
	A3DVECTOR3 m_vXZ;
	A3DVECTOR3 m_vYZ;

};

#endif
