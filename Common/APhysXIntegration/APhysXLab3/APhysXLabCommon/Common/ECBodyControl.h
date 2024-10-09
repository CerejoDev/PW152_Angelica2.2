/*
 * FILE: ECBodyControl.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2007/8/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2007 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DVector.h>
#include <A3DMatrix.h>
#include <AArray.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_HEADBODYDELTA	90.0f	//	Degree

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DBoneBodyTurnController;
class A3DSkeleton;
class ApxActorBase;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECPlayerBodyCtrl
//	
///////////////////////////////////////////////////////////////////////////

//	Body yaw flag used by SetHeadYaw
enum
{
	EC_DOYAW_CW = 0,	//	Yaw clockwise
	EC_DOYAW_CCW,		//	Yaw counter-clockwise
	EC_DOYAW_NEAR,		//	Yaw on near direction
};

A3DVECTOR3 glb_YawToVector(float fYaw);
void glb_ClampYaw(float& fYaw);
float glb_YawLessSub(float fYaw1, float fYaw2);
float glb_DoYaw(float fCurYaw, float fDestYaw, float fSpeed, float fTime, int iYawFlag/* EC_DOYAW_NEAR */);
float glb_VectorToYaw(const A3DVECTOR3& v);
void glb_ClampYaw(float& fYaw);

class ECPlayerBodyCtrl
{
public:		//	Types	

	//	Turn body data
	struct TURN_VALUE
	{
		bool	bTurn;		//	true, turn body
		int		iYawFlag;	//	Yaw flag
		float	fDestYew;	//	Destination yaw
		float	fSpeed;		//	Turn speed, degree / second
	};


public:		//	Constructor and Destructor
	ECPlayerBodyCtrl();
	virtual ~ECPlayerBodyCtrl();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool BindPlayer(ApxActorBase* pPlayer);
	void Release();

	//	Tick routine
	void Tick(float fDeltaTime);

	//	Rotate body
	void RotateBody(int iYawFlag, float fDestYaw, float fSpeed);
	//	Rotate Head
	void RotateHead(int iYawFlag, float fHeadDestYaw,float fBodyDestYaw,  float fSpeed);


	//	Set / Get head yaw
	float GetHeadYaw() { return m_fHeadYaw; }
	float GetDestBodyYaw() { return m_TurnBody.fDestYew; }
	//	Set / Get body yaw
	float GetBodyYaw() { return m_fBodyYaw; }
	//	Enable body follow flag
	void EnableBodyFollow(bool bEnable) { m_bBodyFollow = bEnable; }
	//	Is body follow flag enabled ?
	bool GetBodyFollowFlag() { return m_bBodyFollow; }
	//	Set head speed
	void SetHeadSpeed(float fSpeed) { m_fHeadSpeedYaw = fSpeed; }
	//	Add head speed (注： 响应MouseMove的函数由主线程触发，而我们的Tick在逻辑线程中执行所以要将 速度基类起来)
	void AddHeadSpeed(float fSpeed) { m_fHeadSpeedYaw += fSpeed; }
	//	Get stamp current yaw for net msg
	float GetStampYaw() { return m_fStampYaw; }
	//	Get stamp speed for net msg
	float GetStampSpeed() { return m_fStampSpd; }
	//	Set head yaw
	void SetHeadYaw(float fHeadYaw) { m_fHeadYaw = fHeadYaw; }

	void ResetBoneCtrl();

protected:	//	Attributes

	ApxActorBase*		m_pPlayer;			//	Player object
	float			m_fHeadYaw;			//	Head direction represented by yaw degree
	float			m_fBodyYaw;			//	Body direction represented by yaw degree
	bool			m_bBodyFollow;		//	true, body yaw will chase head yaw until they are same
	TURN_VALUE		m_TurnBody;			//	Turn body data
	TURN_VALUE		m_TurnHead;
	float			m_fOldHeadYaw;		//	Turn right or left from this value now

	float			m_fHeadSpeedYaw;	//	用该将老版本中在响应消息里转头放到Tick里处理
	float			m_fStampYaw;		//	For move message
	float			m_fStampSpd;		//	For move message

	A3DBoneBodyTurnController*	m_pHeadCtrl;	//	Head controller
	A3DBoneBodyTurnController*	m_pNeckCtrl;	//	Neck controller
	A3DBoneBodyTurnController*	m_pSpineCtrl0;	//	Spine controller
	A3DBoneBodyTurnController*	m_pSpineCtrl1;	//	Spine1 controller
	A3DBoneBodyTurnController*	m_pSpineCtrl2;	//	Spine2 controller
	A3DBoneBodyTurnController*	m_pSpineCtrl3;	//	Spine3 controller
	A3DBoneBodyTurnController*	m_pSpineCtrl4;	//	Spine4 controller

	A3DBone*	m_pHead;	//	Head 
	A3DBone*	m_pNeck;	//	Neck 
	A3DBone*	m_pSpine0;	//	Spine 
	A3DBone*	m_pSpine1;	//	Spine1 
	A3DBone*	m_pSpine2;	//	Spine2 
	A3DBone*	m_pSpine3;	//	Spine3 
	A3DBone*	m_pSpine4;	//	Spine4 

protected:	//	Operations
	//	Create look at controller for specified bone and set reference axis for it
	A3DBoneBodyTurnController* CreateBoneController(A3DSkeleton* pSkeleton, const char* szBone, A3DBone* &pBone);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

