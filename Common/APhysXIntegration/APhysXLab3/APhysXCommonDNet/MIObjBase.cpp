#include "Stdafx.h"
#include "MIObjBase.h"
#include "PhysXObjDynamic.h"
#include "PhysXObjParticle.h"
#include "ScriptFactory.h"

using namespace System;
using namespace System::Collections::Generic;

namespace APhysXCommonDNet
{
	MIObjBase^ MIObjBase::CreateInstance(IObjBase* pObjBase, MIObjBase^ parent)
	{
		CPhysXObjParticle* pParticle = dynamic_cast<CPhysXObjParticle*>(pObjBase);
		if (pParticle != nullptr)
		{
			return gcnew MPhysXObjParticle(pParticle, parent);
		}

		CPhysXObjDynamic* pDynamic = dynamic_cast<CPhysXObjDynamic*>(pObjBase);
		if (pDynamic != nullptr)
		{
			return gcnew MPhysXObjDynamic(pDynamic, parent);
		}

		IPhysXObjBase* pPhysXObj = dynamic_cast<IPhysXObjBase*>(pObjBase);
		if (pPhysXObj != nullptr)
		{
			return gcnew MIPhysXObjBase(pPhysXObj, parent);
		}

		CRegion* pRegion = dynamic_cast<CRegion*>(pObjBase);
		if (pRegion != nullptr)
		{
			return gcnew MRegion(pRegion, parent);
		}

		return gcnew MIObjBase(pObjBase, parent);
	}
	
	MIObjBase::MIObjBase(IObjBase* pObjBase)
	{
		SetObjAndParent(pObjBase, nullptr);
	}
	MIObjBase::MIObjBase(IObjBase* pObjBase, MIObjBase^ parent)
	{
		SetObjAndParent(pObjBase, parent);
	}
	void MIObjBase::SetObjAndParent(IObjBase* pObjBase, MIObjBase^ parent)
	{
		m_pObjBase = pObjBase;
		m_parent = parent;
		m_children = gcnew List<MIObjBase^>();
		m_scriptBehaviors = gcnew List<MIScriptBehavior^>();

		if (m_pObjBase == nullptr)
			return;

		int childNum = m_pObjBase->GetChildNum();
		for (int i = 0; i < childNum; i++)
		{
			IObjBase* pObj = dynamic_cast<IObjBase*>(m_pObjBase->GetChild(i));
			if (pObj)
			{
				m_children->Add(CreateInstance(pObj, this));
			}
		}
	}
	MIObjBase::MIObjBase()
	{
		SetObjAndParent(nullptr, nullptr);
	}


	Vector3 MIObjBase::GetGPos()
	{
		return Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(m_pObjBase->GetGPos()));
	}
	void	   MIObjBase::SetGPos(Vector3 vPos)
	{
		m_pObjBase->SetGPos(Vector3::ToA3DVECTOR3(vPos));
	}
	
	void MIObjBase::GetGDirAndUp(Vector3% vDir, Vector3% vUp)
	{
		A3DVECTOR3 dir, up;
		m_pObjBase->GetGDirAndUp(dir, up);
		vDir = Vector3::FromA3DVECTOR3(dir);
		vUp = Vector3::FromA3DVECTOR3(up);
	}
	void	   MIObjBase::SetGDirAndUp(Vector3 vDir, Vector3 vUp)
	{
		m_pObjBase->SetGDirAndUp(Vector3::ToA3DVECTOR3(vDir), Vector3::ToA3DVECTOR3(vUp));
	}
	Matrix4 MIObjBase::GetGPose()
	{
		A3DMATRIX4 mat;
		m_pObjBase->GetGPose(mat);
		return Matrix4::FromA3DMATRIX(mat);
	}
	void	   MIObjBase::SetGPose(Matrix4 mat44)
	{
		m_pObjBase->SetGPose(Matrix4::ToA3DMATRIX(mat44));
	}
	float	   MIObjBase::GetGScale()
	{
		return m_pObjBase->GetGScale();
	}
	void	   MIObjBase::SetGScale(float fScale)
	{
		m_pObjBase->SetGScale(fScale);
	}

	Quaternion MIObjBase::GetGRot()
	{
		return Quaternion::FromNxQuat(m_pObjBase->GetGRot());
	}
	void       MIObjBase::SetGRot(Quaternion quat)
	{
		m_pObjBase->SetGRot(Quaternion::ToNxQuat(quat));
	}

	Vector3 MIObjBase::GetLPos()
	{
		A3DVECTOR3 vPos = APhysXConverter::N2A_Vector3(m_pObjBase->GetLPos());
		return Vector3::FromA3DVECTOR3(vPos);
	}
	void	   MIObjBase::SetLPos(Vector3 vPos)
	{
		m_pObjBase->SetLPos(Vector3::ToA3DVECTOR3(vPos));
	}
	void       MIObjBase::GetLDirAndUp(Vector3% vDir, Vector3% vUp)
	{
		A3DVECTOR3 dir, up;
		m_pObjBase->GetLDirAndUp(dir, up);
		vDir = Vector3::FromA3DVECTOR3(dir);
		vUp = Vector3::FromA3DVECTOR3(up);
	}
	
	void	   MIObjBase::SetLDirAndUp(Vector3 vDir, Vector3 vUp)
	{
		m_pObjBase->SetLDirAndUp(Vector3::ToA3DVECTOR3(vDir), Vector3::ToA3DVECTOR3(vUp));
	}
	Matrix4 MIObjBase::GetLPose()
	{
		A3DMATRIX4 mat;
		m_pObjBase->GetLPose(mat);
		return Matrix4::FromA3DMATRIX(mat);
	}
	void	   MIObjBase::SetLPose(Matrix4 mat44)
	{
		m_pObjBase->SetLPose(Matrix4::ToA3DMATRIX(mat44));
	}
	float	   MIObjBase::GetLScale()
	{
		return m_pObjBase->GetLScale();
	}
	void	   MIObjBase::SetLScale(float fScale)
	{
		m_pObjBase->SetLScale(fScale);
	}
	Quaternion MIObjBase::GetLRot()
	{
		return Quaternion::FromNxQuat(m_pObjBase->GetLRot());
	}
	void       MIObjBase::SetLRot(Quaternion quat)
	{
		m_pObjBase->SetLRot(Quaternion::ToNxQuat(quat));
	}

	void MIObjBase::ResetPose(bool bResetChild)
	{
		A3DMATRIX4 mat = m_pObjBase->GetProperties()->GetPose(false);
		float fScale = m_pObjBase->GetProperties()->GetScale(false);

		m_pObjBase->SetGPose(mat);
		m_pObjBase->SetGScale(fScale);

		for ( int i = 0; i < m_children->Count; i++)
		{
			m_children[i]->ResetPose(bResetChild);
		}
	}

	//add & remove script
	void MIObjBase::AddScript(MIScriptBehavior^ scriptbehavior)
	{
		m_scriptBehaviors->Add(scriptbehavior);
	}
	void MIObjBase::RemoveScript(MIScriptBehavior^ scriptbehavior)
	{
		m_scriptBehaviors->Remove(scriptbehavior);
	}
	void MIObjBase::RemoveAllScript()
	{
		m_scriptBehaviors->Clear();
	}
	void MIObjBase::InserScript(int index, MIScriptBehavior^ scriptBehavior)
	{
		m_scriptBehaviors->Insert(index, scriptBehavior);
	}
	array<String^>^ MIObjBase::GetScriptNames()
	{
		if (m_pObjBase == nullptr)
			return nullptr;

		array<String^>^ scriptNames = gcnew array<String^>(m_pObjBase->GetProperties()->GetScriptCount());
		for (int i = 0; i < m_pObjBase->GetProperties()->GetScriptCount(); i++)
		{
			AString aName = m_pObjBase->GetProperties()->GetScriptName(i);
			String^ strname = gcnew String(aName.operator const char*());
			scriptNames[i] = strname;
		}
		return scriptNames;
	}

	//get object uniquely name
	System::String^ MIObjBase::GetObjName()
	{
		const char* str = m_pObjBase->GetProperties()->GetObjName_cstr();
		return gcnew System::String(str);
	}
	//set object name, return false if the name is occupied by others 
	bool MIObjBase::SetObjName(System::String^ strName)
	{
		if (GetObjName() == strName)
			return true;

		if (MScene::Instance->GetMIObjBase(strName) != nullptr )
			return false;

		pin_ptr<const wchar_t> wch = PtrToStringChars(strName);
		AString strObjName = WC2AS(wch);

		//to update: to check is occupied by others
		//m_pObjBase->GetProperties()->SetObjName(strObjName);
		return true;
	}
	MIObjBase^ MIObjBase::GetChildByLocalName(System::String^ strLName)
	{
		pin_ptr<const wchar_t> wchl = PtrToStringChars(strLName);
		TString strALName = _TWC2TSting(wchl);
		IObjBase* pChild = m_pObjBase->GetChildByLocalName(strALName);
		return GetMIObjBase(pChild);
	}

	bool MIObjBase::EnterRuntime()
	{
		return m_pObjBase->EnterRuntime(MScene::Instance->GetAPhysXScene());
	}
	void MIObjBase::LeaveRuntime()
	{
		m_pObjBase->LeaveRuntime();
	}
	bool MIObjBase::HasPhysXBody()
	{
		return m_pObjBase->GetProperties()->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	}
	bool MIObjBase::RayTraceObject(MPhysRay^ ray, [Out]MPhysRayTraceHit% hitInfo, bool getRootObj)
	{
		hitInfo.obj = nullptr;
		hitInfo.t = 1;
		hitInfo.vHit.Clear();
		hitInfo.vNormal.Clear();

		PhysRay theRay;
		theRay.vStart = Vector3::ToA3DVECTOR3(ray->vStart);
		theRay.vDelta = Vector3::ToA3DVECTOR3(ray->vDelta);
		PhysRayTraceHit theHitInfo;
		bool bRtn = m_pObjBase->RayTraceObj(theRay, theHitInfo, getRootObj, ObjManager::OBJ_TYPEID_ALL);
		if (bRtn)
		{
			hitInfo.obj = MScene::Instance->GetMIObjBase((IObjBase*)theHitInfo.UserData);
			hitInfo.t = theHitInfo.t;
			hitInfo.vHit = Vector3::FromA3DVECTOR3(theHitInfo.vHit);
			hitInfo.vNormal = Vector3::FromA3DVECTOR3(theHitInfo.vNormal);
		}
		return bRtn;
	}

	//script operation
	void MIObjBase::ScriptStart()
	{
		//generate script object
		int i;
		
		String^ defscript = MScene::Instance->DefScriptFile;
		MIScriptBehavior^ behavior;
		if (defscript != "")
		{
			Object^ obj = ScriptFactory::Instance->CreateInstanceFromFile(defscript);
			behavior = dynamic_cast<MIScriptBehavior^>(obj);
			if (behavior != nullptr)
			{
				m_scriptBehaviors->Insert(0, behavior);
			}
		}
		array<String^>^ scriptnames = GetScriptNames();
		for (i = 0; i < scriptnames->Length; i++)
		{
			Object^ obj = ScriptFactory::Instance->CreateInstanceFromFile(scriptnames[i]);
			behavior = dynamic_cast<MIScriptBehavior^>(obj);
			if (behavior != nullptr)
			{
				m_scriptBehaviors->Add(behavior);
			}
		}
		
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			m_scriptBehaviors[i]->ObjBase = this;
			MKeyEventMgr::Instance->Register(m_scriptBehaviors[i]);
			TRYCALL(m_scriptBehaviors[i]->Start())
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptStart())
		}
	}
	void MIObjBase::ScriptEnd()
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			MKeyEventMgr::Instance->UnRegister(m_scriptBehaviors[i]);
			TRYCALL(m_scriptBehaviors[i]->End())
		}
		m_scriptBehaviors->Clear();

		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptEnd())
		}
	}
	void MIObjBase::ScriptPreTick(float dtSec)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			if (m_scriptBehaviors[i]->IsPause)
				continue;

			TRYCALL(m_scriptBehaviors[i]->PreTick(dtSec))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptPreTick(dtSec))
		}
	}
	void MIObjBase::ScriptPostTick(float dtSec)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			if (m_scriptBehaviors[i]->IsPause)
				continue;

			TRYCALL(m_scriptBehaviors[i]->PostTick(dtSec))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptPostTick(dtSec))
		}
	}

	//key & mouse event
	void MIObjBase::ScriptOnLButtonDown(int x, int y, unsigned int flag)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			TRYCALL(m_scriptBehaviors[i]->OnLButtonDown(x, y, flag))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptOnLButtonDown(x, y, flag))
		}
	}
	void MIObjBase::ScriptOnLButtonUp(int x, int y, unsigned int flag)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			TRYCALL(m_scriptBehaviors[i]->OnLButtonUp(x, y, flag))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptOnLButtonUp(x, y, flag))
		}
	}
	void MIObjBase::ScriptOnMouseMove(int x, int y, unsigned int nFlags)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			TRYCALL(m_scriptBehaviors[i]->OnMouseMove(x, y, nFlags))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptOnMouseMove(x, y, nFlags))
		}
	}
	void MIObjBase::ScriptOnMouseWheel(int x, int y, int zDelta, unsigned int nFlags)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			TRYCALL(m_scriptBehaviors[i]->OnMouseWheel(x, y, zDelta, nFlags))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptOnMouseWheel(x, y, zDelta, nFlags))
		}
	}

	//script defined render
	void MIObjBase::ScriptOnRender(MRender^ render)
	{
		int i;
		for (i = 0; i < m_scriptBehaviors->Count; i++)
		{
			TRYCALL(m_scriptBehaviors[i]->OnRender(render))
		}
		//children script
		for (i = 0; i < m_children->Count; i++)
		{
			TRYCALL(m_children[i]->ScriptOnRender(render))
		}
	}

	bool MIObjBase::AddChild(MIObjBase^ pObj)
	{
		m_pObjBase->AddChild(pObj->GetIObjBase());
		m_children->Add(pObj);
		pObj->m_parent = this;

		return true;
	}
	bool MIObjBase::RemoveChild(MIObjBase^ pObj)
	{
		m_pObjBase->RemoveChild(pObj->GetIObjBase());
		pObj->m_parent = nullptr;
		m_children->Remove(pObj);

		return true;
	}
	void MIObjBase::Remvechildren()
	{
		for (int i = 0; i < m_children->Count; i++)
		{
			RemoveChild(m_children[i]);
		}
	}
	int MIObjBase::GetRelationshipCode(MIObjBase^ pObj)
	{
		if (nullptr == pObj)
			return 0;
		IObjBase* pObject = pObj->GetIObjBase();
		if (0 == pObject)
			return 0;
		return m_pObjBase->GetRelationshipCode(*pObject);
	}

	MIObjBase^ MIObjBase::GetMIObjBase(System::String^ objName)
	{
		if (GetObjName() == objName)
			return this;

		for (int i = 0; i < m_children->Count; i++)
		{
			MIObjBase^ obj = m_children[i]->GetMIObjBase(objName);
			if (obj != nullptr)
				return obj;
		}

		return nullptr;
	}
	MIObjBase^ MIObjBase::GetMIObjBase(const IObjBase* pObj)
	{
		if (0 == pObj)
			return nullptr;
		if (m_pObjBase == pObj)
			return this;

		for (int i = 0; i < m_children->Count; i++)
		{
			MIObjBase^ obj = m_children[i]->GetMIObjBase(pObj);
			if (obj != nullptr)
				return obj;
		}
		return nullptr;
	}
}