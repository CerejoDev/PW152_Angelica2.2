/*
* FILE: PhysXObjDynamic.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/05/19
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"

CPhysXObjDynamic::CPhysXObjDynamic()
{
/*	m_pAC = 0;
	OnSetToEmpty();*/
}

CPhysXObjDynamic::~CPhysXObjDynamic()
{
//	delete m_pAC;
}
/*
void CPhysXObjDynamic::OnSetToEmpty()
{
	m_PlayEnable = false;
	m_IsRestart  = true;

	if (0 != m_pAC)
		delete m_pAC;
	m_pAC = 0;
	
	APhysXCCCapsuleDesc ccDesc;
	m_FlagsCC = ccDesc.flags;
	m_FlagsPM = 0;
	m_Channel = CNL_UNKNOWN;
	m_FlagsCCRuntime = 0;
	m_FlagsPMRuntime = 0;
	m_ChannelRuntime = CNL_UNKNOWN;
	pCCLiveIn = 0;
	FlagOfCCType = OBPI_HAS_APHYSX_CC;
	m_Height = 0.0f;
}

void CPhysXObjDynamic::OnCopyBasicProperty(const IPhysXObjBase& obj)
{
	if (GetObjType() != obj.GetObjType())
		return;

	const CPhysXObjDynamic* pDynObj = static_cast<const CPhysXObjDynamic*>(&obj);

	m_PlayEnable = pDynObj->m_PlayEnable;
	m_IsRestart  = pDynObj->m_IsRestart;

	if (0 != pDynObj->m_pAC)
		SetActionPlayer(*(pDynObj->m_pAC));
	else
	{
		delete m_pAC;
		m_pAC = 0;
	}

	m_FlagsCC = pDynObj->m_FlagsCC;
	m_FlagsPM = pDynObj->m_FlagsPM;
	m_Channel = pDynObj->m_Channel;

	m_FlagsCCRuntime = pDynObj->m_FlagsCCRuntime;
	m_FlagsPMRuntime = pDynObj->m_FlagsPMRuntime;
	m_ChannelRuntime = pDynObj->m_ChannelRuntime;
}

bool CPhysXObjDynamic::OnLoadDeserialize(NxStream& stream, const unsigned int nVersion)
{
	if (nVersion < 0xCC000006)
		return false;

	stream.readBuffer(&m_FlagsCC, sizeof(int));
	stream.readBuffer(&m_FlagsPM, sizeof(int));
	m_FlagsCCRuntime = 0;
	m_FlagsPMRuntime = 0;
	
	m_Channel = CNL_COMMON;
	m_FlagsPMRuntime = CNL_UNKNOWN;
	if (nVersion >= 0xCC00000F)
		stream.readBuffer(&m_Channel, sizeof(CollisionChannel));
	return true;
}

void CPhysXObjDynamic::OnSaveSerialize(NxStream& stream) const
{
	stream.storeBuffer(&m_FlagsCC, sizeof(int));
	stream.storeBuffer(&m_FlagsPM, sizeof(int));
	stream.storeBuffer(&m_Channel, sizeof(CollisionChannel));
}
*/
bool CPhysXObjDynamic::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	// compatible old version archive
/*	if (ReadPeopleInfo(OBPI_HAS_APHYSX_CC))
	{
		if ((CNL_INDEPENDENT != m_Channel) && (CNL_INDEPENDENT_CCHOST != m_Channel))
			m_Channel = CNL_INDEPENDENT_CCHOST;
	}*/

	if (IPhysXObjBase::OnEnterRuntime(aPhysXScene))
	{
		GetProperties()->EnterRuntimeNotify(aPhysXScene);
		return true;
	}
	return false;
}

void CPhysXObjDynamic::OnLeaveRuntime()
{
	GetProperties()->LeaveRuntimeNotify();
	IPhysXObjBase::OnLeaveRuntime();
}

bool CPhysXObjDynamic::OnTickMove(float deltaTimeSec)
{
	CDynProperty* pProps = GetProperties();
	if (pProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (!pProps->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		{
			pProps->UpdateMove(deltaTimeSec);
			pProps->DoMoveCC(deltaTimeSec);
		}
	}
	return true;
}

bool CPhysXObjDynamic::OnTickAnimation(const unsigned long dwDeltaTime)
{
	CDynProperty* pProps = GetProperties();
	// Note: main actor must be runtime only! Or it can't be reset pose to original edited position.
	if (pProps->QueryRoleType(CDynProperty::ROLE_ACTOR_AVATAR))
	{
		assert(true == pProps->GetFlags().ReadFlag(OBF_RUNTIME_ONLY));
	}

	return pProps->PlayAction(dwDeltaTime);
}

void CPhysXObjDynamic::OnChangeDrivenMode(const DrivenMode& dmNew)
{
	const CDynProperty* pProps = GetProperties();
	if (!pProps->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		return;

	if (IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX != dmNew)
	{
		ResetPose();

	//	if (IObjPropertyBase::DRIVEN_BY_PURE_PHYSX != GetProperties()->GetDrivenMode())
	//		return;

	/*	if (0 != pCCLiveIn)
		{
			RaisePeopleInfo(FlagOfCCType);
			ApplyPeopleInfo(*pCCLiveIn);
		}*/
	}
	else
	{
	/*	pCCLiveIn = 0;
		if ((0 != m_pAC) && ReadFlag(OBFI_ON_HIT_SKILL))
		{
			if (ReadPeopleInfo(OBPI_HAS_APHYSX_CC) || ReadPeopleInfo(OBPI_HAS_APHYSX_LWCC) || ReadPeopleInfo(OBPI_HAS_APHYSX_DYNCC))
			{
				ClearPeopleInfo(OBPI_HAS_APHYSX_CC);
				ClearPeopleInfo(OBPI_HAS_APHYSX_LWCC);
				ClearPeopleInfo(OBPI_HAS_APHYSX_DYNCC);
				APhysXCCBase* pCC = m_pAC->GetAPhysXCCBase();
				if (0 != pCC)
					pCCLiveIn = &(pCC->GetAPhysXScene());
				FlagOfCCType = OBPI_HAS_APHYSX_CC;
				APhysXLWCharCtrler* pLWCC = m_pAC->GetAPhysxLWCC();
				if (0 != pLWCC)
					FlagOfCCType = OBPI_HAS_APHYSX_LWCC;
				APhysXDynCharCtrler* pDynCC = m_pAC->GetAPhysxDynCC();
				if (0 != pDynCC)
					FlagOfCCType = OBPI_HAS_APHYSX_DYNCC;
				ApplyPeopleInfo(*pCCLiveIn);
			}
		}*/
	}
}

bool CPhysXObjDynamic::ApplyPhysXBody(const bool bEnable, APhysXScene* pScene)
{
	CDynProperty* pProp = GetProperties();
	bool lastVal = pProp->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	if (bEnable == lastVal)
		return true;

	if (bEnable)
	{
		pProp->GetFlags().RaiseFlag(OBF_ENABLE_PHYSX_BODY);
		if (pProp->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			assert(0 != pScene);
			if (0 == pScene)
				return false;
			if (!InstancePhysXObj(*pScene))
				return false;

		//	ApplyChannel(m_ChannelRuntime);

			// to deal with the case: CC is existing with No-RBbody then create RB Body. to set channel correct!!
			APhysXCollisionChannel channel;
			if (GetCollisionChannel(channel))
				pProp->SetHostModelChannel(channel);
		}
	}
	else
	{
		pProp->GetFlags().ClearFlag(OBF_ENABLE_PHYSX_BODY);
		if (pProp->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			ReleasePhysXObj();
		pProp->SetHostModelChannel(APX_COLLISION_CHANNEL_CC_COMMON_HOST);
	}
	return true;
}

A3DVECTOR3 CPhysXObjDynamic::GetHeadTopPos(A3DMATRIX4* pOutMat) const
{
	A3DSkinModel* pModel = GetSkinModel();
	A3DMATRIX4 mat = pModel->GetAbsoluteTM();
	if (0 != pOutMat)
		*pOutMat = mat;

	const CDynProperty* pProp = GetProperties();
	float height = pProp->GetOriginalLoacalHeight();
	height *= pProp->GetScale();
	height *= 1.05f;
	A3DVECTOR3 up(0, height, 0); 
	A3DVECTOR3 newUp = up * mat; 
	return newUp;
}

bool CPhysXObjDynamic::GetCollisionChannel(APhysXCollisionChannel& outChannel) const
{
	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if (0 == pSRB)
		return false;

	outChannel = pSRB->GetCollisionChannel();
	return true;
}

float CPhysXObjDynamic::GetYOffsetForRevive() const
{
	float yOffset = 0;
	const CDynProperty* pProp = GetProperties();
	if (pProp->QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
		return yOffset;
		
	if (pProp->m_Status.ReadFlag(OBFI_IS_IN_RUNTIME) && pProp->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
	{
		A3DAABB aabb;
		if (!GetAABB(aabb))
		{
			if (!GetModelAABB(aabb))
				aabb.Mins.y = aabb.Maxs.y = 0;
		}
		yOffset = (aabb.Maxs.y - aabb.Mins.y) * 0.5f;
	}
	return yOffset;
}

/*
void CPhysXObjDynamic::OnInstancePhysXObjPost(APhysXScene& aPhysXScene)
{
	bool HasPMFlags = ReadFlag(OBFI_INITED_PMFLAGS);
	if (HasPMFlags)
	{
		if (ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			ApplyChannel(m_ChannelRuntime);
			ApplyPMFlags(m_FlagsPMRuntime);
			ApplyCCFlags(m_FlagsCCRuntime);
		}
		else
		{
			ApplyChannel(m_Channel);
			ApplyPMFlags(m_FlagsPM);
			ApplyCCFlags(m_FlagsCC);
			m_IsRestart = true;
		}	
	}
	else
	{
		m_FlagsPM = GetPMFlags(true);
		m_FlagsPMRuntime = m_FlagsPM;
		RaiseFlag(OBFI_INITED_PMFLAGS);
		m_Channel = GetChannel(true);
		m_ChannelRuntime = m_Channel;
	}

	if (ReadFlag(OBFI_TO_KINEMATIC))
	{
		UpdateNxActorsAttributes(0, true, 0);
		RaiseFlag(OBF_DRIVENMODE_ISLOCKED);
	}

	A3DAABB aabb;
	if (!GetAABB(aabb))
	{
		if (!GetModelAABB(aabb))
		{
			m_Height = 0.0f;
			return;
		}
	}
	m_Height = aabb.Extents.y * 2;
}

int CPhysXObjDynamic::GetCCFlags() const
{
	if (ReadFlag(OBFI_IS_IN_RUNTIME))
		return m_FlagsCCRuntime;
	else
		return m_FlagsCC;
}

void CPhysXObjDynamic::SetCCFlags(const int flags)
{
	if (ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (flags == m_FlagsCCRuntime)
			return;
	}
	else
	{
		if (flags == m_FlagsCC)
			return;

		m_FlagsCC = flags;
	}

	ApplyCCFlags(flags);
}

void CPhysXObjDynamic::ApplyCCFlags(const int flags)
{
	m_FlagsCCRuntime = flags;
	if (0 == m_pAC)
		return;

	APhysXCharacterController* pCC = m_pAC->GetAPhysxCC();
	if (0 == pCC)
		return;

	if (APhysXCharacterController::APX_CCF_SLIDE_ENABLE & flags)
		pCC->RaiseFlag(APhysXCharacterController::APX_CCF_SLIDE_ENABLE);
	else
		pCC->ClearFlag(APhysXCharacterController::APX_CCF_SLIDE_ENABLE);
	if (APhysXCharacterController::APX_CCF_ADD_PUSHFORCE & flags)
		pCC->RaiseFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE);
	else
		pCC->ClearFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE);
	if (APhysXCharacterController::APX_CCF_ADD_STANDFORCE & flags)
		pCC->RaiseFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE);
	else
		pCC->ClearFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE);
	if (APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN & flags)
		pCC->RaiseFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN);
	else
		pCC->ClearFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN);
	if (APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE & flags)
		pCC->RaiseFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE);
	else
		pCC->ClearFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE);
}

void CPhysXObjDynamic::ApplyPMFlags(const int flags)
{
	m_FlagsPMRuntime = flags;
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return;

	pPSRB->SetObjectFlag(APX_OBJFLAG_CC_ADD_PUSHFORCE,	(flags & APX_OBJFLAG_CC_ADD_PUSHFORCE)? true:false);
	pPSRB->SetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE, (flags & APX_OBJFLAG_CC_ADD_STANDFORCE)? true:false);
	pPSRB->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH,		(flags & APX_OBJFLAG_CC_CAN_ATTACH)? true:false);
}

int CPhysXObjDynamic::GetPMFlags(const bool forceRefresh) const
{
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return m_FlagsPM;

	if (forceRefresh)
	{
		int flags = 0;
		if (pPSRB->GetObjectFlag(APX_OBJFLAG_CC_ADD_PUSHFORCE))
			flags |= APX_OBJFLAG_CC_ADD_PUSHFORCE;
		if (pPSRB->GetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE))
			flags |= APX_OBJFLAG_CC_ADD_STANDFORCE;
		if (pPSRB->GetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH))
			flags |= APX_OBJFLAG_CC_CAN_ATTACH;
		return flags;
	}
	
	return m_FlagsPMRuntime;
}

void CPhysXObjDynamic::SetPMFlags(const int flags)
{
	if (ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (flags == m_FlagsPMRuntime)
			return;
	}
	else
	{
		if (flags == m_FlagsPM)
			return;
		
		m_FlagsPM = flags;
	}

	ApplyPMFlags(flags);
}

CPhysXObjDynamic::CollisionChannel CPhysXObjDynamic::GetChannel(const bool forceRefresh) const
{
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return m_Channel;
		
	if (forceRefresh)
	{
		CollisionChannel cnl = CNL_UNKNOWN;
		APhysXCollisionChannel acc = pPSRB->GetCollisionChannel();
		switch (acc)
		{
		case APX_COLLISION_CHANNEL_INVALID:
		case APX_COLLISION_CHANNEL_COLLIDE_ALL:
		case APX_COLLISION_CHANNEL_CC_SKIP_COLLISION:
			break;
		case APX_COLLISION_CHANNEL_COMMON:
			cnl = CNL_COMMON;
			break;
		case APX_COLLISION_CHANNEL_CC_COMMON_HOST:
			cnl = CNL_INDEPENDENT_CCHOST;
			break;
		default:
			cnl = CNL_INDEPENDENT;
			break;
		}
		return cnl;
	}

	return m_ChannelRuntime;
}

void CPhysXObjDynamic::SetChannel(const CollisionChannel channel)
{
	if (ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (channel == m_ChannelRuntime)
			return;
	}
	else
	{
		if (channel == m_Channel)
			return;
		
		m_Channel = channel;
	}
	
	if (CNL_UNKNOWN == channel)
		return;

	ApplyChannel(channel);
}

void CPhysXObjDynamic::ApplyChannel(const CollisionChannel cnl)
{
	bool IsIndependent = false;
	m_ChannelRuntime = cnl;
	switch (cnl)
	{
	case CNL_COMMON:
		SetDefaultCollisionChannel();
		break;
	case CNL_INDEPENDENT:
		SetIndependentCollisionChannel(false);
		IsIndependent = true;
		break;
	case CNL_INDEPENDENT_CCHOST:
		SetIndependentCollisionChannel(true);
		IsIndependent = true;
		break;
	default:
		break;
	}
	
	if (IsIndependent)
	{
		if (0 != m_pAC)
		{
			APhysXCCBase* pCC = m_pAC->GetAPhysXCCBase();
			if (0 != pCC)
			{
				APhysXCollisionChannel cc;
				if (GetCollisionChannel(cc))
					pCC->SetHostModelChannel(cc);
			}
		}
	}
}*/
/*
bool CPhysXObjDynamic::UpdateNxActorsAttributes(const NxGroupsMask* pGroupMask, const bool ChangeToKinematic, NxActor**ppoutFirstActor)
{
	if (0 != ppoutFirstActor)
		*ppoutFirstActor = 0;

	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if(0 == pPSRB)
		return false;

	if ((0 == pGroupMask) && !ChangeToKinematic)
	{
		*ppoutFirstActor = pPSRB->GetNxActor(0);
		return true;
	}
	
	NxActor* pActor = 0;
	const int nCount = pPSRB->GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
	{
		pActor = pPSRB->GetNxActor(i);
		if (ChangeToKinematic && pActor->isDynamic())
			pActor->raiseBodyFlag(NX_BF_KINEMATIC);

		if (0 != pGroupMask)
		{
			const int nShapes = pActor->getNbShapes();
			NxShape*const* ppShapes = pActor->getShapes();
			for (int j = 0; j < nShapes; ++j)
				ppShapes[j]->setGroupsMask(*pGroupMask);
		}
	}

	if (0 != ppoutFirstActor)
		*ppoutFirstActor = pPSRB->GetNxActor(0);
	return true;
}
*/
int CPhysXObjDynamic::GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor) const
{
	APhysXCCBase* pCC = GetProperties()->GetCCMgr().GetAPhysXCCBase();
	if ((0 != pTestActor) && (0 != pCC))
	{
		if (pCC->IsComponent(*pTestActor))
			outVec.Add(pTestActor);
	}

	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return 0;
	
	int nCount = pPSRB->GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
		outVec.Add(pPSRB->GetNxActor(i));
	return nCount;
}

bool CPhysXObjDynamic::GetAPhysXInstanceAABB(NxBounds3& outAABB) const
{
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return false;

	pPSRB->GetAABB(outAABB);
	int nCount = pPSRB->GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
		outAABB.include(pPSRB->GetNxActor(i)->getGlobalPosition());
	return true;
}

/*
NxActor* CPhysXObjDynamic::GetFirstNxActor() const
{
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return 0;

	if (0 == pPSRB->GetNxActorNum())
		return 0;

	return pPSRB->GetNxActor(0);
}

bool CPhysXObjDynamic::PlayAttackAction(const IPhysXObjBase& objTarget)
{
	if (0 != m_pAC)
		m_pAC->FinishCurrentAction();

	return OnPlayAttackAction(objTarget);
}
*/
