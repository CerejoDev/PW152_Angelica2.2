/********************************************************************
  created:	   27/10/2005  
  filename:	   ECBrushManUtil.h
  author:      Wangkuiwu  
  description: utility of brush manager
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma  once 

#include <vector.h>
#include "aabbcd.h"
#include "a3dgeometry.h"
#include "hashtab.h"
#include "ConvexBrush.h"
using namespace abase;
using namespace CHBasedCD;

class ECBrushProvider;
class CAABBTreeLeaf;
class ECTraceInfo;



//@note : verbose statistics. By Kuiwu[26/10/2005]
#define BMAN_VERBOSE_STAT  0
#define  BMAN_USE_GRID
//#define  BMAN_AUTO_BUILD

//#define  BMAN_TRACE_OUTOFRANGE

#if BMAN_VERBOSE_STAT
class CBManStat
{
public:
	DWORD dwBuildTime;
	DWORD dwTraceTime;
	int   nProvider;
	int   nBrush;
	int   nTraceBrush;
	CCDBrush * pHitBrush;
#ifdef BMAN_USE_GRID
	int   nOutOfRangeBrush;
	int   nCellSize;
	int   nWidth, nHeight;
#endif
	CBManStat()
	{
		dwBuildTime = dwTraceTime = 0;
		nProvider = nBrush = nTraceBrush = 0;
		pHitBrush = NULL;
#ifdef BMAN_USE_GRID
		nOutOfRangeBrush = nCellSize = nWidth = nHeight = 0;
#endif
	}
protected:
private:
};
#endif

//////////////////////////////////////////////////////////////////////////


class ECBaseBrush
{
public:
	ECBaseBrush(){}
	virtual ~ECBaseBrush(){}

	virtual void Release() = 0;
	virtual DWORD GetReservedDWORD() = 0;
	virtual void SetReservedDWORD(DWORD dwReserved) = 0;
	virtual A3DAABB GetAABB()const = 0;
	virtual bool Trace(BrushTraceInfo * pInfo){ return false;}
	virtual bool Trace(AbstractBrushTraceInfo* pInfo, bool bCheckFlag) { return false;}

	//get its provider
	virtual ECBrushProvider* GetProvider() { return NULL; }
};

class ECCDBrush: public ECBaseBrush
{
public:
	ECCDBrush() { m_pBrush = new CCDBrush;}
	~ECCDBrush() { Release(); delete m_pBrush;}

public:

	DWORD GetReservedDWORD()
	{
		return m_pBrush->GetReservedDWORD();
	}

	void SetReservedDWORD(DWORD dwReserved)
	{
		m_pBrush->SetReservedDWORD(dwReserved);	
	}
	void Release()
	{
		m_pBrush->Release();
	}
	A3DAABB GetAABB()const
	{
		return m_pBrush->GetAABB();
	}

	bool Trace(BrushTraceInfo * pInfo)
	{ 
		return m_pBrush->Trace(pInfo);
	}

	CCDBrush* GetCDBrush() {return m_pBrush;}


protected:
	CCDBrush* m_pBrush;
private:
};

class ECConvexBrush: public ECBaseBrush
{
public:
	//
	ECConvexBrush(CConvexBrush* pBrush, ECBrushProvider* pProvider, const A3DAABB& aabbInWorld)
	{
		m_pBrush = pBrush;
		m_pProvider = pProvider;
		m_aabbInWorld = aabbInWorld;
	}
	~ECConvexBrush() { Release(); }
public:
	DWORD GetReservedDWORD()
	{
		return m_pBrush->GetCollisionFlag();
	}

	void SetReservedDWORD(DWORD dwReserved)
	{
		m_pBrush->SetCollisionFlag(dwReserved);	
	}
	void Release()
	{
	//	m_pBrush->Release();
	}
	//aabb (world coordinate)
	A3DAABB GetAABB()const
	{
		return m_aabbInWorld;
	}

	bool Trace(AbstractBrushTraceInfo* pInfo, bool bCheckFlag)
	{
		return m_pBrush->BrushTrace(pInfo, bCheckFlag);
	}

	virtual ECBrushProvider* GetProvider() { return m_pProvider; }

protected:
	CConvexBrush* m_pBrush;
	A3DAABB       m_aabbInWorld;//convex brush aabb(world coordinate)
	ECBrushProvider* m_pProvider;//pointer to provider
};

//////////////////////////////////////////////////////////////////////////

class ECBrushCell
{
public:
	short   m_iX, m_iZ;  //maybe useless
	A3DAABB   m_AABB;

//	vector <CCDBrush * > m_pBrushes;
	vector<ECBaseBrush*> m_pBrushes; // ºÊ»› aabb collision£¨∫Õ capsule collision

protected:
private:
};

//@desc : using grid to manage the brushes. By Kuiwu[2/11/2005]
class ECBrushGrid
{
public:
	ECBrushGrid(int iCellSize = 80, int iW = 11, int iH = 11 );
	~ECBrushGrid();  //non-virtual
	void AddProvider(ECBrushProvider * pProvider);
	bool RemoveProvider(ECBrushProvider * pProvider);
	void Build(const A3DVECTOR3& vCenter, bool bForce = false);
	bool  Trace(BrushTraceInfo * pInfo, bool bCheckFlag = true);

	// new for capsule collision
	bool  Trace(AbstractBrushTraceInfo* pInfo, bool bCheckFlag = true);

	bool  Trace(ECTraceInfo* pInfo, bool bCheckFlag = true);

	void Render(); // Yongdong: TODO delete.

#if BMAN_VERBOSE_STAT
	CBManStat * GetStat()
	{
		return &m_Stat;
	}
#endif
protected:
private:
	int   m_iCellSize;
	int   m_iW, m_iH;   //in cell
	short m_iCenterX, m_iCenterZ;
	
	float    m_fTraceRange2;
	
	ECBrushCell * m_pBrushCell;  //as a brush cell buffer
	//@note : use hash table to accelerate searching cell buffer. By Kuiwu[2/11/2005]
	typedef hashtab<ECBrushCell* , DWORD, abase::_hash_function>  CellTable;
	CellTable	m_CellTbl;

	vector<ECBrushProvider *> m_UnOrganizedProvider;  //to be built.

	//	vector<CCDBrush * > m_OutOfRangeBrush; 	
	vector<ECBaseBrush*> m_OutOfRangeBrush;

	
	inline void _GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z);
	inline ECBrushCell * _FindCell(short x, short z);

	bool _UpdateCenter(const A3DVECTOR3& vCenter, bool bForce);


	//inline bool _AddBrush(CCDBrush* pBrush);

	inline bool	_AddBrush(ECBaseBrush* pBrush);
#if BMAN_VERBOSE_STAT
	CBManStat     m_Stat;
#endif	

};