/*
 * FILE: RawObjManager.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

RawObjMgr::RawOMD* RawObjMgr::RawOMD::Clone() const
{
	return new RawOMD(*this);
}

const TCHAR* RawObjMgr::GetObjTypeIDText(const ObjTypeID id)
{
	switch (id)
	{
	case OBJ_TYPEID_MANAGER:
		return _T("OBJ_MANAGER");
	case OBJ_TYPEID_STATIC:
		return _T("OBJ_STATIC");
	case OBJ_TYPEID_SKINMODEL:
		return _T("OBJ_SKINMODEL");
	case OBJ_TYPEID_ECMODEL:
		return _T("OBJ_ECMODEL");
	case OBJ_TYPEID_PARTICLE:
		return _T("OBJ_PARTICLE");
	case OBJ_TYPEID_FORCEFIELD:
		return _T("OBJ_FORCEFIELD");
	case OBJ_TYPEID_REGION:
		return _T("OBJ_REGION");
	}

	assert(!"Unknown ObjTypeID!");
	return IPropObjBase::szEmpty;
}

bool RawObjMgr::GetObjTypeID(const TCHAR* pStr, ObjTypeID& outTypeID)
{
	if (0 == a_strcmp(pStr, IPropObjBase::szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_MANAGER)))
	{
		outTypeID = OBJ_TYPEID_MANAGER;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_STATIC)))
	{
		outTypeID = OBJ_TYPEID_STATIC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_SKINMODEL)))
	{
		outTypeID = OBJ_TYPEID_SKINMODEL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_ECMODEL)))
	{
		outTypeID = OBJ_TYPEID_ECMODEL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_PARTICLE)))
	{
		outTypeID = OBJ_TYPEID_PARTICLE;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_FORCEFIELD)))
	{
		outTypeID = OBJ_TYPEID_FORCEFIELD;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_REGION)))
	{
		outTypeID = OBJ_TYPEID_REGION;
		return true;
	}
	assert(!"Unknown ObjTypeIDText!");
	return false;
}

bool RawObjMgr::IsPhysXObjType(const ObjTypeID& id)
{
	if (OBJ_TYPEID_MANAGER >= id)
		return false;
	if (OBJ_TYPEID_FORCEFIELD < id)
		return false;
	return true;
}

const RawObjMgr::ObjTypeInfo& RawObjMgr::GetObjTypeInfo(const ObjTypeID id)
{
	static bool bInited = false;
	static ObjTypeInfo theInfo[OBJ_MAX_BOUND];

	assert(id < OBJ_MAX_BOUND);
	if (bInited)
	{	
		if (IsPhysXObjType(id))
			return theInfo[id];
		return theInfo[0];
	}

	bInited = true;
	theInfo[OBJ_TYPEID_MANAGER].strWCExt   = IPropObjBase::szEmpty;
	theInfo[OBJ_TYPEID_MANAGER].strFilter  = IPropObjBase::szEmpty;
	theInfo[OBJ_TYPEID_STATIC].strWCExt    = _T("*.umd");
	theInfo[OBJ_TYPEID_STATIC].strFilter   = _T("UMD 文件(*.umd)|*.umd||");
	theInfo[OBJ_TYPEID_SKINMODEL].strWCExt = _T("*.smd");
	theInfo[OBJ_TYPEID_SKINMODEL].strFilter= _T("SMD 文件(*.smd)|*.smd||");
	theInfo[OBJ_TYPEID_ECMODEL].strWCExt   = _T("*.ecm");
	theInfo[OBJ_TYPEID_ECMODEL].strFilter  = _T("ECM3 文件(*.ecm3)|*.ecm3||");
	theInfo[OBJ_TYPEID_PARTICLE].strWCExt  = _T("*.gfx3");
	theInfo[OBJ_TYPEID_PARTICLE].strFilter = _T("GFX3 文件(*.gfx3)|*.gfx3||");
	theInfo[OBJ_TYPEID_FORCEFIELD].strWCExt  = _T("*.xff");
	theInfo[OBJ_TYPEID_FORCEFIELD].strFilter = _T("XFF 文件(*.xff)|*.xff||");
	return GetObjTypeInfo(id);
}

RawObjMgr* RawObjMgr::GetInstance()
{
	static RawObjMgr rawObjMgr;
	return  &rawObjMgr;
}

RawObjMgr::RawObjMgr()
{
	ClearUniqueSequenceID();
	SetArchiveStyle(true);
}

void RawObjMgr::ClearUniqueSequenceID()
{
	m_SequenceID = 0;
}

int RawObjMgr::GetUniqueSequenceID()
{
	int sid = m_SequenceID;
	++m_SequenceID;
	return sid;
}

bool RawObjMgr::SaveSerialize(NxStream& stream) const
{
	SaveVersion(&stream);
	stream.storeDword(m_SequenceID);
	return true;
}

bool RawObjMgr::LoadDeserialize(NxStream& stream, bool& outIsLowVersion)
{
	if (m_ArchiveOldStyle)
	{
		m_SequenceID = stream.readDword();
		return true;
	}

	LoadVersion(&stream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVersion = true;

	if(m_StreamObjVer >= 0xAA000001)
		m_SequenceID = stream.readDword();

	return true;
}

IObjBase* RawObjMgr::CreateRawObject(const ObjTypeID objType, const IObjMgrData& omd)
{
	IObjBase* pObj = 0;
	switch(objType)
	{
	case OBJ_TYPEID_STATIC:
		pObj = new CPhysXObjStatic(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_SKINMODEL:
		pObj = new CPhysXObjSkinModel(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_ECMODEL:
		pObj = new CPhysXObjECModel(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_PARTICLE:
		pObj = new CPhysXObjParticle(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_FORCEFIELD:
//		pObj = new CPhysXObjForceField(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_REGION:
		pObj = new Region(GetUniqueSequenceID());
		break;
	default:
		assert(!"Oops! Unknown PhysXObjType!");
		break;
	}

	if (0 == pObj)
		return 0;

	if (objType != pObj->GetProperties()->GetObjType())
	{
		delete pObj;
		return 0;
	}

	IObjMgrData* pMgrData = omd.Clone();
	if (0 == pMgrData)
	{
		delete pObj;
		return 0;
	}

	int idx = m_aObjects.GetSize();
	pMgrData->SetRawMgrIdx(idx);
	pObj->SetMgrData(pMgrData);
	m_aObjects.Add(pObj);
	return pObj;
}

void RawObjMgr::ReleaseRawObject(IObjBase* pObject)
{
	if (0 == pObject)
		return;

	IObjMgrData* pMgrData = pObject->GetMgrData();
	assert(0 != pMgrData);

	const int idx = pMgrData->GetRawMgrIdx();
	assert(0 <= idx);
	assert(idx < m_aObjects.GetSize());

	delete pObject;
	delete pMgrData;
	m_aObjects.RemoveAtQuickly(idx);

	if (idx < m_aObjects.GetSize())
	{
		IObjMgrData* pSwapped = m_aObjects[idx]->GetMgrData();
		assert(0 != pSwapped);
		pSwapped->SetRawMgrIdx(idx);
	}
}
