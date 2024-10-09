/********************************************************************
created:	   8/11/2005  
filename:	   ECTriangleMan.h
author:      Wangkuiwu  
description: Triangle manager, mainly used by shadow generator.
Copyright (c) 2005 Archosaur Studio , All Rights Reserved.
*********************************************************************/

#pragma  once

#include <vector.h>
#include <a3dgeometry.h>
#include <a3dcamerabase.h>
#include <hashtab.h>
using namespace abase;
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////
class ECBuilding;
class ECTriProvider;


//build flag
enum
{
	TMAN_BUILD_DEFAULT = 0,
	TMAN_BUILD_FORCE = 1
};


//check  flag
enum
{
	TMAN_CHECK_DEFAULT = 0,				//fast, but not accurate
	TMAN_CHECK_CULLBACK = 1,			//cull back face
	TMAN_CHECK_STRICT   =  (1<<1),		//more accurate but a little slow
	TMAN_CHECK_OUTOFRANGE = (1<<2),		//check with out-of-range triangle, OBSOLETE!
	TMAN_CHECK_FRUSTUM = (1<<3),		// view frustum culling
  	TMAN_CHECK_AUTOBUILD = (1<<4),      //auto build when necessary    
	TMAN_CHECK_EXACT = (1<<5)			//exact check but expensive
};

class A3DLitModel;
class ECTriangleCell;
typedef hashtab<ECTriangleCell* , DWORD, abase::_hash_function>  TManCellTable;

struct  TMan_Overlap_Info
{
	//input
	A3DVECTOR3   vStart;			// as an aabb's center
	A3DVECTOR3   vExtent;     
	A3DVECTOR3   vDelta;
	A3DCameraBase    *pCamera;			//optional, for frustum culling
	DWORD        dwFlag;			//check flag, OBSOLETE!
	bool		 bCheckNormal;

	//output
	vector<A3DVECTOR3 >  pVert;   
	vector<WORD>         pIndex;  
	TMan_Overlap_Info()
	{
		pCamera = NULL;
		dwFlag = TMAN_CHECK_DEFAULT;
		vDelta.Clear();
		pIndex.clear();
	}

};


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class TManTriangle
//	
///////////////////////////////////////////////////////////////////////////

struct TManTriangle
{
	ECTriProvider * pProvider;
	A3DVECTOR3     vVert[3];
	
	//A3DVECTOR3    vCenter;
	A3DVECTOR3    vNormal;  //face normal
	DWORD         dwCheckStamp;
	//@note : not need a DWORD, but for alignment. By Kuiwu[18/11/2005]
	DWORD          dwRef;     // ref count,  by triangle cell
	
	TManTriangle()
	:dwCheckStamp(0), dwRef(0)
	{
	}
	
};



///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTManTrianglePool
//	
///////////////////////////////////////////////////////////////////////////
class ECTManTrianglePool
{
public:
	ECTManTrianglePool(int nCount = 40000, int nGrow = 40000);
	~ECTManTrianglePool();
	inline int CreateTriangle();
	TManTriangle * GetTriangle(int index)
	{
		return &m_pBuf[index];
	}
protected:
private:
	TManTriangle * m_pBuf;
	int            m_nMaxCount;

	int            m_nLast;
	int            m_nGrow;

	inline void _Grow(int  newcount);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTriangleMan
//	
///////////////////////////////////////////////////////////////////////////
class ECTriangleCell
{
public:
	A3DAABB  m_AABB;
	//vector<TManTriangle *>   m_Triangles;
	vector<int>   m_Triangles;
	ECTriangleCell()
	{
		m_AABB.Clear();
		m_Triangles.clear();
	}
protected:
private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTriangleMan
//	
///////////////////////////////////////////////////////////////////////////
class ECTriangleGrid
{
public:
	ECTriangleGrid(int iCellSize = 10, int iW =11, int iH = 11);
	~ECTriangleGrid();

	void AddProvider(ECTriProvider * pProvider);
	bool RemoveProvider(ECTriProvider * pProvider);
	void BuildGradually(const A3DVECTOR3& vCenter, DWORD dwFlag = TMAN_BUILD_DEFAULT);

	void TestOverlapNF(TMan_Overlap_Info& tInfo);	

	//	Only for test add trangle Niuyadong[2009-10-19]
	int TestProvider(A3DLitModel * pLitModel);
protected:
private:
	int		m_iCellSize;
	int		m_iW, m_iH;   //in cell
	short	m_iCenterX, m_iCenterZ;
	int		m_nCellCount;  //max cell count



	ECTManTrianglePool*		m_pTrianglePool;
	DWORD					m_dwTestStamp;
	TManCellTable			m_CellTbl;
	vector<ECTriProvider *>	m_UnOrganizedProvider;  //to be built.

	int _AddProvider(ECTriProvider * pProvider);
	bool _UpdateCenter(const A3DVECTOR3& vCenter, DWORD dwFlag);
	void _GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z);
	void _CompactCellTbl();
	ECTriangleCell * _FindCell(short x, short z);
	void _BuildOBB(const TMan_Overlap_Info& tInfo,  A3DOBB& obb);
	inline bool _TestTriangleNF(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri, const A3DMATRIX4& w2lMat, const A3DMATRIX4& l2wMat);
	void _ReleaseCellTbl();
	bool _AddTriangle(TManTriangle * pTri,  int index);
		
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTriangleMan
//	
///////////////////////////////////////////////////////////////////////////

class ECTriangleMan
{
public:		//	Types


protected:	//	Types


public:	//	Constructor and Destructor

	ECTriangleMan();
	virtual ~ECTriangleMan();

public:		//	Attributes

public:		//	Operations
	void AddProvider(ECTriProvider * pProvider);
	bool RemoveProvider(ECTriProvider * pProvider);
	void Build(const A3DVECTOR3& vCenter, DWORD dwFlag  = TMAN_BUILD_DEFAULT);

	void TestOverlap(TMan_Overlap_Info& tInfo);
	int TestProvider(A3DLitModel * pLitModel);


protected:	//	Attributes
	ECTriangleGrid * m_pGrid;


protected:	//	Operations
	
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////