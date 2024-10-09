/*
 * FILE: PhysXObjForceField.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/03/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "NxForceFieldKernelDefs.h"

CPhysXObjForceField::CPhysXObjForceField(int FFType) : IPhysXObjBase(FFType)
{
	m_pAFF = 0;
	m_pFF = 0;
	m_ffMat.id();
	m_hostCNL = -1;

	m_CKID = CKID_NONE;
	m_pCustomKnl = 0;
}

CPhysXObjForceField::~CPhysXObjForceField()
{
	if (0 < m_FFOD.GetNum())
		m_FFOD.Release();
	Release(); 
}

bool CPhysXObjForceField::ReloadModel()
{
	AString thefile = GetFilePathName();
	if (thefile.IsEmpty())
		return false;

	if (0 < m_FFOD.GetNum())
		m_FFOD.Release();
	if (0 == m_FFOD.GetPhysXObjectDesc(thefile))
		return false;

	return true;
}

bool CPhysXObjForceField::OnDeletionNotify(const APhysXObject& rObject)
{
	if (m_pAFF == &rObject)
	{
		m_pAFF = 0;
		m_pFF = 0;
		return true;
	}
	return false;
}

bool CPhysXObjForceField::OnLoadModel(CRender& render, const char* szFile)
{
	m_CKID = CKID_NONE;
	if (0 == m_FFOD.GetPhysXObjectDesc(szFile))
	{
		a_LogOutput(1, "CPhysXObjForceField::OnLoadModel: Failed to create FFObjectDesc!");
		return false;
	}

	AString strName;
	af_GetFileTitle(szFile, strName);
	const AString theName("pressure.xff");
	if (theName == strName)
		m_CKID = CKID_AURA;
	const AString theName1("Rive.xff");
	if (theName1 == strName)
		m_CKID = CKID_RIVE;

	m_ffMat.id();
	return true;
}

void CPhysXObjForceField::OnReleaseModel()
{
	if (0 < m_FFOD.GetNum())
		m_FFOD.Release();
	m_ffMat.id();
}

void CPhysXObjForceField::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	assert(0 != pWC);
	if (!ReadFlag(OBF_DRAW_BINDING_BOX))
	{
		A3DOBB obb;
		if (GetOBB(obb))
			pWC->AddOBB(obb, 0xfff0f0f0);
	}
	else
	{
		int nCount = m_FFOD.GetNum();
		assert(0 < nCount);
		assert(false == ReadFlag(OBFI_IS_IN_RUNTIME));
		APhysXObjectDesc* pDesc = m_FFOD.GetPhysXObjectDesc(nCount - 1);
		APhysXForceFieldObjectDesc* pFFOD = static_cast<APhysXForceFieldObjectDesc*>(pDesc);
		if (0 != pFFOD)
		{
			// at present, we only draw build-in shape group;
			DrawAllFFShapes(*pWC, pFFOD->mIncludeShapes, 0xffffff00);
		}
	}
	if (!ReadFlag(OBFI_IS_IN_RUNTIME))
		pWC->AddSphere(GetPos(), 0.1f, 0xffff0000);
}

NX_START_FORCEFIELD(Aura)
	NxFConst(K);
	NxFConst(R);
	NxBConst(Rest);
	NxBConst(EnhanceMode);
	NxVConst(LocalXSide);     // input a normalized value
	NxVConst(LocalYUp);       // input a normalized value
	NxVConst(LocalZFaceTo);   // input a normalized value

	NX_START_FUNCTION
		NxFinishIf(Rest)

		NxFloat magPos = Position.magnitude();
		NxFloat RecipMagPos = magPos.recip();
		NxVector posNormalize;
		posNormalize.setX(Position.getX() * RecipMagPos);
		posNormalize.setY(Position.getY() * RecipMagPos);
		posNormalize.setZ(Position.getZ() * RecipMagPos);

		NxFloat RecipR = R;
		RecipR = RecipR.recip();
		NxFloat posRatio = magPos * RecipR;
		NxFinishIf(posRatio > 1.0f);

		NxFloat chance = 0.0f;
		NxFloat costheta = LocalZFaceTo.dot(posNormalize);

		NxBoolean bInPushRange(costheta >= 0.0f);
		if (NxForceFieldInternals::NxSw::getBoolVal(bInPushRange))
		{
			// in the push range
			// compute the chance of disable push
			NxFloat cPushRange = 0.8f;
			NxFloat RecipPRange = cPushRange;
			RecipPRange = RecipPRange.recip();

			NxFloat maxRatio = cPushRange;
			NxFloat minRatio = 0.3f;
			NxFloat maxChance = NxSelect(EnhanceMode, 0.3f, 0.7f);  // [last]% (EnhanceMode: [first]%) chance to disable push at maxRatio
			NxFloat minChance = NxSelect(EnhanceMode, 0.1f, 0.45f);  // [last]% (EnhanceMode: [first]%) chance to disable push at minRatio
			NxFloat RecipDRatio = maxRatio - minRatio;
			RecipDRatio = RecipDRatio.recip();
			chance = minChance + (posRatio - minRatio) * RecipDRatio * (maxChance - minChance);
			chance = NxSelect(posRatio > maxRatio, +1.0f, chance);
			chance = NxSelect(posRatio < minRatio, -1.0f, chance);
			NxBoolean bDisablePush(NxMath::rand(0.0f, 1.0f) <= chance);
			NxFinishIf(bDisablePush); 

			// compute push dir
			NxVector dirPush;
			dirPush.setX(posNormalize.getX());
			dirPush.setZ(posNormalize.getZ());
			NxFloat ranY = (NxMath::rand(0.0f, 1.0f));
			ranY = NxSelect(ranY > 0.7f * R, ranY * 0.4f, ranY);
			dirPush.setY(ranY);

			// normalize the dir
			NxFloat magDP = dirPush.magnitude();
			NxFloat RecipMagDP = magDP.recip();
			dirPush.setX(dirPush.getX() * RecipMagDP);
			dirPush.setY(dirPush.getY() * RecipMagDP);
			dirPush.setZ(dirPush.getZ() * RecipMagDP);

			NxFloat degDecay = costheta;
			NxFloat cofPush = degDecay + (1 - posRatio * RecipPRange) * (1 - degDecay);
			force = dirPush * K * cofPush;
			NxFinishIf(bInPushRange);
		}

		NxFloat cofPull = costheta * 2 + 1;
		NxBoolean bInPullRange(cofPull >= 0.0f);
		if (NxForceFieldInternals::NxSw::getBoolVal(bInPullRange))
		{
			// in the pull range
			// compute the chance of disable pull
			chance = NxSelect(EnhanceMode, 0.3f, 0.55f);  // [last]% (EnhanceMode: [first]%) chance to disable pull
			NxBoolean bDisablePull(NxMath::rand(0.0f, 1.0f) <= chance);
			NxFinishIf(bDisablePull); 

			// compute pull dir and force
			NxVector dirPull = LocalXSide;
			NxFloat theta = dirPull.dot(posNormalize);
			NxFloat signD = NxSelect(theta > 0, -1.0f, 1.0f);
			dirPull.setX(dirPull.getX() * signD);
			dirPull.setY(dirPull.getY() * signD);
			dirPull.setZ(dirPull.getZ() * signD);
			
			cofPull = (1 - cofPull) * 1.3f * posRatio;
			force = dirPull * K * cofPull;
			NxFinishIf(bInPullRange);		
		}

		// in the random range
		NxVector dirOpPull = LocalXSide;
		NxFloat theta = dirOpPull.dot(posNormalize);
		if (NxForceFieldInternals::NxSw::getFloatVal(theta) < 0.0f)
		{
			dirOpPull.setX(dirOpPull.getX() * -1);
			dirOpPull.setY(dirOpPull.getY() * -1);
			dirOpPull.setZ(dirOpPull.getZ() * -1);
		}

		// compute random dir
		NxBoolean bOppoPullDir(NxMath::rand(0.0f, 1.0f) <= 0.25f); // xx% chance to get the opposite of pull dir
		NxVector randDir;
		randDir.setX(NxSelect(bOppoPullDir, dirOpPull.getX(), NxMath::rand(-1.0f, 1.0f)));
		randDir.setZ(NxSelect(bOppoPullDir, dirOpPull.getZ(), NxMath::rand(-1.0f, 1.0f)));
		randDir.setY(NxMath::rand(-0.5f, 1.5f));
		
		// normalize the dir
		NxFloat magRD = randDir.magnitude();
		NxFloat RecipMagRD = magRD.recip();
		randDir.setX(randDir.getX() * RecipMagRD);
		randDir.setY(randDir.getY() * RecipMagRD);
		randDir.setZ(randDir.getZ() * RecipMagRD);

		NxFloat cofRand = NxMath::rand(0.0f, 1.0f);
		cofRand = NxSelect(EnhanceMode, NxMath::rand(0.4f, 1.3f), cofRand);
		cofRand *= NxSelect(bOppoPullDir, 0.5f, 1);
		force = randDir * K * cofRand;
	NX_END_FUNCTION
NX_END_FORCEFIELD(Aura)

/*  // obsoleted implementation
NX_START_FORCEFIELD(Rive)
	NxFConst(K);
	NxFConst(R);
	NxFConst(HalfH);
//	NxBConst(Rest);
//	NxBConst(EnhanceMode);
//	NxVConst(LocalXSide);     // input a normalized value
//	NxVConst(LocalYUp);       // input a normalized value
//	NxVConst(LocalZFaceTo);   // input a normalized value
	
	NX_START_FUNCTION
//		NxFinishIf(Rest)

		NxVector dirPush = Position;
		NxFloat YVal = dirPush.getY();
		NxFloat NewY = 0.0f;
		NewY = NxSelect(YVal > HalfH, YVal - HalfH, NewY);
		NewY = NxSelect(YVal < -HalfH, YVal + HalfH, NewY);
		dirPush.setY(NewY);
		NxFloat mag = dirPush.magnitude();
		NxFloat RecipMag = mag.recip();
		dirPush.setX(dirPush.getX() * RecipMag);
		dirPush.setY(dirPush.getY() * RecipMag);
		dirPush.setZ(dirPush.getZ() * RecipMag);

		NxFloat RecipR = R;
		RecipR = RecipR.recip();
		NxFloat posRatio = (1 - mag * RecipR) + 0.5f;

		bool InTopSemiSphere(false);
		bool InCylinder(false);
		bool InBtmSemiSphere(false);
		float fNewY = NxForceFieldInternals::NxSw::getFloatVal(NewY);
		if ((fNewY < 0.00001f) && (fNewY > -0.00001f))
		{
			InCylinder = true;
		}
		else
		{
			if (fNewY > 0.0f)
				InTopSemiSphere = true;
			else
				InBtmSemiSphere = true;
		}

		NxVector Front;
		Front.setX(0);
		Front.setY(0);
		Front.setZ(1);
		NxFloat cosT = Front.dot(dirPush);
		if (NxForceFieldInternals::NxSw::getFloatVal(cosT) > 0.5f)
		{
			NxVector Up;
			Up.setX(0);
			Up.setY(1);
			Up.setZ(0);
			NxFloat sign = 1.0f;
			sign = NxSelect(dirPush.getX() > 0.0f, sign, -sign);
			Up.setY(Up.getY() * sign);
			NxVector dirX =  Up.cross(dirPush);
			if (InCylinder || InBtmSemiSphere)
			{
				dirPush = dirX;
			}
			else
			{
				NxVector dirUpNew = dirX.cross(dirPush);
				dirPush = dirUpNew;
			}
		}
		else if (NxForceFieldInternals::NxSw::getFloatVal(cosT) > 0.0f)
		{
			NxFloat ZVal = dirPush.getZ();
			if (NxForceFieldInternals::NxSw::getFloatVal(ZVal) > 0.0f)
				dirPush.setZ(ZVal * -1);
		}
		else
		{
			NxFinishIf(true);
		}
		
		force = dirPush * K * posRatio;
		if (NxMath::rand(0.0f, 1.0f) < 0.5f)
		{
			NxVector randDir;
			randDir.setX(NxMath::rand(0.0f, 1.0f));
			randDir.setZ(NxMath::rand(0.0f, 1.0f));
			randDir.setY(NxMath::rand(0.0f, 1.0f));
			NxFloat magRD = randDir.magnitude();
			NxFloat RecipMagRD = magRD.recip();
			randDir.setX(randDir.getX() * RecipMagRD);
			randDir.setY(randDir.getY() * RecipMagRD);
			randDir.setZ(randDir.getZ() * RecipMagRD);
			NxVector Noise = randDir * K * NxMath::rand(0.0f, 0.3f);
			force += Noise;
		}

	NX_END_FUNCTION
NX_END_FORCEFIELD(Rive)*/

NX_START_FORCEFIELD(Rive)
	NxFConst(K);
	NxFConst(HalfX);
	NxFConst(HalfY);
	NxFConst(HalfZ);
	
	NX_START_FUNCTION
		float xPos = NxForceFieldInternals::NxSw::getFloatVal(Position.getX());
		float yPos = NxForceFieldInternals::NxSw::getFloatVal(Position.getY());
		float zPos = NxForceFieldInternals::NxSw::getFloatVal(Position.getZ());
		NxFinishIf(xPos > HalfX);
		NxFinishIf(yPos > HalfY);
		NxFinishIf(zPos > 0);
		NxFinishIf(xPos < -HalfX);
		NxFinishIf(yPos < -HalfY);
		NxFinishIf(zPos < -2 * HalfZ);

		NxFloat XZRatio = HalfZ;
		XZRatio = XZRatio.recip();
		XZRatio *= HalfX;

		NxFloat absXPos = NxMath::abs(xPos);
		NxFloat absZPos = NxMath::abs(zPos);
		NxFloat XZRatioReal = absZPos;
		XZRatioReal = XZRatioReal.recip();
		XZRatioReal *= absXPos;
		NxFinishIf(XZRatioReal > XZRatio * XZRatio);

		NxFloat RecipHX = HalfX;
		RecipHX = RecipHX.recip();
		NxFloat xPosRatio = absXPos * RecipHX;

		NxFloat RecipZ = 2 * HalfZ;
		RecipZ = RecipZ.recip();
		NxFloat zPosRatio = absZPos * RecipZ;

		NxVector dirNZ;
		dirNZ.setX(0);
		dirNZ.setY(0);
		dirNZ.setZ(-1);
		force = dirNZ * K * (1 - xPosRatio) * NxMath::rand(0.7f, 1.2f);

		NxVector dirSide;
		dirSide.setX(1);
		dirSide.setY(0);
		dirSide.setZ(0);
		if (xPos < 0.0f)
			dirSide.setX(-1);
		force += dirSide * K * zPosRatio * NxMath::rand(0.7f, 1.2f);

		if (NxMath::rand(0.0f, 1.0f) < 0.5f)
		{
			NxVector randDir;
			randDir.setX(NxMath::rand(0.0f, 1.0f));
			randDir.setZ(NxMath::rand(0.0f, 1.0f));
			randDir.setY(NxMath::rand(0.0f, 1.0f));
			NxFloat magRD = randDir.magnitude();
			NxFloat RecipMagRD = magRD.recip();
			randDir.setX(randDir.getX() * RecipMagRD);
			randDir.setY(randDir.getY() * RecipMagRD);
			randDir.setZ(randDir.getZ() * RecipMagRD);
			NxVector Noise = randDir * K * NxMath::rand(0.0f, 0.3f);
			force += Noise;
		}
	NX_END_FUNCTION
NX_END_FORCEFIELD(Rive)


void CPhysXObjForceField::SetState(bool toRest, bool IsRunning)
{
	if (CKID_AURA == m_CKID)
	{
		NxForceFieldKernelAura* pknlAura = static_cast<NxForceFieldKernelAura*>(m_pCustomKnl);
		pknlAura->setRest(toRest);
		pknlAura->setEnhanceMode(IsRunning);
		return;
	}

	if (m_pAFF->IsObjectType(APX_OBJTYPE_FORCEFIELD_BOSS))
	{
		APhysXBossForceFieldObject* pBossAura = static_cast<APhysXBossForceFieldObject*>(m_pAFF);
		pBossAura->SetRestState(toRest);
		pBossAura->SetEnhanceMode(IsRunning);
		return;
	}
}

bool CPhysXObjForceField::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	int nCount = m_FFOD.GetNum();
	assert(0 < nCount);
	if (0 >= nCount)
		return false;

	if (0 != m_pFF)
	{
		if (&m_pFF->getScene() == aPhysXScene.GetNxScene())
			return true;

		ReleasePhysXObj();
	}

	assert(0 == m_pAFF);
	APhysXObjectInstanceDesc objInstanceDesc;
	objInstanceDesc.mPhysXObjectDesc = m_FFOD.GetPhysXObjectDesc(nCount - 1);
	objInstanceDesc.mGlobalPose = m_ffMat;

	if (CKID_AURA == m_CKID)
	{
		NxForceFieldKernelAura* pknlAura = new NxForceFieldKernelAura;
		APhysXForceFieldObjectDesc* pFFDesc = static_cast<APhysXForceFieldObjectDesc*>(objInstanceDesc.mPhysXObjectDesc);
		pFFDesc->mFFCoordinates = NX_FFC_CARTESIAN;
		pFFDesc->mCustomKernel = pknlAura;
		pFFDesc->mFlags = 0;
		APhysXShapeDesc* pDesc = pFFDesc->mIncludeShapes.GetPhysXShapeDesc(0);
		APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
		pknlAura->setK(20);
		pknlAura->setR(1.5);
		pknlAura->setRest(false);
		pknlAura->setEnhanceMode(false);
		pknlAura->setLocalXSide(NxVec3(1, 0, 0));
		pknlAura->setLocalYUp(NxVec3(0, 1, 0));
		pknlAura->setLocalZFaceTo(NxVec3(0, 0, 1));
		m_pCustomKnl = pknlAura;
	}
	else if (CKID_RIVE == m_CKID)
	{
		NxForceFieldKernelRive* pknlRive = new NxForceFieldKernelRive;
		APhysXForceFieldObjectDesc* pFFDesc = static_cast<APhysXForceFieldObjectDesc*>(objInstanceDesc.mPhysXObjectDesc);
		pFFDesc->mFFCoordinates = NX_FFC_CARTESIAN;
		pFFDesc->mCustomKernel = pknlRive;
		pFFDesc->mFlags = 0;
		APhysXShapeDesc* pDesc = pFFDesc->mIncludeShapes.GetPhysXShapeDesc(0);
		APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
	//	pknlRive->setK(20);
	//	pknlRive->setR(0.8f);
	//	pknlRive->setHalfH(0.6);
		pknlRive->setK(5);
		pknlRive->setHalfX(1);
		pknlRive->setHalfY(2);
		pknlRive->setHalfZ(3);
		m_pCustomKnl = pknlRive;
	}

	m_pAFF = static_cast<APhysXForceFieldObject*>(aPhysXScene.CreatePhysXObject(objInstanceDesc));
	if (0 != m_pAFF)
		m_pFF = m_pAFF->GetNxForceField();

	return (0 == m_pFF)? false : true;
}

void CPhysXObjForceField::OnReleasePhysXObj(const bool RunTimeIsEnd)
{
	if (0 != m_pFF)
	{
		APhysXScene* pScene = static_cast<APhysXScene*>(m_pFF->getScene().userData);
		APhysXForceFieldObject* pFF = static_cast<APhysXForceFieldObject*>(m_pFF->userData);
		pScene->ReleasePhysXObject(pFF, true);
		m_pFF = 0;
	}
	m_pAFF = 0;

	delete m_pCustomKnl;
	m_pCustomKnl = 0;
}

bool CPhysXObjForceField::OnGetSelectedAABB(A3DAABB& outAABB) const
{
	outAABB.Center = GetPos();
	outAABB.Extents.Set(0.5f, 0.5f, 0.5f);
	outAABB.CompleteMinsMaxs();
	return true;
}

void CPhysXObjForceField::AttachTo(NxActor* pActor)
{
	m_pFF->setActor(pActor);
}

int CPhysXObjForceField::GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor) const
{
	return 0;
}

NxActor* CPhysXObjForceField::GetFirstNxActor() const
{
	return 0;
}

void CPhysXObjForceField::OnSetPos(const A3DVECTOR3& vPos)
{
	m_ffMat.t = APhysXConverter::A2N_Vector3(vPos);
	if (0 != m_pAFF)
		m_pAFF->SetPose(m_ffMat);
}

void CPhysXObjForceField::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	m_ffMat.M.setColumn(0, APhysXConverter::A2N_Vector3(CrossProduct(vUp, vDir)));
	m_ffMat.M.setColumn(1, APhysXConverter::A2N_Vector3(vUp));
	m_ffMat.M.setColumn(2, APhysXConverter::A2N_Vector3(vDir));
	if (0 != m_pAFF)
		m_pAFF->SetPose(m_ffMat);
}

void CPhysXObjForceField::DrawAllFFShapes(A3DWireCollector& wc, const APhysXShapeDescManager& sdm, int color) const
{
	int nCount = sdm.GetNum();
	if (0 >= nCount)
		return;

	for (int i = 0; i < nCount; ++i)
	{
		APhysXShapeDesc* pDesc = sdm.GetPhysXShapeDesc(i);
		switch(pDesc->GetShapeType())
		{
		case APX_SHAPETYPE_BOX:
			{
				APhysXBoxShapeDesc* pBox = static_cast<APhysXBoxShapeDesc*>(pDesc);
				NxMat34 mat(m_ffMat);
				mat.multiply(m_ffMat, pBox->mLocalPose);
				APhysXDrawBasic::DrawBox(wc, NxBox(mat.t, pBox->mDimensions, mat.M), color);
			}
			break;
		case APX_SHAPETYPE_SPHERE:
			{
				APhysXSphereShapeDesc* pSphere = static_cast<APhysXSphereShapeDesc*>(pDesc);
				NxMat34 mat(m_ffMat);
				mat.multiply(m_ffMat, pSphere->mLocalPose);
				APhysXDrawBasic::DrawSphere(wc, pSphere->mRadius, color, 0xffffffff, &mat);
			}
			break;
		case APX_SHAPETYPE_CAPSULE:
			{
				APhysXCapsuleShapeDesc* pCapsule = static_cast<APhysXCapsuleShapeDesc*>(pDesc);
				NxMat34 mat(m_ffMat);
				mat.multiply(m_ffMat, pCapsule->mLocalPose);
				APhysXDrawBasic::DrawCapsule(wc, pCapsule->mRadius, pCapsule->mHeight, color, 0xffffffff, &mat);
			}
			break;
		case APX_SHAPETYPE_CONVEX:
			{
				APhysXConvexShapeDesc* pConvex = static_cast<APhysXConvexShapeDesc*>(pDesc);
				NxMat34 mat(m_ffMat);
				mat.multiply(m_ffMat, pConvex->mLocalPose);
				NxConvexShapeDesc descConvexShape;
				pConvex->GetNxShapeDesc(descConvexShape);
				NxConvexMesh* pMesh = static_cast<NxConvexMesh*>(descConvexShape.meshData);
				NxConvexMeshDesc descConvexMesh;
				pMesh->saveToDesc(descConvexMesh);
				APhysXDrawBasic::DrawConvex(wc, descConvexMesh, color, &mat);
			}
			break;
		}
	}
}

void CPhysXObjForceField::SetHostChannel(const APhysXCollisionChannel* pHostCNL)
{
	if (0 == pHostCNL)
		m_hostCNL = -1;
	else
		m_hostCNL = *pHostCNL;

	if (0 != m_pFF)
	{
		NxGroupsMask gm;
		APhysXScene* pScene = static_cast<APhysXScene*>(m_pFF->getScene().userData);
		if (-1 != m_hostCNL)
		{
			gm = pScene->GetCollisionChannelManager()->GetChannelGroupMask(m_hostCNL);
			gm.bits0 = ~gm.bits0;
			gm.bits1 = ~gm.bits1;
			gm.bits2 = ~gm.bits2;
			gm.bits3 = ~gm.bits3;
		}
		else
		{
			gm = pScene->GetCollisionChannelManager()->GetChannelGroupMask(APX_COLLISION_CHANNEL_COMMON);
		}
		m_pFF->setGroupsMask(gm);
	}
}
