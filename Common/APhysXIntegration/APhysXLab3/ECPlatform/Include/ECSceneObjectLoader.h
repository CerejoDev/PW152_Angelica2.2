/*
* FILE: ECSceneObjectLoader.h
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

#include <ARect.h>

#include <ABaseDef.h>
#include <AExpDecl.h>
#include <AList2.h>
#include "ECScene.h"

class ECScene;
class ECSceneObjectMgr;
class ECScnBuilding;
///////////////////////////////////////////////////////////////////////////
//	
//	Base loader class of scene object. This class parse the raw data in object
//  descriptor into detailed scene object's struct.
//
//	NOTE: ECSceneObjectLoader is different form ECResLoader.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECSceneObjectLoader
{
public:
	struct innerUserData 
	{
	//	ECSceneObjectMgr* pScnMgr;
		ECScene* pScene;
		DWORD id; 
	};

public:
	ECSceneObjectLoader(ECScene* scene,int type)
	{
		m_pScene = scene;
		m_type = type;
	};
	virtual ~ECSceneObjectLoader(){};
	virtual bool Init()
	{
		if (m_pScene)
		{
			m_pResLoadModule = m_pScene->m_pResLoadModule;
			return true;
		}
		else return false;
	}
	virtual int GetType() { return m_type;}

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter) = 0;
	virtual void UnLoadObject(DWORD id);

	ECScene*	GetOwnerScene() const { return m_pScene;}
	
protected:		

	int m_type;

	ECScene* m_pScene;
	ECResLoadModule* m_pResLoadModule;

private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for box area
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnBoxAreaLoader: public ECSceneObjectLoader
{
public:
	ECScnBoxAreaLoader(ECScene* scene);
	~ECScnBoxAreaLoader(){}
	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for ECScnECModel object. 
//	NOTE: class ECScnECModel is different from ECModel.
//
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnECModelLoader: public ECSceneObjectLoader
{
public:
	ECScnECModelLoader(ECScene* scene);
	virtual ~ECScnECModelLoader();
	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	void UnLoadObject(DWORD id);

private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for bezier
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnBezierLoader: public ECSceneObjectLoader
{
public:
	ECScnBezierLoader(ECScene* scene);
	~ECScnBezierLoader(){}
	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
protected:
private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for ECScnBuilding.
//  NOTE: ECScnBuilding is different from ECBuilding.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnBuildingLoader: public ECSceneObjectLoader  // building 
{
	friend class ECScnBuilding;
public:
	ECScnBuildingLoader(ECScene* scene);
	virtual ~ECScnBuildingLoader();

	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	void UnLoadObject(DWORD id);

	bool LoadLODBuilding(bool bFirstLoad,ECScnBuilding* pBuilding);

protected:
	void UnLoadBuilding(ECScnBuilding* pBuilding);

private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for ECScnDungeion.
//	NOTE: ECScnDungeon is different from ECDungeon.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnDungeonLoader: public ECSceneObjectLoader
{
public:
	ECScnDungeonLoader(ECScene* scene);
	virtual ~ECScnDungeonLoader(){};
	
	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	/*void UnLoadObject(DWORD id);*/
protected:
private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for Camera.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnCameraLoader: public ECSceneObjectLoader
{
public:
	ECScnCameraLoader(ECScene* scene);
	virtual ~ECScnCameraLoader(){};

	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
protected:
private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for light.
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnLightLoader: public ECSceneObjectLoader
{
public:
	ECScnLightLoader(ECScene* scene);
	virtual ~ECScnLightLoader(){};

	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	void UnLoadObject(DWORD id);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for water.
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnWaterAreaLoader: public ECSceneObjectLoader
{
public:
	ECScnWaterAreaLoader(ECScene* scene);
	virtual ~ECScnWaterAreaLoader(){};

	bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	void UnLoadObject(DWORD id);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Loader class for Occluder
//	
///////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnOccluderLoader: public ECSceneObjectLoader
{
public:
	ECScnOccluderLoader(ECScene* scene);
	virtual ~ECScnOccluderLoader(){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	/*virtual void UnLoadObject(DWORD id);*/
};

//////////////////////////////////////////////////////////////////////////
//
// Gfx Loader
//
//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnGfxLoader: public ECSceneObjectLoader
{
public:
	ECScnGfxLoader(ECScene* scene);
	virtual ~ECScnGfxLoader(){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	
};
//////////////////////////////////////////////////////////////////////////
//
// Critter group Loader
//
//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnCritterGroupLoader: public ECSceneObjectLoader
{
public:
	ECScnCritterGroupLoader(ECScene* scene);
	virtual ~ECScnCritterGroupLoader(){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	virtual void UnLoadObject(DWORD id);
};

class _AEXP_DECL ECScnRoadLoader: public ECSceneObjectLoader
{
public:
	ECScnRoadLoader(ECScene* scene);
	virtual ~ECScnRoadLoader(){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	virtual void UnLoadObject(DWORD id);
};

class _AEXP_DECL ECScnGroundDecalLoader: public ECSceneObjectLoader
{
public:
	ECScnGroundDecalLoader(ECScene* scene);
	virtual ~ECScnGroundDecalLoader(){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
	virtual void UnLoadObject(DWORD id);
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
class _AEXP_DECL ECScnAudioLoader: public ECSceneObjectLoader
{
public:
	ECScnAudioLoader(ECScene* scene):ECSceneObjectLoader(scene,ECScene::SO_TYPE_AUDIO){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
};

class _AEXP_DECL ECScnReverbLoader: public ECSceneObjectLoader
{
public:
	ECScnReverbLoader(ECScene* scene):ECSceneObjectLoader(scene,ECScene::SO_TYPE_REVERBERANT){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
};

class _AEXP_DECL ECScnVolumeLoader: public ECSceneObjectLoader
{
public:
	ECScnVolumeLoader(ECScene* scene):ECSceneObjectLoader(scene,ECScene::SO_TYPE_VOLUME){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
};

class _AEXP_DECL ECScnMusicAreaLoader: public ECSceneObjectLoader
{
public:
	ECScnMusicAreaLoader(ECScene* scene):ECSceneObjectLoader(scene,ECScene::SO_TYPE_MUSIC_AREA){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
};

class _AEXP_DECL ECScnAEBoxLoader: public ECSceneObjectLoader
{
public:
	ECScnAEBoxLoader(ECScene* scene):ECSceneObjectLoader(scene,ECScene::SO_TYPE_AE_BOX){};

	virtual bool LoadObject(ECScene::OBJECTDTATDDESC* pDataDesc,const A3DVECTOR3& vLoadCenter);
};