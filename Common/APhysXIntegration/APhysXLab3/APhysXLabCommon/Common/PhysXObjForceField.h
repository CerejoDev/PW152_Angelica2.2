/*
 * FILE: PhysXObjForceField.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2012/07/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJFORCEFIELD_H_
#define _APHYSXLAB_PHYSXOBJFORCEFIELD_H_

#include "PhysXObjBase.h"

class CPhysXObjForceField : public IPhysXObjBase, public IPropPhysXObjBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000001; }

	static const TCHAR* GetForceFieldTypeText(const APhysXObjectType ffType);
	static bool GetForceFieldType(const TCHAR* pStr, APhysXObjectType& outFFType);
	static const TCHAR* GetFFCoordonateText(const NxForceFieldCoordinates ffc);
	static bool GetFFCoordonate(const TCHAR* pStr, NxForceFieldCoordinates& outFFC);
	static const TCHAR* GetFFScaleText(const NxForceFieldType ffst);
	static bool GetFFScale(const TCHAR* pStr, NxForceFieldType& outFFST);
	static bool GetShapeInfo(const APhysXForceFieldObjectDesc& ffDesc, CRegion::ShapeType& outST, A3DVECTOR3* poutVol = 0);
	static bool SetShapeInfo(const CRegion::ShapeType& st, APhysXForceFieldObjectDesc& ffDesc, NxVec3* pVol = 0);

	CPhysXObjForceField(int UID = -1, APhysXObjectType FFType = APX_OBJTYPE_FORCEFIELD);
	virtual ~CPhysXObjForceField();
	CPhysXObjForceField& operator= (const CPhysXObjForceField& rhs);

	const APhysXForceFieldObjectDesc* GetAPhysXFFDesc() const { return m_pAFFDesc; }
	void DrawAllFFShapes(A3DWireCollector& wc) const;
	bool OnDeletionNotify(APhysXObject& object);
	bool ImportAndExportFFData(bool import, const AString& strFile);
	bool SetShapeInfo(const CRegion::ShapeType& st, const A3DVECTOR3& vol);

//	void SetHostChannel(const APhysXCollisionChannel* pHostCNL = 0);
/*	bool ReloadModel();
	bool OnDeletionNotify(const APhysXObject& rObject);
	void SetState(bool toRest, bool IsRunning);
	void AttachTo(NxActor* pActor);

	virtual NxActor* GetFirstNxActor() const;
*///	virtual bool GetPhysXRigidBodyAABB(NxBounds3& outAABB) const { return false; /* unimplement*/ }
	virtual int  GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor = 0) const;
	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid();
	virtual bool OnPropertyChanged(PropItem& prop);
	virtual void OnUpdatePropsGrid();

protected:
	CPhysXObjForceField(const CPhysXObjForceField& rhs);
	virtual CPhysXObjForceField* CreateMyselfInstance(bool snapValues) const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePostNotify();
	virtual bool OnTickMove(float deltaTimeSec) { return true; }
	virtual bool OnTickAnimation(const unsigned long deltaTime) { return true; }
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime);

	virtual bool OnInstancePhysXObj(APhysXScene& aPhysXScene);
	virtual void OnReleasePhysXObj();
	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale) { return false; }

private:
	void SetDefaultOBB();
	void CopyAPhysXObjectDescManager(const APhysXObjectDescManager& rhs);
	bool OnPropertyChangedFFCustom(const PropItem& prop);

	virtual CPhysXObjForceField* NewObjAndCloneData() const;
	virtual void OnDoSleepTask();
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

	virtual bool GetAPhysXInstanceAABB(NxBounds3& outAABB) const;
	virtual bool OnLoadModel(IEngineUtility& eu, const char* szFile);
	virtual void OnReleaseModel();
	virtual bool SyncDataPhysXToGraphicMyself() { return true; }
	virtual bool OnGetModelAABB(A3DAABB& outAABB) const;

private:
	bool			m_bEnableDraw;
	int				m_DrawColor;
	FFRenderable	m_FFRender;
	A3DOBB			m_EditorOBB;
	APhysXObjectDescManager		m_objDescMgr;
	APhysXForceFieldObjectDesc* m_pAFFDesc;
	APhysXForceFieldObject*		m_pAFFObj;
	NxForceField*				m_pNxFF;
};

#endif