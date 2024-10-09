/*
 * FILE: PhysObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSOBJECT_H_
#define _PHYSOBJECT_H_

#include <A3DVector.h>
#include "AString.h"

#include "PhysRayTrace.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


#define DeclarePhysObjType(type)								\
	public:													\
		virtual int GetObjType() const {return type;}		\
	protected:												


enum EPhysObjectType
{
	PHYS_OBJTYPE_BASE,
	
	PHYS_OBJTYPE_STATIC,
	
	PHYS_OBJTYPE_SKELETON,

	PHYS_OBJTYPE_PARTICLE,

};



///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DViewport;
class NxStream;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CPhysObject
//	
///////////////////////////////////////////////////////////////////////////

class CPhysObject
{

	DeclarePhysObjType(PHYS_OBJTYPE_BASE);

public:		//	Types

	//	Physical state
	enum
	{
		PHY_STATE_PURESIMULATE = 0,		//	Pure physical simulation
		PHY_STATE_ANIMATION,			//	Pure model animation
		PHY_STATE_PARTSIMULATE,			//	Partial physical simulation
		NUM_PHY_STATE,
	};

public:		//	Constructor and Destructor

	CPhysObject();
	virtual ~CPhysObject();

public:		//	Attributes

public:		//	Operations


	// load/save all my data from the NxStream
	virtual bool Load(NxStream* pStream);
	virtual bool Save(NxStream* pStream);

	// load the data from the file...
	virtual bool LoadModel(const char* szFile); 

	//	Release object
	virtual void Release() {}
	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime) { return true; }
	//	Sync physical data to graphics data
	virtual bool Sync() { return true; }
	//	Render routine
	virtual bool Render(A3DViewport* pViewport) { return true; }
	//	Render proxy
	virtual bool RenderProxy(A3DViewport* pViewport) { return true; }

	//----------------------------------------------------
	// model pose access...

	//	Set position
	virtual void SetPos(const A3DVECTOR3& vPos) {}
	//	Set orientation
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) {}

	virtual A3DVECTOR3 GetPos() { return A3DVECTOR3(0);}
	virtual void GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) { }
	//----------------------------------------------------

	//	Setup object's pose, while its corresponding physX object will be instantialized untill you call InstantializePhysX...
	virtual bool Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) 
	{ 
		m_vPos = vPos;
		m_vDir = vDir;
		m_vUp = vUp;

		SetPos(vPos);
		SetDirAndUp(vDir, vUp);
		
		return true; 
	}

	// reset the object's pose to initial pose setup by the function Setup
	virtual void ResetPose() {}
	
	virtual bool InstantializePhysX() { return true;}
	virtual bool ReleasePhysX() { return true; }

	virtual bool TurnOnPhysX() { return true; }
	virtual bool TurnOffPhysX() { return true; }

	//	Change physical state
	virtual bool ChangePhysicalState(int iState) { return true; }

	virtual bool RayTrace(const PhysRay& ray, PhysRayTraceHit& hitInfo)
	{
		return false;
	}

	virtual void Move(const A3DVECTOR3& vDelta) 
	{
		// do nothing...
	}

protected:	//	Attributes

	int		m_iPhysState;

	// final pose in the scene, we will instantialize physX object according to this pos...
	A3DVECTOR3 m_vPos;
	A3DVECTOR3 m_vDir;
	A3DVECTOR3 m_vUp;

	// it should be the relative path so that we can open the scene file in other machine...
	AString m_strModelFilePath;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_PHYSOBJECT_H_
