/*
* FILE: ECSceneModule.h
*
* DESCRIPTION: 
*
* CREATED BY: Yongdong Wang, 2009/9/2
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#pragma once 

#include <A3DVector.h>
#include <AExpDecl.h>
#include <ABaseDef.h>
#include <hashtab.h>
#include <hashmap.h>
#include <AArray.h>
#include <ARect.h>
#include <A3DTypes.h>
#include <AString.h>
#include "ECSceneFile.h"
#include "ECSceneObjectMgr.h"

class ECTerrain;
class A3DViewport;
class A3DGFXEx;
class AM3DSoundBuffer;
class CECModel;
class ECScnBoxArea;
class ECSceneModule;
class ECResLoadModule;
class ECResLoader;
class ECSceneObjectMgr;
class ECDynGfxLightMgr;
//class ECGrassLand;
//class ECGrassType;
class A3DGrassLand;
class ECPrecinctSet;
class ECSceneRenderer;
class ECBuilding;

//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL IResLoaderCallBack
{
public:
	//virtual ECResLoader* GenerateBuildingLoader(ECScene* pScene,int id,const char* szFile = NULL)=0;
	//virtual ECResLoader* GenerateECModelLoader(ECScene* pScene,int id,const char* szFile = NULL) =0;

	virtual void LoadBuliding(int id,ECBuilding* pBuilding) {};
	virtual void ReleaseBuilding(int id,ECBuilding* pBuilding) {};
	virtual void LoadECModel(int id,const char* szFile = NULL) {};
	virtual void ReleaseECModel(int id,const char* szFile = NULL) {};

protected:
private:
};

//////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScene
{
	friend class ECSceneObjectLoader;

public:
	enum
	{
		SO_TYPE_NO = 0,
		SO_TYPE_LIGHT,		
		SO_TYPE_WATER,
		SO_TYPE_BEZIER,
		SO_TYPE_BOXAREA,
		SO_TYPE_SOUNDPOINT		= 5,
		SO_TYPE_DUMMY,
		SO_TYPE_AIAREA,
		SO_TYPE_SPECIALLY,
		SO_TYPE_ECMODEL,
		SO_TYPE_CRITTER_GROUP	= 10,
		SO_TYPE_FIXED_NPC,
		SO_TYPE_PRECINCT,
		SO_TYPE_GATHER,
		SO_TYPE_RANGE,
		SO_TYPE_CLOUD_BOX		= 15,
		SO_TYPE_INSTANCE_BOX,
		SO_TYPE_DYNAMIC,			// 策划数据，改为clt格式了。
		SO_TYPE_TEST, // obsoleted		
		SO_TYPE_DUNGEON	,	
		SO_TYPE_BUILDING		= 20,
		SO_TYPE_CAMERA,
		SO_TYPE_OCCLUDER,

		SO_TYPE_SPEEDTREE,
		SO_TYPE_AUDIO			= 24,
		SO_TYPE_TRANSMITBOX		= 25, // 策划数据
		SO_TYPE_ROAD			= 26,

		SO_TYPE_REVERBERANT		= 29, // 混响区域
		SO_TYPE_GROUNDDECAL		= 30, // 地表贴花

		SO_TYPE_BUFF			= 31, // just for sever

		SO_TYPE_VOLUME			= 32,			
		SO_TYPE_MUSIC_AREA		= 33,	
		SO_TYPE_AE_BOX			= 34,	

		SO_TYPE_USER			= 1000,

		SO_TYPE_NUM
	};
	// type info
	struct TypeResInfo 
	{
		DWORD typeResID;
		AString strTypeFileName;
	};

	// block info
	struct ObjInfoInBlock 
	{
		int iTypeID;
		DWORD dwObjID;
	};
	struct BlockInfo
	{
		BlockInfo()
		{
			objCount = 0;
			iRowIdxInWld = 0;
			iColIdxInWld = 0;
			pObjInfo = NULL;
		}
		int iRowIdxInWld;
		int iColIdxInWld;
		int objCount;
		ObjInfoInBlock* pObjInfo;
	};
/*	struct OBJECTDTATDDESC 
	{
		OBJECTDTATDDESC(){ pRawData = NULL;}
		int iTypeID;
		DWORD dwObjectID;
		A3DVECTOR3 vPos; // 该对象的位置
		DWORD dwVersion;
		float fRadius; // 物体本身半径
		int iLoadDist;// 该种资源在 scene的加载距离,定义为int型。
		void* pRawData; // 数据指针
		int iRawDataLen;// 数据长度
		bool bLoaded; // 是否被加载
		int offset; // 偏移
	};
	*/

#pragma pack(1)


	struct OBJECTDTATDDESC
	{
		OBJECTDTATDDESC(){ pRawData = NULL;}
		unsigned short iTypeID;//int iTypeID;		
		unsigned short dwVersion; //DWORD dwVersion;
		unsigned short bLoaded:1; // bool bLoaded; // 是否被加载
		unsigned short iLoadDist:15; //int iLoadDist;// 该种资源在 scene的加载距离,定义为int型。		
		DWORD dwObjectID; //DWORD dwObjectID;
		float fRadius; // 物体本身半径		
		void* pRawData; // 数据指针
		int iRawDataLen;// 数据长度		
		int offset; // 偏移
		A3DVECTOR3 vPos; // 该对象的位置
	};

#pragma pack()

	typedef abase::hashtab<BlockInfo*,int,abase::_hash_function> BlockTable;
	typedef abase::hashtab<OBJECTDTATDDESC*,DWORD,abase::_hash_function> ResRawDataTable;
	typedef abase::hashtab<ECSceneObjectLoader*,int,abase::_hash_function> ObjectLoaderTable;


public:

	ECScene(ECSceneModule* esSceneModule);
	virtual ~ECScene();

	bool Init();
	bool Release();	

	bool Tick(DWORD dt,const A3DVECTOR3& vLoadCenter);
	bool Tick_Audio(DWORD dt,const A3DVECTOR3& vCamera);

	bool Render(A3DViewport* pViewport);	// Render scene objects.
	//	Build scene renderer
	void BuildSceneRenderer(A3DViewport* pViewport, ECSceneRenderer* pRenderer);
	
	bool LoadSceneData(const char* szFile, float cx, float cz);//Load scene data from file.
	void InitDefaultObjLoader();
	void RegisterObjLoader(int resType,ECSceneObjectLoader* loader);
	ECSceneObjectLoader* GetScnObjLoader(int loaderType);

    bool SetLoadCenter(const A3DVECTOR3& vCenter); 
	//set active radius in scene. This affect the active area block!!
    void SetActRadius(float fActRadius) { m_fActRadius = fActRadius; }
	void SetResLoadDist(int type,float dist);
	void SetGrassLoadRadius(float fRadius);
	
	float GetActRadius() const { return m_fActRadius; }
	const A3DVECTOR3& GetLoadCenter() { return m_vLoadCenter; }
	float GetCenterX() { return m_fWorldCenterX; }
	float GetCenterZ() { return m_fWorldCenterZ; }
	float GetBlockSizeX() { return m_fBlockSizeX; }
	float GetBlockSizeZ() { return m_fBlockSizeZ; }
	ECSceneObjectMgr* GetSceneObjectMgr() { return &m_ScnObjectMgr;}
	ECSceneModule* GetSceneModule() {return m_pSceneModule;}
	void SetResLoadModule(ECResLoadModule* pRes) { m_pResLoadModule = pRes;}

	//bool GetLight(const A3DVECTOR3& vPos,ECScnLight& light);  // 此函数要去掉！
	bool GetLightParam(const A3DVECTOR3& vPos,A3DLIGHTPARAM& param);


	const AString& GetTerrainDetailTexturePath() const { return m_strTerrainDetailTex;}

	bool LoadGrass(const char* szFile);

	void SetResLoaderCallBack(IResLoaderCallBack* pCB) {m_pResLoaderCB = pCB;};
	IResLoaderCallBack* GetResLoaderCallBack() { return m_pResLoaderCB;}


protected:

	void CalcAreaInBlocks(const A3DVECTOR3& vCenter, float fRadius, ARectI& rcArea);
	bool UpdateScnObject(const A3DVECTOR3& vLoadCenter);
	bool LoadObjectData(const char* szFile);
	bool LoadSceneInfoFromFile(const char* szFile); // load ecwld
	bool LoadGrassType(const char* szFile);  // grass.dat
	bool UpdateGrass(const A3DVECTOR3& vLoadCenter);

	bool LoadPrecinctSet(const char* szFile);

private:

	BlockTable m_BlockTable;
	ResRawDataTable m_ObjectDataTable;
	ResRawDataTable m_ActObjDataTable;
	ObjectLoaderTable m_ObjectLoaderTable;

	ECSceneObjectMgr m_ScnObjectMgr;
	ECDynGfxLightMgr* m_pDynGfxLightMgr;

private:

	float	m_aResLoadDist[SO_TYPE_NUM];//	Resource loading distance

	ARectF		m_rcWorld;			//	Whole world area in logic unit (meters)
	ARectI		m_actBlockRect; // Or we can use active block index.

	A3DVECTOR3	m_vLoadCenter;		//	Load center
	float		m_fBlockSizeX;		//	Block size (on x and z axis) in logic unit (meters)
	float		m_fBlockSizeZ;
	float		m_fHalfBlockRadius; //  it takes place of original value:(m_fBlockSize * 0.5)!
	int			m_iNumBlock;		//	Number of block
	int			m_iNumBlockRow;		//	Block row number in whole world
	int			m_iNumBlockCol;		//	Block column number of whole world
	float		m_fWorldCenterX;	//	Center position of whole world
	float		m_fWorldCenterZ;	
	float		m_fActRadius;		//	Active area radius in logic unit (meters)

	AString		m_strTerrainDetailTex; // detail texture path, be added in version 2

	ECTerrain* m_pECTerrain;
	ECResLoadModule* m_pResLoadModule;
	ECSceneModule* m_pSceneModule;

	IResLoaderCallBack* m_pResLoaderCB;
};
