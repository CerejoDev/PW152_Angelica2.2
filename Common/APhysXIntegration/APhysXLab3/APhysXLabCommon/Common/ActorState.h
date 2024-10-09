/*
* FILE: ActorState.h
*
* DESCRIPTION: used for actor state translate
*
* animation & ragdoll
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


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ActorState, State of apxActorBase 
//	
///////////////////////////////////////////////////////////////////////////

class ApxActorBase;

class ActorState
{
	

public:		//	Types	
	enum State
	{
		Normal,//animation
		Ragdoll,//
		Ragdoll2Normal,
	};

public:		//	Constructor and Destructor

	ActorState();
	virtual ~ActorState();

public:		//	Attributes

public:		//	Operations

	void Release();
	//	Initialize object
	bool BindPlayer(ApxActorBase* pPlayer);

	//	Tick routine
	void Tick(float fDeltaTime);

	//trans to normal or ragdoll
	bool TranState(State state, float fTransTime = 0.0f);

	State GetState() const { return m_state; }

protected:	//	Attributes

	ApxActorBase*		m_pPlayer;			//	Player object

	State           m_state;        //��ǰ״̬

	float           m_fTransTime; //ragdoll to normal trans time
	float           m_fTransCnt;  //ragdoll to normal trans time count

	float           m_fGetUpTime;//����������ʱ��
	float           m_fGetUpCnt;//�������ۼ�ʱ��
	bool            m_bGetUpAnim;//��ʼ����������

	//the following two variables for ragdoll to animation
	abase::vector<A3DQUATERNION> m_QuatsRagdoll;//bone snap for ragdoll
	abase::vector<A3DQUATERNION> m_QuatsTarget;//bone snap for target pose
	abase::vector<A3DMATRIX4>    m_matTarget;//relative target matrix
	abase::vector<A3DMATRIX4>    m_matRagdoll;//relative matrix ragdoll

	abase::vector<AString> m_strAnim;//�����ö���
	int                    m_iAnim;//������index
	
	A3DVECTOR3             m_vPosRagdoll;
	A3DVECTOR3             m_vPosTarget;

	A3DBoneMatrixController* m_pPelvisCtr;//pelvis controller
	A3DBone*                 m_pPelvisBone;

	A3DMATRIX4 m_matRagdollPelvis;
	A3DMATRIX4 m_matTargetPelvis;
	A3DMATRIX4 m_matTargetBip01;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

