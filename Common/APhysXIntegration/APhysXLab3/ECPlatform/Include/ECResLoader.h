/*
 * FILE: ECResLoader.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECRESLOADER_H_
#define _ECRESLOADER_H_

#include <AExpDecl.h>
#include <ABaseDef.h>
#include <AString.h>

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
class ECResLoadModule;
class CECModel;
class ECBuilding;
class ECDungeon;
class ECScene;
class A3DTerrainRoad;
class A3DTerrainGroundDecal;
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECResLoader
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECResLoader
{
public:		//	Types

	//	Predefined resource type
	enum
	{
		RES_TYPE_UNKNOWN	= 0,
		RES_TYPE_ECMODEL	= 1,
		RES_TYPE_BUILDING	= 2,
		RES_TYPE_DUNGEON	= 3,
		RES_TYPE_ROAD		= 4,
		RES_TYPE_GROUNDDECAL= 5,

		//	User resource type starts from here
		RES_TYPE_USER		= 200,
	};

	friend class ECResLoadModule;

public:		//	Constructor and Destructor

	ECResLoader(int iType);
	virtual ~ECResLoader() {}

public:		//	Attributes

public:		//	Operations

	//	Load resources routine called in loading thread (if multithread enabled)
	virtual bool Load() { return true; }
	//	Unload resources routine called in loading thread (if multithread enabled)
	virtual bool Unload() { return true; }

	//	Override new and delete to ensure all ECResLoader object are
	//	managed by platform lib, so platform can delete a ECResLoader even if 
	//	it's created by application or libs (dll), without worrying
	//	about new/delete matching
	void* operator new (size_t size);
	void operator delete (void* pData);

	//	Get inner resource flag
	bool IsInnerRes() const { return m_bInnerRes; }
	//	Get resource type
	int GetType() const { return m_iType; }

	virtual void SetLoadResult() {}

	ECScene* GetScene() { return m_pScene;}

protected:	//	Attributes

	ECResLoadModule*	m_pLoadModule;		//	Load module object

	bool	m_bInnerRes;	//	true, inner resource managed by platform itself
	int		m_iType;		//	resource type
	
	ECScene* m_pScene;

protected:	//	Operations

	//	bInnerRes parameter has no effect, just used to make a different constructor
	ECResLoader(int iType, bool bInnerRes,ECScene* pScene);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECModelLoader
//	
///////////////////////////////////////////////////////////////////////////

class ECModelLoader : public ECResLoader
{
public:		//	Types

	friend class ECResLoadModule;
	friend class ECScnECModelLoader;
//	friend 	DWORD GetResFromResLoader(ECResLoader* loader);

public:		//	Constructor and Destructor

//	ECModelLoader();
	virtual ~ECModelLoader();

public:		//	Attributes

public:		//	Operations

	//	Set arguments for loading
	void SetLoadArgs(const char* szFile, a_uiptr UserData);
	//	Set arguments for unloading
	void SetUnloadArgs(CECModel* pModel, a_uiptr UserData);

	//	Load resources routine called in loading thread (if multithread enabled)
	virtual bool Load();
	//	Unload resources routine called in loading thread (if multithread enabled)
	virtual bool Unload();

	// Add these two functions since resource callback function need access m_UserData and m_pModel.
	CECModel* GetECModel() { return m_pModel;}
	a_uiptr GetUserData() { return m_UserData;}

	void SetLoadResult();

protected:	//	Attributes

	AString		m_strFile;		//	File name
	CECModel*	m_pModel;		//	Model pointer
	a_uiptr		m_UserData;		//	User data

	int		m_id;

protected:	//	Operations

	//	bInnerRes parameter has no effect, just used to make a different constructor
	ECModelLoader(ECScene* pScene,int id,bool bInnerRes);
	ECModelLoader(bool bInnerRes);  // for unloading
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECBuildingLoader
//	
///////////////////////////////////////////////////////////////////////////

class ECBuildingLoader : public ECResLoader
{
public:		//	Types

	friend class ECResLoadModule;
	friend class ECScnBuildingLoader;

public:		//	Constructor and Destructor

//	ECBuildingLoader();
	virtual ~ECBuildingLoader();

public:		//	Attributes

public:		//	Operations

	//	Load resources routine called in loading thread (if multithread enabled)
	virtual bool Load();
	//	Unload resources routine called in loading thread (if multithread enabled)
	virtual bool Unload();

	// parameters for loading/unloading
	void SetLoadArgs(const char* szFile, a_uiptr UserData);
	void SetUnloadArgs(ECBuilding* pModel, a_uiptr UserData);
	ECBuilding* GetBuilding() { return m_pModel;}
	a_uiptr GetUserData() { return m_UserData;}

	void SetLoadResult();

protected:	//	Attributes

	AString		 m_strFile;		//	File name
	ECBuilding*  m_pModel;		//	Model pointer
	a_uiptr		 m_UserData;	//	User data

	int		m_id;

protected:	//	Operations

	//	bInnerRes parameter has no effect, just used to make a different constructor
	ECBuildingLoader(ECScene* pScene,int id,bool bInnerRes);
	ECBuildingLoader(bool bInnerRes); // for unloading
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECDungeonLoader
//	
///////////////////////////////////////////////////////////////////////////
/*
class ECDungeonLoader : public ECResLoader
{
public:		//	Types

	friend class ECResLoadModule;
	friend class ECScnDungeonLoader;

public:		//	Constructor and Destructor

	ECDungeonLoader();
	virtual ~ECDungeonLoader();

public:		//	Attributes

public:		//	Operations

	//	Load resources routine called in loading thread (if multithread enabled)
	virtual bool Load();
	//	Unload resources routine called in loading thread (if multithread enabled)
	virtual bool Unload();

	// parameters for loading/unloading
	void SetLoadArgs(const char* szFile, a_uiptr UserData);
	void SetUnloadArgs(ECDungeon* pDungeon, a_uiptr UserData);
	ECDungeon* GetDungeon() { return m_pDungeon;}
	a_uiptr GetUserData() { return m_UserData;}

protected:	//	Attributes

	AString		 m_strFile;		//	File name
	ECDungeon*	 m_pDungeon;	//	ECDungeon pointer
	a_uiptr		 m_UserData;	//	User data

protected:	//	Operations

	//	bInnerRes parameter has no effect, just used to make a different constructor
	ECDungeonLoader(bool bInnerRes);
};
*/

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////


class ECRoadLoader : public ECResLoader
{
public:		//	Types

	friend class ECResLoadModule;
	friend class ECScnRoadLoader;

public:		//	Constructor and Destructor

	ECRoadLoader(ECScene* pScene,int id, const char* szFile);
	ECRoadLoader(A3DTerrainRoad* pRoad);

	virtual ~ECRoadLoader(){};

public:		//	Attributes

public:		//	Operations

	//	Load resources routine called in loading thread (if multi thread enabled)
	virtual bool Load();
	//	Unload resources routine called in loading thread (if multi thread enabled)
	virtual bool Unload();

	A3DTerrainRoad* GetRoad() { return m_pModel;}

	void SetLoadResult();

protected:	//	Attributes

	AString				m_strFile;		//	File name
	A3DTerrainRoad*		m_pModel;		//	Model pointer

	int		m_id;

protected:	//	Operations

	//	bInnerRes parameter has no effect, just used to make a different constructor
	
};

//////////////////////////////////////////////////////////////////////////


class ECGroundDecalLoader : public ECResLoader
{
public:		//	Types

	friend class ECResLoadModule;
	friend class ECScnRoadLoader;

public:		//	Constructor and Destructor

	ECGroundDecalLoader(ECScene* pScene,int id, const char* szFile);
	ECGroundDecalLoader(A3DTerrainGroundDecal* pDecal);
	virtual ~ECGroundDecalLoader(){};

public:		//	Attributes

public:		//	Operations

	//	Load resources routine called in loading thread (if multi thread enabled)
	virtual bool Load();
	//	Unload resources routine called in loading thread (if multi thread enabled)
	virtual bool Unload();

	A3DTerrainGroundDecal* GetGroundDecal() { return m_pModel;}

	void SetLoadResult();

protected:	//	Attributes

	AString				m_strFile;		//	File name
	A3DTerrainGroundDecal*		m_pModel;		//	Model pointer

	int		m_id;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



#endif	//	_ECRESLOADER_H_
