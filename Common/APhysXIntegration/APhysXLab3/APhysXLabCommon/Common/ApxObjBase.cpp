
#include "stdafx.h"


ApxObjOperationReport* ApxObjBase::m_spOperationReport = NULL;


void ApxObjBase::SetLRotAndPos(const NxQuat& qRot, const NxVec3& vPos)
{

	if(m_qRotLToP == qRot)
	{
		SetLPos(vPos);
	}
	else
	{
		if(m_vOffsetLToP == vPos)
			SetLRot(qRot);
		else
		{
			m_qRotLToP = qRot;
			m_vOffsetLToP = vPos;

			UpdateGPose();
			RaiseDirtyFlag(Dirty_Flag_mtRotLToP);
		}
	}

}

void ApxObjBase::SetGRotAndPos(const NxQuat& qRot, const NxVec3& vPos)
{

	if(m_qRotLToG == qRot)
	{
		SetGPos(vPos);
	}
	else
	{
		if(m_vOffsetLToG == vPos)
			SetGRot(qRot);
		else
		{
			m_qRotLToG = qRot;
			m_vOffsetLToG = vPos;
			RaiseDirtyFlag(Dirty_Flag_mtRotLToG);

			UpdateLTransform(true, true, false);
			UpdateGPose(false);

		}
	}

}

void ApxObjBase::RaiseChildrenFlag(APhysXU32 flag)
{
	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->RaiseFlag(flag);
		m_arrChildren[i]->RaiseChildrenFlag(flag);
	}
}

void ApxObjBase::ClearChildrenFlag(APhysXU32 flag)
{
	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->ClearFlag(flag);
		m_arrChildren[i]->ClearChildrenFlag(flag);
	}
}


void ApxObjBase::UpdateMyGRot()
{
	if(m_pParent)
	{
		m_qRotLToG = m_pParent->m_qRotLToG * m_qRotLToP;
	}
	else
	{
		m_qRotLToG = m_qRotLToP;
	}

	RaiseDirtyFlag(Dirty_Flag_mtRotLToG);

}
void ApxObjBase::UpdateMyGPos()
{
	if(m_pParent)
	{
		m_vOffsetLToG = m_pParent->m_qRotLToG.rot(m_vOffsetLToP);
		m_vOffsetLToG *= m_pParent->m_fGScale;
		m_vOffsetLToG += m_pParent->m_vOffsetLToG;
	}
	else
	{
		m_vOffsetLToG = m_vOffsetLToP;
	}
}

void ApxObjBase::UpdateMyGPose()
{
	if(m_pParent)
	{
		m_vOffsetLToG = m_pParent->m_qRotLToG.rot(m_vOffsetLToP);
		m_vOffsetLToG *= m_pParent->m_fGScale;
		m_vOffsetLToG += m_pParent->m_vOffsetLToG;

		m_qRotLToG = m_pParent->m_qRotLToG * m_qRotLToP;

	}
	else
	{
		m_vOffsetLToG = m_vOffsetLToP;

		m_qRotLToG = m_qRotLToP;
	}	

	RaiseDirtyFlag(Dirty_Flag_mtRotLToG);

}

void ApxObjBase::UpdateMyGScale()
{
	if(m_pParent)
	{
		m_fGScale = m_pParent->m_fGScale * m_fLScale;
	}
	else
	{
		m_fGScale = m_fLScale;
	}
}


void ApxObjBase::UpdateGRot(bool bUpdateMe)
{

	if(CheckFlag(Object_Flag_UpdateTransform_Async))
	{
		RaiseDirtyFlag(Dirty_Flag_GRot);
	}
	else 
	{
		if(bUpdateMe)
		{
			UpdateMyGRot();
		}

		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGRot();

	}


	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->UpdateGRot();
		m_arrChildren[i]->UpdateGPos();
	}

}

void ApxObjBase::UpdateGPos(bool bUpdateMe)
{
	if(CheckFlag(Object_Flag_UpdateTransform_Async))
	{
		RaiseDirtyFlag(Dirty_Flag_GPos);
	}
	else
	{
		if(bUpdateMe)
		{
			UpdateMyGPos();
		}

		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGPos();

	}

	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->UpdateGPos();
	}
	
}

void ApxObjBase::UpdateGPose(bool bUpdateMe)
{

	if(CheckFlag(Object_Flag_UpdateTransform_Async))
	{
		RaiseDirtyFlag(Dirty_Flag_GRot);
		RaiseDirtyFlag(Dirty_Flag_GPos);
	}
	else
	{
		if(bUpdateMe)
		{
			UpdateMyGPose();
		}
		
		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGRot();
		OnUpdateGPos();

	}

	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->UpdateGPose();
	}

}

void ApxObjBase::UpdateGScale(bool bUpdateMe)
{
	
	if(CheckFlag(Object_Flag_UpdateTransform_Async))
	{
		RaiseDirtyFlag(Dirty_Flag_GScale);
	}
	else
	{
		if(bUpdateMe)
		{
			UpdateMyGScale();
		}
		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGScale();

	}


	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->UpdateGScale();
		m_arrChildren[i]->UpdateGPos();
	}

}

void ApxObjBase::UpdateMyLTransform(bool bUpdateRot, bool bUpdatePos, bool bUpdateScale)
{
	if(m_pParent)
	{
		if(bUpdateRot || bUpdatePos)
		{
			NxQuat q(m_pParent->m_qRotLToG);
			q.invert();

			if(bUpdateRot)
			{
				m_qRotLToP.multiply(q, m_qRotLToG);
			}

			if(bUpdatePos)
			{
				m_vOffsetLToP = (m_vOffsetLToG - m_pParent->m_vOffsetLToG);
				m_vOffsetLToP = q.rot(m_vOffsetLToP);

				m_vOffsetLToP /= m_pParent->m_fGScale;
			}
		}

		if(bUpdateScale)
		{
			m_fLScale = m_fGScale / m_pParent->m_fGScale;
		}

	}
	else
	{
		if(bUpdateRot) m_qRotLToP = m_qRotLToG;
		if(bUpdatePos) m_vOffsetLToP = m_vOffsetLToG;
		if(bUpdateScale) m_fLScale = m_fGScale;
	}

	if(bUpdateRot)
	{
		RaiseDirtyFlag(Dirty_Flag_mtRotLToP);
	}
}

void ApxObjBase::UpdateLTransform(bool bUpdateRot, bool bUpdatePos, bool bUpdateScale)
{

	if(CheckFlag(Object_Flag_UpdateTransform_Async))
	{
		if(bUpdateRot) RaiseDirtyFlag(Dirty_Flag_LRot);
		if(bUpdatePos) RaiseDirtyFlag(Dirty_Flag_LPos);
		if(bUpdateScale) RaiseDirtyFlag(Dirty_Flag_LScale);
	}
	else
	{
		UpdateMyLTransform(bUpdateRot, bUpdatePos, bUpdateScale);
	}


}

bool ApxObjBase::AddChild(ApxObjBase* pObj)
{
	if(pObj->m_pParent == this)
		return false;

	// we can not add my ancestor as my child...
	if(HasAncestor(pObj))
		return false;

	if(pObj->m_pParent)
		pObj->m_pParent->RemoveChild(pObj);

	pObj->SetParent(this);
	m_arrChildren.pushBack(pObj);
	
	OnAddChild(pObj);
	if(m_spOperationReport)
		m_spOperationReport->OnAddChildNotify(this, pObj);

	RaiseDirtyFlag(Dirty_Flag_AABB);

	return true;
}


bool ApxObjBase::RemoveChild(ApxObjBase* pObj)
{
	if(pObj->m_pParent != this)
		return false;

	m_arrChildren.deleteEntry(pObj);
	pObj->SetParent(NULL);

	OnRemoveChild(pObj);
	if(m_spOperationReport)
		m_spOperationReport->OnRemoveChildNotify(this, pObj);

	RaiseDirtyFlag(Dirty_Flag_AABB);
	return true;
}

void ApxObjBase::RemoveChildren()
{
	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->SetParent(NULL);

		OnRemoveChild(m_arrChildren[i]);
		if(m_spOperationReport)
			m_spOperationReport->OnRemoveChildNotify(this, m_arrChildren[i]);
	}

	m_arrChildren.clear();
	RaiseDirtyFlag(Dirty_Flag_AABB);
}

void ApxObjBase::ReleaseChildren()
{
	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->SetParent(NULL);
		if(m_arrChildren[i]->CheckFlag(Object_Flag_NoDeleteAsChild))
		{
			if(m_spOperationReport)
				m_spOperationReport->OnRemoveChildNotify(this, m_arrChildren[i]);
		}
		else
		{
			m_arrChildren[i]->Release();
			delete m_arrChildren[i];
		}
	}

	m_arrChildren.clear();
	
}

const NxBounds3& ApxObjBase::GetAABB() const
{
	if(CheckDirtyFlag(Dirty_Flag_AABB))
	{
		m_AABB.setEmpty();

		NxBounds3 objAABB;
		GetObjAABB(objAABB);
		m_AABB.combine(objAABB);

		for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
		{
			NxBounds3 childAABB;	
			m_AABB.combine(m_arrChildren[i]->GetAABB());
		}
		ClearDirtyFlag(Dirty_Flag_AABB);
	}
	return m_AABB;
}

bool ApxObjBase::GetOBB(NxBox& outObjOBB) const
{
	if (m_ObjOrigAABB.isEmpty())
		return false;

	m_ObjOrigAABB.getCenter(outObjOBB.center);
	outObjOBB.center = TransformLPos2GPos(outObjOBB.center);

	m_ObjOrigAABB.getExtents(outObjOBB.extents);
	outObjOBB.extents *= m_fGScale;

	GetGRot(outObjOBB.rot);
	return true;
}

bool ApxObjBase::GetObjAABB(NxBounds3& outObjAABB) const
{
	NxBox obb;
	if (GetOBB(obb))
	{
		outObjAABB.boundsOfOBB(obb.rot, obb.center, obb.extents);
		return true;
	}
	return false;
}

void ApxObjBase::Tick(float dt)
{
	if(CheckFlag(Object_Flag_UpdateTransform_Async))
		AsyncUpdateTransform();

	OnTick(dt);

	for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->Tick(dt);
	}

}

void ApxObjBase::AsyncUpdateTransform()
{

	if(CheckDirtyFlag(Dirty_Flag_GRot))
	{
		UpdateMyGRot();

		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGRot();

		ClearDirtyFlag(Dirty_Flag_GRot);
	}
	
	if(CheckDirtyFlag(Dirty_Flag_GPos))
	{
		UpdateMyGPos();

		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGPos();

		ClearDirtyFlag(Dirty_Flag_GPos);
	}

	if(CheckDirtyFlag(Dirty_Flag_GScale))
	{
		UpdateMyGScale();
		
		RaiseDirtyFlag(Dirty_Flag_AABB);
		OnUpdateGScale();

		ClearDirtyFlag(Dirty_Flag_GScale);
	}

	UpdateMyLTransform(CheckDirtyFlag(Dirty_Flag_LRot), CheckDirtyFlag(Dirty_Flag_LPos), CheckDirtyFlag(Dirty_Flag_LScale));
	ClearDirtyFlag(Dirty_Flag_LRot);
	ClearDirtyFlag(Dirty_Flag_LPos);
	ClearDirtyFlag(Dirty_Flag_LScale);
}

bool ApxObjBase::RayTraceAABB(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool bTraceChildren /*= true*/) const
{
	NxBounds3 aabbTest;
	if(bTraceChildren) 
		aabbTest = GetAABB();
	else
		GetObjAABB(aabbTest);

	PhysAABB aabb;
	aabb.vMin = APhysXConverter::N2A_Vector3(aabbTest.min);
	aabb.vMax = APhysXConverter::N2A_Vector3(aabbTest.max);

	return ::RayTraceAABB(ray, aabb, hitInfo);

}

bool ApxObjBase::RayTraceOBB(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool bTraceChildren /*= true*/) const
{
	NxBox nxOBB;
	GetOBB(nxOBB);
	
	PhysOBB obb;
	obb.Set(	APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(0)), 
				APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(1)), 
				APhysXConverter::N2A_Vector3(nxOBB.rot.getColumn(2)), 
				APhysXConverter::N2A_Vector3(nxOBB.center), 
				APhysXConverter::N2A_Vector3(nxOBB.extents)
			);

	void* bkUserData = hitInfo.UserData;
	if(::RayTraceOBB(ray, obb, hitInfo))
		hitInfo.UserData = (void*)this;

	if(bTraceChildren)
	{
		for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
		{
			m_arrChildren[i]->RayTraceOBB(ray, hitInfo);
		}
	}

	if (bkUserData == hitInfo.UserData)
		return false;
	return true;
}


ApxObjBase* ApxObjBase::GetCloned() const
{
	ApxObjBase* pCloned = NewObjAndCloneData();
	if(pCloned)
	{
		// clone ApxObjBase's data
		pCloned->m_fLScale = m_fLScale;
		pCloned->m_fGScale = m_fGScale;
			
		pCloned->m_qRotLToP = m_qRotLToP;
		pCloned->m_qRotLToG = m_qRotLToG;

		pCloned->m_mtRotLToP = m_mtRotLToP;
		pCloned->m_mtRotLToG = m_mtRotLToG;

		pCloned->m_vOffsetLToP = m_vOffsetLToP;
		pCloned->m_vOffsetLToG = m_vOffsetLToG;

		pCloned->m_uDirtyFlags = m_uDirtyFlags;
		pCloned->m_uFlags = m_uFlags;

		pCloned->mUserData = mUserData;

		pCloned->SetParent(NULL);

		for(APhysXU32 i=0; i<m_arrChildren.size(); i++)
		{
			pCloned->AddChild(m_arrChildren[i]->GetCloned());
		}

	}

	return pCloned;
}

void ApxObjBase::RaiseDirtyFlag(APhysXU32 flag) const
{
	APhysXFlagOperation::RaiseFlag(m_uDirtyFlags, flag);

	if(flag & Dirty_Flag_AABB)
	{
		ApxObjBase* pParent = GetParent();
		if(pParent)
		{
			pParent->RaiseDirtyFlag(Dirty_Flag_AABB);
		}
	}
}

bool ApxObjBase::HasAncestor(const ApxObjBase* pObj) const
{
	ApxObjBase* pParent = GetParent();
	while(pParent)
	{
		if(pParent == pObj) return true;
		pParent = pParent->GetParent();
	}
	
	return false;
}