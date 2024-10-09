#include "Stdafx.h"
#include "MPushable.h"
#include "Pushable.h"

#include "MScene.h"

using namespace System;

namespace APhysXCommonDNet
{
	MPushable::MPushable()
	{
		m_pPushable = new Pushable;
	}
	MPushable::~MPushable()
	{
		Release();
	}
	MPushable::!MPushable()
	{
		Release();
	}

	void MPushable::Init(MIPhysXObjBase^ pObj)
	{
		IPhysXObjBase* pPhysXObj = NULL;
		if (pObj != nullptr)
			pPhysXObj = pObj->GetIPhysXObjBase();
		m_pPushable->Init(pPhysXObj, MScene::Instance->GetScene());
		m_pPhysXObj = pObj;
	}
	void MPushable::Release()
	{
		if (m_pPushable)
			m_pPushable->Release();

		delete m_pPushable;
		m_pPushable = NULL;
	}

}