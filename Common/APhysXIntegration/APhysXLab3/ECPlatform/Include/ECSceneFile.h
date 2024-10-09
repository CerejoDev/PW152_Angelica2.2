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
	int		iDayTex0Len // �����������0��·������(int)
	char*	szDayTex0// �����������0��·��(char[iDayTex0Len])
	int		iDayTex1Len // �����������1��·������(int)
	char*	szDayTex1// �����������1��·��(char[iDayTex1Len])
	int		iDayTex2Len // �����������2��·������(int)
	char*	szDayTex2// �����������2��·��(char[iNightTex2Len])
	int		iNightTex0Len // �����������0��·������(int)
	char*	szNightTex0// �����������0��·��(char[iNightTex0Len])
	int		iNightTex1Len // �����������1��·������(int)
	char*	szNightTex1// �����������1��·��(char[iNightTex1Len])
	int		iNightTex2Len // �����������2��·������(int)
	char*	szNightTex2// �����������2��·��(char[iNightTex2Len])
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
	int		iDayTex0Len // �����������0��·������(int)
	char*	szDayTex0// �����������0��·��(char[iDayTex0Len])
	int		iDayTex1Len // �����������1��·������(int)
	char*	szDayTex1// �����������1��·��(char[iDayTex1Len])
	int		iDayTex2Len // �����������2��·������(int)
	char*	szDayTex2// �����������2��·��(char[iNightTex2Len])
	int		iNightTex0Len // �����������0��·������(int)
	char*	szNightTex0// �����������0��·��(char[iNightTex0Len])
	int		iNightTex1Len // �����������1��·������(int)
	char*	szNightTex1// �����������1��·��(char[iNightTex1Len])
	int		iNightTex2Len // �����������2��·������(int)
	char*	szNightTex2// �����������2��·��(char[iNightTex2Len])
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

	bool	bAffectTerrain;	// �Ƿ�Ӱ����β���
	DWORD	dwDiffuse;		// �����
	DWORD	dwSpecular;		// �߹�
	float	fSpecularPower; // �߹�ǿ��
	float	fShadowEnergyDay;// ������ӰŨ��
	float	fShadowEnergyNight;// ������ӰŨ��

	float	fPowerAmb1D;//������1��2��ɫ����
	float	fPowerAmb2D;//������1��2��ɫ����
	float	fPowerAmb1N;//������1��2��ɫ����
	float	fPowerAmb2N;//������1��2��ɫ����

// 	iDayTex0Len				// �����������0��·������(int)
// 		szDayTex0				// �����������0��·��(char[iDayTex0Len])
// 		iDayTex1Len				// �����������1��·������(int)
// 		szDayTex1				// �����������1��·��(char[iDayTex1Len])
// 		iDayTex2Len				// �����������2��·������(int)
// 		szDayTex2				// �����������2��·��(char[iNightTex2Len])
// 		iNightTex0Len			// �����������0��·������(int)
// 		szNightTex0				// �����������0��·��(char[iNightTex0Len])
// 		iNightTex1Len			// �����������1��·������(int)
// 		szNightTex1				// �����������1��·��(char[iNightTex1Len])
// 		iNightTex2Len			// �����������2��·������(int)
// 		szNightTex2				// �����������2��·��(char[iNightTex2Len])
};

struct ECBOXAREADATAFILE9 :public ECBOXAREADATAFILE8
{
	float	fPowerSunD;//̫����ɫ����
	float	fPowerSunN;//̫����ɫ����
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

	bool	bAffectTerrain;	// �Ƿ�Ӱ����β���
	DWORD	dwDiffuse;		// �����
	DWORD	dwSpecular;		// �߹�
	float	fSpecularPower; // �߹�ǿ��

	float	fPowerAmb1D;//������1��2��ɫ����
	float	fPowerAmb2D;//������1��2��ɫ����
	float	fPowerAmb1N;//������1��2��ɫ����
	float	fPowerAmb2N;//������1��2��ɫ����

	float	fPowerSunD;//̫����ɫ����
	float	fPowerSunN;//̫����ɫ����

// 	iDayTex0Len				// �����������0��·������(int)
// 		szDayTex0				// �����������0��·��(char[iDayTex0Len])
// 
// 		iNightTex0Len			// �����������0��·������(int)
// 		szNightTex0				// �����������0��·��(char[iNightTex0Len])
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

	bool	bAffectTerrain;	// �Ƿ�Ӱ����β���
	DWORD	dwDiffuse;		// �����
	DWORD	dwSpecular;		// �߹�
	float	fSpecularPower; // �߹�ǿ��

	float	fPowerAmb1D;//������1��2��ɫ����
	float	fPowerAmb2D;//������1��2��ɫ����
	float	fPowerAmb1N;//������1��2��ɫ����
	float	fPowerAmb2N;//������1��2��ɫ����

	float	fPowerSunD;//̫����ɫ����
	float	fPowerSunN;//̫����ɫ����

};

struct ECBOXAREADATAFILE16 : public ECBOXAREADATAFILE15
{
	float	fPowerSunECM; //����ģ�͵�̫����ɫ����
	float	fPowerAmbECM; //����ģ�͵Ļ�����ɫ����
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

	bool	bAffectTerrain;	// �Ƿ�Ӱ����β���
	DWORD	dwDiffuse;		// �����
	DWORD	dwSpecular;		// �߹�
	float	fSpecularPower; // �߹�ǿ��

	float	fPowerAmb1D;//������1��2��ɫ����
	float	fPowerAmb2D;//������1��2��ɫ����
	float	fPowerAmb1N;//������1��2��ɫ����
	float	fPowerAmb2N;//������1��2��ɫ����

	float	fPowerSunD;//̫����ɫ����
	float	fPowerSunN;//̫����ɫ����

	float	fPowerSunECM; //����ģ�͵�̫����ɫ����,����
	float	fPowerAmbECM; //����ģ�͵Ļ�����ɫ����,����
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
	A3DVECTOR3 vTarget;//Ŀ���
	float fRotateYDeg;	//��Y����ת�Ƕ�
	float fLevelAngelDeg;	//��XZƽ��н�
	float fDistance;		//�۲����Ŀ������
	float fRollDeg;		//����������Ƕ�
	float fFov;			//Field Of View
	float fFar;			//Զ����
};

///////////////////////////////////////////////////////////////////////////

//	light, Version < 5
struct ECLIGHTFILE
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vDir;
	A3DVECTOR3	vUp;
	int         nType;			// 0:���Դ   1:ƽ�й�    2:�۹��
	float       fRange;			// ���߷�Χ
	DWORD       dwLightColor;	// ɢ���
	float       fFalloff;		// ��ɢϵ��
	float       fAttenuation0;  // ˥��0
	float       fAttenuation1;  // ˥��1
	float       fAttenuation2;  // ˥��2
	float       fTheta;			// �۹���ڽ�
	float       fPhi;			// �۹�����
	int         nEffectType;	//	�ƹ�Ӱ��
};

//	Version >= 5
struct ECLIGHTFILE5
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vDir;
	A3DVECTOR3	vUp;
	int         nType;			// 0:���Դ   1:ƽ�й�    2:�۹��
	float       fRange;			// ���߷�Χ
	DWORD       dwLightColor;	// ɢ���
	float       fFalloff;		// ��ɢϵ��
	float       fAttenuation0;  // ˥��0
	float       fAttenuation1;  // ˥��1
	float       fAttenuation2;  // ˥��2
	float       fTheta;			// �۹���ڽ�
	float       fPhi;			// �۹�����
	int         nEffectType;	//	�ƹ�Ӱ��
	bool		bDynamic;		// �Ƿ�̬��Դ
	float		fSpecFactor;	// �߹�ǿ��
};

struct ECLIGHTFILE6 : public ECLIGHTFILE5
{
	int			iWaveType;		//��̬�Ⲩ������
	int			iWavePeriod;		//��̬�Ⲩ������
	float		fRangeAmplitude;	//��̬�ⷶΧ����ϵ�� delta = m_fRangeAmplitude* m_fRange
	float		fPowerAmplitude;  //��̬����ɫ����ϵ�� delta = m_fPowerAmplitude *  m_dwLightColor;
};

#pragma pack(1)
struct ECLIGHTFILE7
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vDir;
	A3DVECTOR3	vUp;
	int         nType;			// 0:���Դ   1:ƽ�й�    2:�۹��
	float       fRange;			// ���߷�Χ
	DWORD       dwLightColor;	// ɢ���
	float       fFalloff;		// ��ɢϵ��
	float       fAttenuation0;  // ˥��0
	float       fAttenuation1;  // ˥��1
	float       fAttenuation2;  // ˥��2
	float       fTheta;			// �۹���ڽ�
	float       fPhi;			// �۹�����
	int         nEffectType;	//	�ƹ�Ӱ��
	bool		bDynamic;		// �Ƿ�̬��Դ
	float		fSpecFactor;	// �߹�ǿ��
	int			iWaveType;		//��̬�Ⲩ������
	int			iWavePeriod;		//��̬�Ⲩ������
	float		fRangeAmplitude;	//��̬�ⷶΧ����ϵ�� delta = m_fRangeAmplitude* m_fRange
	float		fPowerAmplitude;  //��̬����ɫ����ϵ�� delta = m_fPowerAmplitude *  m_dwLightColor;
};
#pragma pack()

///////////////////////////////////////////////////////////////////////////

// occluder
struct ECOCCLUDERFILE
{
	A3DVECTOR3		vPos;	  //λ��
	A3DVECTOR3		vDir;	  //
	A3DVECTOR3		vUp;	  //
	float			fLength;  //��
	float			fHeight;  //��
};

///////////////////////////////////////////////////////////////////////////

