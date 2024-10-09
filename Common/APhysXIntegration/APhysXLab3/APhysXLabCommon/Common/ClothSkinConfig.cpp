#include "stdafx.h"

#include "ClothSkinConfig.h"
#include "A3DSkinPhysSyncData.h"

ClothSkinItem::~ClothSkinItem()
{
	for(NxU32 i = 0; i < mClothLinks.size(); ++i)
		delete mClothLinks[i];
	mClothLinks.clear();

	if(0 != mSkinPhysSyncData)
	{
		delete mSkinPhysSyncData;
		mSkinPhysSyncData = 0;
	}
}

CClothSkinConfig::CClothSkinConfig(const char* szConfigFilePath)
{
/**********************************************************************

// the format of the config file...

[SkinInfo]
SkinNum = 2

[ClothSkin_0]

SkinFilePath = Ski\hair1.ski
ClothLinkNum = 1

ClothLink_0_ActorName = rb
ClothLink_0_HHName = HH_head
ClothLink_0_IsHHBone = false


[ClothSkin_1]
SkinFilePath = Ski\hair2.ski
ClothLinkNum = 2

ClothLink_0_ActorName = rb0
ClothLink_0_HHName = HH_head
ClothLink_0_IsHHBone = false

ClothLink_1_ActorName = rb1
ClothLink_1_HHName = HH_hair
ClothLink_1_IsHHBone = false

**********************************************************************/

	AIniFile configFile;
	if(!configFile.Open(szConfigFilePath))
		return;

	int iClothSkinNum  = configFile.GetValueAsInt("SkinInfo", "SkinNum", 0);
	
	char szSection[128];
	char szSketch[128];

	for(int i = 0; i < iClothSkinNum; ++i)
	{
		sprintf(szSection, "ClothSkin_%d", i);

		AString strSkinPath = configFile.GetValueAsString(szSection, "SkinFilePath");
		int iLinkNum = configFile.GetValueAsInt(szSection, "ClothLinkNum", 0);
		if(strSkinPath.IsEmpty() || 0 == iLinkNum)
			continue;

		ClothSkinItem* pClothSkinItem = new ClothSkinItem;
		if(0 == pClothSkinItem)
			continue;
		pClothSkinItem->mSkinFilePath = gGlobalStringManager->GetString(strSkinPath);
		
		// load the skin physX sync data...
		af_ChangeFileExt(strSkinPath, ".sphy");
		AString strSkinPhysSyncFile = af_GetBaseDir();
		strSkinPhysSyncFile += "\\";
		strSkinPhysSyncFile += strSkinPath;

		for(int j = 0; j < iLinkNum; ++j)
		{
			CLOTH_LINK* pLink = new CLOTH_LINK;
			if(0 == pClothSkinItem)
				continue;

			sprintf(szSketch, "ClothLink_%d_ActorName", j);
			pLink->szRBActor = gGlobalStringManager->GetString(configFile.GetValueAsString(szSection, szSketch));

			sprintf(szSketch, "ClothLink_%d_HHName", j);
			pLink->szHookHH = gGlobalStringManager->GetString(configFile.GetValueAsString(szSection, szSketch));

			sprintf(szSketch, "ClothLink_%d_IsHHBone", j);
			AString strBool = configFile.GetValueAsString(szSection, "MultithreadedLoading", "false");
		
			if(strBool == "true" || strBool == "True" || strBool == "TRUE")
				pLink->bHHIsBone = true;
			else
				pLink->bHHIsBone = false;

			if(pLink->szRBActor && pLink->szHookHH)
				pClothSkinItem->mClothLinks.push_back(pLink);
			else
				delete pLink;
		}

		if(pClothSkinItem->mClothLinks.size())
			m_arrClothSkins.push_back(pClothSkinItem);
		else
			delete pClothSkinItem;
	}
	configFile.Close();
}

CClothSkinConfig::~CClothSkinConfig()
{
	for(NxU32 i = 0; i < m_arrClothSkins.size(); ++i)
		delete m_arrClothSkins[i];
	m_arrClothSkins.clear();
}