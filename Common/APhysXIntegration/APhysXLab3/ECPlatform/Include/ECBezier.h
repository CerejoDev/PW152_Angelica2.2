/*
 * FILE: ECBezier.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY:
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma	once

#include <ABaseDef.h>
#include <AExpDecl.h>
#include <A3DVector.h>
#include <AString.h>
#include <stdio.h>
#include "ECSceneObject.h"
#include "PolygonFunc.h"

class AFile;
class A3DWireCollector;

class _AEXP_DECL ECBezierPoint
{

public:
	
	ECBezierPoint(){};
	virtual ~ECBezierPoint(){};
	
public:
	
	const A3DVECTOR3& GetPos() const { return m_vPos; };
	const A3DVECTOR3& GetDir() const { return m_vDir; };
	void SetPos( const A3DVECTOR3& pos)	{ m_vPos = pos; };
	void SetDir( const A3DVECTOR3& dir) { m_vDir = dir; };
	void SetSpeed(float speed) { m_fSpeed = speed;}
	void SetFov(float fov) { m_fFov = fov;}
	void SetViewRadius(float viewRadius){ m_fViewRadius = viewRadius;}

protected:	

	A3DVECTOR3 m_vPos;//摄像机位置
	A3DVECTOR3 m_vDir;//摄像机方向
	float	   m_fSpeed;//摄像机移动速度
	float	   m_fFov; // 摄像机 Fov
	float	   m_fViewRadius;//视野距离

private:

};

class _AEXP_DECL ECBezierSeg
{
public:
	
	ECBezierSeg();
	virtual ~ECBezierSeg();

public:

	inline A3DVECTOR3	GetAnchorHead() const { return m_vAnchorHead; };
	inline A3DVECTOR3	GetAnchorTail() const { return m_vAnchorTail; };

	inline float		GetSegLength() const { return m_fLenght; }

	inline void		SetAnchorHead( const A3DVECTOR3& v ) { m_vAnchorHead = v; };
	inline void		SetAnchorTail( const A3DVECTOR3& v ) { m_vAnchorTail = v; };

	inline void		SetSegLenght( float lenght) { m_fLenght = lenght; }	

	inline void		SetKnot1(const A3DVECTOR3& v) { m_vKnot1 = v;}
	inline void		SetKnot2(const A3DVECTOR3& v) { m_vKnot2 = v;}

	inline void		SetBezierPoint1(const ECBezierPoint& p){ m_bezierPoint1 = p; }
	inline void		SetBezierPoint2(const ECBezierPoint& p){ m_bezierPoint2 = p; }
	
	inline ECBezierPoint GetBezierPoint1(){ return m_bezierPoint1; }
	inline ECBezierPoint GetBezierPoint2(){ return m_bezierPoint2; }

	A3DVECTOR3	Bezier( float u ,bool bForward);
	A3DVECTOR3	Vector( float u ,bool bForward);

protected:
	
	A3DVECTOR3	m_vAnchorHead;//控制点1
	A3DVECTOR3	m_vAnchorTail;//控制点2       
	float       m_fLenght;// 曲线段长度
	
private:
	A3DVECTOR3	m_vKnot1;
	A3DVECTOR3	m_vKnot2;

	ECBezierPoint m_bezierPoint1;
	ECBezierPoint m_bezierPoint2;
};

class _AEXP_DECL ECBezier : public ECSceneObject
{
public:
	ECBezier();
	virtual ~ECBezier();

public:

	bool			Load(const void* pData, int iDataLen, DWORD dwVersion);

	void            SetOffset(A3DVECTOR3 vOffset);
//	void			SetObjectID( DWORD id){  m_nObjectID = id; };
	void            Release();

//	DWORD			GetObjectID() const { return m_nObjectID; };
	int				GetSegmentNum() { return m_nNumSeg; }
	ECBezierSeg*	GetSegment(int n) const { ASSERT(n >= 0 && n < m_nNumSeg); return &m_pListSeg[n];}
	int				GetPointNum() { return m_nNumPoint; }
	ECBezierPoint*	GetPoint(int n) const { ASSERT(n>=0 && n< m_nNumPoint); return &m_pListPoint[n];}

	bool			IsValid() { return GetPointNum() >= 3;};

protected:

//	int             m_nObjectID;
	int				m_nNumPoint;
	int				m_nNumSeg;
	
	ECBezierPoint*	m_pListPoint;
	ECBezierSeg*	m_pListSeg;
};



///////////////////////////////////////////////////////////////////////////
//	
//	Class ECBezierWalker
//	
///////////////////////////////////////////////////////////////////////////

class ECBezierWalker
{
public:		//	Types

public:		//	Constructor and Destructor

	ECBezierWalker();
	virtual ~ECBezierWalker();

public:		//	Attributes

public:		//	Operations

	//	Bind bezier route
	bool BindBezier(ECBezier* pBezier);
	//	Start walk
	bool StartWalk(bool bLoop, bool bForward);
	//	Pause walk
	void Pause(bool bPause);

	//	Tick routine
	bool Tick(int iDeltaTime);

	//	Get current position
	A3DVECTOR3 GetPos();
	//	Get current direction
	A3DVECTOR3 GetDir();

	//	Forward flag
	void SetForwardFlag(bool bForward);
	bool GetForwardFlag() { return m_bForward; }
	//	Move speed
	void SetSpeed(float fSpeed);
	float GetSpeed() { return m_fSpeed; }
	//	Get total time
	int GetTotalTime() { return m_iTotalTime; }
	//	Loop flag
	void SetLoopFlag(bool bLoop) { m_bLoop = bLoop; }
	bool GetLoopFlag() { return m_bLoop; }
	//	Get walking flag
	bool IsWalking() { return m_bWalking; }
	//	Get pause flag
	bool IsPause() { return m_bPause; }

protected:	//	Attributes

	ECBezier*	m_pBezier;		//	Bezier route data
	bool		m_bForward;		//	Forward flag
	float		m_fSpeed;		//	Moving speed
	int			m_iTotalTime;	//	Total time of whole bezier route
	int			m_iTimeCnt;		//	Time counter
	int			m_iCurSeg;		//	Current segment
	int			m_iCurSegTime;	//	Total time of current segment
	int			m_iPassSegTime;	//	Total time of passed segments
	bool		m_bLoop;		//	Loop flag
	bool		m_bWalking;		//	true, is walking
	bool		m_bPause;		//	Pause flag
	bool		m_bForwardStop;	//	Stop flag
protected:	//	Operations

};


//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECBezierUtil
{
public:
	ECBezierUtil();
	ECBezierUtil(ECBezier* pBezier);
	~ECBezierUtil();

	//数据
public:
private:
	ECBezier*		m_pBezier;
	POINT_FLOAT*	m_pPtLists;
	int				m_nPtNums;

	//操作
public:
	bool			IsInBezier(const POINT_FLOAT vEye);
	float			GetDisToBezier(const POINT_FLOAT vEye);
	bool			Reset(ECBezier* pBezier);
	void			Release();

	void			drawArea(A3DWireCollector* pWireCollector,float fTop,float fBottom,DWORD clr);

protected:
	float			dis(const POINT_FLOAT& p,const POINT_FLOAT& q);
	float			dot(const POINT_FLOAT& p,const POINT_FLOAT& q,const POINT_FLOAT& o);
	float			cross(const POINT_FLOAT& p,const POINT_FLOAT& q,const POINT_FLOAT& o);
	float			fix(const POINT_FLOAT& o,const POINT_FLOAT& p,const POINT_FLOAT& q);
};
