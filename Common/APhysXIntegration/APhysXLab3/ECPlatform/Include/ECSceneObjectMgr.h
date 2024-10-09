/*
* FILE: ECSceneObjectMgr.h
*
* DESCRIPTION: 
*
* CREATED BY: Yongdong Wang, 2009/09/22
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#pragma once

#include <ABaseDef.h>
#include <hashtab.h>
#include <AExpDecl.h>
#include <A3DVector.h>
#include <A3DTrace.h >
#include "ECCounter.h"
#include "CConvexHullDataSetManager.h"

class ECSceneObject;
class ECScnECModel;
class ECScnBoxArea;
class ECResLoader;
class A3DViewport;
class ECScene;
class ECScnCritterGroup;
class ECSceneObjectLoader;
class ECBrushMan;
class ECTriangleMan;
class ECScnLight;
class A3DOccluderMan;
class ECSceneRenderer;
class ECTraceInfo;
class ECScnBuilding;
class ECScnDynamic;
class ECBezier;
class ECScnMusicArea;

#define USING_BRUSH_MAN
#undef USING_BRUSH_MAN

using namespace CHBasedCD;
 
class _AEXP_DECL ECSceneObjectMgr
{
public:

	//	Entity in ECTRACERT
	enum
	{
		ECENT_NONE = 0,
		ECENT_TERRAIN,
		ECENT_BUILDING,
		ECENT_PLAYER,
		ECENT_NPC,
		ECENT_MATTER,
		ECENT_DYN_OBJ,
		ECENT_FOREST,
		ECENT_HOST,
		ECENT_CARRIER,
	};
	//	Trace result
	struct ECTRACERT
	{
		//	In
		A3DVECTOR3		vStart;			//	Start position
		A3DVECTOR3		vDelta;			//	Total delta

		//	Out
		int				iEntity;		//	Collision entity type
		int				iObjectID;		//	Object ID
		float			fFraction;		//	Trace faction
		A3DVECTOR3		vNormal;		//	Collision face normal
	};

	//	Ray trace information
	struct ECRAYTRACE
	{
		//	IN parameters
		A3DVECTOR3		vStart;			//	Start position
		A3DVECTOR3		vDelta;			//	Move delta
		int				iTraceObj;		//	Trace object
		DWORD			dwUserData;		//	User data
		DWORD			dwKeyStates;	//	Key states

		//	OUT parameters
		RAYTRACERT		TempTraceRt;	//	Temporary trace result
		RAYTRACERT*		pTraceRt;		//	Trace result
		ECTRACERT*		pECTraceRt;		//	Trace result
	};

	typedef abase::hashtab<ECSceneObject*,DWORD,abase::_hash_function> ObjectTable;
	
	typedef abase::hash_map<int,int> DynID2ObjID; // ¶¯Ì¬ÎïÆ·

public:	
	ECSceneObjectMgr();
	virtual ~ECSceneObjectMgr();

public:

	bool Init(ECScene* scene);
	ECSceneObject* GetScnObject(int type,DWORD id);
	//ECScnDynamic*  GetDynamicObj(int dynID);

	// In: A3DVECTOR v, 
	// Out: pDefBox, pCurBox
	void GetDefAndCurBoxArea(const A3DVECTOR3& v,ECScnBoxArea** pDefBox,ECScnBoxArea** pCurBox);
	void GetFirstDefBox(ECScnBoxArea** pDefBox);

#ifdef USING_BRUSH_MAN
	ECBrushMan* GetBrushMan() { return m_pBrushMan;}
#endif
	
	ECTriangleMan* GetTriangleMan() { return m_pTriangleMan;}

	ECScnLight* GetSceneLight(const A3DVECTOR3& vPos);
	ECBezier* GetBezier(unsigned int id);

	void AddScnObject(ECSceneObject* pObj);
	void ReleaseScnObject(int type,DWORD id);

	void TickBuilding(bool bTick,const A3DVECTOR3& vLoaderCenter,DWORD dt=0); //we just add static object into brush manager.

	//void RemoveBuildingBurshInfo(int id);
	void RemoveBuildingBurshInfo(ECScnBuilding* pBuilding);

	void ReleaseECBuildings(ECSceneObjectLoader* pLoader); // release scene object and resource by loader thread.
	void ReleaseECModels(ECSceneObjectLoader* pLoader);

	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, float * pFraction, A3DVECTOR3& vNormal);
	bool RayTrace(ECRAYTRACE* pTraceInfo);


	//added by yx. member of pInfo is invalidate unless the return value is true. 
    //NOTE: the ClipPlane's normal is already in world coordinate 
	bool Trace(ECTraceInfo* pInfo);//capsule, ray or aabb brush trace

	bool Tick(DWORD dt,const A3DVECTOR3& vLoadCenter);
	bool Tick_Audio(DWORD dt,const A3DVECTOR3& vLoadCenter);

	bool Render(A3DViewport* pViewport);
	bool RenderDebugInfo(A3DViewport* pViewport);
	//	Build scene renderer
	void BuildSceneRenderer(A3DViewport* pViewport, ECSceneRenderer* pRenderer);
	
	void Release();

	void ParseDebugCommand(const char* szCmd);

	void AddOccluderToMgr(A3DOccluderMan* pOccMan);

	CConvexHullDataSetManager& GetConvexHullManger() { return m_convexMan;}

	void GetCurMusicArea(const A3DVECTOR3& vCamera,ECScnMusicArea*& pCurArea);
	void GetVolumeByVolumeArea(const A3DVECTOR3& vCamera,ECScnMusicArea* pArea,float& fVolume_Music,float& fVolume_Audio,unsigned int uTimeDelta);

	ObjectTable& GetScnBuildingTab() { return m_ScnBuildingTable;}
	ObjectTable& GetScnECModelTab() { return m_ScnECModelTable;}

protected:
private:
	
	ObjectTable m_ScnECModelTable;
	ObjectTable m_ScnBoxAreaTable;
	ObjectTable m_ScnBezierTable;
	ObjectTable m_ScnDungeonTable;
	ObjectTable m_ScnBuildingTable;
	ObjectTable m_ScnCameraTable;
	ObjectTable m_ScnLightTable;
	ObjectTable m_ScnWaterAreaTable;
	ObjectTable m_ScnOccluderTable;

	ObjectTable m_ScnGfxTable;
	ObjectTable m_ScnCritterGroupTable;
	ObjectTable m_ScnRoadTable;
	ObjectTable m_ScnGroundDecalTable;

	ObjectTable m_ScnAudioTable;
	ObjectTable m_ScnReverbTable;
	ObjectTable m_ScnVoluemTable;
	ObjectTable m_ScnMusicAreaTable;
	ObjectTable m_ScnAEBoxTable;

	ECScene* m_pScene;

#ifdef USING_BRUSH_MAN
	ECBrushMan*	m_pBrushMan;
#endif
	
	ECTriangleMan* m_pTriangleMan;
	ECCounter	m_BrushManCnt;		//	Brush man time counter

	CConvexHullDataSetManager m_convexMan;

	ECScnMusicArea*		m_pCurMusicArea;
	// debug cmd type
	int iDebugCmdType;

};