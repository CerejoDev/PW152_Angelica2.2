/*
 * FILE: ECScnBoxArea.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY:
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "ECSceneObject.h"
#include <A3DGeometry.h>
#include <AString.h>
#include <A3DTypes.h>


class AFile;
class ECScene;

class _AEXP_DECL ECScnBoxArea : public ECSceneObject
{
public:
	//	Area flags
	enum
	{
		FLAG_DIRLIGHT	= 0x0001,		//	Area has it's own directional light
		FLAG_RAIN		= 0x0002,		//	Area has snow effect
		FLAG_SNOW		= 0x0004,		//	Area has rain effect
		FLAG_SUN		= 0x0008,		//	Area has sun
		FLAG_MOON		= 0x0010,		//	Area has moon
	};

public:		

	ECScnBoxArea();
	virtual ~ECScnBoxArea();

public:

	//	Load data from file
	bool Load(const void* pData, int iDataLen, DWORD dwVersion);

	//void SetAreaID(DWORD dwID) { m_dwAreaID = dwID; }
	//void SetObjectID(DWORD dwID) { m_dwAreaID = dwID; }
	//DWORD GetAreaID() { return m_dwAreaID; }
	//DWORD GetObjectID() const{ return m_dwAreaID;}

	//	Get area flags
	DWORD GetAreaFlags() { return m_dwAreaFlags; }

	//	Is default area
	bool IsDefaultArea() { return m_bDefArea; }
	//	Get area box
	const A3DOBB& GetBox() { return m_obb; }
	//	Get fog parameters in daylight
	float GetFogStart() { return m_fFogStart; }
	float GetFogEnd() { return m_fFogEnd; }
	float GetFogDensity() { return m_fFogDensity; }
	DWORD GetFogColor() { return m_dwFogColor; }

	//	Get under water fog parameters in daylight
/*	float GetUWFogStart() { return m_fUWFogStart; }
	float GetUWFogEnd() { return m_fUWFogEnd; }
	float GetUWFogDensity() { return m_fUWFogDensity; }
	DWORD GetUWFogColor() { return m_dwUWFogColor; }
	//	Get under water fog parameters in night
	float GetUWFogStart_n() { return m_fUWFogStart_n; }
	float GetUWFogEnd_n() { return m_fUWFogEnd_n; }
	float GetUWFogDensity_n() { return m_fUWFogDensity_n; }
	DWORD GetUWFogColor_n() { return m_dwUWFogColor_n; }*/

	//	Get fog parameters in night
	float GetFogStart_n() { return m_fFogStart_n; }
	float GetFogEnd_n() { return m_fFogEnd_n; }
	float GetFogDensity_n() { return m_fFogDensity_n; }
	DWORD GetFogColor_n() { return m_dwFogColor_n; }
	//	Get light parameters in daylight
	DWORD GetAmbient() { return m_dwAmbient; }
	DWORD GetAmbient2() { return m_dwAmbient2;}
	DWORD GetDirLightColor() { return m_dwDirLtCol; }
	float GetAmbientPower1() const { return m_fPowerAmb1D; }
	float GetAmbientPower2() const { return m_fPowerAmb2D; }
	float GetSunPower() { return m_fSunPower; }
	//	Get light parameters in night
	DWORD GetAmbient_n() { return m_dwAmbient_n; }
	DWORD GetAmbient2_n() { return m_dwAmbient2_n; }
	DWORD GetDirLightColor_n() { return m_dwDirLtCol_n; }
	float GetAmbientPower1_n() const { return m_fPowerAmb1N; }
	float GetAmbientPower2_n() const { return m_fPowerAmb2N; }
	float GetSunPower_n() { return m_fSunPower_n; }
	//	Get light direction
	bool HasOwnDirLight() { return m_iDirLightType == 1; }
	int GetDirLightType() { return m_iDirLightType;}
	const A3DVECTOR3 GetLightDir() { return m_vLightDir; }
	const A3DVECTOR3 GetLightDir_n() { return m_vLightDir_n; }

	//	Get sky move speed
	float GetSkySpeedU() { return m_fSkySpeedU; }
	float GetSkySpeedV() { return m_fSkySpeedV; }

	//	Get sky files in daylight	
	const char* GetSkyFile() { return m_strFullSkyFile;}

	//	Get sky files in night
	const char* GetSkyFile_n() {return m_strFullSkyFile_n;}

	//	Get transform time in ms
	DWORD GetTransformTime() { return m_dwTransTime; }
	//	Get music file
	const char* GetMusicFile() { return m_strMusicFile; }
	//	Get rain properties
	bool HasRain() { return (m_dwAreaFlags & FLAG_RAIN) ? true : false; }
	int GetRainSpeed() { return m_iRainSpeed; }
	float GetRainSize() { return m_fRainSize; }
	DWORD GetRainColor() { return m_dwRainCol; }
	//	Get snow properties
	bool HasSnow() { return (m_dwAreaFlags & FLAG_SNOW) ? true : false; }
	int GetSnowSpeed() { return m_iSnowSpeed; }
	float GetSnowSize() { return m_fSnowSize; }
	DWORD GetSnowColor() { return m_dwSnowCol; }
	//	Get cloud color
	DWORD GetCloudAmb() { return m_dwCloudAmb; }
	DWORD GetCloudDirCol() { return m_dwCloudDirCol; }
	DWORD GetCloudAmb_n() { return m_dwCloudAmb_n; }
	DWORD GetCloudDirCol_n() { return m_dwCloudDirCol_n; }
	//	Sun / moon flag
	bool HasSun() { return (m_dwAreaFlags & FLAG_SUN) ? true : false; }
	bool HasMoon() { return (m_dwAreaFlags & FLAG_MOON) ? true : false; }
	//	Terrain properties
	bool HasOwnTrnProps() { return m_bAffectTerrain; }
	DWORD GetTrnDiffuse() { return m_dwTrnDiffuse; }
	DWORD GetTrnSpecular() { return m_dwTrnSpecular; }
	float GetTrnSpecularPower() { return m_fTrnSpecPower; }
	DWORD GetTrnLtScale() {return m_dwTrnLtScale;}

	//	Get shadow deepness
	//float GetShadowDeep() const { return m_fShadowDeep; }
	//float GetShadowDeep_n() const { return m_fShadowDeep_n; }

	float GetPowerSunECM() const {return m_fPowerSunECM;}
	float GetPowerAmbECM() const {return m_fPowerAmbECM;}

	float GetPowerSunECM_N() const {return m_fPowerSunECM_n;}
	float GetPowerAmbECM_N() const { return m_fPowerAmbECM_n;}

	const char* GetColorCorrectionFile() {return m_strColorCorrectionFile;}

	bool GetUseTexCCFlag() const { return m_bUseTexCC;}

protected:	//	Attributes

//	DWORD		m_dwAreaID;			//	Area ID
	bool		m_bDefArea;			//	true, is default area
	A3DOBB		m_obb;				//	Area box
	DWORD		m_dwTransTime;		//	Transform time
	DWORD		m_dwAreaFlags;		//	Area flags

	float		m_fFogStart;		//	Fog start distance in daylight
	float		m_fFogEnd;			//	Fog end distance in daylight
	float		m_fFogDensity;		//	Fog density in daylight
	DWORD		m_dwFogColor;		//	Fog color in daylight

	// remove by version 15
// 	float		m_fUWFogStart;		//	Under water fog start in daylight
// 	float		m_fUWFogEnd;		//	Under water fog end in daylight
// 	float		m_fUWFogDensity;	//	Under water fog density in daylight
// 	DWORD		m_dwUWFogColor;		//	Under water fog color in daylight

	float		m_fFogStart_n;		//	Fog start distance in night
	float		m_fFogEnd_n;		//	Fog end distance in night
	float		m_fFogDensity_n;	//	Fog density in night
	DWORD		m_dwFogColor_n;		//	Fog color in night

// 	float		m_fUWFogStart_n;	//	Under water fog start in night
// 	float		m_fUWFogEnd_n;		//	Under water fog end in night
// 	float		m_fUWFogDensity_n;	//	Under water fog density in night
// 	DWORD		m_dwUWFogColor_n;	//	Under water fog color in night

	int			m_iDirLightType;	//  Directional light type,
										//  0-use default light
										//  1-custom light
										//  2-not use light
	A3DVECTOR3	m_vLightDir;		//	Light direction
	DWORD		m_dwAmbient;		//	Ambient color in daylight
	DWORD		m_dwDirLtCol;		//	Directional light color in daylight
	DWORD		m_dwAmbient_n;		//	Ambient color in night

	DWORD		m_dwDirLtCol_n;		//	Directional light color in night

	DWORD		m_dwCloudAmb;		//	Cloud ambient color
	DWORD		m_dwCloudAmb_n;		//	Cloud ambient color at night
	DWORD		m_dwCloudDirCol;	//	Cloud directional color
	DWORD		m_dwCloudDirCol_n;	//	Cloud directional color at night

	bool		m_bAffectTerrain;	//	Affect terrain material flag
	DWORD		m_dwTrnDiffuse;		//	Terrain's diffuse
	DWORD		m_dwTrnSpecular;	//	Terrain's specular
	float		m_fTrnSpecPower;	//	Terrain's specular power

	float		m_fSkySpeedU;		//	Sky moving speed on U axis
	float		m_fSkySpeedV;		//	Sky moving speed on V axis

	AString		m_strMusicFile;		//	Music file name

	AString		m_strFullSkyFile;
	AString		m_strFullSkyFile_n;

	AString		m_strColorCorrectionFile; // version 17

	int			m_iRainSpeed;		//	Rain properties
	float		m_fRainSize;
	DWORD		m_dwRainCol;
	int			m_iSnowSpeed;		//	Snow properties
	float		m_fSnowSize;
	DWORD		m_dwSnowCol;

	// remove by version 14
//	float		m_fShadowDeep;		//	Shadow deepness in daylight白天阴影浓度
//	float		m_fShadowDeep_n;	//	Shadow deepness in night晚上阴影浓度



	// 版本8 去掉了几个变量 , 版本9 又加上了
	float		m_fSunPower;		//	Sun power
	float		m_fSunPower_n;


	// 版本8 新加的变量
	A3DVECTOR3  m_vLightDir_n;  // 
	DWORD		m_dwAmbient2;// 
	DWORD		m_dwAmbient2_n;   //// 
	DWORD		m_dwTrnLtScale;		// 

	float	m_fPowerAmb1D;// 环境光1和2颜色亮度
	float	m_fPowerAmb2D;// 环境光1和2颜色亮度
	float	m_fPowerAmb1N;// 环境光1和2颜色亮度
	float	m_fPowerAmb2N;// 环境光1和2颜色亮度


	// add by version 16
	float	m_fPowerSunECM; //动画模型的太阳颜色亮度
	float	m_fPowerAmbECM; //动画模型的环境颜色亮度 

	// version 18
	float	m_fPowerSunECM_n; // 动画模型的太阳颜色亮度，晚上
	float	m_fPowerAmbECM_n;

	// version 17
	bool	m_bUseTexCC;

protected:	//	Operations

};