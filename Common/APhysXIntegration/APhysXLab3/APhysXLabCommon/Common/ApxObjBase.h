
/*----------------------------------------------------------------------

	ApxObjBase.h:
		
		define the abstract interface of A hierachical Object in 3D space

	Created by He Wenfeng, May. 17, 2011

----------------------------------------------------------------------*/


#ifndef _APHYSXLAB_APXOBJBASE_H_
#define _APHYSXLAB_APXOBJBASE_H_

#include "RayTrace.h"

class ApxObjBase;

class ApxObjOperationReport
{
public:

	virtual void OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild){}
	virtual void OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild){}

};

class ApxObjBase : public APhysXFlagOperation
{

	//-------------------------------------------------------------------------------
	// constants and enumerations...

	// for lazy update, update until user get it.
	enum 
	{
		Dirty_Flag_mtRotLToP		=	1 << 0,
		Dirty_Flag_mtRotLToG		=	1 << 1,

		Dirty_Flag_AABB				=	1 << 2,

		Dirty_Flag_GPos				=	1 << 10,
		Dirty_Flag_GRot				=	1 << 11,
		Dirty_Flag_GScale			=	1 << 12,

		Dirty_Flag_LPos				=	1 << 13,
		Dirty_Flag_LRot				=	1 << 14,
		Dirty_Flag_LScale			=	1 << 15,
	};

public:

	enum
	{
		Object_Flag_UpdateTransform_Async			= 1 << 0,					// if unchecked, update global and children transform immediately; if checked, update them until Ticked
		Object_Flag_UpdateTransform_Passive			= 1 << 1,					// if unchecked, update entity's transform by my global pose; if checked, update my global pose by entity's pose

		Object_Flag_NoDeleteAsChild					= 1 << 10,					// if checked, I can not be released and deleted by my parent, i.e. camera object
	};

	//-------------------------------------------------------------------------------
	// static members...
public:
	static void SetOperationReport(ApxObjOperationReport* pOperationReport)
	{
		m_spOperationReport = pOperationReport;
	}

protected:

	static ApxObjOperationReport* m_spOperationReport;

	//-------------------------------------------------------------------------------
	// common members...
public:
	
	ApxObjBase()
	{
		m_pParent = NULL;
		Reset(false, false);
	}

	virtual ~ApxObjBase()
	{
	}

	//----------------------------------------------
	// reset and release functions...

	// reset my pose/scale and other data to default values
	void Reset(bool bResetParent = true, bool bResetChildren = true)
	{
		m_fLScale = m_fGScale = 1.0f;

		m_qRotLToP.id();
		m_qRotLToG.id();

		m_mtRotLToP.id();
		m_mtRotLToG.id();

		m_vOffsetLToP.zero();
		m_vOffsetLToG.zero();

		m_uDirtyFlags = 0;
		m_uFlags = 0;

		mUserData = NULL;

		if(bResetParent && m_pParent)
		{
			m_pParent->RemoveChild(this);
			m_pParent = NULL;
		}

		if(bResetChildren)
			RemoveChildren();
	}

	// release pointers referenced, here are my parents and children. 
	// call Release() before I'm deleted.
	void Release()
	{
		OnRelease();

		if(m_pParent)
		{
			m_pParent->m_arrChildren.deleteEntry(this);
			m_pParent = NULL;
		}

		ReleaseChildren();

	}

	// Note: GetCloned() will return a root object with m_pParent is NULL...
	ApxObjBase* GetCloned() const;

	//----------------------------------------------
	// tick function

	// Tick myself and my children. Called at each frame, dt (unit in second)
	// make sure my parent has been ticked before ticking me.
	void Tick(float dt);	

	//----------------------------------------------
	// id/name/flag functions...
	void SetType(APhysXU32 uType)
	{
		m_uType = uType;
	}
	APhysXU32 GetType() const
	{
		return m_uType;
	}

	void SetName(const char* szName)
	{
		m_strName = szName;
	}
	void SetName(const AString& strName)
	{
		m_strName = strName;
	}
	AString GetObjName() const
	{
		return m_strName;
	}

	void RaiseFlag(APhysXU32 flag)
	{
		APhysXFlagOperation::RaiseFlag(m_uFlags, flag);

		// handle some flags which are iterative to my children...
		if(APhysXFlagOperation::CheckFlag(flag, Object_Flag_UpdateTransform_Async))
		{
			RaiseChildrenFlag(Object_Flag_UpdateTransform_Async);
		}
	}

	void ClearFlag(APhysXU32 flag)
	{
		APhysXFlagOperation::ClearFlag(m_uFlags, flag);
		
		// handle some flags which are iterative to my children...
		if(APhysXFlagOperation::CheckFlag(flag, Object_Flag_UpdateTransform_Async))
		{
			ClearChildrenFlag(Object_Flag_UpdateTransform_Async);
		}
	}

	bool CheckFlag(APhysXU32 flag) const
	{
		return APhysXFlagOperation::CheckFlag(m_uFlags, flag);
	}

	//----------------------------------------------
	// tree functions...

	ApxObjBase* GetParent() const { return m_pParent; }
	int GetChildNum() const { return m_arrChildren.size();} 
	ApxObjBase* GetChild(int i) const { return m_arrChildren[i]; }

	bool HasAncestor(const ApxObjBase* pObj) const;
	bool HasOffspring(const ApxObjBase* pObj) const
	{
		return pObj->HasAncestor(this);
	}

	// the function do the following stuff step by step
	// 1. remove pObj from its old parent
	// 2. add pObj to my children array.
	// 3. notify AddChild event to ApxObjOperationReport
	bool AddChild(ApxObjBase* pObj);

	// Remove-XXX function do the following stuff step by step
	// 1. remove child from my children array
	// 2. set the child's parent to NULL.
	// 3. notify RemoveChild event to ApxObjOperationReport
	bool RemoveChild(ApxObjBase* pObj);
	void RemoveChildren();

	//----------------------------------------------
	// pose/scale functions...
	
	// Get functions...	
	// Beware:	in async update way, GetXXX function should be called after Tick(), 
	//			and in the callback function OnTick() is OK
	//
	float GetLScale() const { return m_fLScale; }
	const NxVec3& GetLPos() const { return m_vOffsetLToP; }
	const NxQuat& GetLRot() const { return m_qRotLToP; }
	void GetLRot(NxMat33& mtRot) const
	{
		if(CheckDirtyFlag(Dirty_Flag_mtRotLToP))
		{
			m_mtRotLToP.fromQuat(m_qRotLToP);
			ClearDirtyFlag(Dirty_Flag_mtRotLToP);
		}

		mtRot = m_mtRotLToP;
	}
	void GetLDirAndUp(NxVec3& vDir, NxVec3& vUp) const
	{
		NxMat33 mtRot;
		GetLRot(mtRot);
		vDir = mtRot.getColumn(2);
		vUp = mtRot.getColumn(1);
	}

	void GetLDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const
	{
		NxMat33 mtRot;
		GetLRot(mtRot);
		vDir = APhysXConverter::N2A_Vector3(mtRot.getColumn(2));
		vUp = APhysXConverter::N2A_Vector3(mtRot.getColumn(1));
	}
	void GetLPose(A3DMATRIX4& mtPose) const
	{
		NxMat33 mtRot;
		GetLRot(mtRot);
		A3DMATRIX3 amtRot;
		APhysXConverter::N2A_Matrix33(mtRot, amtRot);
		
		Mat44_SetRotatePart(amtRot, mtPose);
		Mat44_SetTransPart(APhysXConverter::N2A_Vector3(GetLPos()), mtPose);
	}

	float GetGScale() const { return m_fGScale; }
	const NxVec3& GetGPos() const { return m_vOffsetLToG; }
	const NxQuat& GetGRot() const { return m_qRotLToG; }
	void GetGRot(NxMat33& mtRot) const
	{
		if(CheckDirtyFlag(Dirty_Flag_mtRotLToG))
		{
			m_mtRotLToG.fromQuat(m_qRotLToG);
			ClearDirtyFlag(Dirty_Flag_mtRotLToG);
		}

		mtRot = m_mtRotLToG;
	}

	void GetGDirAndUp(NxVec3& vDir, NxVec3& vUp) const
	{
		NxMat33 mtRot;
		GetGRot(mtRot);
		vDir = mtRot.getColumn(2);
		vUp = mtRot.getColumn(1);
	}

	void GetGDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const
	{
		NxMat33 mtRot;
		GetGRot(mtRot);
		vDir = APhysXConverter::N2A_Vector3(mtRot.getColumn(2));
		vUp = APhysXConverter::N2A_Vector3(mtRot.getColumn(1));
	}
	void GetGPose(A3DMATRIX4& mtPose) const
	{
		NxMat33 mtRot;
		GetGRot(mtRot);
		A3DMATRIX3 amtRot;
		APhysXConverter::N2A_Matrix33(mtRot, amtRot);

		Mat44_SetRotatePart(amtRot, mtPose);
		Mat44_SetTransPart(APhysXConverter::N2A_Vector3(GetGPos()), mtPose);
	}

	// Set Local-To-Parent Pose/Scale
	void SetLPos(const NxVec3& vLPos)
	{
		if(m_vOffsetLToP == vLPos) return;
		m_vOffsetLToP = vLPos;
		UpdateGPos();
	}

	void SetLPos(const A3DVECTOR3& vLPos) 
	{ 
		SetLPos(APhysXConverter::A2N_Vector3(vLPos)); 
	}
	
	void SetLRot(const NxQuat& qRot, bool bRotMatrixDirty = true)
	{
		if(m_qRotLToP == qRot) return;
		m_qRotLToP = qRot;
		
		if(bRotMatrixDirty)
			RaiseDirtyFlag(Dirty_Flag_mtRotLToP);

		UpdateGRot();
	}

	void SetLRot(const NxMat33& mtRot)
	{
		m_mtRotLToP = mtRot;

		NxQuat q;
		mtRot.toQuat(q);
		SetLRot(q, false);
	}
	
	
	// Note: user should make sure that vDir and vUp are perpendicular and normalized.
	void SetLDirAndUp(const NxVec3& vDir, const NxVec3& vUp)
	{
		NxVec3 vRight = vUp ^ vDir;
		NxMat33 mtRot;
	
		// Note: in A3D, Dir/Up/Right represent Z-Axis/Y-Axis/X-Axis respectively.
		mtRot.setColumn(0, vRight);
		mtRot.setColumn(1, vUp);
		mtRot.setColumn(2, vDir);

		SetLRot(mtRot);
	}



	void SetLDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
	{
		SetLDirAndUp(APhysXConverter::A2N_Vector3(vDir), APhysXConverter::A2N_Vector3(vUp));
	}

	void SetLRotAndPos(const NxQuat& qRot, const NxVec3& vPos);


	// Note: there should not be scale component in mtLPose!
	void SetLPose(const A3DMATRIX4& mtPose)
	{
		A3DMATRIX3 rot(A3DMATRIX3::IDENTITY);
		Mat44_GetRotatePart(mtPose, rot);
		APhysXConverter::A2N_Matrix33(rot, m_mtRotLToP);
		NxQuat q(m_mtRotLToP);

		A3DVECTOR3 t = Mat44_GetTransPart(mtPose);
		NxVec3 v = APhysXConverter::A2N_Vector3(t);
		SetLRotAndPos(q, v);
		ClearDirtyFlag(Dirty_Flag_mtRotLToP);
	}
	
	void SetLScale(float fScale)
	{
		m_fLScale = fScale;
		UpdateGScale();
	}


	// Set Local-To-Global Pose/Scale
	void SetGPos(const NxVec3& vGPos)
	{
		if(m_vOffsetLToG == vGPos) return;

		m_vOffsetLToG = vGPos;
		UpdateLTransform(false, true, false);
		UpdateGPos(false);
	}

	void SetGPos(const A3DVECTOR3& vGPos) 
	{ 
		SetGPos(APhysXConverter::A2N_Vector3(vGPos)); 
	}

	void SetGRot(const NxQuat& qRot, bool bRotMatrixDirty = true)
	{
		if(m_qRotLToG == qRot) return;

		m_qRotLToG = qRot;
		
		if(bRotMatrixDirty)	
			RaiseDirtyFlag(Dirty_Flag_mtRotLToG);

		UpdateLTransform(true, false, false);
		UpdateGRot(false);
	}

	void SetGRot(const NxMat33& mtRot)
	{
		m_mtRotLToG = mtRot;

		NxQuat q;
		mtRot.toQuat(q);

		SetGRot(q, false);
	}

	// Note: user should make sure that vDir and vUp are perpendicular and normalized.
	void SetGDirAndUp(const NxVec3& vDir, const NxVec3& vUp)
	{
		NxVec3 vRight = vUp ^ vDir;
		NxMat33 mtRot;

		// Note: in A3D, Dir/Up/Right represent Z-Axis/Y-Axis/X-Axis respectively.
		mtRot.setColumn(0, vRight);
		mtRot.setColumn(1, vUp);
		mtRot.setColumn(2, vDir);

		SetGRot(mtRot);
	}



	void SetGDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
	{
		SetGDirAndUp(APhysXConverter::A2N_Vector3(vDir), APhysXConverter::A2N_Vector3(vUp));
	}

	void SetGRotAndPos(const NxQuat& qRot, const NxVec3& vPos);


	// Note: there should not be scale component in mtLPose!
	void SetGPose(const A3DMATRIX4& mtPose)
	{
		A3DMATRIX3 rot(A3DMATRIX3::IDENTITY);
		Mat44_GetRotatePart(mtPose, rot);
		APhysXConverter::A2N_Matrix33(rot, m_mtRotLToG);
		NxQuat q(m_mtRotLToG);

		A3DVECTOR3 t = Mat44_GetTransPart(mtPose);
		NxVec3 v = APhysXConverter::A2N_Vector3(t);
		SetGRotAndPos(q, v);
		ClearDirtyFlag(Dirty_Flag_mtRotLToG);
	}

	void SetGScale(float fScale)
	{
		m_fGScale = fScale;
		UpdateLTransform(false, false, true);
		UpdateGScale(false);
	}

	// transform local pos to global ref-frame pos
	NxVec3 TransformLPos2GPos(const NxVec3& vPos) const
	{
		NxVec3 vResPos = m_qRotLToG.rot(vPos);
		vResPos *= m_fGScale;
		vResPos += m_vOffsetLToG;
		return vResPos;
	}

	// transform local pos to parent ref-frame pos
	NxVec3 TransformLPos2PPos(const NxVec3& vPos) const
	{
		NxVec3 vResPos = m_qRotLToP.rot(vPos);
		vResPos *= m_fLScale;
		vResPos += m_vOffsetLToP;
		return vResPos;
	}

	//----------------------------------------------
	// AABB/RayTrace functions...
	
	// combine all AABBs of myself and my children
	// Beware:	in async update way, GetAABB function must be called after Tick().
	const NxBounds3& GetAABB() const;

	bool RayTraceAABB(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool bTraceChildren = true) const;
	bool RayTraceOBB(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool bTraceChildren = true) const;
	
	//-------------------------------------------------------------------------------
	// virtual functions to be overridden...

	// only my OBB, should be the OBB after scaled, but not considering Children's volume
	virtual bool GetOBB(NxBox& outObjOBB) const;
	// only my AABB, should be the AABB after scaled, but not considering Children's AABB
	virtual bool GetObjAABB(NxBounds3& outObjAABB) const;

protected:

	// some routines...
	virtual void PostCreate() {}									// called immediately after I myself am created
	virtual void PreRelease() {}									// called just before I myself am released.

	virtual void PostGamePlay() {}									// called immediately after the game-play start
	virtual void PreGameEnd() {}									// called just before the game-play start

	virtual void PostLoadResource(const char* szResName) {}			// called immediately after the major resource is loaded, 
																	// we can do some extra loading here, such as Brush loading, APhysX Data loading...

	virtual void OnUpdateParent() {	UpdateLTransform(); }			// called immediately after we update the parent object.
	virtual void OnAddChild(ApxObjBase* pObj) {}					// called immediately after we add one child object.
	virtual void OnRemoveChild(ApxObjBase* pObj) {}					// called immediately after we remove one child object.

	virtual void OnUpdateGRot() {}									// called immediately after we update the global position, override to update the position of corresponding entity
	virtual void OnUpdateGPos() {}									// called immediately after we update the global rotation, override to update the rotation of corresponding entity
	virtual void OnUpdateGScale() {}								// called immediately after we update the global scale, override to update the scale of corresponding entity

	virtual void OnTick(float dt) = 0;								// override to tick corresponding entity...
	virtual void OnRelease() {}										// override to handle some release work

	virtual ApxObjBase* NewObjAndCloneData() const = 0;				// override to new a derived class pointer and clone my data to it.

	//-------------------------------------------------------------------------------
	// internal functions...

protected:

	void RaiseDirtyFlag(APhysXU32 flag) const;

	void ClearDirtyFlag(APhysXU32 flag) const
	{
		APhysXFlagOperation::ClearFlag(m_uDirtyFlags, flag);
	}

	bool CheckDirtyFlag(APhysXU32 flag) const
	{
		return APhysXFlagOperation::CheckFlag(m_uDirtyFlags, flag);
	}
	void RaiseChildrenFlag(APhysXU32 flag);
	void ClearChildrenFlag(APhysXU32 flag);

	// we can not set one's parent directly by users, use AddChild() instead.
	void SetParent(ApxObjBase* pParent)
	{
		if (m_pParent != pParent)
		{
			m_pParent = pParent;
			OnUpdateParent();
		}
	}

	// ReleaseChildren function will release and delete the child besides RemoveChildren()
	void ReleaseChildren();

	void UpdateGRot(bool bUpdateMe = true);
	void UpdateGPos(bool bUpdateMe = true);
	void UpdateGPose(bool bUpdateMe = true);
	void UpdateGScale(bool bUpdateMe = true);

	void UpdateMyGRot();
	void UpdateMyGPos();
	void UpdateMyGPose();
	void UpdateMyGScale();

	void UpdateLTransform(bool bUpdateRot=true, bool bUpdatePos=true, bool bUpdateScale=true);
	void UpdateMyLTransform(bool bUpdateRot=true, bool bUpdatePos=true, bool bUpdateScale=true);
	void AsyncUpdateTransform();

	const NxBounds3& GetObjOrigAABB() const { return m_ObjOrigAABB; }

	//-------------------------------------------------------------------------------
	// disable copy constructor and = operator

private:  // emphasize the following members are private
	ApxObjBase(const ApxObjBase&);
	const ApxObjBase& operator=(const ApxObjBase&);

	//-------------------------------------------------------------------------------
	// data members...

protected:

	//----------------------------------------------
	// pose and scale info...
	// 
	//		Here are some equations for pose computation:
	//	
	//		P_p = R_l2p * S_l * P_l + O_l2p
	//		P_g = R_l2g * S_g * P_l + O_l2g
	//
	//		where, 
	//			P_p:	pos in parent ref-frame									P_g:	pos in global ref-frame
	//			R_l2p:	rotation from local ref-frame to parent ref-frame		R_l2g:	rotation from local ref-frame to global ref-frame
	//			S_l:	scale in object ref-frame								S_g:	scale in global ref-frame
	//			P_l:	pos in local ref-frame
	//			O_l2p:	offset from local origin to parent origin				O_l2g:	offset from local origin to global origin


	// note: we currently only support uniform scaling...
	float m_fLScale;			// scale in local ref-frame
	float m_fGScale;			// scale in global ref-frame

	// rotation and offset which transform P from local ref-frame to parent ref-frame
	NxQuat m_qRotLToP;
	NxVec3 m_vOffsetLToP;

	mutable NxMat33 m_mtRotLToP;

	// rotation and offset which transform P from local ref-frame to global ref-frame
	NxQuat m_qRotLToG;
	NxVec3 m_vOffsetLToG;

	mutable NxMat33 m_mtRotLToG;

	//----------------------------------------------
	// tree info...
	ApxObjBase* m_pParent;
	APhysXArray<ApxObjBase*> m_arrChildren;

	//----------------------------------------------
	// id info...
	APhysXU32 m_uType;
	AString m_strName;

	mutable APhysXU32 m_uDirtyFlags;		// dirty flags for lazy updating mechanism

	APhysXU32 m_uFlags;						

	//----------------------------------------------
	// AABB info...
	// object AABB in object ref-frame, used to construct the OBB for ray-tracing.
	// acquire this after resource is loaded.
	NxBounds3 m_ObjOrigAABB;
	mutable NxBounds3 m_AABB;

	//----------------------------------------------
	// public data members...
public:
	void* mUserData;

};



#endif