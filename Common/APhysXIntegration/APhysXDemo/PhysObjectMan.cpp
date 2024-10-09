/*
 * FILE: PhysObjectMan.cpp
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
#include "PhysObjectMan.h"
#include "PhysObject.h"

#include "PhysStaticObject.h"
#include "PhysModelObject.h"
#include "PhysParticleObject.h"

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

CPhysObject* CPhysObjectMan::CreatePhysObject(int iType)
{
	CPhysObject* pPhysObj = NULL;
	switch(iType)
	{
	case PHYS_OBJTYPE_STATIC:

		pPhysObj = new CPhysStaticObject;

		break;

	case PHYS_OBJTYPE_SKELETON:

		pPhysObj = new CPhysModelObject;

	    break;
	
	case PHYS_OBJTYPE_PARTICLE:

		pPhysObj = new CPhysParticleObject;

		break;

	default:
		ASSERT(0);
	    break;
	}

	return pPhysObj;

}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CPhysObjectMan
//	
///////////////////////////////////////////////////////////////////////////

CPhysObjectMan::CPhysObjectMan()
{
}

CPhysObjectMan::~CPhysObjectMan()
{
}

//	Initialize object
bool CPhysObjectMan::Init()
{
	return true;
}

//	Release object
void CPhysObjectMan::Release()
{
	//	Release all objects
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Release();
		delete pObject;
	}

	m_aObjects.RemoveAll(true);
}

//	Reset object
bool CPhysObjectMan::Reset()
{
	//	Release all objects
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Release();
		delete pObject;
	}

	m_aObjects.RemoveAll(true);

	return true;
}

//	Tick routine
bool CPhysObjectMan::Tick(DWORD dwDeltaTime)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Tick(dwDeltaTime);
	}

	return true;
}

//	Sync physical data to graphics data
bool CPhysObjectMan::Sync()
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Sync();
	}

	return true;
}

//	Render routine
bool CPhysObjectMan::Render(A3DViewport* pViewport)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Render(pViewport);
	}

	return true;
}

//	Add physical object
bool CPhysObjectMan::AddPhysObject(CPhysObject* pObject)
{
	m_aObjects.Add(pObject);
	return true;
}

//	Change physical state
void CPhysObjectMan::ChangePhysicalState(int iPhysState)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->ChangePhysicalState(iPhysState);
	}
}

void CPhysObjectMan::InstantializePhysX()
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->InstantializePhysX();
	}

}

void CPhysObjectMan::ResetPose()
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->ResetPose();
	}

}

void CPhysObjectMan::ReleasePhysX()
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->ReleasePhysX();
	}
}

bool CPhysObjectMan::Load(NxStream* pStream)
{
	Reset();

	int iObjNum = pStream->readDword();
	for (int i=0; i < iObjNum; i++)
	{
		int iType = pStream->readDword();
		CPhysObject* pObject = CreatePhysObject(iType);
		if(pObject )
		{
			if(pObject->Load(pStream))
				m_aObjects.Add(pObject);
			else
				delete pObject;
		}
		
	}

	return true;
}

bool CPhysObjectMan::Save(NxStream* pStream)
{
	pStream->storeDword(m_aObjects.GetSize());

	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pStream->storeDword(pObject->GetObjType());
		pObject->Save(pStream);
	}

	return true;

}

void CPhysObjectMan::RemovePhysObject(CPhysObject* pObject)
{

	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		if(pObject == m_aObjects[i])
		{
			pObject->Release();
			delete pObject;

			m_aObjects.RemoveAtQuickly(i);

			return;
		}
	}


}

void CPhysObjectMan::Move(const A3DVECTOR3& vDelta)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Move(vDelta);
	}

}