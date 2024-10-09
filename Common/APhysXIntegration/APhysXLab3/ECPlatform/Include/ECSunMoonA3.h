//--------------------------------------------------------------------
//FileName: ECSunMoonA3.h
//Created by liyi 2011, 3, 2
//--------------------------------------------------------------------
#ifndef _EC_SUN_MOON_A3_H_
#define _EC_SUN_MOON_A3_H_

#include "ECSunMoonInterface.h"
#include "A3DMtlInstance.h"

class A3DStream;
class A3DDevice;
class A3DFXMaterial;
class A3DFXParam;
class A3DFXMaterialMan;
class ECGraphicsModule;
class A3DEngine;
class A3DVertexDecl;
class A3DRenderTarget;

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECSunMoonMtlInst
//	
///////////////////////////////////////////////////////////////////////////

class ECSunMoonMtlInst : public A3DMtlInstance
{
public:		//	Types

public:		//	Constructor and Destructor

	ECSunMoonMtlInst();
	virtual ~ECSunMoonMtlInst();

public:		//	Attributes

	A3DFXParam*		pParamSunWVP;
	A3DFXParam*		pParamSunColor;
	A3DFXParam*		pParamSunVisibility;
	A3DFXParam*		pParamSunShineColor;
	A3DFXParam*		pParamMoonColor;
	A3DFXParam*		pParamMoonVisibility;
	//	Flare alpha
	A3DFXParam*		pParamGBufferTex;
	A3DFXParam*		pParamRadius;
	A3DFXParam*		pParamCenter;
	A3DFXParam*		pParamGBufferWidth;
	A3DFXParam*		pParamGBufferHeight;
	A3DFXParam*		pParamAlpha;
	//	Occlusion
	A3DFXParam*		pParamOcclusionTex;

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();
	//	Re-Compile FX material
	virtual bool ReCompile();

protected:	//	Attributes

	A3DEngine*	m_pA3DEngine;

protected:	//	Operations

};

class ECSunMoonA3 : public ECSunMoonInterface
{
public:

	ECSunMoonA3();
	~ECSunMoonA3();

	//Interface of ECSunMoonInterface

	// Initialize and finalize
	virtual bool Init(ECGraphicsModule* pGraphics);
	virtual bool Release();

	// render and tick
	virtual bool Render(A3DViewport * pViewport);
	virtual bool RenderSolarFlare(A3DViewport * pViewport);
	virtual bool Tick(DWORD dwDeltaTime) ;


	//Util Func
	// time adjust
	virtual bool SetTimeOfTheDay(float vTime);
	virtual inline float GetTimeOfTheDay(){ return (float)m_vTimeOfTheDay; }

	// sun moon alpha adjust
	virtual inline bool SetSunColor(const A3DCOLORVALUE& color) { m_colorSun = color; return true;}
	virtual inline bool SetMoonColor(const A3DCOLORVALUE& color) { m_colorMoon = color ; return true;}

	//	Set/Get sun light's direction
	//	bOnlySetLongitude: true, only change sun's longitude and keep sun's height unchanged
	virtual bool SetLightDir(const A3DVECTOR3& vecLightDir, bool bOnlySetLongitude=false);
	virtual inline const A3DVECTOR3& GetLightDir() { return m_vecLightDir;}

	virtual inline const A3DCOLORVALUE& GetSunColor() {return m_colorSun;}
	virtual inline const A3DCOLORVALUE& GetMoonColor() {return m_colorMoon;}

	virtual inline float GetSunPitch() { return m_vSunPitch;}
	virtual inline float GetMoonPitch() { return m_vMoonPitch;}

	virtual inline float GetDNFactor() { return m_fDNFactor;}
	virtual inline float GetDNFactorDest() { return m_fDNFactorDest;}

	virtual inline bool IsSunVisible() { return m_bSunVisible;}
	virtual inline void SetSunVisible(bool bFlag) { m_bSunVisible = bFlag;}
	virtual inline bool IsMoonVisible() { return m_bMoonVisible;}
	virtual inline void SetMoonVisible(bool bFlag) { m_bMoonVisible = bFlag;}
	virtual inline bool IsSunFlareOn() { return m_bSunFlareOn; }
	virtual inline void SetSunFlareOn(bool bFlag) { m_bSunFlareOn = bFlag; }

	virtual inline bool IsSunTime() {return m_vSunPitch > 0.0f;}

	// get day night trans time in millisecond.
	virtual int GetDNTransTime();

protected:
	bool UpdateWithTime();
	bool CreateSunStream(A3DDevice* pA3DDevice);
	bool CreateMoonStream(A3DDevice* pA3DDevice);
	bool CreateSunShineStream(A3DDevice* pA3DDevice);

	bool InitFlare();
	bool ReleaseFlare();
	bool UpdateLensFlares(A3DViewport * pViewport, const A3DVECTOR3 vecCamPos, const A3DVECTOR3 vecCamDir, const A3DVECTOR3 vecSunPos);

protected:

	A3DStream*			m_pStreamSun;
	A3DStream*			m_pStreamMoon;
	A3DStream*			m_pStreamSunShine;
	A3DStream*			m_pStreamFlares;
	A3DStream *			m_pStreamFlareAlpha;	// stream stand for alpha accumulate

	A3DRenderTarget *	m_pFlareAlphaTarget;	// render target to store flare's alpha
	bool				m_bCanDoFlare;
	bool				m_bSunFlareOn;			//	true, Sun flare enabled

	float				m_vSunSize;				// the size of the sun
	float				m_vMoonSize;			// the size of the moon

	A3DCOLORVALUE		m_colorSun;				// color of sun
	A3DCOLORVALUE		m_colorMoon;			// color of moon

	float				m_vSunPitch;			// the pitch of the sun
	float				m_vMoonPitch;			// the pitch of the moon
	A3DVECTOR3			m_vecLightDir;			// light direction

	float				m_vDeg;					// the degree of the light source

	double				m_vTimeOfTheDay;		// time of the day 0.0f means 00:00, 1.0f means 24:00
	float				m_fDNFactor;			// day or night factor
	float				m_fDNFactorDest;		// day or night factor dest

	bool				m_bTimeHasBeenSet;		// flag indicates time has been set
	bool				m_bDirHasBeenSet;		// flag indicates dir has been set
	bool				m_bFirstAlpha;			// flag indicates alpha target has not been cleared yet.

	float				m_vSunVisibility;		// sun visibility factor
	float				m_vMoonVisibility;		// moon visibility factor

	bool				m_bSunVisible;			// sun visible flag
	bool				m_bMoonVisible;			// moon visible flag

	ECGraphicsModule*	m_pGraphics;			// Graphics module

	A3DFXMaterialMan*	m_pFXMaterialMan;
	ECSunMoonMtlInst*   m_pMtlInst;
	A3DVertexDecl*		m_pVertDeclSunMoon;
	A3DVertexDecl*		m_pVertDeclFlare;
};

#endif //_EC_SUN_MOON_A3_H_
