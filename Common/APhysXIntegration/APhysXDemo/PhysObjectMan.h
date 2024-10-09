/*
 * FILE: PhysObjectMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSOBJECTMAN_H_
#define _PHYSOBJECTMAN_H_

#include <AArray.h>

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

class CPhysObject;
class A3DViewport;

class NxStream;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CPhysObjectMan
//	
///////////////////////////////////////////////////////////////////////////

class CPhysObjectMan
{

public:

	static CPhysObject* CreatePhysObject(int iType);

public:		//	Types

public:		//	Constructor and Destructor

	CPhysObjectMan();
	virtual ~CPhysObjectMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();
	//	Reset object
	bool Reset();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Sync physical data to graphics data
	bool Sync();
	//	Render routine
	bool Render(A3DViewport* pViewport);

	//	Add physical object
	bool AddPhysObject(CPhysObject* pObject);
	void RemovePhysObject(CPhysObject* pObject);

	//	Change physical state
	void ChangePhysicalState(int iPhysState);


	void ResetPose();
	void InstantializePhysX();
	void ReleasePhysX();

	void Move(const A3DVECTOR3& vDelta);

	// load/save all my data from the NxStream
	bool Load(NxStream* pStream);
	bool Save(NxStream* pStream);

	// array manipulation...
	int GetNum() { return m_aObjects.GetSize(); }
	CPhysObject* GetPhysObject(int i) { return m_aObjects[i]; }

	
protected:	//	Attributes

	APtrArray<CPhysObject*>		m_aObjects;		//	Physical objects

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_PHYSOBJECTMAN_H_
