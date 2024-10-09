#pragma once

#include "Pushable.h"
namespace APhysXCommonDNet
{
	public ref class MPushable
	{
	internal:
		Pushable* GetPushable() { return m_pPushable; }

	public:
		MPushable();
		virtual ~MPushable();
		!MPushable();

		void Init(MIPhysXObjBase^ pObj);
		void Release();

		MIPhysXObjBase^ GetMIPhysXObjBase() { return m_pPhysXObj; }

		property MIPhysXObjBase^ PhysXObjBase
		{
			MIPhysXObjBase^ get() { return m_pPhysXObj; } 
		}

		bool IsPushing() { return m_pPushable->IsPushing(); }

	protected:
		Pushable* m_pPushable;
		MIPhysXObjBase^     m_pPhysXObj;
	};
	

}