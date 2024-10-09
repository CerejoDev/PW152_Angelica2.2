/*
 * FILE: PhysStaticObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "PhysStaticObject.h"
#include "Render.h"
#include "Game.h"

#include "APhysX.h"

#include <A3DLitModel.h>
#include <A3DMacros.h>
#include <A3DViewport.h>
#include <A3DEngine.h>
#include <A3DWireCollector.h>
#include <AFI.h>

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
//	Implement CPhysStaticObject
//	
///////////////////////////////////////////////////////////////////////////

CPhysStaticObject::CPhysStaticObject()
{
	m_pModel			= NULL;
	m_pPhysXRBObject	= NULL;
}

CPhysStaticObject::~CPhysStaticObject()
{
}

//	Load object from file
bool CPhysStaticObject::LoadModel(const char* szFile)
{

	if(!CPhysObject::LoadModel(szFile))
		return false;

	//	Load model
	if (!LoadMoxModel(szFile))
		return false;

	return true;
}

//	Release object
void CPhysStaticObject::Release()
{
	A3DRELEASE(m_pModel);

	APhysXScene* pPhysXScene = g_Game.GetPhysXScene();

	if (m_pPhysXRBObject)
	{
		pPhysXScene->ReleasePhysXObject(m_pPhysXRBObject);
		m_pPhysXRBObject = NULL;
	}
}

//	Load model from file
bool CPhysStaticObject::LoadMoxModel(const char* szFile)
{
	m_pModel = new A3DLitModel;
	if (!m_pModel)
		return false;
	
	if (!m_pModel->LoadFromMox(g_Render.GetA3DDevice(), szFile))
	{
		A3DRELEASE(m_pModel);
		a_LogOutput(1, "CPhysStaticObject::LoadModel, Failed to load mox file !");
		return false;
	}

	//	Save physical data file name
	m_strPhysFile = szFile;
	af_ChangeFileExt(m_strPhysFile, ".pxd");

	//	Set default position and orientation
	m_pModel->SetPos(g_vOrigin);
	m_pModel->SetDirAndUp(g_vAxisZ, g_vAxisY);

	return true;
}

//	Tick routine
bool CPhysStaticObject::Tick(DWORD dwDeltaTime)
{
	return true;
}

//	Render routine
bool CPhysStaticObject::Render(A3DViewport* pViewport)
{
	if (m_pModel)
		m_pModel->Render(pViewport);	

	return true;
}

//	Render proxy
bool CPhysStaticObject::RenderProxy(A3DViewport* pViewport)
{
	if (!m_pModel)
		return true;
	
	A3DWireCollector* pwc = g_Render.GetA3DEngine()->GetA3DWireCollector();
	const A3DAABB& aabb = m_pModel->GetModelAABB();
	pwc->AddAABB(aabb, 0xffffff00);

	return true;
}

//	Set position
void CPhysStaticObject::SetPos(const A3DVECTOR3& vPos)
{
	if (m_pModel)
		m_pModel->SetPos(vPos);
}

//	Set orientation
void CPhysStaticObject::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (m_pModel)
		m_pModel->SetDirAndUp(vDir, vUp);
}

//	Setup object
/*
bool CPhysStaticObject::Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	APhysXScene* pPhysXScene = g_Game.GetPhysXScene();

	if (m_pPhysXRBObject)
	{
		pPhysXScene->ReleasePhysXObject(m_pPhysXRBObject);
		m_pPhysXRBObject = NULL;
	}

	APhysXObjectInstanceDesc desc;
	desc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(m_strPhysFile);
	if (!desc.mPhysXObjectDesc)
		return false;

	A3DVECTOR3 vRight = CrossProduct(vUp, vDir);
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	mat.SetRow(0, vRight);
	mat.SetRow(1, vUp);
	mat.SetRow(2, vDir);
	mat.SetRow(3, vPos);
	APhysXConverter::A2N_Matrix44(mat, desc.mGlobalPose);

	m_pPhysXRBObject = pPhysXScene->CreatePhysXObject(desc);
	if (!m_pPhysXRBObject)
		return false;

	return true;
}
*/

bool CPhysStaticObject::InstantializePhysX()
{
	APhysXScene* pPhysXScene = g_Game.GetPhysXScene();
	if(!pPhysXScene) return false;

	if (m_pPhysXRBObject)
	{
		pPhysXScene->ReleasePhysXObject(m_pPhysXRBObject);
		m_pPhysXRBObject = NULL;
	}

	APhysXObjectInstanceDesc desc;
	desc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(m_strPhysFile);
	if (!desc.mPhysXObjectDesc)
		return false;

	A3DVECTOR3 vRight = CrossProduct(m_vUp, m_vDir);
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	mat.SetRow(0, vRight);
	mat.SetRow(1, m_vUp);
	mat.SetRow(2, m_vDir);
	mat.SetRow(3, m_vPos);
	APhysXConverter::A2N_Matrix44(mat, desc.mGlobalPose);

	m_pPhysXRBObject = pPhysXScene->CreatePhysXObject(desc);
	if (!m_pPhysXRBObject)
		return false;

	return true;


}


bool CPhysStaticObject::ReleasePhysX()
{

	APhysXScene* pPhysXScene = g_Game.GetPhysXScene();
	if(m_pPhysXRBObject && pPhysXScene)
	{
		pPhysXScene->ReleasePhysXObject(m_pPhysXRBObject);
		m_pPhysXRBObject = NULL;

	}

	return true;
}

bool CPhysStaticObject::RayTrace(const PhysRay& ray, PhysRayTraceHit& hitInfo)
{
	if(m_pModel)
	{
		PhysAABB rtAABB;
		A3DAABB aabb = m_pModel->GetModelAABB();
		aabb.CompleteMinsMaxs();
		rtAABB.vMin = aabb.Mins;
		rtAABB.vMax = aabb.Maxs;

		if(RayTraceAABB(ray, rtAABB, hitInfo))
		{
			// need we make some further raytracing such as raytracing the mesh?...
			hitInfo.UserData = this;
			return true;
		}
		else
		{
			return false;
		}


	}
	else
		return false;

}

A3DVECTOR3 CPhysStaticObject::GetPos() 
{ 
	if(m_pModel)
		return m_pModel->GetPos();
	else
		return A3DVECTOR3(0);
}

void CPhysStaticObject::GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) 
{ 
	if(m_pModel)
	{
		vDir = m_pModel->GetDir();
		vUp = m_pModel->GetUp();
	}
}