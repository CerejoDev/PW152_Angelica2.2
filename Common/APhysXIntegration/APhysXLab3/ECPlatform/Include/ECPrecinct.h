/*
* FILE: ECPrecinct.h
*
* DESCRIPTION: 
*
* CREATED BY: Yongdong Wang
*
* HISTORY: 
*
* Copyright (c) 2010, All Rights Reserved.
*/


#pragma once

#include <AAssist.h>
#include <A3DVector.h>
#include <AArray.h>
#include <vector.h>
#include <AWScriptFile.h>
//////////////////////////////////////////////////////////////////////////
//
// Precinct
//
//////////////////////////////////////////////////////////////////////////

#define ELPCTFILE_VERSION		1

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

//	Data file header

struct PRECINCTFILEHEADER
{
	unsigned int	dwVersion;		//	File version
	int				iNumPrecinct;	//	Number of NPC generator
	unsigned int	dwTimeStamp;	//	Time stamp of this data file
};

#pragma pack()

class _AEXP_DECL ECPrecinct
{
public:
	struct NPCMARKINFO
	{
		ACString strName;
		A3DVECTOR3 vPos;
	};
	//	Music loop type 歌曲之间的循环方式。
	enum
	{
		LOOP_NONE = 0, //0 不循环；
		LOOP_WHOLE, //从头循环；
		LOOP_SKIPFIRST,//从第二首开始循环
	};
	struct VECTOR3
	{
		float x, y, z;
	};

public:
	ECPrecinct();
	virtual ~ECPrecinct();

	bool Load(AWScriptFile* pFile, int iVersion);

	bool IsPointIn(float x, float z);

	const ACHAR* GetName() const { return m_strAlias;}

	int GetPriority() const { return m_iPriority;}
	int GetSelfCity() const { return m_iSelfCity;}
	int GetCity() const { return m_iCity;}
	
	const A3DVECTOR3& GetCityPos() const { return m_vCityPos;}

	DWORD GetID() const { return m_dwPrecinctID;}
	int GetAlternation() const {return m_iAlternation;}
	int GetLoopType() const {return m_iLoopType;}

	const char* GetDaySoundFile() const { return m_strDaySoundPath;}
	const char* GetNightSoundFile() const { return m_strNightSoundPath;}

	int GetMarkNum() { return m_aNPCMarks.GetSize(); }
	NPCMARKINFO* GetMark(int n) { return m_aNPCMarks[n]; }

	int GetMusicFileNum() { return m_aBGMusicFiles.GetSize(); }
	const char* GetMusicFile(int n) { return *m_aBGMusicFiles[n]; }

	bool GetNightSFX() { return m_bNightSFX; }
	void SetNightSFX(bool bFlag) { m_bNightSFX = bFlag; }

protected:
	bool IsCrossLine(float x, float z, int iIndex);
	//	Build precinct bound box
	void BuildBoundBox();

private:

	ACString	m_strAlias; // 名字

//	unsigned int m_uiIdBezier;  // 关联的贝塞尔曲线的ID
	int m_iPriority;			// 优先顺序
	int m_iSelfCity;			// 自己的副本世界ID
	int m_iCity;				// 回城点对应的副本ID，默认情况是,表示回到主要副本区域(world)
	A3DVECTOR3 m_vCityPos;		// 回城点

	// 区域编号，如果两个区域的区域编号同，那么这两个区域所围成的面积我们
	// 认为它是一个区域,如果dwPrecinctID==0那么他表示该数据只用于客户端，不是真的区域
	DWORD m_dwPrecinctID;		

	int m_iAlternation;		// 两首歌之间的时间间隔
	int m_iLoopType;		// 歌曲之间的循环方式。0 不循环；从头循环；从第二首开始循环

	AString		m_strDaySoundPath;
	AString		m_strNightSoundPath;

	APtrArray<NPCMARKINFO*>	m_aNPCMarks;

	APtrArray<AString*>	m_aBGMusicFiles; //背景音乐

	abase::vector<VECTOR3>	m_aPoints;	//	Precinct points

	//////////////////////////////////////////////////////////////////////////
	float	m_fLeft;		//	Bound box of precinct
	float	m_fTop;
	float	m_fRight;
	float	m_fBottom;

	bool	m_bNightSFX;		//	flag indicates current night sfx is activated
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CELPrecinctSet
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECPrecinctSet
{
public:		//	Types

public:		//	Constructor and Destructor

	ECPrecinctSet();
	virtual ~ECPrecinctSet();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(const char* szFileName);

	//	Get the precinct specified position is in
	ECPrecinct* IsPointIn(float x, float z, int instanceID) const;

	//	Get time stamp
	unsigned int GetTimeStamp() { return m_dwTimeStamp; }
	//	Get precinct number
	int GetPrecinctNum() const { return m_aPrecincts.size(); }
	//	Get precinct
	ECPrecinct* GetPrecinct(int n) const{ return m_aPrecincts[n]; } 

protected:	//	Attributes

	unsigned int	m_dwTimeStamp;		//	Time stamp of precinct data

	abase::vector<ECPrecinct*>	m_aPrecincts;	//	Precinct array

protected:	//	Operations

};