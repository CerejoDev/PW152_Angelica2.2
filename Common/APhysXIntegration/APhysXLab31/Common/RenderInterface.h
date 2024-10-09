/*----------------------------------------------------------------------
	
	Descption: 

		To reconstruct and abstract the render routines for APhysXlab3, 
	a very light-weight interface for rendering...

	Created by He Wenfeng, Feb. 21, 2011

----------------------------------------------------------------------*/

#pragma once

#ifndef _RENDERINTERFACE_H_
#define _RENDERINTERFACE_H_

#include <AArray.h>

// interface for rendering routine...

class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DWireCollector;
class A3DFlatCollector;

class IRenderUtility
{
public:
	virtual	~IRenderUtility() {}

	virtual A3DEngine* GetA3DEngine() const = 0;
	virtual A3DDevice* GetA3DDevice() const = 0;
	virtual A3DCamera* GetCamera() const = 0;
	virtual A3DViewport* GetViewport() const = 0;
	virtual A3DWireCollector* GetWireCollector() const = 0;
	virtual A3DFlatCollector* GetFlatCollector() const = 0;
	virtual void TextOut(int x, int y, const ACHAR* szText, int iLen, DWORD color) const = 0;
};

class A3DShadowCaster;
class A3DViewport;

class IRenderable
{
public:
	virtual	~IRenderable() {}

	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck = true) const = 0;
	virtual void RenderExtraData(const IRenderUtility& renderUtil) const = 0;

	virtual void RegisterShadowCasters(APtrArray<A3DShadowCaster*>& aCasters);
	virtual void RenderShadowMap(A3DViewport* pViewport, int iSlice);

public:
	void RenderLPP(A3DViewport* pViewport) const { Render(pViewport); }

protected:
	virtual A3DShadowCaster* GetShadowCaster() { return 0; }

private:
	friend class IRenderManager;
};

class IRenderManager : public IRenderable
{
public:
	virtual ~IRenderManager() = 0 {};

	// override functions...
	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck = true) const;
	virtual void RenderExtraData(const IRenderUtility& renderUtil) const;
	virtual void RegisterShadowCasters(APtrArray<A3DShadowCaster*>& aCasters);
	virtual void RenderShadowMap(A3DViewport* pViewport, int iSlice);

public:
	// some management functions...
	virtual void RegisterRenderable(IRenderable* pRenderable);
	virtual void UnregisterRenderable(IRenderable* pRenderable);
	virtual void ClearRenderableStuff();

protected:
	APtrArray<IRenderable*> m_arrRenderableStuff;
};

#endif