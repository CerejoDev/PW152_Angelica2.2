
#ifndef _PHYSPARTICLEOBJECT_H_
#define _PHYSPARTICLEOBJECT_H_

#include "PhysObject.h"
#include "A3DGeometry.h"

class A3DGFXEx;


class CPhysParticleObject : public CPhysObject
{

	DeclarePhysObjType(PHYS_OBJTYPE_PARTICLE);

public:		//	Types

public:		//	Constructor and Destructor

	CPhysParticleObject();
	virtual ~CPhysParticleObject();

public:		//	Attributes

public:		//	Operations


	//	Load object from file
	bool LoadModel(const char* szFile);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(A3DViewport* pViewport);
	//	Render proxy
	virtual bool RenderProxy(A3DViewport* pViewport);

	//	Set position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set orientation
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	
	//	Setup object
	// virtual bool Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool InstantializePhysX();
	virtual bool ReleasePhysX();

	virtual bool RayTrace(const PhysRay& ray, PhysRayTraceHit& hitInfo);

	virtual A3DVECTOR3 GetPos() 
	{ 
		return m_EmitterOBB.Center;
	}
	
	virtual void GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) 
	{ 
		vDir = m_EmitterOBB.ZAxis;
		vUp = m_EmitterOBB.YAxis;
	}

	virtual bool TurnOnPhysX();
	virtual bool TurnOffPhysX();


protected:	//	Attributes

	A3DGFXEx* m_pGfxEx;

	A3DOBB m_EmitterOBB;		// the proxy for rendering...


protected:	//	Operations

};



#endif