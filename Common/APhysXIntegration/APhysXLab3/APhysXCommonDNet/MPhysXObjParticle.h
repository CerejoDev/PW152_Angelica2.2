#pragma once

//wrapper for CPhysXObjParticle
class CPhysXObjParticle;

namespace APhysXCommonDNet
{

	public ref class MPhysXObjParticle : MIPhysXObjBase
	{
	public:
		bool ControlState(bool EnablePlay);

	internal:
		MPhysXObjParticle(CPhysXObjParticle* pPhysXBase);
		MPhysXObjParticle(CPhysXObjParticle* pParticle, MIObjBase^ parent);
		MPhysXObjParticle();

	private:
		CPhysXObjParticle* m_pParticle;

	};

}