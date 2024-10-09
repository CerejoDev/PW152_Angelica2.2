/*
 * FILE: PhysObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "PhysObject.h"

#include "APhysX.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CPhysObject
//	
///////////////////////////////////////////////////////////////////////////

CPhysObject::CPhysObject()
{
	m_iPhysState = PHY_STATE_PURESIMULATE;

	m_vPos.Set(0, 0, 0);
	m_vDir = g_vAxisZ;
	m_vUp = g_vAxisY;


}

CPhysObject::~CPhysObject()
{
}


bool CPhysObject::Load(NxStream* pStream)
{

	// load the pose...
	pStream->readBuffer(&m_vPos, sizeof(A3DVECTOR3));
	pStream->readBuffer(&m_vDir, sizeof(A3DVECTOR3));
	pStream->readBuffer(&m_vUp, sizeof(A3DVECTOR3));

	// load the path of the model...
	m_strModelFilePath = APhysXSerializerBase::ReadString(pStream);

	//??? release here???
	Release();

	if(!LoadModel(m_strModelFilePath))
		return false;

	Setup(m_vPos, m_vDir, m_vUp);

	return true;

}

bool CPhysObject::Save(NxStream* pStream)
{
	
	// save the pose...
	pStream->storeBuffer(&m_vPos, sizeof(A3DVECTOR3));
	pStream->storeBuffer(&m_vDir, sizeof(A3DVECTOR3));
	pStream->storeBuffer(&m_vUp, sizeof(A3DVECTOR3));

	// save the path of the model...
	APhysXSerializerBase::StoreString(pStream, m_strModelFilePath);

	return true;
}

bool CPhysObject::LoadModel(const char* szFile)
{ 
	// we will keep the relative file path in m_strModelFilePath...
	char szRelativeFilePath[256];
	af_GetRelativePath(szFile, szRelativeFilePath);

	m_strModelFilePath = szRelativeFilePath;


	return true;
}
