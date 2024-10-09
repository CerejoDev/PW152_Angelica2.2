
#include "stdafx.h"

A3DMATRIX4 ISkinModelSocket::s_Mat44ID(A3DMATRIX4::IDENTITY);

const TCHAR* ApxObjSocketBase::GetSocketTypeText(const ApxSkinModelSocketType st)
{
	switch (st)
	{
	case ApxSM_SocketType_Bone:
		return _T("LinkToBone");
	case ApxSM_SocketType_Hook:
		return _T("LinkToHook");
	}

	assert(!"Unknown Socket Type!");
	return szEmpty;
}

bool ApxObjSocketBase::GetSocketType(const TCHAR* pStr, ApxSkinModelSocketType& outST)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetSocketTypeText(ApxSM_SocketType_Bone)))
	{
		outST = ApxSM_SocketType_Bone;
		return true;
	}
	if (0 == a_strcmp(pStr, GetSocketTypeText(ApxSM_SocketType_Hook)))
	{
		outST = ApxSM_SocketType_Hook;
		return true;
	}

	assert(!"Unknown ApxSkinModelSocketType!");
	return false;
}

ApxObjSocketBase::ApxObjSocketBase()
{
	SetEmpty();
}

ApxObjSocketBase::ApxObjSocketBase(const ApxObjSocketBase& rhs) : IPropObjBase(rhs)
{
	SetEmpty();
}

void ApxObjSocketBase::SetEmpty()
{
	m_dwRenderColor = 0xff652093;
	m_EnableCreateSocket = true;
	m_aabb.setCenterExtents(NxVec3(0.0f), NxVec3(0.3f));

	m_pObjBinding = 0;
	m_pSkinModelSocket = 0;

	m_apxSocketType = ApxSM_SocketType_Bone;
	m_apxSocketTypeEdit = ApxSM_SocketType_Bone;
}

ApxObjSocketBase::~ApxObjSocketBase()
{
	if (0 != m_pSkinModelSocket)
	{
		delete m_pSkinModelSocket;
		m_pSkinModelSocket = NULL;
	}
}

ApxObjSocketBase& ApxObjSocketBase::operator= (const ApxObjSocketBase& rhs)
{
	bool bEnableCopy = false;
	ISkinModelSocket* pNewSocket = 0;
	if (0 == rhs.m_pSkinModelSocket)
	{
		bEnableCopy = true;
	}
	else
	{
		pNewSocket = rhs.m_pSkinModelSocket->Clone();
		if (0 != pNewSocket)
			bEnableCopy = true;
	}

	if (bEnableCopy)
	{
		m_EnableCreateSocket = rhs.m_EnableCreateSocket;
		m_pObjBinding = rhs.m_pObjBinding;
		delete m_pSkinModelSocket;
		m_pSkinModelSocket = pNewSocket;
		m_strSocketName = rhs.m_strSocketName;
		m_apxSocketType = rhs.m_apxSocketType;
		m_strSocketNameEdit = rhs.m_strSocketNameEdit;
		m_apxSocketTypeEdit = rhs.m_apxSocketTypeEdit;
		IPropObjBase::operator=(rhs);
	}
	return *this;
}

ISkinModelSocket* ApxObjSocketBase::CreateSocket(CPhysXObjDynamic& objDyn, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType)
{
	A3DSkinModel* pSkinModel = objDyn.GetSkinModel();
	if (0 == pSkinModel)
		return 0;

	ISkinModelSocket* pNewSocket = 0;
	if(apxSMSocketType == ApxSM_SocketType_Bone)
	{
		A3DBone* pBone = pSkinModel->GetSkeleton()->GetBone(_TWC2AS(szSocketName), NULL);
		if(pBone)
			pNewSocket = new ApxSkinModelSocketBone(pBone);
	}
	else if(apxSMSocketType == ApxSM_SocketType_Hook)
	{
		A3DSkeletonHook* pHook = pSkinModel->GetSkeletonHook(_TWC2AS(szSocketName), true);
		if(pHook)
			pNewSocket = new ApxSkinModelSocketHook(pHook);
	}
	return pNewSocket;
}

void ApxObjSocketBase::SetSocket(ISkinModelSocket* pISMS)
{
	delete m_pSkinModelSocket;
	m_pSkinModelSocket = pISMS;
	if (0 != m_pSkinModelSocket)
		UpdateTransforms();

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Pos_xyz);
		pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Dir_xyz);
		pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Up_xyz);
		if (m_pSkinModelSocket)
			pSelGroup->RefreshFrame();
	}
}

void ApxObjSocketBase::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropObjBase::EnterRuntimePreNotify(aPhysXScene);
	ApxObjSocketBase* pBK = dynamic_cast<ApxObjSocketBase*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void ApxObjSocketBase::LeaveRuntimePostNotify()
{
	IPropObjBase::LeaveRuntimePostNotify();
	ApxObjSocketBase* pBK = dynamic_cast<ApxObjSocketBase*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

void ApxObjSocketBase::SetBindingObject(ApxObjBase* pObj)
{
	IObjBase* pObjBase = dynamic_cast<IObjBase*>(pObj);
	if (0 != pObjBase)
	{
		ObjManager::ObjTypeID ot = pObjBase->GetProperties()->GetObjType();
		if ((ObjManager::OBJ_TYPEID_ECMODEL == ot) || (ObjManager::OBJ_TYPEID_SKINMODEL == ot))
			m_pObjBinding = dynamic_cast<CPhysXObjDynamic*>(pObjBase);
		else
			m_pObjBinding = 0;
	}
	else
	{
		m_pObjBinding = 0;
	}

	if (!m_EnableCreateSocket)
		return;

	ISkinModelSocket* pSMS = 0;
	if (0 != m_pObjBinding)
	{
		const TString& smsName = GetBoneHookName();
		if (!smsName.IsEmpty())
			pSMS = CreateSocket(*m_pObjBinding, smsName, GetSocketType());
	}
	SetSocket(pSMS);
}

ApxSkinModelSocketType ApxObjSocketBase::GetSocketType(bool bRuntimeType) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (bRuntimeType)
			return m_apxSocketType;
		else
			return m_apxSocketTypeEdit;
	}
	return m_apxSocketTypeEdit;
}

void ApxObjSocketBase::SetSocketType(ApxSkinModelSocketType smsType)
{
	if (GetSocketType() == smsType)
		return;

	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		m_apxSocketType = smsType;
	else
		m_apxSocketTypeEdit = smsType;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_SocketType);

	if (m_EnableCreateSocket)
	{
		ISkinModelSocket* pSMS = 0;
		if (0 != m_pObjBinding)
		{
			const TString& smsName = GetBoneHookName();
			if (!smsName.IsEmpty())
				pSMS = CreateSocket(*m_pObjBinding, smsName, smsType);
		}
		SetSocket(pSMS);
	}
}

const TCHAR* ApxObjSocketBase::GetBoneHookName_cstr(bool bRuntimeName) const
{
	const TString& smsName = GetBoneHookName(bRuntimeName);
	return smsName;
}

const TString& ApxObjSocketBase::GetBoneHookName(bool bRuntimeName) const
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if (bRuntimeName)
			return m_strSocketName;
		else
			return m_strSocketNameEdit;
	}
	return m_strSocketNameEdit;
}

void ApxObjSocketBase::SetBoneHookName(const TString& smsName)
{
	if (GetBoneHookName() == smsName)
		return;

	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		m_strSocketName = smsName;
	else
		m_strSocketNameEdit = smsName;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_SocketName);

	if (m_EnableCreateSocket)
	{
		ISkinModelSocket* pSMS = 0;
		if (0 != m_pObjBinding)
		{
			ApxSkinModelSocketType smsType = GetSocketType();
			pSMS = CreateSocket(*m_pObjBinding, smsName, smsType);
		}
		SetSocket(pSMS);
	}
}

bool ApxObjSocketBase::SaveToFile(NxStream& nxStream)
{
	if (IPropObjBase::SaveToFile(nxStream))
	{
		SaveVersion(&nxStream);
		nxStream.storeBuffer(&m_apxSocketTypeEdit, sizeof(ApxSkinModelSocketType));
		APhysXSerializerBase::StoreString(&nxStream, _TWC2AS(m_strSocketNameEdit));
		return true;
	}
	return false;
}

bool ApxObjSocketBase::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (IPropObjBase::LoadFromFile(nxStream, outIsLowVersion))
	{
		LoadVersion(&nxStream);
		if(m_StreamObjVer >= 0xAA000001)
		{
			m_strSocketNameEdit.Empty();
			nxStream.readBuffer(&m_apxSocketTypeEdit, sizeof(ApxSkinModelSocketType));
			const char* pStr = APhysXSerializerBase::ReadString(&nxStream);
			if (0 != pStr)
				m_strSocketNameEdit = _TAS2WC(pStr);
		}
		if(m_StreamObjVer < GetVersion())
			outIsLowVersion = true;
		return true;
	}
	return false;
}

bool ApxObjSocketBase::OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	if (!IObjBase::OnDoWakeUpTask(eu, pPSToRuntime))
		return false;

	if (0 == m_pSkinModelSocket)
	{
		if (0 != m_pObjBinding)
		{
			if (IPropObjBase::OBJECT_STATE_SLEEP == m_pObjBinding->GetProperties()->GetObjState())
				m_pObjBinding->WakeUp(eu, pPSToRuntime);

			const TString& smsName = GetBoneHookName();
			if (!smsName.IsEmpty())
			{
				ISkinModelSocket* pSMS = CreateSocket(*m_pObjBinding, smsName, GetSocketType());
				if (0 != pSMS)
					SetSocket(pSMS);
			}
		}
	}
	return true;
}

bool ApxObjSocketBase::OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const
{
	NxMat34 mat;
	GetGRot(mat.M);
	mat.t = GetGPos();
	NxVec3 exts;
	m_aabb.getExtents(exts);
	exts *= GetScale(); 

	PhysOBB rtOBB;
	rtOBB.Set(APhysXConverter::N2A_Vector3(mat.M.getColumn(0)), 
		APhysXConverter::N2A_Vector3(mat.M.getColumn(1)), 
		APhysXConverter::N2A_Vector3(mat.M.getColumn(2)), 
		APhysXConverter::N2A_Vector3(mat.t), 
		APhysXConverter::N2A_Vector3(exts)
		);

	if (::RayTraceOBB(ray, rtOBB, hitInfo))
	{
		hitInfo.UserData = (void*)this;
		return true;
	}
	return false;
}

void ApxObjSocketBase::BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor)
{
	if (IsRuntime())
	{
		if (!m_pScene->IsPhysXDebugRenderEnabled())
			return;
	}

	NxBox nxOBB;
	if (GetOBB(nxOBB))
	{
		A3DOBB obb;
		obb.Center = APhysXConverter::N2A_Vector3(nxOBB.center);
		obb.XAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(0));
		obb.YAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(1));
		obb.ZAxis = APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(2));
		if (0 == dwColor)
		{
			dwColor = m_dwRenderColor;
			NxVec3 exts;
			m_aabb.getExtents(exts);
			obb.Extents = APhysXConverter::N2A_Vector3(exts);
		}
		else
		{
			obb.Extents = APhysXConverter::N2A_Vector3(nxOBB.extents);
		}
		obb.CompleteExtAxis();
		er.AddOBB(obb, dwColor);
	}

	if (!m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		if(0 == m_pSkinModelSocket)
			er.AddSphere(APhysXConverter::N2A_Vector3(GetGPos()), 0.1f, 0xffff0000);
		else
		{
			A3DVECTOR3 vPos = m_pSkinModelSocket->GetGPose().GetRow(3);
			A3DVECTOR3 vX = m_pSkinModelSocket->GetGPose().GetRow(0);
			A3DVECTOR3 vY = m_pSkinModelSocket->GetGPose().GetRow(1);
			A3DVECTOR3 vZ = m_pSkinModelSocket->GetGPose().GetRow(2);
			er.AddSphere(vPos, 0.05f, 0xff888888);
			er.AddLine(vPos, vPos + vX * 0.25f, 0xffff0000);
			er.AddLine(vPos, vPos + vY * 0.25f, 0xff00ff00);
			er.AddLine(vPos, vPos + vZ * 0.25f, 0xff0000ff);
		}
	}
}

bool ApxObjSocketBase::OnSendToPropsGrid()
{
	bool bRtn = IPropObjBase::OnSendToPropsGrid();
	if (bRtn)
		GetSelGroup()->FillGroupSocket(*this);
	return bRtn;
}

bool ApxObjSocketBase::OnPropertyChanged(PropItem& prop)
{
	if (IPropObjBase::OnPropertyChanged(prop))
		return true;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	SetSelGroup(0);
	bool bRtnValue = false;

	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_SocketType == propValue)
	{
		ApxSkinModelSocketType smsST;
		bool rtn = GetSocketType(_bstr_t(prop.GetValue()), smsST);
		if (rtn)
		{
			SetSocketType(smsST);
			pSelGroup->RaiseUpdateMarkToSocketName();
			bRtnValue = true;
		}
	}
	else if (PID_ITEM_SocketName == propValue)
	{
		_bstr_t linkName = prop.GetValue();
		TCHAR* pLN = (TCHAR*)linkName;
		SetBoneHookName(pLN);
		pSelGroup->RefreshFrame();
		bRtnValue = true;
	}
	SetSelGroup(pSelGroup);
	return bRtnValue;
}

void ApxObjSocketBase::OnUpdatePropsGrid()
{
	IPropObjBase::OnUpdatePropsGrid();
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 == pSelGroup)
		return;
	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	const bool u1 = pSelGroup->ReadUpdateMark(PID_ITEM_SocketDesc);
	if (u1)
	{
		assert(!"Shouldn't be changed or not supported yet!");
	}

	if (pSelGroup->ReadUpdateMark(PID_GROUP_Socket))
	{
		pSelGroup->FillGroupSocket(*this);
		return;
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_SocketType))
	{
		const TCHAR* pType = GetSocketTypeText(GetSocketType());
		pSelGroup->ExecuteUpdateItem(PID_ITEM_SocketType, pType, bIsMerge);
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_SocketName))
	{
		const TCHAR* pName = GetBoneHookName_cstr();
		pSelGroup->ExecuteUpdateItem(PID_ITEM_SocketName, pName, bIsMerge);
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ApxObjSkinModelToSocket::ApxObjSkinModelToSocket(int UID)
{
	assert(0 <= UID);
	InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_SMSOCKET, *this, UID);
	SetProperties(*this);
	SetObjName(_T("SMToSocket"));

	SetOriginalAABB(m_aabb);
	SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
}

ApxObjSkinModelToSocket::~ApxObjSkinModelToSocket()
{
	if (0 <= GetUID())
	{
		assert(false == IsObjAliveState());
	} 
}

ApxObjSkinModelToSocket& ApxObjSkinModelToSocket::operator= (const ApxObjSkinModelToSocket& rhs)
{
	ApxObjSocketBase::operator=(rhs);
	return *this;
}

ApxObjSkinModelToSocket* ApxObjSkinModelToSocket::CreateMyselfInstance(bool snapValues) const
{
	ApxObjSkinModelToSocket* pProp = new ApxObjSkinModelToSocket(*this);
	return pProp;
}

ApxObjSkinModelToSocket* ApxObjSkinModelToSocket::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_SMSOCKET, true, *m_pScene);
	ApxObjSkinModelToSocket* pObj = dynamic_cast<ApxObjSkinModelToSocket*>(pObjBase);
	if (0 != pObj)
		*pObj = *this;
	return pObj;
}

const TCHAR* ApxObjSkinModelToSocket::GetDescText() const
{
	return _T("Passive(FromSM");
}

bool ApxObjSkinModelToSocket::Attach(CPhysXObjDynamic* pObjDynamic, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType)
{
	if (0 == pObjDynamic)
		return false;

	if (GetParent() == pObjDynamic)
	{
		if (GetSocketType() == apxSMSocketType)
		{
			if (GetBoneHookName() == szSocketName)
				return true;
		}
	}

	ISkinModelSocket* pNewSocket = CreateSocket(*pObjDynamic, szSocketName, apxSMSocketType);
	if (0 == pNewSocket)
		return false;

	m_EnableCreateSocket = false;
	SetSocketType(apxSMSocketType);
	SetBoneHookName(szSocketName);
	//--------------------------------------
	// note here, We have to add this as the child of pObjDynamic 
	// so that my Tick will be done after the tick of the SkinModel,
	// and this can make sure my Pose and Scale data are updated in this frame.
	pObjDynamic->AddChild(this);
	m_EnableCreateSocket = true;
	SetSocket(pNewSocket);

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_GROUP_Socket);
	return true;
}

void ApxObjSkinModelToSocket::UpdateTransforms()
{
	ISkinModelSocket* pISMS = GetSocket(); 
	if (0 != pISMS)
	{
		assert(0 != GetBindingObject());
		// update my pose and scale...
		SetGPose(pISMS->GetGPose());
		SetGScale(pISMS->GetGScale());
	}
}

void ApxObjSkinModelToSocket::OnTick(float dt)
{
	IObjBase::OnTick(dt);
	if (IsActive())
		UpdateTransforms();
	IObjBase::BuildExtraRenderData();
}

void ApxObjSkinModelToSocket::OnUpdateParent()
{
	IObjBase::OnUpdateParent();
	SetBindingObject(GetParent());
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMarkToSocketName();
}

bool ApxObjSkinModelToSocket::OnGetPos(A3DVECTOR3& vOutPos) const
{
	ISkinModelSocket* pISMS = GetSocket(); 
	if (0 == pISMS)
		vOutPos = GetPos(false);
	else
		vOutPos = pISMS->GetGPose().GetRow(3);
	return true;
}

bool ApxObjSkinModelToSocket::OnGetDir(A3DVECTOR3& vOutDir) const
{
	ISkinModelSocket* pISMS = GetSocket(); 
	if (0 == pISMS)
		vOutDir = GetDir(false);
	else
		vOutDir = pISMS->GetGPose().GetRow(2);
	return true; 
}

bool ApxObjSkinModelToSocket::OnGetUp(A3DVECTOR3& vOutUp) const
{
	ISkinModelSocket* pISMS = GetSocket(); 
	if (0 == pISMS)
		vOutUp = GetUp(false);
	else
		vOutUp = pISMS->GetGPose().GetRow(1);
	return true; 
}

bool ApxObjSkinModelToSocket::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	ISkinModelSocket* pISMS = GetSocket(); 
	if (0 == pISMS)
		matOutPose = GetPose(false);
	else
		matOutPose = pISMS->GetGPose();
	outWithScaled = false;
	return true; 
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ApxObjSocketToSkinModel::ApxObjSocketToSkinModel(int UID)
{
	assert(0 <= UID);
	InitTypeIDAndHostObject(ObjManager::OBJ_TYPEID_SOCKETSM, *this, UID);
	SetProperties(*this);
	SetObjName(_T("SocketToSM"));

	m_aabb.setCenterExtents(NxVec3(0.0f), NxVec3(0.25f));
	SetOriginalAABB(m_aabb);
	SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	m_dwRenderColor = 0xff658870;
}

ApxObjSocketToSkinModel::~ApxObjSocketToSkinModel()
{
	if (0 <= GetUID())
	{
		assert(false == IsObjAliveState());
	} 
}

ApxObjSocketToSkinModel& ApxObjSocketToSkinModel::operator= (const ApxObjSocketToSkinModel& rhs)
{
	ApxObjSocketBase::operator=(rhs);
	return *this;
}

ApxObjSocketToSkinModel* ApxObjSocketToSkinModel::CreateMyselfInstance(bool snapValues) const
{
	ApxObjSocketToSkinModel* pProp = new ApxObjSocketToSkinModel(*this);
	return pProp;
}

ApxObjSocketToSkinModel* ApxObjSocketToSkinModel::NewObjAndCloneData() const
{
	assert(0 != m_pScene);
	IObjBase* pObjBase = ObjManager::GetInstance()->CreateObject(ObjManager::OBJ_TYPEID_SOCKETSM, true, *m_pScene);
	ApxObjSocketToSkinModel* pObj = dynamic_cast<ApxObjSocketToSkinModel*>(pObjBase);
	if (0 != pObj)
		*pObj = *this;
	return pObj;
}

const TCHAR* ApxObjSocketToSkinModel::GetDescText() const
{
	return _T("Initiative(ToSM)");
}

bool ApxObjSocketToSkinModel::Attach(CPhysXObjDynamic* pObjDynamic, const TCHAR* szSocketName, ApxSkinModelSocketType apxSMSocketType)
{
	if (0 == pObjDynamic)
		return false;

	if (GetBindingObject() == pObjDynamic)
	{
		if (GetSocketType() == apxSMSocketType)
		{
			if (GetBoneHookName() == szSocketName)
				return true;
		}
	}

	ISkinModelSocket* pNewSocket = CreateSocket(*pObjDynamic, szSocketName, apxSMSocketType);
	if (0 == pNewSocket)
		return false;

	m_EnableCreateSocket = false;
	SetSocketType(apxSMSocketType);
	SetBoneHookName(szSocketName);
	SetBindingObject(pObjDynamic);
	//--------------------------------------
	// note here, We have to add pObjDynamic as the child of this(before update ISkinModelSocket) 
	// so that my Tick will be done before the tick of the SkinModel,
	// and this can make sure the pObjDynamic Pose and Scale data are updated in this frame.
	if (this != pObjDynamic->GetParent())
		AddChild(pObjDynamic);
	m_EnableCreateSocket = true;
	SetSocket(pNewSocket);

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_GROUP_Socket);
	return true;
}

void ApxObjSocketToSkinModel::UpdateTransforms()
{
	ISkinModelSocket* pISMS = GetSocket(); 
	if (0 != pISMS)
	{
		CPhysXObjDynamic* pObjBinding = GetBindingObject();
		assert(0 != pObjBinding);
		A3DMATRIX4 poseModel(A3DMATRIX4::IDENTITY);
		pObjBinding->GetGPose(poseModel);

		A3DMATRIX4 poseBoneOrHook = pISMS->GetGPose();
		A3DMATRIX4 bhInv = poseBoneOrHook;
		bhInv.InverseTM();

		A3DMATRIX4 modelRelativeBH = poseModel * bhInv;
		pObjBinding->SetLPose(modelRelativeBH);
	}
}

void ApxObjSocketToSkinModel::SetBindingObject(ApxObjBase* pObj)
{
	CPhysXObjDynamic* pOldObj = GetBindingObject();
	if (0 != pOldObj)
		pOldObj->GetProperties()->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED_ONSIM);

	ApxObjSocketBase::SetBindingObject(pObj);
	CPhysXObjDynamic* pDynObj = GetBindingObject();
	if (0 != pDynObj)
	{
		if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			pDynObj->GetProperties()->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION, false); 
		else
			pDynObj->GetProperties()->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION, true);
		pDynObj->GetProperties()->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED_ONSIM);
	}
}

void ApxObjSocketToSkinModel::OnTick(float dt)
{
	IObjBase::OnTick(dt);
	IObjBase::BuildExtraRenderData();
}

void ApxObjSocketToSkinModel::OnUpdateParent()
{
	IObjBase::OnUpdateParent();
	if (0 != GetParent())
	{
		A3DMATRIX4 smPose(A3DMATRIX4::IDENTITY);
		SetLPose(smPose);
		CPhysXObjSelGroup* pSelGroup = GetSelGroup();
		if (0 != pSelGroup)
		{
			pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Pos_xyz);
			pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Dir_xyz);
			pSelGroup->RaiseUpdateMark(PID_SUBGROUP_Up_xyz);
			pSelGroup->RefreshFrame();
		}
	}
}

void ApxObjSocketToSkinModel::OnAddChild(ApxObjBase* pObj)
{
	if (0 != GetBindingObject())
		return;

	SetBindingObject(pObj);
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMarkToSocketName();
}

void ApxObjSocketToSkinModel::OnRemoveChild(ApxObjBase* pObj)
{
	CPhysXObjDynamic* pObjBinding = GetBindingObject();
	if (pObjBinding != pObj)
		return;

	if (0 == pObjBinding)
	{
		assert(0 == GetSocket());
		return;
	}

	CPhysXObjDynamic* pDynObj = 0;
	int nConut = GetChildNum();
	for (int i = 0; i < nConut; ++i)
	{
		pDynObj = dynamic_cast<CPhysXObjDynamic*>(GetChild(i));
		if (0 != pDynObj)
			break;
	}

	SetBindingObject(pDynObj);
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMarkToSocketName();
}

bool ApxObjSocketToSkinModel::OnGetPos(A3DVECTOR3& vOutPos) const
{
	vOutPos = GetPos(false);
	return true;
}

bool ApxObjSocketToSkinModel::OnGetDir(A3DVECTOR3& vOutDir) const
{
	vOutDir = GetDir(false);
	return true; 
}

bool ApxObjSocketToSkinModel::OnGetUp(A3DVECTOR3& vOutUp) const
{
	vOutUp = GetUp(false);
	return true; 
}

bool ApxObjSocketToSkinModel::OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const
{
	matOutPose = GetPose(false);
	outWithScaled = false;
	return true; 
}
