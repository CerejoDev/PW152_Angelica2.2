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

CPhysXObjDynamic::CPhysXObjDynamic(const int type) : IPhysXObjBase(type)
{
	m_HasOneWayBackUp = false;
	m_BackUpOneWayFlag = false;
	m_HasBDGroupBackUp = false;
	m_BackUpBDGroup = APX_COLLISION_GROUP_DEFAULT;

	m_pAC = 0;
	OnSetToEmpty();
}

CPhysXObjDynamic::~CPhysXObjDynamic()
{
	delete m_pAC;
}

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

bool CPhysXObjDynamic::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	// compatible old version archive
	if (ReadPeopleInfo(OBPI_HAS_APHYSX_CC))
	{
		if ((CNL_INDEPENDENT != m_Channel) && (CNL_INDEPENDENT_CCHOST != m_Channel))
			m_Channel = CNL_INDEPENDENT_CCHOST;
	}

	if (IPhysXObjBase::OnEnterRuntime(aPhysXScene))
	{
		ApplyPeopleInfo(aPhysXScene);
		SetPlayEnable(true);
		return true;
	}
	return false;
}

void CPhysXObjDynamic::OnLeaveRuntime()
{
	IPhysXObjBase::OnLeaveRuntime();
	if (0 != m_pAC)
	{
		m_pAC->ReleaseAPhysxCC();
		m_pAC->ReleaseAPhysxLWCC();
		m_pAC->ReleaseAPhysxDynCC();
	}
	SetPlayEnable(false);
}

float CPhysXObjDynamic::ComputeYOffset(APhysXScene& aPhysXScene)
{
	float yOffset = 0;
	if ((0 == pCCLiveIn) || (&aPhysXScene != pCCLiveIn))
		return yOffset;
		
	if (ReadFlag(OBFI_IS_IN_RUNTIME) && (DRIVEN_BY_PURE_PHYSX == GetDrivenMode()))
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

void CPhysXObjDynamic::ApplyPeopleInfo(APhysXScene& aPhysXScene)
{
	if (!ReadFlag(OBFI_NEW_PEOPLE_INFO))
		return;

	if (ReadPeopleInfo(OBPI_IS_NONE_PEOPLE))
	{
		RecoverOneWayCollision();
		RecoverCollisionGroup();
		delete m_pAC;
		m_pAC = 0;
		return;
	}

	if (ReadPeopleInfo(OBPI_IS_MAIN_ACTOR))
		SetActionPlayer(ACMAWalk(*this));
	else if (ReadPeopleInfo(OBPI_IS_NPC))
	{
		if (ReadPeopleInfo(OBPI_ACTION_DANCE))
			SetActionPlayer(ACNPCDance(*this));
		else if (ReadPeopleInfo(OBPI_ACTION_WALK))
			SetActionPlayer(ACNPCWalk(*this, false));
		else if (ReadPeopleInfo(OBPI_ACTION_WALK_AROUND))
			SetActionPlayer(ACNPCWalk(*this, true));
		else if (ReadPeopleInfo(OBPI_ACTION_WALK_XAXIS))
			SetActionPlayer(ACNPCWalk(*this, true, true));
		else
		{
			RaisePeopleInfo(OBPI_ACTION_DANCE);
			SetActionPlayer(ACNPCDance(*this));
		}
	}

	if (ReadPeopleInfo(OBPI_HAS_APHYSX_CC))
	{
		assert(0 != m_pAC);
		if (0 == m_pAC)
			ClearPeopleInfo(OBPI_HAS_APHYSX_CC);
		else 
		{
			APhysXCharacterController* pCC = m_pAC->GetAPhysxCC();
			if (0 == pCC)
			{
				m_pAC->ReleaseAllCC();
				float yOffset = ComputeYOffset(aPhysXScene);
//				SetOneWayCollision();
//				SetCollisionGroupToCCHost();
				m_pAC->CreateAPhysxCC(aPhysXScene, yOffset);
			}
		}
	}
	else
	{
		if (0 != m_pAC)
			m_pAC->ReleaseAPhysxCC();
	}

	if (ReadPeopleInfo(OBPI_HAS_APHYSX_LWCC))
	{
		assert(0 != m_pAC);
		if (0 == m_pAC)
			ClearPeopleInfo(OBPI_HAS_APHYSX_LWCC);
		else
		{
			APhysXLWCharCtrler* pLWCC = m_pAC->GetAPhysxLWCC();
			if (0 == pLWCC)
			{
				m_pAC->ReleaseAllCC();
				float yOffset = ComputeYOffset(aPhysXScene);
//				SetOneWayCollision();
//				SetCollisionGroupToCCHost();
				m_pAC->CreateAPhysxLWCC(aPhysXScene, yOffset);
			}
		}
	}
	else
	{
		if (0 != m_pAC)
			m_pAC->ReleaseAPhysxLWCC();
	}
	
	if (ReadPeopleInfo(OBPI_HAS_APHYSX_DYNCC))
	{
		assert(0 != m_pAC);
		if (0 == m_pAC)
			ClearPeopleInfo(OBPI_HAS_APHYSX_DYNCC);
		else
		{
			m_pAC->ReleaseAllCC();
			APhysXDynCharCtrler* pDynCC = m_pAC->GetAPhysxDynCC();
			if (0 == pDynCC)
			{
				float yOffset = ComputeYOffset(aPhysXScene);
//				SetOneWayCollision();
//				SetCollisionGroupToCCHost();
				m_pAC->CreateAPhysxDynCC(aPhysXScene, yOffset);
			}
		}
	}
	else
	{
		if (0 != m_pAC)
			m_pAC->ReleaseAPhysxDynCC();
	}

	if (ReadPeopleInfo(OBPI_HAS_BRUSH_CC))
	{
		assert( 0!= m_pAC);
		if (0 == m_pAC)
			ClearPeopleInfo(OBPI_HAS_BRUSH_CC);
		else
		{
			BrushcharCtrler* pBrushCC = m_pAC->GetBrushCC();
			if (0 == pBrushCC)
			{
				m_pAC->ReleaseAllCC();
				m_pAC->CreateBrushCC();
			}
		}
	}
	else
	{
		if (0 != m_pAC)
			m_pAC->ReleaseBrushCC();
	}
	if (ReadPeopleInfo(OBPI_HAS_APHYSX_NXCC))
	{
		assert(0 != m_pAC);
		if (0 == m_pAC)
			ClearPeopleInfo(OBPI_HAS_APHYSX_DYNCC);
		else
		{
			APhysXNxCharCtrler* pNxCC = m_pAC->GetAPhysxNxCC();
			if (0 == pNxCC)
			{
				m_pAC->ReleaseAllCC();
				float yOffset = ComputeYOffset(aPhysXScene);
//				SetOneWayCollision();
				m_pAC->CreateAPhysxNxCC(aPhysXScene, yOffset);
			}
		}
	}
	else
	{
		if (0 != m_pAC)
			m_pAC->ReleaseAPhysxNxCC();
	}

	if (!ReadPeopleInfo(OBPI_HAS_APHYSX_CC) && !ReadPeopleInfo(OBPI_HAS_APHYSX_LWCC) && !ReadPeopleInfo(OBPI_HAS_APHYSX_DYNCC))
	{
		RecoverOneWayCollision();
		RecoverCollisionGroup();
	}
}

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
	//		ApplyChannel(m_Channel);
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

void CPhysXObjDynamic::SetPeopleFlags(const int flags, APhysXScene& aPhysXScene)
{
	if (GetPeopleInfo() == flags)
		return;
	
	if (flags & OBPI_IS_NONE_PEOPLE)
		RaisePeopleInfo(OBPI_IS_NONE_PEOPLE);
	else if (flags & OBPI_IS_MAIN_ACTOR)
		RaisePeopleInfo(OBPI_IS_MAIN_ACTOR);
	else if (flags & OBPI_IS_NPC)
		RaisePeopleInfo(OBPI_IS_NPC);

	if (flags & OBPI_HAS_APHYSX_CC)
		RaisePeopleInfo(OBPI_HAS_APHYSX_CC);
	else
		ClearPeopleInfo(OBPI_HAS_APHYSX_CC);
	
	if (flags & OBPI_HAS_APHYSX_LWCC)
		RaisePeopleInfo(OBPI_HAS_APHYSX_LWCC);
	else
		ClearPeopleInfo(OBPI_HAS_APHYSX_LWCC);

	if (flags & OBPI_HAS_APHYSX_DYNCC)
		RaisePeopleInfo(OBPI_HAS_APHYSX_DYNCC);
	else
		ClearPeopleInfo(OBPI_HAS_APHYSX_DYNCC);

	if (flags & OBPI_HAS_BRUSH_CC)
		RaisePeopleInfo(OBPI_HAS_BRUSH_CC);
	else
		ClearPeopleInfo(OBPI_HAS_BRUSH_CC);
	
	if (flags & OBPI_HAS_APHYSX_NXCC)
		RaisePeopleInfo(OBPI_HAS_APHYSX_NXCC);
	else
		ClearPeopleInfo(OBPI_HAS_APHYSX_NXCC);

	if (flags & OBPI_ACTION_DANCE)
		RaisePeopleInfo(OBPI_ACTION_DANCE);
	else if (flags & OBPI_ACTION_WALK)
		RaisePeopleInfo(OBPI_ACTION_WALK);
	else if (flags & OBPI_ACTION_WALK_AROUND)
		RaisePeopleInfo(OBPI_ACTION_WALK_AROUND);
	else if (flags & OBPI_ACTION_WALK_XAXIS)
		RaisePeopleInfo(OBPI_ACTION_WALK_XAXIS);

	if (ReadFlag(OBFI_IS_IN_RUNTIME))
		ApplyPeopleInfo(aPhysXScene);
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
		const CPhysXObjDynamic::CollisionChannel cnl = GetChannel(true);
		if (channel == cnl)
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
				assert(!"Shouldn't be here. To update");
			}
		}
	}
}

bool CPhysXObjDynamic::OnTickMove(float deltaTimeSec)
{
	if (0 != m_pAC)
	{
		if (ReadFlag(OBFI_IS_IN_RUNTIME) && (DRIVEN_BY_PURE_PHYSX != GetDrivenMode()))
			m_pAC->UpdateMove(deltaTimeSec);
	}
	return true;
}

bool CPhysXObjDynamic::OnTickAnimation(const unsigned long dwDeltaTime)
{
	if (0 == m_pAC) 
		return false;
	
	if (!m_PlayEnable)
		return false;
	
	if (DRIVEN_BY_PURE_PHYSX == GetDrivenMode())
		return false;
	
	if (!m_IsRestart)
	{
		m_pAC->PlayAction(dwDeltaTime, false);
		return true;
	}

	m_pAC->FinishCurrentAction();
	m_pAC->PlayAction(dwDeltaTime, true);
	m_IsRestart = false;
	return true;
}

void CPhysXObjDynamic::SetActionPlayer(const IActionPlayerBase& ac)
{
	if (0 != m_pAC)
	{
		if (ac.GetTypeID() == m_pAC->GetTypeID())
			return;

		delete m_pAC;
	}

	m_pAC = ac.Clone();
}

void CPhysXObjDynamic::FinishCurrentAction()
{
	if (0 != m_pAC)
		m_pAC->FinishCurrentAction(); 
}

bool CPhysXObjDynamic::GetPhysXRigidBodyAABB(NxBounds3& outAABB) const
{
	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return false;
	
	IPhysXObjBase::GetPhysXRigidBodyAABB(*pPSRB, outAABB);
	return true;
}

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

int CPhysXObjDynamic::GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor) const
{
	APhysXCCBase* pCC = (0 == m_pAC)? 0 : m_pAC->GetAPhysXCCBase();
	if ((0 != pTestActor) && (0 != pCC))
	{
		if (pCC->IsComponent(*pTestActor))
			outVec.push_back(pTestActor);
	}

	APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
	if (0 == pPSRB)
		return 0;
	
	int nCount = pPSRB->GetNxActorNum();
	for (int i = 0; i < nCount; ++i)
		outVec.push_back(pPSRB->GetNxActor(i));
	return nCount;
}

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

A3DVECTOR3 CPhysXObjDynamic::GetHeadTopPos(A3DMATRIX4* pOutMat) const
{
	A3DVECTOR3 pos = GetPos();

	A3DSkinModel* pModel = GetSkinModel();
	A3DVECTOR3 up = pModel->GetUp();
	if (0 != pOutMat)
		*pOutMat = pModel->GetAbsoluteTM();

	up *= m_Height* 1.05f;
	pos += up;
	return pos;
}

bool CPhysXObjDynamic::ApplyPhysXBody(const bool bEnable, APhysXScene* pScene)
{
	bool lastVal = ReadFlag(OBF_ENABLE_PHYSX_BODY);
	if (bEnable == lastVal)
		return true;
	
	if (bEnable)
	{
		RaiseFlag(OBF_ENABLE_PHYSX_BODY);
		if (ReadFlag(OBFI_IS_IN_RUNTIME))
		{
			if (0 == pScene)
				return false;
			
			if (!InstancePhysXObj(*pScene))
				return false;
			
			ApplyChannel(m_ChannelRuntime);
			if (0 != m_pAC)
			{
				APhysXCCBase* pCC = m_pAC->GetAPhysXCCBase();
				if (0 != pCC)
				{
					bool bHasNewCnl = false;
					APhysXCollisionChannel oldCNL, newCNL;
					APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
					if (0 != pPSRB)
					{
						oldCNL = pPSRB->GetCollisionChannel();
						if (pCC->RegisterHostModel(pPSRB, newCNL))
							bHasNewCnl = true;
					}
					if (bHasNewCnl)
					{
						if (oldCNL != newCNL)
						{
							assert(APX_COLLISION_CHANNEL_CC_COMMON_HOST == newCNL);
							// note here: EC model keep the old channel value because RegisterHostModel() update the channel value around ECM 
						}
					}
				}
			}
		}
	}
	else
	{
		if (0 != m_pAC)
		{
			APhysXCCBase* pCC = m_pAC->GetAPhysXCCBase();
			if (0 != pCC)
			{
				APhysXSkeletonRBObject* pPSRB = GetSkeletonRBObject();
				if (0 != pPSRB)
				{
					APhysXCollisionChannel oldCNL, origCNL;
					oldCNL = pPSRB->GetCollisionChannel();
					if (pCC->UnRegisterHostModel(pPSRB, origCNL))
					{
						if (oldCNL != origCNL)
						{
							assert(APX_COLLISION_CHANNEL_COMMON == origCNL);
							SetDefaultCollisionChannel();
						}
					}
				}
			}
		}
		ClearFlag(OBF_ENABLE_PHYSX_BODY);
		if (ReadFlag(OBFI_IS_IN_RUNTIME))
			ReleasePhysXObj();

	}
	return true;
}

void CPhysXObjDynamic::OnChangeDrivenMode(const DrivenMode dmNew)
{
	if (DRIVEN_BY_PURE_PHYSX != dmNew)
	{
		ResetPose();

		if (DRIVEN_BY_PURE_PHYSX != GetDrivenMode())
			return;
		
		if (!ReadFlag(OBFI_IS_IN_RUNTIME))
			return;
		
		if (0 != pCCLiveIn)
		{
			RaisePeopleInfo(FlagOfCCType);
			ApplyPeopleInfo(*pCCLiveIn);
		}
	}
	else
	{
		pCCLiveIn = 0;
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
		}
	}
}

bool CPhysXObjDynamic::SetCollisionGroupToCCHost()
{
	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if(0 == pSRB)
		return false;

    APhysXCollisionGroup oldGroup = pSRB->GetCollisionGroup();
	if (APX_COLLISION_GROUP_CC_HOST_RB != oldGroup)
	{
		m_HasBDGroupBackUp = true;
		m_BackUpBDGroup = oldGroup;
		pSRB->SetCollisionGroup(APX_COLLISION_GROUP_CC_HOST_RB);
	}
	return true;
}

bool CPhysXObjDynamic::RecoverCollisionGroup()
{
	if (!m_HasBDGroupBackUp)
		return true;
	m_HasBDGroupBackUp = false;
	
	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if(0 == pSRB)
		return true;
	
	pSRB->SetCollisionGroup(m_BackUpBDGroup);
	return true;
}

bool CPhysXObjDynamic::SetOneWayCollision()
{
	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if(0 == pSRB)
		return false;

	bool currentFlag = pSRB->GetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION);
	if (currentFlag)
		return true;

	if (!m_HasOneWayBackUp)
	{
		m_HasOneWayBackUp = true;
		m_BackUpOneWayFlag = currentFlag;
	}
	pSRB->SetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION, true);
	return true;
}

bool CPhysXObjDynamic::RecoverOneWayCollision()
{
	if (!m_HasOneWayBackUp)
		return true;
	m_HasOneWayBackUp = false;

	APhysXSkeletonRBObject* pSRB = GetSkeletonRBObject();
	if(0 == pSRB)
		return true;
	
	pSRB->SetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION, m_BackUpOneWayFlag);
	return true;
}

void CPhysXObjDynamic::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	return;
}
