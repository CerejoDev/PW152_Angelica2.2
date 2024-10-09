//--------------------------------------------------------------------
//FileName: ECSunMoonInterface.h
//Created by liyi 2011, 3, 2
//--------------------------------------------------------------------

#ifndef _EC_SUN_MOON_INTERFACE_H_
#define _EC_SUN_MOON_INTERFACE_H_

#include "A3DTypes.h"

class A3DViewport;
class ECGraphicsModule;

#define SUN_UNIT_SIZE			60.0f
#define MOON_UNIT_SIZE			100.0f

#define SUN_MAX_PITCH			(DEG2RAD(70.0f))
#define MOON_MAX_PITCH			(DEG2RAD(60.0f))

#define DAWN_TIME				(3.5f / 24.0f)
#define MID_NOON				(12.0f / 24.0f)
#define SUN_SET					(21.0f / 24.0f)
#define MID_NIGHT				(24.0f / 24.0f)

#define NIGHT_DAY_START			(3.0f / 24.0f)
#define NIGHT_SUN_RISE_MIN		(4.0f / 24.0f)
#define NIGHT_DAY_END			(7.0f / 24.0f)
#define DAY_NIGHT_START			(18.0f / 24.0f)
#define DAY_SUN_SET_MAX			(19.5f / 24.0f)
#define DAY_NIGHT_END			(21.0f / 24.0f)

#define TIME_SCALE				(6.0f)

#define SKY_FAR					600.0f

enum SunMoonType
{
	SUN_MOON_OLD = 0,
	SUN_MOON_A3
};

class ECSunMoonInterface
{
public:
	virtual ~ECSunMoonInterface() {};

	static ECSunMoonInterface* CreateSunMoonInterface(SunMoonType type = SUN_MOON_OLD);
	
	// Initialize and finalize
	virtual bool Init(ECGraphicsModule* pGraphics) = 0;
	virtual bool Release() = 0;

	// render and tick
	virtual bool Render(A3DViewport * pViewport) = 0;
	virtual bool RenderSolarFlare(A3DViewport * pViewport) = 0;
	virtual bool Tick(DWORD dwDeltaTime) = 0;

	// time adjust
	virtual bool SetTimeOfTheDay(float vTime) = 0;
	virtual float GetTimeOfTheDay() = 0;

	// sun moon alpha adjust
	virtual bool SetSunColor(const A3DCOLORVALUE& color) = 0;
	virtual bool SetMoonColor(const A3DCOLORVALUE& color) = 0;

	//	Set/Get sun light's direction
	//	bOnlySetLongitude: true, only change sun's longitude and keep sun's height unchanged
	virtual bool SetLightDir(const A3DVECTOR3& vecLightDir, bool bOnlySetLongitude=false) = 0;
	virtual const A3DVECTOR3& GetLightDir() = 0;

	virtual const A3DCOLORVALUE& GetSunColor() = 0;
	virtual const A3DCOLORVALUE& GetMoonColor() = 0;

	virtual float GetSunPitch() = 0;
	virtual float GetMoonPitch() = 0;

	virtual float GetDNFactor() = 0;
	virtual float GetDNFactorDest() = 0;

	virtual bool IsSunVisible() = 0;
	virtual void SetSunVisible(bool bFlag) = 0;
	virtual bool IsMoonVisible() = 0;
	virtual void SetMoonVisible(bool bFlag) = 0;
	virtual bool IsSunFlareOn() = 0;
	virtual void SetSunFlareOn(bool bFlag) = 0;

	virtual bool IsSunTime() = 0;

	// get day night trans time in millisecond.
	virtual int GetDNTransTime() = 0;


};

#endif //_EC_SUN_MOON_INTERFACE_H_