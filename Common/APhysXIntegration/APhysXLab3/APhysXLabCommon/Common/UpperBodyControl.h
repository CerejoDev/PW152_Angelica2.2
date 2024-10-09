/*
* FILE: UpperBodyControl.h
*
* DESCRIPTION: upper body bend to some direction
*
* 
*
* HISTORY: 
*
* Copyright (c) 2012 Perfect World, All Rights Reserved.
*/
#pragma once

#include <A3DVector.h>
#include <A3DMatrix.h>
#include <AArray.h>

#include <A3DBoneController.h>

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

class A3DBoneRotController;
class A3DSkeleton;
class ApxActorBase;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class UpperBodyCtrl
//	
///////////////////////////////////////////////////////////////////////////

class BodyBoneCtrlInterface
{
public:
	BodyBoneCtrlInterface() {}
	virtual ~BodyBoneCtrlInterface() {}

	virtual void OnStartBoneCtrl() {}
	virtual void OnEndBoneCtrl() {}
};

class UpperBodyCtrl
{

public:		//	Types	
	enum State
	{
		Free,//自由状态
		Bend,//弯曲状态
		Renew,//由弯曲状态恢复到自由状态
	};

public:		//	Constructor and Destructor

	UpperBodyCtrl();
	virtual ~UpperBodyCtrl();

public:		//	Attributes

public:		//	Operations

	void Release();
	//	Initialize object
	bool BindPlayer(ApxActorBase* pPlayer);

	//	Tick routine
	void Tick(float fDeltaTime);

	//vBendDir弯向的水平方向，fBendDeg弯曲角度， fBendTime弯曲所用时间，fReTime恢复所用时间
	void BendBody(const A3DVECTOR3& vBendDir, float fBendDeg, float fBendTime, float fReTime);

	State GetState() const { return m_state; }

	void SetBoneCtrlInterface(BodyBoneCtrlInterface* pInterface) { m_pBoneCtrlInterface = pInterface; }

protected:	//	Attributes

	ApxActorBase*		m_pPlayer;			//	Player object

	BodyBoneCtrlInterface*  m_pBoneCtrlInterface;

	float           m_fTargetBendDeg;
	float           m_fBendDegCur;
	float           m_fBendDegVel;
	A3DVECTOR3      m_vBendDirH;
	float           m_fReTime;//bend to free 
	A3DVECTOR3      m_vRotAxis;

	State           m_state;

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

