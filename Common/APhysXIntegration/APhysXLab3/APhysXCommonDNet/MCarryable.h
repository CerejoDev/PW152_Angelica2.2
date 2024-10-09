#pragma once

#include "Carryable.h"


namespace APhysXCommonDNet
{

	ref class MActorBase;
	ref class MActorActionReport;

	public enum class MCarryableState
	{
		//according to CCarryable::State
		Free,		//����ģʽ
		ToFree,     //��Ҫ���ɣ����ӳ�˲�䣬���ܰ󶨹�����Ӱ��
		Hold,       //�������ܰ󶨹�����Ӱ��
		ToHold,     //��Ҫ�����𣬲��ܰ󶨹���Ӱ��
	};
	class CCarryableImp;
	public ref class MCarryable
	{
	internal:
		//get internal pointer
		CCarryable* GetCarryable();

	public:
		//constructor & destructor
		MCarryable();
		virtual ~MCarryable();
		!MCarryable();

		//to do override it
		virtual void OnStateChanged(MCarryableState oldState, MCarryableState newState);

		//////////////////////////////////////////////////////////////////////////
		//wrap function from CCarryable
		bool Init(MIPhysXObjBase^ pPhysXObj);
		void Release();

		bool GetPos(Vector3% vPos);
		void SetPos(Vector3 vPos);
		bool GetDirAndUp(Vector3% vDir, Vector3% vUp);
		void SetDirAndUp(Vector3 vDir, Vector3 vUp);

		//����actionReport
		void SetActorActionReport(MActorActionReport^ pActorActionReport);

		MCarryableState GetState();

		bool GetModelAABB(AABB% aabb);

		property MIPhysXObjBase^ PhysXObjBase
		{
			MIPhysXObjBase^ get() { return m_pPhysXObj; } 
		}

	protected:
		CCarryableImp* m_pCarryable;
		MActorActionReport^ m_pActorActionReport;

		MIPhysXObjBase^     m_pPhysXObj;
	};
	
	class CCarryableImp : public CCarryable
	{
	public:
		//constructor & destructor
		CCarryableImp();
		virtual ~CCarryableImp();
		
		void SetMCarryablePtr(MCarryable^ pMCarryable);

	protected:
		virtual void OnStateChanged(CCarryable::State oldState, CCarryable::State newState);

	protected:
		gcroot<MCarryable^> m_pMCarryable;
	};

}