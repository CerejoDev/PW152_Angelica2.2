// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here
//#include <A3DEngine.h>
//#include <A3DDevice.h>
//#include <A3DFXParam.h>
//#include <A3DFXMaterial.h>
//#include <A3DFXMaterialMan.h>
//#include <A3DMtlSkinBase.h>
//#include <A3DSkin.h>
//#include <A3DSkinMan.h>
//#include <A3DSkeleton.h>
//#include <A3DSkinModel.h>
//#include <A3DSkinModelAct.h>
//#include <A3DSkinModelMan.h>
//#include <A3DUnlitModel.h>
//#include <AFI.h>
//#include <AMSoundBufferMan.h>
//#include <AMSoundEngine.h>
//#include <SpeedTree4_1/ECTree.h>
//#include <SpeedTree4_1/ECTreeInst.h>
//#include <SpeedTree4_1/ECTreeForest.h>
//#include <LuaState.h>
//
//#include "EC_Model.h"
//#include "A3DGFXEx.h"
//#include "A3DGFXTexture.h"
//#include "A3DGFXExMan.h"
//#include "A3DGfxEngine.h"
//#include "A3DCombinedAction.h"
//#include "EVENT_INFO.h"
//#include "A3DSkillGfxComposer.h"
//#include "A3DSkillGfxEvent.h"
//#include "RandStringContainer.h"

#include <cassert>

#include "AFileExpDecl.h"
#include "AResFile.h"
#include "ARes.h"
#include "AResAfm.h"
#include "AResAtt.h"
#include "AResBsp.h"
#include "AResBon.h"
#include "AResChf.h"
#include "AResEcm.h"
#include "AResGfx.h"
#include "AResLmd.h"
#include "AResSingle.h"
#include "AResSki.h"
#include "AResSmd.h"
#include "AResSpt.h"
#include "AResUmd.h"
#include "AResUmds.h"
#include "AResFactory.h"
#include "AResManager.h"

#include "IResLog.h"
#include "IRes.h"
#include "IResInterface.h"
#include "IResFxMaterial.h"
#include "IResFxUIParam.h"
#include "IResComAct.h"
#include "IResComActEvent.h"
#include "IResECModel.h"
#include "IResGfx.h"
#include "IResAtt.h"
#include "IResGfxElement.h"
#include "IResSkin.h"
#include "IResSkinModel.h"
#include "IResSpeedTree.h"
#include "IResUnlitModel.h"
#include "IResUnlitModelSet.h"

extern IResLog* g_pResLog;