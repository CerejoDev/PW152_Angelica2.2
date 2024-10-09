/*
* FILE: EC_TriangleProvider.cpp
*
* DESCRIPTION: class for provide triangle

* CREATED BY: Guo Jie, 2012/8/22
*
* HISTORY: 
*
*/

#pragma once
#include "stdafx.h"

#include "EC_TriangleProvider.h"

#include "EC_Model.h"
#include "A3DSkin.h"
#include "A3DSkinMesh.h"
#include "A3DRigidMesh.h"
#include "A3DLitModel.h"
#include "A3DBone.h"
#include "A3DSkeleton.h"
#include "A3DTexture.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement ECMTriProvider
//	
///////////////////////////////////////////////////////////////////////////


A3DSkin* ECMTriProvider::GetSkin(int idxSkin) const
{
	return m_pSkinModel->GetA3DSkin(m_iSkin);
}

A3DMeshBase* ECMTriProvider::GetMesh(A3DSkin* pSkin, int idxMesh) const
{
	if (!pSkin)
		return NULL;

	switch (m_eMeshType)
	{
	case MESHTYPE_RIGID:
		return pSkin->GetRigidMesh(idxMesh);
	case MESHTYPE_SKIN:
		return pSkin->GetSkinMesh(idxMesh);
	default:
		return NULL;
	}
}

int ECMTriProvider::GetVertNum(int iSub) const
{
	A3DMeshBase* pMesh = GetSkinMesh(m_iSkin, iSub);
	if (!pMesh)
		return 0;

	return pMesh->GetVertexNum();
}

WORD* ECMTriProvider::GetIndices(int iSub)
{
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(m_iSkin);
	if (!pSkin)
		return NULL;

	switch (m_eMeshType)
	{
	case MESHTYPE_RIGID:
		{
			A3DRigidMesh* pMesh = pSkin->GetRigidMesh(iSub);
			return pMesh ? pMesh->GetOriginIndexBuf() : NULL;
		}
	case MESHTYPE_SKIN:
		{
			A3DSkinMesh* pMesh = pSkin->GetSkinMesh(iSub);
			return pMesh ? pMesh->GetOriginIndexBuf() : NULL;
		}
	default:
		return NULL;
	}
}

int ECMTriProvider::GetSubMeshNum() const
{
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(m_iSkin);
	if (!pSkin)
		return 0;

	switch (m_eMeshType)
	{
	case MESHTYPE_RIGID:
		return pSkin->GetRigidMeshNum();
	case MESHTYPE_SKIN:
		return pSkin->GetSkinMeshNum();
	default:
		return 0;
	}
}

ECMTriProvider::~ECMTriProvider()
{
	Release();
}

void ECMTriProvider::Release()
{
	for (int i = 0; i < m_iNumMesh; i++)
	{
		delete[] m_ppVert[i];
	}
	delete[] m_ppVert;

	m_ppVert = NULL;
	m_pSkinModel = NULL;
	m_iNumMesh = 0;
	m_aabb.Clear();
}

bool ECMTriProvider::Init(CECModel* pECModel, MESH_TYPE eMeshType, int idxSkin /* = 0 */)
{
	Release();

	if (!pECModel)
		return false;

	A3DSkinModel* pSkinModel = pECModel->GetA3DSkinModel();
	if (!pSkinModel || pSkinModel->GetSkinNum() <= 0)
	{
		//assert(pSkinModel);
		return false;
	}

	m_pSkinModel = pSkinModel;
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return false;

	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(idxSkin);
	if (!pSkin)
	{
		//assert(!pSkin);
		return false;
	}

	switch (eMeshType)
	{
	case MESHTYPE_RIGID:
		m_iNumMesh = pSkin->GetRigidMeshNum();
		break;
	case MESHTYPE_SKIN:
		m_iNumMesh = pSkin->GetSkinMeshNum();
		break;
	default:
		ASSERT(0);
		return false;
	}

	if (m_iNumMesh == 0)
		return false;

	m_eMeshType = eMeshType;

	m_iSkin = idxSkin;
	m_ppVert = new A3DVECTOR3*[m_iNumMesh];

	A3DBone* pBone;
	A3DMATRIX4 mat;

	switch (eMeshType)
	{
	case MESHTYPE_RIGID:
		for (int i = 0; i < m_iNumMesh; i++)
		{
			A3DRigidMesh* pRigidMesh = pSkin->GetRigidMesh(i);
			pBone = pSkeleton->GetBone(pRigidMesh->GetBoneIndex());
			if (!pBone)
			{
				ASSERT(0);
				continue;
			}

			mat = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();
			int num = pRigidMesh->GetVertexNum();
			m_ppVert[i] = new A3DVECTOR3[num];
			RIGID_VERTEX* pVert = pRigidMesh->GetOriginVertexBuf(); 
			for (int j = 0; j < num; j++)
			{
				m_ppVert[i][j] = A3DVECTOR3(pVert[j].x, pVert[j].y, pVert[j].z) * mat;
				m_aabb.AddVertex(m_ppVert[i][j]);
			}
		}
		m_aabb.CompleteCenterExts();
		break;

	case MESHTYPE_SKIN:
	{
		int iBoneNum = pSkeleton->GetBoneNum();
		A3DMATRIX4* pBoneMats = new A3DMATRIX4[iBoneNum];
		for(int i=0; i<iBoneNum; i++)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			pBoneMats[i] = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();
		}

		for (int i = 0; i < m_iNumMesh; i++)
		{
			A3DSkinMesh* pSkinMesh = pSkin->GetSkinMesh(i);
			int iNumVert = pSkinMesh->GetVertexNum();
			int iNumIdx = pSkinMesh->GetIndexNum();

			A3DVECTOR3* pOutVert	= new A3DVECTOR3[iNumVert];
			A3DVECTOR3* pOutNormal	= new A3DVECTOR3[iNumVert];
			pSkinMesh->GetBlendedVertices(pSkin, pBoneMats, pOutVert, pOutNormal);

			m_ppVert[i] = new A3DVECTOR3[iNumVert];
			for (int vi = 0; vi < iNumVert; vi++)
			{
				m_ppVert[i][vi] = pOutVert[vi];
				m_aabb.AddVertex(m_ppVert[i][vi]);
			}

			delete[] pOutVert;
			delete[] pOutNormal;
		}

		delete[] pBoneMats;

		m_aabb.CompleteCenterExts();
		break;
	}
	}

	return true;
}

A3DTexture* ECMTriProvider::GetTexturePtr(int iSub)
{
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(m_iSkin);
	return GetMesh(pSkin, iSub)->GetTexturePtr(pSkin);
}

int ECMTriProvider::GetFaceNum(int iSub) const
{
	A3DMeshBase* pMesh = GetSkinMesh(m_iSkin, iSub);
	return pMesh ? pMesh->GetIndexNum() / 3 : 0;
}

const A3DAABB& ECMTriProvider::GetModelAABB() const
{
	return m_aabb;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Class LModelTriProvider: Lit Model triangle provider
//	
///////////////////////////////////////////////////////////////////////////

int LModelTriProvider::GetVertNum(int iSub) const
{
	assert(m_pLitModel);
	return m_pLitModel->GetMesh(iSub)->GetNumVerts();
}
void LModelTriProvider::GetVert(int iSub, int index, float& x, float& y, float& z)
{
	assert(m_pLitModel);
	short* pVert = m_pLitModel->GetMesh(iSub)->GetUpdateVertsPos();
	index *= 3;

	ConvertFromPackFloat(x, pVert[index]); 
	ConvertFromPackFloat(y, pVert[index + 1]); 
	ConvertFromPackFloat(z, pVert[index + 2]); 
}

WORD* LModelTriProvider::GetIndices(int iSub)
{
	assert(m_pLitModel);
	return m_pLitModel->GetMesh(iSub)->GetIndices();
}

int LModelTriProvider::GetSubMeshNum() const
{
	if (m_pLitModel)
	{
		return m_pLitModel->GetNumMeshes();
	}
	return 0;
}

int LModelTriProvider::GetFaceNum(int iSub) const 
{ 
	if (m_pLitModel)
	{
		return m_pLitModel->GetMesh(iSub)->GetNumFaces();
	}
	return 0; 
}

const A3DAABB& LModelTriProvider::GetModelAABB() const
{
	if (m_pLitModel)
		return m_pLitModel->GetModelAABB();

	return TriangleProvider::GetModelAABB();
}

A3DTexture* LModelTriProvider::GetTexturePtr(int iSub)
{
	return m_pLitModel->GetMesh(iSub)->GetTexturePtr();
}