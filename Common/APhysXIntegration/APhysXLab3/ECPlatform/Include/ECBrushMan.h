/********************************************************************
  created:	   25/10/2005  
  filename:	   ECBrushMan.h
  author:      Wangkuiwu  
  description: brush manager for cd
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma  once

#include "ECBrushManUtil.h"

class ECTraceInfo;
//@desc : A interface for building brush manager. By Kuiwu[25/10/2005]
class ECBrushProvider
{
	
public:
	ECBrushProvider() 
	{
	};
	virtual ~ECBrushProvider()
	{
	};
	virtual int GetBrushCount() = 0;
	virtual ECBaseBrush * GetBrush(int index) = 0;

	//get rotation and translation
	const A3DMATRIX4& GetRotAndTrans() const { return m_rotAndTrans; }
	//get model scale
	float GetScale() const { return m_fScale; }

	const AString& GetChbFile() { return m_strChbFile;}

protected:

	//for collision detection(yx)
	A3DMATRIX4 m_rotAndTrans;  //model rotation and translation
	float     m_fScale;        //model scale
	AString   m_strChbFile;    //file name of convex brush

private:


};


//@todo : REFACTOR!  use GOF bridge pattern to support 
//		  different discrete implementations(tree or grid, etc), 
//		  but it seems unnecessary.	  	 By Kuiwu[4/11/2005]
class ECBrushMan  
{
public:
	ECBrushMan();
	virtual ~ECBrushMan();
	void Release();
	void Build(const  A3DVECTOR3& vCenter, bool bForce = false);
	bool RemoveProvider(ECBrushProvider * pBrushProvider);
	/*
	 * 
	 * @desc :
	 * @param :     
	 * @return :
	 * @note:  the caller should assure not to add duplicate providers.
	 * @todo:   
	 * @author: kuiwu [27/10/2005]
	 * @ref:
	 */
	void AddProvider(ECBrushProvider * pBrushProvider);
	/*
	 *
	 * @desc :
	 * @param bCheckFlag: check the brush flag if true, UGLY, but for compatibility.    
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [27/10/2005]
	 * @ref:
	 */
	bool  Trace(BrushTraceInfo * pInfo, bool bCheckFlag = true);
	
	bool  Trace(AbstractBrushTraceInfo* pInfo, bool bCheckFlag = true);

	bool  Trace(ECTraceInfo* pInfo, bool bCheckFlag = true);


	void RenderBrush();  // Yongdong: TODO delete

#if BMAN_VERBOSE_STAT
	CBManStat * GetStat()
	{
		return m_pBrushGrid->GetStat();
	}
#endif

private:
	
	ECBrushGrid    * m_pBrushGrid;
};

