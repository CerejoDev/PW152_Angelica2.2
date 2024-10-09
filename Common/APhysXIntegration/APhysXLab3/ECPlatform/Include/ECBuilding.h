#ifndef _ECBUILDING_H_
#define _ECBUILDING_H_

#include <ABasedef.h>
#include <AExpDecl.h>
#include "EC_Model.h"
#include "ECBrushMan.h"
#include "ConvexHullDataSet.h"

class A3DLitModel;
class A3DDevice;
class A3DViewport;
class A3DVECTOR3;
class A3DAABB;
class A3DBSP;
class ECBitMapUtil;
class ECSceneModule;
class ECCameraPosState;
class ECScene;
class ECEntity;
class ECTraceInfo;
//class CConvexHullDataSet;

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECTriProvider
//	
///////////////////////////////////////////////////////////////////////////
class ECTriProvider
{
public:
	virtual A3DAABB GetModelAABB() = 0;
	virtual A3DLitModel* GetA3DLitModel() = 0;
};



///////////////////////////////////////////////////////////////////////////
//	
//	Encapsulate lit model
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECBuilding: public ECBrushProvider,public ECTriProvider
{
public:
	typedef abase::vector<CConvexHullData*> ConvexHullDataVector;
	typedef abase::vector<ConvexHullDataVector> ModelConvexHullArray;

public:
	ECBuilding();
	virtual ~ECBuilding();

	bool Init(A3DDevice* pDevice);
	bool Release();
	bool Load(const char* szFile); // 废弃
	bool Load2(const char* szFile,ECScene* pScene);

	bool Render(A3DViewport* pViewport, bool bCheckVis=true);

	bool RenderConvexHull(A3DViewport* pViewport);

	bool Tick(DWORD dt);

	A3DAABB GetModelAABB();
	A3DLitModel* GetA3DLitModel()		{ return m_pLitModel; }

	void SetTreeFlag(bool bTree) { m_bTree = bTree;}
	void SetReflectFlag(bool bReflectFlag) { m_bReflect = bReflectFlag;}
	void SetRefractionFlag(bool bRefraction) { m_bRefraction = bRefraction;}
	void SetShadowFlag(bool bShadow) { m_bShadow = bShadow;}

	void SetPos(const A3DVECTOR3& pos);
	void SetDirAndUp(const A3DVECTOR3& vDir,const A3DVECTOR3& vUp);
	// Set Collide Only Flag
	bool SetCollideOnly(bool bFlag);
	bool GetCollideOnly() { return m_bCollideOnly; }

	//member of pInfo is invalidate unless the return value is true;
	//NOTE: ClipPlane's normal is under local coordinate (m_pLitModel->GetAbsoluteTM())
	bool TraceWithBrush(ECTraceInfo* pInfo, bool bCheckCHFlags = true);

//	void ReleaseConvexHull();
	virtual int GetBrushCount(){ return (int)m_CDBrushes.size(); }
	virtual ECBaseBrush * GetBrush(int index) { return m_CDBrushes[index]; }
	
	//get convex hull aabb
	const A3DAABB& GetCHAABB() const { return m_CHAABB; }

	const AString& GetPxdFile() const { return m_strPxdFileName;}

private:
	A3DLitModel*			m_pLitModel;

	A3DAABB					m_CHAABB;//bound aabb of convex hull brush
	CConvexHullDataSet*		m_pCHDataSet;
//	vector<CCDBrush *>		m_CDBrushes;
	vector<ECBaseBrush *>		m_CDBrushes;
	ECEntity*               m_pEntity;//collision entity

	AString m_strPxdFileName;
	bool m_bCollideOnly;

	bool m_bShadow;
	bool m_bReflect;
	bool m_bRefraction;
	bool m_bTree;

	A3DDevice* m_pA3DDevice;

	ECScene*   m_pECScene;	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Encapsulate BSP
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECDungeon
{
public:
	ECDungeon();
	ECDungeon(ECSceneModule* pOwner);
	virtual ~ECDungeon();

	bool Load(const char* szFile);
	bool Release();

	bool SetupCullState(A3DCameraBase* pCamera, ECCameraPosState* pCullingState);

	void GetDungeonAABB(A3DVECTOR3& vMin,A3DVECTOR3& vMax);

	bool LeafIsVisiblebyIdx(int iLeafIdx);
	bool CanBeenSeenFromLeaf(const A3DVECTOR3& vPos,int idx);
	void ShiftPosition(const A3DVECTOR3& vOffset);

private:
	A3DBSP* m_pBSP;
	int* m_paPVSLeaves;	//	PVS leaves

	ECBitMapUtil* m_pBitMap; // Flag which indicate the visibility of bsp's leaves.
};

///////////////////////////////////////////////////////////////////////////
//	
//	Bit map utility class
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECBitMapUtil
{
public:
	ECBitMapUtil(int num);
	~ECBitMapUtil();

	void Reset(bool val); // reset each bit by val;
	void SetBitValue(int pos,bool val);
	bool GetBitValue(int pos);

private:
	BYTE* m_pByte;
	int m_iBitNum; // 总位数
	int m_iByteNum; // 总字节数
};

#endif