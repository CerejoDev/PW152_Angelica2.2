
#include "stdafx.h"

#include "RenderInterface.h"
#include <A3DLitModel.h>
#include <A3DSkinModel.h>
#include <A3DGFXEx.h>
#include <EC_Model.h>



//---------------------------------------------------------------------------
// Implementation of IRenderManager...
template<class T>
void Remove(APtrArray<T*>& arr, T* pElement)
{
	int iIndex = arr.Find(pElement);
	if(iIndex >= 0)
		arr.RemoveAtQuickly(iIndex);
}
void IRenderManager::RegisterLitModel(A3DLitModel* pModel)
{
	m_arrLitModel.UniquelyAdd(pModel);
}
void IRenderManager::UnRegisterLitModel(A3DLitModel* pModel)
{
	Remove(m_arrLitModel, pModel);
}
void IRenderManager::ClearLitModel()
{
	m_arrLitModel.RemoveAll();
}

void IRenderManager::RegisterSkinModel(A3DSkinModel* pModel)
{
	m_arrSkinModel.UniquelyAdd(pModel);
}
void IRenderManager::UnRegisterSkinModel(A3DSkinModel* pModel)
{
	Remove(m_arrSkinModel, pModel);
}
void IRenderManager::ClearSkinModel()
{
	m_arrSkinModel.RemoveAll();
}

void IRenderManager::RegisterGFXEx(A3DGFXEx* pModel)
{
	m_arrGfx.UniquelyAdd(pModel);
}
void IRenderManager::UnRegisterGFXEx(A3DGFXEx* pModel)
{
	Remove(m_arrGfx, pModel);
}
void IRenderManager::ClearGFXEx()
{
	m_arrGfx.RemoveAll();
}

void IRenderManager::RegisterECModel(CECModel* pModel)
{
	m_arrECM.UniquelyAdd(pModel);
}
void IRenderManager::UnRegisterECModel(CECModel* pModel)
{
	Remove(m_arrECM, pModel);
}
void IRenderManager::ClearECModel()
{
	m_arrECM.RemoveAll();
}

void IRenderManager::RegisterFF(CPhysXObjForceField* pFF)
{
	m_arrFF.UniquelyAdd(pFF);
}

void IRenderManager::UnRegisterFF(CPhysXObjForceField* pFF)
{
	Remove(m_arrFF, pFF);
}

void IRenderManager::ClearFF()
{
	m_arrFF.RemoveAll();
}

void IRenderManager::RegisterRegion(CRegion* pRegion)
{
	m_arrRgn.UniquelyAdd(pRegion);
}

void IRenderManager::UnRegisterRegion(CRegion* pRegion)
{
	Remove(m_arrRgn, pRegion);
}

void IRenderManager::ClearRegion()
{
	m_arrRgn.RemoveAll();
}

void IRenderManager::RegisterWireRender(ExtraRenderable* pWireRenderable)
{
	m_arrWireRender.UniquelyAdd(pWireRenderable);
}
void IRenderManager::UnRegisterWireRender(ExtraRenderable* pWireRenderable)
{
	Remove(m_arrWireRender, pWireRenderable);
}
void IRenderManager::ClearWireRender()
{
	m_arrWireRender.RemoveAll();
}

//---------------------------------------------------------------------------
// Implementation of LitModelRenderable...
LitModelRenderable::LitModelRenderable() : m_pLitModel(NULL)
{
}
void LitModelRenderable::Register(IRenderManager* pManager)
{
	if (NULL == m_pLitModel)
		return;
	pManager->RegisterLitModel(m_pLitModel);
}
void LitModelRenderable::UnRegister(IRenderManager* pManager)
{
	if (NULL == m_pLitModel)
		return;
	pManager->UnRegisterLitModel(m_pLitModel);
	SetModelPtr(0);
}
void LitModelRenderable::SetModelPtr(A3DLitModel* pModel)
{
	m_pLitModel = pModel;
}

//---------------------------------------------------------------------------
// Implementation of SkinModelRenderable...
SkinModelRenderable::SkinModelRenderable() : m_pSkinModel(NULL)
{
}
void SkinModelRenderable::Register(IRenderManager* pManager)
{
	if (NULL == m_pSkinModel)
		return;
	pManager->RegisterSkinModel(m_pSkinModel);
}
void SkinModelRenderable::UnRegister(IRenderManager* pManager)
{
	if (NULL == m_pSkinModel)
		return;
	pManager->UnRegisterSkinModel(m_pSkinModel);
	SetModelPtr(0);
}
void SkinModelRenderable::SetModelPtr(A3DSkinModel* pModel)
{
	m_pSkinModel = pModel;
}

//---------------------------------------------------------------------------
// Implementation of GFXRenderable...
GFXRenderable::GFXRenderable() : m_pGfx(NULL)
{
}
void GFXRenderable::Register(IRenderManager* pManager)
{
	if (NULL == m_pGfx)
		return;
	pManager->RegisterGFXEx(m_pGfx);
}
void GFXRenderable::UnRegister(IRenderManager* pManager)
{
	if (NULL == m_pGfx)
		return;
	pManager->UnRegisterGFXEx(m_pGfx);
	SetModelPtr(0);
}
void GFXRenderable::SetModelPtr(A3DGFXEx* pModel)
{
	m_pGfx = pModel;
}

//---------------------------------------------------------------------------
// Implementation of ECMRenderable...
ECMRenderable::ECMRenderable() : m_pECM(NULL)
{
}
void ECMRenderable::Register(IRenderManager* pManager)
{
	if (NULL == m_pECM)
		return;
	pManager->RegisterECModel(m_pECM);
}
void ECMRenderable::UnRegister(IRenderManager* pManager)
{
	if (NULL == m_pECM)
		return;
	pManager->UnRegisterECModel(m_pECM);
	SetModelPtr(0);
}
void ECMRenderable::SetModelPtr(CECModel* pModel)
{
	m_pECM = pModel;
}

//---------------------------------------------------------------------------
// Implementation of FFRenderable...
FFRenderable::FFRenderable() : m_pFF(NULL)
{
}
void FFRenderable::Register(IRenderManager* pManager)
{
	if (NULL == m_pFF)
		return;
	pManager->RegisterFF(m_pFF);
}
void FFRenderable::UnRegister(IRenderManager* pManager)
{
	if (NULL == m_pFF)
		return;
	pManager->UnRegisterFF(m_pFF);
	SetModelPtr(0);
}
void FFRenderable::SetModelPtr(CPhysXObjForceField* pModel)
{
	m_pFF = pModel;
}

//---------------------------------------------------------------------------
// Implementation of WireRenderable...
template<class T>
void ReleseAPtrArray(APtrArray<T*> &arr)
{
	for (int i = 0; i < arr.GetSize(); i++)
	{
		delete arr[i];
	}
}
ExtraRenderable::ExtraRenderable()
{
}
ExtraRenderable::~ExtraRenderable()
{
	Clear();
}
void ExtraRenderable::Register(IRenderManager* pManager)
{
	pManager->RegisterWireRender(this);
}
void ExtraRenderable::UnRegister(IRenderManager* pManager)
{
	pManager->UnRegisterWireRender(this);
}
void ExtraRenderable::AddAABB(const A3DAABB& aabb, DWORD dwcolor)
{
	m_arrAABB.Add(AABB(aabb, dwcolor));
}
void ExtraRenderable::AddOBB(const A3DOBB& obb, DWORD dwcolor)
{
	m_arrOBB.Add(OBB(obb, dwcolor));
}
void ExtraRenderable::AddSphere(const A3DVECTOR3& vPos, float radius, DWORD dwcolor, const A3DMATRIX4* pMat)
{
	m_arrSphere.Add(Sphere(vPos, radius, dwcolor, pMat));
}
void ExtraRenderable::AddCapsule(const A3DCAPSULE& cap, DWORD dwcolor, const A3DMATRIX4* pMat)
{
	m_arrCapsule.Add(Capsule(cap, dwcolor, pMat));
}
void ExtraRenderable::AddLine(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD dwcolor)
{
	m_arrLine.Add(Line(v1, v2, dwcolor));
}
void ExtraRenderable::AddRenderData_3D(const A3DVECTOR3* aVerts, int iNumVert, const WORD* aIndices, int iNumIdx, DWORD dwcolor)
{
	RenderData3D *pData = new RenderData3D(aVerts, iNumVert, aIndices, iNumIdx, dwcolor);
	m_arrRenderData3D.Add(pData);
}
void ExtraRenderable::Clear()
{
	m_arrAABB.RemoveAll(false);
	m_arrOBB.RemoveAll(false);
	m_arrSphere.RemoveAll(false);
	m_arrCapsule.RemoveAll(false);
	m_arrLine.RemoveAll(false);
	ReleseAPtrArray(m_arrRenderData3D);
	m_arrRenderData3D.RemoveAll(false);
	m_arrRenderData3D.RemoveAll(false);
}