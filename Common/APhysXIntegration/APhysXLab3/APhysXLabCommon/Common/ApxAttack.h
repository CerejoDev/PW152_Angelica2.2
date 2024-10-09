/************************************************************************

 ApxAttack.h

	this file implement some stuff of PhysX attacking effect

	Created by:		He Wenfeng 
	Date:			Jan. 7, 2010
	update by		Yang Liu  Oct. 13, 2011

************************************************************************/

#pragma once
#ifndef _APHYSXLAB_APXATTACK_H_
#define _APHYSXLAB_APXATTACK_H_

#include <list>
#include "ApxActorBase.h"
#include "A3DSkillGfxEvent.h"
#ifdef _ANGELICA3
	#include "A3DSkillGfxEventMan.h"
#else
	typedef _SGC::A3DSkillGfxEvent	A3DSkillGfxEvent;
	typedef _SGC::A3DSkillGfxMan	A3DSkillGfxMan;
	typedef _SGC::GfxMoveMode		GfxMoveMode;
	typedef _SGC::GETPOSITIONBYID_PARAMS		GETPOSITIONBYID_PARAMS;
	class A3DSkillGfxEventMan : public A3DSkillGfxMan
	{
//		friend class ApxAttackEvent;

	public:
		A3DSkillGfxEventMan() 
		{
//			m_fForceMag = 0.0f;
			m_vHitPos.Clear();
			m_pHitTarget = NULL;
		}

//		void SetHitForceMag(float fForceMag){ m_fForceMag = fForceMag;}
//		float GetHitForceMag() { return m_fForceMag; }

		// Note: call this function before PlayAttackAction...
		void SetHitInfo(const A3DVECTOR3& vHitPos, IPhysXObjBase* pHitObj)
		{
			m_vHitPos = vHitPos;
			m_pHitTarget = pHitObj;
		}

//		const A3DVECTOR3& GetHitPos() const
//		{
//			return m_vHitPos;
//		}

	protected:
		virtual A3DSkillGfxEvent* CreateOneEvent(GfxMoveMode mode);

		virtual bool GetPositionById(clientid_t nId, A3DVECTOR3& vPos, _SGC::GfxHitPos HitPos, const char* szHook = NULL, bool bRelHook = false, const A3DVECTOR3* pOffset = NULL, const char* szHanger = NULL, bool bChildHook = false);
		virtual bool GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp);
		virtual float GetTargetScale(clientid_t nTargetId);

//		float m_fForceMag;
		A3DVECTOR3 m_vHitPos;
		IPhysXObjBase* m_pHitTarget;
	};

#endif

class SweepReport : public NxUserEntityReport<NxSweepQueryHit>
{
public:
	struct sweepData 
	{
		NxVec3 motionDir;
		NxSweepQueryHit result;
	};

public:
	SweepReport() : m_pCaster(0) {}
	virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities);

	NxVec3 m_motionDir;
	IPhysXObjBase* m_pCaster; 
	std::vector<sweepData> m_HitInfo;

private:
	void AddToArray(const NxSweepQueryHit& sqh);
};

class RaycastReport : public NxUserRaycastReport
{
public:
	RaycastReport() : m_bIsHitTarget(false), m_pTargetActor(0) {}
	virtual bool onHit(const NxRaycastHit& hit);

	bool m_bIsHitTarget;
	NxActor* m_pTargetActor;
};

class ApxAttackEvent: public A3DSkillGfxEvent
{
public:
	static IRenderManager* GetRenderManager();
	static void SetRenderManager(IRenderManager* pRM);
	static void EnableDebugRender(bool bEnable);
	static bool IsDebugRenderEnable();

#ifdef _ANGELICA3
	ApxAttackEvent(A3DSkillGfxEventMan* pMan, GfxMoveMode mode) : A3DSkillGfxEvent(pMan, mode) { m_bEnable = true; m_bIsGetHitInfo = false; m_bIsGetFixPoint = false; }
#else
	ApxAttackEvent(A3DSkillGfxMan* pMan, GfxMoveMode mode) : A3DSkillGfxEvent(pMan, mode) { m_bEnable = true; m_bIsGetHitInfo = false; m_bIsGetFixPoint = false; }
#endif
	~ApxAttackEvent() { ClearPhysXTargetData(); }
	virtual void Tick(DWORD dwDeltaTime);

	bool IsEnable() const { return m_bEnable; }
	bool IsGetHitInfo() const { return m_bIsGetHitInfo; }
	bool IsGetFixPoint() const { return m_bIsGetFixPoint; }
	void SetAttDamInfo(const ApxAttackActionInfo& aaai);
	IAssailable* GetTargetIA(bool* poutIsFromPhysX = 0) const;
	const NxVec3& GetLocalHitPos(const bool bIsFromPhysX) const;
	const ApxAttackActionInfo& GetAttDamInfo() const
	{
		if (0 < m_pFromPhysX.GetSize())
			return m_pFromPhysX[0]->m_aaai;
		return m_TDFromUser.m_aaai;
	}

protected:
	virtual void HitTarget();

private:
	void ClearPhysXTargetData();
	bool DoPhysXSweep(const NxVec3& start, const NxVec3& motion);
	void HandleSweepResult(const NxVec3& start, const NxVec3& dir, float dist);
	float ComputeHitForceRatio(const NxVec3& start, const NxVec3& dir, float dist, const NxVec3& hitPos, NxActor* pHitActor);
	bool CheckHitTargetBySweep();
	bool CheckHitTargetBySweep(const A3DVECTOR3& deltaMove, const A3DVECTOR3& instantPos);
	void UpdateLoacalHitPos(const NxVec3& hitPos, const NxVec3& hitDir);

private:
	struct TargetData 
	{
		TargetData();
		NxVec3 m_hitLocalPos;
		ApxAttackActionInfo m_aaai;
		void UpdateLoacalHitPos(const NxVec3& hitDir);
	};

private:
	static float gCapsuleRadius;
	static bool	 gEnableDebugRender;
	static IRenderManager* gpRenderManager;

	bool m_bEnable;
	bool m_bIsGetHitInfo;
	bool m_bIsGetFixPoint;
//	ApxAttackActionInfo m_aaai;
	TargetData m_TDFromUser;
//	TargetData m_TDFromPhysX;
	APtrArray<TargetData*> m_pFromPhysX;

	SweepReport m_stReport;
	RaycastReport m_rcReport;
	ExtraRenderable m_DebugRender;
};

class Scene;
class ApxAttackSkillGfxMan : public A3DSkillGfxEventMan
{
	friend class ApxAttackEvent;

public:
	ApxAttackSkillGfxMan(Scene* pScene) 
	{
		m_bIsClearQueue = false;
		m_pScene = pScene;
		EmptyAttackInfo();
	}

	Scene* GetScene() const { return m_pScene; }
	void SetAttDamInfo(const ApxAttackActionInfo& aaai)
	{
		m_IsNewAAI = true;
		m_aaai = aaai;
	}
	void ClearAttackQueue(bool bClear = true) // to update: by Gfxcommon callback function
	{
		m_bIsClearQueue = bClear;
	}
	bool IsClearAttackQueue() const { return m_bIsClearQueue; }

protected:
	virtual A3DSkillGfxEvent* CreateOneEvent(GfxMoveMode mode);
	virtual bool GetPositionById(const GETPOSITIONBYID_PARAMS& param, A3DVECTOR3& vPos);
	virtual bool GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp);
	virtual float GetTargetScale(clientid_t nTargetId);
	virtual void PushEvent(clientid_t nCasterId, A3DSkillGfxEvent* pEvent);

private:
	void EmptyAttackInfo() { m_IsNewAAI = false; }

private:
	bool m_bIsClearQueue;
	Scene* m_pScene;
	bool m_IsNewAAI;
	ApxAttackActionInfo m_aaai;
};

struct HitObjStat
{
	HitObjStat() { pHitObj = 0; nHitCount = 0; }
	HitObjStat(CPhysXObjDynamic& hitObj) { pHitObj = &hitObj; nHitCount = 1; }
	CPhysXObjDynamic* pHitObj;
	int nHitCount;
};

class ApxTargetHistory
{
public:
	void AddHitTarget(CPhysXObjDynamic& target);
	int GetHistoryTargetObjs(APtrArray<CPhysXObjDynamic*>& outObjs);

private:
	std::vector<HitObjStat> m_History;
};

class OLTestReport : public NxUserEntityReport<NxShape*>
{
public:
	virtual bool onEvent(NxU32 nbEntities, NxShape** entities);

	APtrArray<NxActor*> m_pArray;
};

class GeneralAttackMgr
{
public:
	GeneralAttackMgr();
	~GeneralAttackMgr();
	ApxAttackSkillGfxMan* GetAttackSkillGfxMan() const { return m_pAASGM; }

	bool InitAttackManager(Scene& scene, A3DGFXExMan& gfxExMan);
	void ReleaseAttackManager();
	void ClearAttackQueue(bool bClear = true);// to update: by Gfxcommon callback function

	bool Tick(float dtSec);
	void Render();
	void RenderNextFrameDebugData();
	void ClearFrameDebugData();

	bool DirectRayAttack(const int x, const int y);
	bool AddCurrentDamage(const ApxAttackActionInfo& apxAttActInfo);
	void OnPlayAttackBegin(const ApxAttackActionInfo& apxAttActInfo);
	void OnPlayAttackEnd(const ApxAttackActionInfo& apxAttActInfo);

private:
	struct ApxAttInfoHistory
	{
		ApxAttInfoHistory(const ApxAttackActionInfo& aaai) : m_AAAI(aaai), m_pHistory(0) {}
		~ApxAttInfoHistory() { ReleaseAll(); }
		void ReleaseAll()
		{
			delete m_pHistory;
			m_pHistory = 0;
		}
		ApxAttackActionInfo m_AAAI;
		ApxTargetHistory* m_pHistory;
	};
	typedef std::list<ApxAttInfoHistory*>		attInfoQueue;
	typedef std::vector<NxMat34>				arrayPose;
	typedef std::vector<NxVec3>					arrayVec;

	class Condition : public std::unary_function<ApxAttackActionInfo*, bool> 
	{
	public:
		virtual bool operator() (ApxAttackActionInfo* pAAAI) const = 0;
	};
	class ToDelTarget : public Condition 
	{
	public:
		ToDelTarget(IAssailable* pIA) : m_pIA(pIA) {}
		virtual bool operator() (ApxAttackActionInfo* pAAAI) const;
	private:
		IAssailable* m_pIA;
	};
	class ToDelCaster : public Condition 
	{
	public:
		ToDelCaster(IAssailable* pIA) : m_pIA(pIA) {}
		virtual bool operator() (ApxAttackActionInfo* pAAAI) const;
	private:
		IAssailable* m_pIA;
	};

	struct FrameDebugInfo 
	{
		FrameDebugInfo()
		{
			fCapRadius = fCapHalfLen = 0.0f;
			nCapStart = nCapEnd = 0;
			nSphereStart = nSphereEnd = 0;
			nLineStart = nLineEnd = 0;
		}

		float fCapRadius;
		float fCapHalfLen;
		NxU32 nCapStart;
		NxU32 nCapEnd;
		NxU32 nSphereStart;
		NxU32 nSphereEnd;
		NxU32 nLineStart;
		NxU32 nLineEnd;
	};
	struct PoseColor 
	{
		PoseColor(const NxMat34& p, int c) : pose(p), color(c) {} 
		NxMat34 pose;
		int color;
	};
	struct SphereColor
	{
		SphereColor(const NxVec3& p, int c, float r) : pos(p), color(c), radius(r) {} 
		NxVec3 pos;
		int color;
		float radius;
	};
	struct LineColor
	{
		LineColor(const NxVec3& ps, const NxVec3& pe, int c) : p0(ps), p1(pe), color(c) {} 
		NxVec3 p0;
		NxVec3 p1;
		int color;
	};

	typedef std::vector<FrameDebugInfo>		arrayFrameDebug;
	typedef std::vector<PoseColor>			arrayDebugPose;
	typedef std::vector<SphereColor>		arrayDebugSphere;
	typedef std::vector<LineColor>			arrayDebugLine;

private:
	void TickHitDamageForTimeDelay(float dtSec);

	NxVec3 GetFrontalPlane(NxPlane& outPlane, const NxCapsule& capsule, const NxVec3& moveDir);
	float GetMinimumDist(const NxPlane& plane, NxShape& shape);
	bool GetMoveBackDistance(const NxCapsule& capsule, const NxVec3& moveDir, const APtrArray<NxActor*>& arrActor, float& outMinDist);
	void AddSweepDebugData(const NxCapsule& capsule, const NxVec3& motion, const std::vector<SweepReport::sweepData>& hitInfo);
	void BuildDebugPoseData();

	void AddLinePose(const NxVec3& p0, const NxVec3& p1, int color = 0);
	void AddSpherePose(const NxVec3& pos, float fR, int color = 0);
	void AddCapPose(const NxMat34& pose, bool bToDataArray = true, bool bToDebugArray = false, int color = 0);
	void InterpolationPose(const NxMat34& oldPose, const NxMat34& newPose, float dtTickSec, float radius, float halfLen);
	void CheckHitTargetByPhysX(ApxAttInfoHistory& aaih, float dtTickSec);
	void AttackCheckHit(const ApxAttackActionInfo& apxAttActInfo);
	void BreakLastAttackAction(IAssailable* pAttacked);
	void RemoveAttackAction(Condition& cond, attInfoQueue& attQueue, attInfoQueue::iterator& itBegin, bool bIsFirstOnly = false);

private:
	Scene* m_pScene;
	ApxAttackSkillGfxMan* m_pAASGM;

	OLTestReport m_oltReport;
	SweepReport m_stReport;

	NxU32 m_nPoseCount;
	arrayPose m_arrPose;

	attInfoQueue m_arrAttacksQueue;
	attInfoQueue m_curDamageQueue;
	ExtraRenderable m_DebugRender;

	NxU32 m_nCurrentFrame;
	NxU32 m_nFrameCount;
	arrayFrameDebug m_FrameInfo;
	NxU32 m_nDebugPoseCount;
	arrayDebugPose m_arrDebugPose;
	NxU32 m_nDebugSphereCount;
	arrayDebugSphere m_arrDebugSphere;
	NxU32 m_nDebugLineCount;
	arrayDebugLine m_arrDebugLine;
};

#endif