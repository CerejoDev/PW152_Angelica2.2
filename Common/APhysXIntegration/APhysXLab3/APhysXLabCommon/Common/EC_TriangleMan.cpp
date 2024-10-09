/********************************************************************
  created:	   8/11/2005  
  filename:	   EC_TriangleMan.cpp
  author:      Wangkuiwu  
  description: Triangle manager, mainly used by shadow generator.
  Copyright (c) 2005 Archosaur Studio , All Rights Reserved.
*********************************************************************/

// turn off the warning in hashtab.h : check operator¨C>( ) return.  By kuiwu. [15/3/2006]
#include "stdafx.h"
#pragma  warning(disable : 4284)

#ifndef _ANGELICA3

#include "APhysX.h"

#include "A3DDevice.h"
#include "A3DCollision.h"
#include "A3dframe.h"
#include "aassist.h"
#include "A3dlitmodel.h"
#include "A3dcamera.h"
#include "EC_TriangleProvider.h"
#include "EC_TriangleMan.h"
//#include "EL_BrushBuilding.h"
#include "CDWithCH.h"

#include <A3DTexture.h>

#ifdef TMAN_VERBOSE_STAT
#include "acounter.h"
#endif

//#define  TMAN_DEBUG

#ifdef TMAN_DEBUG
char msg[200];
#endif
//low =x, high = z

inline DWORD MAKE_CELL_KEY(int x, int z)    
{
	return DWORD((x + 512) * 1024 + z + 512);
}

#define MAKE_SUB_SPACE_KEY(x, y, z) (((z) << 20) + ((y) << 10) + (x))

#define TMAN_TEST_RANGE       10.0f
#define TMAN_BUILD_LIMIT      4000

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
/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
template<class InputIterator> inline
     InputIterator reorder(vector<int>& leftIndex, InputIterator first, InputIterator last)
{
	if (leftIndex.size() == (unsigned int)(last -first))
	{
		return last;
	}
	unsigned int i; 
	InputIterator it = first;
	for(i = 0; i <leftIndex.size(); ++i, ++it)
	{
		if (i != (unsigned int)leftIndex[i])
		{
			*it = *(first+leftIndex[i]); 
			
		}
	}

	return it;
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
inline static bool PlaneAABBOverlap(const A3DVECTOR3& normal, const float d, const A3DVECTOR3& maxbox)
{
	A3DVECTOR3 vMin, vMax;
	for (int i = 0; i <=2; ++i)
	{
		if (normal.m[i] > 0.0f)
		{
			vMin.m[i] = -maxbox.m[i];
			vMax.m[i] = maxbox.m[i];
		}
		else
		{
			vMax.m[i] = -maxbox.m[i];
			vMin.m[i] = maxbox.m[i];
		}
	}

	if (DotProduct(normal, vMin) +d > 0.0f)
	{
		return false;
	}
	if (DotProduct(normal, vMax) +d >= 0.0f)
	{
		return true;
	}
	return false;
}

#define AXISTEST_X01(a, b, fa, fb, ext)							\
	fmin = (a)*v0.y - (b)*v0.z;									\
	fmax = (a)*v2.y - (b)*v2.z;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.y + (fb) * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;


#define AXISTEST_X2(a, b, fa, fb, ext)							\
	fmin = (a)*v0.y - (b)*v0.z;									\
	fmax = (a)*v1.y - (b)*v1.z;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.y + (fb) * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;

#define AXISTEST_Y02(a, b, fa, fb, ext)							\
	fmin = (b)*v0.z - (a)*v0.x;									\
	fmax = (b)*v2.z - (a)*v2.x;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + fb * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;

#define AXISTEST_Y1(a, b, fa, fb, ext)							\
	fmin = (b)*v0.z - (a)*v0.x;									\
	fmax = (b)*v1.z - (a)*v1.x;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + (fb) * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;

#define AXISTEST_Z12(a, b, fa, fb, ext)							\
	fmin = (a)*v1.x - (b)*v1.y;									\
	fmax = (a)*v2.x - (b)*v2.y;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + (fb) * ext.y;					\
	if(fmin>rad || fmax<-rad) return false;


#define AXISTEST_Z0(a, b, fa, fb, ext)							\
	fmin = (a)*v0.x - (b)*v0.y;									\
	fmax = (a)*v1.x - (b)*v1.y;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + (fb) * ext.y;					\
	if(fmin>rad || fmax<-rad) return false;


/*
 * AABB triangle overlap test without translation. assume aabb center is 0.
 * @desc : This is the code from Tomas Moller, 
 *		   a bit optimized with some more lazy evaluation (faster path on PC).
 * @param :     
 * @return :
 * @note: The A3dsdk implementation is not tested thoroughly, so do not use it.
 * @todo: Handle the intersections for triangles completely in a box plane.  
 * @author: kuiwu [18/11/2005]
 * @ref: Tomas Moller's original code
 */
static bool AABBTriangleOverlapNT(const A3DVECTOR3& vExt, const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& normal)
{

	// use separating axis theorem to test overlap between triangle and aabb 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	float fmin, fmax;
	// Find min, max of the triangle in x-direction, and test for overlap in X
	fmin = min3(v0.x, v1.x, v2.x);
	fmax = max3(v0.x, v1.x, v2.x);
	if (fmin > vExt.x || fmax < -vExt.x)
	{
		return false;
	}
	// Same for Y
	fmin = min3(v0.y, v1.y, v2.y);
	fmax = max3(v0.y, v1.y, v2.y);
	if (fmin > vExt.y || fmax < -vExt.y)
	{
		return false;
	}
	// Same for Z
	fmin = min3(v0.z, v1.z, v2.z);
	fmax = max3(v0.z, v1.z, v2.z);
	if (fmin > vExt.z || fmax < -vExt.z) 
	{
		return false;
	}

	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	float d = - DotProduct(normal, v0);
	if (!PlaneAABBOverlap(normal, d, vExt))
	{
		return false;
	}

	// 3) "Class III" tests
	float rad;

	const A3DVECTOR3 e0(v1 - v0);
	const  float fey0 = fabsf(e0.y);
	const  float fez0 = fabsf(e0.z);
	AXISTEST_X01(e0.z, e0.y, fez0, fey0, vExt);
	const  float fex0 = fabsf(e0.x);
	AXISTEST_Y02(e0.z, e0.x, fez0, fex0, vExt);
	AXISTEST_Z12(e0.y, e0.x, fey0, fex0, vExt);	

	const	A3DVECTOR3 e1(v2 - v1);
	const	float fey1  = fabsf(e1.y);
	const	float fez1  = fabsf(e1.z);
	AXISTEST_X01(e1.z, e1.y, fez1, fey1, vExt);
	const float fex1 = fabsf(e1.x);					
	AXISTEST_Y02(e1.z, e1.x, fez1, fex1, vExt);
	AXISTEST_Z0(e1.y, e1.x, fey1, fex1, vExt);
	const A3DVECTOR3	e2(v0 - v2);	
	const float fey2 = fabsf(e2.y);					
	const float fez2 = fabsf(e2.z);					
	AXISTEST_X2(e2.z, e2.y, fez2, fey2, vExt);	
	const float fex2 = fabsf(e2.x);					
	AXISTEST_Y1(e2.z, e2.x, fez2, fex2, vExt);			
	AXISTEST_Z12(e2.y, e2.x, fey2, fex2, vExt);

	
	return  true;


}


/*
 * Triangle-aabb overlap test using the separating axis theorem(SAT).
 * @desc : This is the code from Tomas Moller, 
 *		   a bit optimized with some more lazy evaluation (faster path on PC).
 * @param :     
 * @return :
 * @note: The A3dsdk implementation is not tested thoroughly, so do not use it.
 * @todo:  
 * @author: kuiwu [10/11/2005]
 * @ref: Tomas Moller's original code
 */
static  bool AABBTriangleOverlap(const A3DAABB& aabb, const A3DVECTOR3& v0, 
								 const A3DVECTOR3& v1, const A3DVECTOR3& v2,
								 const A3DVECTOR3& normal)
{

	// move everything so that the aabb center is in (0,0,0) 
	A3DVECTOR3 vert0(v0 - aabb.Center), 
			   vert1(v1 - aabb.Center), 
			   vert2(v2 - aabb.Center);

	return AABBTriangleOverlapNT(aabb.Extents, vert0, vert1, vert2, normal);

	
}

/*
 *
 * @desc : Currently transform the triangle to obb's local space,
 *         then do aabb-triangle overlap test.
 * @param :     
 * @return :
 * @note:  
 * @todo:   Implement a highly optimized explicit version.
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
static bool OBBTriangleOverlap(const A3DOBB& obb, const A3DVECTOR3& v0, 
							   const A3DVECTOR3& v1, const A3DVECTOR3& v2,
							   const A3DVECTOR3& normal)
{
	
	//from  world space to obb local space
	A3DMATRIX4 mat;
	
	// TR ,  (TR)^(-1) = R^(-1) T(-t) = Transpose(R) T(-t)
	mat._11 = obb.XAxis.x;
	mat._21 = obb.XAxis.y;
	mat._31 = obb.XAxis.z;
	mat._12 = obb.YAxis.x;
	mat._22 = obb.YAxis.y;
	mat._32 = obb.YAxis.z;
	mat._13 = obb.ZAxis.x;
	mat._23 = obb.ZAxis.y;
	mat._33 = obb.ZAxis.z;

//	mat._41 = -DotProduct(obb.Center, obb.XAxis);
//	mat._42 = -DotProduct(obb.Center, obb.YAxis);
//	mat._43 = -DotProduct(obb.Center, obb.ZAxis);
	mat._41  = mat._42 = mat._43 = 0.0f;
	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;

	A3DAABB  aabb;
	//aabb.Center = A3DVECTOR3(0);
	aabb.Center   = obb.Center* mat;
	aabb.Extents = obb.Extents;
	aabb.CompleteMinsMaxs();

	//transform the vert
	A3DVECTOR3   vert0(v0*mat), vert1(v1*mat), vert2(v2*mat);
	//transform the normal,  mat is a rigid transform matrix, 
	//it is safe to use mat to transform normal directly.
	A3DVECTOR3  vN(normal*mat);

	return AABBTriangleOverlap(aabb, vert0, vert1, vert2, vN);
	
}
/*
 * Use aabb to approximate triangle
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
static bool OBBTriangleOverlapApprox(const A3DOBB& obb, const A3DVECTOR3& v0, 
							   const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	A3DAABB  aabb;
	aabb.Clear();
	aabb.AddVertex(v0);
	aabb.AddVertex(v1);
	aabb.AddVertex(v2);
	aabb.CompleteCenterExts();
	return CLS_AABBOBBOverlap(aabb, obb);
}
/*
 *
 * @desc : currently use aabb to approximate triangle.
 * @param :     
 * @return :
 * @note:
 * @todo:   Implement an exact version.
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
static  bool ViewTriangleOverlap( A3DCameraBase * pCamera,  const A3DVECTOR3& v0, 
							   const A3DVECTOR3& v1, const A3DVECTOR3& v2,
							   const A3DVECTOR3& normal)
{
#if 0
	
#else
	//treat triangle as aabb
	A3DAABB  aabb;
	aabb.Clear();
	aabb.AddVertex(v0);
	aabb.AddVertex(v1);
	aabb.AddVertex(v2);
	aabb.CompleteCenterExts();
	
	return pCamera->AABBInViewFrustum(aabb);
	
#endif
	
}

#define TMAN_IN     -1    
#define TMAN_OUT	 1	
#define TMAN_PART	 0

/*
 * Yet another aabb triangle overlap test method.
 * @desc : adjust  if the given triangle  can be trivially accepted or rejected, or need to
 *		   be clipped.
 * @param :     
 * @return : -1: completely in the AABB  
 *            1: completely out the AABB
 *            0: partly in&out, need clip * @note:
 * @todo:   
 * @author: kuiwu [18/11/2005]
 * @ref:
 */
static int AABBTriangleFastExclude(const A3DVECTOR3& vExt, const A3DVECTOR3 vert[3])
{

  // vertex bit codes for 6 least sign bit ,  l,r,b,t,n,f
  // set bit means out of corresponding frustum plane
  unsigned int Code[3]={0,0,0};  
  for (int i=0; i<3; i++)        
  {
	Code[i]	|= ((vert[i].x < -vExt.x)<<5);
	Code[i]	|= ((vert[i].x >  vExt.x)<<4);	
	Code[i]	|= ((vert[i].y < -vExt.y)<<3);
	Code[i]	|= ((vert[i].y >  vExt.y)<<2);
	Code[i]	|= ((vert[i].z < -vExt.z)<<1);
	Code[i]	|= ((vert[i].z >  vExt.z));
  }
  
  // each vertex is outside of at least one plane
  if (Code[0] & Code[1] & Code[2]) return(TMAN_OUT);     
  // all vertices are inside of all planes

  if (!(Code[0] | Code[1] | Code[2])) return(TMAN_IN); 
  //partly
  return(TMAN_PART);

}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [19/11/2005]
 * @ref:
 */
static int PlaneClipTriangle(const A3DVECTOR3& ext, const A3DVECTOR3 *inVert, int inNum, A3DVECTOR3 *outVert, int planeflag )
{
   const A3DVECTOR3 *A, *B;  //edge AB
   int Aout, Bout;     //edge endpoint outside condition
	
   int i;
   int iOut = 0;
   for (i = 0; i < inNum; i++) 
   {
	  A = inVert + i;
	  B = inVert + (i+1)%inNum;

     switch(planeflag)    //set endpoint outside condition 
	 {
		 case 0: Aout=(A->x < -ext.x); Bout=(B->x <-ext.x); break;  // LEFT
		 case 1: Aout=(A->x > ext.x);  Bout=(B->x > ext.x); break;  // RIGHT
		 case 2: Aout=(A->y <-ext.y);  Bout=(B->y <-ext.y); break;  // BOTTOM
 		 case 3: Aout=(A->y > ext.y);  Bout=(B->y > ext.y);  break;  // TOP
		 case 4: Aout=(A->z < -ext.z); Bout=(B->z < -ext.z); break;  // NEAR
		 case 5: Aout=(A->z > ext.z);  Bout=(B->z > ext.z ); break;  // FAR		 
		 default: assert(0);
	 };
	
	
	if (Aout != Bout)    //partly in(out), intersection occurs
    { 
	  float t;   //edge parametric value, used to intersect interpolation
      switch(planeflag)
      {
		case 0: t=(-ext.x - A->x)/(B->x - A->x ); break;  // LEFT
		case 1: t=(ext.x - A->x) /(B->x - A->x ); break;  // RIGHT
		case 2: t=(-ext.y - A->y)/(B->y - A->y ); break;  // BOTTOM
        case 3: t=(ext.y - A->y) /(B->y - A->y );  break;  // TOP
        case 4: t=(-ext.z -A->z)/(B->z -A->z ); break;  // NEAR
		case 5: t=(ext.z - A->z) /(B->z -A->z );  break;  // FAR
      };
      // interpolate geometry
	  outVert[iOut] = (*A) + ((*B) - (*A))*t;
      iOut++;
    }

	if ( (Aout && !Bout) || (!Aout && !Bout) )   // output 2nd edge vertex
    { 
	  outVert[iOut] = (*B);	
      iOut++;
    }
  }
	
	return iOut;

	
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note: without +z (far) plane.
 * @todo:   
 * @author: kuiwu [19/11/2005]
 * @ref:
 */
static int AABBClipTriangle(const A3DVECTOR3& vExt, const A3DVECTOR3 invert[3], A3DVECTOR3 outvert[9])
{

  A3DVECTOR3 tmpInVert[9];	

  
  int i, NumOutVert;

  for (i = 0; i < 3; i++) 
  {
	  tmpInVert[i] = invert[i];
  }
  NumOutVert = PlaneClipTriangle(vExt, tmpInVert, 3,          outvert, 0);			//left
  NumOutVert = PlaneClipTriangle(vExt, outvert,   NumOutVert, tmpInVert, 1);		//right
  NumOutVert = PlaneClipTriangle(vExt, tmpInVert, NumOutVert, outvert, 2);			//bottom
  NumOutVert = PlaneClipTriangle(vExt, outvert,   NumOutVert, tmpInVert, 3);		//top
  NumOutVert = PlaneClipTriangle(vExt, tmpInVert, NumOutVert, outvert, 4);          //near	
//  NumOutVert = PlaneClipTriangle(vExt, outvert,   NumOutVert, tmpInVert, 5);        //far	
//  
//  for (i = 0; i < NumOutVert; ++i)
//  {
//	  outvert[i] = tmpInVert[i];
//  }
//  
  return(NumOutVert);	
}

extern void TriangleManLoadProgress(float fRatio);

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [19/11/2005]
 * @ref:
 */
void CTriangleGrid::_BuildOBB(const TMan_Overlap_Info& tInfo,  A3DOBB& obb)
{
	bool square = false;
	obb.Center = tInfo.vStart + tInfo.vDelta *0.5f;

	float fMax = max2(tInfo.vExtent.x, tInfo.vExtent.y);

	obb.ZAxis  = tInfo.vDelta;
	obb.Extents.z = obb.ZAxis.Normalize();
	obb.Extents.z *= 0.5f;

	if(tInfo.bUsedirs)
	{
		obb.XAxis = tInfo.vRight;
		obb.YAxis = tInfo.vBottom;

		obb.Extents.x = tInfo.vExtent.x;
		obb.Extents.y = tInfo.vExtent.y;
	}
	else
	{
		if (fabs(obb.ZAxis.y)> 0.7f ) 
		{
			obb.XAxis  = CrossProduct(A3DVECTOR3(1,0,0), obb.ZAxis);
		}
		else
		{
			obb.XAxis  = CrossProduct(A3DVECTOR3(0,1,0), obb.ZAxis);
		}
		obb.XAxis.Normalize();

		obb.YAxis = CrossProduct(obb.XAxis, obb.ZAxis);
		obb.YAxis.Normalize();

		obb.Extents.x = obb.Extents.y = fMax;
	}

	
	

	obb.CompleteExtAxis();

}

triman_mem_man* _cur_triman_mem_man = 0;
CRITICAL_SECTION _cs_triman;
int _cs_trimanref = 0;

void* triman_alloc::allocate(size_t size)
{
	assert(_cur_triman_mem_man);
	return _cur_triman_mem_man->allocate(size);
}

CECTriangleMan::CECTriangleMan()
{
	if (++_cs_trimanref == 1)
	{
		::InitializeCriticalSection(&_cs_triman);
	}

	//m_pGrid = new CTriangleGrid(20, 17, 17);
	//m_pGrid = new CTriangleGrid(4, 256, 256);
	::EnterCriticalSection(&_cs_triman);
	_cur_triman_mem_man = &m_MemoryMan;
	m_pGrid = new CTriangleGrid(this, 1, 1024, 1024);
	_cur_triman_mem_man = 0;
	::LeaveCriticalSection(&_cs_triman);

	m_dwThreadId = ::GetCurrentThreadId();

	m_pLoadProgressCallback = NULL;
	m_pLoadPrgCBParam = NULL;
	m_bReadyToUse = false;
}

CECTriangleMan::~CECTriangleMan()
{
	delete m_pGrid;
	m_MemoryMan.clear();

	if (--_cs_trimanref == 0)
		DeleteCriticalSection(&_cs_triman);
}

void CECTriangleMan::Clear()
{
	::EnterCriticalSection(&_cs_triman);
	m_pGrid->Clear();
	::LeaveCriticalSection(&_cs_triman);
}

void CECTriangleMan::AddProvider(TriangleProvider* pProvider)
{
	::EnterCriticalSection(&_cs_triman);
	m_pGrid->AddProvider(pProvider);
	::LeaveCriticalSection(&_cs_triman);
}
void CECTriangleMan::RemoveProvider(TriangleProvider* pProvider)
{
	::EnterCriticalSection(&_cs_triman);
	m_pGrid->RemoveProvider(pProvider);
	::LeaveCriticalSection(&_cs_triman);
}

void CECTriangleMan::Build(const A3DVECTOR3& vCenter, DWORD dwFlag /* = TMAN_DEFAULT_BUILD */)
{
	::EnterCriticalSection(&_cs_triman);
	_cur_triman_mem_man = &m_MemoryMan;
	m_pGrid->BuildGradually(vCenter, dwFlag);
	_cur_triman_mem_man = 0;
	::LeaveCriticalSection(&_cs_triman);
}

void CECTriangleMan::BuildFromTerrain(const float* pTerrainHeight, int nWidth, int nHeight)
{
	::EnterCriticalSection(&_cs_triman);
	_cur_triman_mem_man = &m_MemoryMan;
	m_pGrid->BuildFromTerrain(pTerrainHeight, nWidth, nHeight);
	_cur_triman_mem_man = 0;
	::LeaveCriticalSection(&_cs_triman);
}

void CECTriangleMan::TestOverlap(TMan_Overlap_Info& tInfo)
{
	if (!IsReadyToUse())
		return;

	//m_pGrid->TestOverlap(tInfo);
	m_pGrid->TestOverlapNF(tInfo);
}

bool CECTriangleMan::VertRayTrace(TMRayTraceInfo& rtInfo)
{
	if (!IsReadyToUse())
		return false;

	return m_pGrid->VertRayTrace(rtInfo);
}

bool CECTriangleMan::CapsuleTrace( TMCapsuleTraceInfo& trInfo )
{
	if (!IsReadyToUse())
		return false;

	return m_pGrid->CapsuleTrace(trInfo);
}

void CECTriangleMan::GetCell(TMan_Overlap_Info& tInfo)
{
	if (m_dwThreadId != ::GetCurrentThreadId())
		return;

	if (!IsReadyToUse())
		return;

	m_pGrid->GetCell(tInfo);
}

static const float _ray_range = 15.f;
static const float _ray_multi = 3.f;
static const int cell_max_count = 1024 * 1024;
static CTriangleCell* cell_buffer[cell_max_count];
static bool celll_buffer_in_use = false;

//---------------------------------------------------------------

CTriangleGrid::CTriangleGrid(CECTriangleMan* pTriMan, int iCellSize /* = 10 */, int iW /* =11 */, int iH /* = 11 */)
	:	m_pTriMan(pTriMan)
	,	m_iCellSize(iCellSize)
	,	m_SmallBufMemMan(512 * 1024)
	,	m_iW(iW)
	,	m_iH(iH)
	,	m_btTestStamp(0)
	,	m_fOffset(0/*0.02f*/)
{
	if (m_iW % 2 == 0)
	{
		--m_iW;
	}
	if (m_iH % 2 == 0)
	{
		--m_iH;
	}
	m_nCellCount = m_iW * m_iH * 2;
	m_UnOrganizedProvider.clear();
	m_iCenterX = -30000;
	m_iCenterZ = -30000;

	if (!celll_buffer_in_use)
	{
		m_CellTbl = cell_buffer;
		memset(m_CellTbl, 0, sizeof(CTriangleCell*) * cell_max_count);
		celll_buffer_in_use = true;
		m_bEnable = true;
	}
	else
		m_bEnable = false;

	m_pTrianglePool  = new CTManTrianglePool();

#ifdef TMAN_VERBOSE_STAT
	m_Stat.nCellSize = m_iCellSize;
	m_Stat.nWidth = m_iW;
	m_Stat.nHeight = m_iH;
	m_Stat.pCellTbl = &m_CellTbl;
#endif

	m_bBreakLoadFlag = false;
}

CTriangleGrid::~CTriangleGrid()
{
	Clear();
	delete m_pTrianglePool;

	if (m_bEnable)
		celll_buffer_in_use = false;
}

void CTriangleGrid::Clear()
{
	if (!m_bEnable)
		return;

	m_UnOrganizedProvider.clear();
	_ReleaseCellTbl();
	m_btTestStamp = 0;
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note: the caller should assure NOT to add duplicate providers.
 * @todo:   
 * @author: kuiwu [8/11/2005]
 * @ref:
 */
void CTriangleGrid::AddProvider(TriangleProvider* pProvider)
{
	if (!m_bEnable)
		return;

	if (pProvider->IsAddedToMan())
		return;

	pProvider->SetAddedToMan(true);
	m_UnOrganizedProvider.push_back(pProvider);

	//a_LogOutput(1, "@%d: TriGrid 0x%X AddProvider 0x%x, Type %d", a_GetTime(), this, pProvider, pProvider->GetType());
}
void CTriangleGrid::RemoveProvider(TriangleProvider* pProvider)
{
	if (!m_bEnable)
		return;

	if (!pProvider->IsAddedToMan())
		return;

	vector<TriangleProvider *>::iterator it = linear_find(m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.end(), pProvider);
	if (it != m_UnOrganizedProvider.end())
	{
		pProvider->SetAddedToMan(false);
		m_UnOrganizedProvider.erase(it);

		//a_LogOutput(1, "@%d: TriGrid 0x%X RemoveProvider From UnOrganized: 0x%x, Type %d", a_GetTime(), this, pProvider, pProvider->GetType());
		return;
	}

	bool bFound = false;
	for (int i = 0; i < m_pTrianglePool->GetTriangleNum(); i++)
	{
		TManTriangle* pTri = m_pTrianglePool->GetTriangle(i);
		if (pTri->pProvider == pProvider)
		{
			bFound = true;
			pTri->pProvider = NULL;

			//a_LogOutput(1, "@%d: TriGrid 0x%X RemoveProvider From TrianglePool[%d-th] Succ: 0x%x, Type %d",
			//	a_GetTime(), this, i, pProvider, pProvider->GetType());

			//	TOO slow... ignore it
			//CTriangleCell * pCell;
			//for (TManCellTable::iterator it1 = m_CellTbl.begin(); it1 != m_CellTbl.end(); ++it1)
			//{
			//	pCell = *(it1.value());
			//	vector<int>::iterator it2 = linear_find(pCell->m_Triangles.begin(), pCell->m_Triangles.end(), i);
			//	if (it2 != pCell->m_Triangles.end())
			//		pCell->m_Triangles.erase(it2);
			//}
		}
	}

	pProvider->SetAddedToMan(false);
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [22/11/2005]
 * @ref:
 */
int CTriangleGrid::_AddProvider(TriangleProvider * pProvider, bool bPreCompute)
{
	int nAdd = 0;

	for (int j = 0; j < pProvider->GetSubMeshNum(); ++j)
	{		
		//-------------------------------------------------------------
		// tmp commented by wenfeng, Jun. 7, 2012.
		//		Disable alpha texture test here to make sure each triangle
		// are handled...
		//
		// bool bAlphaTex = (pTex ? pTex->IsAlphaTexture() : false);
		bool bAlphaTex = false;


		WORD* pIndices = pProvider->GetIndices(j);

		A3DVECTOR3 v0, v1, v2;
		int nFace = pProvider->GetFaceNum(j);
		for (int k = 0; k < nFace; ++k)
		{
			//@note : push out the vert to avoid z fighting. By Kuiwu[18/11/2005]
			pProvider->GetVert(j, pIndices[k*3], v0.x, v0.y, v0.z);
			pProvider->GetVert(j, pIndices[k*3 + 1], v1.x, v1.y, v1.z);
			pProvider->GetVert(j, pIndices[k*3 + 2], v2.x, v2.y, v2.z);

			A3DVECTOR3 e01(v1 - v0);
			A3DVECTOR3 e02(v2 - v0);
			A3DVECTOR3 vNormal = CrossProduct(e01, e02);
			float s = vNormal.Normalize() * .5f;

			if (!bAlphaTex && s > 0.f)
			{
				int index = m_pTrianglePool->CreateTriangle();
				TManTriangle* pTri  = m_pTrianglePool->GetTriangle(index);
				assert(pTri != NULL);
				pTri->iSubMesh = j;
				pTri->pProvider = pProvider;

				pTri->index[0] = pIndices[k*3];
				pTri->index[1] = pIndices[k*3+1];
				pTri->index[2] = pIndices[k*3+2];
				_AddTriangle(pTri, index, v0, v1, v2, vNormal, bPreCompute);
			}
		}

		nAdd += pProvider->GetFaceNum(j);
	}

	return (nAdd);
}

void CTriangleGrid::Build(const A3DVECTOR3& vCenter, DWORD dwFlag)
{
	if (!m_bEnable)
		return;

#ifdef TMAN_VERBOSE_STAT
	DWORD tStart;
	tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
#endif	


	bool bCenterChange = _UpdateCenter(vCenter, dwFlag);
	if (bCenterChange)
	{
		vector<int> leftIndex;

		A3DAABB  gridAABB;
		gridAABB.Center = vCenter;
		gridAABB.Extents.x = m_iCellSize *  (float)m_iW * 0.5f;
		gridAABB.Extents.y = 9999.0f;
		gridAABB.Extents.z = m_iCellSize *  (float)m_iH * 0.5f;
		gridAABB.CompleteMinsMaxs();
		TriangleProvider* pProvider;
		leftIndex.clear();
		A3DAABB prvAABB;
		int nAdd = 0;
		for (int i = 0; i < (int)m_UnOrganizedProvider.size(); ++i )
		{
			pProvider = m_UnOrganizedProvider[i];
			prvAABB = pProvider->GetModelAABB();
			if (!CLS_AABBAABBOverlap(gridAABB.Center, gridAABB.Extents, prvAABB.Center, prvAABB.Extents))
			{
				leftIndex.push_back(i);
				continue;
			}
			//try to add the provider
			nAdd += (_AddProvider(pProvider, true));
		}

		for (int i = 0; i < (int)m_UnOrganizedProvider.size(); ++i )
		{
			pProvider = m_UnOrganizedProvider[i];
			prvAABB = pProvider->GetModelAABB();
			if (!CLS_AABBAABBOverlap(gridAABB.Center, gridAABB.Extents, prvAABB.Center, prvAABB.Extents))
			{
				continue;
			}
			//try to add the provider
			_AddProvider(pProvider, false);
		}

		TriangleProvider ** ppPrv = reorder(leftIndex, m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.end());
		m_UnOrganizedProvider.erase(ppPrv, m_UnOrganizedProvider.end());
	#ifdef TMAN_VERBOSE_STAT
		m_Stat.nUnorgPrv = m_UnOrganizedProvider.size();
		m_Stat.nTriangle += nAdd; 
	#endif

	}
#ifdef TMAN_VERBOSE_STAT
	m_Stat.dwBuildTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif

}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [22/11/2005]
 * @ref:
 */
void CTriangleGrid::BuildGradually(const A3DVECTOR3& vCenter, DWORD dwFlag /* = TMAN_BUILD_DEFAULT */)
{
	if (!m_bEnable)
		return;

#ifdef TMAN_VERBOSE_STAT
	DWORD tStart;
	tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	m_Stat.nAddTriangle = 0;
#endif	

	bool bCenterChange = _UpdateCenter(vCenter, dwFlag);
	//@note : try to build even if center doesn't change. By Kuiwu[22/11/2005]
	vector<int> leftIndex;
	A3DAABB  gridAABB;
	gridAABB.Center = vCenter;
	gridAABB.Extents.x = m_iCellSize *  (float)m_iW * 0.5f;
	gridAABB.Extents.y = 9999.0f;
	gridAABB.Extents.z = m_iCellSize *  (float)m_iH * 0.5f;
	gridAABB.CompleteMinsMaxs();
	TriangleProvider* pProvider;
	leftIndex.clear();
	A3DAABB prvAABB;
	int nAdd = 0;

	//a_LogOutput(1, "@%d: TriGrid 0x%X BuildGradually: UnOrganized Size %d", a_GetTime(), this, m_UnOrganizedProvider.size());

	for (size_t i = 0; i < m_UnOrganizedProvider.size(); ++i)
	{
		if (m_bBreakLoadFlag)
			break;

		pProvider = m_UnOrganizedProvider[i];
		//a_LogOutput(1, "@%d: TriGrid 0x%X BuildGradually: %d-th, 0x%x, Type %d", a_GetTime(), this, i, pProvider, pProvider->GetType());

		prvAABB = pProvider->GetModelAABB();
		if(((nAdd > TMAN_BUILD_LIMIT) && !(dwFlag&TMAN_BUILD_FORCE))
			|| (!CLS_AABBAABBOverlap(gridAABB.Center, gridAABB.Extents, prvAABB.Center, prvAABB.Extents)))
		{
				leftIndex.push_back(i);
				continue;
		}
		//try to add the provider
		nAdd += (_AddProvider(pProvider, true));
	}

	for (size_t i = 0; i < m_UnOrganizedProvider.size(); ++i)
	{
		if (m_bBreakLoadFlag)
			break;

		pProvider = m_UnOrganizedProvider[i];
		//a_LogOutput(1, "@%d: TriGrid 0x%X BuildGradually: %d-th, 0x%x, Type %d", a_GetTime(), this, i, pProvider, pProvider->GetType());

		prvAABB = pProvider->GetModelAABB();
		if(((nAdd > TMAN_BUILD_LIMIT) && !(dwFlag&TMAN_BUILD_FORCE))
			|| (!CLS_AABBAABBOverlap(gridAABB.Center, gridAABB.Extents, prvAABB.Center, prvAABB.Extents)))
		{
			continue;
		}
		//try to add the provider
		_AddProvider(pProvider, false);
	}

	TriangleProvider ** ppPrv = reorder(leftIndex, m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.end());
	m_UnOrganizedProvider.erase(ppPrv, m_UnOrganizedProvider.end());
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nUnorgPrv = m_UnOrganizedProvider.size();
	m_Stat.nTriangle += nAdd; 
	m_Stat.nAddTriangle = nAdd;
	m_Stat.dwBuildTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif
	
}

void CTriangleGrid::BuildFromTerrain(const float* pTerrainHeight, int nWidth, int nHeight)
{
}

bool CTriangleGrid::_UpdateCenter(const A3DVECTOR3& vCenter, DWORD dwFlag)
{
	short iNewX, iNewZ;
	_GetCellIndex(vCenter, iNewX, iNewZ);
	
	/*
	if( ((dwFlag& TMAN_BUILD_FORCE) != TMAN_BUILD_FORCE ) 
		&& (iNewZ == m_iCenterZ) 
		&& (iNewX == m_iCenterX ))
	{
		return false;
	}
	short ix, iz, ix0, iz0;
	int  halfW = (m_iW -1) / 2;
	ix0 = iNewX - halfW;
	int halfH = (m_iH -1) /2;
	iz0 = iNewZ - halfH;
	CTriangleCell * pCell;
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
			pCell = new CTriangleCell;
			dwKey = MAKE_CELL_KEY(ix, iz);
			m_CellTbl.put(dwKey, pCell);

			pCell->m_AABB.Center = A3DVECTOR3(((float)ix + 0.5f) * m_iCellSize, 0.0f, ((float)iz + 0.5f) * m_iCellSize);
			pCell->m_AABB.Extents = A3DVECTOR3(0.5f*m_iCellSize, 9000.0f, 0.5f*m_iCellSize);
			pCell->m_AABB.CompleteMinsMaxs();
			pCell->m_AABB.Mins.y = 9000.0f;
			pCell->m_AABB.Maxs.y = -9000.0f;

		}
	}
	*/

	m_iCenterX = iNewX;
	m_iCenterZ = iNewZ;
	
	return true;
}

void CTriangleGrid::_GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z)
{
	//x = (short)(vPos.x / (float)m_iCellSize);
	//z=  (short)(vPos.z / (float)m_iCellSize);
	x = (short) floor(vPos.x /(float)m_iCellSize);
	z = (short) floor(vPos.z /(float)m_iCellSize);
}

CTriangleCell * CTriangleGrid::_FindCell(short x, short z)
{
	DWORD   dwKey = MAKE_CELL_KEY(x, z);

	if (dwKey >= cell_max_count)
		return NULL;

	return m_CellTbl[dwKey];
}

inline float _get_pos_in_tri_height(const A3DVECTOR3& v0,  const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& p)
{
	float v1x = v1.x - v0.x;
	float v1z = v1.z - v0.z;
	float v2x = v2.x - v0.x;
	float v2z = v2.z - v0.z;
	float px = p.x - v0.x;
	float pz = p.z - v0.z;

	float fdiv = v1x * v2z - v1z * v2x;

	if (fdiv > -1e-5 && fdiv < 1e-5)
		return 0.0f;

	float m = px * v2z - pz * v2x;
	float c1 = m / fdiv;

	if (c1 <= 0)
		return 0;

	m = pz * v1x - px * v1z;
	float c2 = m / fdiv;

	if (c2 <= 0 || c1 + c2 >= 1.f)
		return 0;

	return v0.y + (v1.y - v0.y) * c1 + (v2.y - v0.y) * c2;
}

bool CTriangleGrid::_AddTriangle(TManTriangle * pTri,  int index, const A3DVECTOR3& v0,  const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& vNormal, bool bPreComput)
{
	short x, z, xMin, zMin, xMax, zMax;

	_GetCellIndex(v0, xMin, zMin);
	xMax = xMin;
	zMax = zMin;
	_GetCellIndex(v1, x, z);
	xMax = max2(xMax, x);
	zMax = max2(zMax, z);
	xMin = min2(xMin, x);
	zMin = min2(zMin, z);
	_GetCellIndex(v2, x, z);
	xMax = max2(xMax, x);
	zMax = max2(zMax, z);
	xMin = min2(xMin, x);
	zMin = min2(zMin, z);

	CTriangleCell * pCell;
	A3DAABB aabb;
	aabb.Extents = A3DVECTOR3(0.5f*m_iCellSize, 9000.0f, 0.5f*m_iCellSize);

	short xx, zz;
	for (zz = zMin; zz <= zMax; ++zz)
	{
		for (xx = xMin; xx <= xMax; ++xx)
		{
			aabb.Center = A3DVECTOR3((xx + 0.5f) * m_iCellSize, 0.0f, (zz + 0.5f) * m_iCellSize);
			if (!AABBTriangleOverlap(aabb, v0, v1, v2, vNormal))
			{
				continue;
			}
			pCell = _FindCell(xx, zz);
			if (!pCell)
			{
				DWORD dwKey = MAKE_CELL_KEY(xx, zz);

				if (dwKey < cell_max_count)
				{
					pCell = (CTriangleCell*)m_SmallBufMemMan.Allocate(sizeof(CTriangleCell));
					pCell->Reset();
					m_CellTbl[dwKey] = pCell;
				}
			}

			if (pCell)
			{
				if (bPreComput)
					pCell->wCount++;
				else
				{
					if (pCell->m_Triangles == NULL)
						pCell->m_Triangles = (int*)m_SmallBufMemMan.Allocate(sizeof(int) * pCell->wCount);

					if (pCell->wCurIndex < pCell->wCount)
						pCell->m_Triangles[pCell->wCurIndex++] = index;

					float fMaxHeight = _get_pos_in_tri_height(v0, v1, v2, aabb.Center);
					int nMaxHeight = int(fMaxHeight * (65535.f / 800.f));
					a_Clamp(nMaxHeight, 0, 65535);

					if (nMaxHeight > pCell->m_wMaxHeight)
						pCell->m_wMaxHeight = (WORD)(DWORD)nMaxHeight;
				}
			}
		}	
	}
	
	return true;
}
#if 0
void CTriangleGrid::TestOverlap(TMan_Overlap_Info& tInfo)
{

	short cx, cz;
	_GetCellIndex(tInfo.vStart, cx, cz);

	if((tInfo.dwFlag & TMAN_CHECK_AUTOBUILD) == TMAN_CHECK_AUTOBUILD )
	{
		const float thresh = 2.5f;
		if ((fabs(cx - m_iCenterX) > thresh)
			||(fabs(cz - m_iCenterZ) > thresh))
		{
			Build(tInfo.vStart);
		}
	}
	tInfo.pVert.clear();
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nTestTriangle  = 0;
	DWORD tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	
#endif
	//assert(tInfo.pCamera != NULL);
	A3DAABB  aabb;
	aabb.Center = tInfo.vStart;
	aabb.Extents = tInfo.vExtent;
	aabb.CompleteMinsMaxs();
	A3DVECTOR3  pt[16];
	aabb.GetVertices(pt, NULL, false);
	A3DVECTOR3 * pV0, *pV1;
	pV0 = pt;
	pV1 = pt+8;
	int i;
	for (i = 0; i < 8; ++i)
	{
		pV1[i] = pV0[i] + tInfo.vDelta;
	}
	//construct obb
	A3DOBB  obb;
	obb.Build(pt, 16);
#ifdef TMAN_VERBOSE_STAT
	m_Stat.obb = obb;
#endif
	//check  cell

	const float testRange = 100.0f;
	int nHalf = (int) ( testRange/(float)m_iCellSize + 0.5f);
	
	short ix, iz, ix0, iz0;
	ix0 = cx - nHalf;
	iz0 = cz - nHalf;
	
	CTriangleCell * pCell;

	for (iz = iz0; iz < iz0 + nHalf*2; ++iz)
	{
		for (ix = ix0; ix < ix0 + nHalf*2; ++ix)
		{
			pCell = _FindCell(ix, iz);
			if (!pCell || pCell->m_Triangles.empty()
				||(!CLS_AABBOBBOverlap(pCell->m_AABB, obb))
				||(tInfo.pCamera && !tInfo.pCamera->AABBInViewFrustum(pCell->m_AABB)))
			{
				continue;
			}
			for (int j = 0; j < (int)pCell->m_Triangles.size(); ++j)
			{
				_TestTriangle(tInfo, obb, pCell->m_Triangles[j]);
			}
#ifdef TMAN_VERBOSE_STAT
			m_Stat.nTestTriangle += (pCell->m_Triangles.size());
#endif
		}
	}
#ifdef TMAN_VERBOSE_STAT
	m_Stat.dwTestTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif

}

#endif



void CTriangleGrid::TestOverlapNF(TMan_Overlap_Info& tInfo)
{
	if (!m_bEnable)
		return;

	tInfo.pVert.clear();
	tInfo.pIndex.clear();

	//need camera to back face culling
	if (!tInfo.pCamera)
	{
		return;
	}

	short cx, cz;
	_GetCellIndex(tInfo.vStart, cx, cz);

	
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nTestTriangle  = 0;
	DWORD tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	
#endif

	//construct obb
	A3DOBB  obb;
	_BuildOBB(tInfo, obb);
	//obb.Extents *= 3.0f;
	
	A3DMATRIX4   w2lMat;  //world to obb local
	A3DMATRIX4   l2wMat;  //obb local to world
	w2lMat.SetCol(0, obb.XAxis);   //Rotation
	w2lMat.SetCol(1, obb.YAxis);
	w2lMat.SetCol(2, obb.ZAxis);
	
	w2lMat._41 = -DotProduct(obb.Center, obb.XAxis);   //translation
	w2lMat._42 = -DotProduct(obb.Center, obb.YAxis);
	w2lMat._43 = -DotProduct(obb.Center, obb.ZAxis);

	w2lMat.SetCol(3, A3DVECTOR3(0));
	w2lMat._44 = 1.0f;

	l2wMat.SetRow(0, obb.XAxis);  //rotation
	l2wMat.SetRow(1, obb.YAxis);
	l2wMat.SetRow(2, obb.ZAxis);

	l2wMat.SetRow(3, obb.Center);  //translation

	l2wMat.SetCol(3, A3DVECTOR3(0));
	l2wMat._44 = 1.0f;

	//A3DMATRIX4 tmpMat(w2lMat*l2wMat);  //expect I
	//A3DVECTOR3 tmpVec(obb.Center*w2lMat); //expect zero
	//tmpVec = tmpVec * l2wMat;


#ifdef TMAN_VERBOSE_STAT
	m_Stat.obb = obb;
#endif
	//check  cell
	int nHalf = (int) ( TMAN_TEST_RANGE/(float)m_iCellSize);
	
	short ix, iz, ix0, iz0;
	ix0 = cx - nHalf;
	iz0 = cz - nHalf;

	CTriangleCell * pCell;

	++m_btTestStamp;

	for (iz = iz0; iz < iz0 + nHalf*2; ++iz)
	{
		for (ix = ix0; ix < ix0 + nHalf*2; ++ix)
		{
			pCell = _FindCell(ix, iz);

			if (!pCell)
				continue;

			for (int j = 0; j < pCell->GetTriCount(); ++j)
			{
				TManTriangle * pTri = m_pTrianglePool->GetTriangle(pCell->m_Triangles[j]);

				if (pTri->wCheckStamp != m_btTestStamp)
				{
#ifdef TMAN_VERBOSE_STAT
					++m_Stat.nTestTriangle;
#endif				
					_TestTriangleNF(tInfo, obb, *pTri, w2lMat, l2wMat);
					pTri->wCheckStamp = m_btTestStamp;
				}
			}
		}
	}
#ifdef TMAN_VERBOSE_STAT
	m_Stat.dwTestTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;-
#endif
}

void CTriangleGrid::GetCell(TMan_Overlap_Info& tInfo)
{
	if (!m_bEnable)
		return;

	if (tInfo.bIncCellStamp)
		m_btTestStamp++;

	short left = (short)floorf((tInfo.vStart.x - tInfo.vExtent.x) / m_iCellSize);
	short right = (short)ceilf((tInfo.vStart.x + tInfo.vExtent.x) / m_iCellSize);
	short top = (short)floorf((tInfo.vStart.z - tInfo.vExtent.z) / m_iCellSize);
	short bottom = (short)ceilf((tInfo.vStart.z + tInfo.vExtent.z) / m_iCellSize);
	float fMaxY = tInfo.vStart.y;
	float fMinY = tInfo.vExtent.y;

	A3DVECTOR3 v0, v1, v2;

	for (short col = left; col < right; col++)
	{
		for (short row = top; row < bottom; row++)
		{
			CTriangleCell * pCell;
			pCell = _FindCell(col, row);

			if (!pCell)
				continue;

			if (pCell->m_btTestStamp == m_btTestStamp && pCell->m_bAllTested)
				continue;

			pCell->m_btTestStamp = m_btTestStamp;
			pCell->m_bAllTested = true;
			int sz = pCell->GetTriCount();

			for (int i = 0; i < sz; i++)
			{
				TManTriangle* pTri = m_pTrianglePool->GetTriangle(pCell->m_Triangles[i]);

				if (!pTri->pProvider || !pTri->pProvider->IsEnableForUse()
					|| pTri->wCheckStamp == m_btTestStamp)
					continue;

				DWORD flag = 0;
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[0], v0.x, v0.y, v0.z);
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[1], v1.x, v1.y, v1.z);
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[2], v2.x, v2.y, v2.z);
				

#define CHECK_VERTS_MIN_MAX_HEGIT(n)	{		\
					if (v##n.y > fMaxY)			\
						flag |= 0x1;			\
					else if (v##n.y < fMinY)	\
						flag |= 0x2;			\
					else						\
						goto here;				\
				}

				CHECK_VERTS_MIN_MAX_HEGIT(0)
				CHECK_VERTS_MIN_MAX_HEGIT(1)
				CHECK_VERTS_MIN_MAX_HEGIT(2)

				if (flag != 0x3)
				{
					pCell->m_bAllTested = false;
					continue;
				}

here:
				pTri->wCheckStamp = m_btTestStamp;

				A3DVECTOR3 e01(v1 - v0);
				A3DVECTOR3 e02(v2 - v0);
				A3DVECTOR3 vNormal = CrossProduct(e01, e02);
				vNormal.Normalize();

				if (tInfo.bCheckNormal && ::DotProduct(tInfo.vDelta, vNormal) > 0.0f)
					continue;

				WORD base = tInfo.pVert.size();
				A3DVECTOR3 vOffset = vNormal * .1f;
				tInfo.pVert.push_back(v0 + vOffset);
				tInfo.pVert.push_back(v1 + vOffset);
				tInfo.pVert.push_back(v2 + vOffset);
				tInfo.pIndex.push_back(base);
				tInfo.pIndex.push_back(base+1);
				tInfo.pIndex.push_back(base+2);
				//tInfo.pNormals.push_back(pTri->vNormal);
				//tInfo.pNormals.push_back(pTri->vNormal);
				//tInfo.pNormals.push_back(pTri->vNormal);
				tInfo.vNormal = vNormal;
			}
		}
	}
}

float CTriangleGrid::GetPosHeight(const A3DVECTOR3& vPos)
{
	if (!m_bEnable)
		return 0.0f;

	short x, z;
	_GetCellIndex(vPos, x, z);
	CTriangleCell* pCell = _FindCell(x, z);

	if (pCell)
	{
		float fHeight = pCell->m_wMaxHeight * (800.f / 65535.f);
		return fHeight;
	}
}

bool CTriangleGrid::_TestTriangleNF(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri, const A3DMATRIX4& w2lMat, const A3DMATRIX4& l2wMat)
{

	if (tri.pProvider == NULL)
		return false;

	A3DVECTOR3 vert[3];
	tri.pProvider->GetVert(tri.iSubMesh, tri.index[0], vert[0].x, vert[0].y, vert[0].z);
	tri.pProvider->GetVert(tri.iSubMesh, tri.index[1], vert[1].x, vert[1].y, vert[1].z);
	tri.pProvider->GetVert(tri.iSubMesh, tri.index[2], vert[2].x, vert[2].y, vert[2].z);

	A3DVECTOR3 e01(vert[1] - vert[0]);
	A3DVECTOR3 e02(vert[2] - vert[0]);
	A3DVECTOR3 vNormal = CrossProduct(e01, e02);
	vNormal.Normalize();

	//back face culling  and  light dir culling
	//assume camera ready, dir is from camera(light) to vertex (world space)
	if (/*(DotProduct(tri.vVert[0] - tInfo.pCamera->GetPos(), tri.vNormal) > 0.0f)*/
		tInfo.bCheckNormal && (DotProduct(tInfo.vDelta, vNormal) > 0.0f))
	{
		return false;
	}
	
	//exact test  
	//transform the vert to obb local
	A3DVECTOR3 normal;  //no need translation
	normal.x = DotProduct(vNormal, w2lMat.GetCol(0) );
	normal.y = DotProduct(vNormal, w2lMat.GetCol(1) );
	normal.z = DotProduct(vNormal, w2lMat.GetCol(2) );

	vert[0] = vert[0] * w2lMat;
	vert[1] = vert[1] * w2lMat;
	vert[2] = vert[2] * w2lMat;
	if (!AABBTriangleOverlapNT(obb.Extents, vert[0], vert[1], vert[2], normal))
	{
		return false;
	}

	A3DVECTOR3 clipvert[9];
	int nVert = AABBClipTriangle(obb.Extents, vert, clipvert);
	if (nVert <= 0)
	{
		return false;
	}
	int i;
	WORD base = tInfo.pVert.size();
	//transform back to world space
	for (i = 0; i < nVert; ++i)
	{
		clipvert[i] = clipvert[i] * l2wMat;
		tInfo.pVert.push_back(clipvert[i] + vNormal * .1f);
		//tInfo.pNormals.push_back(tri.vNormal);
		tInfo.vNormal = vNormal;
	}
	//simple triangulation
	for (i = 0; i < nVert -2; ++i)
	{//as a triangle fan
	   tInfo.pIndex.push_back(base);
	   tInfo.pIndex.push_back(base + i +1);
	   tInfo.pIndex.push_back(base + i+ 2);
	}
	
	return true;
}

bool CTriangleGrid::VertRayTrace(TMRayTraceInfo& rtInfo)
{
	if (!m_bEnable)
		return false;

	short cx, cz;
	_GetCellIndex(rtInfo.vStart, cx, cz);


	//check  cell
	int nHalf = 1; //(int) ( TMAN_TEST_RANGE/(float)m_iCellSize);

	short ix0, iz0;
	ix0 = cx - nHalf;
	iz0 = cz - nHalf;

	CTriangleCell * pCell;
	
	
	A3DVECTOR3 vT[3];

	//for (iz = iz0; iz < iz0 + nHalf*2; ++iz)
	{
		//for (ix = ix0; ix < ix0 + nHalf*2; ++ix)
		{
			//pCell = _FindCell(ix, iz);
			pCell = _FindCell(cx, cz);

			if (!pCell)
				return false;

			for (int j = 0; j < pCell->GetTriCount(); ++j)
			{
				TManTriangle * pTri = m_pTrianglePool->GetTriangle(pCell->m_Triangles[j]);

				if (!pTri->pProvider || !pTri->pProvider->IsEnableForUse())
					continue;
				
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[0], vT[0].x, vT[0].y, vT[0].z);
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[1], vT[1].x, vT[1].y, vT[1].z);
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[2], vT[2].x, vT[2].y, vT[2].z);

				A3DVECTOR3 e01(vT[1] - vT[0]);
				A3DVECTOR3 e02(vT[2] - vT[0]);
				A3DVECTOR3 vNormal = CrossProduct(e01, e02);
				vNormal.Normalize();

				float f;
				A3DVECTOR3 vPos;

				if(CHBasedCD::FastRayIntersectWithTriangle(rtInfo.vStart, rtInfo.vDelta, vT, vPos, f, rtInfo.bCullBackFace) && f < rtInfo.fFraction)
				{
					rtInfo.fFraction = f;
					rtInfo.vHitPos = vPos;
					rtInfo.vHitNormal = vNormal;
				}

			}
		}
	}

	if(rtInfo.fFraction < 1.0f)
		return true;
	else
		return false;
	
}

bool CTriangleGrid::CapsuleTrace(TMCapsuleTraceInfo& trInfo )
{
	if (!m_bEnable)
		return false;

	A3DAABB aabb;
	aabb.Clear();

	A3DVECTOR3 ext(trInfo.fCapsuleRadius, trInfo.fCapsuleRadius + trInfo.fCylinderHei / 2.f, trInfo.fCapsuleRadius); 
	aabb.AddVertex(trInfo.vStart);
	aabb.AddVertex(trInfo.vStart + ext);
	aabb.AddVertex(trInfo.vStart - ext);
	aabb.AddVertex(trInfo.vStart + trInfo.vDelta);
	aabb.AddVertex(trInfo.vStart + trInfo.vDelta + ext);
	aabb.AddVertex(trInfo.vStart + trInfo.vDelta - ext);
	aabb.CompleteCenterExts();

	int nHalfX = (int)ceilf(aabb.Extents.x / (float)m_iCellSize); 
	int nHalfZ = (int)ceilf(aabb.Extents.z / (float)m_iCellSize);

	short cx, cz;
	_GetCellIndex(trInfo.vStart, cx, cz);
	short ix, iz;

	CTriangleCell * pCell;

	NxUtilLib* pLib = NxGetUtilLib();
	NxVec3 hit, normal;
	float d;
	NxU32 index = 0;
	
	NxVec3 start = APhysXConverter::A2N_Vector3(trInfo.vStart);	
	NxVec3 dir = APhysXConverter::A2N_Vector3(trInfo.vDelta);
	float len = dir.normalize();
	NxVec3 hitFinal, normalFinal;
	float dFinal = len + 1.f;
	
	A3DVECTOR3 vT[3];
	const int nMaxTri = 50;//max triangle 
	NxTriangle triangle[nMaxTri];
	NxU32 edgeflag[nMaxTri];
	int bSet[nMaxTri];
	if (trInfo.bCullBackFace)
	{
		for (int k = 0; k < nMaxTri; k++)
			edgeflag[k] = 7;
	}
	else
	{
		for (int k = 0; k < nMaxTri; k++)
			edgeflag[k] = 8;
	}
	
	//char *bSet = new char[m_pTrianglePool->GetTriangleNum()];
	//memset(bSet, 0, m_pTrianglePool->GetTriangleNum());

	int nTriangle = 0;

	for (iz = cz - nHalfZ; iz <= cz + nHalfZ; ++iz)
	{
		for (ix = cx - nHalfX; ix <= cx + nHalfX; ++ix)
		{
			pCell = _FindCell(ix, iz);
			if (!pCell )
				continue;

			float minx = ix * m_iCellSize, minz = iz * m_iCellSize;
			if(minx > aabb.Maxs.x || minz > aabb.Maxs.z
				|| minz + m_iCellSize < aabb.Mins.z || minx + m_iCellSize < aabb.Mins.x)
			{
				continue;
			}

			for (int j = 0; j < pCell->GetTriCount(); ++j)
			{
				TManTriangle * pTri = m_pTrianglePool->GetTriangle(pCell->m_Triangles[j]);

				if (!pTri->pProvider || !pTri->pProvider->IsEnableForUse() )
					continue;

				//find whether it has been used
				if (linear_find(bSet, bSet + nTriangle, pCell->m_Triangles[j]) != bSet + nTriangle)
					continue;

				bSet[nTriangle] = pCell->m_Triangles[j];
				
				//bSet[pCell->m_Triangles[j] ] = 1;
				NxTriangle& tri = triangle[nTriangle];
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[0], tri.verts[0].x, tri.verts[0].y, tri.verts[0].z);
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[1], tri.verts[1].x, tri.verts[1].y, tri.verts[1].z);
				pTri->pProvider->GetVert(pTri->iSubMesh, pTri->index[2], tri.verts[2].x, tri.verts[2].y, tri.verts[2].z);
				////
				if (aabb.Mins.x > max3(tri.verts[0].x, tri.verts[1].x, tri.verts[2].x)
					|| aabb.Mins.y > max3(tri.verts[0].y, tri.verts[1].y, tri.verts[2].y)
					|| aabb.Mins.z > max3(tri.verts[0].z, tri.verts[1].z, tri.verts[2].z)
					|| min3(tri.verts[0].x, tri.verts[1].x, tri.verts[2].x) > aabb.Maxs.x
					|| min3(tri.verts[0].y, tri.verts[1].y, tri.verts[2].y) > aabb.Maxs.y
					|| min3(tri.verts[0].z, tri.verts[1].z, tri.verts[2].z) > aabb.Maxs.z)
					continue;

				nTriangle++;
				if (nTriangle < nMaxTri)
					continue;

				bool bRet = pLib->NxSweepCapsuleTriangles(1, nTriangle, triangle, edgeflag, start, trInfo.fCapsuleRadius, trInfo.fCylinderHei, dir, len, hit, normal, d, index);
				if (bRet && d < dFinal)
				{
					dFinal = d;
					hitFinal = hit;
					normalFinal = normal;					
				}
				trInfo.nTriangle += nTriangle;
				nTriangle = 0;
			}
			
		}
	}

	if (nTriangle > 0)
	{
		bool bRet = pLib->NxSweepCapsuleTriangles(1, nTriangle, triangle, edgeflag, start, trInfo.fCapsuleRadius, trInfo.fCylinderHei, dir, len, hit, normal, d, index);
		trInfo.nTriangle += nTriangle;
		if (bRet && d < dFinal)
		{
			dFinal = d;
			hitFinal = hit;
			normalFinal = normal;			
		}
	}

	//delete[] bSet;

	if (dFinal <= len)
	{
		trInfo.vHitNormal = APhysXConverter::N2A_Vector3(normalFinal);
		trInfo.vHitPos = APhysXConverter::N2A_Vector3(hitFinal);
		trInfo.fFraction = dFinal / len;
		return true;
	}
	return false;
}

void CTriangleGrid::_ReleaseCellTbl()
{
	for (int i = 0; i < cell_max_count; i++)
	{
		if (m_CellTbl[i])
		{
			m_CellTbl[i] = NULL;
		}
	}

	m_SmallBufMemMan.Release();
}

CTManTrianglePool::CTManTrianglePool()
{
	m_nMaxCount = m_nTriManufSize;
	TManTriangle* pBuf = new TManTriangle[m_nMaxCount];
	m_aBuf.reserve(16);
	m_aBuf.push_back(pBuf);
	m_nLast = 0;
}

CTManTrianglePool::~CTManTrianglePool()
{
	for (size_t i = 0; i < m_aBuf.size(); i++)
		delete[] m_aBuf[i];
}

/*
 * 
 * @desc :   assume the input param is correct for efficiency.
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [18/11/2005]
 * @ref:
 */
void CTManTrianglePool::_Grow()
{
	TManTriangle* pNewBuf = new TManTriangle[m_nTriManufSize];
	m_aBuf.push_back(pNewBuf);
	m_nMaxCount += m_nTriManufSize;
}

#endif  //_ANGELICA3

