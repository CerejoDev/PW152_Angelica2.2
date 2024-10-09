// ELBrushBuilding.cpp: implementation of the CELBrushBuilding class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "aphysxlab.h"
#include "ELBrushBuilding.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CELBrushBuilding::CELBrushBuilding()
{
	m_pEntity = NULL;
	m_trans.Identity();
}

CELBrushBuilding::~CELBrushBuilding()
{
	Release();
}

int CELBrushBuilding::GetBrushCount()
{
	return m_pEntity->GetBrushCount();
}

CConvexBrush* CELBrushBuilding::GetBrush(int index)
{
	return m_pEntity->GetBrush(index);
}

A3DMATRIX4 CELBrushBuilding::GetTransMat()
{
	return m_trans;
}

bool CELBrushBuilding::Load(const AString& strFile)
{
	Release();
	m_pEntity = new CChbEntity;

	if (!m_pEntity->Load(strFile))
	{
		delete m_pEntity;
		m_pEntity = NULL;
		return false;
	}
	return true;
}

void CELBrushBuilding::Release()
{
	delete m_pEntity;
	m_pEntity = NULL;
	m_trans.Identity();
}
