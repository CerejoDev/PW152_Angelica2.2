/*
* FILE: EngineInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/03/04
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSXLAB_IENGINE_H_
#define _APHYSXLAB_IENGINE_H_

class A3DEngine;
class A3DDevice;

class IEngineUtility
{
public:
	virtual	~IEngineUtility() {}

	virtual A3DEngine* GetA3DEngine() const = 0;
	virtual A3DDevice* GetA3DDevice() const = 0;
	virtual A3DGfxEngine* GetGFXEngine() const = 0;
	virtual GFXPhysXSceneShell* GetGFXPhysXSceneShell() = 0;
	virtual CRender* GetCRender() const { return 0; }
};

class LoadPackage : public IEngineUtility
{
public:
	LoadPackage() : m_pSource(0) {}
	void SetRender(CRender& render) { m_pSource = &render; }

	virtual A3DEngine* GetA3DEngine() const { return (0 == m_pSource)? 0 : m_pSource->GetA3DEngine(); }
	virtual A3DDevice* GetA3DDevice() const { return (0 == m_pSource)? 0 : m_pSource->GetA3DDevice(); }
	virtual A3DGfxEngine* GetGFXEngine() const { return (0 == m_pSource)? 0 : m_pSource->GetGFXEngine(); }
	virtual GFXPhysXSceneShell* GetGFXPhysXSceneShell() { return &m_GfxPSS; }
	virtual CRender* GetCRender() const { return m_pSource; }

private:
	CRender* m_pSource;
	GFXPhysXSceneShell m_GfxPSS;
};

#endif