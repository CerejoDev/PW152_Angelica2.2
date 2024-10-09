/*
 * FILE: IObjBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 20011/05/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_IOBJBASE_H_
#define _APHYSXLAB_IOBJBASE_H_

class IPropObjBase;

class IObjBase: public IRenderable
{
public:
	IObjBase();
	virtual ~IObjBase() {};

	void SetMgrData(IObjMgrData* omd) { m_pOMD = omd; }
	IObjMgrData* GetMgrData() const { return m_pOMD; } 

	IPropObjBase* GetProperties() const { return m_pProperties; }
	IPropObjBase* CreatePropSnap() const;

	virtual bool SaveSerialize(NxStream& stream) const;
	virtual bool LoadDeserialize(NxStream& stream, bool& outIsLowVersion);
 
	virtual void ToDie(const bool IsAsync = false) = 0;
	virtual bool Revive(IEngineUtility& eu) = 0;

	/* not supported if pure PhysX driven mode using in runtime */
	virtual bool GetOBB(A3DOBB& outOBB) const = 0;
	virtual bool GetAABB(A3DAABB& outAABB) const = 0;

	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const = 0;
	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck /*= true*/) const = 0;
	virtual void RenderExtraData(const IRenderUtility& renderUtil) const = 0;

protected:
	virtual void SetProperties(IPropObjBase& prop) { m_pProperties = &prop; }
	virtual void ApplyPose(bool bRuntimePose) = 0;

	virtual bool OnResetPose() = 0;
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const = 0;
	virtual void OnSetPos(const A3DVECTOR3& vPos) = 0;
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const = 0;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const = 0;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) = 0;
	virtual bool OnGetPose(A3DMATRIX4& matOutPose) const = 0;
	virtual void OnSetPose(const A3DMATRIX4& matPose) = 0;
	virtual bool OnSetScale(const float& scale) = 0;

private:
	friend class IPropObjBase;

	// forbidden behavior
	IObjBase(const IObjBase& rhs);
	IObjBase& operator= (const IObjBase& rhs);

private:
	IObjMgrData* m_pOMD;
	IPropObjBase* m_pProperties;
};

#endif
