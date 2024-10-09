/*
 * FILE: ECSceneFile.h
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

#include <ABaseDef.h>

////////////////////////////////////////////////////////////////////////////
//	
//	Define scene object struct. These struts don't include variable length data member (such as file name).
//	
///////////////////////////////////////////////////////////////////////////

// Box area
struct ECBOXAREADATAFILE
{
	int		iDefArea;		//	non-zero, is default area
	DWORD	dwAreaFlags;	//	Area flags
	A3DVECTOR3	vPos;		//	Area position
	A3DVECTOR3	vExt;		//	Area half extents
	A3DVECTOR3	vAxisX;		//	Area normalized x axis
	A3DVECTOR3	vAxisY;		//	Area normalized y axis
	A3DVECTOR3	vAxisZ;		//	Area normalized z axis
	float   fFogStart;		//	Fog start distance in daylight
	float   fFogEnd;		//	Fog end distance in daylight
	float   fFogDensity;	//	Fog density in daylight
	DWORD   dwFogColor;		//	Fog color in daylight
	float   fUWFogStart;	//	Under water fog start in daylight
	float   fUWFogEnd;		//	Under water fog end in daylight
	float   fUWFogDensity;	//	Under water fog density in daylight
	DWORD   dwUWFogCol;		//	Under water fog color in daylight
	float	fFogStart_n;	//	Fog start distance in night
	float	fFogEnd_n;		//	Fog end distance in night
	float	fFogDensity_n;	//	Fog density in night
	DWORD	dwFogColor_n;	//	Fog color in night
	float   fUWFogStart_n;	//	Under water fog start in night
	float   fUWFogEnd_n;	//	Under water fog end in night
	float   fUWFogDensity_n;//	Under water fog density in night
	DWORD   dwUWFogCol_n;	//	Under water fog color in night		
	int    iUseLightMethod; //  Use light method:
								//  0-use default light
								//  1-custom light
								//  2-not use light
	A3DVECTOR3	vLightDir;	//	Directional light's direction
	DWORD	dwLightColor;	//	light color
	DWORD	dwAmbient;		//	Ambient color in daylight
	DWORD	dwDirLtCol;		//	Directional light color in daylight
	DWORD	dwAmbient_n;	//	Ambient color in night
	DWORD	dwDirLtCol_n;	//	Directional light color in night
	float	vSkySpeed[2];	//	Sky moving speed on 2 axises
	DWORD	dwTrnLtScale;	//	Terrain light scale
	DWORD	dwTransTime;	//	Transform time
	int     iRainSpeed;		//	Rain properties
	float   fRainSize;
	DWORD   dwRainCol;
	int     iSnowSpeed;		//	Snow properties
	float   fSnowSize;
	DWORD   dwSnowCol;
	float	fSunPower;		//	Sun power
	float	fSunPower_n;
	DWORD	dwCloudAmb;		//	Cloud ambient color in daylight
	DWORD	dwCloudDir;		//	Cloud direction light color in daylight
	DWORD	dwCloudAmb_n;	//	Cloud ambient color in night
	DWORD	dwCloudDir_n;	//	Cloud direction light color in night
	bool	bAffectTerrain;	//	Affect terrain material flag
	DWORD	dwTrnDiffuse;	//	Terrain's diffuse
	DWORD	dwTrnSpecular;	//	Terrain's specular
	float	fTrnSpecPower;	//	Terrain's specular power

/*	Variable length data
	int		iDayTex0Len // 白天天空纹理0的路径长度(int)
	char*	szDayTex0// 白天天空纹理0的路径(char[iDayTex0Len])
	int		iDayTex1Len // 白天天空纹理1的路径长度(int)
	char*	szDayTex1// 白天天空纹理1的路径(char[iDayTex1Len])
	int		iDayTex2Len // 白天天空纹理2的路径长度(int)
	char*	szDayTex2// 白天天空纹理2的路径(char[iNightTex2Len])
	int		iNightTex0Len // 晚上天空纹理0的路径长度(int)
	char*	szNightTex0// 晚上天空纹理0的路径(char[iNightTex0Len])
	int		iNightTex1Len // 晚上天空纹理1的路径长度(int)
	char*	szNightTex1// 晚上天空纹理1的路径(char[iNightTex1Len])
	int		iNightTex2Len // 晚上天空纹理2的路径长度(int)
	char*	szNightTex2// 晚上天空纹理2的路径(char[iNightTex2Len])
*/
};

// Box area. Version >= 5
struct ECBOXAREADATAFILE5
{
	int		iDefArea;		//	non-zero, is default area
	DWORD	dwAreaFlags;	//	Area flags
	A3DVECTOR3	vPos;		//	Area position
	A3DVECTOR3	vExt;		//	Area half extents
	A3DVECTOR3	vAxisX;		//	Area normalized x axis
	A3DVECTOR3	vAxisY;		//	Area normalized y axis
	A3DVECTOR3	vAxisZ;		//	Area normalized z axis
	float   fFogStart;		//	Fog start distance in daylight
	float   fFogEnd;		//	Fog end distance in daylight
	float   fFogDensity;	//	Fog density in daylight
	DWORD   dwFogColor;		//	Fog color in daylight
	float   fUWFogStart;	//	Under water fog start in daylight
	float   fUWFogEnd;		//	Under water fog end in daylight
	float   fUWFogDensity;	//	Under water fog density in daylight
	DWORD   dwUWFogCol;		//	Under water fog color in daylight
	float	fFogStart_n;	//	Fog start distance in night
	float	fFogEnd_n;		//	Fog end distance in night
	float	fFogDensity_n;	//	Fog density in night
	DWORD	dwFogColor_n;	//	Fog color in night
	float   fUWFogStart_n;	//	Under water fog start in night
	float   fUWFogEnd_n;	//	Under water fog end in night
	float   fUWFogDensity_n;//	Under water fog density in night
	DWORD   dwUWFogCol_n;	//	Under water fog color in night		
	int    iUseLightMethod; //  Use light method:
								//  0-use default light
								//  1-custom light
								//  2-not use light
	A3DVECTOR3	vLightDir;	//	Directional light's direction
	DWORD	dwLightColor;	//	light color
	DWORD	dwAmbient;		//	Ambient color in daylight
	DWORD	dwDirLtCol;		//	Directional light color in daylight
	DWORD	dwAmbient_n;	//	Ambient color in night
	DWORD	dwDirLtCol_n;	//	Directional light color in night
	float	vSkySpeed[2];	//	Sky moving speed on 2 axises
	DWORD	dwTrnLtScale;	//	Terrain light scale
	DWORD	dwTransTime;	//	Transform time
	int     iRainSpeed;		//	Rain properties
	float   fRainSize;
	DWORD   dwRainCol;
	int     iSnowSpeed;		//	Snow properties
	float   fSnowSize;
	DWORD   dwSnowCol;
	float	fSunPower;		//	Sun power
	float	fSunPower_n;
	DWORD	dwCloudAmb;		//	Cloud ambient color in daylight
	DWORD	dwCloudDir;		//	Cloud direction light color in daylight
	DWORD	dwCloudAmb_n;	//	Cloud ambient color in night
	DWORD	dwCloudDir_n;	//	Cloud direction light color in night
	bool	bAffectTerrain;	//	Affect terrain material flag
	DWORD	dwTrnDiffuse;	//	Terrain's diffuse
	DWORD	dwTrnSpecular;	//	Terrain's specular
	float	fTrnSpecPower;	//	Terrain's specular power
	float	fShadowDeep;	//	Shadow deepness in daylight
	float	fShadowDeep_n;	//	Shadow deepness in night

/*	Variable length data
	int		iDayTex0Len // 白天天空纹理0的路径长度(int)
	char*	szDayTex0// 白天天空纹理0的路径(char[iDayTex0Len])
	int		iDayTex1Len // 白天天空纹理1的路径长度(int)
	char*	szDayTex1// 白天天空纹理1的路径(char[iDayTex1Len])
	int		iDayTex2Len // 白天天空纹理2的路径长度(int)
	char*	szDayTex2// 白天天空纹理2的路径(char[iNightTex2Len])
	int		iNightTex0Len // 晚上天空纹理0的路径长度(int)
	char*	szNightTex0// 晚上天空纹理0的路径(char[iNightTex0Len])
	int		iNightTex1Len // 晚上天空纹理1的路径长度(int)
	char*	szNightTex1// 晚上天空纹理1的路径(char[iNightTex1Len])
	int		iNightTex2Len // 晚上天空纹理2的路径长度(int)
	char*	szNightTex2// 晚上天空纹理2的路径(char[iNightTex2Len])
*/
};
struct ECBOXAREADATAFILE8
{
	int		iDefArea;		//	non-zero, is default area
	DWORD	dwAreaFlags;	//	Area flags
	A3DVECTOR3	vPos;		//	Area position
	A3DVECTOR3	vExt;		//	Area half extents
	A3DVECTOR3	vAxisX;		//	Area normalized x axis
	A3DVECTOR3	vAxisY;		//	Area normalized y axis
	A3DVECTOR3	vAxisZ;		//	Area normalized z axis

	float   fFogStart;		//	Fog start distance in daylight
	float   fFogEnd;		//	Fog end distance in daylight
	float   fFogDensity;	//	Fog density in daylight
	DWORD   dwFogColor;		//	Fog color in daylight

	float   fUWFogStart;	//	Under water fog start in daylight
	float   fUWFogEnd;		//	Under water fog end in daylight
	float   fUWFogDensity;	//	Under water fog density in daylight
	DWORD   dwUWFogCol;		//	Under water fog color in daylight

	float	fFogStart_n;	//	Fog start distance in night
	float	fFogEnd_n;		//	Fog end distance in night
	float	fFogDensity_n;	//	Fog density in night
	DWORD	dwFogColor_n;	//	Fog color in night

	float   fUWFogStart_n;	//	Under water fog start in night
	float   fUWFogEnd_n;	//	Under water fog end in night
	float   fUWFogDensity_n;//	Under water fog density in night
	DWORD   dwUWFogCol_n;	//	Under water fog color in night		

	int		iUseLightMethod;//	Use light method
	A3DVECTOR3	vLightDir;	//	Directional light's direction
	A3DVECTOR3  vLightDir_n;//  Directional light's direction in night

	DWORD	dwAmbient1;		//	Ambient color in daylight
	DWORD	dwDirLtCol;		//	Directional light color in daylight
	DWORD	dwAmbient1_n;	//	Ambient color in night
	DWORD	dwDirLtCol_n;	//	Directional light color in night
	DWORD	dwAmbient2;		//	Ambient color in daylight
	DWORD	dwAmbient2_n;		//	Ambient color in daylight

	float	vSkySpeed[2];	//	Sky moving speed on 2 axises
	DWORD	dwTrnLtScale;	//	Terrain light scale
	DWORD	dwTransTime;	//	Transform time

	int     iRainSpeed;		//	Rain properties
	float   fRainSize;
	DWORD   dwRainCol;
	int     iSnowSpeed;		//	Snow properties
	float   fSnowSize;
	DWORD   dwSnowCol;
	DWORD	dwCloudAmb;		//	Cloud ambient color in daylight
	DWORD	dwCloudDir;		//	Cloud direction light color in daylight
	DWORD	dwCloudAmb_n;	//	Cloud ambient color in night
	DWORD	dwCloudDir_n;	//	Cloud direction light color in night

	bool	bAffectTerrain;	// 是否影响地形材质
	DWORD	dwDiffuse;		// 反射光
	DWORD	dwSpecular;		// 高光
	float	fSpecularPower; // 高光强度
	float	fShadowEnergyDay;// 白天阴影浓度
	float	fShadowEnergyNight;// 晚上阴影浓度

	float	fPowerAmb1D;//环境光1和2颜色亮度
	float	fPowerAmb2D;//环境光1和2颜色亮度
	float	fPowerAmb1N;//环境光1和2颜色亮度
	float	fPowerAmb2N;//环境光1和2颜色亮度

// 	iDayTex0Len				// 白天天空纹理0的路径长度(int)
// 		szDayTex0				// 白天天空纹理0的路径(char[iDayTex0Len])
// 		iDayTex1Len				// 白天天空纹理1的路径长度(int)
// 		szDayTex1				// 白天天空纹理1的路径(char[iDayTex1Len])
// 		iDayTex2Len				// 白天天空纹理2的路径长度(int)
// 		szDayTex2				// 白天天空纹理2的路径(char[iNightTex2Len])
// 		iNightTex0Len			// 晚上天空纹理0的路径长度(int)
// 		szNightTex0				// 晚上天空纹理0的路径(char[iNightTex0Len])
// 		iNightTex1Len			// 晚上天空纹理1的路径长度(int)
// 		szNightTex1				// 晚上天空纹理1的路径(char[iNightTex1Len])
// 		iNightTex2Len			// 晚上天空纹理2的路径长度(int)
// 		szNightTex2				// 晚上天空纹理2的路径(char[iNightTex2Len])
};

struct ECBOXAREADATAFILE9 :public ECBOXAREADATAFILE8
{
	float	fPowerSunD;//太阳颜色亮度
	float	fPowerSunN;//太阳颜色亮度
};

struct ECBOXAREADATAFILE14
{
	int		iDefArea;		//	non-zero, is default area
	DWORD	dwAreaFlags;	//	Area flags
	A3DVECTOR3	vPos;		//	Area position
	A3DVECTOR3	vExt;		//	Area half extents
	A3DVECTOR3	vAxisX;		//	Area normalized x axis
	A3DVECTOR3	vAxisY;		//	Area normalized y axis
	A3DVECTOR3	vAxisZ;		//	Area normalized z axis

	float   fFogStart;		//	Fog start distance in daylight
	float   fFogEnd;		//	Fog end distance in daylight
	float   fFogDensity;	//	Fog density in daylight
	DWORD   dwFogColor;		//	Fog color in daylight

	float   fUWFogStart;	//	Under water fog start in daylight
	float   fUWFogEnd;		//	Under water fog end in daylight
	float   fUWFogDensity;	//	Under water fog density in daylight
	DWORD   dwUWFogCol;		//	Under water fog color in daylight

	float	fFogStart_n;	//	Fog start distance in night
	float	fFogEnd_n;		//	Fog end distance in night
	float	fFogDensity_n;	//	Fog density in night
	DWORD	dwFogColor_n;	//	Fog color in night

	float   fUWFogStart_n;	//	Under water fog start in night
	float   fUWFogEnd_n;	//	Under water fog end in night
	float   fUWFogDensity_n;//	Under water fog density in night
	DWORD   dwUWFogCol_n;	//	Under water fog color in night		

	int		iUseLightMethod;//	Use light method
	A3DVECTOR3	vLightDir;	//	Directional light's direction
	A3DVECTOR3  vLightDir_n;//  Directional light's direction in night

	DWORD	dwAmbient1;		//	Ambient color in daylight
	DWORD	dwDirLtCol;		//	Directional light color in daylight
	DWORD	dwAmbient1_n;	//	Ambient color in night
	DWORD	dwDirLtCol_n;	//	Directional light color in night
	DWORD	dwAmbient2;		//	Ambient color in daylight
	DWORD	dwAmbient2_n;		//	Ambient color in daylight

	float	vSkySpeed[2];	//	Sky moving speed on 2 axises
	DWORD	dwTrnLtScale;	//	Terrain light scale
	DWORD	dwTransTime;	//	Transform time

	int     iRainSpeed;		//	Rain properties
	float   fRainSize;
	DWORD   dwRainCol;
	int     iSnowSpeed;		//	Snow properties
	float   fSnowSize;
	DWORD   dwSnowCol;
	DWORD	dwCloudAmb;		//	Cloud ambient color in daylight
	DWORD	dwCloudDir;		//	Cloud direction light color in daylight
	DWORD	dwCloudAmb_n;	//	Cloud ambient color in night
	DWORD	dwCloudDir_n;	//	Cloud direction light color in night

	bool	bAffectTerrain;	// 是否影响地形材质
	DWORD	dwDiffuse;		// 反射光
	DWORD	dwSpecular;		// 高光
	float	fSpecularPower; // 高光强度

	float	fPowerAmb1D;//环境光1和2颜色亮度
	float	fPowerAmb2D;//环境光1和2颜色亮度
	float	fPowerAmb1N;//环境光1和2颜色亮度
	float	fPowerAmb2N;//环境光1和2颜色亮度

	float	fPowerSunD;//太阳颜色亮度
	float	fPowerSunN;//太阳颜色亮度

// 	iDayTex0Len				// 白天天空纹理0的路径长度(int)
// 		szDayTex0				// 白天天空纹理0的路径(char[iDayTex0Len])
// 
// 		iNightTex0Len			// 晚上天空纹理0的路径长度(int)
// 		szNightTex0				// 晚上天空纹理0的路径(char[iNightTex0Len])
};


struct ECBOXAREADATAFILE15
{
	int		iDefArea;		//	non-zero, is default area
	DWORD	dwAreaFlags;	//	Area flags
	A3DVECTOR3	vPos;		//	Area position
	A3DVECTOR3	vExt;		//	Area half extents
	A3DVECTOR3	vAxisX;		//	Area normalized x axis
	A3DVECTOR3	vAxisY;		//	Area normalized y axis
	A3DVECTOR3	vAxisZ;		//	Area normalized z axis

	float   fFogStart;		//	Fog start distance in daylight
	float   fFogEnd;		//	Fog end distance in daylight
	float   fFogDensity;	//	Fog density in daylight
	DWORD   dwFogColor;		//	Fog color in daylight

	float	fFogStart_n;	//	Fog start distance in night
	float	fFogEnd_n;		//	Fog end distance in night
	float	fFogDensity_n;	//	Fog density in night
	DWORD	dwFogColor_n;	//	Fog color in night	

	int		iUseLightMethod;//	Use light method
	A3DVECTOR3	vLightDir;	//	Directional light's direction
	A3DVECTOR3  vLightDir_n;//  Directional light's direction in night

	DWORD	dwAmbient1;		//	Ambient color in daylight
	DWORD	dwDirLtCol;		//	Directional light color in daylight
	DWORD	dwAmbient1_n;	//	Ambient color in night
	DWORD	dwDirLtCol_n;	//	Directional light color in night
	DWORD	dwAmbient2;		//	Ambient color in daylight
	DWORD	dwAmbient2_n;		//	Ambient color in daylight

	float	vSkySpeed[2];	//	Sky moving speed on 2 axises
	DWORD	dwTrnLtScale;	//	Terrain light scale
	DWORD	dwTransTime;	//	Transform time

	int     iRainSpeed;		//	Rain properties
	float   fRainSize;
	DWORD   dwRainCol;
	int     iSnowSpeed;		//	Snow properties
	float   fSnowSize;
	DWORD   dwSnowCol;
	DWORD	dwCloudAmb;		//	Cloud ambient color in daylight
	DWORD	dwCloudDir;		//	Cloud direction light color in daylight
	DWORD	dwCloudAmb_n;	//	Cloud ambient color in night
	DWORD	dwCloudDir_n;	//	Cloud direction light color in night

	bool	bAffectTerrain;	// 是否影响地形材质
	DWORD	dwDiffuse;		// 反射光
	DWORD	dwSpecular;		// 高光
	float	fSpecularPower; // 高光强度

	float	fPowerAmb1D;//环境光1和2颜色亮度
	float	fPowerAmb2D;//环境光1和2颜色亮度
	float	fPowerAmb1N;//环境光1和2颜色亮度
	float	fPowerAmb2N;//环境光1和2颜色亮度

	float	fPowerSunD;//太阳颜色亮度
	float	fPowerSunN;//太阳颜色亮度

};

struct ECBOXAREADATAFILE16 : public ECBOXAREADATAFILE15
{
	float	fPowerSunECM; //动画模型的太阳颜色亮度
	float	fPowerAmbECM; //动画模型的环境颜色亮度
};

#pragma pack(1)
struct ECBOXAREADATAFILE17
{
	int		iDefArea;		//	non-zero, is default area
	DWORD	dwAreaFlags;	//	Area flags
	A3DVECTOR3	vPos;		//	Area position
	A3DVECTOR3	vExt;		//	Area half extents
	A3DVECTOR3	vAxisX;		//	Area normalized x axis
	A3DVECTOR3	vAxisY;		//	Area normalized y axis
	A3DVECTOR3	vAxisZ;		//	Area normalized z axis

	float   fFogStart;		//	Fog start distance in daylight
	float   fFogEnd;		//	Fog end distance in daylight
	float   fFogDensity;	//	Fog density in daylight
	DWORD   dwFogColor;		//	Fog color in daylight

	float	fFogStart_n;	//	Fog start distance in night
	float	fFogEnd_n;		//	Fog end distance in night
	float	fFogDensity_n;	//	Fog density in night
	DWORD	dwFogColor_n;	//	Fog color in night	

	int		iUseLightMethod;//	Use light method
	A3DVECTOR3	vLightDir;	//	Directional light's direction
	A3DVECTOR3  vLightDir_n;//  Directional light's direction in night

	DWORD	dwAmbient1;		//	Ambient color in daylight
	DWORD	dwDirLtCol;		//	Directional light color in daylight
	DWORD	dwAmbient1_n;	//	Ambient color in night
	DWORD	dwDirLtCol_n;	//	Directional light color in night
	DWORD	dwAmbient2;		//	Ambient color in daylight
	DWORD	dwAmbient2_n;		//	Ambient color in daylight

	float	vSkySpeed[2];	//	Sky moving speed on 2 axises
	DWORD	dwTrnLtScale;	//	Terrain light scale
	DWORD	dwTransTime;	//	Transform time

	int     iRainSpeed;		//	Rain properties
	float   fRainSize;
	DWORD   dwRainCol;
	int     iSnowSpeed;		//	Snow properties
	float   fSnowSize;
	DWORD   dwSnowCol;
	DWORD	dwCloudAmb;		//	Cloud ambient color in daylight
	DWORD	dwCloudDir;		//	Cloud direction light color in daylight
	DWORD	dwCloudAmb_n;	//	Cloud ambient color in night
	DWORD	dwCloudDir_n;	//	Cloud direction light color in night

	bool	bAffectTerrain;	// 是否影响地形材质
	DWORD	dwDiffuse;		// 反射光
	DWORD	dwSpecular;		// 高光
	float	fSpecularPower; // 高光强度

	float	fPowerAmb1D;//环境光1和2颜色亮度
	float	fPowerAmb2D;//环境光1和2颜色亮度
	float	fPowerAmb1N;//环境光1和2颜色亮度
	float	fPowerAmb2N;//环境光1和2颜色亮度

	float	fPowerSunD;//太阳颜色亮度
	float	fPowerSunN;//太阳颜色亮度

	float	fPowerSunECM; //动画模型的太阳颜色亮度,白天
	float	fPowerAmbECM; //动画模型的环境颜色亮度,白天
};
struct ECBOXAREADATAFILE18:public ECBOXAREADATAFILE17
{
	float	fPowerSunECM_n;
	float   fPowerAmbECM_n;
};
#pragma pack()
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

// camera
struct ECCAMERAFILE
{
	A3DVECTOR3 vTarget;//目标点
	float fRotateYDeg;	//对Y轴旋转角度
	float fLevelAngelDeg;	//与XZ平面夹角
	float fDistance;		//观察点与目标点距离
	float fRollDeg;		//摄像机滚动角度
	float fFov;			//Field Of View
	float fFar;			//远截面
};

///////////////////////////////////////////////////////////////////////////

//	light, Version < 5
struct ECLIGHTFILE
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vDir;
	A3DVECTOR3	vUp;
	int         nType;			// 0:点光源   1:平行光    2:聚光灯
	float       fRange;			// 光线范围
	DWORD       dwLightColor;	// 散射光
	float       fFalloff;		// 发散系数
	float       fAttenuation0;  // 衰减0
	float       fAttenuation1;  // 衰减1
	float       fAttenuation2;  // 衰减2
	float       fTheta;			// 聚光灯内角
	float       fPhi;			// 聚光灯外角
	int         nEffectType;	//	灯光影响
};

//	Version >= 5
struct ECLIGHTFILE5
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vDir;
	A3DVECTOR3	vUp;
	int         nType;			// 0:点光源   1:平行光    2:聚光灯
	float       fRange;			// 光线范围
	DWORD       dwLightColor;	// 散射光
	float       fFalloff;		// 发散系数
	float       fAttenuation0;  // 衰减0
	float       fAttenuation1;  // 衰减1
	float       fAttenuation2;  // 衰减2
	float       fTheta;			// 聚光灯内角
	float       fPhi;			// 聚光灯外角
	int         nEffectType;	//	灯光影响
	bool		bDynamic;		// 是否动态光源
	float		fSpecFactor;	// 高光强度
};

struct ECLIGHTFILE6 : public ECLIGHTFILE5
{
	int			iWaveType;		//动态光波动类型
	int			iWavePeriod;		//动态光波动周期
	float		fRangeAmplitude;	//动态光范围波动系数 delta = m_fRangeAmplitude* m_fRange
	float		fPowerAmplitude;  //动态光颜色波动系数 delta = m_fPowerAmplitude *  m_dwLightColor;
};

#pragma pack(1)
struct ECLIGHTFILE7
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vDir;
	A3DVECTOR3	vUp;
	int         nType;			// 0:点光源   1:平行光    2:聚光灯
	float       fRange;			// 光线范围
	DWORD       dwLightColor;	// 散射光
	float       fFalloff;		// 发散系数
	float       fAttenuation0;  // 衰减0
	float       fAttenuation1;  // 衰减1
	float       fAttenuation2;  // 衰减2
	float       fTheta;			// 聚光灯内角
	float       fPhi;			// 聚光灯外角
	int         nEffectType;	//	灯光影响
	bool		bDynamic;		// 是否动态光源
	float		fSpecFactor;	// 高光强度
	int			iWaveType;		//动态光波动类型
	int			iWavePeriod;		//动态光波动周期
	float		fRangeAmplitude;	//动态光范围波动系数 delta = m_fRangeAmplitude* m_fRange
	float		fPowerAmplitude;  //动态光颜色波动系数 delta = m_fPowerAmplitude *  m_dwLightColor;
};
#pragma pack()

///////////////////////////////////////////////////////////////////////////

// occluder
struct ECOCCLUDERFILE
{
	A3DVECTOR3		vPos;	  //位置
	A3DVECTOR3		vDir;	  //
	A3DVECTOR3		vUp;	  //
	float			fLength;  //长
	float			fHeight;  //高
};

///////////////////////////////////////////////////////////////////////////

