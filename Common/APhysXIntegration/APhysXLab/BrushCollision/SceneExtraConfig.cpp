#include "SceneExtraConfig.h"
#include "Game.h"
#include "Render.h"
#include "RayTrace.h"
#include "ECCDR.h"
#include "MainActor.h"
#include "BrushObjAttr.h"

SceneExtraConfig::SceneExtraConfig()
{
	m_InitCC = 2;
	m_bCCSwitchAble = true;
	m_pEditingRegion = NULL;
	m_bLock = false;
}

SceneExtraConfig::~SceneExtraConfig()
{

}

bool SceneExtraConfig::ApplyInitCC()
{
	//apply cc
	for (int i = 0; i < g_Game.m_MainActors.GetSize(); i++)
	{
		g_Game.m_MainActors[i]->SwtichCC(m_InitCC);
		if (m_InitCC == 2)//brush cc
		{
			g_Game.m_bIsCloseModelClothes = true;
			g_Game.m_objMgr.ControlClothes(g_Game.m_bIsCloseModelClothes);
		}
	}
	return true;
}

bool SceneExtraConfig::InitDefault()
{
	g_Game.m_BrushesObjMgr.Init(&g_Game.m_objMgr);

	m_InitCC = 2;
	m_bCCSwitchAble = true;
	m_pEditingRegion = NULL;
	m_bLock = false;
	m_vRegion.clear();

	return true;
}

bool SceneExtraConfig::CreateRegionActors()
{
	//create actor in region
	int i;
	for (i = 0; i < m_vRegion.size(); i++)
	{
		for (int j = 0; j < m_vRegion[i].actorNum; j++)
		{
			float x = a_Random(m_vRegion[i].aabb.Mins.x, m_vRegion[i].aabb.Maxs.x);
			float z = a_Random(m_vRegion[i].aabb.Mins.z, m_vRegion[i].aabb.Maxs.z);
			A3DVECTOR3 vStart(x, m_vRegion[i].aabb.Center.y + 100, z);
			A3DVECTOR3 vDelta(0, -100, 0);
			BrushCollideParam para;
			para.vStart = vStart;
			para.vDelta = vDelta;
			para.fCapsuleCenterHei = 0.8f;
			para.fCapsuleHei = 1;
			para.fCapsuleR = 0.3f;
			para.vCapsuleUp = A3DVECTOR3(0, 1, 0);
			A3DVECTOR3 vHitPos;
			if (RetrievePlane(&para, vHitPos))
			{
				vHitPos.y -= para.fCapsuleCenterHei;
				CMainActor *pActor = g_Game.CreateMainActor();
				pActor->m_objModel.SetPos(vHitPos);
				A3DVECTOR3 vDirH(0, 0, 0);
				vDirH.x = a_Random(0.f, 1.f);
				vDirH.z = a_Random(0.f, 1.f);
				vDirH.Normalize();
				pActor->m_objModel.SetDirAndUp(vDirH, A3DVECTOR3(0, 1, 0));
			}
		}
		
	}
	
	return true;
}
bool SceneExtraConfig::Load(const NxStream& nxStream)
{
	g_Game.m_BrushesObjMgr.Init(&g_Game.m_objMgr);
	int count = g_Game.m_BrushesObjMgr.GetSize();
	int i;
	m_bCCSwitchAble = (bool)nxStream.readByte();
	m_InitCC = nxStream.readDword();
	//movement attribute
	count = nxStream.readDword();
	for (i = 0; i < count; i++)
	{
		NxU8 charRead = nxStream.readByte();
		if (charRead != 0)
		{
			CBrushesObj* pBrushObj = g_Game.m_BrushesObjMgr.GetBrushesObj(i);
			BrushObjAttr objAttr;
			objAttr.DeSerialize(nxStream);
			pBrushObj->AddBrushAttr(&objAttr);
		}
	}
	//actor birthplace
	count = nxStream.readDword();
	for (i = 0; i < count; i++)
	{
		Region region;
		nxStream.readBuffer(&region.aabb.Center, sizeof(A3DVECTOR3));
		nxStream.readBuffer(&region.aabb.Extents, sizeof(A3DVECTOR3));
		region.aabb.CompleteMinsMaxs();
		region.actorNum = nxStream.readDword();
		region.coordinate.SetEditType(CCoordinateDirection::EDIT_TYPE_SCALE);
		region.coordinate.SetPos(region.aabb.Center);
		m_vRegion.push_back(region);
	}
	return true;
}
bool SceneExtraConfig::Save(NxStream& nxStream)
{
	//assert(g_Game.m_BrushesObjMgr.GetSize() == g_Game.m_objMgr.SizeModels());

	nxStream.storeByte((NxU8)m_bCCSwitchAble);
	nxStream.storeDword(m_InitCC);
	//movement attribute
	int count = g_Game.m_BrushesObjMgr.GetSize();
	nxStream.storeDword(count);
	int i;
	for (i = 0; i < count; i++)
	{
		CBrushesObj* pBrushObj = g_Game.m_BrushesObjMgr.GetBrushesObj(i);
		nxStream.storeByte((NxU8)pBrushObj->IsDynObj());
		BrushObjAttr* pAttr = pBrushObj->GetBrushObjAttr();
		if (pAttr)
		{
			pAttr->Serialize(nxStream);
		}
	}
	//actor birthplace
	nxStream.storeDword(m_vRegion.size());
	for (i = 0; i < m_vRegion.size(); i++)
	{
		nxStream.storeBuffer(&m_vRegion[i].aabb.Center, sizeof(A3DVECTOR3));
		nxStream.storeBuffer(&m_vRegion[i].aabb.Extents, sizeof(A3DVECTOR3));
		nxStream.storeDword(m_vRegion[i].actorNum);
	}
	return true;
}

void SceneExtraConfig::Reset()
{
	m_vRegion.clear();
	m_pEditingRegion = NULL;
	m_bCCSwitchAble = true;
}

void SceneExtraConfig::Render(A3DEngine* pEngine)
{
	//render birthplace
	A3DWireCollector *pWC = pEngine->GetA3DWireCollector();
	ACString str;
	DWORD dwColor = 0xff00ff00;
	A3DVECTOR3 vScrenePos;
	A3DViewport* pView = g_Render.GetViewport();
	
	for (int i = 0; i < m_vRegion.size(); i++)
	{
		if (m_pEditingRegion == &m_vRegion[i])
			pWC->AddAABB(m_vRegion[i].aabb, 0xffffff00);
		else
			pWC->AddAABB(m_vRegion[i].aabb, dwColor);
		str.Format(_AL("actor num: %d"), m_vRegion[i].actorNum);
		pView->Transform(m_vRegion[i].aabb.Center, vScrenePos);
		g_Render.TextOut(vScrenePos.x, vScrenePos.y, str, dwColor);
	}
	if (m_pEditingRegion)
	{
		m_pEditingRegion->coordinate.Draw(*pWC);
	}
}

void SceneExtraConfig::OnMouseMove(int x, int y, unsigned int nFlags)
{
	if (m_pEditingRegion != NULL)
	{
		m_pEditingRegion->coordinate.OnMouseMove(x, y, nFlags);
		if (nFlags & MK_LBUTTON)
		{
			m_pEditingRegion->aabb.Extents *= m_pEditingRegion->coordinate.GetScale();
			if (m_pEditingRegion->aabb.Extents.x > 20)
				m_pEditingRegion->aabb.Extents = A3DVECTOR3(20, 20, 20);
			else if (m_pEditingRegion->aabb.Extents.x < 2)
				m_pEditingRegion->aabb.Extents = A3DVECTOR3(2, 2, 2);
			m_pEditingRegion->aabb.CompleteMinsMaxs();
		}
	}
}

void SceneExtraConfig::OnLButtonDown(int x, int y, unsigned int nFlags)
{
	A3DViewport* pView = g_Render.GetViewport();
	A3DVECTOR3 vDest((float)x, (float)y, 1.0f);
	pView->InvTransform(vDest, vDest);
	A3DVECTOR3 vStart = g_Render.GetCamera()->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;
	if (nFlags & MK_CONTROL)
	{
		//generate region
		A3DTerrain2 *pTerr = g_Game.GetTerrain();
		RAYTRACERT rt;
		if (pTerr->RayTrace(vStart, vDelta, 1, &rt))
		{
			vDest = rt.vHitPos;
			Region region;
			region.aabb.Center = vDest;
			region.aabb.Extents = 5;
			region.aabb.CompleteMinsMaxs();
			region.actorNum = 10;
			region.coordinate.SetEditType(CCoordinateDirection::EDIT_TYPE_SCALE);
			region.coordinate.SetPos(vDest);
			m_vRegion.push_back(region);
		}
	}
	else
	{
		//edit region
		A3DVECTOR3 vTempPos, vTempNormal;
		float fTempFrac;
		float fMinFraction = 1.1f;
		m_pEditingRegion = NULL;
		for (int i = 0; i < m_vRegion.size(); i++)
		{
			const A3DAABB aabb = m_vRegion[i].aabb;
			if (CLS_RayToAABB3(vStart, vDelta, aabb.Mins, aabb.Maxs, vTempPos, &fTempFrac, vTempNormal) &&
				fTempFrac < fMinFraction)
			{
				fMinFraction	= fTempFrac;
				m_pEditingRegion = &m_vRegion[i];
			}
		}

		if (m_pEditingRegion )
			m_pEditingRegion->coordinate.OnLButtonDown(x, y, nFlags);
	}
}

void SceneExtraConfig::OnLButtonUp(int x, int y, unsigned int nFlags)
{
	
}

void SceneExtraConfig::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_ADD:
		if (m_pEditingRegion)
		{
			m_pEditingRegion->actorNum++;
		}
		break;
	case VK_SUBTRACT:
		if (m_pEditingRegion)
		{
			m_pEditingRegion->actorNum--;
		}
		break;
	case VK_DELETE:
		{
			if (m_pEditingRegion)
			{
				for (int i = 0; i < m_vRegion.size(); i++)
				{
					if (&m_vRegion[i] == m_pEditingRegion)
					{
						m_vRegion.erase(m_vRegion.begin() + i);
						m_pEditingRegion = NULL;
						break;
					}
				}
			}
			
		}
		break;
	}
}