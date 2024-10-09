#pragma once

//////////////////////////////////////////////////////////////////////////
//managed IPhysXObjBase class
//////////////////////////////////////////////////////////////////////////

class IPhysXObjBase;
#pragma make_public(IPhysXObjBase)

namespace APhysXCommonDNet
{
	ref class MActorBase;
	ref class MAttackActionInfo;

	public ref struct MPhysXRBDesc
	{
		MPhysXRBDesc()
		{
			mIsKinematic = false;
			mContactRBMass = mSkeletonRBMass = 0.0f;
			mContactPointVelocity.Clear();
			mContactRBLinearVelocity.Clear();
			mContactRBAngularVelocity.Clear();
		}
		MPhysXRBDesc(const PhysXRBDesc& desc)
		{
			mIsKinematic = desc.IsKinematic; 
			mContactRBMass = desc.ContactRBMass;
			mSkeletonRBMass = desc.SkeletonRBMass;
			mContactPointVelocity = Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(desc.ContactPointVelocity));
			mContactRBLinearVelocity = Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(desc.ContactRBLinearVelocity));
			mContactRBAngularVelocity = Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(desc.ContactRBAngularVelocity));
		}

		bool mIsKinematic; 
		float mContactRBMass;
		float mSkeletonRBMass;

		Vector3 mContactPointVelocity;
		Vector3 mContactRBLinearVelocity;
		Vector3 mContactRBAngularVelocity;
	};

	public interface class MIAssailable
	{
	public:
		virtual IAssailable* GetIAssailable();
		virtual MIObjBase^ GetObject();
		virtual MActorBase^ GetActorBase();
		virtual Vector3 GetCurrentPos();
		virtual Matrix4 GetGPose();
		virtual bool OnPreAttackedQuery();
		virtual void OnPreAttackedNotify(MAttackActionInfo^ pAttActInfo, MPhysXRBDesc^ pAttacker);
		virtual bool OnAttackedPreHitEvent(MActorBase^ pCaster, int attackType, unsigned int idxCasterTeam);
	};

	//according to IObjPropertyBase::DrivenMode
	public enum class MDrivenMode
	{
		DRIVEN_BY_PURE_PHYSX = 0,
		DRIVEN_BY_ANIMATION  = 1,		
		DRIVEN_BY_PART_PHYSX = 2,		
	};

	public ref class MIPhysXObjBase : public MIObjBase, public MIAssailable
	{
	public:
		virtual ~MIPhysXObjBase() {}
		
		//get file path of this object
		System::String^ GetObjFilePath();

		//driven mode operation
		void		SetDrivenModeEnforce(MDrivenMode dm);
		void		SetDrivenMode(MDrivenMode dm);
		MDrivenMode GetDrivenMode();
		bool	    QueryDrivenMode(MDrivenMode dm);
		System::String^ GetDrivenModeTextChinese();
		System::String^ GetDrivenModeText();

		virtual bool GetAABB(AABB% outAABB);
		virtual IAssailable* GetIAssailable() { return m_pPhysXBase; }
		virtual MIAssailable^ GetMIAssailable();
		virtual MIObjBase^ GetObject() { return this; }
		virtual MActorBase^ GetActorBase() { return nullptr; }
		virtual Vector3 GetCurrentPos() { return MIObjBase::GetGPos(); }
		virtual Matrix4 GetGPose() { return MIObjBase::GetGPose(); }
		virtual bool OnPreAttackedQuery() { return true; }
		virtual void OnPreAttackedNotify(MAttackActionInfo^ pAttActInfo, MPhysXRBDesc^ pAttacker) {}
		virtual bool OnAttackedPreHitEvent(MActorBase^ pCaster, int attackType, unsigned int idxCasterTeam) { return true; }

	internal:
		MIPhysXObjBase(IPhysXObjBase* pPhysXBase);
		MIPhysXObjBase(IPhysXObjBase* pPhysXBase, MIObjBase^ parent);
		
		MIPhysXObjBase();

	public:
		IPhysXObjBase* GetIPhysXObjBase() { return m_pPhysXBase; }

	protected:
		IPhysXObjBase* m_pPhysXBase;		

	};


}