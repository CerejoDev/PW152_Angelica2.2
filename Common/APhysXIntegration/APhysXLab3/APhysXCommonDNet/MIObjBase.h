#pragma once

//////////////////////////////////////////////////////////////////////////
//managed wrapper fro IObjBase class
//////////////////////////////////////////////////////////////////////////

class IObjBase;

namespace APhysXCommonDNet
{
	value class Vector3;
	value class Matrix4;
	ref   class MIScriptBehavior;
	ref   class MRender;
	value class AABB;
	value struct MPhysRay;
	value struct MPhysRayTraceHit;

	public ref class MIObjBase
	{
	public:
		virtual ~MIObjBase() {}
		
		Vector3 GetGPos();
		void	   SetGPos(Vector3 vPos);		
		void	   GetGDirAndUp(Vector3% vDir, Vector3% vUp);
		void	   SetGDirAndUp(Vector3 vDir, Vector3 vUp);
		Matrix4 GetGPose();
		void	   SetGPose(Matrix4 mat44);
		float	   GetGScale();
		void	   SetGScale(float fScale);
		Quaternion GetGRot();
		void       SetGRot(Quaternion quat);

		Vector3 GetLPos();
		void	   SetLPos(Vector3 vPos);
		void       GetLDirAndUp(Vector3% vDir, Vector3% vUp);
		void	   SetLDirAndUp(Vector3 vDir, Vector3 vUp);
		Matrix4 GetLPose();
		void	   SetLPose(Matrix4 mat44);
		float	   GetLScale();
		void	   SetLScale(float fScale);
		Quaternion GetLRot();
		void       SetLRot(Quaternion quat);

		//reset pose to edit
		void ResetPose(bool bResetChild);

		// Enable/Disable my function
		void Enable(bool bEnable) { if(m_pObjBase) m_pObjBase->Enable(bEnable); }
		bool IsEnabled() 
		{ 
			if(m_pObjBase) 
				return m_pObjBase->IsEnabled();
			else
				return true;
		}
				

		//add & remove script
		void AddScript(MIScriptBehavior^ scriptbehavior);
		void RemoveScript(MIScriptBehavior^ scriptbehavior);
		void RemoveAllScript();
		void InserScript(int index, MIScriptBehavior^ scriptBehavior);
		virtual array<System::String^>^ GetScriptNames();

		//get object uniquely name
		System::String^ GetObjName();
		//set object name, return false if the name is occupied by others 
		bool SetObjName(System::String^ strName);
		MIObjBase^ GetChildByLocalName(System::String^ strLName);

		virtual bool EnterRuntime();
		virtual void LeaveRuntime();

		IObjBase* GetIObjBase() { return m_pObjBase; }
		bool HasPhysXBody();
		bool RayTraceObject(MPhysRay^ ray, [Out]MPhysRayTraceHit% hitInfo, bool getRootObj);

		//tree functions
		MIObjBase^ GetParent() { return m_parent; }
		int GetChildNum() { return m_children->Count; }
		MIObjBase^ GetChild(int i) { return m_children[i]; }

		bool AddChild(MIObjBase^ pObj);
		bool RemoveChild(MIObjBase^ pObj);
		void Remvechildren();
		int GetRelationshipCode(MIObjBase^ pObj);

		property System::Collections::Generic::List<MIScriptBehavior^>^ ScriptBehaviors
		{
			System::Collections::Generic::List<MIScriptBehavior^>^ get() { return m_scriptBehaviors; } 
		}

	internal:
		static MIObjBase^ CreateInstance(IObjBase* pObjBase, MIObjBase^ parent);
		MIObjBase(IObjBase* pObjBase);
		MIObjBase(IObjBase* pObjBase, MIObjBase^ parent);
		
		MIObjBase();

		//retrieve MIObjBase(including children) 
		MIObjBase^ GetMIObjBase(System::String^ objName);
		MIObjBase^ GetMIObjBase(const IObjBase* pObj);

		//script operation
		virtual void ScriptStart();
		virtual void ScriptEnd();
		virtual void ScriptPreTick(float dtSec);
		virtual void ScriptPostTick(float dtSec);

		//key & mouse event
		virtual void ScriptOnLButtonDown(int x, int y, unsigned int flag);
		virtual void ScriptOnLButtonUp(int x, int y, unsigned int flag);
		virtual void ScriptOnMouseMove(int x, int y, unsigned int nFlags);
		virtual void ScriptOnMouseWheel(int x, int y, int zDelta, unsigned int nFlags);

		//script defined render
		virtual void ScriptOnRender(MRender^ render);

	protected:
		IObjBase* m_pObjBase;		

		//tree info
		MIObjBase^ m_parent;
		System::Collections::Generic::List<MIObjBase^>^ m_children;
		
		//script for object
		System::Collections::Generic::List<MIScriptBehavior^>^ m_scriptBehaviors;

		void SetObjAndParent(IObjBase* pObjBase, MIObjBase^ parent);
	};


}