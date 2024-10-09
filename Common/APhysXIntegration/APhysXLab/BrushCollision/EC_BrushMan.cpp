/********************************************************************
  created:	   25/10/2005  
  filename:	   EC_BrushMan.cpp
  author:      Wangkuiwu  
  description: brush manager for cd
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/


//#include "stdafx.h"
#include "EC_BrushMan.h"
#include "a3dcollision.h"

#if BMAN_VERBOSE_STAT
#include "acounter.h"
#endif

//#define  BMAN_DEBUG

#ifdef BMAN_DEBUG
#define OUTPUT_DEBUG_INFO(s)   OutputDebugStringA(s)
#endif



//low =x, high = z
#define MAKE_CELL_KEY(x, z)    ((DWORD)(((WORD)((x) & 0xffff)) | ((DWORD)((WORD)((z) & 0xffff))) << 16))

/*
 * The template function determines the lowest value of N in the range 
 * [0, last - first) for which the predicate *(first + N) == val is true. 
 * It then returns first + N. If no such value exists, the function returns 
 * last. It evaluates the predicate once, at most, for each N.
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [2/11/2005]
 * @ref: std find.
 */
template<class InputIterator, class T> inline
     InputIterator linear_find(InputIterator first, InputIterator last, const T& value)
{
	InputIterator it = first;
	while (it != last)
	{
		if (*it == value)
			break;	
		++it;
	}

	return it;
}


CECBrushMan::CECBrushMan()
{
	m_pBrushGrid = new CBrushGrid(60, 15, 15);
}

CECBrushMan::~CECBrushMan()
{
	Release();
}

void CECBrushMan::Release()
{
   if (m_pBrushGrid)
   {
	   delete m_pBrushGrid;
	   m_pBrushGrid = NULL;
   }
}

void CECBrushMan::Build(const A3DVECTOR3& vCenter, bool bForce)
{

	//@note : make the caller happy. By Kuiwu[3/11/2005]
	//return;
	
	m_pBrushGrid->Build(vCenter, bForce);
}



bool CECBrushMan::RemoveProvider(CBrushProvider * pBrushProvider)
{

	//@note : make the caller happy. By Kuiwu[3/11/2005]
	//return true;

   if (m_pBrushGrid == NULL)
		return false;

   bool bRet=	m_pBrushGrid->RemoveProvider(pBrushProvider);

#ifdef BMAN_DEBUG
	char msg[100];
	sprintf(msg, "remove provider\n");
	OUTPUT_DEBUG_INFO(msg);
#endif

	return bRet;
}


void CECBrushMan::AddProvider(CBrushProvider * pBrushProvider)
{
	//@note : make the caller happy. By Kuiwu[3/11/2005]
	//return;

	m_pBrushGrid->AddProvider(pBrushProvider);
#ifdef BMAN_DEBUG
	char msg[100];
	sprintf(msg, "add provider \n");
	OUTPUT_DEBUG_INFO(msg);
#endif

	
}


bool CECBrushMan::Trace(AbstractBrushTraceInfo * pInfo, bool bCheckFlag /* = true */)
{
	return m_pBrushGrid->Trace(pInfo, bCheckFlag);
}




CBrushGrid::CBrushGrid(int iCellSize /* = 80 */, int iW /* = 11 */, int iH /* = 11  */)
:m_iCellSize(iCellSize), 
 m_iW(iW), m_iH(iH), 
 m_CellTbl(256)
{
    m_pBrushCell = new CBrushCell[iW * iH];
	assert(m_pBrushCell != NULL);
	m_UnOrganizedProvider.clear();
	m_OutOfRangeBrush.clear();
	m_iCenterX = -30000;
	m_iCenterZ = -30000;
	m_CellTbl.clear();
#if BMAN_VERBOSE_STAT
#ifdef BMAN_USE_GRID
	m_Stat.nCellSize = m_iCellSize;
	m_Stat.nWidth = m_iW;
	m_Stat.nHeight = m_iH;
	
#endif
#endif

	m_fTraceRange2 = 300.0f * 300.0f ;
}

CBrushGrid::~CBrushGrid()
{
	delete[] m_pBrushCell;
	m_UnOrganizedProvider.clear();
	m_OutOfRangeBrush.clear();
	m_CellTbl.clear();
}


void CBrushGrid::AddProvider(CBrushProvider * pProvider)
{
	assert(pProvider != NULL);

	int n = pProvider->GetBrushCount();
	m_UnOrganizedProvider.push_back(pProvider);
#if BMAN_VERBOSE_STAT
	m_Stat.nProvider++;
	m_Stat.nBrush += (pProvider->GetBrushCount());
#endif

}

bool CBrushGrid::RemoveProvider(CBrushProvider * pProvider)
{
#if BMAN_VERBOSE_STAT
	m_Stat.pHitBrush = NULL;
	m_Stat.nProvider--;
	m_Stat.nBrush-= (pProvider->GetBrushCount());
#endif
	//find un-organize list  first
	CBrushProvider ** ppPrv = linear_find(m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.begin()+ m_UnOrganizedProvider.size(), pProvider);
	if (ppPrv != m_UnOrganizedProvider.begin() + m_UnOrganizedProvider.size() )
	{//in the un-organized list
		m_UnOrganizedProvider.erase(ppPrv);
		return true;
	}
	//maybe in the organized cells
	
	CellTable::iterator it = m_CellTbl.begin();
	for (; it != m_CellTbl.end(); ++it)
	{
		CBrushCell* pBrushCell = *it.value(); 
		
		for (int i = 0; i < pBrushCell->m_pBrushDatas.size(); )
		{
			if (pBrushCell->m_pBrushDatas[i].pProvider == pProvider)
				pBrushCell->m_pBrushDatas.erase(pBrushCell->m_pBrushDatas.begin() + i);
			else
				i++;
		}
	}
	for (int i = 0; i < m_OutOfRangeBrush.size(); )
	{
		if (m_OutOfRangeBrush[i].pProvider == pProvider)
			m_OutOfRangeBrush.erase(m_OutOfRangeBrush.begin() + i);
		else
			i++;
	}
	
	return true;
}

void CBrushGrid::_GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z)
{
	x = (short)(vPos.x / (float)m_iCellSize);
	z=  (short)(vPos.z / (float)m_iCellSize);

}

bool CBrushGrid::_UpdateCenter(const A3DVECTOR3& vCenter, bool bForce)
{

	short iNewX, iNewZ;
	_GetCellIndex(vCenter, iNewX, iNewZ);
	if(!bForce && (iNewZ == m_iCenterZ) && (iNewX == m_iCenterX ))
	{
		return false;
	}
	short ix, iz, ix0, iz0;
	int  halfW = (m_iW -1) / 2;
	ix0 = iNewX - halfW;
	int halfH = (m_iH -1) /2;
	iz0 = iNewZ - halfH;
	CBrushCell * pCell;
	DWORD dwKey;
	for (iz = iz0; iz < iz0 + m_iH; ++iz)
	{
		for (ix = ix0; ix < ix0 + m_iW; ++ix)
		{
			pCell = _FindCell(ix, iz);
			if (pCell)
			{//common
				continue;
			}
			int nTbl = m_CellTbl.size();
			if (nTbl < m_iW * m_iH)
			{//not full
				pCell = &m_pBrushCell[nTbl];
			}
			else
			{//need replace 
				short iRX, iRZ;
				iRX = iNewX + m_iCenterX - ix;
				iRZ = iNewZ + m_iCenterZ - iz;
				pCell = _FindCell(iRX, iRZ);
				assert(pCell != NULL && "replace cell not exist!");
				if (pCell)
				{//put back the brushes
					for (unsigned int i = 0; i < pCell->m_pBrushDatas.size(); ++i)
					{
						m_OutOfRangeBrush.push_back(pCell->m_pBrushDatas[i]);
					}
				 //remove from the hashtab
				   dwKey = MAKE_CELL_KEY(iRX, iRZ);
				   m_CellTbl.erase(dwKey);
				}
			}
			pCell->m_iX = ix;
			pCell->m_iZ = iz;
			pCell->m_AABB.Clear();
			pCell->m_pBrushDatas.clear();
			dwKey = MAKE_CELL_KEY(ix, iz);
			m_CellTbl.put(dwKey, pCell);
		}
	}
	m_iCenterX = iNewX;
	m_iCenterZ = iNewZ;

	return true;
}

void CBrushGrid::Build(const A3DVECTOR3& vCenter, bool bForce)
{

#ifdef BMAN_DEBUG
	char msg[200];
	sprintf(msg, "build center %f %f %f ", vCenter.x, vCenter.y, vCenter.z);
	OUTPUT_DEBUG_INFO(msg);
#endif
#if BMAN_VERBOSE_STAT
	DWORD tStart;
	tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	
#endif	

	int nBrush = m_OutOfRangeBrush.size(); //TRICK! accelerate the building process.

	bool bCenterChange = _UpdateCenter(vCenter, bForce);
#ifdef BMAN_DEBUG
	sprintf(msg, " update center ");
	OUTPUT_DEBUG_INFO(msg);
#endif
	int i, j;
	CBrushProvider  * pProvider;
	CConvexBrush * pBrush;
	for (i = 0; i < (int)m_UnOrganizedProvider.size(); ++i )
	{
		pProvider = m_UnOrganizedProvider[i];
		for (j = 0; j < pProvider->GetBrushCount(); ++j)
		{
			pBrush = pProvider->GetBrush(j);
			A3DAABB aabb = pBrush->GetTransformedAABB(pProvider->GetTransMat());
			if (!_AddBrush(pBrush, pProvider, aabb))
			{//add to out of range list
				CBrushCell::ConvexBrushData data;
				data.pBrush = pBrush;
				data.pProvider = pProvider;
			   m_OutOfRangeBrush.push_back(data);
			}			
		}
	}
	m_UnOrganizedProvider.clear();
#ifdef BMAN_DEBUG
	sprintf(msg, " unorg provider ");
	OUTPUT_DEBUG_INFO(msg);
#endif
	if (bCenterChange)
	{
		vector<CBrushCell::ConvexBrushData > testRange(m_OutOfRangeBrush);
		m_OutOfRangeBrush.clear();
		for (i = 0; i < nBrush; ++i)
		{
			
			if (!_AddBrush(testRange[i].pBrush, testRange[i].pProvider, testRange[i].aabb))
			{//add to out of range list
				m_OutOfRangeBrush.push_back(testRange[i]);
			}
		}
		for (i = nBrush; i < (int)testRange.size(); ++i)
		{
			m_OutOfRangeBrush.push_back(testRange[i]);
		}
	}
#if BMAN_VERBOSE_STAT
	m_Stat.dwBuildTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#ifdef BMAN_USE_GRID
	m_Stat.nOutOfRangeBrush = m_OutOfRangeBrush.size();
#endif
#endif	
#ifdef BMAN_DEBUG
	sprintf(msg, " build ok\n");
	OUTPUT_DEBUG_INFO(msg);
#endif
}

bool CBrushGrid::_AddBrush(CConvexBrush* pBrush, CBrushProvider* pProvider, const A3DAABB& aabb)
{
	short x, z;

	_GetCellIndex(aabb.Center, x, z);
	CBrushCell * pCell = _FindCell(x, z);
	
	if (!pCell)
	{
		return false;
	}
	CBrushCell::ConvexBrushData data;
	data.aabb = aabb;
	data.pBrush = pBrush;
	data.pProvider = pProvider;
	pCell->m_pBrushDatas.push_back(data);

	pCell->m_AABB.Merge(aabb);
	return true;
}

CBrushCell * CBrushGrid::_FindCell(short x, short z)
{
	DWORD   dwKey = MAKE_CELL_KEY(x, z);
	CellTable::pair_type  pair;	
	pair = m_CellTbl.get(dwKey);
	if (pair.second)
	{
		return *pair.first;
	}
	
	return NULL;
}

bool CBrushGrid::Trace(AbstractBrushTraceInfo * pInfo, bool bCheckFlag /* = true */)
{
	
	
#ifdef BMAN_DEBUG
	char msg[200];
#endif

#ifdef BMAN_AUTO_BUILD
	//build first
	Build(pInfo->vStart);
#endif

	//@todo : skipping some cells may be faster.  By Kuiwu[2/11/2005]
	bool bCollide = false;
#if BMAN_VERBOSE_STAT
	DWORD tStart;
	tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	m_Stat.pHitBrush = NULL;
	m_Stat.nTraceBrush = 0;
#endif
	//save original result
	bool		bStartSolid ;	//	Collide something at start point
	bool		bAllSolid ;		//	All in something
	A3DVECTOR3 vNormal; //clip plane normal
	float       fDist ;	//clip plane dist
	A3DMATRIX4  mat;
	
	float fFraction = 100.0f ;


	bool bIsRay = false;
	if (RayBrushTraceInfo::IsInstance(pInfo))
		bIsRay = true;
	//check the cells
	int i, j;
	CBrushCell * pCell;
	CConvexBrush * pBrush;
	A3DVECTOR3 vDummyPoint, vDummyNormal;
	float fDummyFraction;
	for (i = 0; i < (int)m_CellTbl.size(); ++i)
	{
		pCell = &m_pBrushCell[i];
		if (pCell->m_pBrushDatas.empty())
		{
			continue;
		}
		
		if (bIsRay 
			&& !CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, pCell->m_AABB.Mins, pCell->m_AABB.Maxs, vDummyPoint, &fDummyFraction, vDummyNormal ) )
		{
			continue;
		}
		if (!bIsRay 
			&& !CLS_AABBAABBOverlap(pCell->m_AABB.Center, pCell->m_AABB.Extents, pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents) ) 
		{
			continue;
		}

		CBrushProvider* pLastProvider = NULL;
		A3DVECTOR3 up(0, 1, 0);
		if (CapusuleBrushTraceInfo::IsInstance(pInfo))
			up = ((CapusuleBrushTraceInfo*)pInfo)->vUp;

		for (j = 0; j < (int)pCell->m_pBrushDatas.size(); ++j)
		{
			pBrush = pCell->m_pBrushDatas[j].pBrush;
			CBrushProvider* pProvider = pCell->m_pBrushDatas[j].pProvider;
			if (pProvider != pLastProvider)
			{				
				pInfo->SetTransMat(pProvider->GetTransMat(), up);
				pLastProvider = pProvider;
			}
			
#if BMAN_VERBOSE_STAT 
			m_Stat.nTraceBrush++;
#endif
			if (pBrush->BrushTrace(pInfo, bCheckFlag) && (pInfo->fFraction < fFraction)) 
			{
				//update the saving info
				bStartSolid = pInfo->bStartSolid;
				bAllSolid = pInfo->bAllSolid;
				fFraction = pInfo->fFraction;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();
				mat  = pInfo->mat;
				bCollide = true;
#if BMAN_VERBOSE_STAT
				m_Stat.pHitBrush = pBrush;
#endif				
				if (pInfo->fFraction == 0.0f)
				{
					goto LOCAL_EXIT;
				}
			}
		}
	}
//@note : TRICK! skipping out-of-range brush will accelerate the tracing process greatly.
//		  but maybe give inaccurate tracing result(it rarely happens and can be ignored). 		
//		  By Kuiwu[3/11/2005]
#ifdef BMAN_TRACE_OUTOFRANGE
	//check the out of range brush
	for (i = 0; i < m_OutOfRangeBrush.size(); ++i)
	{
		pBrush = m_OutOfRangeBrush[i].pBrush;
#if BMAN_VERBOSE_STAT 
		m_Stat.nTraceBrush++;
#endif
		if (pBrush->BrushTrace(pInfo, bCheckFlag) && (pInfo->fFraction < fFraction)) 
		{
			//update the saving info
			bStartSolid = pInfo->bStartSolid;
			bAllSolid = pInfo->bAllSolid;
			fFraction = pInfo->fFraction;
			vNormal = pInfo->ClipPlane.GetNormal();
			fDist = pInfo->ClipPlane.GetDist();
			mat = pInfo->mat;
			bCollide = true;
#if BMAN_VERBOSE_STAT
			m_Stat.pHitBrush = pBrush;
#endif				
			if (pInfo->fFraction == 0.0f)
			{
				goto LOCAL_EXIT;
			}
		}
	}
#endif

LOCAL_EXIT:
#if BMAN_VERBOSE_STAT
	m_Stat.dwTraceTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif
	if (bCollide)
	{
		//set back
		pInfo->bStartSolid = bStartSolid;
		pInfo->bAllSolid = bAllSolid;
		pInfo->fFraction = fFraction;
		mat._41 = mat._42 = mat._43 = 0;
		pInfo->ClipPlane.SetNormal(vNormal * mat);
		pInfo->ClipPlane.SetD(fDist);
		pInfo->mat = mat;
	}
	return bCollide;
}