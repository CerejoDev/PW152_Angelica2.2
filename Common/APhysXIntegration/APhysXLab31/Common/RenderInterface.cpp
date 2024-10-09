
#include <A3DShadowMap.h>
#include "RenderInterface.h"

//---------------------------------------------------------------------------
// Implementation of IRenderable...

//	Register shadow caster
void IRenderable::RegisterShadowCasters(APtrArray<A3DShadowCaster*>& aCasters)
{
	A3DShadowCaster* pSC = GetShadowCaster();
	if (0 != pSC)
		aCasters.UniquelyAdd(pSC);
}

void IRenderable::RenderShadowMap(A3DViewport* pViewport, int iSlice)
{
	A3DShadowCaster* pSC = GetShadowCaster();
	if(0 != pSC)
	{
		DWORD dwSliceMask = 1 << iSlice;
		if(pSC->dwSlices & dwSliceMask)
			Render(pViewport, false);
	}
}

//---------------------------------------------------------------------------
// Implementation of IRenderManager...
void IRenderManager::Render(A3DViewport* pViewport, bool bDoVisCheck /*= true*/) const
{
	const int nCount = m_arrRenderableStuff.GetSize();
	for(int i = 0; i < nCount; ++i)
		m_arrRenderableStuff[i]->Render(pViewport, bDoVisCheck);
}

void IRenderManager::RenderExtraData(const IRenderUtility& renderUtil) const
{
	const int nCount = m_arrRenderableStuff.GetSize();
	for(int i = 0; i < nCount; ++i)
		m_arrRenderableStuff[i]->RenderExtraData(renderUtil);
}

void IRenderManager::RegisterShadowCasters(APtrArray<A3DShadowCaster*>& aCasters)
{
	const int nCount = m_arrRenderableStuff.GetSize();
	for(int i = 0; i < nCount; ++i)
		m_arrRenderableStuff[i]->RegisterShadowCasters(aCasters);
}

void IRenderManager::RenderShadowMap(A3DViewport* pViewport, int iSlice)
{
	const int nCount = m_arrRenderableStuff.GetSize();
	for(int i = 0; i < nCount; ++i)
		m_arrRenderableStuff[i]->RenderShadowMap(pViewport, iSlice);
}

// some management functions...
void IRenderManager::RegisterRenderable(IRenderable* pRenderable)
{
	m_arrRenderableStuff.UniquelyAdd(pRenderable);
}

void IRenderManager::UnregisterRenderable(IRenderable* pRenderable)
{
	int iIndex = m_arrRenderableStuff.Find(pRenderable);
	if(iIndex >= 0)
		m_arrRenderableStuff.RemoveAtQuickly(iIndex);
}

void IRenderManager::ClearRenderableStuff() 
{
	m_arrRenderableStuff.RemoveAll();
}
