#include "StdAfx.h"
#include "IKHandTarget.h"
#include "CoordinateDirection.h"
#include <A3DIKSolver.h>
#include "IKSolverArm.h"

#ifdef _ANGELICA3
	#include <A3DIKGoal.h>
	#include <A3DIKTrigger.h>
	#include <A3DIKSolver2Joints.h>
#endif

CIKHandTarget::CIKHandTarget(void)
{
	
	m_pSkinModel = NULL;
	m_pTrigger = NULL;
	m_pIKBone  = NULL;

	m_Quat.Clear();

	m_dwFlag = IK_CARRY_MODE;

	m_state = State_FK;
	
	m_vTargetDir.Clear();
	m_fRotSpeed = 1.f; // rad/s
	m_fRotAngle = 0;

	m_vInitLHandDir = A3DVECTOR3(0, 0, 1);
}

CIKHandTarget::~CIKHandTarget(void)
{
	Release();
}

void CIKHandTarget::Init(A3DSkinModel* pSkinModel, const char* strIKBoneName)
{
#ifdef _ANGELICA3
	Release();

	m_pSkinModel = pSkinModel;
	
	m_pIKSolver = new IKSolverArm(m_pSkinModel->GetA3DEngine());
	m_pTrigger = new A3DIKTrigger;
	
	APtrStack<A3DBone*> aBones;
	A3DBone* pBone = m_pSkinModel->GetSkeleton()->GetBone(strIKBoneName, NULL);
	aBones.Push(pBone);
	aBones.Push(pBone->GetParentPtr());
	aBones.Push(pBone->GetParentPtr()->GetParentPtr());
	A3DIKSolver::IK_JOINT joint;
	while (aBones.GetElementNum())
	{
		A3DBone* pCurBone = aBones.Pop();
		joint.strBoneName = pCurBone->GetName();
		m_pIKSolver->AddJoint(joint);
	}

	m_pIKSolver->Enable(true);
	m_pTrigger->SetAllActionFlag(true);

	m_pTrigger->AddIKSolver(m_pIKSolver);
	m_pTrigger->SetActionChannelMask(0xffff);
	m_pSkinModel->BindIKTrigger(m_pTrigger);

	m_pIKBone = pBone;
	m_Quat = m_pIKBone->GetRelativeQuat();
#endif
}
void CIKHandTarget::Release()
{
#ifdef _ANGELICA3
	if (m_pTrigger)
	{
		if (m_pSkinModel)
		{
			m_pSkinModel->UnbindIKTrigger(m_pTrigger);
			for (int i = 0; i < m_pTrigger->GetIKSolverNum(); i++)
			{
				delete m_pTrigger->GetIKSolver(i);
			}
		}
		delete m_pTrigger;
		m_pTrigger = NULL;
	}
#endif
	m_pSkinModel = NULL;
	m_pIKBone  = NULL;
	m_Quat.Clear();
}

bool CIKHandTarget::UpdateRefFrameOnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	if (m_pSkinModel == NULL)
		return false;

	return m_refFrame.OnLButtonDown(x, y, nFlags);
}
bool CIKHandTarget::UpdateRefFrameOnMouseMove(const int x, const int y, const unsigned int nFlags)
{
	if (m_pSkinModel == NULL)
		return false;

	bool rtn = m_refFrame.OnMouseMove(x, y, nFlags);

	if (m_dwFlag == IK_DRAG_MODE)
	{
		if (m_pIKSolver && QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_MOVE))
		{
#ifdef _ANGELICA3
			m_pIKSolver->SetIKGoal_Generic(m_refFrame.GetPos());
#endif
		}
		else if (m_pIKSolver && QueryRefFrameEditMode(CCoordinateDirection::EDIT_TYPE_ROTATE))
		{
			float angRad = 0.0f;
			A3DVECTOR3 localAxis(0.0f);
			A3DVECTOR3 globalAxis(0.0f);
			bool rtn = m_refFrame.GetLastRotateInfo(angRad, localAxis, globalAxis);
			assert(true == rtn);
			SetIKTargetRot(angRad, localAxis, globalAxis);
		}
	}

	return rtn;
}

void CIKHandTarget::SetIKTargetRot(const float angleRadians, const A3DVECTOR3& localAxis, const A3DVECTOR3& globalAxis)
{
	if (m_pIKBone == NULL)
		return;

	if (NxMath::equals(angleRadians, 0.0f, APHYSX_FLOAT_EPSILON))
		return;
	if (NxMath::equals(localAxis.x, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(localAxis.y, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(localAxis.z, 0.0f, APHYSX_FLOAT_EPSILON))
		return;

	A3DQUATERNION quatRotL;
	quatRotL.ConvertFromAxisAngle(localAxis, angleRadians);	

	A3DQUATERNION oldQuat = m_pIKBone->GetRelativeQuat();
	
	m_Quat = quatRotL * oldQuat;

	m_Quat.Normalize();
}

void CIKHandTarget::Tick(DWORD dwTime)
{
#ifdef _ANGELICA3
	if (m_pIKSolver)
		m_pIKSolver->Tick(dwTime);

	////own tick
	//if (m_fRotAngle != 0)
	//{
	//	A3DVECTOR3 vHandDir = GetHandDir();
	//	
	//	float fTheta = m_fRotSpeed * (0.001f * dwTime);
	//	if (m_fRotAngle <= fTheta)
	//	{
	//		fTheta = m_fRotAngle;
	//		m_fRotAngle = 0;
	//	}
	//	else
	//		m_fRotAngle -= fTheta;

	//	
	//}
	if (m_fRotAngle != 0)
	{
		float fTheta = m_fRotSpeed * (0.001f * dwTime);
		if (m_fRotAngle <= fTheta)
		{
			fTheta = m_fRotAngle;
			m_fRotAngle = 0;
		}
		else
			m_fRotAngle -= fTheta;

		A3DQUATERNION quat(m_vRotAxis, fTheta);
		
		A3DMATRIX4 mtBone = m_pIKBone->GetAbsoluteTM();
		A3DMATRIX4 mtRot(A3DMATRIX4::IDENTITY);
		mtRot.RotateAxis(m_vRotAxis, fTheta);
		Mat44_SetRotatePart(mtRot, mtBone);

		A3DBone* pParentBone = m_pIKBone->GetParentPtr();
		A3DMATRIX4 mtParent = pParentBone->GetAbsoluteTM();
		mtBone *= mtParent.GetInverseTM();

		m_Quat.ConvertFromMatrix(mtBone);
		A3DMATRIX3 rot(A3DMATRIX3::IDENTITY);
		Mat44_GetRotatePart(mtBone, rot);
		m_pIKBone->SetRelativeRotation(rot);

		m_pSkinModel->GetSkeleton()->RefreshAfterIK(m_pIKBone);
	}
#endif

}
void CIKHandTarget::DrawFrame(A3DWireCollector* pWC)
{
	if (0 != pWC && m_pSkinModel != NULL)
	{
		m_refFrame.Draw(*pWC);
		A3DVECTOR3 vPos = Mat44_GetTransPart(m_pIKBone->GetAbsoluteTM());
		pWC->Add3DLine(vPos, vPos + GetHandDir() * 0.5f, 0x80808000);
	}
}

void CIKHandTarget::SetRefFrameEditMode(const EditType& et) 
{ 
	if (m_pIKBone)
	{
		A3DMATRIX4 mat = m_pIKBone->GetNoScaleAbsTM();
		m_refFrame.SetDirAndUp(mat.GetRow(2), mat.GetRow(1));
	}
	m_refFrame.SetEditType(et); 
}
CIKHandTarget::EditType CIKHandTarget::GetRefFrameEditMode() const 
{ 
	return m_refFrame.GetEditType(); 
}

A3DVECTOR3 CIKHandTarget::GetHandDir() const
{
	if (m_pIKBone != NULL)
	{
		A3DMATRIX4 mat = m_pIKBone->GetNoScaleAbsTM();
		assert(!_isnan(mat._31));
		Mat44_SetTransPart(A3DVECTOR3(0, 0, 0), mat);
		A3DVECTOR3 vDir = m_vInitLHandDir * mat;
		vDir.Normalize();
		return vDir;
	}
	return A3DVECTOR3(0, 0, 0);
}

void CIKHandTarget::GetRotMatrix(const A3DVECTOR3& vFrom, const A3DVECTOR3& vTo, A3DMATRIX3& retMat, A3DVECTOR3& vRotAxis)
{
	const float pi = 3.1415926f;

	float fDot = DotProduct(vFrom, vTo);
	if (fDot > 0.999f)
	{
		retMat.Identity();
		vRotAxis.Clear();
		return;
	}
	else if (fDot < -0.999f)
	{
		A3DMATRIX4 mat;
		mat.RotateX(pi);
		Mat44_GetRotatePart(mat, retMat);
		vRotAxis.Clear();
	}
	else
	{
		vRotAxis = CrossProduct(vFrom, vTo);

		float fM = vRotAxis.Normalize();
		a_Clamp(fM, -1.f, 1.f);
		fM = ::asin(fM);
		if (fDot < 0)
			fM = pi - fM;

		A3DQUATERNION qCharacter(vRotAxis, fM);
#ifdef _ANGELICA3
		qCharacter.ConvertToMatrix3(retMat);
#else
		A3DMATRIX4 mat44(A3DMATRIX4::IDENTITY);
		qCharacter.ConvertToMatrix(mat44);
		Mat44_GetRotatePart(mat44, retMat);
#endif
	}
}

void CIKHandTarget::InternalSetHandDir(const A3DVECTOR3& vHandDir)
{
#ifdef _ANGELICA3
	if (m_pIKBone != NULL)
	{
		A3DMATRIX3 mtRotCharacter;
		A3DMATRIX3 mtOldMat;
		Mat44_GetRotatePart(m_pIKBone->GetNoScaleAbsTM(), mtOldMat);
		A3DVECTOR3 vAxis;
		GetRotMatrix(GetHandDir(), vHandDir, mtRotCharacter, vAxis);

		A3DMATRIX3 mtAbsOrtTarget = mtOldMat * mtRotCharacter;

		// update the bone's global matrix
		A3DMATRIX4 mtBone = m_pIKBone->GetAbsoluteTM();
		Mat44_SetRotatePart(mtAbsOrtTarget, mtBone);

		//---------------------------------------------------------------
		// update the bone's local/relative matrix
		A3DBone* pParentBone = m_pIKBone->GetParentPtr();
		A3DMATRIX4 mtParent = pParentBone->GetAbsoluteTM();
		mtBone *= mtParent.GetInverseTM();

		m_Quat.ConvertFromMatrix(mtBone);
		m_pIKBone->SetRelativeRotation(mtBone.GetRotatePart());
		m_pSkinModel->GetSkeleton()->RefreshAfterIK(m_pIKBone);
	}
#endif
}

void CIKHandTarget::InternalSetHandDir(const A3DVECTOR3& vHandDir, const A3DVECTOR3& vFinger)
{
#ifdef _ANGELICA3
	if (m_pIKBone != NULL)
	{
		A3DMATRIX3 mat;
		mat.SetRow(0, vFinger);
		mat.SetRow(1, CrossProduct(vHandDir, vFinger));
		mat.SetRow(2, vHandDir);

		A3DMATRIX4 mtBone = m_pIKBone->GetAbsoluteTM();
		Mat44_SetRotatePart(mat, mtBone);

		A3DBone* pParentBone = m_pIKBone->GetParentPtr();
		A3DMATRIX4 mtParent = pParentBone->GetAbsoluteTM();
		mtBone *= mtParent.GetInverseTM();

		m_Quat.ConvertFromMatrix(mtBone);
		m_pIKBone->SetRelativeRotation(mtBone.GetRotatePart());

		m_pSkinModel->GetSkeleton()->RefreshAfterIK(m_pIKBone);
	}
#endif
}

void CIKHandTarget::SetHandDir(const A3DVECTOR3& vHandDir, const A3DVECTOR3& vFinger, bool bAtOnce)
{
	A3DVECTOR3 vAxisX = vFinger;
	A3DVECTOR3 vAxisY;
	vAxisY = CrossProduct(vHandDir, vFinger);
	
	A3DVECTOR3 vAxisZ = vHandDir;
	vAxisX = CrossProduct(vAxisY, vAxisZ);
	vAxisX.Normalize();
	vAxisY.Normalize();
	vAxisZ.Normalize();

	if (bAtOnce)
	{
		InternalSetHandDir(vAxisZ, vAxisX);
		m_vTargetDir.Clear();
		m_fRotAngle = 0;
	}
	else
	{
		A3DMATRIX3 mat;//目标朝向
		mat.m[0][0] = vAxisX.x;
		mat.m[0][1] = vAxisX.y;
		mat.m[0][2] = vAxisX.z;
		mat.m[1][0] = vAxisY.x;
		mat.m[1][1] = vAxisY.y;
		mat.m[1][2] = vAxisY.z;
		mat.m[2][0] = vAxisZ.x;
		mat.m[2][1] = vAxisZ.y;
		mat.m[2][2] = vAxisZ.z;

		A3DMATRIX3 matCur;
		Mat44_GetRotatePart(m_pIKBone->GetNoScaleAbsTM(), matCur);
#ifdef _ANGELICA3
		A3DMATRIX3 matCurInv;
		matCur.Inverse(matCurInv);
#else
		NxMat33 nxMatCur;
		APhysXConverter::A2N_Matrix33(matCur, nxMatCur);
		NxMat33 nxMatCurInv;
		nxMatCur.getInverse(nxMatCurInv);
		A3DMATRIX3 matCurInv;
		APhysXConverter::N2A_Matrix33(nxMatCurInv, matCurInv);
#endif
		A3DMATRIX3 matRot = mat * matCurInv;

		A3DQUATERNION quat;
#ifdef _ANGELICA3
		quat.ConvertFromMatrix3(matRot);
#else
		A3DMATRIX4 mat44(A3DMATRIX4::IDENTITY);
		Mat44_SetRotatePart(matRot, mat44);
		quat.ConvertFromMatrix(mat44);
#endif
		quat.ConvertToAxisAngle(m_vRotAxis, m_fRotAngle);
	}
}

void CIKHandTarget::SetHandDir(const A3DVECTOR3& vHandDir, bool bAtOnce)
{
	if (bAtOnce)
	{
		InternalSetHandDir(vHandDir);
		m_vTargetDir.Clear();
		m_fRotAngle = 0;
	}
	else
	{
		m_vTargetDir = vHandDir;
		A3DMATRIX3 mtRotCharacter;
		GetRotMatrix(GetHandDir(), vHandDir, mtRotCharacter, m_vRotAxis);
		if (m_vRotAxis.IsZero())//计算不出转轴
			SetHandDir(vHandDir, true);
		else
		{
			A3DQUATERNION quat;
#ifdef _ANGELICA3
			quat.ConvertFromMatrix3(mtRotCharacter);
#else
			A3DMATRIX4 mat44(A3DMATRIX4::IDENTITY);
			Mat44_SetRotatePart(mtRotCharacter, mat44);
			quat.ConvertFromMatrix(mat44);
#endif
			quat.ConvertToAxisAngle(m_vRotAxis, m_fRotAngle);
		}
	}
}

void CIKHandTarget::StartCarry(DWORD dwTime, DWORD dwResistance)
{
#ifdef _ANGELICA3
	if (m_pIKBone)
	{

		//if (!m_pSkinModel->IsIKEnabled())
		//	m_pSkinModel->EnableIK(true);
		
		m_pIKSolver->SetTransTime(dwTime);
		m_pIKSolver->SetStateFK2IK(dwResistance);
	}
#endif
}

void CIKHandTarget::StartThrow(DWORD dwTime, DWORD dwResistance)
{
#ifdef _ANGELICA3
	if (m_pIKBone)
	{
		m_pIKSolver->SetTransTime(dwTime);
		m_pIKSolver->SetStateIK2FK(dwResistance);
	}
#endif
}

void CIKHandTarget::EnableIK(bool bEnable )
{
#ifdef _ANGELICA3
	if (m_pIKSolver)
	{
		m_pIKSolver->Enable(bEnable);
	}
#endif
}
bool CIKHandTarget::IsIKEnable() const
{
#ifdef _ANGELICA3
	if (m_pIKSolver)
	{
		return m_pIKSolver->IsEnabled();
	}
#endif
	return false;
}

void CIKHandTarget::SetPos(const A3DVECTOR3& vPos, bool bAtOnce /* = false */)
{ 
	m_refFrame.SetPos(vPos);
#ifdef _ANGELICA3
	if (bAtOnce)
		m_pIKSolver->SetIKGoal_Generic(vPos);
#endif
}

void CIKHandTarget::SetIKGoalPos(const A3DVECTOR3& vPos)
{
#ifdef _ANGELICA3
	if (m_pIKSolver)
		m_pIKSolver->SetIKGoal_Generic(vPos);
#endif
}

void CIKHandTarget::SetSolverInterface(IKSolverArmInterface* pInterface)
{
#ifdef _ANGELICA3
	m_pIKSolver->SetSolverInterface(pInterface);
#endif
}

