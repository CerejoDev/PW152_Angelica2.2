//created by yx, 2011/8/18
//to support IK

#pragma once
#ifndef _APHYSXLAB_APPINTERFACE_H_
#define _APHYSXLAB_APPINTERFACE_H_

#ifdef _ANGELICA3

#include <A3DAppInterface.h>
class A3DTerrain2;

class AppInterface : public A3DAppInterface
{
public:
	static AppInterface* GetInstance();

	void SetTerrain(A3DTerrain2* pTerr) { m_pTerrain = pTerr; }
	void Render(A3DWireCollector* pWC) const;
	virtual bool GetIKGroundPosAndNormal(const A3DVECTOR3& vRefPos, A3DVECTOR3& vGndPos, A3DVECTOR3& vGndNormal);

protected:
	AppInterface();

public:
	A3DVECTOR3 m_vLastRefPosL;
	A3DVECTOR3 m_vLastRefPosR;

	A3DVECTOR3 m_vLastRefDirL;
	A3DVECTOR3 m_vLastRefDirR;

protected:
	int m_i;
	A3DTerrain2* m_pTerrain;
};

#else

#include "A3DIKGoal.h"

class IKInterfaceImp : public A3DIKInterface
{

public:
	IKInterfaceImp() { m_HasPhysXCC = false; m_pTerrain = 0; }
	virtual bool GetIKGroundPosAndNormal(const A3DVECTOR3& vRefPos, A3DVECTOR3& vGndPos, A3DVECTOR3& vGndNormal, float fUpDist = 1.0f, float fDownDist = 1.0f, void* IKUserData = NULL);
	void SetTerrain(A3DTerrain2* pTerr) { m_pTerrain = pTerr; }
	void HasPhysXCCNotify(bool bHasPhysXCC) { m_HasPhysXCC = bHasPhysXCC; }

protected:
	bool m_HasPhysXCC;
	A3DTerrain2* m_pTerrain;
};

#endif		// #ifdef _ANGELICA3

#endif
