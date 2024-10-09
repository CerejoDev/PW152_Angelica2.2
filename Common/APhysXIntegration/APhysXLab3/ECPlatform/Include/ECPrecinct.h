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
	//	Music loop type ����֮���ѭ����ʽ��
	enum
	{
		LOOP_NONE = 0, //0 ��ѭ����
		LOOP_WHOLE, //��ͷѭ����
		LOOP_SKIPFIRST,//�ӵڶ��׿�ʼѭ��
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

	ACString	m_strAlias; // ����

//	unsigned int m_uiIdBezier;  // �����ı��������ߵ�ID
	int m_iPriority;			// ����˳��
	int m_iSelfCity;			// �Լ��ĸ�������ID
	int m_iCity;				// �سǵ��Ӧ�ĸ���ID��Ĭ�������,��ʾ�ص���Ҫ��������(world)
	A3DVECTOR3 m_vCityPos;		// �سǵ�

	// �����ţ�������������������ͬ����ô������������Χ�ɵ��������
	// ��Ϊ����һ������,���dwPrecinctID==0��ô����ʾ������ֻ���ڿͻ��ˣ������������
	DWORD m_dwPrecinctID;		

	int m_iAlternation;		// ���׸�֮���ʱ����
	int m_iLoopType;		// ����֮���ѭ����ʽ��0 ��ѭ������ͷѭ�����ӵڶ��׿�ʼѭ��

	AString		m_strDaySoundPath;
	AString		m_strNightSoundPath;

	APtrArray<NPCMARKINFO*>	m_aNPCMarks;

	APtrArray<AString*>	m_aBGMusicFiles; //��������

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