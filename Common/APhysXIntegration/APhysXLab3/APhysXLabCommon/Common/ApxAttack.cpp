#include "stdafx.h"
#include "ApxAttack.h"
#include "DetectFilterMgr.h"
#include "A3DSkillGfxComposer.h"

#ifndef _ANGELICA3

A3DSkillGfxEvent* A3DSkillGfxEventMan::CreateOneEvent(GfxMoveMode mode)
{
	return new ApxAttackEvent(this, mode);
}

bool A3DSkillGfxEventMan::GetPositionById(clientid_t nId, A3DVECTOR3& vPos, _SGC::GfxHitPos HitPos, const char* szHook, bool bRelHook, const A3DVECTOR3* pOffset, const char* szHanger, bool bChildHook)
{
	IPhysXObjBase* pObj = (IPhysXObjBase* ) nId;
	if(pObj == m_pHitTarget)
	{
		vPos = m_vHitPos;
	}
	else
	{
		vPos = APhysXConverter::N2A_Vector3(pObj->GetGPos());
		vPos.y += 1.0f;
	}
	return true;
}

bool A3DSkillGfxEventMan::GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp)
{
	IPhysXObjBase* pObj = (IPhysXObjBase* ) nId;
	pObj->GetGDirAndUp(vDir, vUp);
	return true;
}

float A3DSkillGfxEventMan::GetTargetScale(clientid_t nTargetId)
{
	return 1.0f;
}

#endif

bool SweepReport::onEvent(NxU32 nbEntities, NxSweepQueryHit* entities)
{
	for (NxU32 i = 0; i < nbEntities; ++i)
		AddToArray(entities[i]);
	return true;
}

void SweepReport::AddToArray(const NxSweepQueryHit& sqh)
{
	NxActor& hitActor = sqh.hitShape->getActor();
	IPhysXObjBase* pHitObj = ObjManager::GetInstance()->GetPhysXObject(hitActor);
	if (0 == pHitObj)
		return;

	assert(0 != m_pCaster);
	if (pHitObj == m_pCaster)
		return;

	size_t nCount = m_HitInfo.size();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		if (sqh.hitShape == m_HitInfo[i].result.hitShape)
			return;
		if (pHitObj == m_HitInfo[i].result.userData)
		{
			if (sqh.t < m_HitInfo[i].result.t)
			{
				m_HitInfo[i].result = sqh;
				m_HitInfo[i].result.userData = pHitObj;
			}
			return;
		}
	}

	sweepData sd;
	sd.motionDir = m_motionDir;
	sd.result = sqh;
	sd.result.userData = pHitObj;
	m_HitInfo.push_back(sd);
}

bool RaycastReport::onHit(const NxRaycastHit& hit)
{
	assert(NX_RAYCAST_SHAPE & hit.flags);
	NxActor* pHitActor = &(hit.shape->getActor());
	if (m_pTargetActor == pHitActor)
		m_bIsHitTarget = true;
	return true;
}

//-------------------------------------------------------------------------------------
// implementation of ApxAttackEvent

ApxAttackEvent::TargetData::TargetData()
{
	m_hitLocalPos.set(0.0f);
}

void ApxAttackEvent::TargetData::UpdateLoacalHitPos(const NxVec3& hitDir)
{
	if (0 == m_aaai.mApxAttack.mpTarget)
	{
		m_hitLocalPos.set(0.0f);
		return;
	}

	A3DMATRIX4 matPose(A3DMATRIX4::IDENTITY);
	m_aaai.mApxAttack.mpTarget->GetGPose(matPose);
	NxMat34 matNxPose;
	APhysXConverter::A2N_Matrix44(matPose, matNxPose);

	NxMat34 mat;
	matNxPose.getInverse(mat);
	NxVec3 hpOutside = m_aaai.mApxDamage.mHitPos - hitDir * 0.1f;
	mat.multiply(hpOutside, m_hitLocalPos);
}

float ApxAttackEvent::gCapsuleRadius = 0.25f;
bool ApxAttackEvent::gEnableDebugRender = false;
IRenderManager* ApxAttackEvent::gpRenderManager = 0;

IRenderManager* ApxAttackEvent::GetRenderManager()
{
	return gpRenderManager;
}

void ApxAttackEvent::SetRenderManager(IRenderManager* pRM)
{
	gpRenderManager = pRM;
}

void ApxAttackEvent::EnableDebugRender(bool bEnable)
{
	gEnableDebugRender = bEnable;
}

bool ApxAttackEvent::IsDebugRenderEnable()
{
	if (0 == gpRenderManager)
		return false;
	return gEnableDebugRender;
}

void ApxAttackEvent::SetAttDamInfo(const ApxAttackActionInfo& aaai)
{
	m_bEnable = true;
	if (IsDebugRenderEnable())
		m_DebugRender.Register(gpRenderManager);

	m_TDFromUser.m_aaai = aaai;
	m_TDFromUser.m_aaai.mApxAttack.mDamageDelayStartSec = 0;
	m_TDFromUser.m_aaai.mApxAttack.mDamageDurationSec = 0;
	ClearPhysXTargetData();

	NxVec3 hitDir = aaai.mApxDamage.mHitDir;
	hitDir.normalize();
	m_TDFromUser.UpdateLoacalHitPos(hitDir);

	if (0 != aaai.mApxAttack.mpTarget)
		m_bIsGetFixPoint = true;
}

IAssailable* ApxAttackEvent::GetTargetIA(bool* poutIsFromPhysX) const
{
	if (0 < m_pFromPhysX.GetSize())
	{
		if (0 != poutIsFromPhysX)
			*poutIsFromPhysX = true;
		return m_pFromPhysX[0]->m_aaai.mApxAttack.mpTarget; 
	}

	if (0 != poutIsFromPhysX)
		*poutIsFromPhysX = false;
	return m_TDFromUser.m_aaai.mApxAttack.mpTarget;
}

const NxVec3& ApxAttackEvent::GetLocalHitPos(const bool bIsFromPhysX) const
{
	if (bIsFromPhysX)
		return m_pFromPhysX[0]->m_hitLocalPos;
	return m_TDFromUser.m_hitLocalPos;
}

void ApxAttackEvent::Tick(DWORD dwDeltaTime)
{
	m_DebugRender.Clear();
	if (m_bEnable)
	{
		ApxAttackSkillGfxMan* pAttackMan = (ApxAttackSkillGfxMan*)m_pMan;
		if (pAttackMan->IsClearAttackQueue())
			m_bEnable = false;
	}
	else
	{
		if (0 == dwDeltaTime)
		{
			ApxAttackSkillGfxMan* pAttackMan = (ApxAttackSkillGfxMan*)m_pMan;
			if (!pAttackMan->IsClearAttackQueue())
				m_bEnable = true;
		}
	}
	A3DSkillGfxEvent::Tick(dwDeltaTime);

	if (m_bEnable && (enumFlying == m_enumState))
	{
		if (m_TDFromUser.m_aaai.mApxAttack.mIsPhysXDetection)
		{
#ifdef _ANGELICA3
			A3DVECTOR3 moveSeg(0, 0, 0);
			if (GetDeltaMoveSegment(moveSeg))
			{
				if (CheckHitTargetBySweep(moveSeg, m_pMoveMethod->GetPos()))
					HitTarget();
			}
#endif
		}
	}
}

void ApxAttackEvent::HitTarget()
{
	m_bIsGetHitInfo = true;
	if (0 == GetCastTargetID())
		SetCastTargetID(reinterpret_cast<int>(GetTargetIA()));

	A3DSkillGfxEvent::HitTarget();
#ifdef _ANGELICA3
	if (IsTimeout())
	{
		m_bIsGetHitInfo = false;
		return;
	}
#endif

	GeneralAttackMgr* pGAM = IAssailable::GetGeneralAttackMgr();
	if (0 != pGAM)
	{
		int nCount = m_pFromPhysX.GetSize();
		for (int i = 0; i < nCount; ++i)
			pGAM->AddCurrentDamage(m_pFromPhysX[i]->m_aaai);

		if (0 == nCount)
		{
			bool bIsAttackUser = true;
			if (IsGetFixPoint())
			{
				if (m_TDFromUser.m_aaai.mApxAttack.mIsPhysXDetection)
				{
					bIsAttackUser = false;
					CheckHitTargetBySweep();
					int nSize = m_pFromPhysX.GetSize();
					for (int j = 0; j < nSize; ++j)
						pGAM->AddCurrentDamage(m_pFromPhysX[j]->m_aaai);
				}
			}

			if (bIsAttackUser)
			{
				if (0 != m_TDFromUser.m_aaai.mApxAttack.mpTarget)
				{
					m_TDFromUser.m_aaai.mApxAttack.mpTarget->OnPreAttackedNotify(m_TDFromUser.m_aaai);
					pGAM->AddCurrentDamage(m_TDFromUser.m_aaai);
				}
			}
		}
	}
	m_bIsGetHitInfo = false;
}

void ApxAttackEvent::ClearPhysXTargetData()
{
	int nCount = m_pFromPhysX.GetSize();
	for (int i = 0; i < nCount; ++i)
		delete m_pFromPhysX[i];
	m_pFromPhysX.RemoveAll(false);
}

bool ApxAttackEvent::DoPhysXSweep(const NxVec3& start, const NxVec3& motion)
{
	IPhysXObjBase* pObjCaster = (IPhysXObjBase*)m_nCasterID;
	if (0 == pObjCaster)
		return false;

	CPhysXObjDynamic* pDynObj = dynamic_cast<CPhysXObjDynamic*>(pObjCaster);
	if (0 == pDynObj)
		return false;

	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = start;
	worldCapsule.radius = gCapsuleRadius;

	if (IsDebugRenderEnable())
	{
		NxVec3 pos = start + motion;
		m_DebugRender.AddSphere(APhysXConverter::N2A_Vector3(pos), worldCapsule.radius, 0xffaa8856);
	}

	ApxAttackSkillGfxMan* pAttackMan = (ApxAttackSkillGfxMan*)m_pMan;
	Scene* pScene = pAttackMan->GetScene();
	APhysXScene* pAPhysXScene = pScene->GetAPhysXScene();
	NxScene* pNxScene = pAPhysXScene->GetNxScene();
	DetectFilterMgr dfMgr;
	if (!dfMgr.FilterBegin(*pAPhysXScene))
		return false;

	dfMgr.FilterOutGroup(APX_COLLISION_GROUP_ATTACHER);
	dfMgr.FilterOutGroup(APX_COLLISION_GROUP_CLOTH_ATTACHER);
	dfMgr.FilterOutDynObject(pDynObj);

	m_stReport.m_pCaster = pObjCaster;
	m_stReport.m_HitInfo.clear();
	NxU32 flags = NX_SF_STATICS | NX_SF_DYNAMICS | NX_SF_ALL_HITS;
	pNxScene->linearCapsuleSweep(worldCapsule, motion, flags, 0, 0, 0, &m_stReport, dfMgr.GetActiveGroup(), dfMgr.GetGroupsMask());
	if (0 < m_stReport.m_HitInfo.size())
		return true;
	return false;
}

void ApxAttackEvent::HandleSweepResult(const NxVec3& start, const NxVec3& dir, float dist)
{
	IPhysXObjBase* pObjCaster = (IPhysXObjBase*)m_nCasterID;
	ApxAttackSkillGfxMan* pAttackMan = (ApxAttackSkillGfxMan*)m_pMan;
	Scene* pScene = pAttackMan->GetScene();
	assert(0 == m_pFromPhysX.GetSize());

	size_t nCount = m_stReport.m_HitInfo.size();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		IPhysXObjBase* pHitObj =  reinterpret_cast<IPhysXObjBase*>(m_stReport.m_HitInfo[i].result.userData);
		if (0 != pHitObj)
		{
			TargetData* pTD = new TargetData(m_TDFromUser);
			if (0 != pTD)
			{
				NxActor* pActor = &(m_stReport.m_HitInfo[i].result.hitShape->getActor());
				pTD->m_aaai.mApxAttack.mpTarget = pScene->GetActorBase(pHitObj);
				if (0 == pTD->m_aaai.mApxAttack.mpTarget)
				{
					pTD->m_aaai.mApxAttack.mpTarget = pHitObj;
					float mass = pActor->getMass();
					pTD->m_aaai.mApxDamage.mHitMomentum *= mass;
				}
				if (0 == i)
					SetCastTargetID(reinterpret_cast<int>(pTD->m_aaai.mApxAttack.mpTarget));  // to update: GfxCommon only support one target
				pTD->m_aaai.mApxDamage.mHitRBActor = pActor;
				pTD->m_aaai.mApxDamage.mHitPos = m_stReport.m_HitInfo[i].result.point;
				pTD->m_aaai.mApxDamage.mHitDir = dir;
				pTD->m_aaai.UpdateDamageSource();
				pTD->m_aaai.mApxDamage.mHitMomentum *= ComputeHitForceRatio(start, dir, dist, m_stReport.m_HitInfo[i].result.point, pActor);
				pTD->UpdateLoacalHitPos(dir);
				pTD->m_aaai.mApxAttack.mpTarget->OnPreAttackedNotify(pTD->m_aaai);
				m_pFromPhysX.Add(pTD);
			}
		}
	}
}

float ApxAttackEvent::ComputeHitForceRatio(const NxVec3& start, const NxVec3& dir, float dist, const NxVec3& hitPos, NxActor* pHitActor)
{
	NxRay nxRay;
	nxRay.orig = start;
	nxRay.dir = dir;

	ApxAttackSkillGfxMan* pAttackMan = (ApxAttackSkillGfxMan*)m_pMan;
	Scene* pScene = pAttackMan->GetScene();
	APhysXScene* pAPhysXScene = pScene->GetAPhysXScene();

	m_rcReport.m_bIsHitTarget = false;
	m_rcReport.m_pTargetActor = pHitActor;
	NxU32 nbHit = pAPhysXScene->GetNxScene()->raycastAllShapes(nxRay, m_rcReport, NX_ALL_SHAPES, 0xffffffff, dist, NX_RAYCAST_SHAPE);
	if (m_rcReport.m_bIsHitTarget)
		return 1.0f;

	NxPlane plane(start, nxRay.dir);
	NxVec3 hitPosProj = plane.project(hitPos);
	float d = start.distance(hitPosProj);
	float halfR = 0.5f * gCapsuleRadius;
	float res = (halfR - d) / halfR;
	if (0 < res)
		res = 1.0f;
	else
		res *= -1.0f;
	if (0.2f > res)
		res = 0.2f;
	return res;
}

bool ApxAttackEvent::CheckHitTargetBySweep()
{
#ifdef _ANGELICA3
	bool bIsFromPhysX = false;
	IAssailable* pTarget = GetTargetIA(&bIsFromPhysX);
	if (bIsFromPhysX)
		return false;
	if (0 == pTarget)
		return false;

	NxVec3 start = APhysXConverter::A2N_Vector3(m_vLastPoint);
	A3DVECTOR3 dist = m_vFixedPoint - m_vLastPoint;
	NxVec3 motion = APhysXConverter::A2N_Vector3(dist);
	NxVec3 dir(motion);
	float fDist = dir.normalize();
	fDist += 0.1f;
	motion = dir * fDist;
	if (DoPhysXSweep(start, motion))
	{
		HandleSweepResult(start, dir, fDist);
		return true;
	}
#endif
	return false;
}

bool ApxAttackEvent::CheckHitTargetBySweep(const A3DVECTOR3& deltaMove, const A3DVECTOR3& instantPos)
{
	float dist = deltaMove.Magnitude();
	if (NxMath::equals(dist, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;

	NxVec3 start = APhysXConverter::A2N_Vector3(instantPos - deltaMove);
	NxVec3 motion = APhysXConverter::A2N_Vector3(deltaMove);
	NxVec3 dir(motion);
	float fDist = dir.normalize();
	if (DoPhysXSweep(start, motion))
	{
		HandleSweepResult(start, dir, fDist);
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------
// implementation of ApxAttackSkillGfxMan

A3DSkillGfxEvent* ApxAttackSkillGfxMan::CreateOneEvent(GfxMoveMode mode)
{
	ApxAttackEvent* pAAE = new ApxAttackEvent(this, mode);
#ifdef _ANGELICA3
	pAAE->SetUserData(pAAE);
#else
	pAAE->SetUserData((DWORD)pAAE);
#endif
	return pAAE;
}

bool ApxAttackSkillGfxMan::GetPositionById(const GETPOSITIONBYID_PARAMS& param, A3DVECTOR3& vPos)
{
	if (0 == param.m_nId)
		return false;

	ApxAttackEvent* pAAE = (ApxAttackEvent*)param.m_pUserData;
	if (0 == pAAE)
	{
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}
	if (!pAAE->IsEnable())
		return false;

	if (param.m_nId == pAAE->GetCastTargetID())
	{
		bool bIsFromPhysX = false;
		IAssailable* pTarget = pAAE->GetTargetIA(&bIsFromPhysX);
		assert((int)param.m_nId == (int)pTarget);
		if (!bIsFromPhysX)
		{
			if (pAAE->IsGetFixPoint())
			{
				const ApxAttackActionInfo& aaai = pAAE->GetAttDamInfo();
				vPos = APhysXConverter::N2A_Vector3(aaai.mApxDamage.mHitPos);
				A3DVECTOR3 dir = APhysXConverter::N2A_Vector3(m_aaai.mApxDamage.mHitDir);
				vPos -= dir * 0.1f;
				return true;
			}
		}
		A3DMATRIX4 matPose(A3DMATRIX4::IDENTITY);
		pTarget->GetGPose(matPose);
		NxMat34 matNxPose;
		APhysXConverter::A2N_Matrix44(matPose, matNxPose);
		NxVec3 nxpos;
		matNxPose.multiply(pAAE->GetLocalHitPos(bIsFromPhysX), nxpos);
		vPos = APhysXConverter::N2A_Vector3(nxpos);
		return true;
	}
	else if (param.m_nId == pAAE->GetCasterID())
	{
		IPhysXObjBase* pObj = (IPhysXObjBase*)param.m_nId;
		vPos = APhysXConverter::N2A_Vector3(pObj->GetGPos());
#ifdef _ANGELICA3
		if (enumHitCenter == param.m_HitPos)
#else
		if (_SGC::enumHitCenter == param.m_HitPos)
#endif
			vPos += pObj->GetProperties()->GetUp();
		return true;
	}
	else
	{
		assert(!"Shouldn't be here! Debug it!");
	}
	return false;
}

bool ApxAttackSkillGfxMan::GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp)
{
	assert(!"To debug nId is caster or target!");
	IAssailable* pTarget = (IAssailable*)nId;
	IPhysXObjBase* pPhysXTarget = dynamic_cast<IPhysXObjBase*>(pTarget);
	if (0 != pPhysXTarget)
	{
		pPhysXTarget->GetGDirAndUp(vDir, vUp);
		return true;
	}
	return false;
}

float ApxAttackSkillGfxMan::GetTargetScale(clientid_t nTargetId)
{
	IAssailable* pTarget = (IAssailable*)nTargetId;
	IPhysXObjBase* pPhysXTarget = dynamic_cast<IPhysXObjBase*>(pTarget);
	if (0 != pPhysXTarget)
		return pPhysXTarget->GetGScale();
	return 1.0f;
}

void ApxAttackSkillGfxMan::PushEvent(clientid_t nCasterId, A3DSkillGfxEvent* pEvent)
{
	A3DSkillGfxEventMan::PushEvent(nCasterId, pEvent);
	if (m_IsNewAAI)
	{
		ApxAttackEvent* pAAE = dynamic_cast<ApxAttackEvent*>(pEvent);
		pAAE->SetAttDamInfo(m_aaai);
		EmptyAttackInfo();
	}
}


//-------------------------------------------------------------------------------------
// implementation of GeneralAttackMgr

void ApxTargetHistory::AddHitTarget(CPhysXObjDynamic& target)
{
	CPhysXObjDynamic* pT = &target;
	std::vector<HitObjStat>::iterator it = m_History.begin();
	std::vector<HitObjStat>::iterator itEnd = m_History.end();
	for (; it != itEnd; ++it)
	{
		if (it->pHitObj == pT)
		{
			it->nHitCount += 1;
			return;
		}
	}
	m_History.push_back(HitObjStat(target));
}

int ApxTargetHistory::GetHistoryTargetObjs(APtrArray<CPhysXObjDynamic*>& outObjs)
{
	std::vector<HitObjStat>::iterator it = m_History.begin();
	std::vector<HitObjStat>::iterator itEnd = m_History.end();
	for (; it != itEnd; ++it)
		outObjs.Add(it->pHitObj);
	return outObjs.GetSize();
}

bool OLTestReport::onEvent(NxU32 nbEntities, NxShape** entities)
{
	bool bIsNew = false;
	int nbActor = 0;
	NxActor* pActor = 0;
	NxShape* pShape = 0;
	for (NxU32 i = 0; i < nbEntities; ++i)
	{
		pShape = entities[i];
		if (pShape->getFlag(NX_TRIGGER_ENABLE))
			continue;

		pActor = &(pShape->getActor());
		bIsNew = true;
		nbActor = m_pArray.GetSize();
		for (int i = 0; i < nbActor; ++i)
		{
			if (m_pArray[i] == pActor)
			{
				bIsNew = false;
				break;
			}
		}
		if (bIsNew)
			m_pArray.Add(pActor);
	}
	return true;
}

GeneralAttackMgr* IAssailable::pgGAM = 0;

bool GeneralAttackMgr::ToDelTarget::operator() (ApxAttackActionInfo* pAAAI) const
{
	assert(0 != m_pIA);
	if (0 == pAAAI)
		return true;
	if (pAAAI->mApxAttack.mpTarget == m_pIA)
		return true;
	return false;
}

bool GeneralAttackMgr::ToDelCaster::operator() (ApxAttackActionInfo* pAAAI) const
{
	if (0 == pAAAI)
		return true;
	if (pAAAI->mApxDamage.mpAttacker == m_pIA)
		return true;
	return false;
}

GeneralAttackMgr::GeneralAttackMgr()
{
	m_pScene = 0;
	m_pAASGM = 0;
	m_nPoseCount = 0;
	m_nCurrentFrame = 0;
	m_nFrameCount = 0;
	m_nDebugPoseCount = 0;
	m_nDebugSphereCount = 0;
	m_nDebugLineCount = 0;
}

GeneralAttackMgr::~GeneralAttackMgr()
{
	ReleaseAttackManager();
}

bool GeneralAttackMgr::InitAttackManager(Scene& scene, A3DGFXExMan& gfxExMan)
{
	m_pScene = &scene;
	if (0 == m_pAASGM)
		m_pAASGM = new ApxAttackSkillGfxMan(m_pScene);
	if (0 != m_pAASGM)
	{
		bool bIsInited = false;
#ifdef _ANGELICA3
		bIsInited = m_pAASGM->Init(&gfxExMan);
#else
		m_pAASGM->Init(scene.GetRender()->GetA3DDevice());
		bIsInited = true;
#endif
		if (bIsInited)
		{
			IAssailable::SetGeneralAttackMgr(this);
			return true;
		}
	}
	return false;
}

void GeneralAttackMgr::ReleaseAttackManager()
{
	ClearAttackQueue();

	if (0 != m_pAASGM)
	{
		m_pAASGM->Release();
		delete m_pAASGM;
		m_pAASGM = 0;
	}
	m_pScene = 0;
	IAssailable::SetGeneralAttackMgr(0);
}

void GeneralAttackMgr::ClearAttackQueue(bool bClear)
{
	attInfoQueue::iterator it = m_arrAttacksQueue.begin();
	attInfoQueue::iterator itEnd = m_arrAttacksQueue.end();
	for (; it != itEnd; ++it)
		delete (*it);
	m_arrAttacksQueue.clear();

	it = m_curDamageQueue.begin();
	itEnd = m_curDamageQueue.end();
	for (; it != itEnd; ++it)
		delete (*it);
	m_curDamageQueue.clear();

	if (0 != m_pAASGM)
		m_pAASGM->ClearAttackQueue(bClear);
	ClearFrameDebugData();
}

bool GeneralAttackMgr::Tick(float dtSec)
{
	unsigned int dtMSec = unsigned int(dtSec * 1000);
	if (0 != m_pAASGM)
		m_pAASGM->Tick(dtMSec);

	TickHitDamageForTimeDelay(dtSec);
	return true;
}

void GeneralAttackMgr::Render()
{
	if (0 != m_pAASGM)
		m_pAASGM->Render();
	m_DebugRender.Clear();
	if (ApxAttackEvent::IsDebugRenderEnable())
		BuildDebugPoseData();
}

void GeneralAttackMgr::RenderNextFrameDebugData()
{
	m_nCurrentFrame += 1;
	if (m_nCurrentFrame >= m_nFrameCount)
		m_nCurrentFrame = 0;
}

void GeneralAttackMgr::ClearFrameDebugData()
{
	m_nCurrentFrame = 0;
	m_nFrameCount = 0;
	m_nDebugPoseCount = 0;
	m_nDebugSphereCount = 0;
	m_nDebugLineCount = 0;
}

bool GeneralAttackMgr::AddCurrentDamage(const ApxAttackActionInfo& apxAttActInfo)
{
	assert(false == (0 < apxAttActInfo.mApxAttack.mDamageDelayStartSec));
	if (3 == apxAttActInfo.mApxAttack.mAttackType)
	{
		assert(0 == apxAttActInfo.mApxDamage.mpAttacker);
	}
	else
	{
		assert(0 != apxAttActInfo.mApxDamage.mpAttacker);
	}
	if (0 == apxAttActInfo.mApxAttack.mpTarget)
		return false;

	ApxAttInfoHistory* pAAIH = new ApxAttInfoHistory(apxAttActInfo);
	if (0 != pAAIH)
	{
		m_curDamageQueue.push_back(pAAIH);
		BreakLastAttackAction(apxAttActInfo.mApxAttack.mpTarget);
		return true;
	}
	return false;
}

void GeneralAttackMgr::OnPlayAttackBegin(const ApxAttackActionInfo& apxAttActInfo)
{
	if (3 == apxAttActInfo.mApxAttack.mAttackType)
	{
		assert(0 == apxAttActInfo.mApxDamage.mpAttacker);
	}
	else
	{
		assert(0 != apxAttActInfo.mApxDamage.mpAttacker);
	}
	if (apxAttActInfo.mAnimInfo.mBasicAction)
	{
		assert(false == (0.0f > apxAttActInfo.mApxAttack.mDamageDelayStartSec));
		assert(false == (0.0f > apxAttActInfo.mApxAttack.mDamageDurationSec));
		ApxAttInfoHistory* pAAIH = new ApxAttInfoHistory(apxAttActInfo);
		if (0 != pAAIH)
		{
			ClearFrameDebugData();
			m_arrAttacksQueue.push_back(pAAIH);
			if (apxAttActInfo.mApxAttack.mIsPhysXDetection)
			{
				if (ApxAttackEvent::IsDebugRenderEnable())
					m_DebugRender.Register(ApxAttackEvent::GetRenderManager());
			}
		}
	}
	else
	{
		assert(true == NxMath::equals(0.0f, apxAttActInfo.mApxAttack.mDamageDelayStartSec, 0.00001f));
		assert(true == NxMath::equals(0.0f, apxAttActInfo.mApxAttack.mDamageDurationSec, 0.00001f));
		if (0 != m_pAASGM)
			m_pAASGM->SetAttDamInfo(apxAttActInfo);
	}
}

void GeneralAttackMgr::OnPlayAttackEnd(const ApxAttackActionInfo& apxAttActInfo)
{
	if (!apxAttActInfo.mAnimInfo.mBasicAction)
		return;

	if (apxAttActInfo.mApxAttack.mIsPhysXDetection)
	{
		ToDelCaster delCaster(apxAttActInfo.mApxDamage.mpAttacker);
		RemoveAttackAction(delCaster, m_arrAttacksQueue, m_arrAttacksQueue.begin(), true);
	}
}

NxVec3 GeneralAttackMgr::GetFrontalPlane(NxPlane& outPlane, const NxCapsule& capsule, const NxVec3& moveDir)
{
	outPlane.set(capsule.p0, moveDir);
	NxVec3 pt = capsule.p0 + moveDir;

	float d = outPlane.distance(capsule.p1);
	if (0 < d)
		pt = capsule.p1 + moveDir;
	outPlane.set(pt, moveDir);
	return pt;
}

float GeneralAttackMgr::GetMinimumDist(const NxPlane& plane, NxShape& shape)
{
	float dRtn = FLT_MAX;
	switch(shape.getType())
	{
	case NX_SHAPE_BOX:
		{
			NxBox obb;
			shape.isBox()->getWorldOBB(obb);
			static NxVec3 pts[8];
			if (NxComputeBoxPoints(obb, pts))
			{
				float d = 0.0f;
				for (int i = 0; i < 8; ++i)
				{
					d = plane.distance(pts[i]);
					if (dRtn > d)
						dRtn = d;
				}
			}
		}
		break;
	case NX_SHAPE_SPHERE:
		{
			float r = shape.isSphere()->getRadius();
			NxVec3 pos = shape.isSphere()->getGlobalPosition();
			float d = plane.distance(pos);
			dRtn = d - r;
		}
		break;
	case NX_SHAPE_CAPSULE:
		{
			NxCapsule capsule;
			shape.isCapsule()->getWorldCapsule(capsule);
			float d0 = plane.distance(capsule.p0);
			float d1 = plane.distance(capsule.p1);
			if (d0 < d1)
				dRtn = d0 - capsule.radius;
			else
				dRtn = d1 - capsule.radius;
		}
		break;
	default:
		assert("Shouldn't be here! Other type of shape is not supported!");
		break;
	}
	return dRtn;
}

bool GeneralAttackMgr::GetMoveBackDistance(const NxCapsule& capsule, const NxVec3& moveDir, const APtrArray<NxActor*>& arrActor, float& outMinDist)
{
	outMinDist = 0.0f;
	int nActor = arrActor.GetSize();
	if (0 == nActor)
		return false;

	NxPlane plane;
	NxVec3 frontPos = GetFrontalPlane(plane, capsule, moveDir);

	static const float fMinDistLimit = -3.0f;
	NxActor* pActor = 0;
	NxU32 nbShape = 0;
	NxShape* const* ppShapes = 0;
	for (int i = 0; i < nActor; ++i)
	{
		pActor = arrActor[i];
		nbShape = pActor->getNbShapes();
		ppShapes = pActor->getShapes();
		for (NxU32 j = 0; j < nbShape; ++j)
		{
			float d = GetMinimumDist(plane, *(ppShapes[j]));
			if (fMinDistLimit < d)
			{
				if (outMinDist > d)
					outMinDist = d;
			}
		}
	}

	if (0 > outMinDist)
		return true;
	return false;
}

void GeneralAttackMgr::AddSweepDebugData(const NxCapsule& capsule, const NxVec3& motion, const std::vector<SweepReport::sweepData>& hitInfo)
{
	NxVec3 up = capsule.p0 - capsule.p1;
	up.normalize();
	NxVec3 zdir(0, 0, 1);
	NxVec3 xdir(1, 0, 0);
	float angle = xdir.dot(up);
	if ((0.6f > angle) && (-0.6 < angle))
		zdir = xdir.cross(up);
	else
		zdir = zdir.cross(up);
	xdir = up.cross(zdir);

	NxMat34 pose;
	pose.M.setColumn(0, xdir);
	pose.M.setColumn(1, up);
	pose.M.setColumn(2, zdir);
	NxVec3 center = (capsule.p0 + capsule.p1) * 0.5f;
	pose.t = center;
	AddCapPose(pose, false, true, 0xffaa8856);
	pose.t += motion;
	AddCapPose(pose, false, true, 0xff991033);
	assert(0 < m_nFrameCount);
	m_FrameInfo[m_nFrameCount - 1].nCapEnd += 2;

	AddSpherePose(center, 0.05f, 0xffaa8856);
	AddLinePose(center, pose.t, 0xff5588bb);

	NxVec3 pt;
	size_t nCount = hitInfo.size();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		AddSpherePose(hitInfo[i].result.point, 0.02f, 0xffbbbb00);
		pt = hitInfo[i].result.point - motion;
		AddLinePose(hitInfo[i].result.point, pt, 0xffbbbb00);
	}
}

void GeneralAttackMgr::BuildDebugPoseData()
{
	if (m_nCurrentFrame >= m_nFrameCount)
		return;

	NxU32 nStart = m_FrameInfo[m_nCurrentFrame].nCapStart;
	NxU32 nEnd = m_FrameInfo[m_nCurrentFrame].nCapEnd;
	A3DCAPSULE cap;
	for (NxU32 i = nStart; i < nEnd; ++i)
	{
		cap.vCenter = APhysXConverter::N2A_Vector3(m_arrDebugPose[i].pose.t);
		cap.vAxisX = APhysXConverter::N2A_Vector3(m_arrDebugPose[i].pose.M.getColumn(0));
		cap.vAxisY = APhysXConverter::N2A_Vector3(m_arrDebugPose[i].pose.M.getColumn(1));
		cap.vAxisZ = APhysXConverter::N2A_Vector3(m_arrDebugPose[i].pose.M.getColumn(2));
		cap.fRadius = m_FrameInfo[m_nCurrentFrame].fCapRadius;
		cap.fHalfLen = m_FrameInfo[m_nCurrentFrame].fCapHalfLen;
		m_DebugRender.AddCapsule(cap, m_arrDebugPose[i].color);
		if (nStart < i)
			m_DebugRender.AddLine(APhysXConverter::N2A_Vector3(m_arrDebugPose[i - 1].pose.t), APhysXConverter::N2A_Vector3(m_arrDebugPose[i].pose.t), 0xff8855bb);
	}

	nStart = m_FrameInfo[m_nCurrentFrame].nSphereStart;
	nEnd = m_FrameInfo[m_nCurrentFrame].nSphereEnd;
	for (NxU32 i = nStart; i < nEnd; ++i)
		m_DebugRender.AddSphere(APhysXConverter::N2A_Vector3(m_arrDebugSphere[i].pos), m_arrDebugSphere[i].radius, m_arrDebugSphere[i].color);

	nStart = m_FrameInfo[m_nCurrentFrame].nLineStart;
	nEnd = m_FrameInfo[m_nCurrentFrame].nLineEnd;
	for (NxU32 i = nStart; i < nEnd; ++i)
		m_DebugRender.AddLine(APhysXConverter::N2A_Vector3(m_arrDebugLine[i].p0), APhysXConverter::N2A_Vector3(m_arrDebugLine[i].p1), m_arrDebugLine[i].color);
}

void GeneralAttackMgr::AddLinePose(const NxVec3& p0, const NxVec3& p1, int color)
{
	size_t arrSize = m_arrDebugLine.size();
	if (m_nDebugLineCount < arrSize)
	{
		m_arrDebugLine[m_nDebugLineCount].p0 = p0;
		m_arrDebugLine[m_nDebugLineCount].p1 = p1;
		m_arrDebugLine[m_nDebugLineCount].color = color;
		m_nDebugLineCount += 1;
	}
	else
	{
		m_arrDebugLine.push_back(LineColor(p0, p1, color));
		m_nDebugLineCount = arrSize + 1;
	}
}

void GeneralAttackMgr::AddSpherePose(const NxVec3& pos, float fR, int color)
{
	size_t arrSize = m_arrDebugSphere.size();
	if (m_nDebugSphereCount < arrSize)
	{
		m_arrDebugSphere[m_nDebugSphereCount].pos = pos;
		m_arrDebugSphere[m_nDebugSphereCount].color = color;
		m_arrDebugSphere[m_nDebugSphereCount].radius = fR;
		m_nDebugSphereCount += 1;
	}
	else
	{
		m_arrDebugSphere.push_back(SphereColor(pos, color, fR));
		m_nDebugSphereCount = arrSize + 1;
	}
}

void GeneralAttackMgr::AddCapPose(const NxMat34& pose, bool bToDataArray, bool bToDebugArray, int color)
{
	if (bToDataArray)
	{
		size_t arrSize = m_arrPose.size();
		if (m_nPoseCount < arrSize)
		{
			m_arrPose[m_nPoseCount] = pose;
			m_nPoseCount += 1;
		}
		else
		{
			m_arrPose.push_back(pose);
			m_nPoseCount = arrSize + 1;
		}
	}

	if (bToDebugArray)
	{
		size_t arrSize = m_arrDebugPose.size();
		if (m_nDebugPoseCount < arrSize)
		{
			m_arrDebugPose[m_nDebugPoseCount].pose = pose;
			m_arrDebugPose[m_nDebugPoseCount].color = color;
			m_nDebugPoseCount += 1;
		}
		else
		{
			m_arrDebugPose.push_back(PoseColor(pose, color));
			m_nDebugPoseCount = arrSize + 1;
		}
	}
}

void GeneralAttackMgr::InterpolationPose(const NxMat34& oldPose, const NxMat34& newPose, float dtTickSec, float radius, float halfLen)
{
	const bool bHasDebugData = ApxAttackEvent::IsDebugRenderEnable();
	FrameDebugInfo fdi;
	if (bHasDebugData)
	{
		fdi.fCapRadius = radius;
		fdi.fCapHalfLen = halfLen;
		fdi.nCapStart = m_nDebugPoseCount;
	}

	m_nPoseCount = 0;
	AddCapPose(oldPose, true, bHasDebugData, 0xff09bb66);
	bool bEnableSlerp = false;
	bool bSlerpBigStep = false;
	NxVec3 motionDist = newPose.t - oldPose.t;
	NxVec3 oldUp = oldPose.M.getColumn(1);
	NxVec3 newUp = newPose.M.getColumn(1);
	float fcos = oldUp.dot(newUp);
	if (0.5f > fcos)
	{
		bEnableSlerp = true;
		float fMD = motionDist.magnitude();
		if (0.3 < fMD)
			bSlerpBigStep = true;
	}
	if (bEnableSlerp)
	{
		float fSimStep = m_pScene->GetAPhysXScene()->GetSimulateTimeStep();
		if (bSlerpBigStep)
			fSimStep *= 2;
		NxQuat oldQuat(oldPose.M); 
		NxQuat newQuat(newPose.M); 
		NxQuat tempQuat;
		NxMat34 temp;
		float t = 0.0f;
		float fTime = fSimStep;
		while (fTime < dtTickSec)
		{
			if (NxMath::equals(fTime, dtTickSec, APHYSX_FLOAT_EPSILON))
				break;
			t = fTime / dtTickSec;

			temp.t = oldPose.t + motionDist * t;
			tempQuat.slerp(t, oldQuat, newQuat);
			temp.M.fromQuat(tempQuat);
			AddCapPose(temp, true, bHasDebugData, 0xffa067aa);
			fTime += fSimStep;
		}
	}
	AddCapPose(newPose, true, bHasDebugData, 0xff999911);

	if (bHasDebugData)
	{
		fdi.nCapEnd = m_nDebugPoseCount;
		size_t arrSize = m_FrameInfo.size();
		if (m_nFrameCount < arrSize)
		{
			m_FrameInfo[m_nFrameCount] = fdi;
			m_nFrameCount += 1;
		}
		else
		{
			m_FrameInfo.push_back(fdi);
			m_nFrameCount = arrSize + 1;
		}
		RenderNextFrameDebugData();
	}
}

void GeneralAttackMgr::CheckHitTargetByPhysX(ApxAttInfoHistory& aaih, float dtTickSec)
{
	APhysXScene* pScene = m_pScene->GetAPhysXScene();
	NxScene* pNxScene = pScene->GetNxScene();
	DetectFilterMgr dfMgr;
	if (!dfMgr.FilterBegin(*pScene))
		return;

	if (0 == aaih.m_AAAI.mApxDamage.mpAttacker)
		return;
	ApxActorBase* pAttacker = aaih.m_AAAI.mApxDamage.mpAttacker->GetActorBase();
	if (0 == pAttacker)
		return;

	NxMat34 oldPose, newPose;
	NxCapsule oldCapsule, newCapsule;
	if (!pAttacker->UpdateWeaponPose(&oldCapsule, &newCapsule, &oldPose, &newPose))
		return;

	dfMgr.FilterOutGroup(APX_COLLISION_GROUP_ATTACHER);
	dfMgr.FilterOutGroup(APX_COLLISION_GROUP_CLOTH_ATTACHER);
	CPhysXObjDynamic* pAttackObj = pAttacker->GetObjDyn();
	dfMgr.FilterOutDynObject(pAttackObj);
	if (0 != aaih.m_pHistory)
	{
		APtrArray<CPhysXObjDynamic*> hitObjs;
		int nCount = aaih.m_pHistory->GetHistoryTargetObjs(hitObjs);
		for (int i = 0; i < nCount; ++i)
			dfMgr.FilterOutDynObject(hitObjs[i]);
	}
	m_stReport.m_pCaster = pAttackObj;
	m_stReport.m_HitInfo.clear();

	float fHalfLen = oldCapsule.computeLength() * 0.5f;
	InterpolationPose(oldPose, newPose, dtTickSec, oldCapsule.radius, fHalfLen);

	const bool bHasDebugData = ApxAttackEvent::IsDebugRenderEnable();
	if (bHasDebugData)
	{
		m_FrameInfo[m_nFrameCount - 1].nSphereStart = m_nDebugSphereCount;
		m_FrameInfo[m_nFrameCount - 1].nLineStart = m_nDebugLineCount;
	}
	for (NxU32 i = 0; i < m_nPoseCount - 1; ++i)
	{
		NxVec3 motionDir = m_arrPose[i + 1].t - m_arrPose[i].t;
		float fMotionDist = motionDir.normalize();

		NxCapsule capsule;
		capsule.radius = oldCapsule.radius;
		NxVec3 half = m_arrPose[i].M.getColumn(1);
		half *= fHalfLen;
		capsule.p0 = m_arrPose[i].t + half;
		capsule.p1 = m_arrPose[i].t - half;

/*		NxVec3 half = m_arrPose[i + 1].M.getColumn(1);  // using end pose sweep
		half *= fHalfLen;
		capsule.p0 = m_arrPose[i + 1].t + half;
		capsule.p1 = m_arrPose[i + 1].t - half;
*/
		m_oltReport.m_pArray.RemoveAll(false);
		pNxScene->overlapCapsuleShapes(capsule, NX_DYNAMIC_SHAPES, 0, 0, &m_oltReport, dfMgr.GetActiveGroup(), dfMgr.GetGroupsMask(), true);
		float fDistMin = 0.0f;
		GetMoveBackDistance(capsule, motionDir, m_oltReport.m_pArray, fDistMin);
		float fMoveBack = -0.05f;
//		float fMoveBack = fMotionDist * -1 - 0.05f;
		if (fDistMin < fMoveBack)
			fMoveBack = fDistMin;

		NxVec3 deltaMoveBack = motionDir * fMoveBack;
		NxCapsule capStartSweep = capsule;
		capStartSweep.p0 += deltaMoveBack;
		capStartSweep.p1 += deltaMoveBack;

		NxVec3 deltaMove = motionDir * (fMotionDist - fMoveBack);
//		NxVec3 deltaMove = deltaMoveBack * -1;
		m_stReport.m_motionDir = motionDir;
		pNxScene->linearCapsuleSweep(capStartSweep, deltaMove, NX_SF_DYNAMICS | NX_SF_ALL_HITS, 0, 0, 0, &m_stReport, dfMgr.GetActiveGroup(), dfMgr.GetGroupsMask());
		if (bHasDebugData)
			AddSweepDebugData(capStartSweep, deltaMove, m_stReport.m_HitInfo);
	}
	if (bHasDebugData)
	{
		m_FrameInfo[m_nFrameCount - 1].nSphereEnd = m_nDebugSphereCount;
		m_FrameInfo[m_nFrameCount - 1].nLineEnd = m_nDebugLineCount;
	}

	size_t nCount = m_stReport.m_HitInfo.size();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		IPhysXObjBase* pHitObj =  reinterpret_cast<IPhysXObjBase*>(m_stReport.m_HitInfo[i].result.userData);
		if (0 != pHitObj)
		{
			aaih.m_AAAI.mApxAttack.mpTarget = pHitObj->GetAssailableInterface();
			aaih.m_AAAI.mApxDamage.mHitRBActor = &(m_stReport.m_HitInfo[i].result.hitShape->getActor());
			aaih.m_AAAI.mApxDamage.mHitPos = m_stReport.m_HitInfo[i].result.point;
			aaih.m_AAAI.mApxDamage.mHitDir = m_stReport.m_HitInfo[i].motionDir;
			aaih.m_AAAI.UpdateDamageSource();
			if (pAttacker->OnAttackingCheckHit(aaih.m_AAAI))
			{
				aaih.m_AAAI.mApxAttack.mpTarget->OnPreAttackedNotify(aaih.m_AAAI);
				AddCurrentDamage(aaih.m_AAAI);
			}
			if (0 == aaih.m_pHistory)
				aaih.m_pHistory = new ApxTargetHistory;
			if (0 != aaih.m_pHistory)
			{
				CPhysXObjDynamic* pTarget = dynamic_cast<CPhysXObjDynamic*>(pHitObj);
				if (0 != pTarget)
					aaih.m_pHistory->AddHitTarget(*pTarget);
			}
		}
	}
}

void GeneralAttackMgr::TickHitDamageForTimeDelay(float dtSec)
{
	attInfoQueue::iterator it = m_arrAttacksQueue.begin();
	attInfoQueue::iterator itEnd = m_arrAttacksQueue.end();
	ApxAttInfoHistory* pAAIH = 0;
	ApxAttackInfo* pAAI = 0;
	ApxActorBase* pAttacker =0;
	while (it != itEnd)
	{
		pAAIH = *it;
		if (0 == pAAIH)
		{
			it = m_arrAttacksQueue.erase(it);
		}
		else
		{
			pAAI = &(pAAIH->m_AAAI.mApxAttack);
			float scaleDTSec = dtSec * pAAIH->m_AAAI.mPlaySpeed;
			pAAI->mDamageDelayStartSec -= scaleDTSec;
			if (0 >= pAAI->mDamageDelayStartSec)
			{
				float fEndTime = pAAI->mDamageDelayStartSec + pAAI->mDamageDurationSec;
				if (0 >= fEndTime)
				{
					delete pAAIH;
					it = m_arrAttacksQueue.erase(it);
				}
				else
				{
					if (pAAI->mIsPhysXDetection)
					{
						CheckHitTargetByPhysX(*pAAIH, scaleDTSec);
						++it;
					}
					else
					{
						AttackCheckHit(pAAIH->m_AAAI);
						delete pAAIH;
						it = m_arrAttacksQueue.erase(it);
					}
				}
			}
			else
			{
				if (pAAI->mIsPhysXDetection)
				{
					if (0 != pAAIH->m_AAAI.mApxDamage.mpAttacker)
					{
						pAttacker = pAAIH->m_AAAI.mApxDamage.mpAttacker->GetActorBase();
						if (0 != pAttacker)
							pAttacker->UpdateWeaponPose();
					}
				}
				++it;
			}
		}
	}

	IAssailable* pAtteckedActor = 0;
	it = m_curDamageQueue.begin();
	itEnd = m_curDamageQueue.end();
	while (it != itEnd)
	{
		// Note: the attacker maybe be released during the delay-time...
		pAAIH = *it;
		pAtteckedActor = pAAIH->m_AAAI.mApxAttack.mpTarget;
		bool bRtn = pAtteckedActor->OnAttackedHitEvent(pAAIH->m_AAAI.mApxDamage);
		if (0 != pAAIH->m_AAAI.mApxDamage.mpAttacker)
			pAttacker = pAAIH->m_AAAI.mApxDamage.mpAttacker->GetActorBase();
		if (0 != pAttacker)
			pAttacker->OnAttackingHitEventPostNotify(pAAIH->m_AAAI.mApxDamage, pAtteckedActor, bRtn);
		if (!bRtn)
		{
			ToDelTarget delTarget(pAtteckedActor);
			RemoveAttackAction(delTarget, m_arrAttacksQueue, m_arrAttacksQueue.begin());
			attInfoQueue::iterator itNext = it;
			++itNext;
			RemoveAttackAction(delTarget, m_curDamageQueue, itNext);
		}
		delete pAAIH;
		m_curDamageQueue.pop_front();
		it = m_curDamageQueue.begin();
	}
}

void GeneralAttackMgr::AttackCheckHit(const ApxAttackActionInfo& apxAttActInfo)
{
	APtrArray<IAssailable*>	allAssailable;
	assert(0 != apxAttActInfo.mApxDamage.mpAttacker);
	ApxActorBase* pCaster = apxAttActInfo.mApxDamage.mpAttacker->GetActorBase();
	assert(0 != pCaster);

	IAssailable* pTargetActor = 0;
	if (0 > apxAttActInfo.mApxAttack.mAttHalfRadianRange)
	{
		// single target
		pTargetActor = apxAttActInfo.mApxAttack.mpTarget;
		if (0 != pTargetActor)
		{
			if (pTargetActor != pCaster)
				allAssailable.Add(pTargetActor);
		}
	}
	else
	{
		// multi-targets
		IPhysXObjBase* pCasterObj = pCaster->GetObjDyn();
		m_pScene->GetAllAssailablePhysXObj(pCasterObj, allAssailable);
	}

	ApxAttackActionInfo tmpAAAI = apxAttActInfo;
	tmpAAAI.mApxAttack.mpTarget = 0;

	IAssailable* pTarget = 0;
	int nAll = allAssailable.GetSize();
	for (int i = 0; i < nAll; ++i)
	{
		pTarget = allAssailable[i];
		tmpAAAI.mApxAttack.mpTarget = pTarget;
		if (pCaster->OnAttackingCheckHit(tmpAAAI))
		{
			tmpAAAI.mApxAttack.mpTarget->OnPreAttackedNotify(tmpAAAI);
			AddCurrentDamage(tmpAAAI);
		}
	}
}

void GeneralAttackMgr::BreakLastAttackAction(IAssailable* pAttacked)
{
	if (0 == pAttacked)
		return;

	ApxAttInfoHistory* pAAIH = 0;
	attInfoQueue::reverse_iterator it = m_arrAttacksQueue.rbegin();
	attInfoQueue::reverse_iterator itREnd = m_arrAttacksQueue.rend();
	for (; it != itREnd; ++it)
	{
		pAAIH = *it;
		if (0 != pAAIH)
		{
			if (pAAIH->m_AAAI.mApxDamage.mpAttacker == pAttacked)
			{
				delete (*it);
				*it = 0;
				break;
			}
		}
	}
}

void GeneralAttackMgr::RemoveAttackAction(Condition& cond, attInfoQueue& attQueue, attInfoQueue::iterator& itBegin, bool bIsFirstOnly)
{
	ApxAttInfoHistory* pAAIH = 0; 
	attInfoQueue::iterator it = itBegin;
	attInfoQueue::iterator itEnd = attQueue.end();
	while (it != itEnd)
	{
		pAAIH = *it;
		if (cond(&(pAAIH->m_AAAI)))
		{
			delete pAAIH;
			it = attQueue.erase(it);
			if (bIsFirstOnly)
				return;
		}
		else
		{
			++it;
		}
	}
}

bool GeneralAttackMgr::DirectRayAttack(const int x, const int y)
{
	if (0 == m_pScene)
		return false;

	PhysRay ray;
	m_pScene->ScreenPointToRay(x, y, ray);

	POINT pt;
	pt.x = x;
	pt.y = y;
	PhysRayTraceHit rayhit;
	APhysXRayHitInfo nxHitInfo;
	if (!m_pScene->RayTraceObject(pt, ray, rayhit, true, APX_RFTYPE_ALL, &nxHitInfo))
		return false;

	IObjBase* pHitObj = reinterpret_cast<IObjBase*>(rayhit.UserData);
	if (!ObjManager::IsPhysXObjType(pHitObj->GetProperties()->GetObjType()))
		return false;

	IPhysXObjBase* pPhysXHitObj = dynamic_cast<IPhysXObjBase*>(pHitObj);
	ApxAttackActionInfo aaai;
	aaai.mApxAttack.mIsPhysXDetection = true;
	aaai.mApxAttack.mAttackType = 3;
	aaai.mApxAttack.mpTarget = pPhysXHitObj->GetAssailableInterface();
	aaai.mApxDamage.mpAttacker = 0;
	aaai.mApxDamage.mHitRBActor = 0;
	aaai.mApxDamage.mHitPos = nxHitInfo.mHitPos;
	NxVec3 vDir;
	vDir.x = ray.vDelta.x;
	vDir.y = 0;
	vDir.z = ray.vDelta.z;
	vDir.normalize();
	vDir.y = a_Random(0.1f, 0.5f);
	vDir.normalize();
	aaai.mApxDamage.mHitDir = vDir;
	aaai.UpdateDamageSource();

	ApxActorBase* pHitActorBase = 0;
	IPhysXObjBase* pTarget = ObjManager::GetInstance()->GetPhysXObject(*(nxHitInfo.mActor));
	CMainActor* pMA = 0;
	int nAll = m_pScene->GetMainActorCount();
	for (int i = 0; i < nAll; ++i)
	{
		pMA = m_pScene->GetMainActor(i);
		if (pMA->GetDynObj() == pTarget)
		{
			pHitActorBase = pMA->GetActorBase();
			break;
		}
	}
	if (0 == pHitActorBase)
	{
		IRoleActor* pRA = 0;
		nAll = m_pScene->GetNPCActorCount();
		for (int i = 0; i < nAll; ++i)
		{
			pRA = m_pScene->GetNPCActor(i);
			if (pRA->GetDynObj() == pTarget)
			{
				pHitActorBase = pRA->GetActorBase();
				break;
			}
		}
	}

	if (0 != pHitActorBase)
	{
		pHitActorBase->OnPreAttackedNotify(aaai, 0);
		aaai.mApxDamage.mHitDamage = 100;
	}
	else
	{
		if (ObjManager::OBJ_TYPEID_ECMODEL == pTarget->GetProperties()->GetObjType())
		{
			CPhysXObjDynamic* pDynObj = dynamic_cast<CPhysXObjDynamic*>(pTarget);
			if (0 != pDynObj)
				pDynObj->OnPreAttackedNotify(aaai, 0);
		}
	}
	AddCurrentDamage(aaai);
	return true;
}
