#include "Global.h"
#include <EC_Model.h>
#include "ClothSkinConfig.h"
#include "A3DSkinPhysSyncData.h"

ClothSkinItem::~ClothSkinItem()
{
	for(int i=0; i<mClothLinks.size(); i++)
	{
		delete mClothLinks[i];
	}

	mClothLinks.clear();

	if(mSkinPhysSyncData)
	{
		delete mSkinPhysSyncData;
		mSkinPhysSyncData = NULL;
	}

}

//---------------------------------------------------------------
void CClothSkinConfig::Release()
{

	for(int i=0; i<m_arrClothSkins.size(); i++)
	{
		delete m_arrClothSkins[i];
	}

	m_arrClothSkins.clear();

}


bool CClothSkinConfig::LoadConfig(const char* szConfigFilePath)
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
		return false;
	
	int iClothSkinNum  = configFile.GetValueAsInt("SkinInfo", "SkinNum", 0);
	
	char szSection[128];
	char szSketch[128];

	for(int i=0; i<iClothSkinNum; i++)
	{
		sprintf(szSection, "ClothSkin_%d", i);

		AString strSkinPath = configFile.GetValueAsString(szSection, "SkinFilePath");
		int iLinkNum = configFile.GetValueAsInt(szSection, "ClothLinkNum", 0);

		if(strSkinPath.IsEmpty() || iLinkNum == 0)
			continue;


		ClothSkinItem* pClothSkinItem = new ClothSkinItem;

		pClothSkinItem->mSkinFilePath  = gGlobalStringManager->GetString(strSkinPath);
		
		// load the skin physX sync data...
		af_ChangeFileExt(strSkinPath, ".sphy");
		AString strSkinPhysSyncFile = g_szWorkDir;
		strSkinPhysSyncFile += "\\";
		strSkinPhysSyncFile += strSkinPath;
		/*
		pClothSkinItem->mSkinPhysSyncData = new A3DSkinPhysSyncData;
		if(!pClothSkinItem->mSkinPhysSyncData->Load(strSkinPhysSyncFile))
		{
			delete pClothSkinItem;
			continue;
		}
		*/


		for(int j=0; j<iLinkNum; j++)
		{
			
			CLOTH_LINK* pLink = new CLOTH_LINK;

			sprintf(szSketch, "ClothLink_%d_ActorName", j);
			pLink->szRBActor = gGlobalStringManager->GetString(configFile.GetValueAsString(szSection, szSketch));

			sprintf(szSketch, "ClothLink_%d_HHName", j);
			pLink->szHookHH = gGlobalStringManager->GetString(configFile.GetValueAsString(szSection, szSketch));

			sprintf(szSketch, "ClothLink_%d_IsHHBone", j);
			AString strBool = configFile.GetValueAsString(szSection, "MultithreadedLoading", "false");
			if(strBool == "true" || strBool == "True" || strBool == "TRUE")
			{
				pLink->bHHIsBone = true;
				
			}
			else if(strBool == "false" || strBool == "False" || strBool == "FALSE" )
			{
				pLink->bHHIsBone = false;
			}

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

	// whether we load the skins correctly?
	if(m_arrClothSkins.size())
		return true;
	else
		return false;

}
