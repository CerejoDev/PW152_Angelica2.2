//wrapper for MainActor

#pragma once

class CMainActor;
class IRoleActor;
#include "MActorBase.h"
namespace APhysXCommonDNet
{	
	value class Vector3;
	ref class MIPhysXObjBase;

	public ref class MIRoleActor
	{
	internal:
		MIRoleActor(IRoleActor* pActor, MIObjBase^ pObjDyn);

	public:
		!MIRoleActor() {}
		~MIRoleActor();

		MActorBase^ GetMActorBase() { return m_pMActorBase; }
		MIObjBase^ GetPhysXObjBase() { return m_pObjBase; }
		IRoleActor* GetRoleActor() { return m_pRActor; }

	private:
		IRoleActor* m_pRActor;
		MIObjBase^  m_pObjBase;
		MActorBase^	m_pMActorBase;
	};


	public enum class MCCType
	{
		//according to CAPhysXCCMgr::CCType
		// For compatible old archive...
		// Do NOT change the order, just add new items following the sequence 
		CC_UNKNOWN_TYPE	= 0,
		CC_TOTAL_EMPTY	= 1,
		CC_APHYSX_LWCC	= 2,
		CC_APHYSX_NXCC	= 3,
		CC_APHYSX_EXTCC	= 4,
		CC_APHYSX_DYNCC	= 5,
		CC_BRUSH_CDRCC	= 6,
	};

	public ref class MMainActor
	{
	private:
		CMainActor* _pActor;
		MIPhysXObjBase^ _pObjBase;
		MActorBase^		m_pMActorBase;

	internal:
		MMainActor(CMainActor* pActor, MPhysXObjDynamic^ pObjDyn);
	
	public:
		!MMainActor();
		~MMainActor();

		void RegisterKeyEvent();
		void UnRegisterKeyEvent();

		MIPhysXObjBase^ GetPhysXObjBase() { return _pObjBase; }
		MActorBase^ GetMActorBase() { return m_pMActorBase; }
		CMainActor* GetMainActor() { return _pActor; }

		Vector3 GetPos();
		System::String^ GetCCTypeText();

		void SetLButtonDown();
		void SetLButtonUp();
		void UpdateTargetPT(int x, int y);

		void WalkTo(Vector3 pos);
		bool IsInAir();
		bool IsKeepWalking();
		bool IsRunEnable();
		void EnableRun(bool bEnable);
		void Jump(float fJumpSpeed);

		Vector3 GetDeltaMove();

		void SwitchCCType(MCCType ct);
		void SwitchToRagDoll();

		void SetMActionReport(MActorActionReport^ pActionReport);
		void EnterCameraBinding(bool bIsKeepDir);
	};

}