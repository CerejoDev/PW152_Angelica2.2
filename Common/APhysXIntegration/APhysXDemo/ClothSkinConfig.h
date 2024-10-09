/************************************************************************

	ClothSkinConfig.h
	  
		This class manages the loading of the cloth-style skin 
	information for ASkinModel...

	
	Created by:		He Wenfeng 
	Date:			Oct. 15, 2008
		

************************************************************************/


#ifndef _CLOTHSKINCONFIG_H_
#define _CLOTHSKINCONFIG_H_

#include "APhysX.h"
class CECmodel;
#include "A3DModelPhysics.h"

//	Cloth link info
struct CLOTH_LINK
{
	const char*	szRBActor;	//	RB actor name
	const char*	szHookHH;	//	hh-type hook name
	bool		bHHIsBone;	//	hh-type hook is a bone instead of hook
};

struct ClothSkinItem
{

	const char* mSkinFilePath;
	A3DSkinPhysSyncData* mSkinPhysSyncData;

	APhysXArray<CLOTH_LINK*> mClothLinks;

	ClothSkinItem()
	{
		mSkinFilePath = NULL;
		mClothLinks = NULL;

		mSkinPhysSyncData = NULL;
	}

	~ClothSkinItem();

};

class CClothSkinConfig
{
public:
	CClothSkinConfig(){};
	~CClothSkinConfig()
	{
		Release();
	};

	void Release();


	bool LoadConfig(const char* szConfigFilePath);

	int GetClothSkinNum() { return m_arrClothSkins.size(); }
	ClothSkinItem* GetClothSkin(int i)
	{
		return m_arrClothSkins[i];
	}

	bool HasClothSkin()
	{
		return ( GetClothSkinNum()>0 );	
	}

protected:


	APhysXArray<ClothSkinItem*> m_arrClothSkins;


};

#endif