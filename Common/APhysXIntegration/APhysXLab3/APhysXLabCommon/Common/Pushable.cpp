#include "Stdafx.h"
#include "Pushable.h"

#include "Scene.h"

A3DVECTOR3 Pushable::s_v1(0.0f, 0.0f, 0.0f);
A3DVECTOR3 Pushable::s_v2(0.0f, 0.0f, 0.0f);
A3DVECTOR3 Pushable::s_v1Normal(0.0f, 0.0f, 0.0f);
A3DVECTOR3 Pushable::s_v2Normal(0.0f, 0.0f, 0.0f);

void Pushable::RenderTest(A3DWireCollector* pWC)
{
	pWC->AddSphere(s_v1, 0.05f, 0x80800000);
	pWC->AddSphere(s_v2, 0.05f, 0x80800000);

	pWC->Add3DLine(s_v1, s_v1Normal, 0x80800000);
	pWC->Add3DLine(s_v2, s_v2Normal, 0x80800000);
}
Pushable::Pushable()
{
	Release();
}
Pushable::~Pushable()
{
	Release();
}

void Pushable::Init(IPhysXObjBase* pObj, Scene* pScene)
{
	m_pPhysXObj = pObj;
	m_pScene = pScene;
}
void Pushable::Release()
{
	m_pPhysXObj = NULL;
	m_pScene = NULL;
	m_pPorter = NULL;

	m_pLBone = NULL;
	m_pRBone = NULL;
}

void Pushable::ReleasePush()
{
	Release();
}

bool Pushable::TryToPush(A3DSkinModel* pModel, A3DBone* pLHand, A3DBone* pRHand, const char* strAnimation, 
			   float fBindFrameRatio)
{
	A3DSkinModelActionCore* pAction = pModel->GetAction(strAnimation);
	if (0 == pAction)
		return false;

	int iFrame = (int)((pAction->GetEndFrame() - pAction->GetStartFrame()) * fBindFrameRatio);

	A3DVECTOR3 vLShoulderPos, vRShoulderPos;
	A3DMATRIX4 matLShoulder, matRShoulder;
	if (GetBoneGPoseAtFrame(pModel, strAnimation, pLHand->GetParentPtr()->GetParentPtr()->GetName(), iFrame, matLShoulder))
		vLShoulderPos = Mat44_GetTransPart(matLShoulder);
	else
		vLShoulderPos = Mat44_GetTransPart(pLHand->GetParentPtr()->GetParentPtr()->GetAbsoluteTM());
	if (GetBoneGPoseAtFrame(pModel, strAnimation, pRHand->GetParentPtr()->GetParentPtr()->GetName(), iFrame, matRShoulder))
		vRShoulderPos = Mat44_GetTransPart(matRShoulder);
	else
		vRShoulderPos = Mat44_GetTransPart(pRHand->GetParentPtr()->GetParentPtr()->GetAbsoluteTM());

	//calculate arm length
	float fUpperArmLen, fLowerArmLen;
	A3DBone* pArm = pLHand->GetParentPtr();
	A3DBone* pShoulder = pArm->GetParentPtr();
	A3DVECTOR3 vDelta = Mat44_GetTransPart(pLHand->GetAbsoluteTM()) - Mat44_GetTransPart(pArm->GetAbsoluteTM());
	fLowerArmLen = vDelta.Magnitude();
	vDelta = Mat44_GetTransPart(pArm->GetAbsoluteTM()) - Mat44_GetTransPart(pShoulder->GetAbsoluteTM());
	fUpperArmLen = vDelta.Magnitude();

	m_fArmLen = fUpperArmLen + fLowerArmLen;
	//do capsule sweep
	NxCapsule capsule;
	capsule.radius = 0.01f;
	A3DVECTOR3 vLeft, vRight, vLeftNormal, vRightNormal;
	bool bLeft = false, bRight = false;
	A3DVECTOR3 vDir = pModel->GetDir();
	vDir.y = 0; 
	vDir.Normalize();
	vDelta = vDir * (m_fArmLen + 0.1f);
	NxSweepQueryHit Hit;

	for (int nTry = 0; nTry < 12; nTry++)
	{
		if (!bLeft)
		{
			capsule.p1 = capsule.p0 = APhysXConverter::A2N_Vector3(vLShoulderPos - vDir * 0.1f);
			capsule.p1.y = capsule.p0.y = capsule.p0.y - nTry * capsule.radius;
			if (LinearCapsuleSweep(capsule, APhysXConverter::A2N_Vector3(vDelta), Hit))
			{
				vLeft = APhysXConverter::N2A_Vector3(Hit.point);
				vLeftNormal = APhysXConverter::N2A_Vector3(Hit.normal);

				//validate?
				if ((vLeft - vLShoulderPos).Magnitude() < m_fArmLen)
					bLeft = true;
			}
		}
		if (!bRight)
		{
			capsule.p1 = capsule.p0 = APhysXConverter::A2N_Vector3(vRShoulderPos - vDir * 0.1f);
			capsule.p1.y = capsule.p0.y = capsule.p0.y - nTry * capsule.radius;

			if (LinearCapsuleSweep(capsule, APhysXConverter::A2N_Vector3(vDelta), Hit))
			{
				vRight = APhysXConverter::N2A_Vector3(Hit.point);
				vRightNormal = APhysXConverter::N2A_Vector3(Hit.normal);

				//validate?
				if ((vRight - vRShoulderPos).Magnitude() < m_fArmLen)
					bRight = true;
			}
		}
		if (bLeft && bRight)
			break;
	}

	if (bLeft && bRight)
	{
		m_pPorter = pModel;
		m_pLBone = pLHand;
		m_pRBone = pRHand;

		s_v1Normal = vLeft;//m_lBindDirWorld;
		s_v2Normal = vRight;//m_rBindDirWorld;

		//calculate local position
		const float fPalmOffSet = 0.03f;
		vDelta = vDir * fPalmOffSet;
		
		m_lBindWorld = vLeft - vDelta;
		m_lBindDirWorld = -vLeftNormal;
		m_rBindWorld = vRight - vDelta;
		m_rBindDirWorld = -vRightNormal;

		A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
		mat = pModel->GetAbsoluteTM();
		//m_pPhysXObj->GetGPose(mat);
		mat.InverseTM();
		m_lBindLocal = m_lBindWorld * mat;
		m_rBindLocal = m_rBindWorld * mat;
		mat._41 = mat._42 = mat._43 = 0;
		m_lBindDirLocal = m_lBindDirWorld * mat;
		m_rBindDirLocal = m_rBindDirWorld * mat;

		s_v1 = m_lBindWorld;
		s_v2 = m_rBindWorld;
		s_v1 = vLShoulderPos;
		s_v2 = vRShoulderPos;

		

		return true;
	}
	return false;
}

void Pushable::UpdateBindPosDir()
{
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	//m_pPhysXObj->GetGPose(mat);
	if (m_pPorter)
	{
		mat = m_pPorter->GetAbsoluteTM();
		m_lBindWorld = m_lBindLocal * mat;
		m_rBindWorld = m_rBindLocal * mat;
		mat._41 = mat._42 = mat._43 = 0;
		m_lBindDirWorld = m_lBindDirLocal * mat;
		m_rBindDirWorld = m_rBindDirLocal * mat;

		//调整
		A3DVECTOR3 vLShoulder = Mat44_GetTransPart(m_pLBone->GetParentPtr()->GetParentPtr()->GetAbsoluteTM());
		A3DVECTOR3 vRShoulder = Mat44_GetTransPart(m_pRBone->GetParentPtr()->GetParentPtr()->GetAbsoluteTM());
		A3DVECTOR3 vLHand = m_lBindWorld;
		A3DVECTOR3 vRHand = m_rBindWorld;

		A3DVECTOR3 vLDelta = m_pPorter->GetDir()*(vLShoulder - vLHand).MagnitudeH();
		A3DVECTOR3 vRDelta = m_pPorter->GetDir()*(vRShoulder - vRHand).MagnitudeH();
		
		const float fPalmOffSet = 0.03f;
		NxCapsule capsule;
		capsule.radius = 0.01f;
		capsule.p0 = capsule.p1 = APhysXConverter::A2N_Vector3(vLHand - vLDelta);
		//s_v1 = vLHand - vLDelta;		
		////vLDelta = m_pPorter->GetDir()*m_fArmLen;
		//s_v1Normal = vLHand + vLDelta;
		NxSweepQueryHit Hit;
		if (LinearCapsuleSweep(capsule, APhysXConverter::A2N_Vector3(vLDelta), Hit))
		{
			m_lBindWorld = APhysXConverter::N2A_Vector3(Hit.point) - m_pPorter->GetDir() * fPalmOffSet;
			m_lBindDirWorld = -APhysXConverter::N2A_Vector3(Hit.normal);
		}

		capsule.p0 = capsule.p1 = APhysXConverter::A2N_Vector3(vRHand - vRDelta);
		//s_v2 = vRHand - vRDelta;
		////vRDelta = m_pPorter->GetDir()*m_fArmLen;
		//s_v2Normal = vRHand + vRDelta;
		if (LinearCapsuleSweep(capsule, APhysXConverter::A2N_Vector3(vRDelta), Hit))
		{
			m_rBindWorld = APhysXConverter::N2A_Vector3(Hit.point) - m_pPorter->GetDir() * fPalmOffSet;
			m_rBindDirWorld = -APhysXConverter::N2A_Vector3(Hit.normal);
		}		
	}
}

bool Pushable::GetObjAABB(A3DAABB& aabb) const
{
	NxBounds3 ext;
	if (m_pPhysXObj && m_pPhysXObj->GetObjAABB(ext))
	{
		NxVec3 vCenter, vExt;
		ext.getCenter(vCenter);
		ext.getExtents(vExt);
		aabb.Center = APhysXConverter::N2A_Vector3(vCenter);
		aabb.Extents = APhysXConverter::N2A_Vector3(vExt);
		aabb.CompleteMinsMaxs();
		return true;
	}
	return false;
}

//get bone global pose at specific frame of  action
bool Pushable::GetBoneGPoseAtFrame(A3DSkinModel* pModel, const char* strAnimation, const char* szBoneName, int iFrame, A3DMATRIX4& matOut)
{
	bool bRet = false;
	if (strAnimation != NULL)
	{
		A3DSkinModelActionCore* pAction = pModel->GetAction(strAnimation);
		if (pAction)
		{
			bRet = pModel->GetBoneStateAtFrame(strAnimation, iFrame, szBoneName, matOut);
			if (bRet)
				matOut = matOut * pModel->GetAbsoluteTM();
		}			
	}
	return bRet;

	//int index;
	//A3DBone* pBone = pModel->GetSkeleton()->GetBone(szBoneName, &index);
	//matOut = pBone->GetAbsoluteTM();
	//return true;
}

//sweep the pushable box
bool Pushable::LinearCapsuleSweep(const NxCapsule& worldcapsule, const NxVec3& motion, NxSweepQueryHit& Hit) const
{
	APhysXScene* pPhysXScene = m_pScene->GetAPhysXScene();
	pPhysXScene->LockWritingScene();

	//set special group id to the object
	APtrArray<NxActor*> actors;
	m_pPhysXObj->GetNxActors(actors);
	abase::vector<NxCollisionGroup> cflags;
	for (int i = 0; i < actors.GetSize(); i++)
	{
		NxU32 num = actors[i]->getNbShapes();
		NxShape*const *ppShape = actors[i]->getShapes();
		for ( NxU32 j = 0; j < num; j++)
		{
			cflags.push_back(ppShape[j]->getGroup());//back up old group;
			ppShape[j]->setGroup(31);//new collision group 31
		}

	}

	//do sweep
	int nHit = pPhysXScene->GetNxScene()->linearCapsuleSweep(worldcapsule, motion, NX_SF_STATICS|NX_SF_DYNAMICS,
		NULL, 1, &Hit, NULL, 1<<31);

	//restore old group
	int indexFlag = 0;
	for (int i = 0; i < actors.GetSize(); i++)
	{
		NxU32 num = actors[i]->getNbShapes();
		NxShape*const *ppShape = actors[i]->getShapes();
		for ( NxU32 j = 0; j < num; j++)
		{
			ppShape[j]->setGroup(cflags[indexFlag++]);//
		}

	}

	pPhysXScene->UnlockWritingScene();

	return nHit != 0;
}

//获取绑定时世界坐标系下左右手的位置
bool Pushable::GetBindPosWorld(A3DVECTOR3& vLBindPos, A3DVECTOR3& vRBindPos) const
{
	if (m_pPhysXObj)
	{
		vLBindPos = m_lBindWorld;
		vRBindPos = m_rBindWorld;

		return true;
	}
	return false;
}
//获取绑定时世界坐标系下左右手的朝向
bool Pushable::GetBindDirWorld(A3DVECTOR3& vLBindDir, A3DVECTOR3& vRBindDir) const
{
	if (m_pPhysXObj )
	{
		vLBindDir = m_lBindDirWorld;
		vRBindDir = m_rBindDirWorld;

		return true;
	}
	return false;
}