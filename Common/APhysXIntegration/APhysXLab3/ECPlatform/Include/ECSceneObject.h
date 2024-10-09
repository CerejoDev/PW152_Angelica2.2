/*
 * FILE: ECSceneObject.h
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

#include <AString.h>
#include <AExpDecl.h>
#include <ABaseDef.h>
#include <A3DVector.h>
#include <hashtab.h>
#include <A3DOccluder.h>
#include <A3DTypes.h>
#include <A3DColorValue.h>
#include <AAssist.h>
#include <AArray.h>
#include "ECSceneFile.h"
#include "ECCounter.h"

class A3DViewport;
class AString;
class CECModel;
class ECModelWithBrush;
class ECAutonomousGroup;
class ECAutonomousBehaviour;
class ECScene;
class ECBuilding;
class ECDungeon;
class ECScnDungeon;
class A3DAABB;
class ECSceneModule;
class A3DWaterArea;
class A3DTerrainWater;
class AFile;
class ECBezierWalker;
class ECCameraPosState;
class ECShadowRender;
class A3DShadowCaster;
class A3DCameraBase;
class A3DGFXEx;
class A3DTerrainRoad;
class A3DTerrainGroundDecal;
///////////////////////////////////////////////////////////////////////////
//	
//	Base class for scene object	
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECSceneObject
{
public:

	ECSceneObject(int iType) { m_iType = iType; m_dwObjectID = 0; m_bAddedToBrushMan = false;m_bAddedToTriangleMan = false;}
	virtual ~ECSceneObject() {}

public:
	int GetType() { return m_iType; }
	virtual DWORD GetObjectID() const {return m_dwObjectID;}
	virtual void SetObjectID(DWORD dwID) {m_dwObjectID = dwID;}
	virtual bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter){ return true;};
	virtual bool Render(A3DViewport* pViewport) {return true;}
	virtual void Release(){};// release local data. not including ECBuilding, dungeon, ecmodel. These need to be released by LoaderThread!
	virtual void ReadFileNameFromMem(AString& strName,int*& pData);
	virtual void RegisterShadowCasters(ECShadowRender* pShadowRender, DWORD dwMask, int iGenStep) {}
	//	Render objects onto shadow map
	virtual bool RenderOntoShadowMap(A3DViewport* pViewport, int iSlice, int iGenStep) { return true; }

	void SetAddedToBrushManFlag(bool flag) { m_bAddedToBrushMan = flag;}
	bool GetAddedToBrushManFlag() { return m_bAddedToBrushMan;}
	void SetAddedToTriangleManFlag(bool flag) { m_bAddedToTriangleMan = flag;}
	bool GetAddedToTriangleManFlag() { return m_bAddedToTriangleMan;}
	
	virtual bool IsLoaded() { return true;} //  whether is resource loaed by loader thread; 
protected:
	int		m_iType;		//	Object type
	DWORD	m_dwObjectID;   //	object ID

	bool m_bAddedToBrushMan; //
	bool m_bAddedToTriangleMan;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for CECModel.
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnECModel: public ECSceneObject 
{
public:
	ECScnECModel();
	virtual ~ECScnECModel();

	CECModel* GetECModel();
	ECModelWithBrush* GetBrushECModel() { return m_pBrushECModel;}

	A3DVECTOR3 GetPos() { return m_vPos;}
	const char* GetModelFilePath() { return m_strModelPath;}
	const char* GetActionFileName(){ return m_strActionName;}

	bool SyncECModel(CECModel* pModel);

	//	Load data from memory
	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	bool Tick(DWORD dt,const A3DVECTOR3& vLoadCenter);
	bool Render(A3DViewport* pViewport) ;

	bool IsLoaded();
	void Release();

protected:
private:
//	CECModel* m_pECModel;// this will be release by loader thread.

	ECModelWithBrush* m_pBrushECModel;

	A3DVECTOR3	m_vPos;
	A3DVECTOR3	m_vDir;
	A3DVECTOR3	m_vUp;

	AString m_strModelPath;
	AString m_strActionName;
	
	int m_iUsage;			// 用途：0用于场景；1用于轨迹动画
};

///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for ECBuilding.
//	
///////////////////////////////////////////////////////////////////////////
class ECScnBuildingLoader;

class _AEXP_DECL ECScnBuilding: public ECSceneObject
{
	typedef abase::vector<int> LeafArray;
public:
	ECScnBuilding();
	virtual ~ECScnBuilding(){}

	ECBuilding* GetBuilding() { return m_pBuilding;}
	AString GetModelFilePath() { return m_strModelPath + m_strModelFileExt;}
	bool GetLODModelFilePath(bool bExact, AString& fileName);
	const char* GetDayLightMapPath() { return m_strDayLightMapPath;}
	const char* GetNightLightMapPath() {return m_strNightLightMapPath;}
	unsigned int GetDungeonID() { return m_uiDungeonID; }
	bool BuildingIsVisible(ECScnDungeon* pScnDungeon);

	virtual void RegisterShadowCasters(ECShadowRender* pShadowRender, DWORD dwMask, int iGenStep);
	//	Render objects onto shadow map
	virtual bool RenderOntoShadowMap(A3DViewport* pViewport, int iSlice, int iGenStep);

	void GetModelAABB(A3DAABB& aabb);
	bool SyncBuilding(ECBuilding* pBuilding);

	bool Load(const void* pData, int iDataLen, DWORD dwVersion,ECScnBuildingLoader* pLoader, const A3DVECTOR3& vLoadCenter);
	bool Render(A3DViewport* pViewport) ;
	bool RenderConvexHull(A3DViewport* pViewport);

	bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);

	bool IsLoaded() { return m_pBuilding != NULL;}
	void Release();

	int GetLODLevel() const { return m_iLodLevel;}
	bool ComputeLODLevel(const A3DVECTOR3& vPos);

	float GetDist_LOD1() { return m_aLODDistance[0];}
	float GetDist_LOD2() { return m_aLODDistance[1];}

private:
	ECScnBuildingLoader* m_pScnBuildingLoader;
	ECCounter	m_LodCounter;

	ECBuilding* m_pBuilding; // this will be release by loader thread.

	A3DVECTOR3 m_vPos;
	A3DVECTOR3 m_vDir;
	A3DVECTOR3 m_vUp;

	bool m_bShadow;
	bool m_bReflect;
	bool m_bRefraction;
	bool m_bTree;

	int m_iLodLevel; // 0,1,2 . 0 is highest-resolution model.
	int m_iActLodLevel; // actual lod model

	float m_aLODDistance[2];   // LOD显示距离

	AString m_strModelFileExt;
	AString m_strModelPath; // not including ext
	AString m_strDayLightMapPath;
	AString m_strNightLightMapPath;

	unsigned int m_uiDungeonID;
	LeafArray m_leafArray; 

	A3DShadowCaster*	m_pShadowCaster;	//	Shadow caster info
	int					m_iShadowGen;		//	Shadow generate type, see ECShadowRender::STEP_xxx
};

///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for ECDungeon.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnDungeon: public ECSceneObject
{
	//typedef abase::vector<int> BuildingArray;
	//typedef abase::hashtab<BuildingArray*,int,abase::_hash_function> LeaveTable;

public:
	ECScnDungeon(ECSceneModule* pOwner);
	virtual ~ECScnDungeon();

	const char* GetModelFilePath() { return m_strBSPPath; }
	ECDungeon* GetDungeon() { return m_pDungeon; }
	const A3DVECTOR3& GetPos() { return m_vPos;}
	bool SyncDungeon(ECDungeon* pDungeon);
	bool LeafIsVisible(int leafIdx);
	//	Load data from memory
	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	void Release();
	bool SetupCullState(A3DCameraBase* pCamera, ECCameraPosState* pCullingState);

	//void SetSceneModule(ECSceneModule* pSceneModule) { m_pSceneModule = pSceneModule;}

	bool IsLoaded() { return m_pDungeon!=NULL;}

	float GetDist_LOD1() { return m_aLODDistance[0];}
	float GetDist_LOD2() { return m_aLODDistance[1];}

protected:
private:

	ECDungeon* m_pDungeon;// this will be release by loader thread.

	A3DVECTOR3 m_vPos;

	AString m_strBSPPath;

	ECSceneModule* m_pSceneModule;

	float m_aLODDistance[2];   // LOD显示距离

	//int m_iLeaveNum;
	//LeaveTable m_LeavesTable;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for ECScnCamera.
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnCamera:public ECSceneObject
{
public:
	ECScnCamera();
	~ECScnCamera();

	bool Load(void* pData, int iDataLen,DWORD dwVersion);
	void Release();
protected:
private:
	ECCAMERAFILE m_data;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for ECScnLight.
//	
///////////////////////////////////////////////////////////////////////////
class A3DWaveFunc;

class _AEXP_DECL ECScnLight: public ECSceneObject
{
public:
	ECScnLight();
	ECScnLight(const ECScnLight& v);
	virtual ~ECScnLight();

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	bool Render(A3DViewport* pViewport);
	void Release(){}

	const A3DVECTOR3& GetPosition() const { return m_vPos;}
	const A3DVECTOR3& GetDir() const { return m_vDir;}
	const A3DVECTOR3& GetUp() const { return m_vUp;}
	int GetLightType() const { return m_nType;} // 0:点光源, 1: not used, 2:平行光, 3:聚光灯
	float GetRange();
	A3DCOLORVALUE GetColor();
	float GetFallOff() const {return m_fFalloff;}
	float GetAttenuation0() const {return m_fAttenuation0;}
	float GetAttenuation1() const {return m_fAttenuation1;}
	float GetAttenuation2() const {return m_fAttenuation2;}
	float GetTheta()const { return m_fTheta; }
	float GetPhi() const { return m_fPhi; }
	int	  GetEffectType() const { return m_nEffectType; }
	bool IsDynamic() const { return m_bDynamic; }
	float GetSpecFactor() const { return m_fSpecFactor; }

//	void CreateFromLightParam(A3DLIGHTPARAM* lightParam);
	void GenerateA3DLightParam(A3DLIGHTPARAM& param);

	const ECScnLight& operator=(const ECScnLight& pLight);

	bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);
	bool IsWaveFuncEnabled() const;

protected:

	A3DCOLORVALUE GetWaveColor(const A3DCOLORVALUE& clr);
	float GetWaveRange(float range);

private:

	//ECLIGHTFILE5	m_data;		//	Original data
	//ECLIGHTFILE6	m_data;
	A3DVECTOR3	m_vPos;
	A3DVECTOR3	m_vDir;
	A3DVECTOR3	m_vUp;
	int         m_nType;			// 0:点光源   1:平行光    2:聚光灯
	float       m_fRange;			// 光线范围
	DWORD       m_dwLightColor;	// 散射光
	float       m_fFalloff;		// 发散系数
	float       m_fAttenuation0;  // 衰减0
	float       m_fAttenuation1;  // 衰减1
	float       m_fAttenuation2;  // 衰减2
	float       m_fTheta;			// 聚光灯内角
	float       m_fPhi;			// 聚光灯外角
	int         m_nEffectType;	//	灯光影响
	bool		m_bDynamic;		// 是否动态光源
	float		m_fSpecFactor;	// 高光强度
	int			m_iWaveType;		//动态光波动类型
	int			m_iWavePeriod;		//动态光波动周期
	float		m_fRangeAmplitude;	//动态光范围波动系数 delta = m_fRangeAmplitude* m_fRange
	float		m_fPowerAmplitude;  //动态光颜色波动系数 delta = m_fPowerAmplitude *  m_dwLightColor;

	A3DWaveFunc*	m_pWaveFunc; // wave function

	bool			m_bUseWaveFunc;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for ECScnWaterArea.
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnWaterArea: public ECSceneObject
{
public:
	ECScnWaterArea();
	virtual ~ECScnWaterArea(){}

	bool Load(void* pData, int iDataLen, DWORD dwVersion){}
	bool Load(DWORD id,A3DTerrainWater* pTerrainWater,AFile* pFile);
	void Release();

	A3DWaterArea* GetWaterArea() { return m_pWaterArea;}

private:
	A3DWaterArea* m_pWaterArea;
	//A3DVECTOR3 m_vPos;

};
///////////////////////////////////////////////////////////////////////////
//	
//	Scene object for ECScnOccluder.
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnOccluder: public ECSceneObject,public A3DOccluder
{
public:
	ECScnOccluder();
	virtual ~ECScnOccluder(){}

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	void Release(){};

	bool Render(A3DViewport* pViewport);
	const A3DVECTOR3& GetPos() { return m_data.vPos;}
	const A3DVECTOR3& GetDir() { return m_data.vDir;}
	const A3DVECTOR3& GetUp() { return m_data.vUp;}
	float GetLength() {return m_data.fLength;}
	float GetHeight() { return m_data.fHeight;}

	void ConstructGeometry();
	//virtual void		Release();
	const virtual A3DVECTOR3*	GetVert();
	const virtual WORD*		GetIdx();
	virtual int			GetVertNum();
	virtual int			GetIdxNum();

private:
	ECOCCLUDERFILE	m_data;

	A3DVECTOR3		m_VertexArray[4]; // temp
	WORD			m_wIndexArray[6];
};

//////////////////////////////////////////////////////////////////////////
//
//  Gfx 
//
//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnGfx: public ECSceneObject
{
public:
	ECScnGfx();
	virtual ~ECScnGfx(){}

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	void Release();

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	const A3DVECTOR3& GetDir() const { return m_vDir;}
	const A3DVECTOR3& GetUp() const { return m_vUp;}
	float GetScale() { return m_fScale;}
	float GetAlpha() { return m_fAlpha;}
	bool  IsForceNoFade() { return m_bForceNoFade;}
	int	  GetUsage() { return m_iUsage;}

	virtual bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);
	virtual bool Render(A3DViewport* pViewport);

protected:

	void FadeGFX(bool bFadeIn);

private:
	
	A3DVECTOR3	m_vPos;	// 位置
	A3DVECTOR3	m_vDir;	// 朝向
	A3DVECTOR3	m_vUp;	// 上方向
	float		m_fScale;		// 缩放
	float		m_fAlpha;		// 透明度

	A3DCOLOR	m_dwOuterColor; //外部色彩，需要由客户端调用A3DGFXEx的接口进行设置SetOuterColor

	bool		m_bForceNoFade;// 强制GFX不根据距离淡出(忽略GFX自身属性，程序强行设置SetFadeByDist) 该值为true意味着客户端需要强设SetFadeByDist(false), 否则依照GFX文件自身的设定不去更改
	
	//int m_iPathLen;		// GFX资源路径字符串长度
	//char* m_szPath;		// GFX资源路径 (Relative path under Bin/Gfx/...)
	AString		m_strPath;
	int			m_iUsage;		// 用途：0用于场景；1用于轨迹动画

	//
	A3DGFXEx*	m_pGfx;

};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
/*
class _AEXP_DECL ECScnDynamic: public ECSceneObject
{
public:
	ECScnDynamic();
	virtual ~ECScnDynamic(){}

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	void Release(){};

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	const A3DVECTOR3& GetDir() const { return m_vDir;}
	const A3DVECTOR3& GetUp() const { return m_vUp;}

	unsigned int GetControllerID() const {return m_uiControllerID;}
	unsigned int GetDynamicID() const {return m_uiDynamicID;}
	unsigned int GetGeneratePointCount(){return m_vBindPos.size() ? m_vBindPos.size():1;}
	const A3DVECTOR3& GetGeneratePos(int index) 
	{
		if (m_vBindPos.size())
		{
			return m_vBindPos[index];
		}
		return m_vPos;
	}

	const char* GetResPath() {return m_strResPath;}

private:

	A3DVECTOR3 m_vPos;						// 位置，如果绑定了曲线，这里的位置与实际的BindPos有差别
	A3DVECTOR3 m_vDir;						// 朝向
	A3DVECTOR3 m_vUp;						// 上方向
	unsigned int m_uiControllerID;			// 控制器ID
	unsigned int m_uiDynamicID;				// 动态物品ID

	abase::vector<A3DVECTOR3> m_vBindPos;	// 生成位置, 个数表示 绑定了贝塞尔曲线的生成位置数量

	AString  m_strResPath;					// 路径长度
};
*/
///////////////////////////////////////////////////////////////////////////
//	
//	critter group.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnCritterGroup: public ECSceneObject
{
	//	Behaviour type
	enum
	{
		BEHAVIOUR_WANDER = 0,		// 
		BEHAVIOUR_HOVER,			// 盘旋
		BEHAVIOUR_ARRIVAL,
		BEHAVIOUR_BOID,				// 通用群体行为
		BEHAVIOUR_FLEE,
		BEHAVIOUR_FLEE_AND_BACK,	// 受惊逃散
	};
	

	//	Critter type
	enum
	{
		CRITTER_BUTTERFLY = 0,		// 蝴蝶
		CRITTER_NORMAL_FISH,		// 普通鱼
		CRITTER_DOLTISH_FISH,		// 呆呆的鱼
		CRITTER_BRISK_FISH,			// 敏捷的鱼
		CRITTER_EAGLE,				// 老鹰
		CRITTER_FIREFLY,			// 萤火虫
		CRITTER_CROW,				// 乌鸦
	};

public:
	ECScnCritterGroup(ECScene* scene);
	~ECScnCritterGroup();

	int GetBehaviorType() { return m_iBehaviourType; }
	int GetCritterType() { return m_iCritterType; }
	float GetActAreaRadius() { return m_fRadius; }
//	const A3DVECTOR3& GetActAreaCenter() { return m_vCenter; }
	const A3DVECTOR3& GetPos() {return m_vPos;}
	int GetCritterNum() { return m_iCritterNum; }
	float GetSpeed() { return m_fSpeed;}

	int GetModelTypeByFileName(const char* szFile); // 0: skin model; 1: ECModel, 
	//	Load data from memory
	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	virtual bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);
	virtual bool Render(A3DViewport* pViewport) ;


private:
	ECScene*	m_pScene;

	A3DVECTOR3		m_vPos;			 // 生物群位置
	float			m_fRadius;		 // 生物群半径
	int				m_iCritterNum;	 // 生物个数
	int				m_iBehaviourType;// 生物群运动方式
	int				m_iCritterType;	 // 生物群类型
	unsigned int	m_uiBezierID;	 // 引用的贝塞尔曲线ID,如果没有引用曲线，那么该ID为0
	float			m_fSpeed;		 // 移动速度
//	int				m_iModelPathLen; // 生物模型路径长度
//	const char*		m_szModelPath;	 // 生物模型路径
	AString			m_strModelPath;

	ECAutonomousGroup*			m_pGroup;
	ECAutonomousBehaviour*		m_pBehavior;
	ECBezierWalker*				m_pBezierWalker;
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnRoad: public ECSceneObject
{
public:
	ECScnRoad();
	virtual ~ECScnRoad(){}

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	void Release();

	void SyncRoad(A3DTerrainRoad* pRoad) { m_pRoad = pRoad;}

	const char* GetResFile() { return m_strResPath;}

	A3DTerrainRoad* GetRoad() { return m_pRoad;};

	virtual bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);

	virtual bool Render(A3DViewport* pViewport);

private:

	AString  m_strResPath;					// 路径
	A3DTerrainRoad* m_pRoad;

};
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnGroundDecal: public ECSceneObject
{
public:
	ECScnGroundDecal();
	virtual ~ECScnGroundDecal(){}

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);
	void Release();

	void SyncGroundDecal(A3DTerrainGroundDecal* pDecal) { m_pDecal = pDecal;}

	const char* GetResFile() { return m_strResPath;}

	A3DTerrainGroundDecal* GetGroundDecal() { return m_pDecal;};

	virtual bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);

	virtual bool Render(A3DViewport* pViewport);

private:

	AString  m_strResPath;					// 路径
	A3DTerrainGroundDecal*	m_pDecal;
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
