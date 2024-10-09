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

#include "Render.h"

#ifdef _ANGELICA3
	#include <A3DGfxEngine.h>
	#include <AFI.h>
#endif

class CRender; 
class A3DEngine;
class A3DDevice;
class A3DGFXExMan;
class IRoleActor;
class CPhysXObjDynamic;

class IRoleCallback
{
public:
	virtual ~IRoleCallback() {}
//	virtual bool CreateMARole() = 0;
	virtual IRoleActor* CreateNPCRole(CPhysXObjDynamic& dynObj) = 0;
};

class IEngineUtility
{
public:
	virtual	~IEngineUtility() {}

	virtual CRender* GetCRender() const { return 0; }
	virtual A3DEngine* GetA3DEngine() const = 0;
	virtual A3DDevice* GetA3DDevice() const = 0;
	virtual A3DGFXExMan* GetA3DGfxExMan() const = 0;
#ifdef _ANGELICA3
	virtual A3DGfxEngine* GetGFXEngine() const = 0;
	virtual GFXPhysXSceneShell* GetGFXPhysXSceneShell() = 0;
#endif
};

class LoadPackage : public IEngineUtility
{
public:
	LoadPackage() : m_pSource(0) {}
	void SetRender(CRender& render) { m_pSource = &render; }

	virtual CRender* GetCRender() const { return m_pSource; }
	virtual A3DEngine* GetA3DEngine() const { return (0 == m_pSource)? 0 : m_pSource->GetA3DEngine(); }
	virtual A3DDevice* GetA3DDevice() const { return (0 == m_pSource)? 0 : m_pSource->GetA3DDevice(); }
	virtual A3DGFXExMan* GetA3DGfxExMan() const
	{
#ifdef _ANGELICA3
		return m_pSource->GetGFXEngine()->GetA3DGfxExMan();
#else
		return AfxGetGFXExMan();
#endif
	}
#ifdef _ANGELICA3
	virtual A3DGfxEngine* GetGFXEngine() const { return (0 == m_pSource)? 0 : m_pSource->GetGFXEngine(); }
	virtual GFXPhysXSceneShell* GetGFXPhysXSceneShell() { return &m_GfxPSS; }
#endif


private:
	CRender* m_pSource;
#ifdef _ANGELICA3
	GFXPhysXSceneShell m_GfxPSS;
#endif
};

static bool GetTerrainHeight(A3DTerrain2* pT, const A3DVECTOR3& pos, float& outHeight, A3DVECTOR3* poutNormal = 0)
{
	if (0 == pT)
		return false;
#ifdef _ANGELICA3
		return pT->GetPosHeight(pos, outHeight, poutNormal);
#else
		outHeight = pT->GetPosHeight(pos, poutNormal);
		return true;
#endif
}

static bool IsFileExist(const char * szFileName, bool bCheckPackage)
{
#ifdef _ANGELICA3
	return af_IsFileExist(szFileName, bCheckPackage);
#else
	return af_IsFileExist(szFileName);
#endif
}

static void Mat44_SetRotatePart(const A3DMATRIX3& source, A3DMATRIX4& to)
{
#ifdef _ANGELICA3
	to.SetRotatePart(source);
#else
	A3DMATRIX3& from = const_cast<A3DMATRIX3&>(source);
	to.SetRow(0, from.GetRow(0));
	to.SetRow(1, from.GetRow(1));
	to.SetRow(2, from.GetRow(2));
#endif
}

static void Mat44_SetRotatePart(const A3DMATRIX4& source, A3DMATRIX4& to)
{
#ifdef _ANGELICA3
	to.SetRotatePart(source.GetRotatePart());
#else
	A3DMATRIX4& from = const_cast<A3DMATRIX4&>(source);
	A3DMATRIX3 rot;
	A3DVECTOR3 temp;
	temp = source.GetRow(0);
	rot.m[0][0] = temp.x;
	rot.m[0][1] = temp.y;
	rot.m[0][2] = temp.z;
	temp = source.GetRow(1);
	rot.m[1][0] = temp.x;
	rot.m[1][1] = temp.y;
	rot.m[1][2] = temp.z;
	temp = source.GetRow(2);
	rot.m[2][0] = temp.x;
	rot.m[2][1] = temp.y;
	rot.m[2][2] = temp.z;
	Mat44_SetRotatePart(rot, to);
#endif
}

static void Mat44_GetRotatePart(const A3DMATRIX4& source, A3DMATRIX3& to)
{
#ifdef _ANGELICA3
	to = source.GetRotatePart();
#else
	A3DVECTOR3 temp;
	temp = source.GetRow(0);
	to.m[0][0] = temp.x;
	to.m[0][1] = temp.y;
	to.m[0][2] = temp.z;
	temp = source.GetRow(1);
	to.m[1][0] = temp.x;
	to.m[1][1] = temp.y;
	to.m[1][2] = temp.z;
	temp = source.GetRow(2);
	to.m[2][0] = temp.x;
	to.m[2][1] = temp.y;
	to.m[2][2] = temp.z;
#endif
}

static void Mat44_SetTransPart(const A3DVECTOR3& source, A3DMATRIX4& to)
{
#ifdef _ANGELICA3
	to.SetTransPart(source);
#else
	to.SetRow(3, source);
#endif
}

static A3DVECTOR3 Mat44_GetTransPart(const A3DMATRIX4& source)
{
#ifdef _ANGELICA3
	return source.GetTransPart();
#endif
	A3DMATRIX4& from = const_cast<A3DMATRIX4&>(source);
	return from.GetRow(3); 
}

#endif