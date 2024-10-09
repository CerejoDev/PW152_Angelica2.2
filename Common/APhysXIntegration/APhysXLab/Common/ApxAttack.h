/************************************************************************

 ApxAttack.h

	this file implement some stuff of PhysX attacking effect

	Created by:		He Wenfeng 
	Date:			Jan. 7, 2010

************************************************************************/

#include "A3DSkillGfxEvent.h"

class IPhysXObjBase;

namespace _SGC 
{

class ApxAttackEvent: public A3DSkillGfxEvent
{
public:
	
	ApxAttackEvent(A3DSkillGfxMan* pMan, GfxMoveMode mode):A3DSkillGfxEvent(pMan, mode)
	{

	}


protected:

	virtual void HitTarget();
};


class ApxAttackSkillGfxMan : public A3DSkillGfxMan
{

	friend class ApxAttackEvent;

public:
	ApxAttackSkillGfxMan() 
	{
		m_fForceMag = 0.0f;
		m_vHitPos.Clear();
		m_pHitTarget = NULL;
	}
	~ApxAttackSkillGfxMan() {}

	void SetHitForceMag(float fForceMag){ m_fForceMag = fForceMag;}
	float GetHitForceMag() { return m_fForceMag; }

	// Note: call this function before PlayAttackAction...
	void SetHitInfo(const A3DVECTOR3& vHitPos, IPhysXObjBase* pHitObj)
	{
		m_vHitPos = vHitPos;
		m_pHitTarget = pHitObj;
	}
	
	const A3DVECTOR3& GetHitPos() const
	{
		return m_vHitPos;
	}

protected:

	virtual A3DSkillGfxEvent* CreateOneEvent(GfxMoveMode mode);

	virtual bool GetPositionById(clientid_t nId, A3DVECTOR3& vPos, GfxHitPos HitPos, const char* szHook = NULL, bool bRelHook = false, const A3DVECTOR3* pOffset = NULL, const char* szHanger = NULL, bool bChildHook = false);
	virtual bool GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp);
	virtual float GetTargetScale(clientid_t nTargetId);

	float m_fForceMag;
	A3DVECTOR3 m_vHitPos;
	IPhysXObjBase* m_pHitTarget;
};

// extern A3DSkillGfxMan* AfxGetSkillGfxEventMan();

};	// end of namespace _SGC 
