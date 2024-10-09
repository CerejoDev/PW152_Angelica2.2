
/*----------------------------------------------------------------------

	ApxSkinModelActionController.h:

		Implement an action contoller using angelica's A3DSkinModel.

	Created by He Wenfeng, July 22, 2011

----------------------------------------------------------------------*/

#pragma once

#ifndef _APXSKINMODELACTIONCONTROLLER_H_
#define _APXSKINMODELACTIONCONTROLLER_H_

#define  APX_SKINMODEL_ACTIONCHANNEL_NUM 3

class A3DAnimNodeBlendBase;

class ApxSkinModelActionController: public ApxActionController
{
public:
	ApxSkinModelActionController(CPhysXObjDynamic* pDynObj):m_pDynObj(pDynObj)
	{
		if(m_pDynObj) 
			m_pSkinModel = m_pDynObj->GetSkinModel();
		else
			m_pSkinModel = NULL;

		for(int i = 0; i < APX_SKINMODEL_ACTIONCHANNEL_NUM; ++i)
		{
			m_ActionChannels[i] = -1;		// invalid channel...
		}

		m_pAnimBlend1 = NULL;
		m_pAnimBlend2 = NULL;
	}

	virtual void Init();
	virtual void Release();
	void ReleaseAminBlend();
	virtual void SetActionName(const char* strLAction, const char* strLInvAction, const char* strRAction, const char* strRInvAction,
		const char* strFAction, const char* strBAction, const char* strFWalk);

	virtual void SetActionLayer(ApxActionLayer apxActionLayer);

	virtual float GetCurActionSpan();
	virtual const char* GetFirstActionName() const;
	virtual const char* GetNextActionName() const;

	//fOffForwardDeg [-180, 180)
	virtual void SetMoveYaw(float fOffForwardDeg);
	virtual void SetSpeed(float fSpeed);
	virtual bool IsUseWalkRun() const { return m_bUseWalkRun; }
	virtual void SetUseWalkRun(bool bUseWalkRun);

	virtual void SetPlaySpeed(float fSpeed);

	int GetActionChannel(int iChannel) const { return m_ActionChannels[iChannel]; }

protected:
	virtual bool PlayAnim(const ApxActionAnimInfo& apxAnimInfo);
	virtual bool IsAnimStopped(const ApxActionAnimInfo& apxAnimInfo);
	virtual void OnChangeActiveChannel(float fBlendTime);

	virtual void PreTick(float dt);
	virtual void TickAnim(float dt);

protected:
	A3DSkinModel* m_pSkinModel;
	CPhysXObjDynamic* m_pDynObj;

	// the index 0 of the following array are not used in fact...
	int m_ActionChannels[APX_SKINMODEL_ACTIONCHANNEL_NUM];			// upper-body and lower-body channels
	ApxWeightBlender m_apxWB[APX_SKINMODEL_ACTIONCHANNEL_NUM];


	//blend walk left, right, forward, backward
	A3DAnimNodeBlendBase* m_pAnimBlend1;
	A3DAnimNodeBlendBase* m_pAnimBlend2;
	ApxWeightBlender m_apxWeight1[10];//blend AnimBlend1
	ApxWeightBlender m_apxWeight2[10];//blend AnimBlend2


	float m_fSpeed;
	bool m_bUseWalkRun;

#ifndef _ANGELICA3
	AString m_strCurrentActionName;
	float m_fCurActTotalTime;
#endif
};
#endif
