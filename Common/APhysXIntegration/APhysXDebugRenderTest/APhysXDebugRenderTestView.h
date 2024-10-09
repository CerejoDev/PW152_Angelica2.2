// APhysXDebugRenderTestView.h : interface of the CAPhysXDebugRenderTestView class
//


#pragma once


class A3DIBLScene;
class A3DSky;
class A3DSkinModel;
class A3DModel;
class A3DKDTree;
class A3DVECTOR3;
class AMSoundStream;

class CECModel;

class APhysXScene;
class NxVec3;
class APhysXConvexShapeDesc;

// #include "APhysX.h"

class CAPhysXDebugRenderTestView : public CView
{

protected: // create from serialization only
	CAPhysXDebugRenderTestView();
	DECLARE_DYNCREATE(CAPhysXDebugRenderTestView)

	friend DWORD WINAPI ThreadCreationFunc(void * arg);

// Attributes
public:
	CAPhysXDebugRenderTestDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);



// Implementation
public:
	virtual ~CAPhysXDebugRenderTestView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//user defined

// Operations
public:

	//	Initlaize game
	bool InitGame();

	//	Frame move
	bool Tick(DWORD dwTickTime);
	//	Render 
	bool Render(DWORD dwTickTime);

	void PauseSimulation();

protected:	//	Attributes

	//A3DIBLScene*	m_pA3DIBLScene;		//	IBL scene object
	//A3DSkinModel*	m_pSkinModel;		//	Skin model
	//A3DModel*		m_pModel;			//	Model
	//A3DKDTree*		m_pA3DKDTree;		//	KDT object
	//CECModel* m_pECModel;

	A3DSky*			m_pA3DSky;			//	Sky object
	AMSoundStream*	m_pBackMusic;		//	Sound stream object

	APhysXScene* m_pPhysXScene;

	bool m_bPauseSimulation;

protected:	//	Operations

	//	Release game
	void ReleaseGame();

	//	Move camera
	void MoveCamera(DWORD dwTime);

	//	Rotate camera
	void RotateCamera(DWORD dwTime);

/*
	//	Load IBL scene
	bool LoadIBLScene(const char* szSceneFile, const char* szKDTFile);
	//	Load sky
	bool LoadSky(int iSkyType);
	//	Load skin model
	bool LoadSkinModel(const char* szFile);
	//	Load model
	bool LoadModel(const char* szFile);
	//	Load music
	bool LoadMusic(const char* szFile);
	//	Play a GFX
	bool PlayGFX(const char* szFile, const A3DVECTOR3& vPos, float fScale);

	//	Load EC model
	bool LoadECModel(const char* szFile, const NxVec3& vPos = NxVec3(0.0f, 0.0f, 0.0f));

*/
	//	Draw prompt
	bool DrawPrompt();


//---------------------------------------------------------------
// Physics operation
protected:
	void InitPhysXEngine();
	void ReleasePhysXEngine();
	APhysXObject* CreateCube(const NxVec3& pos, float size=2, const NxVec3* initialVelocity=NULL, const char* szName = NULL, bool bPushToObjList = true);
	APhysXObject* CreateSphere(const NxVec3& pos, float size, const NxVec3* initialVelocity = NULL, const char* szName = NULL, bool bPushToObjList = true);
	APhysXObject* CreateCapsule(const NxVec3& pos, float height, float radius, const NxVec3* initialVelocity = NULL, const char* szName = NULL, bool bPushToObjList = true);


	void CreateStack(int size);
	void CreateTower(int size);
	void CreateCubeFromEye(float size = 3.0f);

	void GenerateCovexMeshShapeDesc(const char* szName);
	void CreateConvex(int i, const NxVec3& pos, const NxVec3* initialVelocity = NULL, const NxVec3& vScale3D = NxVec3(1.0f));
	void CreateConvex(const NxVec3& pos, const NxVec3* initialVelocity, const NxVec3& vScale3D, const char* szFileName, const char* szObjName = NULL);
	void PhysXControl();
	void GenerateCovexRBObjectDesc();

	enum 
	{
		JOINT_TYPE_DISTANCE = 0,

		JOINT_TYPE_D6,

		JOINT_TYPE_SPHEREICAL,

		JOINT_TYPE_REVOLUTE,

		//-----------------------------
		JOINT_TYPE_INVALID
	};

	APhysXObject* CreateJointedObject(const NxVec3& vPos, float fDim, APhysXU32 uJointType = JOINT_TYPE_SPHEREICAL);

	void CreateJointedObjectFromEye();
	void SaveJointedObjectDescs();
	void LoadJointedObjectFromEye();

	void LoadSkeletonObject(const char* szSkeletonObjectFile, const NxVec3& vPos);
	void LoadSkeletonObjectFromEye(const char* szSkeletonObjectFile);

	void LoadPhysXObjectDesc(const char* szFileName);
	void SavePhysXObjectDesc(const char* szFileName);


	void CreateClothObject(const NxVec3& vPos);
	void CreateClothObjectFromEye();

	void CreateFluidEmitter();
	void CreateForceField();

	void FetchPhysXProfileData();

	void PushToObjList(APhysXObject* pObj);
	void RemoveObjListBack();

	void ResetScene();

	APhysXObject* OpenPXD(const char* szPxdName, const NxVec3& vPos = NxVec3(0.0f, 10.0f, 0.0f));

	void TestPhysXStateSync();

	void TestRotation();

// Physics attributes...
protected:

	CPoint m_msLastPos;

	APhysXArray<APhysXConvexShapeDesc*> m_arrConvexShapeDescs;
	
	//APhysXArray<APhysXObjectDesc* > m_arrPhysXObjectDescs;

	APhysXObjectDescManager m_apxObjectDescManager;

	// APhysXPicker m_apxPicker;

	APhysXFluidObject* m_pFluidObject;
	APhysXForceFieldObject* m_pFFObject;

	APhysXArray<APhysXObject*> m_arrPhysXObjList;
	APhysXMutex m_MutexPhysXObjList;

//---------------------------------------------------------------

protected:

	void TestAIniFile();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in APhysXDebugRenderTestView.cpp
inline CAPhysXDebugRenderTestDoc* CAPhysXDebugRenderTestView::GetDocument() const
   { return reinterpret_cast<CAPhysXDebugRenderTestDoc*>(m_pDocument); }
#endif

