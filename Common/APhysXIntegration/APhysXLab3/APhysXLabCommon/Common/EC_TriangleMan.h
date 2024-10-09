/********************************************************************
  created:	   8/11/2005  
  filename:	   EC_TriangleMan.h
  author:      Wangkuiwu  
  description: Triangle manager, mainly used by shadow generator.
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma  once

#ifndef _ANGELICA3

#include <vector.h>
#include "a3dgeometry.h"
#include "hashtab.h"
#include <hashmap.h>
using namespace abase;

//#define TMAN_VERBOSE_STAT

class TriangleProvider;
class CECTriangleMan;
class CTriangleGrid;
struct CTriangleCell;
struct TManTriangle;
class CTManTrianglePool;
class A3DWireCollector;
class A3DCameraBase;
struct A3DLMVERTEX;

class triman_alloc
{
public: 

	static void* allocate(size_t size);

	inline static void deallocate(void * ptr,size_t size) 
	{ 
	}

	inline static void deallocate(void * ptr) 
	{ 
	}	
};

class triman_mem_man
{
public:

	enum
	{
		mem_block_size = (1024 * 1024),
	};

	triman_mem_man()
	{
		cur_block = 0;
		cur_used = 0;
	}

	~triman_mem_man()
	{
		clear();
	}

	void clear()
	{
		for (size_t i = 0; i < blocks.size(); i++)
			delete[] blocks[i];

		blocks.clear();
		cur_block = 0;
		cur_used = 0;
	}

	void* allocate(size_t size)
	{
		if (size >= mem_block_size)
		{
			BYTE* p = new BYTE[size];
			blocks.push_back(p);
			return p;
		}
		else if (size + cur_used > mem_block_size || cur_block == 0)
		{
			BYTE* p = new BYTE[mem_block_size];
			cur_block = p;
			blocks.push_back(p);
			cur_used = size;
			return p;
		}
		else
		{
			BYTE* p = cur_block + cur_used;
			cur_used += size;
			return p;
		}
	}

protected:

	abase::vector<BYTE*> blocks;
	BYTE* cur_block;
	size_t cur_used;
};

//@note : NOT define an abstract interface for triangle provider.
//		  Use CELBrushBuilding directly.
//		  By Kuiwu[8/11/2005]
class CELBrushBuilding;
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

struct  TMan_Overlap_Info
{
	//input
	A3DVECTOR3   vStart;			// as an aabb's center
	A3DVECTOR3   vExtent;     

	A3DVECTOR3   vDelta;			// the front dir
	A3DVECTOR3   vRight;			// the front dir
	A3DVECTOR3   vBottom;			// the front dir
	bool		 bUsedirs;

	A3DCameraBase* pCamera;			//optional, for frustum culling
	DWORD        dwFlag;			//check flag, OBSOLETE!
	bool		 bCheckNormal;
	bool		 bIncCellStamp;

	//output
	vector<A3DVECTOR3 >  pVert;   
	vector<WORD>         pIndex; 
	//vector<A3DVECTOR3 >  pNormals;
	A3DVECTOR3	vNormal;

	TMan_Overlap_Info()
	{
		bUsedirs = false;
		pCamera = NULL;
		dwFlag = TMAN_CHECK_DEFAULT;
		vDelta.Clear();
	}

};

struct TMRayTraceInfo
{
	// input ...
	A3DVECTOR3 vStart;
	A3DVECTOR3 vDelta;
	bool bCullBackFace;

	// output ...
	float fFraction;
	A3DVECTOR3 vHitPos;
	A3DVECTOR3 vHitNormal;

	TMRayTraceInfo()
	{
		fFraction = 1.0f;
		bCullBackFace = true;
	}
};

struct TMCapsuleTraceInfo
{
	A3DVECTOR3 vStart;
	A3DVECTOR3 vDelta;
	bool bCullBackFace;

	float fCapsuleRadius;
	float fCylinderHei;

	//output
	float fFraction;
	A3DVECTOR3 vHitPos;
	A3DVECTOR3 vHitNormal;
	int nTriangle;

	TMCapsuleTraceInfo()
	{
		fFraction = 1.0f;
		bCullBackFace = true;
		nTriangle = 0;
	}
};

#ifdef TMAN_VERBOSE_STAT
class CTManStat
{
public:
	DWORD dwBuildTime;
	DWORD dwTestTime;
	int   nProvider;  // all provider
	int   nUnorgPrv;
	int   nTriangle;  // org triangle 
	int   nTestTriangle;
	int   nOutOfRangeTriangle;
	int   nCellSize;
	int   nWidth, nHeight;
	int   nAddTriangle;  //the number of triangles that added during one build process.
	//CTriangleCell  * pCells;
	TManCellTable * pCellTbl;
	A3DOBB  obb;
	CTManStat()
	{
		dwBuildTime = dwTestTime = 0;
		nUnorgPrv = nProvider = nTriangle = nTestTriangle = nOutOfRangeTriangle = 0;
		nCellSize = nWidth = nHeight =  nAddTriangle = 0;
		obb.Clear();
	}
protected:
private:
};
#endif

//@note : Maybe it is better to use a template class to 
//		  implement the grid method(brush grid, triangle grid, etc). 
//		  By Kuiwu[8/11/2005]
class CTriangleGrid
{
public:
	CTriangleGrid(CECTriangleMan* pTriMan, int iCellSize = 10, int iW =11, int iH = 11);
	~CTriangleGrid();
	//void AddProvider(CELBrushBuilding * pProvider);
	void AddProvider(TriangleProvider* pProvider);
	void RemoveProvider(TriangleProvider* pProvider);

	void Build(const A3DVECTOR3& vCenter, DWORD dwFlag = TMAN_BUILD_DEFAULT);
	void BuildGradually(const A3DVECTOR3& vCenter, DWORD dwFlag = TMAN_BUILD_DEFAULT);
	void BuildFromTerrain(const float* pTerrainHeight, int nWidth, int nHeight);
	void Clear();
	void SetBreakLoadFlag(bool b) { m_bBreakLoadFlag = b; }
	bool GetBreakLoadFlag() const { return m_bBreakLoadFlag; }
	
	/*
	 * discard the flag.
	 * @desc :
	 * @param :     
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [18/11/2005]
	 * @ref:
	 */
	void TestOverlapNF(TMan_Overlap_Info& tInfo);
	void GetCell(TMan_Overlap_Info& tInfo);
	float GetPosHeight(const A3DVECTOR3& vPos);

	void SetMapOrgPos(float x, float z)
	{
		m_fMapOrgX = x;
		m_fMapOrgZ = z;
	}

	bool VertRayTrace(TMRayTraceInfo& rtInfo);
	bool CapsuleTrace(TMCapsuleTraceInfo& trInfo );

#ifdef TMAN_VERBOSE_STAT
	CTManStat * GetStat()
	{
		return &m_Stat;
	}
	
#endif	
protected:
private:

	CECTriangleMan* m_pTriMan;

	int   m_iCellSize;
	int   m_iW, m_iH;   //in cell
	short m_iCenterX, m_iCenterZ;
	float m_fMapOrgX;
	float m_fMapOrgZ;
	
	int   m_nCellCount;  //max cell count
	//CTriangleCell * m_pTriangleCell;  //as a triangle cell buffer

	//@note : use hash table to accelerate searching cell buffer. By Kuiwu[8/11/2005]
	CTriangleCell**	m_CellTbl;
	bool m_bEnable;

	float   m_fOffset;   //push out dist
	
	vector<TriangleProvider *> m_UnOrganizedProvider;  //to be built.
	//vector<TManTriangle > m_OutOfRangeTriangle; 

	CTManTrianglePool   * m_pTrianglePool;
	ASmallBufMemMan		m_SmallBufMemMan;
	BYTE                m_btTestStamp;
	volatile bool		m_bBreakLoadFlag;

	bool _UpdateCenter(const A3DVECTOR3& vCenter, DWORD dwFlag);
	inline void _GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z);
	inline CTriangleCell * _FindCell(short x, short z);
	inline	bool   _AddTriangle(TManTriangle * pTri, int index, const A3DVECTOR3& v0,  const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& vNormal, bool bPreComput);

	bool   _TestTriangle(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri);
	inline bool _TestTriangleNF(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri, const A3DMATRIX4& w2lMat, const A3DMATRIX4& l2wMat);
	void   _ReleaseCellTbl();
	void   _BuildOBB(const TMan_Overlap_Info& tInfo, A3DOBB& obb);

	int    _AddProvider(TriangleProvider* pProvider, bool bPreCompute);

	bool Enabled() const { return m_bEnable; }
	
#ifdef TMAN_VERBOSE_STAT
	CTManStat  m_Stat;
#endif
	
};

typedef void (*TRIMAN_LOADPRG_CALLBACK)(void* pParam, float fStep);



class CECTriangleMan  
{
public:
	CECTriangleMan();
	virtual ~CECTriangleMan();
	//void AddProvider(CELBrushBuilding * pProvider);
	void AddProvider(TriangleProvider* pProvider);
	void RemoveProvider(TriangleProvider* pProvider);
	void Build(const A3DVECTOR3& vCenter, DWORD dwFlag);
	void BuildFromTerrain(const float* pTerrainHeight, int nWidth, int nHeight);
	void TestOverlap(TMan_Overlap_Info& tInfo);
	void GetCell(TMan_Overlap_Info& tInfo);
	float GetPosHeight(const A3DVECTOR3& vPos) { return m_pGrid->GetPosHeight(vPos); }

	DWORD GetThreadId() const { return m_dwThreadId; }
	void SetMapOrgPos(float x, float z) { m_pGrid->SetMapOrgPos(x, z); }

	// 竖直向下RayTrace，其他方向的光线不保证得到的结果正确
	bool VertRayTrace(TMRayTraceInfo& rtInfo);
	bool CapsuleTrace( TMCapsuleTraceInfo& trInfo );

	bool IsReadyToUse() const { return m_bReadyToUse; }
	void SetReadToUse() { m_bReadyToUse = true; }
	void Clear();
	bool GetBreakLoadFlag() const { return m_pGrid->GetBreakLoadFlag(); }
	void SetBreakLoadFlag(bool b) { m_pGrid->SetBreakLoadFlag(b); }

	void SetLoadProgressCallBack(TRIMAN_LOADPRG_CALLBACK pCB, void* pParam)
	{
		m_pLoadProgressCallback = pCB;
		m_pLoadPrgCBParam = pParam;
	}
	TRIMAN_LOADPRG_CALLBACK GetLoadProgressCallBack(void*& pParam)
	{
		pParam = m_pLoadPrgCBParam;
		return m_pLoadProgressCallback;
	}


#ifdef TMAN_VERBOSE_STAT
	CTManStat * GetStat()
	{
		return m_pGrid->GetStat();
	}
#endif

protected:

	TRIMAN_LOADPRG_CALLBACK m_pLoadProgressCallback;
	void* m_pLoadPrgCBParam;

private:
	CTriangleGrid * m_pGrid;
	triman_mem_man m_MemoryMan;
	DWORD m_dwThreadId;
	volatile bool m_bReadyToUse;
};

#pragma pack(1)

struct TManTriangle
{
	TriangleProvider*   pProvider;
	WORD				index[3];
	BYTE                iSubMesh;
	BYTE				wCheckStamp;

	TManTriangle()
	: pProvider(NULL)
	, wCheckStamp(0)
	{
	}
};

struct CTriangleCell
{
public:

	int* m_Triangles;
	WORD wCurIndex;
	WORD wCount;
	WORD m_wMaxHeight;
	BYTE m_btTestStamp;
	bool m_bAllTested;

	void Reset()
	{
		m_Triangles = NULL;
		wCurIndex = 0;
		wCount = 0;
		m_wMaxHeight = 0;
		m_btTestStamp = 0;
		m_bAllTested = false;
	}

	int GetTriCount() const { return wCurIndex; }
};

#pragma pack()

const int m_nTriManufSize = 50000;

class CTManTrianglePool
{
public:
	CTManTrianglePool();
	~CTManTrianglePool();

	int CTManTrianglePool::CreateTriangle()
	{
		if (m_nLast >= m_nMaxCount)
			_Grow();

		int ret = m_nLast;
		m_nLast++;
		return ret;
	}

	TManTriangle * GetTriangle(int index)
	{
		int i = index % m_nTriManufSize;
		int j = index / m_nTriManufSize;
		return &m_aBuf[j][i];
	}

	int GetTriangleNum() const { return m_nLast; }

protected:

	abase::vector<TManTriangle*> m_aBuf;
	int            m_nMaxCount;
	int            m_nLast;
	void _Grow();
};

#endif  //_ANGELICA3