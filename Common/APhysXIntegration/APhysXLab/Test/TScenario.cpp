/*
 * FILE: TScenario.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

ScenarioMgr::ScenarioMgr(NxScene& scene) : IScenario(scene)
{
	m_idxCurrent = -1;
	IScenario* pS = 0;
	pS = new CCBlockerSDKScenario(scene);
	if (0 != pS) m_pvecScenario.push_back(pS);

	pS = new CCTestScenario(scene);
	if (0 != pS) m_pvecScenario.push_back(pS);

	pS = new BasicScenario(scene);
	if (0 != pS) m_pvecScenario.push_back(pS);

	pS = new KinematicScenario(scene);
	if (0 != pS) m_pvecScenario.push_back(pS);

	pS = new TeeterScenario(scene);
	if (0 != pS) m_pvecScenario.push_back(pS);
}

ScenarioMgr::~ScenarioMgr()
{
	NxArray<IScenario*>::const_iterator it = m_pvecScenario.begin();
	NxArray<IScenario*>::const_iterator itEnd = m_pvecScenario.end();
	for (; it != itEnd; ++it)
		delete *it;
	m_pvecScenario.clear();
}

void ScenarioMgr::ReleaseScenario()
{
	const NxU32 nSize = m_pvecScenario.size();
	if (nSize > m_idxCurrent) 
		m_pvecScenario[m_idxCurrent]->ReleaseScenario();
}

void ScenarioMgr::SelectNext()
{
	++m_idxCurrent;
	const NxU32 nSize = m_pvecScenario.size();
	if (nSize <= m_idxCurrent)
		m_idxCurrent = 0;
}

bool ScenarioMgr::ControlScenario(const unsigned int nChar)
{
	const NxU32 nSize = m_pvecScenario.size();
	if (nSize > m_idxCurrent) 
		return m_pvecScenario[m_idxCurrent]->ControlScenario(nChar);

	return false;
}

void ScenarioMgr::CreateScenario(CGame& rGame, const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	const NxU32 nSize = m_pvecScenario.size();
	if (nSize > m_idxCurrent)
		m_pvecScenario[m_idxCurrent]->CreateScenario(rGame, centerPos, pGroupMask);
}

void ScenarioMgr::TickScenario(float dwDeltaTimeSec)
{
	const NxU32 nSize = m_pvecScenario.size();
	if (nSize > m_idxCurrent)
		m_pvecScenario[m_idxCurrent]->TickScenario(dwDeltaTimeSec);
}

void ScenarioImp::CreateScenario(CGame& rGame, const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	m_pGame = &rGame;
	OnCreateScenario(centerPos, pGroupMask);
}

void ScenarioImp::ReleaseScenario()
{
	OnReleaseScenario();
	m_MovementCtrller.ReleaseAll(m_pGame);
}

bool ScenarioImp::ControlScenario(const unsigned int nChar)
{
	if (!m_MovementCtrller.ControlScenario(nChar))
		return OnControlScenario(nChar);

	return true;
}

void ScenarioImp::TickScenario(float dwDeltaTimeSec)
{
	m_MovementCtrller.TickScenario(dwDeltaTimeSec);
}

void BasicScenario::OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	NxActor* pActor = 0;
	
	NxBodyDesc descBody;
	descBody.flags |= NX_BF_DISABLE_GRAVITY;
	descBody.angularDamping = 0.0f;
	NxActorDesc actorDesc;
	actorDesc.body = &descBody;
	actorDesc.density = 500;
	actorDesc.globalPose.t.set(centerPos.x + 4, centerPos.y + 3, centerPos.z);
	NxBoxShapeDesc box;
	box.dimensions.set(1.5f, 0.2f, 1.5f);
	if (0 != pGroupMask)
		box.groupsMask = *pGroupMask;
	actorDesc.shapes.pushBack(&box);
	pActor = m_Scene.createActor(actorDesc);
	if (0 != pActor)
		m_MovementCtrller.Add(*pActor);

	NxBodyDesc descBody1;
	descBody1.flags |= NX_BF_DISABLE_GRAVITY;
	descBody1.angularDamping = 0.0f;
	NxActorDesc actorDesc1;
	actorDesc1.body = &descBody;
	actorDesc1.density = 500;
	actorDesc1.globalPose.t.set(centerPos.x + 4, centerPos.y + 1.5f, centerPos.z - 5);
	NxBoxShapeDesc box1;
	box1.dimensions.set(1.5f, 0.2f, 1.5f);
	if (0 != pGroupMask)
		box1.groupsMask = *pGroupMask;
	actorDesc1.shapes.pushBack(&box1);
	pActor = m_Scene.createActor(actorDesc1);
	if (0 != pActor)
		m_MovementCtrller.Add(*pActor);
	
	NxBodyDesc descBodyWall;
	descBodyWall.flags |= NX_BF_DISABLE_GRAVITY;
	descBodyWall.flags |= NX_BF_KINEMATIC;
	NxActorDesc actorDescWall;
	actorDescWall.body = &descBodyWall;
	actorDescWall.density = 1;
	actorDescWall.globalPose.t.set(centerPos.x + 15, centerPos.y + 5, centerPos.z - 5);
	NxBoxShapeDesc boxWall;
	boxWall.dimensions.set(0.2f, 2.5f, 7);
	if (0 != pGroupMask)
		boxWall.groupsMask = *pGroupMask;
	actorDescWall.shapes.pushBack(&boxWall);
	pActor = m_Scene.createActor(actorDescWall);
	if (0 != pActor)
		m_MovementCtrller.Add(*pActor, true);

	NxBodyDesc descBodyS;
	descBodyS.flags |= NX_BF_DISABLE_GRAVITY;
	NxActorDesc actorDescS;
	actorDescS.body = &descBodyS;
	actorDescS.density = 50;
	actorDescS.globalPose.t.set(centerPos.x + 4, centerPos.y + 4, centerPos.z + 8);
	NxSphereShapeDesc sphere;
	sphere.radius = 3;
	if (0 != pGroupMask)
		sphere.groupsMask = *pGroupMask;
	actorDescS.shapes.pushBack(&sphere);
	pActor = m_Scene.createActor(actorDescS);
	if (0 != pActor)
		m_MovementCtrller.Add(*pActor);

/*	NxBodyDesc descBodyCC;
	descBodyCC.mass = 0.2f;
	descBodyCC.massLocalPose.t.set(0, -0.9f, 0);
	NxComputeBoxInertiaTensor(descBodyCC.massSpaceInertia, 0.2f, 0.6f, 1.8f, 0.6f); 
	NxActorDesc actorDescCC;
	actorDescCC.body = &descBodyCC;
	actorDescCC.globalPose.t.set(pos.x + 4, pos.y + 2, pos.z);
	NxCapsuleShapeDesc capCC;
	capCC.height = 1.2f;
	capCC.radius = 0.3f;
	capCC.groupsMask = pCCM->GetChannelGroupMask(m_pPhysXScene->GetDefaultCollisionChannel());
	actorDescCC.shapes.pushBack(&capCC);
//	NxBoxShapeDesc boxCC;
//	boxCC.dimensions.set(0.2f, 0.5, 0.2f);
//	boxCC.groupsMask = pCCM->GetChannelGroupMask(m_pPhysXScene->GetDefaultCollisionChannel());
//	actorDescCC.shapes.pushBack(&boxCC);
	NxActor* a = pScene->createActor(actorDescCC);
*/
}

void KinematicScenario::OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	NxActor* pActor = 0;
	
	NxBodyDesc descBodyBoard;
	descBodyBoard.flags |= NX_BF_DISABLE_GRAVITY;
	descBodyBoard.flags |= NX_BF_KINEMATIC;
	NxActorDesc actorDescBoard;
	actorDescBoard.body = &descBodyBoard;
	actorDescBoard.density = 1;
	actorDescBoard.globalPose.t.set(centerPos.x + 5, centerPos.y + 3, centerPos.z);
	NxBoxShapeDesc boxBoard;
	boxBoard.dimensions.set(4, 0.2f, 4);
	if (0 != pGroupMask)
		boxBoard.groupsMask = *pGroupMask;
	actorDescBoard.shapes.pushBack(&boxBoard);
	pActor = m_Scene.createActor(actorDescBoard);
	if (0 != pActor)
		m_MovementCtrller.Add(*pActor);

	NxBodyDesc descBodyBoard2;
	descBodyBoard2.flags |= NX_BF_DISABLE_GRAVITY;
	descBodyBoard2.flags |= NX_BF_KINEMATIC;
	NxActorDesc actorDescBoard2;
	actorDescBoard2.body = &descBodyBoard2;
	actorDescBoard2.density = 1;
	actorDescBoard2.globalPose.t.set(centerPos.x - 2, centerPos.y + 3, centerPos.z);
	NxBoxShapeDesc boxBoard2;
	boxBoard2.dimensions.set(0.5f, 0.2f, 0.5f);
	if (0 != pGroupMask)
		boxBoard2.groupsMask = *pGroupMask;
	actorDescBoard2.shapes.pushBack(&boxBoard2);
	pActor = m_Scene.createActor(actorDescBoard2);
	if (0 != pActor)
		m_MovementCtrller.Add(*pActor);
}

void TeeterScenario::OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	m_GM.bits0 = m_GM.bits1 = m_GM.bits2 = m_GM.bits3 = 0;
	if (0 != pGroupMask)
		m_GM = *pGroupMask;

	const float YOffset = 0.5f;

	NxBodyDesc descBody;
	descBody.flags |= NX_BF_KINEMATIC;
	NxActorDesc actorDesc;
	actorDesc.body = &descBody;
	actorDesc.density = 1;
	actorDesc.globalPose.t.set(centerPos.x + 5, centerPos.y + YOffset, centerPos.z + 1);
	NxBoxShapeDesc box;
	box.dimensions.set(0.2f, 1.0f, 0.1f);
	box.groupsMask = m_GM;
	actorDesc.shapes.pushBack(&box);
	NxActor* pRBZP = m_Scene.createActor(actorDesc);
	if (0 != pRBZP)
		m_MovementCtrller.Add(*pRBZP, true);

/*	NxBodyDesc descBodyZN;
	descBodyZN.flags |= NX_BF_KINEMATIC;
	NxActorDesc actorDescZN;
	actorDescZN.body = &descBodyZN;
	actorDescZN.density = 1;
	actorDescZN.globalPose.t.set(centerPos.x + 5, centerPos.y + YOffset, centerPos.z - 1);
	NxBoxShapeDesc boxZN;
	boxZN.dimensions.set(0.2f, 1.0f, 0.1f);
	boxZN.groupsMask = m_GM;
	actorDescZN.shapes.pushBack(&boxZN);
	NxActor* pRBZN = m_Scene.createActor(actorDescZN);
	if (0 != pRBZN)
		m_MovementCtrller.Add(*pRBZN);*/

	NxBodyDesc descBodyCTB;
	NxActorDesc actorDescCTB;
	actorDescCTB.body = &descBodyCTB;
	actorDescCTB.density = 1;
	actorDescCTB.globalPose.t.set(centerPos.x + 5, centerPos.y + YOffset, centerPos.z);
	NxBoxShapeDesc boxCTB;
	boxCTB.dimensions.set(4.0f, 0.1f, 0.9f);
	boxCTB.groupsMask = m_GM;
	actorDescCTB.shapes.pushBack(&boxCTB);
	NxActor* pBoard = m_Scene.createActor(actorDescCTB);
	if (0 != pBoard)
		m_MovementCtrller.Add(*pBoard, true);
	m_SideActorPos = actorDescCTB.globalPose.t;
	m_SideActorPos.x += boxCTB.dimensions.x - 0.5f;
	m_SideActorPos.y += 4;

	NxVec3 globalAnchorZP(centerPos.x + 5, centerPos.y + YOffset, centerPos.z + 0.9f);
	NxVec3 globalAxisZP(0, 0, -1);
	NxCylindricalJointDesc cylDescZP;
    cylDescZP.actor[0] = pRBZP;
    cylDescZP.actor[1] = pBoard;
    cylDescZP.setGlobalAnchor(globalAnchorZP);
    cylDescZP.setGlobalAxis(globalAxisZP);
    m_pJoint = m_Scene.createJoint(cylDescZP);
    m_pJoint->setLimitPoint(globalAnchorZP);
	m_pJoint->addLimitPlane(-globalAxisZP, globalAnchorZP);
	m_pJoint->addLimitPlane(globalAxisZP, globalAnchorZP);
}

void TeeterScenario::OnReleaseScenario()
{
	if (0 != m_pJoint)
	{
		m_Scene.releaseJoint(*m_pJoint);
		m_pJoint = 0;
	}
}

bool TeeterScenario::OnControlScenario(const unsigned int nChar)
{
	if ('I' != nChar)
		return false;

	NxBodyDesc descBodyT;
	NxActorDesc actorDescT;
	actorDescT.body = &descBodyT;
	actorDescT.density = 1;
	actorDescT.globalPose.t = m_SideActorPos;
	NxBoxShapeDesc boxT;
	boxT.dimensions.set(0.5f, 0.5f, 2);
	boxT.groupsMask = m_GM;
	actorDescT.shapes.pushBack(&boxT);
	NxActor* pBox = m_Scene.createActor(actorDescT);
	if (0 != pBox)
		m_MovementCtrller.Add(*pBox);

	return true;
}

void CCTestScenario::OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	m_GM.bits0 = m_GM.bits1 = m_GM.bits2 = m_GM.bits3 = 0;
	if (0 != pGroupMask)
		m_GM = *pGroupMask;

	if (0 == m_pGame)
		return;

	NxActor* pActor = 0;
	IPhysXObjBase* pObj = 0;
	NxVec3 pos(centerPos);

	pos.x -= 11;
	pos.y += 1.5;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\µõÇÅ\\µõÇÅ.ecm", pos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, false, &pActor);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH, true);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE, true);
			m_MovementCtrller.Add(*pObj);
		}
	}

	pos = centerPos;
	pos.x += 5.5f;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\õÎõÎ°å\\õÎõÎ°å.ecm", pos, true, false);
	if (0 != pObj)
	{
		pObj->SetScale(1.8f);
		pObj->EnterRuntime(*m_pGame->GetAPhysXScene());

		m_SideActorPos = APhysXConverter::A2N_Vector3(pObj->GetPos());
		m_SideActorPos.x += 3.5f;
		m_SideActorPos.y += 4;

		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, false, &pActor);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH, true);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE, true);
			m_MovementCtrller.Add(*pObj);
		}
	}

	pos = centerPos;
	pos.x -= 3;
	pos.z -= 5;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\´óÄ¾°å\\´óÄ¾°å.ecm", pos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, true, &pActor);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH, true);
			pDynObj->SetMotionDistance(100);
			pDynObj->SetControlGroup(0);
			m_MovementCtrller.Add(*pObj);
		}
	}

	pos = centerPos;
	pos.x -= 3;
	pos.z -= 10;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\´óÄ¾°å\\´óÄ¾°å.ecm", pos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, true, &pActor);
			pDynObj->SetMotionDistance(100);
			pDynObj->SetControlGroup(0);
			m_MovementCtrller.Add(*pObj);
		}
	}
}

bool CCTestScenario::OnControlScenario(const unsigned int nChar)
{
	if ('I' != nChar)
		return false;

	NxActor* pActor = 0;
	IPhysXObjBase*	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\Ïä×Ó\\Ïä×Ó.ecm", m_SideActorPos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, false, &pActor);
			m_MovementCtrller.Add(*pObj);
		}
	}

	return true;
}

void CCBlockerSDKScenario::OnReleaseScenario()
{
	if (0 != m_pActorS)
	{
		m_Scene.releaseActor(*m_pActorS);
		m_pActorS = 0;
	}
	if (0 != m_pActorD)
	{
		m_Scene.releaseActor(*m_pActorD);
		m_pActorD = 0;
	}
	if (0 != m_pActorK)
	{
		m_Scene.releaseActor(*m_pActorK);
		m_pActorK = 0;
	}
}

void CCBlockerSDKScenario::OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask)
{
	m_GM.bits0 = m_GM.bits1 = m_GM.bits2 = m_GM.bits3 = 0;
	if (0 != pGroupMask)
		m_GM = *pGroupMask;
	m_GMReverse.bits0 = ~m_GM.bits0;
	m_GMReverse.bits1 = ~m_GM.bits1;
	m_GMReverse.bits2 = ~m_GM.bits2;
	m_GMReverse.bits3 = ~m_GM.bits3;

	if (0 == m_pGame)
		return;

	NxActor* pActor = 0;
	IPhysXObjBase* pObj = 0;
	NxVec3 pos(centerPos);

	pos = centerPos;
	pos.x -= 3;
	pos.z -= 5;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\´óÄ¾°å\\´óÄ¾°å.ecm", pos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, true, &pActor);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH, true);
			pDynObj->SetControlGroup(0);
			m_MovementCtrller.Add(*pObj);
		}
	}

	pos = centerPos;
	pos.x -= 3;
	pos.z -= 10;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\´óÄ¾°å\\´óÄ¾°å.ecm", pos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, true, &pActor);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH, true);
			pDynObj->SetControlGroup(0);
			m_MovementCtrller.Add(*pObj);
		}
	}

	pos = centerPos;
	pos.x -= 3;
	pos.z -= 15;
	pObj = m_pGame->CreateObjectRuntime(CPhysXObjMgr::OBJ_TYPEID_ECMODEL, "Models\\PhysXComponents\\´óÄ¾°å\\´óÄ¾°å.ecm", pos, false, true);
	if (0 != pObj)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pDynObj)
		{
			pDynObj->UpdateNxActorsAttributes(&m_GM, true, &pActor);
			pDynObj->GetSkeletonRBObject()->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH, true);
			pDynObj->SetControlGroup(0);
			m_MovementCtrller.Add(*pObj);
		}
	}

	CreateTarget(NxVec3(centerPos.x + 10, centerPos.y + 1, centerPos.z - 5));
}
void CCBlockerSDKScenario::CreateTarget(const NxVec3& targetPos)
{
	OnReleaseScenario();
	NxVec3 pos(targetPos);

	NxActorDesc actorDescS;
	actorDescS.globalPose.t.set(pos);
	NxBoxShapeDesc boxS;
	boxS.groupsMask = m_GM;
	boxS.dimensions.set(1, 0.2f, 1);
	actorDescS.shapes.pushBack(&boxS);
	m_pActorS = m_Scene.createActor(actorDescS);

	pos = targetPos;
	pos.z -= 5;
	NxBodyDesc descBodyD;
	descBodyD.flags |= NX_BF_DISABLE_GRAVITY;
	NxActorDesc actorDescD;
	actorDescD.body = &descBodyD;
	actorDescD.density = 1;
	actorDescD.globalPose.t.set(pos);
	NxBoxShapeDesc boxD;
	boxD.groupsMask = m_GM;
	boxD.dimensions.set(1, 0.2f, 1);
	actorDescD.shapes.pushBack(&boxD);
	m_pActorD = m_Scene.createActor(actorDescD);
	
	pos = targetPos;
	pos.z -= 10;
	NxBodyDesc descBodyK;
	descBodyK.flags |= NX_BF_DISABLE_GRAVITY;
	descBodyK.flags |= NX_BF_KINEMATIC;
	NxActorDesc actorDescK;
	actorDescK.body = &descBodyK;
	actorDescK.density = 1;
	actorDescK.globalPose.t.set(pos);
	NxBoxShapeDesc boxK;
	boxK.groupsMask = m_GM;
	boxK.dimensions.set(1, 0.2f, 1);
	actorDescK.shapes.pushBack(&boxK);
	m_pActorK = m_Scene.createActor(actorDescK);

	PrintTargetGM();
}

bool CCBlockerSDKScenario::OnControlScenario(const unsigned int nChar)
{
	bool rtn = false;
	AString str;
	NxShape* pShape = 0;
	NxGroupsMask gm;

	if ('I' == nChar)
	{
		rtn = true;
		if (0 != m_pActorS)
		{
			pShape = m_pActorS->getShapes()[0];
			gm = pShape->getGroupsMask();
			if ((gm.bits0 == m_GM.bits0) && (gm.bits1 == m_GM.bits1) && (gm.bits2 == m_GM.bits2) && (gm.bits3 == m_GM.bits3))
				pShape->setGroupsMask(m_GMReverse);
			else
				pShape->setGroupsMask(m_GM);
		}
			
		if (0 != m_pActorD)
		{
			pShape = m_pActorD->getShapes()[0];
			gm = pShape->getGroupsMask();
			if ((gm.bits0 == m_GM.bits0) && (gm.bits1 == m_GM.bits1) && (gm.bits2 == m_GM.bits2) && (gm.bits3 == m_GM.bits3))
				pShape->setGroupsMask(m_GMReverse);
			else
				pShape->setGroupsMask(m_GM);
		}
		
		if (0 != m_pActorK)
		{
			pShape = m_pActorK->getShapes()[0];
			gm = pShape->getGroupsMask();
			if ((gm.bits0 == m_GM.bits0) && (gm.bits1 == m_GM.bits1) && (gm.bits2 == m_GM.bits2) && (gm.bits3 == m_GM.bits3))
				pShape->setGroupsMask(m_GMReverse);
			else
				pShape->setGroupsMask(m_GM);
		}
		PrintTargetGM();
	}
	else if ('U' == nChar)
	{
		rtn = true;
		NxVec3 pos;
		if (m_MovementCtrller.GetFirstActorPosInGeneralGroup(pos))
			CreateTarget(NxVec3(pos.x + 10, pos.y + 1, pos.z));
	}

	return rtn;
}

void CCBlockerSDKScenario::PrintTargetGM()
{
	ACString str;
	NxGroupsMask gm;

	if (0 != m_pActorS)
	{
		gm = m_pActorS->getShapes()[0]->getGroupsMask();
		str.Format(_T("---Now m_pActorS GM is %x, %x, %x, %x \n"), gm.bits0, gm.bits1, gm.bits2, gm.bits3);
		OutputDebugString(str);
	}
	if (0 != m_pActorD)
	{
		gm = m_pActorD->getShapes()[0]->getGroupsMask();
		str.Format(_T("---Now m_pActorD GM is %x, %x, %x, %x \n"), gm.bits0, gm.bits1, gm.bits2, gm.bits3);
		OutputDebugString(str);
	}
	if (0 != m_pActorK)
	{
		gm = m_pActorK->getShapes()[0]->getGroupsMask();
		str.Format(_T("---Now m_pActorK GM is %x, %x, %x, %x \n"), gm.bits0, gm.bits1, gm.bits2, gm.bits3);
		OutputDebugString(str);
	}
}
