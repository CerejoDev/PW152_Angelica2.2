//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTORREGION_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_)
#define AFX_ACTORREGION_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <A3DTypes.h>
#include <A3DGeometry.h>
#include <NxSimpleTypes.h>
#include <NxStream.h>
#include <vector.h>
#include <A3DEngine.h>
#include "CoordinateDirection.h"

class CGame;

//////////////////////////////////////////////////////////////////////////
//scene extra config to save and load
//////////////////////////////////////////////////////////////////////////
class SceneExtraConfig
{

	//actor birthplace
	struct Region{
		A3DAABB aabb;
		int actorNum;
		CCoordinateDirection coordinate;
	};
	abase::vector<Region> m_vRegion;//角色出生地信息
	int m_InitCC;//初始CC
	bool m_bCCSwitchAble;//是否可以切换CC
	
	Region* m_pEditingRegion;

	bool m_bLock;

	public:

		SceneExtraConfig();
		~SceneExtraConfig();
		//生成角色
		bool CreateRegionActors();
		//设置初始CC
		bool ApplyInitCC();

		//根据m_objMgr加载运动属性等extra config，确保调用该函数前已经调用m_objMgr.LoadDeserialize
		bool Load(const NxStream& nxStream);
		bool Save(NxStream& nxStream);

		//set to default according to g_Game.m_objMgr;
		bool InitDefault();

		void Reset();
		//
		void Render(A3DEngine* pEngine);

		bool IsCCSwitchAble() const { return m_bCCSwitchAble; }

		void OnMouseMove(int x, int y, unsigned int nFlags); 
		void OnLButtonDown(int x, int y, unsigned int nFlags); 
		void OnLButtonUp(int x, int y, unsigned int nFlags); 
		void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		int GetInitCC() const { return m_InitCC; }

		void Lock(bool bLock) { m_bLock = bLock; }
		bool IsLocked() const { return m_bLock; }

};
#endif