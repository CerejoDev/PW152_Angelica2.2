// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here

#include <cassert>

#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DFXParam.h>
#include <A3DFXMaterial.h>
#include <A3DFXMaterialMan.h>
#include <A3DSkin.h>
#include <A3DSkinMan.h>
#include <A3DSkeleton.h>
#include <A3DSkinModel.h>
#include <A3DSkinModelAct.h>
#include <A3DSkinModelMan.h>
#include <A3DUnlitModel.h>
#include <AFI.h>
#include <AMSoundBufferMan.h>
#include <AMSoundEngine.h>
#include <SpeedTree4_1/ECTree.h>
#include <SpeedTree4_1/ECTreeInst.h>
#include <SpeedTree4_1/ECTreeForest.h>
#include <LuaWrapper/LuaState.h>

#include "EC_Model.h"
#include "EC_ModelStaticData.h"
#include "A3DGFXEx.h"
#include "A3DGFXTexture.h"
#include "A3DGFXExMan.h"
#include "A3DGfxEngine.h"
#include "A3DCombinedAction.h"
#include "EVENT_INFO.h"
#include "A3DSkillGfxComposer.h"
#include "A3DSkillGfxEvent.h"
#include "RandStringContainer.h"