#pragma once

//wrapper for CPhysXObjDynamic
class CPhysXObjDynamic;

namespace APhysXCommonDNet
{
	ref class MIPhysXObjBase;
	value class Vector3;

	public enum class MCollisionChannel
	{
		CNL_UNKNOWN				= 0,
		CNL_COMMON				= 1,
		CNL_INDEPENDENT			= 2,
		CNL_INDEPENDENT_CCHOST  = 3,
	};

	public ref class MPhysXObjDynamic : MIPhysXObjBase
	{
	public:
		bool ApplyPhysXBody(bool bEnable);
		void SetLinearVelocity(Vector3 v);
		void AddForce(Vector3 pt, Vector3 dirANDmag, int iPhysForceType, float fSweepRadius);

		bool SetCollisionChannel(MCollisionChannel cnl);
		bool GetCollisionChannel(MCollisionChannel% outChannel);

	internal:
		MPhysXObjDynamic(CPhysXObjDynamic* pDynamic);
		MPhysXObjDynamic(CPhysXObjDynamic* pDynamic, MIObjBase^ parent);
		MPhysXObjDynamic();

	private:
		CPhysXObjDynamic* m_pDynamic;

	};

}