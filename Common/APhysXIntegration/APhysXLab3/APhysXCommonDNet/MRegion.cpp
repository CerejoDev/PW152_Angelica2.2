#include "Stdafx.h"
#include "MRegion.h"
#include "MRegionScript.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace APhysXCommonDNet
{
	float MRegion::GetDefRadius()
	{
		return CRegion::GetDefRadius();
	}
	bool MRegion::SetDefRadius(float newR)
	{
		return CRegion::SetDefRadius(newR);
	}

	System::String^ MRegion::GetShapeTypeText(MShapeType st)
	{
		const TCHAR* str = CRegion::GetShapeTypeText(static_cast<CRegion::ShapeType>(st));
		return gcnew String(str);
	}
	bool MRegion::GetShapeType(System::String^ strType, MShapeType% outRT)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(strType);
		TString str = _TWC2TSting(wch);
		CRegion::ShapeType type;
		if (CRegion::GetShapeType(str, type))
		{
			outRT = static_cast<MShapeType>(type);
			return true;
		}
		return false;
	}

	MShapeType MRegion::GetShapeType()
	{
		return  (MShapeType)m_pRegion->GetShapeType();
	}
	void MRegion::SetShapeType(MShapeType st)
	{
		m_pRegion->SetShapeType(static_cast<CRegion::ShapeType>(st));
	}

	Vector3 MRegion::GetVolume(bool withScaled)
	{
		return Vector3::FromA3DVECTOR3(m_pRegion->GetVolume(withScaled));
	}
	bool MRegion::SetVolume(Vector3 newVolume, bool withScaled)
	{
		return m_pRegion->SetVolume(Vector3::ToA3DVECTOR3(newVolume), withScaled);
	}

	MRegion::MRegion(CRegion* pRegion) : MIObjBase(pRegion)
	{
		m_pRegion = pRegion;
	}

	MRegion::MRegion(CRegion* pRegion, MIObjBase^ pParent) : MIObjBase(pRegion, pParent)
	{
		m_pRegion = pRegion;
	}

	void MRegion::EnterRegion(unsigned int nbEntities, IObjBase** entities)
	{
		array<MIObjBase^>^ objs = gcnew array<MIObjBase^>(nbEntities);
		MScene^ scene = MScene::Instance;

		for (unsigned int i = 0; i < nbEntities; i++)
		{
			objs[i] = scene->GetMIObjBase(entities[i]);
		}

		//run region script
		for (int i = 0; i < m_scriptBehaviors->Count; i++)
		{
			MRegionScript^ script = dynamic_cast<MRegionScript^>(m_scriptBehaviors[i]);
			if (script != nullptr)
			{
				TRYCALL(script->EnterRegion(objs))
			}
		}
	}
	void MRegion::LeaveRegion(unsigned int nbEntities, IObjBase** entities)
	{
		array<MIObjBase^>^ objs = gcnew array<MIObjBase^>(nbEntities);
		MScene^ scene = MScene::Instance;

		for (unsigned int i = 0; i < nbEntities; i++)
		{
			objs[i] = scene->GetMIObjBase(entities[i]);
		}

		//run region script
		for (int i = 0; i < m_scriptBehaviors->Count; i++)
		{
			MRegionScript^ script = dynamic_cast<MRegionScript^>(m_scriptBehaviors[i]);
			if (script != nullptr)
			{
				TRYCALL(script->LeaveRegion(objs))
			}
		}
	}

}