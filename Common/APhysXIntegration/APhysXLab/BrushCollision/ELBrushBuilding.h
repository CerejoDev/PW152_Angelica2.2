// ELBrushBuilding.h: interface for the CELBrushBuilding class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELBRUSHBUILDING_H__A1F2939E_D5C0_4DF1_802F_548E8CD8F5CD__INCLUDED_)
#define AFX_ELBRUSHBUILDING_H__A1F2939E_D5C0_4DF1_802F_548E8CD8F5CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EC_BrushMan.h"
#include "ChbEntity.h"

using namespace CHBasedCD;

class CELBrushBuilding : public CBrushProvider  
{
public:
	CELBrushBuilding();
	virtual ~CELBrushBuilding();

	bool Load(const AString& strFile);

	void Release();

	virtual int GetBrushCount();
	virtual CConvexBrush* GetBrush(int index);
	virtual A3DMATRIX4 GetTransMat();

	void SetTransMat(const A3DMATRIX4& mat) { m_trans = mat; }

//#ifdef _DEBUG
	void DebugRender(A3DFlatCollector* pFC, float fScale = 1, DWORD dwFColor=0xa0ffff00, const A3DVECTOR3& vRayDir=A3DVECTOR3(1.0f,1.0f,1.0f), const A3DMATRIX4* pTransMatrix=NULL)
	{
		if (m_pEntity)
			m_pEntity->DebugRender(pFC,fScale, dwFColor, vRayDir,pTransMatrix);
	}
//#endif

protected:
	CChbEntity *m_pEntity;
	A3DMATRIX4 m_trans;
};

#endif // !defined(AFX_ELBRUSHBUILDING_H__A1F2939E_D5C0_4DF1_802F_548E8CD8F5CD__INCLUDED_)
