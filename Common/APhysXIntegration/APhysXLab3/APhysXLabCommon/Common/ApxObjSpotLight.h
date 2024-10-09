
/*----------------------------------------------------------------------

	ApxObjSpotLight.h:

	Define the SpotLight object 

	Created by He Wenfeng, Dec. 8, 2011

----------------------------------------------------------------------*/



#ifndef _APHYSXLAB_APXOBJSPOTLIGHT_H_
#define _APHYSXLAB_APXOBJSPOTLIGHT_H_

#ifdef _ANGELICA3
	#include "A3DColorValue.h"
	class A3DLPPLightSpot;
#endif

class ApxObjSpotLight : public IObjBase, public IPropObjBase
{
public:	
	static APhysXU32 GetVersion() { return 0xAA000002; }

	ApxObjSpotLight(int UID = -1);
	virtual ~ApxObjSpotLight(); 
	ApxObjSpotLight& operator= (const ApxObjSpotLight& rhs);

	float GetRange() const { return m_Range; }
	float GetInnerAngle() const { return m_AngleDegreeTheta; }
	float GetOuterAngle() const { return m_AngleDegreePhi; }
	A3DVECTOR3 GetAttenuation() const { return m_Attenuation; }
	A3DCOLORVALUE GetColor() const { return m_color; }

	void SetRange(float fRange);
	void SetInnerAngle(float fInnerAngleDegree);
	void SetOuterAngle(float fOuterAngleDegree);
	void SetAttenuation(const A3DVECTOR3& attenuation);
	void SetColor(const A3DCOLORVALUE& color);

	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid();
	virtual bool OnPropertyChanged(PropItem& prop);
	virtual void OnUpdatePropsGrid();

	virtual void Enable(bool bEnable); 
	virtual bool IsEnabled() const;  

protected:
	ApxObjSpotLight(const ApxObjSpotLight& rhs) : IPropObjBase(rhs) {}
	virtual ApxObjSpotLight* CreateMyselfInstance(bool snapValues) const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePostNotify();
	virtual void OnTick(float dt);
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime);
	
	virtual void OnSetScene(Scene* pScene);
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return true; }
	virtual void OnLeaveRuntime() {}
	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale) { return false; }

private:
#ifdef _ANGELICA3
	bool CreateSpotLight();
#else
	bool CreateSpotLight(A3DLightMan& LightMgr);
#endif

	virtual ApxObjSpotLight* NewObjAndCloneData() const;
	virtual void OnDoSleepTask();
	virtual bool OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

private:
	NxBounds3 m_aabb;
#ifdef _ANGELICA3
	A3DLPPLightSpot* m_pLPPSpotLight;
#else
	A3DLight* m_pSpotLight;
#endif

	float m_Range;
	float m_AngleDegreeTheta;
	float m_AngleDegreePhi;
	A3DVECTOR3 m_Attenuation;
	A3DCOLORVALUE m_color;
};
#endif