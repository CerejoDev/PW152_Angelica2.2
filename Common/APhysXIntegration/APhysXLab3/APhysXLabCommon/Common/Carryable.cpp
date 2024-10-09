#include "StdAfx.h"
#include "Carryable.h"
#include <A3DViewport.h>
#include <A3DSkinModel.h>

A3DVECTOR3 CCarryable::s_vFakeStart = A3DVECTOR3(0, 0, 0);
A3DVECTOR3 CCarryable::s_vFakeEnd = A3DVECTOR3(0, 0, 0);
A3DVECTOR3 CCarryable::s_motion = A3DVECTOR3(0, 0, 0);
A3DAABB    CCarryable::s_aabb = A3DAABB(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0));
A3DMATRIX4 CCarryable::s_mat = A3DMATRIX4(A3DMATRIX4::IDENTITY);
CCoordinateDirection CCarryable::s_coordinate; 
IPhysXObjBase*       CCarryable::s_pPhysX = NULL;
A3DOBB               CCarryable::s_obb;
A3DVECTOR3 CCarryable::s_vLeft1;
A3DVECTOR3 CCarryable::s_vRight1;
A3DVECTOR3 CCarryable::s_vLeft2;
A3DVECTOR3 CCarryable::s_vRight2;

void CCarryable::RenderTest(A3DWireCollector* pWC)
{
	pWC->Add3DLine(s_vFakeStart, s_vFakeEnd, 0xff800000);
	//pWC->Add3DLine(s_vFakeStart, s_motion + s_vFakeStart, 0xff008000);
	//
	
	//pWC->AddAABB(s_aabb, 0xff800000, &s_mat);
	if (s_coordinate.g_Render != NULL && s_pPhysX)
	{
		A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
		s_coordinate.SetPos(APhysXConverter::N2A_Vector3(s_pPhysX->GetGPos()));

		s_pPhysX->GetGPose(mat);
		Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), mat);
		s_coordinate.SetRotate(mat);
		s_coordinate.Draw(*pWC);

		//for (int i = 0; i < s_pPhysX->GetChildNum(); i++)
		//{
		//	mat.Identity();
		//	s_pPhysX->GetChild(i)->GetGPose(mat);
		//	s_coordinate.SetPos(mat.GetTransPart());
		//	mat.SetTransPart(A3DVECTOR3(0, 0, 0));
		//	s_coordinate.SetRotate(mat);
		//	s_coordinate.Draw(*pWC);
		//	
		//}

		NxBox box;
		s_pPhysX->GetOBB(box);
		A3DOBB obb;
		obb.Center = APhysXConverter::N2A_Vector3(box.center);
		obb.Extents = APhysXConverter::N2A_Vector3(box.extents);
		obb.XAxis = APhysXConverter::N2A_Vector3(box.rot.getColumn(0));
		obb.YAxis = APhysXConverter::N2A_Vector3(box.rot.getColumn(1));
		obb.ZAxis = APhysXConverter::N2A_Vector3(box.rot.getColumn(2));
		obb.XAxis.Normalize();
		obb.YAxis.Normalize();
		obb.ZAxis.Normalize();
		obb.CompleteExtAxis();

		//pWC->Add3DLine(s_vLeft1, s_vRight1, 0xff008000);
		//pWC->Add3DLine(s_vLeft2, s_vRight2, 0xff008000);

		//pWC->Add3DLine(s_vLeft1, s_vLeft1 + s_motion, 0xff000080);

		pWC->AddOBB(obb, 0xff800000);
	}
}

CCarryable::CCarryable(void)
{
	Release();
}

CCarryable::~CCarryable(void)
{
	Release();
}

void CCarryable::Release()
{
	m_pBindBone = NULL;
	m_pPhysXObj = NULL;
	m_pScene = NULL;
	m_state = Free;
	m_pPorter = NULL;

	m_pLBindObj = NULL;
	m_pRBindObj = NULL;

	m_bUpdateBind = false;
}

bool CCarryable::Init(IPhysXObjBase* pPhysXObj, Scene* pScene)
{
	assert(pPhysXObj);
	if (pPhysXObj == NULL)
		return false;

	Release();

	m_pPhysXObj = pPhysXObj;
	m_pScene = pScene;
	s_coordinate.g_Render = pScene->GetRender();
	s_pPhysX = pPhysXObj;

	return true;
}

bool CCarryable::Load(const char* szFile)
{
	Release();
		
	return true;
}

void CCarryable::Tick(unsigned int uTimeDelta)
{
	if (m_pPhysXObj)
	{
		//////////////////////////////////////////////////////////////////////////
		//m_pECModel->Tick(uTimeDelta);		//ObjManger will tick it

		if (m_state == ToFree)
		{
			//在此箱子移动已经自由，不受绑定骨骼约束
			
			

			
			m_dwTransCnt += uTimeDelta;
			if (m_dwPreTransTime > 0)
			{
				if (m_dwTransCnt >= m_dwPreTransTime)
				{
					m_dwTransCnt = 0;
					m_dwPreTransTime = 0;
				}
			}
			else if (m_dwTransCnt >= m_dwTransTime)
			{
				m_pBindBone = NULL;//绑定骨骼置为空
				m_pPorter = NULL;
				m_state = Free;
				m_dwTransCnt = 0;
				OnStateChanged(ToFree, m_state);
			}
		}
		else if (m_state == ToHold)
		{
			//移动箱子到绑定位置
			m_dwTransCnt += uTimeDelta;
			float fFactor;
			if (m_dwTransTime != 0)
			   fFactor = (float)m_dwTransCnt / m_dwTransTime;
			else 
				fFactor = 1;//m_dwTransTime == 0, 一帧移动到指定位置

			a_Clamp(fFactor, 0.f, 1.f);
			A3DVECTOR3 newpos = Mat44_GetTransPart(m_matInWorld) * fFactor + m_vPosFreBind * (1 - fFactor);
			SetPos(newpos);
			A3DQUATERNION quat;
			quat = SLERPQuad(m_quatPreBind, m_quatInWorld, fFactor);
			quat.Normalize();
			A3DMATRIX4 mat;
			quat.ConvertToMatrix(mat);
			SetDirAndUp(mat.GetRow(2), mat.GetRow(1));
			if (m_dwTransCnt >= m_dwTransTime)
			{
				m_state = Hold;
				m_dwTransCnt = 0;
				OnStateChanged(ToHold, m_state);
			}
		}
		m_dwTickDelta = uTimeDelta;
		m_bUpdateBind = false;
	}
}

bool CCarryable::GetPos(A3DVECTOR3& vPos) const
{
	if (m_pPhysXObj)
	{
		vPos = APhysXConverter::N2A_Vector3(m_pPhysXObj->GetGPos());
		return true;
	}
	return false;
}
void CCarryable::SetPos(const A3DVECTOR3& vPos)
{
	if (m_pPhysXObj)
	{
		m_pPhysXObj->SetGPos(vPos);
	}
}
bool CCarryable::GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const
{
	if (m_pPhysXObj)
	{
		m_pPhysXObj->GetGDirAndUp(vDir, vUp);		
		return true;
	}
	return false;
}
void CCarryable::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (m_pPhysXObj)
	{
		m_pPhysXObj->SetGDirAndUp(vDir, vUp);		
	}
}

bool CCarryable::GetPhysXAABB(A3DAABB& aabb) const
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

bool CCarryable::GetOBB(A3DOBB& obb) const
{
	NxBox box;
	if (m_pPhysXObj && m_pPhysXObj->GetOBB(box))
	{
		obb.Center = APhysXConverter::N2A_Vector3(box.center);
		obb.Extents = APhysXConverter::N2A_Vector3(box.extents);
		obb.XAxis = APhysXConverter::N2A_Vector3(box.rot.getColumn(0));
		obb.YAxis = APhysXConverter::N2A_Vector3(box.rot.getColumn(1));
		obb.ZAxis = APhysXConverter::N2A_Vector3(box.rot.getColumn(2));
		obb.XAxis.Normalize();
		obb.YAxis.Normalize();
		obb.ZAxis.Normalize();

		obb.CompleteExtAxis();
		return true;
	}
	return false;
}

bool CCarryable::GetModelAABB(A3DAABB& aabb) const
{
	if (m_pPhysXObj && m_pPhysXObj->GetModelAABB(aabb))
	{
		aabb.Extents *= m_pPhysXObj->GetGScale();
		aabb.CompleteMinsMaxs();
		return true;
	}
	return false;
}

bool CCarryable::DetectHandle(A3DVECTOR3& vStart, A3DVECTOR3& vEnd, A3DVECTOR3& vStartNormal, A3DVECTOR3& vEndNormal, A3DMATRIX4* pMat)
{
	if (m_pPhysXObj == NULL)
		return false;

	vStartNormal.Clear();
	vEndNormal.Clear();

	const float fPalmOffSet = 0.03f;

	
	if (m_pPhysXObj)
	{
		A3DMATRIX4 RotMat;

		A3DVECTOR3 vFakeStart(vStart), vFakeEnd(vEnd);		
		if (pMat)
		{
			A3DVECTOR3 vLStart, vLEnd;//local position
			
			RotMat = *pMat;
			RotMat.InverseTM();
			vLStart = vStart * RotMat;
			vLEnd = vEnd * RotMat;
						
			RotMat = GetAbsPose();

			vFakeStart = vLStart * RotMat;
			vFakeEnd = vLEnd * RotMat;
		}
		//s_vFakeStart = vFakeStart;
		//s_vFakeEnd = vFakeEnd;

		//ray trace object
		PhysRay ray;
		PhysRayTraceHit traceHit;
		ray.vStart = vFakeStart, ray.vDelta = vFakeEnd - vFakeStart;
		if (RayTrace(ray, traceHit))
		{
			vStart = traceHit.vHit;
			vStartNormal = traceHit.vNormal;
			ray.vStart = vFakeEnd;
			ray.vDelta = -ray.vDelta;
			if (RayTrace(ray, traceHit))
			{
				vEnd = traceHit.vHit;
				vEndNormal = traceHit.vNormal;
				
				//convert the hit position
				A3DVECTOR3 vLStart, vLEnd, vLStartNormal, vLEndNormal;//local position
				RotMat = GetAbsPose();
				RotMat.InverseTM();
				vLStart = vStart * RotMat;
				vLEnd = vEnd * RotMat;
				Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), RotMat);
				vLStartNormal = vStartNormal * RotMat;
				vLEndNormal = vEndNormal * RotMat;
				
				RotMat = *pMat;
				vStart = vLStart * RotMat;
				vEnd = vLEnd * RotMat;
				Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), RotMat);
				vStartNormal = vLStartNormal * RotMat;
				vEndNormal = vLEndNormal * RotMat;
				vStartNormal.Normalize();
				vEndNormal.Normalize();
				//considering offset
				A3DVECTOR3 vDelta = vEnd - vStart;
				vDelta.Normalize();
				vStart = vStart - vDelta * fPalmOffSet;
				vEnd = vEnd + vDelta * fPalmOffSet;

				return true;
			}
		}
		return false;
		
	}
	
	return false;
}

void CCarryable::CalPhysXEdge(A3DOBB obb)
{
	//aabb近面及远面到物理实体的距离
	m_fDistFarEdge = 0; 
	m_fDistNearEdge = 0;

	//compute near and far edge
	NxCapsule capsule;
	capsule.radius = 0.05f;
	A3DVECTOR3 vExpand(obb.Extents);
	obb.Extents += vExpand;
	obb.CompleteExtAxis();

	A3DMATRIX4 matRot = GetAbsPose(false);
	A3DMATRIX4 matRotNoTrans(matRot);
	Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), matRotNoTrans);
	A3DVECTOR3 vCenter = obb.Center;

	A3DVECTOR3 vLeft = -obb.ExtX + vCenter - obb.ExtZ;
	A3DVECTOR3 vRight = obb.ExtX + vCenter - obb.ExtZ;

	capsule.p0 = APhysXConverter::A2N_Vector3(vLeft);
	capsule.p1 = APhysXConverter::A2N_Vector3(vRight);
	A3DVECTOR3 motion = 2 * obb.ExtZ;
	s_vLeft1 = vLeft, s_vRight1 = vRight;
	NxSweepQueryHit Hit;
	if (LinearCapsuleSweep(capsule, APhysXConverter::A2N_Vector3(motion), Hit))
	{
		m_fDistNearEdge = Hit.t * obb.Extents.z * 2 - vExpand.z + capsule.radius;
	}

	vLeft = -obb.ExtX + vCenter + obb.ExtZ;
	vRight = obb.ExtX + vCenter + obb.ExtZ;
	s_vLeft2 = vLeft, s_vRight2 = vRight, s_motion = motion;
	capsule.p0 = APhysXConverter::A2N_Vector3(vLeft);
	capsule.p1 = APhysXConverter::A2N_Vector3(vRight);
	if (LinearCapsuleSweep(capsule, APhysXConverter::A2N_Vector3(-motion), Hit))
	{
		m_fDistFarEdge = Hit.t * obb.Extents.z * 2 - vExpand.z + capsule.radius;
	}
}

void CCarryable::CalBindMat(A3DSkinModel* pModel, const TCHAR* strAnimation, int iFrame)
{
	if (m_pPhysXObj)
	{
		m_matBind.Identity();
		Mat44_SetTransPart(A3DVECTOR3(-0.4f, 0, 0.3f), m_matBind);

#if 0   //保持原有姿态不旋转
		A3DMATRIX4 mat;
		
		mat = m_pBindBone->GetBoneInitTM().GetInverseTM() * pModel->GetAbsoluteTM();//
		
		A3DMATRIX4 MatWorld = GetAbsPose();//箱子坐标系

		A3DMATRIX4 Absmat = m_matBind * mat;
		MatWorld.SetTransPart(Absmat.GetTransPart());
		m_matInBind = MatWorld * Absmat.GetInverseTM();

#else   //按照m_matBind计算的得到的姿态，

		//给出绑定后与人的相对关系
		A3DMATRIX4 matInPerson;
		matInPerson.Identity();
		A3DOBB obb;
		GetOBB(obb);
		s_obb = obb;
		
		float fOffCenter = 0;
		
#ifdef _ANGELICA3		
		float fHeight = 0.8f * pModel->GetBoneAABB().Extents.y * 2;// - vCenterInModel.y;
#else
		float fHeight = 0.8f * pModel->GetModelAABB().Extents.y * 2;
#endif
		if (m_pLBindObj == NULL || m_pLBindObj == NULL)
		{
			//aabb近面及远面到物理实体的距离
			CalPhysXEdge(obb);
			fOffCenter = (m_fDistFarEdge + m_fDistNearEdge) / 2;
			A3DMATRIX4 matRot = GetAbsPose(false);
			A3DVECTOR3 vCenterInModel = obb.Center * matRot.GetInverseTM();//obb center in model coordinate
			fHeight -= vCenterInModel.y;
		}
		else
		{
			//获取挂点的高度
			fHeight -= APhysXConverter::N2A_Vector3(m_pLBindObj->GetLPos()).y * m_pLBindObj->GetParent()->GetGScale() + 0.1f;
			fOffCenter = 0.05f;
		}

		Mat44_SetTransPart(A3DVECTOR3(0, fHeight, obb.Extents.z - fOffCenter + 0.15f), matInPerson);

		//计算绑定关系的世界坐标系
		A3DMATRIX4 matWorld = matInPerson * pModel->GetAbsoluteTM();
		A3DMATRIX4 matScale;
		matScale.Scale(m_pPhysXObj->GetGScale(), m_pPhysXObj->GetGScale(), m_pPhysXObj->GetGScale());
		matWorld = matScale * matWorld;

		A3DMATRIX4 mat = m_pBindBone->GetBoneInitTM().GetInverseTM() * pModel->GetAbsoluteTM(); //use init pose m_pBindBone->GetAbsoluteTM();
		
		mat = m_matBind * mat;
		
		m_matInBind = matWorld * mat.GetInverseTM();

#endif
		mat = m_pBindBone->GetAbsoluteTM();
		A3DMATRIX4 matOut;
		if (GetBoneGPoseAtFrame(pModel, _TWC2AS(strAnimation), m_pBindBone->GetName(), iFrame, matOut))
			mat = matOut;
		
		m_matInWorld = m_matInBind * m_matBind * mat;
	}
}

bool CCarryable::GetBoneGPoseAtFrame(A3DSkinModel* pModel, const char* strAnimation, const char* szBoneName, int iFrame, A3DMATRIX4& matOut)
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
}

//获取绑定点世界坐标系下的位置
bool CCarryable::GetBindPosWorld(A3DVECTOR3& vLBindPos, A3DVECTOR3& vRBindPos) const
{
	if (m_pPhysXObj && m_pBindBone)
	{
		vLBindPos = m_lBindWorld;
		vRBindPos = m_rBindWorld;

		return true;
	}
	return false;
}

//获取绑定时世界坐标系下左右手的朝向
bool CCarryable::GetBindDirWorld(A3DVECTOR3& vLBindDir, A3DVECTOR3& vRBindDir) const
{
	if (m_pPhysXObj && m_pBindBone)
	{
		vLBindDir = m_lBindDirWorld;
		vRBindDir = m_rBindDirWorld;

		return true;
	}
	return false;
}

//与骨骼绑定
void CCarryable::BindToBone(A3DBone* pLHandBone, A3DBone* pRHandBone)
{
	//bind the box to their ancestor bone
	A3DBone* pLBone = pLHandBone;
	A3DBone* pRBone = pRHandBone;
	while (pLBone != pRBone)
	{
		pLBone = pLBone->GetParentPtr();
		pRBone = pRBone->GetParentPtr();
	}
	m_pBindBone = pLBone->GetParentPtr();

}

bool CCarryable::GetUserDefHandle(const TCHAR* strLChild, const TCHAR* strRChild)
{
	m_pLBindObj = m_pPhysXObj->GetChildByLocalName(strLChild);	
	m_pRBindObj = m_pPhysXObj->GetChildByLocalName(strRChild);
	
	return m_pLBindObj && m_pRBindObj;
}

bool CCarryable::TryToCarry(A3DSkinModel* pModel, A3DBone* pLHand, A3DBone* pRHand, const TCHAR* strAnimation, float fBindFrameRatio, const TCHAR* strLChild, const TCHAR* strRChild)
{
	if (!isEmpty() && m_state == Free)
	{
		GetUserDefHandle(strLChild, strRChild);

		BindToBone(pLHand, pRHand);
		//设置相对绑定骨骼的坐标系
		A3DSkinModelActionCore* pAction = pModel->GetAction(_TWC2AS(strAnimation));
		int iFrame = (int)((pAction->GetEndFrame() - pAction->GetStartFrame()) * fBindFrameRatio);
		CalBindMat(pModel, strAnimation, iFrame);

		const A3DVECTOR3 g_YAxis(0, 1, 0);
		A3DVECTOR3 vDir = pModel->GetDir();
		vDir.y = 0;
		vDir.Normalize();		

		A3DVECTOR3 vLShoulderPos, vRShoulderPos;
		A3DMATRIX4 matLShoulder, matRShoulder;
		if (GetBoneGPoseAtFrame(pModel, _TWC2AS(strAnimation), pLHand->GetParentPtr()->GetParentPtr()->GetName(), iFrame, matLShoulder))
			vLShoulderPos = Mat44_GetTransPart(matLShoulder);
		else
			vLShoulderPos = Mat44_GetTransPart(pLHand->GetParentPtr()->GetParentPtr()->GetAbsoluteTM());
		if (GetBoneGPoseAtFrame(pModel, _TWC2AS(strAnimation), pRHand->GetParentPtr()->GetParentPtr()->GetName(), iFrame, matRShoulder))
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
				
		vDelta = vLShoulderPos - vRShoulderPos;
		float fHalfExt = vDelta.MagnitudeH() / 2 + fUpperArmLen + fLowerArmLen;
		A3DVECTOR3 vLeft = CrossProduct(vDir, g_YAxis);

		A3DAABB aabb;
		
		GetModelAABB(aabb);
		A3DOBB obb;
		GetOBB(obb);
		A3DMATRIX4 matAbs = GetAbsPose();
		s_aabb = aabb;
		A3DVECTOR3 vRigidCenter = obb.Center + obb.ZAxis * ((m_fDistNearEdge - m_fDistFarEdge) / 2);
		vRigidCenter = vRigidCenter * matAbs.GetInverseTM() * m_matInWorld;//according to getmodelaabb

		const float fThresh = -0.9f;
		float fSqureAB = fUpperArmLen * fUpperArmLen + fLowerArmLen * fLowerArmLen;
		float fAMulB = fUpperArmLen * fLowerArmLen;

		//try to catch defined handle first
		if (m_pLBindObj && m_pRBindObj)
		{
			//valid handle ?
			float fPScale = m_pLBindObj->GetParent()->GetGScale();
			A3DVECTOR3 vLPos = APhysXConverter::N2A_Vector3(m_pLBindObj->GetLPos() * fPScale);
			fPScale = m_pRBindObj->GetParent()->GetGScale();
			A3DVECTOR3 vRPos = APhysXConverter::N2A_Vector3(m_pRBindObj->GetLPos() * fPScale);
			vLPos = vLPos * m_matInWorld;
			vRPos = vRPos * m_matInWorld;
			float tmp1 = (vLPos - vLShoulderPos).SquaredMagnitude();
			float tmp2 = (vRPos - vRShoulderPos).SquaredMagnitude();
			float fAngle1 = (fSqureAB - tmp1) / (2 * fAMulB);
			float fAngle2 = (fSqureAB - tmp2) / (2 * fAMulB);
			if (fAngle1 >= fThresh && fAngle2 >= fThresh ) 
			{
				A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
				m_pLBindObj->GetGPose(mat);
				A3DVECTOR3 vLDir = mat.GetRow(0);
				mat.Identity();
				m_pRBindObj->GetGPose(mat);
				A3DVECTOR3 vRDir = mat.GetRow(0);
				SetBindPosAndDir(vLPos, vRPos, vLDir, vRDir);

				m_state = ToHold;

				m_dwTransTime = (DWORD)((pAction->GetEndTime() - pAction->GetStartTime()) * fBindFrameRatio);
				m_dwTransCnt  = 0;
				m_pPorter = pModel;
				OnStateChanged(Free, m_state);

				GetPos(m_vPosFreBind);
				//convert matrix(without scale) to quat 
				APhysXConverter::N2A_Quat(m_pPhysXObj->GetGRot(), m_quatPreBind);
				A3DMATRIX4 RotMat;
				float fScale;
				CHBasedCD::DecomposeMatrix(m_matInWorld, RotMat, fScale);
				m_quatInWorld.ConvertFromMatrix(RotMat);

				//set driven mode to animation, we do it in script
				return true;
			}
			
		}

		//try to find a correct handle
		vDir = m_matInWorld.GetRow(2);//according to getmodelaabb
		vDir.Normalize();

		float fDist = obb.Extents.z - (m_fDistNearEdge + m_fDistFarEdge) / 2;
		float fInitSpan = -1;
		//try to catch center first
		for (int nTry = 0; nTry < 8; nTry++)
		{

			A3DVECTOR3 vCenter = vRigidCenter - (1 - 1.0f / (1 << nTry)) * vDir * fDist;
			A3DVECTOR3 vStart = vCenter + vLeft * fHalfExt;
			A3DVECTOR3 vEnd   = vCenter - vLeft * fHalfExt;

			float fArmLen = fUpperArmLen + fLowerArmLen;
			A3DVECTOR3 vStartNormal, vEndNormal;
			if (DetectHandle(vStart, vEnd, vStartNormal, vEndNormal, &m_matInWorld))
			{
				float tmp1 = (vStart - vLShoulderPos).SquaredMagnitude();
				float tmp2 = (vEnd - vRShoulderPos).SquaredMagnitude();
				float fAngle1 = (fSqureAB - tmp1) / (2 * fAMulB);
				float fAngle2 = (fSqureAB - tmp2) / (2 * fAMulB);

				float fSpan = (vStart - vEnd).Magnitude();//左右手的跨度
				if (fInitSpan < 0)
					fInitSpan = fSpan;

				if (fAngle1 >= fThresh && fAngle2 >= fThresh && fSpan >= fInitSpan / 2)
				{
					//m_lBindWorld = vStart;
					//m_rBindWorld = vEnd;
					//m_lBindDirWorld = -vStartNormal;
					//m_rBindDirWorld = -vEndNormal;

					//A3DMATRIX4 mat = m_matInWorld.GetInverseTM();					
					//m_lBindLocal = m_lBindWorld * mat;
					//m_rBindLocal = m_rBindWorld * mat;
					//mat.SetTransPart(A3DVECTOR3(0, 0, 0));
					//m_lBindDirLocal = m_lBindDirWorld * mat;
					//m_rBindDirLocal = m_rBindDirWorld * mat;

					SetBindPosAndDir(vStart, vEnd, -vStartNormal, -vEndNormal);

					m_state = ToHold;

					m_dwTransTime = (DWORD)((pAction->GetEndTime() - pAction->GetStartTime()) * fBindFrameRatio);
					m_dwTransCnt  = 0;
					m_pPorter = pModel;
					OnStateChanged(Free, m_state);

					GetPos(m_vPosFreBind);
					//convert matrix(without scale) to quat 
					APhysXConverter::N2A_Quat(m_pPhysXObj->GetGRot(), m_quatPreBind);
					A3DMATRIX4 RotMat;
					float fScale;
					CHBasedCD::DecomposeMatrix(m_matInWorld, RotMat, fScale);
					m_quatInWorld.ConvertFromMatrix(RotMat);

					//set driven mode to animation, we do it in script
					return true;
				}
			}
		}
	}
	return false;
}

void CCarryable::SetBindPosAndDir(const A3DVECTOR3& vLWorldPos, const A3DVECTOR3& vRWorldPos, const A3DVECTOR3& vLWorldDir, const A3DVECTOR3& vRWorldDir)
{
	m_lBindWorld = vLWorldPos;
	m_rBindWorld = vRWorldPos;
	m_lBindDirWorld = vLWorldDir;
	m_rBindDirWorld = vRWorldDir;

	A3DMATRIX4 mat = m_matInWorld.GetInverseTM();					
	m_lBindLocal = m_lBindWorld * mat;
	m_rBindLocal = m_rBindWorld * mat;
	Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), mat);
	m_lBindDirLocal = m_lBindDirWorld * mat;
	m_rBindDirLocal = m_rBindDirWorld * mat;
}

//更新BindPosLocal
bool CCarryable::UpdateBindPosDir()
{
	if (m_pBindBone == NULL || m_bUpdateBind)
		return false;

	m_bUpdateBind = true;
	A3DVECTOR3 vDelta(0.f);

	if ((m_state == Hold  && m_pBindBone) || m_state == ToFree || m_state == ToHold)
	{
		A3DMATRIX4 mat;
		if ((m_state == Hold /*|| m_state == ToFree*/) && m_pBindBone)
		{
			//根据绑定骨骼位置更新箱子位置
			mat = m_pBindBone->GetAbsoluteTM();
			mat = m_matInBind * m_matBind * mat;

			SetPos(Mat44_GetTransPart(mat));
			A3DVECTOR3 vDir, vUp;
			vDir = mat.GetRow(2), vUp = mat.GetRow(1);
			vDir.Normalize(); vUp.Normalize();
			SetDirAndUp(vDir, vUp);
		}
		//for test
		else if (m_state == ToFree && m_pBindBone)
		{
			vDelta.Clear();
			if (m_dwPreTransTime == 0 && m_pPorter)
			{
				vDelta = m_pPorter->GetDir();
				vDelta.y = 0;
				vDelta.Normalize();
				vDelta.y += 1;
				vDelta.Normalize();
				// vDelta *= 0.001f * m_dwTickDelta * (m_dwTransCnt * 0.001f * m_fThrowAcc);//(m_dwTickDelta * 0.001f);
				m_fThrowSpeed += (m_dwTickDelta * 0.001f * m_fThrowAcc);
				
				vDelta *= 0.001f * m_dwTickDelta * m_fThrowSpeed;

			}


			mat = m_pBindBone->GetAbsoluteTM();
			mat = m_matInBind * m_matBind * mat;
			if (m_dwPreTransTime == 0)
				mat = m_matLast;
			A3DVECTOR3 newpos = Mat44_GetTransPart(mat) + vDelta;
			Mat44_SetTransPart(newpos, mat);

			SetPos(Mat44_GetTransPart(mat));
			A3DVECTOR3 vDir, vUp;
			vDir = mat.GetRow(2), vUp = mat.GetRow(1);
			vDir.Normalize(); vUp.Normalize();
			SetDirAndUp(vDir, vUp);

			m_matLast = GetAbsPose(true);
		}
		else if (m_state == ToHold)
		{
			mat = m_pBindBone->GetAbsoluteTM();
			mat = m_matInBind * m_matBind * mat;
		}
		//更新手的位置和朝向
		if (m_pLBindObj && m_pRBindObj)
		{
			A3DMATRIX4 matRot(A3DMATRIX4::IDENTITY);
			m_lBindWorld = APhysXConverter::N2A_Vector3(m_pLBindObj->GetGPos());
			m_rBindWorld = APhysXConverter::N2A_Vector3(m_pRBindObj->GetGPos());
			m_pLBindObj->GetGPose(matRot);
			m_lBindDirWorld = matRot.GetRow(0);
			matRot.Identity();
			m_pRBindObj->GetGPose(matRot);
			m_rBindDirWorld = matRot.GetRow(0);
		}
		else
		{
			m_lBindWorld = m_lBindLocal * mat;
			m_rBindWorld = m_rBindLocal * mat;
			Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), mat);
			m_lBindDirWorld = m_lBindDirLocal * mat;
			m_rBindDirWorld = m_rBindDirLocal * mat;
			m_lBindDirWorld.Normalize();
			m_rBindDirWorld.Normalize();
		}

		m_pPhysXObj->Tick(0);	

		////for test
		//if (m_state == ToFree && m_pBindBone)
		//{
		//	//is reachable?
		//	int index;
		//	A3DBone* pLBone = m_pBindBone->GetSkeleton()->GetBone("Bip01 L UpperArm", &index);
		//	A3DBone* pRBone = m_pBindBone->GetSkeleton()->GetBone("Bip01 R UpperArm", &index);
		//	A3DVECTOR3 vLDelta = pLBone->GetAbsoluteTM().GetTransPart() - m_lBindWorld;
		//	A3DVECTOR3 vRDelta = pRBone->GetAbsoluteTM().GetTransPart() - m_rBindWorld;

		//	A3DBone* pLLowerBone = m_pBindBone->GetSkeleton()->GetBone("Bip01 L Forearm", &index);
		//	A3DBone* pLHandBone = m_pBindBone->GetSkeleton()->GetBone("Bip01 L Hand", &index);
		//	float fArmLen = (pLBone->GetAbsoluteTM().GetTransPart() - pLLowerBone->GetAbsoluteTM().GetTransPart()).Magnitude();
		//	fArmLen += (pLLowerBone->GetAbsoluteTM().GetTransPart() - pLHandBone->GetAbsoluteTM().GetTransPart()).Magnitude();
		//	if (vLDelta.Magnitude() > fArmLen || vRDelta.Magnitude() > fArmLen)
		//	{
		//		m_pBindBone = NULL;//绑定骨骼置为空				
		//		m_state = Free;
		//		m_dwTransCnt = 0;
		//		OnStateChanged(ToFree, m_state);
		//	}
		//}

		return true;
	}
	return false;
}



bool CCarryable::TryToThrow(const char* strAnimation, float fReadyRatio, float fThrowRatio, float fSpeed)
{
	if (strAnimation == NULL)
	{
		//直接扔掉
		m_state = Free;
		m_dwTransCnt = 0;
		OnStateChanged(Hold, m_state);
		m_pPorter = NULL;
		return true;
	}

	if (m_pPhysXObj && m_pBindBone && m_state == Hold)
	{
		m_state = ToFree;
		A3DSkinModelActionCore* pAction = m_pPorter->GetAction(strAnimation);
		DWORD dwDuration = (DWORD)(pAction->GetEndTime() - pAction->GetStartTime());
		m_dwPreTransTime = (DWORD)(dwDuration * fReadyRatio);
		m_dwTransTime = (DWORD)(dwDuration * fThrowRatio);
		m_dwTransCnt = 0;
		OnStateChanged(Hold, m_state);
		//m_pPorter = NULL;

		//compute accelerate
		assert(fSpeed > 0);
		m_fThrowSpeed = a_Min(fSpeed, 2.0f);
		m_fThrowAcc = (fSpeed - m_fThrowSpeed) / (m_dwTransTime * 0.001f);
		
		m_matLast = GetAbsPose(true);
		return true;
	}
	return false;
}

//获取当前箱子的姿态矩阵，包含Scale
A3DMATRIX4 CCarryable::GetAbsPose(bool bWithScale) const
{
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	m_pPhysXObj->GetGPose(mat);
	if (bWithScale)
	{
		A3DMATRIX4 matScale;
		matScale.Scale(m_pPhysXObj->GetGScale(), m_pPhysXObj->GetGScale(), m_pPhysXObj->GetGScale());
		mat = matScale * mat;
	}
	
	return mat;
}

bool CCarryable::RayTrace(const PhysRay& ray, PhysRayTraceHit& traceHit) const
{
	//set special group id to the object
	APhysXScene* pPhysXScene = m_pScene->GetAPhysXScene();
	pPhysXScene->LockWritingScene();

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

	//do ray case
	NxRaycastHit hit;
	NxRay nxRay;
	nxRay.orig = APhysXConverter::A2N_Vector3(ray.vStart);
	nxRay.dir = APhysXConverter::A2N_Vector3(ray.vDelta);
	float dist = nxRay.dir.normalize();

	bool bRet = pPhysXScene->GetNxScene()->raycastClosestShape(nxRay, NX_ALL_SHAPES, hit, 1<<31, dist) != NULL;
	if (bRet)
	{
		traceHit.vHit = APhysXConverter::N2A_Vector3(hit.worldImpact);
		traceHit.vNormal = APhysXConverter::N2A_Vector3(hit.worldNormal);
		traceHit.t = hit.distance / dist;
	}

	//restore the original group
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

	return bRet;
}

bool CCarryable::LinearCapsuleSweep(const NxCapsule& worldcapsule, const NxVec3& motion, NxSweepQueryHit& Hit) const
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