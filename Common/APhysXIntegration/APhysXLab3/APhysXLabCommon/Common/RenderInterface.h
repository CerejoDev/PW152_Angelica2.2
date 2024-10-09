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
#include <A3DLitModel.h>
#include <A3DSkinModel.h>
#include <A3DGFXEx.h>
#include <EC_Model.h>
// interface for rendering routine...

class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DWireCollector;
class A3DFlatCollector;
class CPhysXObjForceField;
class CRegion;

//toupdate to be removed
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

class IRenderManager;

class IRenderable
{
public:
	virtual	~IRenderable() {}

	virtual void Register(IRenderManager* pManager) = 0;
	virtual void UnRegister(IRenderManager* pManager) = 0;	

};

class ExtraRenderable : public IRenderable
{
public:
	struct AABB
	{
		A3DAABB aabb;
		DWORD dwColor;
		AABB(){}
		AABB(const A3DAABB& _aabb, DWORD color) : aabb(_aabb), dwColor(color)
		{
		}
	};
	struct OBB
	{
		A3DOBB obb;
		DWORD dwColor;
		OBB(){}
		OBB(const A3DOBB& _obb, DWORD color) : obb(_obb), dwColor(color)
		{
		}
	};
	struct Sphere
	{
		A3DVECTOR3 vCenter;
		float radius;
		DWORD dwColor;
		A3DMATRIX4 mat;
		Sphere(){ mat.Identity(); }
		Sphere(const A3DVECTOR3& vPos, float r, DWORD color, const A3DMATRIX4* pMat) : vCenter(vPos), radius(r), dwColor(color)
		{
			if (0 == pMat)
				mat.Identity();
			else
				mat = *pMat;
		}
	};
	struct Capsule
	{
		A3DCAPSULE capsule;
		DWORD dwColor;
		A3DMATRIX4 mat;
		Capsule(){ mat.Identity(); }
		Capsule(const A3DCAPSULE& cap, DWORD color, const A3DMATRIX4* pMat) : capsule(cap), dwColor(color)
		{
			if (0 == pMat)
				mat.Identity();
			else
				mat = *pMat;
		}
	};
	struct Line
	{
		A3DVECTOR3 vStart;
		A3DVECTOR3 vEnd;
		DWORD dwColor;
		Line(){}
		Line(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD color) : vStart(v1), vEnd(v2), dwColor(color)
		{
		}
	};
	struct RenderData3D
	{
		A3DVECTOR3* aVerts;
		int iNumVert;
		WORD* aIndex;
		int iNumIdx;
		DWORD dwColor;
		RenderData3D(){}
		RenderData3D(const A3DVECTOR3* verts, int NumVert, const WORD* Index, int NumIndx, DWORD color)
		{
			aVerts = new A3DVECTOR3[NumVert];
			memcpy(aVerts, verts, NumVert * sizeof(A3DVECTOR3));
			iNumVert = NumVert;
			aIndex = new WORD[iNumIdx];
			memcpy(aIndex, Index, NumIndx * sizeof(WORD));
			iNumIdx = NumIndx;
			dwColor = color;
		}
		~RenderData3D()
		{
			delete aVerts;
			delete aIndex;
		}
	};
public:
	ExtraRenderable();
	virtual ~ExtraRenderable();
	virtual void Register(IRenderManager* pManager);
	virtual void UnRegister(IRenderManager* pManager);
	void AddAABB(const A3DAABB& aabb, DWORD dwcolor);
	void AddOBB(const A3DOBB& obb, DWORD dwcolor);
	void AddSphere(const A3DVECTOR3& vPos, float radius, DWORD dwcolor, const A3DMATRIX4* pMat = 0);
	void AddCapsule(const A3DCAPSULE& cap, DWORD dwcolor, const A3DMATRIX4* pMat = 0);
	void AddLine(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD dwcolor);
	void AddRenderData_3D(const A3DVECTOR3* aVerts, int iNumVert, const WORD* aIndices, int iNumIdx, DWORD dwcolor);
	void Clear();

	const AArray<AABB>& GetAABBArray() const { return m_arrAABB; }
	const AArray<OBB>& GetOBBArray() const { return m_arrOBB; }
	const AArray<Sphere>& GetSphereArray() const { return m_arrSphere; }
	const AArray<Capsule>& GetCapsuleArray() const { return m_arrCapsule; }
	const AArray<Line>& GetLineArray() const { return m_arrLine; }
	const APtrArray<RenderData3D*>& GetRenderDataArray() const { return m_arrRenderData3D; }

protected:
	AArray<AABB> m_arrAABB;
	AArray<OBB>  m_arrOBB;
	AArray<Sphere>  m_arrSphere;
	AArray<Capsule>  m_arrCapsule;
	AArray<Line>    m_arrLine;
	APtrArray<RenderData3D*> m_arrRenderData3D;
	DWORD           m_dwColor;
};

class IRenderManager
{
public:
	virtual ~IRenderManager() = 0 {};

	// override functions...
	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck = true) const = 0;

public:
	// some management functions...

	void RegisterLitModel(A3DLitModel* pModel);
	void UnRegisterLitModel(A3DLitModel* pModel);
	void ClearLitModel();

	void RegisterSkinModel(A3DSkinModel* pModel);
	void UnRegisterSkinModel(A3DSkinModel* pModel);
	void ClearSkinModel();

	void RegisterGFXEx(A3DGFXEx* pModel);
	void UnRegisterGFXEx(A3DGFXEx* pModel);
	void ClearGFXEx();

	void RegisterECModel(CECModel* pModel);
	void UnRegisterECModel(CECModel* pModel);
	void ClearECModel();

	void RegisterFF(CPhysXObjForceField* pFF);
	void UnRegisterFF(CPhysXObjForceField* pFF);
	void ClearFF();

	void RegisterRegion(CRegion* pRegion);
	void UnRegisterRegion(CRegion* pRegion);
	void ClearRegion();

	void RegisterWireRender(ExtraRenderable* pWireRenderable);
	void UnRegisterWireRender(ExtraRenderable* pWireRenderable);
	void ClearWireRender();

protected:
	APtrArray<A3DLitModel*> m_arrLitModel;
	APtrArray<A3DSkinModel*> m_arrSkinModel;
	APtrArray<A3DGFXEx*> m_arrGfx;
	APtrArray<CECModel*> m_arrECM;
	APtrArray<CPhysXObjForceField*> m_arrFF;
	APtrArray<CRegion*>  m_arrRgn;
	APtrArray<ExtraRenderable*> m_arrWireRender;
};

class LitModelRenderable : public IRenderable
{
public:
	LitModelRenderable();
	virtual void Register(IRenderManager* pManager);
	virtual void UnRegister(IRenderManager* pManager);
	void SetModelPtr(A3DLitModel* pModel); 
protected:
	A3DLitModel* m_pLitModel;
};

class SkinModelRenderable : public IRenderable
{
public:
	SkinModelRenderable();
	virtual void Register(IRenderManager* pManager);
	virtual void UnRegister(IRenderManager* pManager);
	void SetModelPtr(A3DSkinModel* pModel); 
protected:
	A3DSkinModel* m_pSkinModel;
};

class GFXRenderable : public IRenderable
{
public:
	GFXRenderable();
	virtual void Register(IRenderManager* pManager);
	virtual void UnRegister(IRenderManager* pManager);
	void SetModelPtr(A3DGFXEx* pModel); 
protected:
	A3DGFXEx* m_pGfx;
};

class ECMRenderable : public IRenderable
{
public:
	ECMRenderable();
	virtual void Register(IRenderManager* pManager);
	virtual void UnRegister(IRenderManager* pManager);
	void SetModelPtr(CECModel* pModel); 
protected:
	CECModel* m_pECM;
};

class FFRenderable : public IRenderable
{
public:
	FFRenderable();
	virtual void Register(IRenderManager* pManager);
	virtual void UnRegister(IRenderManager* pManager);
	void SetModelPtr(CPhysXObjForceField* pModel); 
protected:
	CPhysXObjForceField* m_pFF;
};

#endif