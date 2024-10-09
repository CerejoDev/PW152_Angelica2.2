/*----------------------------------------------------------------------

	ApxObjSkinModelSocket.h:

		Define the socket object which is corresponding to a bone/hook of 
	a A3DSkinModel, so that we can link other objects as the children of this
	socket as many as expected. This would be helpful since currently we can't
	add/change A3DSkinModel's hook at runtime.

		Created by He Wenfeng, Oct. 20, 2011

----------------------------------------------------------------------*/


#ifndef _APHYSXLAB_APXOBJSKINMODELSOCKET_H_
#define _APHYSXLAB_APXOBJSKINMODELSOCKET_H_


class ISkinModelSocket
{
protected:
	static A3DMATRIX4 s_Mat44ID;

public:
	virtual const A3DMATRIX4& GetGPose() const = 0;
	virtual float GetGScale() const = 0;

	virtual ISkinModelSocket* Clone() const = 0;

};

class ApxSkinModelSocketBone : public ISkinModelSocket
{
public:

	ApxSkinModelSocketBone() :m_pSocketBone(NULL) {}
	ApxSkinModelSocketBone(A3DBone* pSocketBone):m_pSocketBone(pSocketBone) {}

	void SetBone(A3DBone* pSocketBone) { m_pSocketBone = pSocketBone; }

	virtual const A3DMATRIX4& GetGPose() const
	{
		if(m_pSocketBone)
			return m_pSocketBone->GetNoScaleAbsTM();
		else
			return s_Mat44ID;
	}
	virtual float GetGScale() const
	{
		// just use the X component as the G scale since we only support uniform-scaling
		if(m_pSocketBone)
			return m_pSocketBone->GetAbsoluteTM().GetRow(0).Magnitude();
		else
			return 1.0f;
	}

	virtual ISkinModelSocket* Clone() const
	{
		ApxSkinModelSocketBone* pSocketBone = new ApxSkinModelSocketBone;
		pSocketBone->m_pSocketBone = m_pSocketBone;
		return pSocketBone;
	}

protected:
	A3DBone* m_pSocketBone;
};


class ApxSkinModelSocketHook: public ISkinModelSocket
{
public:

	ApxSkinModelSocketHook() :m_pSocketHook(NULL) {}
	ApxSkinModelSocketHook(A3DSkeletonHook* pSocketHook):m_pSocketHook(pSocketHook) {}

	void SetHook(A3DSkeletonHook* pSocketHook) { m_pSocketHook = pSocketHook;}

	virtual const A3DMATRIX4& GetGPose() const
	{
		if(m_pSocketHook)
			return m_pSocketHook->GetNoScaleAbsTM();
		else
			return s_Mat44ID;		
	}

	virtual float GetGScale() const
	{
		// just use the X component as the G scale since we only support uniform-scaling
		if(m_pSocketHook)
			return m_pSocketHook->GetAbsoluteTM().GetRow(0).Magnitude();
		else
			return 1.0f;
	}

	virtual ISkinModelSocket* Clone() const
	{
		ApxSkinModelSocketHook* pSocketHook = new ApxSkinModelSocketHook;
		pSocketHook->m_pSocketHook = m_pSocketHook;
		return pSocketHook;
	}

protected:
	A3DSkeletonHook* m_pSocketHook;
};

enum ApxSkinModelSocketType
{
	ApxSM_SocketType_Bone,
	ApxSM_SocketType_Hook,
};

class ApxObjSocketBase : public IObjBase, public IPropObjBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000001; }
	static const TCHAR* GetSocketTypeText(const ApxSkinModelSocketType st);
	static bool GetSocketType(const TCHAR* pStr, ApxSkinModelSocketType& outST);

	ApxObjSocketBase();
	virtual ~ApxObjSocketBase();
	ApxObjSocketBase& operator= (const ApxObjSocketBase& rhs);
	CPhysXObjDynamic* GetBindingObject() const { return m_pObjBinding; }

	void SetSocketType(ApxSkinModelSocketType smsType);
	ApxSkinModelSocketType GetSocketType(bool bRuntimeType = true) const;

	void SetBoneHookName(const TString& smsName);
	const TString& GetBoneHookName(bool bRuntimeName = true) const;
	const TCHAR* GetBoneHookName_cstr(bool bRuntimeName = true) const;

	virtual const TCHAR* GetDescText() const = 0;
	virtual bool Attach(CPhysXObjDynamic* pObjDynamic, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType) = 0;

	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid();
	virtual bool OnPropertyChanged(PropItem& prop);
	virtual void OnUpdatePropsGrid();

protected:
	ApxObjSocketBase(const ApxObjSocketBase& rhs);
	void SetEmpty();

	ISkinModelSocket* CreateSocket(CPhysXObjDynamic& objDyn, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType);
	ISkinModelSocket* GetSocket() const { return m_pSkinModelSocket; }
	void SetSocket(ISkinModelSocket* pISMS);

	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePostNotify();
	virtual void SetBindingObject(ApxObjBase* pObj);
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime);
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return true; }
	virtual void OnLeaveRuntime() {}
	virtual bool OnResetPose() { return true; }
	virtual void OnSetPos(const A3DVECTOR3& vPos) {}
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) {}
	virtual void OnSetPose(const A3DMATRIX4& matPose) {}
	virtual bool OnSetScale(const float& scale) { return false; }

private:
	virtual void UpdateTransforms() = 0;
	virtual void OnDoSleepTask() {}
	virtual bool OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

protected:
	int m_dwRenderColor;
	bool m_EnableCreateSocket;
	NxBounds3 m_aabb;

private:
	CPhysXObjDynamic* m_pObjBinding;
	ISkinModelSocket* m_pSkinModelSocket;

	TString m_strSocketName;
	ApxSkinModelSocketType m_apxSocketType;

	TString m_strSocketNameEdit;
	ApxSkinModelSocketType m_apxSocketTypeEdit;
};

class ApxObjSkinModelToSocket : public ApxObjSocketBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000001; }

	ApxObjSkinModelToSocket(int UID = -1);
	virtual ~ApxObjSkinModelToSocket();
	ApxObjSkinModelToSocket& operator= (const ApxObjSkinModelToSocket& rhs);

	virtual const TCHAR* GetDescText() const;
	virtual bool Attach(CPhysXObjDynamic* pObjDynamic, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType);

protected:
	ApxObjSkinModelToSocket(const ApxObjSkinModelToSocket& rhs) : ApxObjSocketBase(rhs) {}
	virtual ApxObjSkinModelToSocket* CreateMyselfInstance(bool snapValues) const;

	virtual void OnTick(float dt);
	virtual void OnUpdateParent();

	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;

private:
	virtual ApxObjSkinModelToSocket* NewObjAndCloneData() const;
	virtual void UpdateTransforms();
};

class ApxObjSocketToSkinModel : public ApxObjSocketBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000001; }

	ApxObjSocketToSkinModel(int UID = -1);
	virtual ~ApxObjSocketToSkinModel();
	ApxObjSocketToSkinModel& operator= (const ApxObjSocketToSkinModel& rhs);

	virtual const TCHAR* GetDescText() const;
	virtual bool Attach(CPhysXObjDynamic* pObjDynamic, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType);

protected:
	ApxObjSocketToSkinModel(const ApxObjSocketToSkinModel& rhs) : ApxObjSocketBase(rhs) {}
	virtual ApxObjSocketToSkinModel* CreateMyselfInstance(bool snapValues) const;
	virtual void SetBindingObject(ApxObjBase* pObj);

	virtual void OnTick(float dt);
	virtual void OnUpdateParent();
	virtual void OnAddChild(ApxObjBase* pObj);
	virtual void OnRemoveChild(ApxObjBase* pObj);

	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;

private:
	virtual ApxObjSocketToSkinModel* NewObjAndCloneData() const;
	virtual void UpdateTransforms();
};
#endif