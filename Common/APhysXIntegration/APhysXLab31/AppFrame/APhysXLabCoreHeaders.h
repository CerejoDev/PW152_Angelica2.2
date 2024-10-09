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

#include <limits>

#ifdef UNICODE
	#define _TAS2WC(x)   AS2WC(x)
	#define _TWC2AS(x)   WC2AS(x)
#else
	#define _TAS2WC(x)   x
	#define _TWC2AS(x)   x
#endif

#include <APhysX.h>

#include <A3DMatrix.h>
#include <A3DLitModel.h>
#include <AC.h>
#include <A3DEnvironment.h>
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

#include "RawObjManager.h"
#include "GameProperty.h"
#include "IPropObjBase.h"
#include "IPropPhysXObjBase.h"
#include "PropPhysXObjImp.h"
#include "PropRgnObjImp.h"

#include "RegionMgr.h"
#include "IObjBase.h"
#include "Region.h"
#include "PhysXObjBase.h"
#include "PhysXObjMgr.h"
#include "PhysXObjStatic.h"
#include "PhysXObjDynamic.h"
#include "PhysXObjSkinModel.h"
#include "PhysXObjECModel.h"
#include "PhysXObjParticle.h"
//#include "PhysXObjForceField.h"

#include "UserEvent.h"
#include "Trigger.h"
#include "Game.h"
#include "CameraController.h"

#include "ActionPlayerBase.h"
#include "MainActor.h"

#endif


