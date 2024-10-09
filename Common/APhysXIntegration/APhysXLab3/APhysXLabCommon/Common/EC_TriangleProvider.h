/*
 * FILE: EC_TriangleProvider.h
 *
 * DESCRIPTION: class for provide triangle
 *
 * CREATED BY: Guo Jie, 2012/8/22
 *
 * HISTORY: 
 *
 */

#pragma once

#include "A3DTypes.h"
#include "A3DGeometry.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkin;
class A3DMeshBase;
class A3DSkinModel;
class A3DLitModel;
class CECModel;
class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class TriangleProvider
//	
///////////////////////////////////////////////////////////////////////////

class TriangleProvider
{
public:
	enum TYPE
	{
		TYPE_INVALID	= -1,
		TYPE_ECM		= 0,	//	ECMTriProvider
		TYPE_LITMODEL,			//	LModelTriProvider

		TYPE_COUNT,
	};

public:
	TriangleProvider()
		: m_eType(TYPE_INVALID)
		, m_bEnableForUse(true)
		, m_bAddedToMan(false)
		//, m_bCalcSubSpaceRating(true)
	{ }
	virtual ~TriangleProvider(){ }

	TYPE GetType() const { return m_eType; }

	virtual int GetVertNum(int iSub) const { return 0; }
	virtual void GetVert(int iSub, int index, float& x, float& y, float& z) {  };
	virtual WORD* GetIndices(int iSub) { return NULL; }
	virtual int GetSubMeshNum() const { return 0; }
	virtual int GetFaceNum(int iSub) const { return 0; }
	virtual const A3DAABB& GetModelAABB() const 
	{
		static const A3DAABB _aabb(A3DVECTOR3(0.0f), A3DVECTOR3(0.0f));
		return _aabb; 
	}

	virtual bool IsEcmProvider() const { return false; }
	virtual A3DTexture* GetTexturePtr(int iSub) { return NULL; }

	//bool CanCalcSubSpaceRating() const { return m_bCalcSubSpaceRating; }
	//void SetCalcSubSpaceRating(bool b) { m_bCalcSubSpaceRating = b; }

	bool IsAddedToMan() const { return m_bAddedToMan; }
	void SetAddedToMan(bool b) { m_bAddedToMan = b; }

	bool IsEnableForUse() const { return m_bEnableForUse; }
	void EnableForUse(bool bEnable) { m_bEnableForUse = bEnable; }

protected:

	TYPE m_eType;

	bool m_bEnableForUse;
	bool m_bAddedToMan;
	//bool m_bCalcSubSpaceRating;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECMTriProvider
//	
///////////////////////////////////////////////////////////////////////////

class ECMTriProvider: public TriangleProvider
{
public:
	enum MESH_TYPE
	{
		MESHTYPE_RIGID	= 0,
		MESHTYPE_SKIN,

		MESHTYPE_COUNT,
	};

public:

	ECMTriProvider::ECMTriProvider()
		: m_pSkinModel(NULL)
		, m_eMeshType(MESHTYPE_RIGID)
		, m_ppVert(NULL)
		, m_iSkin(0)
		, m_iNumMesh(0)
	{
		m_eType = TYPE_ECM;
	}

	virtual ~ECMTriProvider();

	virtual int GetVertNum(int iSub) const;
	virtual void GetVert(int iSub, int index, float& x, float& y, float& z)
	{
		x = m_ppVert[iSub][index].x;
		y = m_ppVert[iSub][index].y;
		z = m_ppVert[iSub][index].z;
	}
	virtual WORD* GetIndices(int iSub);
	virtual int GetSubMeshNum() const;
	virtual int GetFaceNum(int iSub) const;
	virtual const A3DAABB& GetModelAABB() const;
	virtual A3DTexture* GetTexturePtr(int iSub);

	bool Init(CECModel* pECModel, MESH_TYPE eMeshType, int idxSkin = 0);
	void Release();

protected:

	A3DSkinModel* m_pSkinModel;
	MESH_TYPE m_eMeshType;

	A3DVECTOR3** m_ppVert;
	int m_iSkin;
	int m_iNumMesh;

	A3DAABB m_aabb;

protected:

	A3DSkin* GetSkin(int idxSkin) const;
	A3DMeshBase* GetMesh(A3DSkin* pSkin, int idxMesh) const;
	A3DMeshBase* GetSkinMesh(int idxSkin, int idxMesh) const { return GetMesh(GetSkin(idxSkin), idxMesh); }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class LModelTriProvider: Lit Model triangle provider
//	
///////////////////////////////////////////////////////////////////////////

class LModelTriProvider: public TriangleProvider
{
public:
	LModelTriProvider::LModelTriProvider()
		: m_pLitModel(NULL)
	{
		m_eType = TYPE_LITMODEL;
	}

	virtual ~LModelTriProvider(){}

	virtual int GetVertNum(int iSub) const;
	virtual void GetVert(int iSub, int index, float& x, float& y, float& z);
	virtual WORD* GetIndices(int iSub);
	virtual int GetSubMeshNum() const;
	virtual int GetFaceNum(int iSub) const;
	virtual const A3DAABB& GetModelAABB() const;
	virtual A3DTexture* GetTexturePtr(int iSub);

	bool Init(A3DLitModel* pLitModel)
	{
		if (!pLitModel)
			return false;

		m_pLitModel = pLitModel;
		return true;
	}

protected:
	A3DLitModel* m_pLitModel;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

