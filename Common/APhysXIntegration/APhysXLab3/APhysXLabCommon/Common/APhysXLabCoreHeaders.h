/*
 * FILE: APhysXLabCoreHeaders.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/02/01
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

// Note: Do not include this file in head files!

#pragma once
#ifndef _APHYSXLAB_CORE_HEADERS_H_
#define _APHYSXLAB_CORE_HEADERS_H_

static int GetAngelicaSDKVersion()
{
	int ver = 0;
#ifdef _ANGELICA21
		ver = 0x02010000;
#endif
#ifdef _ANGELICA22
		ver = 0x02020000
#endif
#ifdef _ANGELICA3
		ver = 0x03000000;
#endif
	return ver;
}

#include <limits>

#ifndef _T
	#ifdef _UNICODE
		#define _T(x)  L ## x
	#else
		#define _T(x) (x)
	#endif
#endif

#ifdef UNICODE
	#define _TAS2WC(x)   AS2WC(x)
	#define _TWC2AS(x)   WC2AS(x)
	#define _TWC2TSting(x)   x
	#define _TAS2TSting(x)   AS2WC(x)
	#define	std_TString		std::wstring
	#define	TString		AWString
#else
	#define _TAS2WC(x)   x
	#define _TWC2AS(x)   x
	#define _TWC2TSting(x)   WC2AS(x)
	#define _TAS2TSting(x)   x
	#define	std_TString		std::string
	#define	TString		AString
#endif

#include <APhysX.h>

#include <A3DMatrix.h>
#include <A3DLitModel.h>
#include <AC.h>

#if defined(_ANGELICA3)
	#include <A3DEnvironment.h>
#endif
#include <A3DShadowMap.h>

#include <EC_Model.h>
#include <A3DModelPhysics.h>
#include <A3DModelPhysSync.h>
#include <A3DSkinPhysSync.h>
#include <A3DModelPhysSyncData.h>
#include <ClothSkinConfig.h>

#include "Render.h"
#include "RayTrace.h"
#include "RenderInterface.h"
#include "EngineInterface.h"

#include "ApxObjBase.h"
#include "ReportInterface.h"
#include "PropsGrid.h"

#include "ObjManager.h"
#include "IPropObjBase.h"
#include "IPropPhysXObjBase.h"
#include "PropPhysXObjImp.h"
#include "PropRgnObjImp.h"

#include "ObjTask.h"
#include "IObjBase.h"
#include "ObjDummy.h"
#include "Region.h"
#include "PhysXObjBase.h"
#include "PhysXObjStatic.h"
#include "PhysXObjDynamic.h"
#include "PhysXObjSkinModel.h"
#include "PhysXObjECModel.h"
#include "PhysXObjParticle.h"
#include "PhysXObjForceField.h"
#include "ApxObjSkinModelSocket.h"
#include "ApxObjSpotLight.h"


#include "PhysXObjSelGroup.h"
#include "UserEvent.h"
#include "Trigger.h"
#include "CloneOperation.h"

#include "CameraController.h"
#include "ActionPlayerBase.h"

// for actor's action and move controlling
#include "ApxActorBase.h"
#include "ApxSkinModelActionController.h"
#include "ActorMoveCtrlImp.h"
#include "ActorMoveControllerImp.h"

#include "MainActor.h"
#include "Game.h"

#endif


