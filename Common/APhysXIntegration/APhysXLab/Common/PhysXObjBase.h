/*
 * FILE: PhysXObjBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJBASE_H_
#define _APHYSXLAB_PHYSXOBJBASE_H_

#include <A3DVector.h>
#include <vector.h>
#include <AString.h>
#define NOMINMAX   // required for PhysX SDK. see PhysX doc "SDK Initialization"
#include <windows.h>

#include "RayTrace.h"
#include "CoordinateDirection.h"

class CRender;
class NxStream;
class APhysXScene;
class A3DViewport;
class NxMat34;
class A3DWireCollector; 
class BackgroundTask;
class A3DAABB;
class A3DOBB;
class NxActor;
class NxBounds3;
class APhysXRigidBodyObject;
class CPhysXObjForceField;

struct ApxHitForceInfo
{
	A3DVECTOR3		mHitPos;				// hit pos must be the result of PhysX Raycasting
	A3DVECTOR3		mHitDir;
	float			mHitForceMagnitude;
};

class IPhysXObjBase
{
public:	
	typedef CCoordinateDirection::EditType EditType;

	enum DrivenMode
	{
		DRIVEN_BY_PURE_PHYSX = 0,
		DRIVEN_BY_ANIMATION  = 1,		
		DRIVEN_BY_PART_PHYSX = 2,		
	};

	enum PeopleInfo
	{
		OBPI_IS_NONE_PEOPLE		= (1<<0),
		OBPI_IS_MAIN_ACTOR		= (1<<1),
		OBPI_IS_NPC				= (1<<2),
		OBPI_HAS_APHYSX_CC		= (1<<3),
		OBPI_ACTION_DANCE		= (1<<4),
		OBPI_ACTION_WALK		= (1<<5),
		OBPI_ACTION_WALK_AROUND	= (1<<6),
		OBPI_ACTION_WALK_XAXIS	= (1<<7),
		OBPI_HAS_APHYSX_LWCC	= (1<<8),
		OBPI_HAS_APHYSX_DYNCC	= (1<<9),
		OBPI_HAS_BRUSH_CC       = (1<<10),
		OBPI_HAS_APHYSX_NXCC    = (1<<11),
	};

	enum FlagsInfo
	{
		OBF_RUNTIME_ONLY		= (1<<0),
		OBF_DRAW_BINDING_BOX	= (1<<1),
		OBF_DRAW_MODEL_AABB		= (1<<2),
		OBF_DRAW_REF_FRAME		= (1<<3),
		OBF_FORCE_LIVING		= (1<<4),
		OBF_CLOSE_DEFAULT_DM	= (1<<5),
		OBF_ASSOCIATE_ALL_ISLOCKED	= (1<<6),
		OBF_ASSOCIATE_ROT_ISLOCKED	= (1<<7),

		// following flags support serialization
		OBF_TO_KINEMATIC		= (1<<26),
		OBF_DRIVENMODE_ISLOCKED = (1<<27),
		OBF_ENABLE_PHYSX_BODY   = (1<<28),
 	};

	IPhysXObjBase(const int type);
	virtual ~IPhysXObjBase() {};

protected:
	enum ObjState
	{
		OBJECT_STATE_NULL   = 0,
		OBJECT_STATE_ACTIVE = 1,		
		OBJECT_STATE_SLEEP  = 2,		
	};
	
	enum FlagsInternal
	{
		OBFI_NEW_PEOPLE_INFO	= (1<<16),
		OBFI_HAS_ASYN_WORK		= (1<<17),  // flag of asynchronous operation unfinished
		OBFI_GOTO_SLEEP			= (1<<18),
		OBFI_GOTO_DESTROY		= (1<<19),
		OBFI_IS_IN_RUNTIME		= (1<<20),
		OBFI_IS_IN_LOADING		= (1<<21),
		OBFI_HAS_BACKUP_POSE	= (1<<22),
		OBFI_ON_HIT_SKILL		= (1<<23),

		// following flags support serialization
		ALIAS_SAVE_SEPARATOR	= (1<<24),  // ALIAS_SAVE_SEPARATOR don't occupy a bit
		OBFI_INITED_AABB		= ALIAS_SAVE_SEPARATOR,
		OBFI_INITED_PMFLAGS		= (1<<25),
		OBFI_TO_KINEMATIC		= OBF_TO_KINEMATIC,
		OBFI_DRIVENMODE_ISLOCKED= OBF_DRIVENMODE_ISLOCKED,
 	};

public:	
	int GetObjType() const { return m_nTypeID; }
	AString GetFilePathName() const { return m_strFilePath; }
	void CopyBasicProperty(const IPhysXObjBase& obj);

	bool ReadFlag(const FlagsInfo flag) const { return (m_flags & flag)? true : false; }
	void ClearFlag(const FlagsInfo flag) { m_flags &= ~flag; }
	void RaiseFlag(const FlagsInfo flag) { m_flags |= flag; }

	bool ReadPeopleInfo(const PeopleInfo flag) const { return (m_PeopleFlags & flag)? true : false; }
	int  GetPeopleInfo() const { return m_PeopleFlags; }
	void ClearPeopleInfo(const PeopleInfo flag);
	void RaisePeopleInfo(const PeopleInfo flag);

	bool LoadModel(CRender& render, const char* szFile, APhysXScene* pPhysXScene = 0, const bool OpenEditInfo = true); 
	void Release(const bool IsAsync = false);
	
	bool WakeUp(CRender& render, APhysXScene* pPhysXScene = 0); 
	void Sleep(const bool IsAsync = false);
	
	bool EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();

	bool InstancePhysXObj(APhysXScene& aPhysXScene);
	void ReleasePhysXObj(const bool RunTimeIsEnd = true);
	void ControlClothes(const bool IsClose = true);

	void UpdateStateSync(const A3DVECTOR3& vCenter, const float fThreshold, CRender& render, APhysXScene& aPhysXScene); 
	void UpdateStateAsync(const A3DVECTOR3& vCenter, const float fThreshold, BackgroundTask& bkTask); 

	bool TickMove(float deltaTimeSec);
	bool TickAnimation(const unsigned long deltaTime);
	void Render(A3DViewport& viewport, A3DWireCollector* pWC = 0, bool isDebug = false) const;
	void RenderShadow(A3DViewport& viewport) const
	{
		if (!IsActive()) return;
		OnRenderShadow(viewport);
	}

	int  GetControlGroup() const { return m_CtrlGroup; }
	void SetControlGroup(const int cg) { m_CtrlGroup = cg; }

	float GetMotionDistance() const { return m_MotionDist; }
	void SetMotionDistance(const float md) { m_MotionDist = md; }

	void ResetPose();
	A3DVECTOR3 GetNonRuntimePos() const;
	A3DVECTOR3 GetPos() const;
	void	   SetPos(const A3DVECTOR3& vPos);

	/* not support runtime pose. there is no proper interfaces from low level. */
	void GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const { vDir = m_vDir; vUp = m_vUp; } 
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

	float GetScale() const { return m_fScale; }
	void  SetScale(const float& fScale);

	void SetRefFrameEditMode(const EditType et);
	void UpdatePoseFromRefFrame();

	DrivenMode GetDrivenMode() const { return m_eDrivenMode; }
	void SetDrivenMode(const DrivenMode dm);
	DrivenMode GetDefaultDMOnSim() const { return m_eDMDefOnSim; }
	void SetDefaultDMOnSim(const DrivenMode dm) { m_eDMDefOnSim = dm; }

	void SetAssociateObj(IPhysXObjBase* pAscObj, const NxMat34* pLocalPose = 0, bool IsRotLocked = false);
	IPhysXObjBase* GetAssociateObj(NxMat34* pOutLocalPose = 0);
	CPhysXObjForceField* GetAssociateFF(NxMat34* pOutLocalPose = 0);

	virtual bool LoadDeserialize(CRender& render, NxStream& stream, const unsigned int nVersion, APhysXScene& aPhysXScene);
	virtual bool SaveSerialize(NxStream& stream) const;

	virtual bool SyncDataPhysXToGraphic() = 0;
	virtual int  GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor = 0) const = 0;
	virtual NxActor* GetFirstNxActor() const = 0;
	virtual bool GetPhysXRigidBodyAABB(NxBounds3& outAABB) const = 0;

	virtual bool OnHitbyForce(const ApxHitForceInfo& hitForceInfo, NxActor* pHitActor = 0) { return true; }
	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;

	//-----------------------------------------------------------
	// for some runtime control of object's pos and orientation
	virtual void  UpdatePos(const A3DVECTOR3& vPos)
	{
		SetPos(vPos);
	}
	virtual void UpdateDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
	{
		SetDirAndUp(vDir, vUp);
	}


private:
	void SetToEmpty();
	void DoSleepTask();
	void DoDestroyTask();
	void ApplyPose();
	bool DoLoadModelInternal(CRender& render, const char* szFile, APhysXScene* pPhysXScene, const bool OpenEditInfo);

	void GetNxActorAABB(NxActor& actor, NxBounds3& outAABB) const;
	bool OnInitPhysXAABB(A3DAABB& outAABB) const;
	int GetPreservableFlags() const;

private:
	virtual void OnSetToEmpty() {};

	virtual void OnCopyBasicProperty(const IPhysXObjBase& obj) {}
	virtual bool OnLoadDeserialize(NxStream& stream, const unsigned int nVersion) { return true; }
	virtual void OnSaveSerialize(NxStream& stream) const {}

	virtual bool OnLoadModel(CRender& render, const char* szFile) = 0; 
	virtual void OnReleaseModel() = 0;

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene) = 0;
	virtual void OnInstancePhysXObjPost(APhysXScene& aPhysXScene) { RaiseFlag(OBFI_INITED_PMFLAGS); };
	virtual void OnReleasePhysXObj(const bool RunTimeIsEnd) = 0;
	virtual void OnControlClothes(const bool IsClose) {}

	virtual bool OnTickMove(float deltaTimeSec) = 0;
	virtual bool OnTickAnimation(const unsigned long deltaTime) = 0;
	virtual void OnRender(A3DViewport& viewport, A3DWireCollector* pWC = 0, bool isDebug = false) const = 0;
	virtual void OnRenderShadow(A3DViewport& viewport) const { /* do nothing at default... */ }

	virtual bool OnResetPose() = 0;
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const { /* to get runtime position */ return false; }
	virtual void OnSetPos(const A3DVECTOR3& vPos) = 0;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) = 0;
	virtual bool OnSetScale(const float& scale) = 0;

	virtual void OnSetRefFrameEditMode(const EditType et) {}
	virtual void OnChangeDrivenMode(const DrivenMode dmNew) {}
	virtual void OnSleep() {}
	virtual bool OnWakeUp(CRender& render, APhysXScene* pPhysXScene = 0) { return false; }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const = 0;
	virtual bool OnGetSelectedAABB(A3DAABB& outAABB) const;

protected:
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) = 0;
	virtual void OnLeaveRuntime() = 0;
	
	bool ReadFlag(const FlagsInternal flag) const { return (m_flags & flag)? true : false; }
	void ClearFlag(const FlagsInternal flag) { m_flags &= ~flag; }
	void RaiseFlag(const FlagsInternal flag) { m_flags |= flag; }

	
	void GetGlobalPose(NxMat34& mat34) const;
	bool GetAABB(A3DAABB& outAABB) const;
	bool GetModelAABB(A3DAABB& outAABB) const;
	bool GetOBB(A3DOBB& outOBB) const; /* only support Non-runtime OBB */
	void GetPhysXRigidBodyAABB(APhysXRigidBodyObject& RB, NxBounds3& outAABB) const;

private:
	// forbidden behavior
	IPhysXObjBase(const IPhysXObjBase& rhs);
	IPhysXObjBase& operator= (const IPhysXObjBase& rhs);

public:
	static A3DWireCollector* pWCTopZBuf;
	
private:
	int	m_nTypeID;
	int m_flags;
	int m_PeopleFlags;
	int m_CtrlGroup;
	float m_MotionDist;

	DrivenMode m_eDrivenMode;
	DrivenMode m_eDMDefOnSim;
	A3DVECTOR3 m_AABBRelativeCenter;
	A3DVECTOR3 m_OriginalExtents; // PhysX AABB extents

	// Must be a relative path.
	AString m_strFilePath;

	int m_BackUpFlags;
	int m_BackUpPeopleFlags;
	A3DVECTOR3 m_vBackUpPos;
	A3DVECTOR3 m_vBackUpDir;
	A3DVECTOR3 m_vBackUpUp;

	A3DMATRIX4 m_ascLocalPose;
	IPhysXObjBase* m_pAssociate;

protected:
	ObjState m_ObjState;
	

	A3DVECTOR3 m_vPos;
	A3DVECTOR3 m_vDir;
	A3DVECTOR3 m_vUp;

public:
	CCoordinateDirection m_refFrame;
	bool IsActive() const;
	float m_fScale;
};

#endif
