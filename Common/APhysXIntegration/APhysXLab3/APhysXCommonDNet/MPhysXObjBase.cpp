#include "Stdafx.h"
#include "MPhysXObjBase.h"

#include "PhysXObjBase.h"

#include "MPhysXObjDynamic.h"
#include "MA3DVector3.h"
#include "MA3DMatrix4.h"
#include "MA3DQuaternion.h"
#include "MScriptBehavior.h"
#include "MAABB.h"
#include "MScene.h"

using namespace System;

namespace APhysXCommonDNet
{

	MIPhysXObjBase::MIPhysXObjBase()
	{
		m_pPhysXBase = nullptr;
	}

	MIPhysXObjBase::MIPhysXObjBase(IPhysXObjBase* pPhysXBase) : MIObjBase(pPhysXBase)
	{
		m_pPhysXBase = pPhysXBase;
	}
	MIPhysXObjBase::MIPhysXObjBase(IPhysXObjBase* pPhysXBase, MIObjBase^ parent) : MIObjBase(pPhysXBase, parent)
	{
		m_pPhysXBase = pPhysXBase;
	}
	
	//get file path of this object
	System::String^ MIPhysXObjBase::GetObjFilePath()
	{
		const char* str = m_pPhysXBase->GetProperties()->GetFilePathName_cstr();
		return gcnew System::String(str);
	}
	void MIPhysXObjBase::SetDrivenModeEnforce(MDrivenMode dm)
	{
		IPropPhysXObjBase* pProp = m_pPhysXBase->GetProperties();
		IPropPhysXObjBase::DrivenMode drivenmode = static_cast<IPropPhysXObjBase::DrivenMode>(dm);
		bool bIsLock = pProp->GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED);
		if (bIsLock)
			pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);
		pProp->SetDrivenMode(drivenmode);
		if (bIsLock)
			pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);
	}
	void MIPhysXObjBase::SetDrivenMode(MDrivenMode dm)
	{
		IPropPhysXObjBase::DrivenMode drivenmode = static_cast<IPropPhysXObjBase::DrivenMode>(dm);
		m_pPhysXBase->GetProperties()->SetDrivenMode(drivenmode);
	}
	MDrivenMode MIPhysXObjBase::GetDrivenMode()
	{
		return static_cast<MDrivenMode>(m_pPhysXBase->GetProperties()->GetDrivenMode());
	}
	bool MIPhysXObjBase::QueryDrivenMode(MDrivenMode dm)
	{
		IPropPhysXObjBase::DrivenMode drivenmode = static_cast<IPropPhysXObjBase::DrivenMode>(dm);
		return m_pPhysXBase->GetProperties()->QueryDrivenMode(drivenmode);
	}
	System::String^ MIPhysXObjBase::GetDrivenModeTextChinese()
	{
		const TCHAR* str = IPropPhysXObjBase::GetDrivenModeTextChinese(m_pPhysXBase->GetProperties()->GetDrivenMode());
		AString astr = _TWC2AS(str);
		return gcnew String(astr);
	}
	System::String^ MIPhysXObjBase::GetDrivenModeText()
	{
		const TCHAR* str = IPropPhysXObjBase::GetDrivenModeText(m_pPhysXBase->GetProperties()->GetDrivenMode());
		AString astr = _TWC2AS(str);
		return gcnew String(astr);
	}	
	bool MIPhysXObjBase::GetAABB(AABB% outAABB)
	{
		if (m_pPhysXBase == nullptr)
			return false;

		A3DAABB aabb;
		const NxBounds3& nxAABB = m_pPhysXBase->GetAABB();
		APhysXConverter::N2A_AABB(nxAABB, aabb);
		outAABB.Mins = Vector3::FromA3DVECTOR3(aabb.Mins);
		outAABB.Maxs = Vector3::FromA3DVECTOR3(aabb.Maxs);
		outAABB.CompleteCenterExts();
		return true;
	}
	MIAssailable^ MIPhysXObjBase::GetMIAssailable()
	{
		return MScene::Instance->GetMIAssailable(m_pPhysXBase);
	}
}