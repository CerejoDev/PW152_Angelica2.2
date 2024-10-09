
#include "Global.h"
#include "A3DGFXExMan.h"
#include "Render.h"
#include "AFI.h"

#include "PhysParticleObject.h"



CPhysParticleObject::CPhysParticleObject()
{
	m_pGfxEx = NULL;

	// clear the obb data to zero...
	memset(&m_EmitterOBB, 0, sizeof(A3DOBB));
	m_EmitterOBB.XAxis.Set(1, 0, 0);
	m_EmitterOBB.YAxis.Set(0, 1, 0);
	m_EmitterOBB.ZAxis.Set(0, 0, 1);

	m_EmitterOBB.Extents.Set(0.3f, 0.3f, 0.3f);

}
	
CPhysParticleObject::~CPhysParticleObject()
{


}


bool CPhysParticleObject::LoadModel(const char* szFile)
{

	if(!CPhysObject::LoadModel(szFile))
		return false;

	// note here, szFile should be under the dir of "\Gfx\" and only the file name without any path name...
	char szOnlyFileName[128];
	WORD wLen = 0;
	af_GetFileTitle(szFile, szOnlyFileName, wLen);
	
	A3DGFXEx* pGfxEx = AfxGetGFXExMan()->LoadGfx(g_Render.GetA3DDevice(), szOnlyFileName);
	if(pGfxEx)
	{

		m_pGfxEx = pGfxEx;

	}
	else
		return false;

	return true;
}
	
void CPhysParticleObject::Release()
{

	if(m_pGfxEx)
	{
		m_pGfxEx->Release();
		delete m_pGfxEx;

		m_pGfxEx = NULL;
	
	}

}

bool CPhysParticleObject::Tick(DWORD dwDeltaTime)
{
	if(m_pGfxEx)
	{

		m_pGfxEx->TickAnimation(dwDeltaTime);
	}

	return true;
}

bool CPhysParticleObject::Render(A3DViewport* pViewport)
{
	if(m_pGfxEx)
	{
		if(m_pGfxEx->GetState() == ST_PLAY)
			AfxGetGFXExMan()->RegisterGfx(m_pGfxEx);
		else
		{
			// render the obb instead...
			A3DWireCollector* pwc = g_Render.GetA3DEngine()->GetA3DWireCollector();
			pwc->AddOBB(m_EmitterOBB, 0xffff0000);

		}
	}


	return true;

}

bool CPhysParticleObject::RenderProxy(A3DViewport* pViewport)
{

	if(m_pGfxEx)
	{
		A3DWireCollector* pwc = g_Render.GetA3DEngine()->GetA3DWireCollector();
		pwc->AddOBB(m_EmitterOBB, 0xffffff00);

	}

	return true;
}

void CPhysParticleObject::SetPos(const A3DVECTOR3& vPos)
{
	if(m_pGfxEx)
	{
		m_pGfxEx->SetPos(vPos);

		// update the obb...
		m_EmitterOBB.Center = vPos;

	}
	
}
	
void CPhysParticleObject::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if(m_pGfxEx)
	{
		m_pGfxEx->SetDirAndUp(vDir, vUp);

		m_EmitterOBB.ZAxis = vDir;
		m_EmitterOBB.YAxis = vUp;

		m_EmitterOBB.XAxis = CrossProduct(vUp,vDir);

		// call this to compute the component extents in different axis...
		m_EmitterOBB.CompleteExtAxis();


	}

}

bool CPhysParticleObject::InstantializePhysX()
{
	if(m_pGfxEx)
	{
		return m_pGfxEx->Start();
	}
	else
		return false;
}
	
bool CPhysParticleObject::ReleasePhysX()
{
	if(m_pGfxEx)
	{
		return m_pGfxEx->Stop();
	}
	else
		return false;
	
}

bool CPhysParticleObject::TurnOnPhysX()
{
	if(m_pGfxEx)
	{
		return m_pGfxEx->Start();
	}
	else
		return false;
}
	
bool CPhysParticleObject::TurnOffPhysX()
{
	if(m_pGfxEx)
	{
		return m_pGfxEx->Stop();
	}
	else
		return false;

}

bool CPhysParticleObject::RayTrace(const PhysRay& ray, PhysRayTraceHit& hitInfo)
{

	PhysOBB rtOBB;

	// transform the obb's center since obb's center is always given by the global reference frame...
	A3DVECTOR3 vOBBCenter;
	vOBBCenter.x = DotProduct(m_EmitterOBB.Center, m_EmitterOBB.XAxis);
	vOBBCenter.y = DotProduct(m_EmitterOBB.Center, m_EmitterOBB.YAxis);
	vOBBCenter.z = DotProduct(m_EmitterOBB.Center, m_EmitterOBB.ZAxis);

	rtOBB.vMin = vOBBCenter - m_EmitterOBB.Extents;
	rtOBB.vMax = vOBBCenter + m_EmitterOBB.Extents;

	rtOBB.vX = m_EmitterOBB.XAxis;
	rtOBB.vY = m_EmitterOBB.YAxis;
	rtOBB.vZ = m_EmitterOBB.ZAxis;

	if(RayTraceOBB(ray, rtOBB, hitInfo))
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