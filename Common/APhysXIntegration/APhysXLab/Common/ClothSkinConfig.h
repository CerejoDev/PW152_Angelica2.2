/*
 * FILE: ClothSkinConfig.h
 *
 * DESCRIPTION: 
 *		This class manages the loading of the cloth-style skin information for ASkinModel...
 *
 * CREATED BY: He Wenfeng, Oct. 15, 2008
 *
 * HISTORY: 
 *	  Revised by Yang Liu, 2009/05/15
 *		make some clean up work...  
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_CLOTHSKINCONFIG_H_
#define _APHYSXLAB_CLOTHSKINCONFIG_H_

#include "APhysX.h"
class A3DSkinPhysSyncData;

struct CLOTH_LINK
{
	const char*	szRBActor;	//	RB actor name
	const char*	szHookHH;	//	hh-type hook name
	bool		bHHIsBone;	//	hh-type hook is a bone instead of hook
};

struct ClothSkinItem
{
	ClothSkinItem() : mSkinFilePath(0), mSkinPhysSyncData(0) {}
	~ClothSkinItem();
	
	const char* mSkinFilePath;
	A3DSkinPhysSyncData* mSkinPhysSyncData;
	APhysXArray<CLOTH_LINK*> mClothLinks;
};

class CClothSkinConfig
{
public:
	CClothSkinConfig(const char* szConfigFilePath);
	~CClothSkinConfig();

	int GetClothSkinNum() const { return m_arrClothSkins.size(); }
	ClothSkinItem* GetClothSkin(const int i) const
	{
		if ((0 > i) || (i >= GetClothSkinNum()))
			return 0;
		return m_arrClothSkins[i];
	}

private:
	// forbidden behavior
	CClothSkinConfig(const CClothSkinConfig& rhs);
	CClothSkinConfig& operator= (const CClothSkinConfig& rhs);

private:
	APhysXArray<ClothSkinItem*> m_arrClothSkins;
};

#endif