// APhysXDebugRenderTestView.cpp : implementation of the CAPhysXDebugRenderTestView class
//



//#define NOMINMAX


#include "stdafx.h"

// #include <vld.h>
// #include <typeinfo.h>

#include "APhysXDebugRenderTest.h"

#include "APhysXDebugRenderTestDoc.h"
#include "APhysXDebugRenderTestView.h"

#include "Global.h"
#include "Render.h"

#include "vector"

// #include "LuaState.h"
// #include "EC_Model.h"
#include "A3DModelPhysics.h"
#include "A3DModelPhysSync.h"


// #include <iostream>


// #include <GL/glut.h>

//#include "assertx.h"

//#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC80.CRT' version='8.0.50608.0' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIME_TICKANIMATION	30		//	ms
#define CONVEX_OBJ_DESC_NUM 3
static char s_szFileName[] = "wftest.pxd";
static char s_szJointedObjectsFileName[] = "wftest_jointed_obj.pxd";
static char s_szSkeletonFileName[] = "Skeleton_Male.pxd";
// static char s_szBreakableSkeletonFileName[] = "Breakable_Stone_Gate.pxd";
static char s_szBreakableSkeletonFileName[] = "BB.pxd";

static char s_szConvexObjDescPrefix[] = "ConvexObjDesc";
static char s_szConvexShapeDescPrefix[] = "ConvexShapeDesc";

static bool gHasHWPhysX = false;

static TCHAR szFPSBuffer[256];

static APhysXMutex s_MutexTest;

static NxActor* s_pControledActor = NULL;
static APhysXRBAttacher* s_gRBAttacher = NULL;

static APhysXRBAttacher* s_pRBAttacher = NULL;

static APhysXScene* s_pDummyScene = NULL;

static NxConvexMesh* s_pConvexMesh = NULL;


static APhysXArray<NxActor* > s_OverlappedActors;
static APhysXArray<NxVec3> s_OverlapNormals;

static APhysXArray<NxActor* > s_NeighborActors;


inline static bool KeyPressed(int InKey)
{
	return ( GetAsyncKeyState(InKey) & 0x8000 );
}

static bool s_bRunStepByStep = false;
static bool s_bStepForward = false;

static APhysXCharacterController* s_pAPhysXCC = NULL;

// 这里用到的网络延时方案不太正确，需要重新设计！

//---------------------------------------------------------
// define some network physics stuff just testing...

class APhysXNetworkActor
{

struct PhysXState
{
	NxVec3 mPos;
	NxVec3 mlv;
	NxQuat mOrt;
	NxVec3 mav;
	bool mSleep;

	float mLastInterv;		// 当前状态快照距离上一次状态快照的时间间隔...

	float mTimeStamp;
};

public:
/*
	APhysXNetworkActor(NxActor* pActor, NxActor* pProxyActor, float fUpdateInterv = 0.5f): m_pActor(pActor), m_pProxyActor(pProxyActor),m_fUpdateInterv(fUpdateInterv), m_bSleep(false)
	{

	}
	~APhysXNetworkActor() {}
*/

	void Init(APhysXRigidBodyObject* pAuthorRB, APhysXRigidBodyObject* pProxyRB, bool bCorrectDiffInterv = false, float fUpdateInterv = 0.5f)
	{
		m_pAuthorRBObject = pAuthorRB;
		m_pProxyRBObject = pProxyRB;
		m_fUpdateInterv = fUpdateInterv;
		m_bSleep = false;

		m_bCorrectDiffInterv = bCorrectDiffInterv;
	}


	void Tick(float dt)
	{
		static float s_dtCounter = 0.0f;
		static float s_dtRcvCounter = 0.0f;	
		static float s_fRcvInterv = 0.0f;
		static float s_fRcvLag = 1.0f;

		static float s_fCurTimeStamp = 0.0f;

		s_fCurTimeStamp += dt;

		// store the state at the interval of m_fUpdateInterv...
		if(s_dtCounter >= m_fUpdateInterv)
		{
			StoreRBState(s_fCurTimeStamp);
			s_dtCounter = 0.0f;
		}
		else
			s_dtCounter += dt;

		// simulate the receive events and sync the state to actors...
		if(m_arrStates.size())
		{
			float fSendTimeStamp = m_arrStates[0].mTimeStamp;

			if(s_fCurTimeStamp > fSendTimeStamp + s_fRcvLag)
			{
				s_fRcvInterv = s_fCurTimeStamp - s_fRcvInterv;
				SyncRBState(s_fRcvInterv);
				
				// generate some net lag...
				// s_fRcvLag = NxMath::rand(0.0f, 0.3f);
				s_fRcvLag = 1.0f;

				s_fRcvInterv = s_fCurTimeStamp;
			}


/*
			if(s_dtRcvCounter >= s_fRcvLag)
			{
				// a simple sample...
				SyncState2Actor(m_pProxyActor, s_fRcvInterv);

				s_dtRcvCounter = 0;
				
				// generate some net lag...
				// s_fRcvLag = NxMath::rand(0.0f, 0.3f);

				// clear the r
				s_fRcvInterv = 0.0f;

			}
			else
				s_dtRcvCounter += dt;

*/

		}

		s_fRcvInterv += dt;


	}

	// Parameter: fLastSyncInterv, the interval time from last sync...
	void SyncRBState(float fLastSyncInterv)
	{
		if(!m_pProxyRBObject) return;
		
		// make sure the state array is not empty...
		APHYSX_ASSERT(m_arrStates.size());

		if(m_arrStates.size())
		{

			for(int i=0; i<m_pProxyRBObject->GetNxActorNum(); i++)

			{

				NxActor* pProxyActor = m_pProxyRBObject->GetNxActor(i);

				// pop the first 
				PhysXState ps = m_arrStates[0];
				m_arrStates.erase(m_arrStates.begin(), m_arrStates.begin()+1);

				// t is a parameter that controls the physics state snap effect...
				// t = 0, do not snap the pos/orientation but revise the velocity...
				// t = 1, snap the pos to the target pos directly
				float t = 0.1f;

				if(ps.mSleep) 
					t = 1.0f;

				// if(ps.mLastInterv>0 && 	fabs(fLastSyncInterv - ps.mLastInterv)>0.5f)
				// 	t = 0.01f;

				// try to revise the target state by the different interval time between the two store events and the two sync events...
				if(m_bCorrectDiffInterv && ps.mLastInterv > 0.0f)	// else, it is the first sync, just use it as the target state...
				{
					float fLerpDt = fLastSyncInterv - ps.mLastInterv;

					// interpolate or extrapolate the pos and orientation states...
					ps.mPos += fLerpDt * ps.mlv;

					NxVec3 vRotAxis = fLerpDt * ps.mav;
					float fRotDegree = vRotAxis.normalize();

					if(!vRotAxis.isZero())
					{
						fRotDegree = NxMath::radToDeg(fRotDegree);
						NxQuat qRot(fRotDegree, vRotAxis);
						ps.mOrt.multiply(qRot, ps.mOrt);
						ps.mOrt.normalize();

					}
				}

				// revise the linear velocity
				NxVec3 vDelta = ps.mPos - pProxyActor->getGlobalPosition();
				vDelta *= 1 - t;
				vDelta /= m_fUpdateInterv;
				pProxyActor->setLinearVelocity(ps.mlv + vDelta);


				// revise the pos
				NxVec3 vRevisePos = APhysXUtilLib::Lerp(pProxyActor->getGlobalPosition(), ps.mPos, t);
				pProxyActor->setGlobalPosition(vRevisePos);


				// revise the orientation
				NxQuat qDelta, qProxy = pProxyActor->getGlobalOrientationQuat();
				NxQuat qReviseOrt;
				qReviseOrt.slerp(t, qProxy, ps.mOrt);
				pProxyActor->setGlobalOrientationQuat(qReviseOrt);


				// revise the angular velocity...
				qProxy.invert();
				qDelta.multiply(ps.mOrt, qProxy);
				qDelta.normalize();

				float fAngel;
				NxVec3 vAxis;
				qDelta.getAngleAxis(fAngel, vAxis);
				fAngel *= 1-t;
				fAngel = NxMath::degToRad(fAngel);
				fAngel /= m_fUpdateInterv;
				NxVec3 vAV = ps.mav + fAngel * vAxis;
				pProxyActor->setAngularVelocity(vAV);


			}
			
		}
	}

protected:

	void StoreRBState(float fCurTimeStamp)
	{
		if(!m_pAuthorRBObject) return;

		bool bStore = true;
		if(m_pAuthorRBObject->IsSleeping())
		{
			if(m_bSleep)
				bStore = false;
			else
				m_bSleep = true;
		}else
			m_bSleep = false;

		if(bStore)
		{
			PhysXState ps;
			ps.mSleep = m_bSleep;

			static bool bFirst = true;
			if(bFirst)
			{
				ps.mLastInterv = 0.0f;
				bFirst = false;
			}
			else
				ps.mLastInterv = m_fUpdateInterv;

			ps.mTimeStamp = fCurTimeStamp;


			for(int i = 0; i<m_pAuthorRBObject->GetNxActorNum(); i++)
			{
				NxActor* pActor = m_pAuthorRBObject->GetNxActor(i);

				ps.mPos = pActor->getGlobalPosition();
				ps.mOrt = pActor->getGlobalOrientationQuat();

				ps.mlv = pActor->getLinearVelocity();
				ps.mav = pActor->getAngularVelocity();

				m_arrStates.push_back(ps);

			}


			/*
			// to reduce the sync data which should be sent to network
			// the smooth operation should be 

			// try to smooth to the sleep state...
			if(m_bSleep)
			{
				int iSmoothCounter = 10;
				while(iSmoothCounter > 0)
				{
					ps.mSleep = false;
					m_arrStates.push_back(ps);

					iSmoothCounter--;
				}

				ps.mSleep = true;
				m_arrStates.push_back(ps);

			}
			else
				m_arrStates.push_back(ps);		// push_back the state directly...

			*/

		}

	}


protected:

	// NxActor* m_pActor;
	// NxActor* m_pProxyActor;
	
	APhysXRigidBodyObject* m_pAuthorRBObject;
	APhysXRigidBodyObject* m_pProxyRBObject;
	

	float m_fUpdateInterv;
	bool m_bSleep;
	bool m_bCorrectDiffInterv;
	NxArray<PhysXState> m_arrStates;
};

static APhysXNetworkActor s_apxNetworkActor;
static APhysXNetworkActor s_apxNetworkActorCorrected;

static APhysXRigidBodyObject* s_pAuthorRB = NULL;
static APhysXRigidBodyObject* s_pProxyRB = NULL;
static APhysXRigidBodyObject* s_pProxyRBCorrected = NULL;

//---------------------------------------------------------
// define my own joint breaking deal...
class MyUserNotify: public NxUserNotify
{
public:
	virtual bool onJointBreak(NxReal breakingForce, NxJoint & brokenJoint)
	{

		return false; //dont delete the joint
	}

	void onWake (NxActor **actors, NxU32 count) {}
	void onSleep (NxActor **actors, NxU32 count) {}

};

static MyUserNotify s_MyUserNotify;

//---------------------------------------------------------
// define my own contact handle function...

#define MAX_BREAKING_FORCE 100

class MyUserContactReport:public NxUserContactReport
{
public:
	virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
	{
		APhysXRigidBodyObject* pRBObj = NULL;
		NxActor* pHitActor = NULL;
		if(pair.actors[0]->readBodyFlag(NX_BF_KINEMATIC))
		{
			pRBObj = (APhysXRigidBodyObject*)pair.actors[0]->userData;
			pHitActor = pair.actors[0];
		}
		else if(pair.actors[1]->readBodyFlag(NX_BF_KINEMATIC))
		{
			pRBObj = (APhysXRigidBodyObject*)pair.actors[1]->userData;
			pHitActor = pair.actors[1];
		}

		if(pRBObj && pair.sumNormalForce.magnitude() > MAX_BREAKING_FORCE)
		{
			// trigger the breaking...
			pRBObj->EnableKinematic(false);
			pHitActor->addForce(-(pair.sumNormalForce + pair.sumFrictionForce));
		}

	}

};
static MyUserContactReport s_MyUserContactReport;

/*
void DrawBox(A3DWireCollector& a3dwc, const NxBoxShape& box, const NxU32& color)
{
	// test..
}
*/


static void VisualizeNxActor(NxActor* pActor, bool bEnableVisualize = true)
{

	if(!pActor) return;

	int iCurShape = pActor->getNbShapes();

	while(iCurShape--)
	{
		NxShape* pCurShape = pActor->getShapes()[iCurShape];

		pCurShape->setFlag(NX_SF_VISUALIZATION, bEnableVisualize);
	}


}

static bool GetOverlapActors(NxActor* pActor, APhysXArray<NxActor* >& OverlapActors, APhysXArray<NxVec3 >& OverlapNormals)
{


#define WF_SWEEP_EPSILON 0.1f

#if 0

	// X+ direction...
	myOverlapDetector.LinearSweep(*pActor, NxVec3(1.0f, 0, 0), WF_SWEEP_EPSILON, HitShapes, errCode);
	if(errCode == COverlapDetector::E_SUCCESS)
	{
		for(unsigned int i=0; i<HitShapes.size(); i++)
			APhysXUtilLib::APhysXArray_PushOnce(OverlapActors, &(HitShapes[i]->getActor()));

	}
	HitShapes.clear();


	// X- direction...
	myOverlapDetector.LinearSweep(*pActor, NxVec3(1.0f, 0, 0), -WF_SWEEP_EPSILON, HitShapes, errCode);
	if(errCode == COverlapDetector::E_SUCCESS)
	{
		for(unsigned int i=0; i<HitShapes.size(); i++)
			APhysXUtilLib::APhysXArray_PushOnce(OverlapActors, &(HitShapes[i]->getActor()));

	}
	HitShapes.clear();

	// Y+ direction...
	myOverlapDetector.LinearSweep(*pActor, NxVec3(0, 1.0f, 0), WF_SWEEP_EPSILON, HitShapes, errCode);
	if(errCode == COverlapDetector::E_SUCCESS)
	{
		for(unsigned int i=0; i<HitShapes.size(); i++)
			APhysXUtilLib::APhysXArray_PushOnce(OverlapActors, &(HitShapes[i]->getActor()));

	}
	HitShapes.clear();

	// Y- direction...
	myOverlapDetector.LinearSweep(*pActor, NxVec3(0, 1.0f, 0), -WF_SWEEP_EPSILON, HitShapes, errCode);
	if(errCode == COverlapDetector::E_SUCCESS)
	{
		for(unsigned int i=0; i<HitShapes.size(); i++)
			APhysXUtilLib::APhysXArray_PushOnce(OverlapActors, &(HitShapes[i]->getActor()));

	}
	HitShapes.clear();

	// Z+ direction...
	myOverlapDetector.LinearSweep(*pActor, NxVec3(0, 0, 1.0f), WF_SWEEP_EPSILON, HitShapes, errCode);
	if(errCode == COverlapDetector::E_SUCCESS)
	{
		for(unsigned int i=0; i<HitShapes.size(); i++)
			APhysXUtilLib::APhysXArray_PushOnce(OverlapActors, &(HitShapes[i]->getActor()));

	}
	HitShapes.clear();

	// Z- direction...
	myOverlapDetector.LinearSweep(*pActor, NxVec3(0, 0, 1.0f), -WF_SWEEP_EPSILON, HitShapes, errCode);
	if(errCode == COverlapDetector::E_SUCCESS)
	{
		for(unsigned int i=0; i<HitShapes.size(); i++)
			APhysXUtilLib::APhysXArray_PushOnce(OverlapActors, &(HitShapes[i]->getActor()));

	}
	HitShapes.clear();

#else

	NxArray<APhysXUtilLib::APhysXDetailOverlapInfo> arrDOInfo;
	if(APX_OTRES_TRUE == APhysXUtilLib::CheckOverlap_Inflate(*pActor, WF_SWEEP_EPSILON, OverlapActors, &arrDOInfo))
	{
		for(unsigned int i=0; i<arrDOInfo.size(); i++)
		{
			OverlapNormals.push_back(arrDOInfo[i].mOverlapNormal);
		}

	}


#endif


	return true;
}

//---------------------------------------------------------

class MyBase
{
public:
	
	static int si;

	virtual void Set(int& i)
	{
		i = si;
	}

	void NVSet(int& i)
	{
		i = si;
	}

	static void StaticSet(int& i)
	{
		i = si;
	}


};

class MyChild:public MyBase
{
public:
	// static int si;
	virtual void Set(int& i)
	{
		i = si;
	}

	void NVSet(int& i)
	{
		i = si;
	}

	static void StaticSet(int& i)
	{
		i = si;
	}
};

// int MyBase::si = 1;
int MyChild::si = 11;

class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}
private:  // emphasize the following members are private
	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
};

class NCChild : public noncopyable
{
};

static NCChild& TestFunc(NCChild& ncChild)
{
	int i= 0;

	return ncChild;
}


class CTestRefReturn
{
public:

	NxVec3& GetPosRef()
	{
		NxVec3 vPos(1.0f);
		return vPos;
	}

	NxVec3 GetPos()
	{
		NxVec3 vPos(1.0f);
		return vPos;
	}

};

class MyTestNxArray
{

public:
	
	void Add(int* p) { myIntArray.push_back(p);}

protected:

	NxArray<int *> myIntArray;
};

class CStackCheck
{
protected:
	CStackCheck(int _temp = 0)
	{
		assert((unsigned)this - (unsigned)&_temp < 1024 && "对象没有创建在栈上");
	}
};

class CMyClass : CStackCheck
{

public:

	CMyClass()
	{
		//char buffer[2048];
		//...
	}

	// char buffer[2048];
};

class CMyClassProhibitHeap
{
#ifdef new
#undef new
	
	static void* operator new(size_t size);
	static void operator delete(void* ptr);

#endif

};

class CMyClassTestDefineScope
{
//#ifdef new
//#undef new

	static void* operator new(size_t size);
	static void operator delete(void* ptr);

//#endif

};

class CChildProhibitHeap : public CMyClassProhibitHeap
{

};

static CMyClassProhibitHeap testObj;

class CBase
{
public:
	virtual ~CBase() 
	{ 
		Release();
		AfxOutputDebugString(TEXT("Base's destruction!\n"));
	}

	virtual void Release()
	{
		AfxOutputDebugString(TEXT("Base's release!\n"));
	}
};

class CChild: public CBase
{
public:

	~CChild()
	{
		Release();
		AfxOutputDebugString(TEXT("Child's destruction!\n"));
	}

	virtual void Release()
	{
		AfxOutputDebugString(TEXT("Child's release!\n"));
	}
};

static void TestHookNxClothMeshRef(NxClothMesh* pNxClothMesh)
{
	static unsigned int s_iChangedRefCount = 10;

	class RefCountable
	{
	public:
		NxU32   numRefs;
	};

	class NpConvexMesh : public NxConvexMesh, public RefCountable
	{      
		void *  data;
	};

	int iOldRefCount = pNxClothMesh->getReferenceCount();

	((NpConvexMesh *)pNxClothMesh)->numRefs = s_iChangedRefCount;
	assert(pNxClothMesh->getReferenceCount() == s_iChangedRefCount);


	((NpConvexMesh *)pNxClothMesh)->numRefs = iOldRefCount;
	assert(pNxClothMesh->getReferenceCount() == s_iChangedRefCount);

}

// for testing multithreading case...
static DWORD WINAPI ThreadCreationFunc(void * arg)
{
	CAPhysXDebugRenderTestView* pView = (CAPhysXDebugRenderTestView*)arg;
	
	static int s_iSpawnType = 0;

	s_iSpawnType++;
	// if(s_iSpawnType>2) s_iSpawnType = 0;
	s_iSpawnType = 2;

	float fSpawnedHeight = (s_iSpawnType + 1 )* 10.0f;

	int iCurSpawnType = s_iSpawnType;
	int iCounter = 0;
	float fX = 0.0f;
	while(iCounter < 10)
	{
		switch(iCurSpawnType)
		{
		case 0:
			
			pView->CreateCube(NxVec3(0, fSpawnedHeight, 0));

			break;
		
		case 1:

			pView->LoadSkeletonObject(s_szSkeletonFileName, NxVec3(fX, fSpawnedHeight, 0));

		    break;

		case 2:

			pView->CreateClothObject(NxVec3(fX, fSpawnedHeight, 0));

			break;

		default:
		    break;
		}

		iCounter++;
		fX += 5.0f;

		::Sleep(1000);
	}

	

	return 0;
}

// static CMyClass myClassObj;


static void TestVariableInForLoop()
{
	int ii;

	for(ii =0; ii<100; ii++)
	{
		if(ii == 8)
			break;
	}

	// int iii = ii;

	for(ii = 0; ii<100; ii++)
	{
		if(ii == 10)
			break;
	}

	// int iii = ii;
}

static void TestTryCatch()
{
	try
	{
		int i=10;
		i = strcmp("haha", NULL);
		i++;
	}
	catch (...)
	{
		::AfxMessageBox(TEXT("some exception occurs!"));
	}

}


//-------------------------------------------------------------------
// test static variable in class member function: is the static variable only
// one copy for all of class's instances or each copy for one instance of the class...
//
// obviously, all instances of the class share the same static variable defined
// in class member function...

class CWFTestStaticVarInClassFunc
{
public:

	int GetValue()
	{
		int static s_i = 0;
		return ++s_i;
	}
};

class A
{
public:
	void* userData;

// private:
	A(){}
};

class B
{
public:

	void* userData;
};

class C:public A, B
{

public:
	C(){}
};


class CWFTestStaticNewDelete
{
public:
	CWFTestStaticNewDelete()
	{
		pI = new int [100];
	}

	~CWFTestStaticNewDelete()
	{
		delete [] pI;
	}

private:

	int* pI;
};


template <class T> 
class CSingleton 
{ 
public: 
	static T *GetPtrInstance() 
	{ 
		static T _instance; 
		return &_instance; 
	}; 

protected: 

// private:

//	CSingleton() {}; 
//	~CSingleton() {}; 
};

class MySingleton: public CSingleton<MySingleton>
{
	//friend class CSingleton<MySingleton>;
public:

// private:
	MySingleton(){}
	~MySingleton() {}

public:
	int GetData() {return 10;}

protected:


};


class MyChildSingleton: public CSingleton<MyChildSingleton>
{
	friend class CSingleton<MyChildSingleton>;
public:
	int GetData() {return 11;}
};


//---------------------------------------------------------------------------------------------------------
// CAPhysXDebugRenderTestView

IMPLEMENT_DYNCREATE(CAPhysXDebugRenderTestView, CView)

BEGIN_MESSAGE_MAP(CAPhysXDebugRenderTestView, CView)
	// Standard printing commands

#if MSVC_VER>6
	
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CAPhysXDebugRenderTestView::OnFileOpen)

#else
	
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)

#endif

	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()


#if defined( _DEBUG )
bool CustomAssertFunction( bool bExp, char* szMsg, int iLine, char* szFile)
{
	if(!bExp)
	{
		char szAssertMessage[1024];
		sprintf(szAssertMessage, "Assert: %s: at the File: %s, Line: %d", szMsg, szFile, iLine);
		// ::AfxMessageBox(szAssertMessage);
	}

	return bExp;
}

#define MyAssert( exp, description ) \
	if(!CustomAssertFunction( (int)(exp), description, __LINE__, __FILE__ ) ) \
{ _asm { int 3 } } //this will cause the debugger to break here on PC's

#else
#define MyAssert( exp, description )
#endif

static bool AddStringToClipboard(char* szInfo)
{

	if( OpenClipboard( NULL ) )
	{
		HGLOBAL hMem;
		char szAssert[256];
		char *pMem;

		sprintf( szAssert, "%s", szInfo );
		hMem = GlobalAlloc( GHND|GMEM_DDESHARE, strlen( szAssert )+1 );

		if( hMem ) {
			pMem = (char*)GlobalLock( hMem );
			strcpy( pMem, szAssert );
			GlobalUnlock( hMem );
			EmptyClipboard();
			SetClipboardData( CF_TEXT, hMem );
		}

		CloseClipboard();

		return true;
	}
	else
		return false;

}
// CAPhysXDebugRenderTestView construction/destruction

CAPhysXDebugRenderTestView::CAPhysXDebugRenderTestView(){
	// TODO: add construction code here

static AString astrTest = "fadsfads";



{
	int i;
	i = MySingleton::GetPtrInstance()->GetData();
	i = MyChildSingleton::GetPtrInstance()->GetData();

	// MySingleton myS;

}
	//m_pA3DIBLScene	= NULL;
	//m_pA3DSky		= NULL;
	//m_pSkinModel	= NULL;
	//m_pModel		= NULL;
	//m_pA3DKDTree	= NULL;
	//m_pBackMusic	= NULL;

	//m_pECModel = NULL;

//#define DEF_ARRAY(num) int iArr[num];

	//DEF_ARRAY(__LINE__);

	long long lltmp;

	m_pPhysXScene = NULL;
	
	m_bPauseSimulation = false;

	MyBase* myBase = new MyChild;
	MyChild* myChild = new MyChild;


// test RTTI...
	//AfxOutputDebugString(typeid(myBase).name());
	//AfxOutputDebugString(typeid(myChild).name());


	int i,ii;
	myBase->Set(i);
	myChild->Set(ii);

	MyChild::si = 10;
	int j, jj;
	myBase->NVSet(j);
	myChild->NVSet(jj);

	int k, kk;
	myBase->StaticSet(k);
	myChild->StaticSet(kk);

	delete myBase;
	delete myChild;

	m_pFluidObject = NULL;
	m_pFFObject = NULL;

	NCChild ncChild1, ncChild2;
	// ncChild1 = ncChild2;
	TestFunc(ncChild1);

	CTestRefReturn testRefReturn;
	NxVec3 v= testRefReturn.GetPosRef();
	v= testRefReturn.GetPos();

	CMyClass myClassObj;

	// _getch();

	// CChildProhibitHeap* pChildPH = new CChildProhibitHeap;
/*
#ifdef new
#undef new

	void* pAddress = ::malloc(sizeof(CChildProhibitHeap));
	CChildProhibitHeap* pChildPH = new (pAddress) CChildProhibitHeap;

#endif
*/

	CBase* pChild = new CChild;
	delete pChild;

	CBase* pBase = new CBase;
	delete pBase;


	// int* pInt = new int[1000];

	i = 0;

	// I can be locked more than once if I'm in the same thread...
	s_MutexTest.Lock();
	s_MutexTest.Lock();
	s_MutexTest.Lock();
	
	i = 10;

	s_MutexTest.Unlock();
	s_MutexTest.Unlock();
	s_MutexTest.Unlock();


	TestVariableInForLoop();



	std::vector<int> wfTestStdVector;
	wfTestStdVector.push_back(1);
	wfTestStdVector.push_back(2);
	wfTestStdVector.push_back(3);

	// int* pMyBase = (int* )(UINT &)(wfTestStdVector.begin());
	// int* pMyBase = (int* )(UINT &)(wfTestStdVector.begin());
	std::vector<int>::iterator it = wfTestStdVector.begin();
	
	int* pMyBase1 = &wfTestStdVector[0];
	int* pMyBase2 = & *wfTestStdVector.begin();



	float fTest = 1.5f;
	
	UINT &uTest0 = (UINT &)fTest;


	UINT uTest = (UINT &)fTest;
	UINT uTest1 = *(UINT *)&fTest;
	
	fTest = 1.6f;

	int iii =0;
	// TestTryCatch();


	//-------------------------------------------------------------------
	// test static variable in class's function: is the static variable only
	// one copy for all of class's instances or each copy for one instance of the class...
	
	CWFTestStaticVarInClassFunc wfTestStatic1, wfTestStatic2;
	iii = wfTestStatic1.GetValue();
	iii = wfTestStatic2.GetValue();

	C cc;

	cc.A::userData = NULL;

	
	APhysXRigidBodyObjectDesc* arrRBDesc = new APhysXRigidBodyObjectDesc[1];
	delete [] arrRBDesc;


	// test assert...

	// MyAssert(0, "assert here!");
	// assert(0);



	AddStringToClipboard("just a test!");
}

CAPhysXDebugRenderTestView::~CAPhysXDebugRenderTestView()
{
	ReleaseGame();
}

BOOL CAPhysXDebugRenderTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;

}


void CAPhysXDebugRenderTestView::OnDraw(CDC* /*pDC*/)
{
	CAPhysXDebugRenderTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CAPhysXDebugRenderTestView printing

BOOL CAPhysXDebugRenderTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAPhysXDebugRenderTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAPhysXDebugRenderTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CAPhysXDebugRenderTestView diagnostics

#ifdef _DEBUG
void CAPhysXDebugRenderTestView::AssertValid() const
{
	CView::AssertValid();
}

void CAPhysXDebugRenderTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAPhysXDebugRenderTestDoc* CAPhysXDebugRenderTestView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAPhysXDebugRenderTestDoc)));
	return (CAPhysXDebugRenderTestDoc*)m_pDocument;
}
#endif //_DEBUG

// CAPhysXDebugRenderTestView message handlers

void CAPhysXDebugRenderTestView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar)
	{
	case VK_F1:

		////	Play skin model action
		//if (m_pSkinModel)
		//{
		//	m_pSkinModel->PlayActionByName("向前跑", 1.0f);
		//}

		break;

	case VK_F2:

		{
#if 0
			if(m_pECModel)
			{
				A3DModelPhysSync* pPhysXSync = m_pECModel->GetPhysSync();
				if(pPhysXSync)
				{
					pPhysXSync->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);

					APhysXSkeletonRBObject* pSkltRBObject = pPhysXSync->GetSkeletonRBObject();

					if(pSkltRBObject)
					{
						// pSkltRBObject->SetLinearVelocity(NxVec3(0, 0, 30.0f));
						// pSkltRBObject->AddForce(NxVec3(0, 0, 30.0f));
					}
				}

			}
#endif

		}
		break;


	case VK_F3:

		//	Play skin model action
		if(gPhysXEngine->GetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES) == 1.0f)
		{
			gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 0);
			gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 0);
		}
		else
		{
			gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);
			gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
		}

		break;

	case VK_F5:
		{
			DWORD threadID;
			::CreateThread(0, 0, ThreadCreationFunc, (void *)this, 0, &threadID);	

			break;

		}

	case VK_F4:
		{
			TestPhysXStateSync();
			// TestRotation();
			break;
		}


	case VK_F8:

		{
			if(m_pPhysXScene)
			{
				m_pPhysXScene->EnableCollisionChannel(!m_pPhysXScene->IsCollisionChannelEnabled());
			}

			break;
		}

	case VK_F9:

		{
			if(m_pPhysXScene)
			{
				m_pPhysXScene->Reset(false);
				m_pPhysXScene->StartSimulate();
			}

			break;
		}

	case VK_F11:
		{
			if(m_pPhysXScene)
			{
				APhysXUtilLib::SnapshotScene(*m_pPhysXScene->GetNxScene());
			}

			break;
		}

	case '1':

		CreateStack(10);
		break;

	case '2':

		CreateTower(20);
		break;

	case VK_SPACE:

		CreateCubeFromEye(.5f);
		break;

	case 'G':
		// GenerateCovexRBObjectDesc();

		s_bStepForward = true;

		break;

	case '0':
		
		SavePhysXObjectDesc(s_szFileName);

		break;

	case '9':
		LoadPhysXObjectDesc(s_szFileName);

		break;

	case '8':
		{
#if 0
			if(m_pECModel && m_pECModel->GetModelPhysics())
				m_pECModel->GetModelPhysics()->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);
				
#endif

			break;
		}

	case 'I':

		TestAIniFile();
		break;
	
	case 'H':

		CreateJointedObjectFromEye();
		break;

	case 'N':

		SaveJointedObjectDescs();
		break;

	case 'M':

		LoadJointedObjectFromEye();

		break;

	case 'B':

		LoadSkeletonObjectFromEye(s_szSkeletonFileName);

		break;

	case 'V':

		LoadSkeletonObjectFromEye(s_szBreakableSkeletonFileName);

		break;

	case 'C':
		
		CreateClothObjectFromEye();

		break;

	case 'F':

		CreateFluidEmitter();

		break;

	case 'J':

		CreateForceField();

		break;
	
	case 'K':

		{
			if(m_pFFObject)
			{
				static int s_ZeroOne = 0;
				if(++s_ZeroOne == 2)
					s_ZeroOne = 0;

				if(s_ZeroOne)
					m_pFFObject->SetForceScale(100.0f);
				else
					m_pFFObject->SetForceScale(1.0f);
			}


			break;
		}

	case 'U':

		{
			gPhysXEngine->CoreDumpPhysX("testPhysXCoreDump.xml", APhysXEngine::APX_COREDUMP_FT_XML);

			break;
		}

	case 'P':

		{
			// PauseSimulation();

			s_bRunStepByStep = ! s_bRunStepByStep;

			break;
		}

	case 'O':
		
		{
			FetchPhysXProfileData();

			break;
		}

	case 'R':

		{

			RemoveObjListBack();
			
			// ResetScene();

			break;
		}

	case 'Y':

		{
			// switch the HW/SW PhysX simulation...
			gPhysXEngine->EnableHardwarePhysX(!gPhysXEngine->IsHardwarePhysXEnabled());

			break;
		}

	case 'X':
		{

			// PushToObjList(OpenPXD("wftest_rope.pxd"));
			OpenPXD("testStatic.pxd");

			//if(s_pRBAttacher)
			//	s_pRBAttacher->GetRBObject()->EnableKinematic(false);

			break;
		}

	case 'L':

		{

			//if(m_pECModel && m_pECModel->GetModelPhysics())
			//{
			//	m_pECModel->GetModelPhysics()->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);
			//}

			break;

		}


	default:
		
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAPhysXDebugRenderTestView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//	Generate a ray from viewpoint to the mouse clicked place
	//if (m_pA3DKDTree)
	//{
	//	//	Transfrom 2D screen position to 3D world position
	//	//A3DVECTOR3 vPos((float)(point.x & 0x0000ffff), (float)((point.y & 0xffff0000) >> 16), 1.0f);
	//	A3DVECTOR3 vPos((float)point.x , (float)point.y, 1.0f);
	//	g_Render.GetA3DViewport()->InvTransform(vPos, vPos);

	//	//	Do ray trace from viewpoint
	//	A3DVECTOR3 vStart = g_Render.GetA3DCamera()->GetPos();

	//	RAYTRACERT TraceRt;	//	Used to store trace result
	//	if (m_pA3DKDTree->RayTrace(&TraceRt, vStart, vPos - vStart, 1.0f))
	//	{
	//		//	Yes, the ray hit some face in the scene, play a GFX at hit point :)
	//		PlayGFX("火箭弹爆炸.gfx", TraceRt.vPoint, 1.0f);

	//		//	Why not play some sound ? :)
	//		AMSoundBufferMan* pSoundBufMan = g_Render.GetA3DEngine()->GetAMEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	//		pSoundBufMan->Play3DAutoSound(0, "sfx\\Tank_Fire02.wav", TraceRt.vPoint, 10.0f, 100.0f);
	//	}
	//}

	CWnd::OnLButtonDown(nFlags, point);
}

//	Initlaize game
bool CAPhysXDebugRenderTestView::InitGame()
{
//#error haha

#if 0

	g_LuaStateMan.Init();


	//	Load vertex shaders which are needed by skin models
	g_Render.GetA3DEngine()->GetA3DSkinMan()->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");
	//	Set directional light which are needed when render skin models
	g_Render.GetA3DEngine()->GetA3DSkinMan()->SetDirLight(g_Render.GetDirLight());



	//	Load IBL scene
	if (!LoadIBLScene("Models\\zhanshanghai-1\\zhanshanghai1.ils", "Models\\zhanshanghai-1\\zhanshanghai1.kdt"))
		return false;

	//	Load sky
	if (!LoadSky(1))
		return false;

	//	Load a A3DModel object
	if (!LoadModel("运兵车\\军用运兵车.mod"))
		return false;

	//	Load a skin model object
	if (!LoadSkinModel("Models\\Woman\\skinwoman.smd"))
		return false;

	//	Load background music
	if (!LoadMusic("Music\\mainmenu.mp3"))
		return false;
#endif

/*
	char szPath[1000];
	::GetCurrentDirectory(1000, szPath);

	//a_LogOutput(1, "%s", szPath);
	g_Log.Log("%s", szPath);
*/

	InitPhysXEngine();

	//	Set camera's position
	g_Render.GetA3DCamera()->SetPos(A3DVECTOR3(0.0f, 10.0f, -20.0f));

	// AfxGetGFXExMan()->Init(g_Render.GetA3DDevice());

	return true;
}

void CAPhysXDebugRenderTestView::ReleasePhysXEngine()
{

#if 1

	// we should release picker tools before 
	// m_apxPicker.Release();

	gPhysXEngine->Release();

	for(unsigned int i=0; i<m_arrConvexShapeDescs.size(); i++)
	{
		delete m_arrConvexShapeDescs[i];
	}

	m_arrConvexShapeDescs.clear();

	m_apxObjectDescManager.Release();

	m_pPhysXScene = NULL;

#endif

}

//	Release game
void CAPhysXDebugRenderTestView::ReleaseGame()
{
	//	Release model
	//if (m_pModel)
	//	g_Render.GetA3DEngine()->GetA3DModelMan()->ReleaseModel(m_pModel);

	//A3DRELEASE(m_pA3DIBLScene);
	//A3DRELEASE(m_pA3DKDTree);
	//A3DRELEASE(m_pA3DSky);
	//A3DRELEASE(m_pSkinModel);
	//A3DRELEASE(m_pECModel);


	//if (m_pBackMusic)
	//{
	//	m_pBackMusic->Stop();
	//	m_pBackMusic->Release();
	//	delete m_pBackMusic;
	//	m_pBackMusic = NULL;
	//}

	ReleasePhysXEngine();

	// g_LuaStateMan.Release();
}


#if 0

//	Load IBL scene
bool CAPhysXDebugRenderTestView::LoadIBLScene(const char* szSceneFile, const char* szKDTFile)
{
	//	Create scene object
	if (!(m_pA3DIBLScene = new A3DIBLScene))
		return false;

	//	Load scene file
	if (!(m_pA3DIBLScene->Load(g_Render.GetA3DEngine(), szSceneFile)))
	{
		g_Log.Log("Failed to load IBL scene file %s", szSceneFile);
		return false;
	}

	//	Create KDT object
	if (szKDTFile)
	{
		if (!(m_pA3DKDTree = new A3DKDTree))
			return false;

		if (!(m_pA3DKDTree->Load(szKDTFile)))
		{
			g_Log.Log("Failed to load KDT file %s", szKDTFile);
			return false;
		}

		m_pA3DKDTree->SetA3DEngine(g_Render.GetA3DEngine());
	}

	return true;
}

//	Load sky
bool CAPhysXDebugRenderTestView::LoadSky(int iSkyType)
{
	if (iSkyType == 0)
	{
		//	Sphere type sky
		A3DSkySphere* pSky = new A3DSkySphere;
		if (!pSky)
			return false;

		if (!pSky->Init(g_Render.GetA3DDevice(), NULL, "Rain\\Roof.bmp", 
			"Rain\\01.bmp", "Rain\\02.bmp"))
		{
			g_Log.Log("Failed to create sphere sky !");
			return false;
		}

		m_pA3DSky = pSky;
	}
	else if (iSkyType == 1)
	{
		//	Sky box
		A3DSkyBox* pSky = new A3DSkyBox;
		if (!pSky)
			return false;

		if (!pSky->Init(g_Render.GetA3DDevice(), NULL, "sky_space00.bmp", 
			"sky_space01.bmp", "sky_space02.bmp", "sky_space03.bmp", 
			"sky_space04.bmp", "sky_space05.bmp"))
		{
			g_Log.Log("Failed to create box sky !");
			return false;
		}

		m_pA3DSky = pSky;
	}

	//	This enable sky can do animation when A3DEngine::TickAnimation is called
	if (m_pA3DSky)
		g_Render.GetA3DEngine()->SetSky(m_pA3DSky);

	return true;
}

//	Load model
bool CAPhysXDebugRenderTestView::LoadModel(const char* szFile)
{
	A3DModel* pModel = NULL;
	if (!g_Render.GetA3DEngine()->GetA3DModelMan()->LoadModelFile(szFile, &pModel))
	{
		g_Log.Log("Failed to load model %s !", szFile);
		return false;
	}

	m_pModel = pModel;

	//	Set model's position
	m_pModel->SetPos(A3DVECTOR3(5.0f, 0.2f, 20.0f));
	//	Set model's direction
	m_pModel->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Update model
	m_pModel->TickAnimation();

	return true;
}

//	Load skin model
bool CAPhysXDebugRenderTestView::LoadSkinModel(const char* szFile)
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (!pModel)
		return false;

	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}

	if (!pModel->Load(szFile))
	{
		g_Log.Log("Failed to load skin model %s !", szFile);
		delete pModel;
		return false;
	}

	A3DRELEASE(m_pSkinModel);

	m_pSkinModel = pModel;

	//	In a real game, skin model should get environment's light infromation every
	//	frame from IBL scene according to it's position in the scene. But in a
	//	tutorial, we simply set the light information here and never change it.
	const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
	A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

	A3DSkinModel::LIGHTINFO LightInfo;

	LightInfo.colAmbient	= colAmbient;	//	Ambient color
	LightInfo.vLightDir		= LightParams.Direction;	//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
	LightInfo.bPtLight		= false;		//	false, disable dynamic point light,

	m_pSkinModel->SetLightInfo(LightInfo);

	//	Erase the force generated by position changing.
	//	m_pSkinModel->ErasePosEffectOnSuppleMeshes();

	//	Set model's position
	// m_pSkinModel->SetPos(A3DVECTOR3(-3.0f, 0.2f, 10.0f));
	m_pSkinModel->SetPos(A3DVECTOR3(0.0f));

	//	Set model's direction
	m_pSkinModel->SetDirAndUp(g_vAxisZ, g_vAxisY);
	//	Update model
	m_pSkinModel->Update(0);

	return true;
}

bool CAPhysXDebugRenderTestView::LoadECModel(const char* szFile, const NxVec3& vPos)
{
/*	
	CECModel* pECModel = new CECModel;
	if(!pECModel->Load(szFile))
	{
		delete pECModel;
		return false;
	}

	A3DRELEASE(m_pECModel);
	m_pECModel = pECModel;

	if (m_pECModel->GetA3DSkinModel() == NULL)
	{
		m_pECModel->Release();
		return false;
	}
	
	const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
	A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();
	A3DSkinModel::LIGHTINFO LightInfo;

	LightInfo.colAmbient	= colAmbient;				//	Ambient color
	LightInfo.vLightDir		= LightParams.Direction;	//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
	LightInfo.bPtLight		= false;					//	false, disable dynamic point light,

	m_pECModel->GetA3DSkinModel()->SetLightInfo(LightInfo);

	//	Set model's position
	m_pECModel->SetPos(APhysXConverter::N2A_Vector3(vPos));
	//	Set model's direction
	m_pECModel->SetDirAndUp(A3DVECTOR3(0, 0, 1.f), A3DVECTOR3(0, 1.0f, 0));

	m_pECModel->SetCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON);

	m_pECModel->ScaleBoneEx("000", A3DVECTOR3(1.0f, 1.0f, 10.0f));
	m_pECModel->EnablePhysSystem(false);
*/

	return true;
}


//	Play a GFX
bool CAPhysXDebugRenderTestView::PlayGFX(const char* szFile, const A3DVECTOR3& vPos, float fScale)
{
	A3DGraphicsFX* pGFX = NULL;
	if (!g_Render.GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile(szFile, NULL, NULL, false, &pGFX, vPos))
	{
		g_Log.Log("Failed to load GFX %s.", szFile);
		return false;
	}

	pGFX->SetScale(fScale);
	pGFX->Start(true);

	return true;
}

//	Load music
bool CAPhysXDebugRenderTestView::LoadMusic(const char* szFile)
{
	if (!(m_pBackMusic = new AMSoundStream))
		return false;

	if (!m_pBackMusic->Init(g_Render.GetA3DEngine()->GetAMEngine()->GetAMSoundEngine(), szFile))
	{
		g_Log.Log("Failed to load background music %s.", szFile);
		return false;
	}

	m_pBackMusic->Play();

	return true;
}

#endif


//	Move camera
void CAPhysXDebugRenderTestView::MoveCamera(DWORD dwTime)
{
	A3DCamera* pA3DCamera = g_Render.GetA3DCamera();

	A3DVECTOR3 vDirH = pA3DCamera->GetDirH();
	A3DVECTOR3 vRightH = pA3DCamera->GetRightH();

	A3DVECTOR3 vDir = pA3DCamera->GetDir();
	A3DVECTOR3 vRight = pA3DCamera->GetRight();

	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDir;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDir;

	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRight;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRight;

	if (GetAsyncKeyState(VK_HOME) & 0x8000)
		vDelta = vDelta + g_vAxisY;
	else if (GetAsyncKeyState(VK_END) & 0x8000)
		vDelta = vDelta - g_vAxisY;

	float fSpeed = 16.0f;
	vDelta = Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);

	pA3DCamera->Move(vDelta);
}

//	Rotate camera
void CAPhysXDebugRenderTestView::RotateCamera(DWORD dwTime)
{
	A3DCamera* pA3DCamera = g_Render.GetA3DCamera();

	float fSpeed = 80.0f;
	fSpeed *= (float)dwTime * 0.001f;

	float fPitch = 0.0f;
	float fYaw = 0.0f;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		fPitch = -1.0f;
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		fPitch = 1.0f;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		fYaw = -1.0f;
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		fYaw = 1.0f;

	if (fYaw)
		pA3DCamera->DegDelta(fYaw * fSpeed);

	if (fPitch)
		pA3DCamera->PitchDelta(fPitch * fSpeed);
}

//	Frame move
static char szTickFPS[256];
static float s_fCurSimTime = 0.0f;

bool CAPhysXDebugRenderTestView::Tick(DWORD dwTickTime)
{

	//	Move camera
	MoveCamera(dwTickTime);
	//	Rotate camera
	RotateCamera(dwTickTime);


#if 0

	// non-physx stuff...
	//	Update scene's PVS
	if (m_pA3DIBLScene)
		m_pA3DIBLScene->UpdateVisibleSets(g_Render.GetA3DViewport());

	//	Update skin model
	if (m_pSkinModel)
		m_pSkinModel->Update(dwTickTime);

	// update the EC Model...
	if(m_pECModel)
		m_pECModel->Tick(dwTickTime);


	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	if ((dwAnimTime += dwTickTime) >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_Render.GetA3DEngine()->TickAnimation();

		//	Update model
		if (m_pModel)
			m_pModel->TickAnimation();

		//	Update ear position so that all 3D sounds' positions are correct
		g_Render.GetA3DCamera()->UpdateEar();
	}

#endif

//--------------------------------------------------
// try to lock the FPS...

// #define	LOCK_FPS
#ifdef	LOCK_FPS

	static double s_fCurTime = 0;
	static double s_fLastTime = 0;

	float dt = s_fCurTime - s_fLastTime;
	s_fLastTime = s_fCurTime;
	

	// 有点不可理喻， double的类型竟然也不够。。。
	// 还是老老实实用 “差”来计算吧。。。
	static DWORD s_dwCurTime;
	static DWORD s_dwLastTime;
	s_dwLastTime = s_dwCurTime;
	s_dwCurTime = a_GetTime();
	
	s_fCurTime = (double)(s_dwCurTime / 1000.0);
	
	int i =dt;



#if 0

	// sleep method, seems not to be good..
	if(dwTickTime < APHYSX_STD_TICKTIME * 1000)
	{
		int iSleepTime = (int)(1000 * APHYSX_STD_TICKTIME - dwTickTime);
		Sleep( iSleepTime );
	}

#else
	
	// while() method...

	float fElapsedTime = s_fCurTime - s_fLastTime;

#if 0

	// using double data type, while, it seems error is still there...

	while (s_fCurTime - s_fLastTime < 0.01)
	{
		s_fCurTime = (double)(a_GetTime() / 1000.0);
	}

#else

	// just lock the FPS not to beyond the APHYSX_STD_FPS...
	while (s_dwCurTime - s_dwLastTime < 1000 * APHYSX_STD_TICKTIME )
	{
		s_dwCurTime = a_GetTime();
	}

#endif

	float fRealElapsedTime = s_fCurTime - s_fLastTime;



	static float s_fSumElapsedTime = 0.0f;
	static int s_iFrameCounter = 0;
	static float s_fAvgFPS = 0.0f;

	s_fSumElapsedTime += fRealElapsedTime;
	s_iFrameCounter++;

	if(s_fSumElapsedTime > 1.0f)
	{
		s_fAvgFPS = s_iFrameCounter / s_fSumElapsedTime;

		s_fSumElapsedTime = 0.0f;
		s_iFrameCounter = 0;
	}

#endif

#endif // ifdef LOCK_FPS


	// now we can support run the simulation step by step...
	if(s_bRunStepByStep)
	{
		if(s_bStepForward)
			s_bStepForward = false;
		else
			return false;
	}
		


	// start physics simulation
#if 0	

	// use the elapsed time directly...
	float dSimTime = dwTickTime * 0.001f;

#else	

	// use the multiple of sub-timestep as the elapsed time...

	float dSimTime = m_pPhysXScene->GetExactSimulateTime(dwTickTime * 0.001f);
	// float dSimTime = 1.0f/60;



	if(dSimTime < 0.0001f)
		return true;

#endif


	// update terrain...
	A3DCamera* pA3DCamera = g_Render.GetA3DCamera();
	A3DVECTOR3 vCamPos = pA3DCamera->GetPos();
	m_pPhysXScene->GetPhysXTerrain()->UpdateSimCenter(APhysXConverter::A2N_Vector3(vCamPos));



#if 0

	// to test some random elapsed time...
	static float s_TickTimeMin = 0.025f;
	static float s_TickTimeMax = 0.03f;
		
	dSimTime = (s_TickTimeMax - s_TickTimeMin) * (float)rand() /((float) RAND_MAX);
	dSimTime += s_TickTimeMin;

#endif

	s_apxNetworkActor.Tick(dSimTime);
	s_apxNetworkActorCorrected.Tick(dSimTime);

	// there maybe some bad behavior if we pass 0 as dSimTim to Simulate()...
	// pPerfItem->Resume();
	gPhysXEngine->Simulate(dSimTime);
	// pPerfItem->EndFrame();

	// m_pPhysXScene->GetNxScene()->setTiming(dSimTime/4.0f);

	
	// gPhysXEngine->Simulate(1.0f/60);

	s_fCurSimTime = dSimTime;

	// OutputDebugStr("\ntest");

	// ------------------------------------------------------
	// it seems to be a bug of PhysX SDK, found by wenfeng, Sept. 17, 2008
	// to make sure the behavior of ForceField works well, we must call simulate()
	// with a constant dt...
	// gPhysXEngine->Simulate(APHYSX_STD_TICKTIME);

	//--------------------------------------------------
	// some post simulate code...
#if 0
	if(m_pECModel)
		m_pECModel->SyncModelPhys();
#endif



	//----------------------------------------------
	// test code...
	// try to get cloths's shape

#if 0

	if(m_pPhysXScene)
	{
		for(int i=0; i<m_pPhysXScene->GetNxScene()->getNbCloths(); i++)
		{
			NxCloth* pCloth = m_pPhysXScene->GetNxScene()->getCloths()[i];

			int iShapeNum = pCloth->queryShapePointers();

			if(iShapeNum > 0)
			{
				NxShape** Shapes = new NxShape*[iShapeNum];
				NxU32*flags  = new NxU32[iShapeNum];

				pCloth->getShapePointers(Shapes, flags);

				delete [] Shapes;
				delete [] flags;

			}
			
		}
	}

#endif

	//----------------------------------------------

	// test fluid object's write data...

#if 0

	if(m_pFluidObject)
	{

		AfxTrace("\n============================================");
		AfxTrace("\n New created particles IDs:\n");
		for(int i=0; i<m_pFluidObject->GetCreatedIdNum(); i++)
		{
			AfxTrace("%d,", m_pFluidObject->GetCreatedParticleIds()[i]);
		}

		AfxTrace("\n New deleted particles IDs:\n");
		for(int i=0; i<m_pFluidObject->GetDeletedIdNum(); i++)
		{
			AfxTrace("%d,", m_pFluidObject->GetDeletedParticleIds()[i]);
		}

		const int iTrackIDNum = 20;
		AfxTrace("\n The first %d particles IDs:\n", iTrackIDNum);
		if(m_pFluidObject->GetParticleIDs() ) // && m_pFluidObject->GetParticleNum()>iTrackIDNum)
		{
			for(int i=0; i<m_pFluidObject->GetParticleNum(); i++)
			{
				AfxTrace("%d,", m_pFluidObject->GetParticleIDs()[i]);
			}
		}

		
	}

#endif


	if(s_pControledActor)	
	{
		m_pPhysXScene->LockWritingScene();
		s_pControledActor->moveGlobalPosition(s_pControledActor->getGlobalPosition () + NxVec3(dSimTime * 6.0f, 0, 0));

		m_pPhysXScene->UnlockWritingScene();
	}

	if(s_gRBAttacher)	
	{
		m_pPhysXScene->LockWritingScene();
		NxMat34 vAttacherPose = s_gRBAttacher->GetAttacherObject()->GetPose();
		vAttacherPose.t += NxVec3(dSimTime * 3.f, 0, 0);
		s_gRBAttacher->SetAttacherPose(vAttacherPose);

		m_pPhysXScene->UnlockWritingScene();
	}

	if(s_pAPhysXCC)
	{
		s_pAPhysXCC->MoveCC(dSimTime, NxVec3(1.0f, 0.0f, 0.0f));
	}
	

	return true;
}

//	Render 


bool CAPhysXDebugRenderTestView::Render(DWORD dwTickTime)
{
	abase::vector<int> myIntArray;

	//	Begin render
	if (!g_Render.BeginRender())
		return false;

	//-----------------------------------------------------------------
	// render FPS...
	
	//	g_Render.TextOut(10, 10, _AL("Angelica SDK 2.0 Tutorial"), dwTextCol);
	float fElapsedTime =  dwTickTime * 0.001f;

	static float s_fSumElapsedTime = 0.0f;
	static int s_iFrameCounter = 0;
	static float s_fAvgFPS = 0.0f;

	s_fSumElapsedTime += fElapsedTime;
	s_iFrameCounter++;

	if(s_fSumElapsedTime > 1.0f)
	{
		s_fAvgFPS = s_iFrameCounter / s_fSumElapsedTime;

		s_fSumElapsedTime = 0.0f;
		s_iFrameCounter = 0;
	}

	_stprintf(szFPSBuffer, TEXT("Current Frame Time: %f"), fElapsedTime);
	g_Render.TextOut(10, 550, szFPSBuffer, 0xffffff00);
	_stprintf(szFPSBuffer, TEXT("Average FPS: %f"), s_fAvgFPS);
	g_Render.TextOut(10, 568, szFPSBuffer, 0xffffff00);

	if(s_fCurSimTime > 0.0f)
	{
		_stprintf(szFPSBuffer, TEXT("Current Simulate Time: %f"), s_fCurSimTime);
		g_Render.TextOut(10, 588, szFPSBuffer, 0xffff0000);
	}

	if(m_pPhysXScene)
	{
		_stprintf(szFPSBuffer, TEXT("Compartment Number: %d"), m_pPhysXScene->GetNxScene()->getNbCompartments());
		g_Render.TextOut(10, 608, szFPSBuffer, 0xffffff00);
	}
	
/*
	if(s_pConvexMesh)
	{
		NxConvexMeshDesc cmDesc;
		s_pConvexMesh->saveToDesc(cmDesc);

		_stprintf(szFPSBuffer, TEXT("Convex Vertex Number: %d"), cmDesc.numVertices);
		g_Render.TextOut(10, 708, szFPSBuffer, 0xffffff00);

	}
*/

	//-----------------------------------------------------------------
	// some test snippet 

/*

#define IR(x)	((APhysXU32&)x)

	int iLoopTimes = 100000000;
	bool bPositiveFloat;
	DWORD dwTimeCounter;

	// general way...
	dwTimeCounter = APhysX_GetMilliseconds();
	for(int i = 0; i < iLoopTimes; i++)
	{
		bPositiveFloat = fElapsedTime > 0.0f;
	}
	dwTimeCounter = APhysX_GetMilliseconds() - dwTimeCounter;

	_stprintf(szFPSBuffer, TEXT("Float positive/negative test(ms): %d"), dwTimeCounter);
	g_Render.TextOut(10, 618, szFPSBuffer, 0xffffff00);


	// bit test way...
	dwTimeCounter = APhysX_GetMilliseconds();
	for(int i = 0; i < iLoopTimes; i++)
	{
		bPositiveFloat = IR(fElapsedTime) & 0x80000000 ;
	}
	dwTimeCounter = APhysX_GetMilliseconds() - dwTimeCounter;

	_stprintf(szFPSBuffer, TEXT("Float positive/negative test(ms): %d"), dwTimeCounter);
	g_Render.TextOut(10, 638, szFPSBuffer, 0xffffff00);

*/


	//-----------------------------------------------------------------


	A3DViewport* pA3DViewport = g_Render.GetA3DViewport();

	//	Activate viewport and clear it
	pA3DViewport->Active();
	pA3DViewport->ClearDevice();

#if 0

	//	Render sky first
	if (m_pA3DSky)
	{
		m_pA3DSky->SetCamera(pA3DViewport->GetCamera());
		m_pA3DSky->Render();
	}

	if (m_pA3DIBLScene)
	{
		//	Render solid faces in scene
		m_pA3DIBLScene->Render(pA3DViewport, A3DSCENE_RENDER_SOLID);

		//	Render alpha faces in scene
		m_pA3DIBLScene->Render(pA3DViewport, A3DSCENE_RENDER_ALPHA);
	}

	//	Render A3DModel objects
	if (m_pModel)
		m_pModel->Render(pA3DViewport);

	//	Register all skin models which need rendering
	if (m_pSkinModel)
		m_pSkinModel->Render(pA3DViewport);

	// if(m_pECModel)
	//	m_pECModel->Render(pA3DViewport);

	//	Really render all skin models.
	g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender()->Render(pA3DViewport);

	//	Render all GFX
	g_Render.GetA3DEngine()->RenderGFX(pA3DViewport, 0xffffffff);

#endif


	//	Draw prompt
	//DrawPrompt();


	// physics debug render...
	// gPhysXEngine->Simulate(APHYSX_STD_TICKTIME);

	 if(m_pPhysXScene)
	 {
		m_pPhysXScene->DebugRender();
	 }



#if 1

	// test render actor and APhysXObject...
	for(unsigned int i=0; i<m_arrPhysXObjList.size(); i++)
	{
		// render the APhysXObject/NxActor using APhysXUtilib functions...
		APhysXUtilLib::DebugDrawAPhysXObject(*g_Render.GetA3DEngine()->GetA3DWireCollector(), m_arrPhysXObjList[i], 0xf0ffff00);

		// render the APhysXObject's aabb...
		//NxBounds3 aabb;
		//m_arrPhysXObjList[i]->GetAABB(aabb);

		//A3DAABB a_aabb;
		//APhysXConverter::N2A_AABB(aabb, a_aabb);

		//A3DWireCollector* pWC = g_Render.GetA3DEngine()->GetA3DWireCollector();
		//pWC->AddAABB(a_aabb, 0xffff0000);
	}

#endif


	// try to render fluid particles...
	if(m_pFluidObject && m_pFluidObject->GetParticlePositions())
	{
		A3DWireCollector* pWC = g_Render.GetA3DEngine()->GetA3DWireCollector();
		NxU32 NbPoints = m_pFluidObject->GetParticleNum();
		const NxVec3* Points = m_pFluidObject->GetParticlePositions();
		const APhysXU32* IDs = m_pFluidObject->GetParticleIDs();
		A3DMATRIX4 mtRot;
		mtRot.Identity();
		const NxQuat* qRots = NULL;
		if(m_pFluidObject->IsObjectType(APX_OBJTYPE_FLUID_ORIENTED_PARTICLE))
		{
			qRots = ((APhysXOrientedParticleObject *)m_pFluidObject)->GetParticleOrientationsByID();
		}

		int iCur = 0;
		while(NbPoints--)
		{
			APhysXU32 uID = IDs[iCur];

			if(qRots)
			{
				A3DQUATERNION aQ;
				APhysXConverter::N2A_Quat(qRots[uID], aQ);
				aQ.ConvertToMatrix(mtRot);
			}

			mtRot.SetRow(3, APhysXConverter::N2A_Vector3(*Points));
			float fR = m_pFluidObject->GetCollisionRadius();
			pWC->AddSphere(A3DVECTOR3(0.0f), m_pFluidObject->GetCollisionRadius(), 0xffffff00, &mtRot);
			// pWC->Add3DBox(A3DVECTOR3(0.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f), A3DVECTOR3(1.0f, 0.0f, 0.0f), A3DVECTOR3(fR, 0.01f, fR), 0xffffff00, &mtRot);
			
			++Points;
			++iCur;
		}

		pWC->Flush();
	}


	// render the neighbor actors;
	for(unsigned int i=0; i<s_NeighborActors.size(); i++)
	{
		APhysXUtilLib::DebugDrawActor(*g_Render.GetA3DEngine()->GetA3DWireCollector(), *s_NeighborActors[i], 0x3000ff00);
	}

	for(unsigned int i=0; i<s_OverlappedActors.size(); i++)
	{

		// render the overlap normals...
		NxVec3 vStart = s_OverlappedActors[i]->getGlobalPosition();
		NxVec3 vEnd = vStart + s_OverlapNormals[i];
		APhysXUtilLib::DebugDrawArraw(*g_Render.GetA3DEngine()->GetA3DWireCollector(), vStart, vEnd);

	}
	
	// render the reference frame ...
	APhysXUtilLib::DebugDrawArraw(*g_Render.GetA3DEngine()->GetA3DWireCollector(), NxVec3(0.0f), NxVec3(3.f, 0, 0), 0xffff0000);
	APhysXUtilLib::DebugDrawArraw(*g_Render.GetA3DEngine()->GetA3DWireCollector(), NxVec3(0.0f), NxVec3(0, 3.f, 0), 0xff00ff00);
	APhysXUtilLib::DebugDrawArraw(*g_Render.GetA3DEngine()->GetA3DWireCollector(), NxVec3(0.0f), NxVec3(0, 0, 3.f), 0xff0000ff);

	// render the proxy RB object...
	if(s_pAuthorRB)
		APhysXUtilLib::DebugDrawAPhysXObject(*g_Render.GetA3DEngine()->GetA3DWireCollector(), s_pAuthorRB, 0xffff0000);

	if(s_pProxyRB)
		APhysXUtilLib::DebugDrawAPhysXObject(*g_Render.GetA3DEngine()->GetA3DWireCollector(), s_pProxyRB, 0xffffff00);

	// if(s_pProxyRBCorrected)
	//	APhysXUtilLib::DebugDrawAPhysXObject(*g_Render.GetA3DEngine()->GetA3DWireCollector(), s_pProxyRBCorrected, 0xff00ffff);

	//	End render
	g_Render.EndRender();

	return true;
}

//	Draw prompt
bool CAPhysXDebugRenderTestView::DrawPrompt()
{
	DWORD dwTextCol = A3DCOLORRGB(255, 255, 0);

	g_Render.TextOut(10, 10, _AL("Angelica SDK 2.0 Tutorial"), dwTextCol);
	g_Render.TextOut(10, 26, _AL("W / S - Move Forward / Backward"), dwTextCol);
	g_Render.TextOut(10, 42, _AL("A / D - Move Left / Right"), dwTextCol);
	g_Render.TextOut(10, 58, _AL("Home / End - Move Up / Down"), dwTextCol);
	g_Render.TextOut(10, 74, _AL("L / R arrow - Camera Rotate Left / Right"), dwTextCol);
	g_Render.TextOut(10, 90, _AL("U / D arrow - Camera Rotate Down / Up"), dwTextCol);
	g_Render.TextOut(10, 106, _AL("F1 - Play skin model action"), dwTextCol);
	g_Render.TextOut(10, 122, _AL("L button - Ray trace test"), dwTextCol);

	g_Render.Draw2DRect(ARectI(5, 5, 300, 150), 0xff00ff00, true);

	return true;
}



void CAPhysXDebugRenderTestView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	//	Resize device
	g_Render.ResizeDevice(cx, cy);
}

//-------------------------------------------------------------------------------
//*

void CAPhysXDebugRenderTestView::PhysXControl()
{
	if(KeyPressed(VK_SPACE))
	{
		CreateCubeFromEye();
	}
	else if(KeyPressed('1'))
	{
		CreateStack(10);
	}
	else if (KeyPressed('2'))
	{
		CreateTower(20);
	}

}

void CAPhysXDebugRenderTestView::LoadPhysXObjectDesc(const char* szFileName)
{
	//APhysXUserStream InFile(szFileName, true);
	m_apxObjectDescManager.Load(szFileName);

}

void CAPhysXDebugRenderTestView::SavePhysXObjectDesc(const char* szFileName)
{
	//APhysXUserStream OutputFile(szFileName, false);
	//m_apxObjectDescManager.Save(szFileName);
	gPhysXEngine->GetObjectDescManager()->Save(szFileName);
	
	// release manually
	gPhysXEngine->GetObjectDescManager()->Release();
}

void CAPhysXDebugRenderTestView::GenerateCovexRBObjectDesc()
{
	for(int i=0; i<CONVEX_OBJ_DESC_NUM; i++)
	{
		char szName[100];
		sprintf(szName, "%s%d", s_szConvexShapeDescPrefix, i);
		GenerateCovexMeshShapeDesc(szName);

		APhysXActorDesc apxActorDesc;
		apxActorDesc.mDensity = 1.0f;

		APhysXMaterialDesc apxMaterialDesc;
		apxMaterialDesc.mDynamicFriction = 0.9f;
		apxMaterialDesc.mRestitution = 0.3f;

		APhysXArray<APhysXShapeDesc* > Shapes;
		Shapes.push_back(m_arrConvexShapeDescs[i]);	

		APhysXDummyRBObjectDesc* pAPXDummyRBObjDesc = (APhysXDummyRBObjectDesc*)gPhysXEngine->GetObjectDescManager()->CreateAndPushObjectDesc(APX_OBJTYPE_DUMMY);
		sprintf(szName, "%s%d", s_szConvexObjDescPrefix, i);
		pAPXDummyRBObjDesc->SetName(szName);
		pAPXDummyRBObjDesc->AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);

	}

}

void CAPhysXDebugRenderTestView::GenerateCovexMeshShapeDesc(const char* szName)
{

	int nbInsideCirclePts = NxMath::rand(3, 8);
	int nbOutsideCirclePts = NxMath::rand(3, 8);
	int nbVerts = nbInsideCirclePts + nbOutsideCirclePts;
	const float scale = 3.0f;

	// Generate random vertices
	NxVec3* verts = new NxVec3[nbVerts];

	for(int i=0;i<nbVerts;i++)
	{
		verts[i].x = scale * NxMath::rand(-2.5f, 2.5f);
		verts[i].y = scale * NxMath::rand(-2.5f, 2.5f);
		verts[i].z = scale * NxMath::rand(-2.5f, 2.5f);
	}

	// Create descriptor for convex mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= nbVerts;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// Cooking from memory
	APhysXMemoryWriteBuffer buf;
	if(gPhysXEngine->GetMeshCooker()->CookConvexMesh(convexDesc, buf))
	{
		APhysXConvexShapeDesc* pConvexShapeDesc = new APhysXConvexShapeDesc;
		pConvexShapeDesc->SetName(szName);
		pConvexShapeDesc->SetConvexMeshBuf(buf);
		m_arrConvexShapeDescs.push_back(pConvexShapeDesc);
	}
}

void CAPhysXDebugRenderTestView::CreateConvex(int i, const NxVec3& pos, const NxVec3* initialVelocity, const NxVec3& vScale3D)
{
	if(gPhysXEngine->GetObjectDescManager()->GetNum())
	{
		NxMat34 mtPose;
		mtPose.t = pos;
		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(i);
		apxObjInstanceDesc.mGlobalPose = mtPose;
		apxObjInstanceDesc.mScale3D = vScale3D;

		m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

	}

}

class CMyCubeGraphicsSync: public APhysXUserGraphicsSync
{
public:

	CMyCubeGraphicsSync(// const NxVec3& vDimensions, 
		A3DFlatCollector* pFC = NULL, A3DWireCollector* pWC = NULL)
		: // m_vDimensions(vDimensions), 
		m_pFC(pFC), m_pWC(pWC)
	{

	}
	
	void SetFCAndWC(A3DFlatCollector* pFC = NULL, A3DWireCollector* pWC = NULL)
	{
		m_pFC = pFC;
		m_pWC = pWC;
	};

	inline virtual void SyncGraphicsData(const APhysXObject* pObject)
	{
		// just render the box
/*
		if(pObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*) pObject;
			NxActor* pActor = pRBObj->GetNxActor(0);
			NxShape* pShape = pActor->getShapes()[0];
			NxBoxShape* pBox = pShape->isBox();
			if (pBox)
			{
				NxMat34 mtPose = pObject->GetPose();

				A3DVECTOR3 vPos = APhysXConverter::N2A_Vector3(mtPose.t);
				A3DVECTOR3 vDir =  APhysXConverter::N2A_Vector3(mtPose.M.getColumn(0));
				A3DVECTOR3 vUp = APhysXConverter::N2A_Vector3(mtPose.M.getColumn(1));
				A3DVECTOR3 vRight = APhysXConverter::N2A_Vector3(mtPose.M.getColumn(2));

				m_pFC->AddBox_3D(vPos, vDir, vUp, vRight, APhysXConverter::N2A_Vector3(pBox->getDimensions()), 0x7000ff00);
				//m_pWC->Add3DBox(vPos, vDir, vUp, vRight, APhysXConverter::N2A_Vector3(pBox->getDimensions()), 0xffff0000);				
			}

		}
*/

	}

	virtual void SyncPhysicsData(APhysXObject* pPhysXObject)
	{
		// do nothing...
	}

	virtual void ReleaseGraphicsData()
	{
		// do nothing...
	}

protected:

	// NxVec3 m_vDimensions;
	A3DFlatCollector* m_pFC;
	A3DWireCollector* m_pWC;

};

static CMyCubeGraphicsSync cubeGSync;


void GetAABoxVertices(const NxVec3& vCenter, const NxVec3& vHalfSize,  NxVec3 Vertices[])
{
	Vertices[0].set(vCenter.x - vHalfSize.x, vCenter.y - vHalfSize.y, vCenter.z - vHalfSize.z);
	Vertices[1].set(vCenter.x - vHalfSize.x, vCenter.y - vHalfSize.y, vCenter.z + vHalfSize.z);
	Vertices[2].set(vCenter.x - vHalfSize.x, vCenter.y + vHalfSize.y, vCenter.z - vHalfSize.z);
	Vertices[3].set(vCenter.x - vHalfSize.x, vCenter.y + vHalfSize.y, vCenter.z + vHalfSize.z);

	Vertices[4].set(vCenter.x + vHalfSize.x, vCenter.y - vHalfSize.y, vCenter.z - vHalfSize.z);
	Vertices[5].set(vCenter.x + vHalfSize.x, vCenter.y - vHalfSize.y, vCenter.z + vHalfSize.z);
	Vertices[6].set(vCenter.x + vHalfSize.x, vCenter.y + vHalfSize.y, vCenter.z - vHalfSize.z);
	Vertices[7].set(vCenter.x + vHalfSize.x, vCenter.y + vHalfSize.y, vCenter.z + vHalfSize.z);

}


APhysXObject* CAPhysXDebugRenderTestView::CreateCube(const NxVec3& pos, float size, const NxVec3* initialVelocity, const char* szName, bool bPushToObjList)
{
	if(m_pPhysXScene == NULL) 
		return NULL;	

#if 0
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = *initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3((float)size, (float)size, (float)size);

	APhysXRigidBodyObjectDesc apxRBObjDesc;
	apxRBObjDesc.mNxActorDesc.shapes.pushBack(&boxDesc);
	apxRBObjDesc.mNxActorDesc.body			= &bodyDesc;
	apxRBObjDesc.mNxActorDesc.density		= 10.0f;
	apxRBObjDesc.mNxActorDesc.globalPose.t  = pos;

#else
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.f;
	apxActorDesc.mLinearDamping = 0.05f;
	apxActorDesc.mAngularDamping = 0.05f;
	apxActorDesc.SetName("CubeActor");

	if(initialVelocity) 
		apxActorDesc.mLinearVelocity = *initialVelocity;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

#if 1
	
	APhysXBoxShapeDesc boxDesc;
	// boxDesc.mDimensions = NxVec3((float)size, (float)size, (float)size);
	boxDesc.mDimensions = NxVec3(size, size, size);
	//boxDesc.mMaterialIndex = APhysXBoxShapeDesc::APX_SHAPEMATERIALINDEX_USEDEFAULT;
	boxDesc.UseDefaultMaterial();

	boxDesc.SetName("Cube");
	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

#else
	
	// size *= 2;

	NxConvexMeshDesc cmDesc;
	NxVec3 vPoints[8];
	GetAABoxVertices(NxVec3(0.0f), NxVec3(size), vPoints);

	cmDesc.numVertices = 8;
	cmDesc.pointStrideBytes = sizeof(NxVec3);
	cmDesc.points = vPoints;
	cmDesc.flags = NX_CF_COMPUTE_CONVEX;

	APhysXMemoryWriteBuffer mwBuffer;
	if(!gPhysXEngine->GetMeshCooker()->CookConvexMesh(cmDesc, mwBuffer))
		return NULL;

	APhysXConvexShapeDesc convexDesc;
	convexDesc.SetConvexMeshBuf(mwBuffer);
	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&convexDesc);

	// NxConvexShapeDesc* pCSDesc = (NxConvexShapeDesc*)convexDesc.CreateNxShapeDesc();
	// s_pConvexMesh = pCSDesc->meshData;
	// delete pCSDesc;

#endif


	// APhysXDummyRBObjectDesc apxDummyRBObjDesc;
	APhysXDynamicRBObjectDesc apxDummyRBObjDesc;
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	apxDummyRBObjDesc.SetName(szName);
	// apxDummyRBObjDesc.mSimInCompartment = true;

#endif

	NxMat34 mtPose;
	mtPose.t = pos;
	
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = &apxDummyRBObjDesc;
	apxObjInstanceDesc.mGlobalPose = mtPose;
	apxObjInstanceDesc.mPhysXUserGraphicsSync = &cubeGSync;
	// apxObjInstanceDesc.mScale3D.set(2.0f);

	// who will be in charge of deleting the mPhysXUserGraphicsSync?

	APhysXObject* pObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);
	
	float fUnitMass = 8 * size * size * size;
	float fCubeMass = ((APhysXRigidBodyObject *)pObj)->GetMass();
	float fScale = fCubeMass / fUnitMass;


	APhysXDynamicRBObjectDesc apxDummyRBObjDescCopy;
	apxDummyRBObjDescCopy = apxDummyRBObjDesc;
	

	// test code for collision channel...
#if 0
	APhysXCollisionChannel colChannel = m_pPhysXScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
	if(pObj && colChannel != APX_COLLISION_CHANNEL_INVALID)
	{
		pObj->SetCollisionChannel(colChannel);
	}
#endif

	// test code for dominant group
#if 0

	if(pObj)
	{
		NxActor* pActor = ((APhysXRigidBodyObject* )pObj)->GetNxActor(0);
		pActor->setDominanceGroup(10);

	}

#endif

	if(bPushToObjList) PushToObjList(pObj);

	return pObj;

	//gScene->createActor(actorDesc)->userData = (void*)size_t(size);
	//printf("Total: %d actors\n", gScene->getNbActors());
}

APhysXObject* CAPhysXDebugRenderTestView::CreateCapsule(const NxVec3& pos, float height, float radius, const NxVec3* initialVelocity /* = NULL */, const char* szName /* = NULL */, bool bPushToObjList /* = true */)
{
	if(m_pPhysXScene == NULL) 
		return NULL;	

	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.f;
	apxActorDesc.mLinearDamping = 0.05f;
	apxActorDesc.mAngularDamping = 0.05f;
	apxActorDesc.SetName("SphereActor");
	apxActorDesc.mBodyFlags |= NX_BF_FROZEN_ROT;

	if(initialVelocity) 
		apxActorDesc.mLinearVelocity = *initialVelocity;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXCapsuleShapeDesc capsuleDesc;
	capsuleDesc.mHeight = height;
	capsuleDesc.mRadius = radius;
	capsuleDesc.UseDefaultMaterial();

	capsuleDesc.SetName("Capsule");
	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&capsuleDesc);


	APhysXDynamicRBObjectDesc apxDummyRBObjDesc;
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	apxDummyRBObjDesc.SetName(szName);

	NxMat34 mtPose;
	mtPose.t = pos;

	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = &apxDummyRBObjDesc;
	apxObjInstanceDesc.mGlobalPose = mtPose;

	// who will be in charge of deleting the mPhysXUserGraphicsSync?

	APhysXObject* pObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

	if(bPushToObjList) PushToObjList(pObj);

	return pObj;
}

APhysXObject* CAPhysXDebugRenderTestView::CreateSphere(const NxVec3& pos, float size, const NxVec3* initialVelocity, const char* szName, bool bPushToObjList)
{
	if(m_pPhysXScene == NULL) 
		return NULL;	

	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.f;
	apxActorDesc.mLinearDamping = 0.05f;
	apxActorDesc.mAngularDamping = 0.05f;
	apxActorDesc.SetName("SphereActor");

	if(initialVelocity) 
		apxActorDesc.mLinearVelocity = *initialVelocity;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXSphereShapeDesc sphereDesc;
	sphereDesc.mRadius = size;
	sphereDesc.UseDefaultMaterial();

	sphereDesc.SetName("Sphere");
	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&sphereDesc);


	APhysXDynamicRBObjectDesc apxDummyRBObjDesc;
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	apxDummyRBObjDesc.SetName(szName);

	NxMat34 mtPose;
	mtPose.t = pos;

	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = &apxDummyRBObjDesc;
	apxObjInstanceDesc.mGlobalPose = mtPose;

	// who will be in charge of deleting the mPhysXUserGraphicsSync?

	APhysXObject* pObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

	if(bPushToObjList) PushToObjList(pObj);

	return pObj;

}


void CAPhysXDebugRenderTestView::CreateConvex(const NxVec3& pos, const NxVec3* initialVelocity, const NxVec3& vScale3D, const char* szFileName, const char* szObjName)
{

	NxMat34 mtPose;
	mtPose.t = pos;
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(szFileName, szObjName);
	apxObjInstanceDesc.mGlobalPose = mtPose;
	apxObjInstanceDesc.mScale3D = vScale3D;

	m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);


}

void CAPhysXDebugRenderTestView::CreateCubeFromEye(float size)
{
	NxVec3 t = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetPos());
	NxVec3 vel = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetDir());;
	vel*= 30.0f;
	
	static int iCur = 0;
	static float fScale = 1.0f;
	if(++iCur >= CONVEX_OBJ_DESC_NUM)
	{
		iCur = 0;
		fScale += 1;

		if(fScale == 6.0f)
			fScale = 1.0f;
	}

#if 0

	char szObjName[100];
	sprintf(szObjName, "%s%d", APHYSX_OBJECTDESCNAME_PREFIX, iCur);
	CreateConvex(t, &vel, NxVec3(fScale), s_szFileName, szObjName);
#else

	//CreateConvex(iCur, t, &vel, NxVec3(fScale));
	CreateCube(t, size, &vel);
	//CreateCapsule(t, 2.0f, size, &vel);
	
	//m_pPhysXScene->CreateDummySphereActor(t, 1.0f);

#endif

}


void CAPhysXDebugRenderTestView::SaveJointedObjectDescs()
{


	int size = 1;
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.SetName("Joint_Cube_1");
	apxActorDesc.mHasBody = true;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3((float)size, (float)size, (float)size);
	//boxDesc.mMaterialIndex = APhysXBoxShapeDesc::APX_SHAPEMATERIALINDEX_USEDEFAULT;
	boxDesc.UseDefaultMaterial();


	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	gPhysXEngine->GetObjectDescManager()->Release();

	// create joints...
	for(int iCurJointType = 0; iCurJointType < JOINT_TYPE_INVALID ; iCurJointType++)
	{
		APhysXObjectDesc* pObjDesc = gPhysXEngine->GetObjectDescManager()->CreateAndPushObjectDesc(APX_OBJTYPE_DUMMY);
		APhysXDummyRBObjectDesc& apxDummyRBObjDesc = (APhysXDummyRBObjectDesc&)*pObjDesc;
		apxActorDesc.SetName("Joint_Cube_2");
		apxActorDesc.mHasBody = true;
		apxActorDesc.mLocalPose.t = NxVec3(0, -3.0f, 0);
		apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);

		switch(iCurJointType)
		{
		case JOINT_TYPE_DISTANCE:

			{
				APhysXDistanceJointDesc distDesc;
				// sphericalDesc.mNxActor1Name = "Joint_Cube_1";
				distDesc.mNxActor1Name = "NULL";
				distDesc.mNxActor2Name = "Joint_Cube_2";
				distDesc.mGlobalAnchor = NxVec3(0.0f);
				distDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);
				distDesc.mMinDistance = 1.0f;
				distDesc.mMaxDistance = 6.0f;
				distDesc.mDistanceJointFlags = NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED;
				apxDummyRBObjDesc.AddInnerConnector(distDesc);
				apxDummyRBObjDesc.SetName("DistanceJointObj");

			}

			break;

		case JOINT_TYPE_D6:
			{


				APhysXD6JointDesc d6Desc;
				d6Desc.mNxActor1Name = "NULL";
				d6Desc.mNxActor2Name = "Joint_Cube_2";
				d6Desc.mGlobalAnchor = NxVec3(0.0f);
				d6Desc.mGlobalAxis = NxVec3(0, 1.0f, 0);

				d6Desc.mTwistMotion = NX_D6JOINT_MOTION_LOCKED;
				d6Desc.mSwing1Motion = NX_D6JOINT_MOTION_LOCKED;
				d6Desc.mSwing2Motion = NX_D6JOINT_MOTION_FREE;

				d6Desc.mXMotion = NX_D6JOINT_MOTION_LOCKED;
				d6Desc.mYMotion = NX_D6JOINT_MOTION_LOCKED;
				d6Desc.mZMotion = NX_D6JOINT_MOTION_LOCKED;

				apxDummyRBObjDesc.AddInnerConnector(d6Desc);

				apxDummyRBObjDesc.SetName("D6JointObj");

			}
			break;

		case JOINT_TYPE_SPHEREICAL:
			{
				APhysXSphericalJointDesc sphericalDesc;
				// sphericalDesc.mNxActor1Name = "Joint_Cube_1";
				sphericalDesc.mNxActor1Name = "NULL";
				sphericalDesc.mNxActor2Name = "Joint_Cube_2";
				sphericalDesc.mGlobalAnchor = NxVec3(0.0f);
				sphericalDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);

				apxDummyRBObjDesc.AddInnerConnector(sphericalDesc);

				apxDummyRBObjDesc.SetName("SphericalJointObj");

			}

			break;
		case JOINT_TYPE_REVOLUTE:
			{
				APhysXRevoluteJointDesc revoluteDesc;
				revoluteDesc.mNxActor1Name = "NULL";
				revoluteDesc.mNxActor2Name = "Joint_Cube_2";
				revoluteDesc.mGlobalAnchor = NxVec3(0.0f);
				revoluteDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);

				revoluteDesc.mMotor.maxForce = 1000;
				revoluteDesc.mMotor.velTarget = 0.15;
				revoluteDesc.mRevoluteJointFlags |= NX_RJF_MOTOR_ENABLED;

				apxDummyRBObjDesc.AddInnerConnector(revoluteDesc);
				
				apxDummyRBObjDesc.SetName("RevoluteJointObj");
			}
			break;
		default:
			break;
		}

	}

	gPhysXEngine->GetObjectDescManager()->Save(s_szJointedObjectsFileName);

	gPhysXEngine->GetObjectDescManager()->Release();
}


void CAPhysXDebugRenderTestView::LoadSkeletonObject(const char* szSkeletonObjectFile, const NxVec3& vPos)
{
	// load jointed object desc...
	APhysXObjectDesc* pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(szSkeletonObjectFile);
	if(!pObjDesc)
	{
		char szAttachedRBName[256];
		sprintf(szAttachedRBName, "CommonCompoundObjectDescName_%s", APX_ATTACHER_ATTACHED_RB_NAME);
		pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(szSkeletonObjectFile, szAttachedRBName);

		if(!pObjDesc) return;
	}
		
/*
	APhysXDynamicRBObjectDesc rbDesc;
	{
	
		// rb attacher object desc...
		APhysXActorDesc apxActorDesc;
		apxActorDesc.mDensity = 1.0f;

		APhysXMaterialDesc apxMaterialDesc;
		apxMaterialDesc.mDynamicFriction = 0.1f;
		apxMaterialDesc.mRestitution = 1.0f;

		APhysXSphereShapeDesc sphereDesc;
		sphereDesc.mRadius = 0.01f;
		sphereDesc.UseDefaultMaterial();

		APhysXArray<APhysXShapeDesc* > Shapes;
		Shapes.push_back(&sphereDesc);

		
		rbDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	
	}
*/

// test breakable skeleton object...

	APhysXSimpleBreakableSkeletonRBObjectDesc SBSRBObjectDesc; // = *(APhysXBreakableSkeletonRBObjectDesc *)pObjDesc;
	//(APhysXSkeletonRBObjectDesc&)BSRBObjectDesc = *(APhysXSkeletonRBObjectDesc *)pObjDesc;
	(APhysXSkeletonRBObjectDesc&)SBSRBObjectDesc = *(APhysXSkeletonRBObjectDesc *)pObjDesc;

	SBSRBObjectDesc.mBreakLimit = 20;
	SBSRBObjectDesc.mDynamic = true;


	APhysXBreakableSkeletonRBObjectDesc BSRBObjectDesc; 	
	(APhysXSkeletonRBObjectDesc&)BSRBObjectDesc = *(APhysXSkeletonRBObjectDesc *)pObjDesc;

	DeclareSphereAttacherDesc(rbDesc);
	//DeclareBoxAttacherDesc(rbDesc);

	NxMat34 matAttacherPose;
	// matAttacherPose.t.set(0, 1.68f, 0);
	matAttacherPose.t.set(0, 1.8f, 0);

	APhysXRBAttacherDesc apxRBAttacherDesc;
	apxRBAttacherDesc.AddRBComponent(&BSRBObjectDesc);
	// apxRBAttacherDesc.AddRBComponent(pObjDesc);

#if 0
	
	apxRBAttacherDesc.AddAttacherComponent(&rbDesc, matAttacherPose);
	// apxRBAttacherDesc.AddAttachInfo("Head", APX_RB_ATTYPE_ROTATION_FREE, NxVec3(0, 1.68f, 0));
	apxRBAttacherDesc.AddAttachInfo("Head", APX_RB_ATTYPE_FIX, NxVec3(0, 1.68f, 0));

#endif

	// test code ...
	APhysXActorDesc apxActorDesc;


	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mGlobalPose.t = vPos;
	// scale the model...
	// apxObjInstanceDesc.mScale3D.set(5.0f);

// #define	TEST_RBATTACHER
#ifdef	TEST_RBATTACHER

	apxObjInstanceDesc.mPhysXObjectDesc = &apxRBAttacherDesc;
	APhysXRBAttacher* pRBAttacher = (APhysXRBAttacher*)m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);
	PushToObjList(pRBAttacher);

	pRBAttacher->EnableAttacherKinematic(false);
	pRBAttacher->GetRBObject()->EnableKinematic();
	s_pRBAttacher = pRBAttacher;

#else


#define TEST_SIMPLE_BREAKABLE 1
#if		TEST_SIMPLE_BREAKABLE
	apxObjInstanceDesc.mPhysXObjectDesc = &SBSRBObjectDesc;
#else
	apxObjInstanceDesc.mPhysXObjectDesc = &BSRBObjectDesc;
#endif


	APhysXObject* pObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

	if(pObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE))
	{
		// snapshot the scene...
		APhysXUtilLib::SnapshotScene(*m_pPhysXScene->GetNxScene());

		APhysXBreakableSkeletonRBObject* pBSRBObj = (APhysXBreakableSkeletonRBObject*) pObj;
		pBSRBObj->BuildActorNeighborGraph(WF_SWEEP_EPSILON);
		pBSRBObj->BuildActorSupportGraph(WF_SWEEP_EPSILON);


		// just test the exporting of breakable object's neighbor graph and support graph...
		BSRBObjectDesc.SetActorNeighborGraph(pBSRBObj->GetActorNeighborGraph());
		BSRBObjectDesc.SetActorSupportGraph(pBSRBObj->GetActorSupportGraph());

		// create the object by the new desc...
		m_pPhysXScene->ReleasePhysXObject(pBSRBObj, true);
		// m_pPhysXScene->ReleasePhysXObject(pBSRBObj);
		
		APhysXBreakableSkeletonRBObjectDesc BSRBObjectDesc_Copy; 
		BSRBObjectDesc_Copy = BSRBObjectDesc;
		// BSRBObjectDesc_Copy.mActorBreakLimit = 100.0f;				// just use the default value...

		// to test the breaking case which has some user-specified fixed actors!
		// APhysXActorDesc* apxModActorDesc = BSRBObjectDesc_Copy.GetActorDesc(10);
		// apxModActorDesc->mBodyFlags |= NX_BF_KINEMATIC;

		apxObjInstanceDesc.mPhysXObjectDesc = &BSRBObjectDesc_Copy;
		pObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

		// snap-shot scene again to make the overlap test take effect...
		APhysXUtilLib::SnapshotScene(*m_pPhysXScene->GetNxScene());

	}
	else if(pObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE))
	{
		APhysXSimpleBreakableSkeletonRBObject* pBSRBObj = (APhysXSimpleBreakableSkeletonRBObject*) pObj;
		// pBSRBObj->EnableKinematic();

	}
	

	PushToObjList(pObj);

#endif


#if 0
	
	s_gRBAttacher = (APhysXRBAttacher *)pSkeletonObj;
	s_gRBAttacher->EnableAttacherKinematic(false);

#endif
	
#define	SAVE_SKELETON_MODEL 0
#if		SAVE_SKELETON_MODEL
	
	// save the data...
	APhysXObjectDescManager apxObjDescManager;

	apxObjDescManager.Add(pObjDesc);
	apxObjDescManager.Save(s_szSkeletonFileName);

#endif

}


void CAPhysXDebugRenderTestView::LoadSkeletonObjectFromEye(const char* szSkeletonObjectFile)
{

	NxVec3 vEyePos = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetPos());
	
	NxVec3 vel = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetDir());;
	vel*= 100.0f;

	LoadSkeletonObject(szSkeletonObjectFile, vEyePos);

}

void CAPhysXDebugRenderTestView::LoadJointedObjectFromEye()
{
	static int iCurJointType = 0;

	NxVec3 t = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetPos());
	NxVec3 vel = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetDir());;
	vel*= 100.0f;

	// load jointed object desc...
	APhysXObjectDesc* pObjDesc = NULL;
	switch(iCurJointType)
	{
	case JOINT_TYPE_DISTANCE:

		{
			pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(s_szJointedObjectsFileName, "DistanceJointObj");
		}

		break;

	case JOINT_TYPE_D6:
		{
			pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(s_szJointedObjectsFileName, "D6JointObj");

		}
		break;

	case JOINT_TYPE_SPHEREICAL:
		{
			pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(s_szJointedObjectsFileName, "SphericalJointObj");
		}

		break;
	case JOINT_TYPE_REVOLUTE:
		{
			pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(s_szJointedObjectsFileName, "RevoluteJointObj");
		}
		break;
	default:
		break;
	}

	NxMat34 mtPose;
	mtPose.t = t;
	static float s_fScale = 1.0f;

	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = pObjDesc;
	apxObjInstanceDesc.mGlobalPose = mtPose;
	apxObjInstanceDesc.mScale3D = NxVec3(s_fScale);
	
	APhysXRigidBodyObject* pJointedObj = (APhysXRigidBodyObject*)m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

#define TEST_RUNTIME_CONNECTOR_CREATION
#ifdef	TEST_RUNTIME_CONNECTOR_CREATION

	// create compound object
	// APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc("MyClothAttacher.pxd");
	apxObjInstanceDesc.mGlobalPose.t = t;
	//apxObjInstanceDesc.mGlobalPose.t.y -= 2.0f;
	APhysXClothAttacher* pClothAttacher = (APhysXClothAttacher*)m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);	
	APhysXRigidBodyObject* pAttacher = pClothAttacher->GetAttacherObject();
	

	APhysXFixedJointDesc apxFixedJointDesc;
	apxFixedJointDesc.mPhysXObject1 = pJointedObj;
	apxFixedJointDesc.mNxActor1 = pJointedObj->GetNxActor(0);

	apxFixedJointDesc.mPhysXObject2 =pAttacher;
	// pAttacher->EnableKinematic(false);
	pClothAttacher->EnableAttacherKinematic(false);
	
	NxMat34 matPose;
	matPose.t = t;
	matPose.t.y -= 2.0f;
	pAttacher->SetPose(matPose);

	apxFixedJointDesc.mNxActor2 = pAttacher->GetNxActor(0);
	
	APhysXSphericalJointDesc sphericalDesc;
	sphericalDesc.mGlobalAnchor = t;
	sphericalDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);
	sphericalDesc.mPhysXObject1 = pJointedObj;
	sphericalDesc.mNxActor1 = pJointedObj->GetNxActor(0);

	sphericalDesc.mPhysXObject2 =pAttacher;
	sphericalDesc.mNxActor2 = pAttacher->GetNxActor(0);

	APhysXObjectConnectorInstanceDesc apxConnectorInstanceDesc;
	apxConnectorInstanceDesc.mPhysXConnectorDesc = &apxFixedJointDesc;
	
	// apxConnectorInstanceDesc.mPhysXConnectorDesc = &sphericalDesc;

	m_pPhysXScene->CreatePhysXObjectConnector(apxConnectorInstanceDesc);


#endif

	
	
	
	if(++iCurJointType == JOINT_TYPE_INVALID)
	{
		iCurJointType = 0;
		s_fScale *= 2.0f;
	}

}

APhysXObject* CAPhysXDebugRenderTestView::CreateJointedObject(const NxVec3& vPos, float fDim, APhysXU32 uJointType /* = JOINT_TYPE_SPHEREICAL */)
{
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.SetName("Joint_Cube_1");
	apxActorDesc.mHasBody = true;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3(fDim, fDim, fDim);
	//boxDesc.mMaterialIndex = APhysXBoxShapeDesc::APX_SHAPEMATERIALINDEX_USEDEFAULT;
	boxDesc.UseDefaultMaterial();


	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	APhysXDummyRBObjectDesc apxDummyRBObjDesc;
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);

	apxActorDesc.SetName("Joint_Cube_2");
	apxActorDesc.mHasBody = true;
	apxActorDesc.mLocalPose.t = NxVec3(0, -3.0f, 0);
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);

	// create joints...
	switch(uJointType)
	{
	case JOINT_TYPE_DISTANCE:

		{
			APhysXDistanceJointDesc distDesc;
			// sphericalDesc.mNxActor1Name = "Joint_Cube_1";
			distDesc.mNxActor1Name = "NULL";
			distDesc.mNxActor2Name = "Joint_Cube_2";
			distDesc.mGlobalAnchor = NxVec3(0.0f);
			distDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);
			distDesc.mMinDistance = 1.0f;
			distDesc.mMaxDistance = 6.0f;
			distDesc.mDistanceJointFlags = NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED;
			apxDummyRBObjDesc.AddInnerConnector(distDesc);

		}

		break;

	case JOINT_TYPE_D6:
		{


			APhysXD6JointDesc d6Desc;
			d6Desc.mNxActor1Name = "NULL";
			d6Desc.mNxActor2Name = "Joint_Cube_2";
			d6Desc.mGlobalAnchor = NxVec3(0.0f);
			d6Desc.mGlobalAxis = NxVec3(0, 1.0f, 0);

			d6Desc.mTwistMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mSwing1Motion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mSwing2Motion = NX_D6JOINT_MOTION_FREE;

			d6Desc.mXMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mYMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mZMotion = NX_D6JOINT_MOTION_LOCKED;

			apxDummyRBObjDesc.AddInnerConnector(d6Desc);

		}
		break;

	case JOINT_TYPE_SPHEREICAL:
		{
			APhysXSphericalJointDesc sphericalDesc;
			sphericalDesc.mNxActor1Name = "Joint_Cube_1";
			//sphericalDesc.mNxActor1Name = "NULL";
			sphericalDesc.mNxActor2Name = "Joint_Cube_2";
			sphericalDesc.mGlobalAnchor = NxVec3(0.0f, -1.5f, 0.0f);
			sphericalDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);

			apxDummyRBObjDesc.AddInnerConnector(sphericalDesc);

		}

		break;
	case JOINT_TYPE_REVOLUTE:
		{
			APhysXRevoluteJointDesc revoluteDesc;
			revoluteDesc.mNxActor1Name = "NULL";
			revoluteDesc.mNxActor2Name = "Joint_Cube_2";
			revoluteDesc.mGlobalAnchor = NxVec3(0.0f);
			revoluteDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);

			revoluteDesc.mMotor.maxForce = 1000;
			revoluteDesc.mMotor.velTarget = 0.15;
			revoluteDesc.mRevoluteJointFlags |= NX_RJF_MOTOR_ENABLED;

			apxDummyRBObjDesc.AddInnerConnector(revoluteDesc);
		}
		break;
	default:
		break;
	}

	//SetJointParameters(sphericalDesc,pendulum[i],0,g);

	NxMat34 mtPose;
	mtPose.t = vPos;
	static float s_fScale = 1.0f;

	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = &apxDummyRBObjDesc;
	apxObjInstanceDesc.mGlobalPose = mtPose;
	apxObjInstanceDesc.mScale3D = NxVec3(s_fScale);
	return m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);
	
}

void CAPhysXDebugRenderTestView::CreateJointedObjectFromEye()
{
	static int iCurJointType = JOINT_TYPE_SPHEREICAL;

	NxVec3 t = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetPos());
	NxVec3 vel = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetDir());;
	vel*= 100.0f;

	// create physX object
	// CreateCube(t-NxVec3(0, 3, 0), 1, 0, "Joint_Cube_0");
	int size = 1;

	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.SetName("Joint_Cube_1");
	apxActorDesc.mHasBody = true;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3((float)size, (float)size, (float)size);
	//boxDesc.mMaterialIndex = APhysXBoxShapeDesc::APX_SHAPEMATERIALINDEX_USEDEFAULT;
	boxDesc.UseDefaultMaterial();

	
	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	APhysXDummyRBObjectDesc apxDummyRBObjDesc;
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	
	apxActorDesc.SetName("Joint_Cube_2");
	apxActorDesc.mHasBody = true;
	apxActorDesc.mLocalPose.t = NxVec3(0, -3.0f, 0);
	apxDummyRBObjDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);

	// create joints...
	switch(iCurJointType)
	{
	case JOINT_TYPE_DISTANCE:
		
		{
			APhysXDistanceJointDesc distDesc;
			// sphericalDesc.mNxActor1Name = "Joint_Cube_1";
			distDesc.mNxActor1Name = "NULL";
			distDesc.mNxActor2Name = "Joint_Cube_2";
			distDesc.mGlobalAnchor = NxVec3(0.0f);
			distDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);
			distDesc.mMinDistance = 1.0f;
			distDesc.mMaxDistance = 6.0f;
			distDesc.mDistanceJointFlags = NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED;
			apxDummyRBObjDesc.AddInnerConnector(distDesc);

		}

		break;
	
	case JOINT_TYPE_D6:
		{

		
			APhysXD6JointDesc d6Desc;
			d6Desc.mNxActor1Name = "NULL";
			d6Desc.mNxActor2Name = "Joint_Cube_2";
			d6Desc.mGlobalAnchor = NxVec3(0.0f);
			d6Desc.mGlobalAxis = NxVec3(0, 1.0f, 0);

			d6Desc.mTwistMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mSwing1Motion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mSwing2Motion = NX_D6JOINT_MOTION_FREE;

			d6Desc.mXMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mYMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.mZMotion = NX_D6JOINT_MOTION_LOCKED;

			apxDummyRBObjDesc.AddInnerConnector(d6Desc);

		}
		break;
	
	case JOINT_TYPE_SPHEREICAL:
		{
			APhysXSphericalJointDesc sphericalDesc;
			sphericalDesc.mNxActor1Name = "Joint_Cube_1";
			//sphericalDesc.mNxActor1Name = "NULL";
			sphericalDesc.mNxActor2Name = "Joint_Cube_2";
			sphericalDesc.mGlobalAnchor = NxVec3(0.0f, -1.5f, 0.0f);
			sphericalDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);

			apxDummyRBObjDesc.AddInnerConnector(sphericalDesc);

		}

	    break;
	case JOINT_TYPE_REVOLUTE:
		{
			APhysXRevoluteJointDesc revoluteDesc;
			revoluteDesc.mNxActor1Name = "NULL";
			revoluteDesc.mNxActor2Name = "Joint_Cube_2";
			revoluteDesc.mGlobalAnchor = NxVec3(0.0f);
			revoluteDesc.mGlobalAxis = NxVec3(0, 1.0f, 0);

			revoluteDesc.mMotor.maxForce = 1000;
			revoluteDesc.mMotor.velTarget = 0.15;
			revoluteDesc.mRevoluteJointFlags |= NX_RJF_MOTOR_ENABLED;

			apxDummyRBObjDesc.AddInnerConnector(revoluteDesc);
		}
	    break;
	default:
	    break;
	}

	//SetJointParameters(sphericalDesc,pendulum[i],0,g);

	NxMat34 mtPose;
	mtPose.t = t;
	static float s_fScale = 1.0f;

	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = &apxDummyRBObjDesc;
	apxObjInstanceDesc.mGlobalPose = mtPose;
	apxObjInstanceDesc.mScale3D = NxVec3(s_fScale);
	m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

/*
	if(++iCurJointType == JOINT_TYPE_INVALID)
	{
		iCurJointType = 0;
		s_fScale *= 2.0f;
	}
*/

}

void CAPhysXDebugRenderTestView::CreateStack(int size)
{
	const float cubeSize = 1.0f;
	const float spacing = -2.0f*gPhysXEngine->GetNxPhysicsSDK()->getParameter(NX_SKIN_WIDTH);
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	float offset = -size * (cubeSize * 2.0f + spacing) * 0.5f;
	while(size)
	{
		for(int i=0;i<size;i++)
		{
			pos.x = offset + (float)i * (cubeSize * 2.0f + spacing);
			CreateCube(pos, (int)cubeSize);
		}

		offset += cubeSize;
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

void CAPhysXDebugRenderTestView::CreateTower(int size)
{
	const float cubeSize = 1.0f;
	const float spacing = 0.01f;
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	while(size)
	{
		CreateCube(pos, (int)cubeSize);
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

//*/

void CAPhysXDebugRenderTestView::PauseSimulation()
{
	if(m_pPhysXScene)
	{
		m_bPauseSimulation = !m_bPauseSimulation;
		m_pPhysXScene->PauseSimulate(m_bPauseSimulation);
	}

}

void CAPhysXDebugRenderTestView::InitPhysXEngine()
{
	AString strTest;
	strTest.Format("Just a test: %d, %f,", 10, 11.0f);

#if 1

	APhysXEngineDesc apxEngineDesc;
	apxEngineDesc.mEnableVRD = true;
	apxEngineDesc.mA3DEngine = g_Render.GetA3DEngine();
	
	AString strPhysXPath = af_GetBaseDir();
	strPhysXPath += "\\PhysX";

	// apxEngineDesc.mPhysXCorePath = strPhysXPath;
	
	// apxEngineDesc.mPhysXCorePath = TEXT("E:\Developement\Source_ Depot\APhysXIntegration\bin");


	// apxEngineDesc.mDisableHardwarePhysXSupport = true;

	if(!gPhysXEngine->Init(apxEngineDesc))
		return;

	/*
	for(int i=0; i<10; i++)
	{
		char szName[100];
		sprintf(szName, "ConvexMesh%d", i);
		GenerateCovexMeshShapeDesc(szName);
	}
	*/

	//float fHWBenchmarckFPS = APhysXUtilLib::PhysXBenchmark();
	//float fSWBenchmarckFPS = APhysXUtilLib::PhysXBenchmark(false);

	//gPhysXLog->Log("PhysX Benchmark HW FPS: %.1f", fHWBenchmarckFPS);
	//gPhysXLog->Log("PhysX Benchmark SW FPS: %.1f", fSWBenchmarckFPS);

	//if(fHWBenchmarckFPS > 50)
	//	gPhysXEngine->EnableHardwarePhysX();


	gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LIMITS, 0);
	gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_JOINT_LOCAL_AXES , 0);
	// gPhysXEngine->SetNxPhysicsSDKParameter(NX_VISUALIZE_ACTOR_AXES, 1);

	APhysXSceneDesc apxSceneDesc;
	if(gPhysXEngine->HasPhysXHardware())
	{
		gHasHWPhysX = true;
		// apxSceneDesc.mNxSceneDesc.simType = NX_SIMULATION_HW;
	}


	apxSceneDesc.mEnableDebugRender = true;
	apxSceneDesc.mEnablePerfStatsRender = true;
	apxSceneDesc.mNxSceneDesc.groundPlane = true;
	apxSceneDesc.mNxSceneDesc.gravity = APHYSX_STD_GRAVITY;

	apxSceneDesc.mDefaultMaterial.mRestitution = 0.3f;
	// apxSceneDesc.mDefaultMaterial.mRestitution = 1.0f;
	apxSceneDesc.mDefaultMaterial.mStaticFriction = 0.5f;
	apxSceneDesc.mDefaultMaterial.mDynamicFriction = 0.5f;
	apxSceneDesc.mGroundPlaneElevation = -3.0f;
	// apxSceneDesc.mSimulateTimeStepMethod = APX_SIMULATE_TSM_VARIABLE;

	// apxSceneDesc.mTimeStep = 0.1f;

	APHYSX_ASSERT(apxSceneDesc.mNxSceneDesc.isValid());
	m_pPhysXScene = gPhysXEngine->CreatePhysXScene(apxSceneDesc);
	m_pPhysXScene->StartSimulate();
	
#if 0
	// to test compartment...
	NxReal rCmptSubTimestep;
	NxU32 uCmptMaxIter;
	NxTimeStepMethod cmptTimestepMethod;
	m_pPhysXScene->GetRBCompartment()->getTiming(rCmptSubTimestep, uCmptMaxIter, cmptTimestepMethod);
	m_pPhysXScene->GetRBCompartment()->setTimeScale(0.1f);

#endif

	// m_pPhysXScene->EnableExtraDebugRender();

	// test terrain...
	// m_pPhysXScene->InitTerrain("Maps\\1\\1Render_ptb.cfg");

	// test multi-scene...
	// s_pDummyScene =  gPhysXEngine->CreatePhysXScene(apxSceneDesc);
	// s_pDummyScene->StartSimulate();

	// m_apxPicker.Init(m_pPhysXScene, g_Render.GetA3DViewport());

	m_pPhysXScene->InitPicker(g_Render.GetA3DViewport());

	// activate my own contact report...
	// m_pPhysXScene->GetNxScene()->setActorGroupPairFlags(0,0,NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_FORCES);

	m_pPhysXScene->GetNxScene()->setUserNotify(&s_MyUserNotify);
	// m_pPhysXScene->GetNxScene()->setUserContactReport(&s_MyUserContactReport);

	//CreateStack(10);
#endif


	cubeGSync.SetFCAndWC(g_Render.GetA3DEngine()->GetA3DFlatCollector(), g_Render.GetA3DEngine()->GetA3DWireCollector());
		
#if 0
	// create a controlled cube
	APhysXRigidBodyObject* pCubeObj = (APhysXRigidBodyObject *)CreateCube(NxVec3(0, 3.0f, 0), 5, NULL, NULL, false);
	s_pControledActor = pCubeObj->GetNxActor(NULL);
	s_pControledActor->raiseBodyFlag(NX_BF_KINEMATIC);
#endif

#if 0

	// test Terrain...
	m_pPhysXScene->InitTerrain("Maps\\testPhysi\\testPhysi_ptb.cfg");
	m_pPhysXScene->EnableTerrainDebugRender();

#endif

	// test CC...
	APhysXCCCapsuleDesc CCDesc;
	CCDesc.radius = 0.3f;
	CCDesc.height = 1.2f;

	s_pAPhysXCC = gPhysXEngine->GetAPhysXCCManager()->CreateCC(*m_pPhysXScene, CCDesc);
	APHYSX_ASSERT(s_pAPhysXCC);

}

//------------------------------------------------------
// for testing ...
class D
{
public:
	// data...
	int i ;
	int* pi;

	// functions...
	D(int ii = 0)
	{
		i = ii;
		if(i > 0)
		{
			pi = new int[i];
			for(int j = 0; j<i; j++)
				pi[j] = j;

		}
		else
			pi = NULL;
	}

	~D()
	{
		delete [] pi;
	}

	const D& operator= (const D& other)
	{
		if(pi) delete pi;

		i = other.i;
		if(i > 0)
		{
			pi = new int[i];
			for(int j = 0; j<i; j++)
				pi[j] = j;

		}
		else
			pi = NULL;

		return *this;
	}
};

class E
{
public:
	// data...
	D m_D;

	// functions...
	E(int i): m_D(i)
	{
	}

};

void CAPhysXDebugRenderTestView::TestAIniFile()
{
	// some code to test AIniFile.h
	AIniFile testIni;
	testIni.WriteIntValue("Map", "Column Number", 8);
	testIni.WriteIntValue("Map", "Row Number", 11);
	testIni.Save("wftestini.ini");

	testIni.Close();

	//bool af_GetFilePath(const char* lpszFile, char* lpszPath, WORD cbBuf);
	//bool af_GetFilePath(const char* lpszFile, AString& strPath);
	char szPath[128];
	char szBuf[128];
	af_GetFilePath("E:\\Developement\\Source_ Depot\\APhysXIntegration\\APhysXDebugRenderTest\\Global.cpp", szPath, 18);

	sprintf(szBuf, "%s\\%d.ptd", szPath, 10);

	// test some other stuff...
	class A
	{
	public:
		int a;
		A() { a = 1;}
	};

	class B: public A
	{
	public:	
		int b;
		B() { b = 2;}
	};

	class C: public A
	{
	public:	
		int c;
		C() { c = 3;}
	};




	E e1(0), e2(10);

	e1 = e2;


	A a1;
	a1.a = 10;
	B b1;
	b1.a = 11;
	b1.b = 20;

	A* pA = new A;
	A* pB = new B;
	A* pC = new C;
	
	B* pBB = (B* )pB;
	C* pCC = (C* )pC;

	*pA = a1;
	*pB = b1;
	*pC = b1;
	
	*pBB = b1;
	//*pCC = b1;

	delete pA;
	delete pB;
	delete pC;

	//ASSERT(0 && "just a test!");
}

void CAPhysXDebugRenderTestView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	// rotate camera by mouse...
	if(nFlags & MK_LBUTTON )
	{

		A3DCamera* pA3DCamera = g_Render.GetA3DCamera();

		if(pA3DCamera)
		{
			float fScale = 0.3f;

			pA3DCamera->DegDelta( (point.x - m_msLastPos.x) * fScale);
			pA3DCamera->PitchDelta( - (point.y - m_msLastPos.y) * fScale);

		}
	}
	else if(nFlags & MK_RBUTTON)
	{
		// m_apxPicker.OnMouseMove(point.x, point.y);
		m_pPhysXScene->GetPhysXPicker()->OnMouseMove(point.x, point.y);
		
	}


	m_msLastPos = point;

	CView::OnMouseMove(nFlags, point);
}

void CAPhysXDebugRenderTestView::CreateClothObject(const NxVec3& vPos)
{

	NxMat34 mtID;
	NxMat34 mtID1(true);

	//-----------------------------------------------------------------------
	// generate a planar patch cloth mesh
	NxClothMeshDesc desc;

	float w = 10.0f;			// width
	float h = 6.0f;			// height
	float d = 0.3f;		// cell size

	int numX = (int)(w / d) + 1;
	int numY = (int)(h / d) + 1;

	desc.numVertices				= (numX+1) * (numY+1);
	desc.numTriangles				= numX*numY*2;
	desc.pointStrideBytes			= sizeof(NxVec3);
	desc.triangleStrideBytes		= 3*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.points						= (NxVec3*)malloc(sizeof(NxVec3)*desc.numVertices);
	desc.triangles					= (NxU32*)malloc(sizeof(NxU32)*desc.numTriangles*3);
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= 0;


	int i,j;
	NxVec3 *p = (NxVec3*)desc.points;
	for (i = 0; i <= numY; i++) {
		for (j = 0; j <= numX; j++) {
			p->set(d*j, 0.0f, d*i); 
			p++;
		}
	}


	NxU32 *id = (NxU32*)desc.triangles;
	for (i = 0; i < numY; i++) {
		for (j = 0; j < numX; j++) {
			NxU32 i0 = i * (numX+1) + j;
			NxU32 i1 = i0 + 1;
			NxU32 i2 = i0 + (numX+1);
			NxU32 i3 = i2 + 1;
			if ((j+i)%2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}


	// init APhysXClothObjectDesc object via the NxClothMeshDesc object
	APhysXClothObjectDesc clothDesc;
	static int s_iClothMaterial = 0;

	// clothDesc.mClothParameters.LoadPresetMaterial(APhysXClothParameters::APX_CLOTH_MATERIAL_SILK);
	// clothDesc.mClothParameters.LoadPresetMaterial(APhysXClothParameters::APX_CLOTH_MATERIAL_FLAX);
	// clothDesc.mClothParameters.LoadPresetMaterial(APhysXClothParameters::APX_CLOTH_MATERIAL_FELT);
	clothDesc.mClothParameters.LoadPresetMaterial(s_iClothMaterial);
	/*
	if(++s_iClothMaterial == 3)
	{	
		s_iClothMaterial = 0;
	}
	*/

	clothDesc.InitClothMesh(desc);

	// if(gHasHWPhysX)
	//	clothDesc.mClothParameters.mFlags |= NX_CLF_HARDWARE;
	
	// clothDesc.mSimInCompartment = true;

	//clothDesc.SetName("cloth");

	// release allocated memory in NxClothMeshDesc
	free((void *)desc.points);
	free((void *)desc.triangles);

	//-----------------------------------------------------------------------
	// init static rigid body object desc
	//*
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3((float)5.0f, (float)1.0f, (float)1.0f);
	boxDesc.UseDefaultMaterial();

	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	APhysXDynamicRBObjectDesc rbDesc;
	rbDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	//rbDesc.SetName("rb");

	//-----------------------------------------------------------------------

#if 0

	// compound object desc...

	APhysXCompoundObjectDesc compoundObjDesc;
	compoundObjDesc.AddObjectComponent(&clothDesc, "cloth");
	compoundObjDesc.AddObjectComponent(&rbDesc, "rb");

	APhysXClothAttachRBConnectorDesc clothAttachRBDesc;
	clothAttachRBDesc.mPhysXObject1Name = "cloth";
	clothAttachRBDesc.mPhysXObject2Name = "rb";

	compoundObjDesc.AddInnerConnector(clothAttachRBDesc);

#else

	// cloth attacher desc...

	APhysXClothAttacherDesc compoundObjDesc;
	compoundObjDesc.AddMultiClothComponent(&clothDesc, "cloth1");
	//compoundObjDesc.AddMultiClothComponent(&clothDesc, "cloth2");
	compoundObjDesc.AddAttacherComponent(&rbDesc);
	// compoundObjDesc.CompleteInnerConnectorInfo();

	
#endif

//#define SAVE_CLOTH_OBJECT_DESC
#ifdef	SAVE_CLOTH_OBJECT_DESC

	APhysXObjectDescManager apxObjDescManager;
	apxObjDescManager.Add(&compoundObjDesc);
	apxObjDescManager.Save("MyClothAttacher.pxd");

#else

	APhysXObjectDescManager apxObjDescManager;



	// create compound object
	APhysXObjectInstanceDesc apxObjInstanceDesc;
#if 0	
	apxObjInstanceDesc.mPhysXObjectDesc = apxObjDescManager.GetPhysXObjectDesc("MyClothAttacher.pxd");
#else

	apxObjInstanceDesc.mPhysXObjectDesc = &compoundObjDesc;

#endif

	apxObjInstanceDesc.mGlobalPose.t = vPos;
	APhysXClothAttacher* pClothObj = (APhysXClothAttacher*)m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);
	pClothObj->EnableAttacherKinematic(false);

#if 1
	// test collision channel...
	if(pClothObj)
	{
		// test code for collision channel...
		APhysXCollisionChannel colChannel = m_pPhysXScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
		if(colChannel != APX_COLLISION_CHANNEL_INVALID)
		{
			pClothObj->SetCollisionChannel(colChannel);
		}


		PushToObjList(pClothObj);

	}

#endif

#endif	//#define SAVE_CLOTH_OBJECT_DESC

	//*/

	/*
	APhysXObjectInstanceDesc clothInstanceDesc;
	clothInstanceDesc.mGlobalPose.t = vPos;
	clothInstanceDesc.mPhysXObjectDesc = &clothDesc;

	m_pPhysXScene->CreatePhysXObject(clothInstanceDesc);
	*/

}

void CAPhysXDebugRenderTestView::CreateClothObjectFromEye()
{
	NxVec3 vEyePos = APhysXConverter::A2N_Vector3(g_Render.GetA3DCamera()->GetPos());

	CreateClothObject(vEyePos);
}

void CAPhysXDebugRenderTestView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// m_apxPicker.OnMouseButtonDown(point.x, point.y);
	m_pPhysXScene->GetPhysXPicker()->OnMouseButtonDown(point.x, point.y);

	CView::OnRButtonDown(nFlags, point);
}

void CAPhysXDebugRenderTestView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	// m_apxPicker.OnMouseButtonUp(point.x, point.y);
	m_pPhysXScene->GetPhysXPicker()->OnMouseButtonUp(point.x, point.y);

	CView::OnRButtonUp(nFlags, point);
}


// static APhysXFluidObject* s_pApxFluidObject = NULL;

void CAPhysXDebugRenderTestView::CreateFluidEmitter()
{
	if(m_pPhysXScene && !m_pFluidObject)
	{
	
#if 0		
		// all parameters use the default values...
		APhysXFluidObjectDesc fluidObjDesc;

		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mPhysXObjectDesc = &fluidObjDesc;

		m_pFluidObject = (APhysXFluidObject *)m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

		if(m_pFluidObject)
		{
			// create the emitter...

			//Create an attached emitter
			NxFluidEmitterDesc emitterDesc;

			emitterDesc.maxParticles = 0;
			emitterDesc.dimensionX = 0.3f;
			emitterDesc.dimensionY = 0.3f;
			emitterDesc.randomAngle = 0.001f;
			emitterDesc.randomPos = NxVec3(0.0f,0.0f,0.0f);
			//The rate has no effect with type NX_FE_CONSTANT_PRESSURE
			emitterDesc.rate = 300.0f;
			emitterDesc.shape = NX_FE_RECTANGULAR;
			emitterDesc.type = NX_FE_CONSTANT_FLOW_RATE;
			emitterDesc.fluidVelocityMagnitude = 10.0f;
			emitterDesc.particleLifetime = 5.0f;

			//attach to actor
			emitterDesc.flags |= NX_FEF_ADD_BODY_VELOCITY;
			emitterDesc.repulsionCoefficient = 0.02f;

			m_pFluidObject->GetNxFluid()->createEmitter(emitterDesc);
		}
#else
		APhysXActorDesc apxActorDesc;
		apxActorDesc.mDensity = 1.0f;

		APhysXMaterialDesc apxMaterialDesc;
		apxMaterialDesc.mDynamicFriction = 0.1f;
		apxMaterialDesc.mRestitution = 1.0f;

		APhysXBoxShapeDesc boxDesc;
		// boxDesc.mDimensions = NxVec3((float)5.0f, (float)1.0f, (float)1.0f);
		boxDesc.mDimensions = NxVec3((float)0.3f, (float)0.3f, (float)0.3f);
		boxDesc.UseDefaultMaterial();

		APhysXArray<APhysXShapeDesc* > Shapes;
		Shapes.push_back(&boxDesc);

		APhysXDynamicRBObjectDesc rbDesc;
		rbDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);		
		
		// all parameters use the default values...
		// APhysXFluidObjectDesc fluidObjDesc;
		APhysXOrientedParticleObjectDesc fluidObjDesc;
		// fluidObjDesc.mSpinRadius = 0.5f;
		// fluidObjDesc.mInitialOrt = APX_OP_IORT_ID;
		// fluidObjDesc.mSpinType = APX_OP_SPINTYPE_FLAT;
		fluidObjDesc.mMaxAngularVel = APHYSX_PI * 4.0f;

		// fluidObjDesc.mMaxAngularVel = 30.0f;
		// fluidObjDesc.mFluidParameters.mMaxParticles = 10;
		fluidObjDesc.mFluidParameters.mCollisionDistanceMultiplier = 1.0f;
		fluidObjDesc.mFluidParameters.mRestDensity = 50;
		fluidObjDesc.mFluidParameters.mRestParticlesPerMeter = 10;
		// fluidObjDesc.mFluidParameters.mSimulationMethod = NX_F_SPH; 
		fluidObjDesc.mFluidParameters.mSimulationMethod = NX_F_NO_PARTICLE_INTERACTION;
		
		fluidObjDesc.mFluidParameters.mRestitutionForStaticShapes = 0.1f;
		//fluidObjDesc.mFluidParameters.mDynamicFrictionForStaticShapes = 0.1f;
		fluidObjDesc.mFluidParameters.mDynamicFrictionForStaticShapes = 0.1f;
		fluidObjDesc.mFluidParameters.mStaticFrictionForStaticShapes = 0.0f;

		fluidObjDesc.mFluidParameters.mPacketSizeMultiplier = 8;
		// fluidObjDesc.mFluidParameters.mDamping = 5.0f;

		// fluidObjDesc.mFluidParameters.mDynamicFrictionForDynamicShapes = 0.5f;

		

		fluidObjDesc.TrackParticles();

		// this is the key reason why the fluid emitter blocks if it is created in
		// force fields.
		fluidObjDesc.mFluidParameters.mFlags &= ~NX_FF_HARDWARE;
		fluidObjDesc.mFluidParameters.mFlags &= ~NX_FF_VISUALIZATION;

		APhysXCompoundObjectDesc compoundObjDesc;
		compoundObjDesc.AddObjectComponent(&fluidObjDesc, "fluid");
		compoundObjDesc.AddObjectComponent(&rbDesc, "rb");

		APhysXFluidEmitterAttachRBConnectorDesc fluidEmitterAttachRBDesc;
		fluidEmitterAttachRBDesc.mPhysXObject1Name = "fluid";
		fluidEmitterAttachRBDesc.mPhysXObject2Name = "rb";
		/*
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRelPose.t.set(0.0f, 0, 1.1f);
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mMaxParticles = 0;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mDimensionX = 0.8f;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mDimensionY = 0.6f;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRandomAngle = 0.001f;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRandomPos = NxVec3(0.0f,0.0f,0.0f);
		//The rate has no effect with type NX_FE_CONSTANT_PRESSURE
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRate = 300.0f;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mShape = NX_FE_ELLIPSE;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mType = NX_FE_CONSTANT_FLOW_RATE;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mFluidVelocityMagnitude = 10.0f;

		//attach to actor
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mFlags |= NX_FEF_ADD_BODY_VELOCITY;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRepulsionCoefficient = 0.02f;

		*/

		// fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRelPose.t.set(0.0f, 0, 1.1f);
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mRelPose.t.set(0.0f, 0, 0.1f);
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mType = NX_FE_CONSTANT_FLOW_RATE;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mParticleLifetime = 10.0f;
		fluidEmitterAttachRBDesc.mFluidEmitterParameters.mFluidVelocityMagnitude = 100.0f;
		
		compoundObjDesc.AddInnerConnector(fluidEmitterAttachRBDesc);

		APhysXFluidEmitterAttacherDesc apxFEAttacherDesc;
		apxFEAttacherDesc.AddFluidComponent(&fluidObjDesc);
		APhysXFluidEmitterParameters feParas;
		// feParas.mRelPose.t.set(0.0f, 0, 1.1f);
		feParas.mType = NX_FE_CONSTANT_FLOW_RATE;
		feParas.mShape = NX_FE_ELLIPSE;
		feParas.mParticleLifetime = 20.0f;
		feParas.mFluidVelocityMagnitude = 5.0f;
#if 0		
		feParas.mRelPose.M.setColumn(0, NxVec3(1.0f, 0, 0));
		feParas.mRelPose.M.setColumn(1, NxVec3(0, 0, 1.0f));
		feParas.mRelPose.M.setColumn(2, NxVec3(0, -1.0f, 0));
		feParas.mRandomPos.set(5.f);
#endif

		feParas.mRelPose.t.set(0.0f, 0, 0.1f);
		
		feParas.mRandomAngle = 0.5f;
		feParas.mRate = 1.0f;

		apxFEAttacherDesc.AddAttacherComponent(&rbDesc, feParas);

		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mGlobalPose.t.set(0, 0.0f, 0);

#if 0
		apxObjInstanceDesc.mPhysXObjectDesc = &compoundObjDesc;
#else

		apxObjInstanceDesc.mPhysXObjectDesc = &apxFEAttacherDesc;

#endif

		APhysXObject* pPhysXObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);
		APhysXFluidEmitterAttacher* pFEAttacher = (APhysXFluidEmitterAttacher*)pPhysXObj;
		m_pFluidObject = pFEAttacher->GetFluidObject();

		pFEAttacher->EnableAttacherKinematic(false);

		PushToObjList(pPhysXObj);

/*
		// create the fluid and fluid emitter directly...
		NxFluidDesc fDesc;
		fDesc.restDensity = 50;
		fDesc.restParticlesPerMeter = 10;
		NxFluid* pFluid = m_pPhysXScene->GetNxScene()->createFluid(fDesc);
		
		NxFluidEmitterDesc feDesc;
		feDesc.type = NX_FE_CONSTANT_FLOW_RATE;
		feDesc.particleLifetime = 10.0f;
		feDesc.fluidVelocityMagnitude = 10.0f;

		pFluid->createEmitter(feDesc);

*/

#endif

	}
}


void CAPhysXDebugRenderTestView::CreateForceField()
{

#if 1

#if 0

	APhysXForceFieldObjectDesc ffObjDesc;
	ffObjDesc.mFFLKernelParameters.mConstant.set(0, 9.81f, 0);
	
	ffObjDesc.mRBType = ffObjDesc.mFluidType = ffObjDesc.mClothType = ffObjDesc.mSBType = NX_FF_TYPE_GRAVITATIONAL;
	ffObjDesc.mFlags = 0;

#else

	// APhysXVacuumForceFieldObjectDesc ffObjDesc;
	// APhysXGravitationForceFieldObjectDesc ffObjDesc(NxVec3(0, 9.81f, 0));
	// APhysXBuoyancyForceFieldObjectDesc ffObjDesc(1.2f);
	// APhysXVortexForceFieldObjectDesc ffObjDesc;
/*	
	APhysXExplosionForceFieldObjectDesc ffObjDesc;
	APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters apxExlposionFFPara;
	//apxExlposionFFPara.mCenterForce = 10000.0f;
	ffObjDesc.mClothForceScale = 0.01f;
	ffObjDesc.SetExplosionFFParameters(apxExlposionFFPara);
*/

	APhysXWindForceFieldObjectDesc ffObjDesc;
	APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters apxWindFFPara;
	apxWindFFPara.mWindForce.set(0, 0, 1.1f);
	apxWindFFPara.mCycleTime = 3.0f;
	apxWindFFPara.mRestTime = 5.0f;
	ffObjDesc.SetWindFFParameters(apxWindFFPara);
	// ffObjDesc.mFFLKernelParameters.mPositionMultiplier.diagonal(NxVec3(0, 0, 0.1f));

#endif	


	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions.set(20.0f);
	boxDesc.mLocalPose.t.set(0, 10.0f, 0);

	ffObjDesc.mIncludeShapes.CreateAndPushShapeDesc(&boxDesc);

	APhysXObjectInstanceDesc objInstanceDesc;
	objInstanceDesc.mPhysXObjectDesc = &ffObjDesc;

	m_pFFObject = (APhysXForceFieldObject*)m_pPhysXScene->CreatePhysXObject(objInstanceDesc);


	NxVec3 vPos(0.0f, 1.0f, 0.0f), vVel(0.0f);
	NxVec3 vForce, vTorque;
	m_pFFObject->GetNxForceField()->samplePoints(1, &vPos, &vVel, &vForce, &vTorque);


	// m_pPhysXScene->GetNxScene()->setForceFieldScale(0, 0, 3.0f);

#else


	NxScene* pNxScene = m_pPhysXScene->GetNxScene();
	if(pNxScene)
	{
		NxForceFieldLinearKernelDesc fflKernelDesc;
		fflKernelDesc.constant.set(0, 9.81f, 0);

		NxForceFieldLinearKernel* pFFLKernel = pNxScene->createForceFieldLinearKernel(fflKernelDesc);

		NxForceFieldDesc ffDesc;

		ffDesc.kernel = pFFLKernel;

		ffDesc.coordinates		= NX_FFC_CARTESIAN;

#if 1

		ffDesc.rigidBodyType	= NX_FF_TYPE_GRAVITATIONAL;
		ffDesc.fluidType	= NX_FF_TYPE_GRAVITATIONAL;
		ffDesc.clothType	= NX_FF_TYPE_GRAVITATIONAL;
		ffDesc.softBodyType	= NX_FF_TYPE_GRAVITATIONAL;		

		ffDesc.flags = 0;

#endif


		//ffDesc.shapeGroups.push_back
		NxBoxForceFieldShapeDesc b;
		b.dimensions = NxVec3(20.0f);
		b.pose.t = NxVec3(0, 10, 0);

		ffDesc.includeGroupShapes.push_back(&b);

		NxForceField* pNxForceField = pNxScene->createForceField(ffDesc);

	}



#endif

}

void CAPhysXDebugRenderTestView::FetchPhysXProfileData()
{
	if(m_pPhysXScene)
	{
		NxScene* pNxScene = m_pPhysXScene->GetNxScene();

		const NxProfileData* pProfileData = pNxScene->readProfileData(false) ;

		const NxProfileZone* pProfileZone_ClientFrame = pProfileData->getNamedZone(NX_PZ_CLIENT_FRAME);
		const NxProfileZone* pProfileZone_CpuSimulate = pProfileData->getNamedZone(NX_PZ_CPU_SIMULATE);

		// pProfileZone_ClientFrame->

		const NxSceneStats2* pSceneStatsEx = pNxScene->getStats2();

		int i=0; 
	}

}


void CAPhysXDebugRenderTestView::PushToObjList(APhysXObject* pObj)
{
	if(!pObj) return;

	m_MutexPhysXObjList.Lock();

	m_arrPhysXObjList.push_back(pObj);

	m_MutexPhysXObjList.Unlock();
}


void CAPhysXDebugRenderTestView::RemoveObjListBack()
{
	if(!m_pPhysXScene) return;

	m_MutexPhysXObjList.Lock();

	if(m_arrPhysXObjList.size()>0)
	{
		APhysXObject* pBackObj = m_arrPhysXObjList[m_arrPhysXObjList.size() - 1];
		m_arrPhysXObjList.pop_back();

		m_pPhysXScene->ReleasePhysXObject(pBackObj);
	}

	m_MutexPhysXObjList.Unlock();
}

APhysXObject* CAPhysXDebugRenderTestView::OpenPXD(const char* szPxdName, const NxVec3& vPos)
{
	APhysXObjectDesc* pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(szPxdName);
	//gPhysXEngine->GetObjectDescManager()->Load(szPxdName);
	//APhysXObjectDesc* pObjDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(szPxdName);

	APhysXObjectInstanceDesc apxObjInstanceDesc;

	apxObjInstanceDesc.mPhysXObjectDesc = pObjDesc;
	apxObjInstanceDesc.mGlobalPose.t = vPos;

	APhysXObject* pObj = m_pPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

	if(pObj->IsAttacher())
	{
		// disable the kinematic attacher...
		APhysXAttacher* pAttacher = (APhysXAttacher*) pObj;
		// APhysXObject* pAttachObj = pAttacher->GetAttacherObject();
		
		// just test the joint release...
		APhysXRigidBodyObject* pRBObject = ((APhysXRBAttacher *)pAttacher)->GetRBObject();

		pRBObject->RemoveJoint("左大臂");

		// pAttacher->EnableAttacherKinematic(true);

	}

	PushToObjList(pObj);

	return pObj;

}





void CAPhysXDebugRenderTestView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	// ::AfxOutputDebugString("Middle mouse button down!\n");

	if(m_pPhysXScene)
	{
		// add force to the traced actor...
		// g_Render.GetA3DViewport()

		NxRay pickRay;

		APhysXPicker::GetPickingRay(g_Render.GetA3DViewport(), point.x, point.y, pickRay);

		APhysXRayHitInfo hitInfo;

		// APhysXRigidBodyObject* pLastRBObject = (APhysXRigidBodyObject *)m_arrPhysXObjList.back();
		// pLastRBObject->AddForce(pickRay, 100000);

		if(APhysXPicker::Raycast(m_pPhysXScene, pickRay, hitInfo, APX_RFTYPE_DANYMIC_RB))
		{
			APHYSX_ASSERT(hitInfo.mActor);

			NxActor* pHitActor = hitInfo.mActor;
			APhysXRigidBodyObject* pRBObject = (APhysXRigidBodyObject*)pHitActor->userData;

#define	ADD_FORCE_TEST
#ifdef	ADD_FORCE_TEST
			
#define FORCE_SCALE 2001


			// pHitActor->addForceAtPos(pickRay.dir * FORCE_SCALE, hitInfo.mHitPos, NX_IMPULSE);
			
			APhysXRBHitInfo rbHitInfo;
			rbHitInfo.mHitActor = pHitActor;
			rbHitInfo.mHitForce = pickRay.dir * FORCE_SCALE;
			rbHitInfo.mHitPos = hitInfo.mHitPos;
			
			pRBObject->OnHit(rbHitInfo);
#endif



// #define	OVERLAP_TEST
#ifdef	OVERLAP_TEST

			s_OverlappedActors.clear();
			GetOverlapActors(pHitActor, s_OverlappedActors, s_OverlapNormals);

			for(unsigned int i=0; i<s_OverlappedActors.size(); i++)
			{
				VisualizeNxActor(s_OverlappedActors[i], false);
			}


			// print id of the actors...

			CString strActorID;
			strActorID.Format("\n Hit the Actor with ID: %d", pRBObject->GetNxActorID(pHitActor));
			AfxOutputDebugString(strActorID);

			// render the neighbor actors of Hit Actor...
			
			s_NeighborActors.clear();

			if(pRBObject->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE))
			{

				APhysXBreakableSkeletonRBObject* pBSRBObj = (APhysXBreakableSkeletonRBObject*) pRBObject;
				// APhysXGraph* pGraph = pBSRBObj->GetActorNeighborGraph();
				APhysXGraph* pGraph = pBSRBObj->GetActorSupportGraph();
				if(pGraph)
				{
					int id = pRBObject->GetNxActorID(pHitActor);

					APhysXIDArray neighbors;
					pGraph->GetVertexNeighbors(id, neighbors);

					for(unsigned int i=0; i<neighbors.size(); i++)
					{
						s_NeighborActors.push_back(pRBObject->GetNxActor(neighbors[i]));
					}

				}

			}
			

#endif

		}


	}

	CView::OnMButtonDown(nFlags, point);
}

void CAPhysXDebugRenderTestView::OnFileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog fileDlg(true, TEXT("ecm"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT("EC Model Files (*.ecm)|*.ecm|"));

	if(IDOK == fileDlg.DoModal())
	{

#ifdef UNICODE

#else
//		LoadECModel(fileDlg.GetPathName());
#endif

	}

}

APhysXScene* gGetAPhysXScene()
{
	return gPhysXEngine->GetAPhysXScene(0);
}

void CAPhysXDebugRenderTestView::ResetScene()
{

	if(m_pPhysXScene)
	{
#if 0
		A3DRELEASE(m_pECModel);
#endif		
		m_pPhysXScene->Reset();
		m_pPhysXScene->StartSimulate();

		s_gRBAttacher = s_pRBAttacher =	NULL;

		s_pControledActor =NULL;

		

	}
}

class APhysXWFTestBreakReport: public APhysXUserBreakReport
{
public:
	virtual void OnBreakNotify(APhysXBreakable* pBreakable)
	{
		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();
		if(pBreakObj && pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE))
		{
			((APhysXSimpleBreakableSkeletonRBObject *)pBreakObj)->EnableKinematic(false);
		}
	};

	virtual void OnFirstBreakNotify(APhysXBreakable* pBreakable)
	{
		// do nothing...
	}

	virtual void OnAllBreakNotify(APhysXBreakable* pBreakable)
	{
		// do nothing...
	}


	APhysXWFTestBreakReport()
	{
		// gPhysXBreakReport->Register(this);
	}
};

static APhysXWFTestBreakReport gWFTestBreakReport;


void CAPhysXDebugRenderTestView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

#ifdef	OVERLAP_TEST

	for(unsigned int i=0; i<s_OverlappedActors.size(); i++)
	{
		VisualizeNxActor(s_OverlappedActors[i]);
	}

	s_NeighborActors.clear();
	s_OverlapNormals.clear();
	
#endif


	CView::OnMButtonUp(nFlags, point);
}

// tmp code...
/*
namespace _SGC
{

class A3DSkillGfxMan;
A3DSkillGfxMan * AfxGetSkillGfxEventMan()
{
	return NULL;
}

}

*/


void CAPhysXDebugRenderTestView::TestPhysXStateSync()
{

	float fDim = 0.5f;

#define TEST_CUBE 1
#if TEST_CUBE
	
	// APhysXRigidBodyObject* pRB = (APhysXRigidBodyObject*)CreateCube(NxVec3(0,10,0), fDim);

	APhysXRigidBodyObject* pRB = (APhysXRigidBodyObject*)CreateJointedObject(NxVec3(0,10,0), fDim);

#else
	
	APhysXRigidBodyObject* pRB = (APhysXRigidBodyObject*)CreateSphere(NxVec3(0,10,0), fDim);

#endif

	APhysXCollisionChannel colChannel = m_pPhysXScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
	pRB->SetCollisionChannel(colChannel);


#if TEST_CUBE

	// APhysXRigidBodyObject* pRBProxy = (APhysXRigidBodyObject*)CreateCube(NxVec3(0,10,0), fDim);

	APhysXRigidBodyObject* pRBProxy = (APhysXRigidBodyObject*)CreateJointedObject(NxVec3(0,10,0), fDim);

#else

	APhysXRigidBodyObject* pRBProxy = (APhysXRigidBodyObject*)CreateSphere(NxVec3(0,10,0), fDim);

#endif

	colChannel = m_pPhysXScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
	pRBProxy->SetCollisionChannel(colChannel);


#if TEST_CUBE

	APhysXRigidBodyObject* pRBProxyCorrected = (APhysXRigidBodyObject*)CreateCube(NxVec3(0,10,0), fDim);

#else

	APhysXRigidBodyObject* pRBProxyCorrected = (APhysXRigidBodyObject*)CreateSphere(NxVec3(0,10,0), fDim);

#endif

	colChannel = m_pPhysXScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
	pRBProxyCorrected->SetCollisionChannel(colChannel);


	s_pAuthorRB= pRB;
	s_pProxyRB = pRBProxy;
	s_pProxyRBCorrected = pRBProxyCorrected;

	s_apxNetworkActor.Init(pRB, pRBProxy);
	s_apxNetworkActorCorrected.Init(pRB, pRBProxyCorrected, true);
	// s_apxNetworkActorCorrected.Init(pRB->GetNxActor(0), pRBProxyCorrected->GetNxActor(0));

	pRB->EnableDebugRender(false);
	pRBProxy->EnableDebugRender(false);
	pRBProxyCorrected->EnableDebugRender(false);

	/*
	// disable the rendering of these actors...
	NxActor* pActor = pRB->GetNxActor(0);
	//pActor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	NxShape* pShape = pActor->getShapes()[0];
	pShape->setFlag(NX_SF_VISUALIZATION, false);

	NxActor* pProxyActor = pRBProxy->GetNxActor(0);
	//pProxyActor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	pShape = pProxyActor->getShapes()[0];
	pShape->setFlag(NX_SF_VISUALIZATION, false);

	NxActor* pProxyActorCorrected = pRBProxyCorrected->GetNxActor(0);
	//pProxyActorCorrected->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	pShape = pProxyActorCorrected->getShapes()[0];
	pShape->setFlag(NX_SF_VISUALIZATION, false);
	*/

}

void CAPhysXDebugRenderTestView::TestRotation()
{
	NxBoxShapeDesc box;
	box.dimensions.set(1.0f, 0.1f, 0.5f);
	
	NxBodyDesc body;
	body.angularDamping = 0.0f;
	body.flags |= NX_BF_DISABLE_GRAVITY;
	
	NxActorDesc actorFrom;
	actorFrom.flags |= NX_AF_DISABLE_COLLISION;
	actorFrom.density = 1.0f;
	actorFrom.body = &body;
	actorFrom.shapes.push_back(&box);
	actorFrom.globalPose.t.set(0, 5, 0);
	actorFrom.globalPose.M.rotX(APHYSX_PI * 0.2f);
	

	NxScene* pScene = m_pPhysXScene->GetNxScene();
	NxActor* pActorFrom = pScene->createActor(actorFrom);

	NxActorDesc actorTo = actorFrom;
	actorTo.globalPose.M.rotZ(APHYSX_PI * 0.2f);

	NxActor* pActorTo = pScene->createActor(actorTo);

	
	// now try to find the rotation to bwtween "From" and "To"

	NxQuat qFrom = pActorFrom->getGlobalOrientationQuat();
	NxQuat qTo = pActorTo->getGlobalOrientationQuat();
	qFrom.invert();

	NxQuat qRotate;
	qRotate.multiply(qTo, qFrom);
	qRotate.normalize();

	// of cause, this works well
	// pActorFrom->setGlobalOrientationQuat( qRotate * pActorFrom->getGlobalOrientationQuat());
	
	float fAngel;
	NxVec3 vAxis;
	// note here, fAngel is not in the unit of radians but in degrees...
	qRotate.getAngleAxis(fAngel, vAxis);
	fAngel = 0.2f * NxMath::degToRad(fAngel);
	pActorFrom->setAngularVelocity(fAngel * vAxis);


}