/*
* FILE: ECSceneObject.h
*
* DESCRIPTION: 
*
* CREATED BY: Yongdong Wang, 2011/07/11
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/



#ifndef _ECAUDIOOBJECT_H_
#define _ECAUDIOOBJECT_H_

#include "ECSceneObject.h"

class ECSceneModule;
class A3DEngine;
class A3DWireCollector;
class ECBezierUtil;

namespace AudioEngine
{
	class EventInstance;
	class Reverb;
};


class _AEXP_DECL ECScnAudio: public ECSceneObject
{
public:
	enum 
	{
		PLAY_ALL,		//白天晚上都播放
		PLAY_DAY,		//仅白天播放
		PLAY_NIGHT		//仅晚上播放
	};

public:
	ECScnAudio(ECSceneModule* pSceneM);
	virtual ~ECScnAudio();

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);

	const char* GetEventFile() { return m_strEventPath;}	

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	const A3DVECTOR3& GetDir() const { return m_vDir;}
	float GetMinDist() const { return m_fMinDist;}
	float GetMaxDist() const { return m_fMaxDist;}
	int	  GetPlayType() const  { return m_ePlayType;}
	float GetInnerAngle() const {return m_fAngle_inner;}
	float GetOuterAngle() const { return m_fAngle_outer;}
	float GetOuterVolume() const { return m_fVolume_outer;}
	float GetNormalVolume() const { return m_fVolume_normal;}

public:
	
	bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);	
	bool IsInAudio() const { return m_bIsInAudio;}
	bool PtInAudio(const A3DVECTOR3& vPoint);
	void OnEnter();
	void OnLeave();
	bool Render(A3DViewport* pViewport);

protected:
	void drawInnerOuter(A3DWireCollector* pWireCollector, DWORD clrInner, DWORD clrOuter);
	void drawTaper(A3DWireCollector* pWireCollector, float fAngle_Rad, DWORD clr,int nLatNum = 4,int nLonAngle = 15);
	void drawRange(A3DWireCollector* pWireCollector,float fMax,float fMin);
	void drawCircle(A3DWireCollector* pWireCollector,float fRad,DWORD clr,int nXYZ = 0);	

	bool Play();
	void Stop();
	void BuildEventInst();

private:

	A3DVECTOR3	m_vPos;					//位置
	A3DVECTOR3	m_vDir;					//锥的朝向
	float		m_fMinDist;				//最小距离
	float		m_fMaxDist;				//最大距离
	AString     m_strEventPath;			//Event路径字符串
	int			m_ePlayType;			//播放类型枚举(0=白天晚上都播放;1=仅白天播放;2=仅晚上播放)
	float		m_fAngle_inner;			//内锥角度(0-360)
	float		m_fAngle_outer;			//外锥角度(0-360)
	float		m_fVolume_outer;		//锥外音量(0.0-1.0)
	float		m_fVolume_normal;		//正常音量(0.0-1.0)

	bool		m_bDistIsUse;//是否使用场景编辑器中最小最大距离,否则使用Event默认的设置;=true,加载时设置以上fMinDist,fMaxDist;=false,不设置
	bool		m_bAngleIsUse;//是否使用场景编辑器中内外锥角度,否则使用Event默认的设置;=true,加载时设置以上fAngle_inner,fAngle_outer;=false,不设置

private:
	AudioEngine::EventInstance* m_pEventInst;
	ECSceneModule*				m_pSceneModule;
	A3DEngine*					m_pA3DEngine;
	bool						m_bIsInAudio;
};

//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnReverb: public ECSceneObject
{
public:
	enum 
	{
		REVERB_CUBE			= 0,
		REVERB_SPHERE		= 1
	};

public:

	ECScnReverb();
	virtual ~ECScnReverb();

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);

	const char* GetEventFile() { return m_strReverbPath;}	

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	float GetMinDist() const { return m_fMinDist;}
	float GetMaxDist() const { return m_fMaxDist;}
	int	  GetReverbType() const  { return m_nReverbType;}

	const A3DVECTOR3& GetBoxExt() const {return m_vExt;}
	float GetSphereRadius() const { return m_fRadius;}

public:
	bool Render(A3DViewport* pViewport);

protected:
	void BuildReverb();	
	void drawCircle(A3DWireCollector* pWireCollector, DWORD clr);

private:

	A3DVECTOR3		m_vPos;				//位置
	float			m_fMinDist;			//最小距离
	float			m_fMaxDist;			//最大距离
	int				m_nReverbType;		//Reverber类型			0: 立方体; 1: 球; 
	A3DVECTOR3		m_vExt;				//立方体时存储长宽高    nReverbType = 0时立方体Ext
	float			m_fRadius;			//球半径				nReverbType = 1时球半径

	AString			m_strReverbPath;			//Event路径字符串

private:
	AudioEngine::Reverb*		m_pCurReverb;
};
//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnVolume: public ECSceneObject
{
public:
	ECScnVolume(ECSceneModule* pSceneModule);
	virtual ~ECScnVolume();

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	int	  GetBezierID() const { return m_nBezierID;}
	float GetMusicVolume() const { return m_fVolume_Music;}
	float GetAudioVolume() const { return m_fVolume_Audio;}
	float GetAttenuTime() const  { return m_fAttenu_Time;}
	float GetTop() const { return m_fHeightTop;}
	float GetBottom() const { return m_fHeightBottom;}

public:
	bool PtInVolume(const A3DVECTOR3& vPoint);
	bool Render(A3DViewport* pViewport);		
	bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);

protected:
	void drawArea(A3DWireCollector* pWireCollector, DWORD clr);

private:

	A3DVECTOR3		m_vPos;				//位置
	unsigned int	m_nBezierID;		//Bezier曲线ID
	float			m_fVolume_Music;	//音乐音量(0-100)
	float			m_fVolume_Audio;	//音效音量(0-100)
	float			m_fAttenu_Time;		//衰减时间(ms)
	float			m_fHeightTop;		//上部高度
	float			m_fHeightBottom;	//下部高度

private:
	ECSceneModule* m_pSceneModule;
	ECBezierUtil*  m_pBezierUtil;
};
//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnMusicArea: public ECSceneObject
{
public:
	ECScnMusicArea(ECSceneModule* pSceneM);
	virtual ~ECScnMusicArea();

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	int	  GetBezierID() const { return m_nBezierID;}

	int GetPriority() const { return m_iPriority;}
	float GetTop() const { return m_fHeightTop;}
	float GetBottom() const { return m_fHeightBottom;}

	const AString& GetDayMusic() const { return m_strDayMusic;}
	const AString& GetNightMusic() const { return m_strNightMusic;}
	const AString& GetDayAudio() const { return m_strDayAudio;}
	const AString& GetNightAudio() const { return m_strNightAudio;}

public:	
	
	bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);
	void Update();
	
	bool IsPlayingEqualTo(ECScnMusicArea* pObj);
	bool PtInMusicArea(const A3DVECTOR3& vPoint);
	void OnEnter();
	void OnLeave();
	bool IsInMusicArea()	{ return m_bIsInMusicArea; }
	void SetLastAttenuTime(float t) { m_fLastAttenuTime = t;}
	bool Render(A3DViewport* pViewport);	

	void TickInVolume(float& fVolume_Music,float& fVolume_Audio,unsigned int uTimeDelta);
	void TickOutVolume(float& fVolume_Music,float& fVolume_Audio,unsigned int uTimeDelta);

protected:
	void BuildEventInst(AudioEngine::EventInstance*& pInst,const AString& eStr);
	void Stop();
	void Play();
	void drawArea(A3DWireCollector* pWireCollector, DWORD clr);

private:

	A3DVECTOR3		m_vPos;				//位置
	unsigned int	m_nBezierID;		//Bezier曲线ID

	AString			m_strDayMusic;		//白天音乐事件名称(先读长度iLength,再读iLength个char)
	AString			m_strNightMusic;	//晚上音乐事件名称(先读长度iLength,再读iLength个char)
	AString			m_strDayAudio;		//白天音效事件名称(先读长度iLength,再读iLength个char)
	AString			m_strNightAudio;	//晚上音效事件名称(先读长度iLength,再读iLength个char)

	float			m_fHeightTop;		//上部高度
	float			m_fHeightBottom;	//下部高度

	int				m_iPriority;		//优先顺序(越大优先级越高)

private:
	AudioEngine::EventInstance*	m_pDayMusicInst;	
	AudioEngine::EventInstance*	m_pNightMusicInst;
	AudioEngine::EventInstance*	m_pDayAudioInst;
	AudioEngine::EventInstance*	m_pNightAudioInst;

	ECSceneModule* m_pSceneModule;
	ECBezierUtil*  m_pBezierUtil;

	bool						m_bIsPlayingDay;
	bool						m_bIsPlayingNight;
	bool						m_bIsInMusicArea;

	// used for fade volume
	bool						m_bIsAffectByVolume;
	float						m_fVolume_Music;
	float						m_fVolume_Audio;

	float						m_fLastVolume_Music;
	float						m_fLastVolume_Audio;
	unsigned int				m_nAttenuDeltaTime;
	float						m_fLastAttenuTime;
};

//////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECScnAEBox: public ECSceneObject
{
public:
	ECScnAEBox(ECSceneModule* pSceneM);
	virtual ~ECScnAEBox();

	bool Load(const void* pData, int iDataLen, DWORD dwVersion);

	const A3DVECTOR3& GetPos() const { return m_vPos;}
	int	  GetBezierID() const { return m_nBezierID;}

	float GetTop() const { return m_fHeightTop;}
	float GetBottom() const { return m_fHeightBottom;}

	float GetFadeHoriDist() const { return m_fDisFade_Hor;}
	float GetFadeVertDist() const { return m_fDisFade_Ver;}

	const AString& GetDayAudio() const { return m_strDayAudio;}
	const AString& GetNightAudio() const { return m_strNightAudio;}

public:
	bool Tick(DWORD dt, const A3DVECTOR3& vLoadCenter);
	void OnEnter();
	void OnLeave();
	bool PtInAEBox(const A3DVECTOR3& vPoint);
	void Update();
	bool IsInAEBox() const { return m_bIsInAEBox; }
	bool Render(A3DViewport* pViewport);
	

protected:
	void Play();
	void Stop();
	void BuildEventInst();
	void drawArea(A3DWireCollector* pWireCollector, DWORD clr);

private:

	A3DVECTOR3		m_vPos;				//位置
	unsigned int	m_nBezierID;		//Bezier曲线ID
	AString			m_strDayAudio;		//白天音效(先读长度iLength,再读iLength个char)
	AString			m_strNightAudio;	//晚上音效(先读长度iLength,再读iLength个char)
	float			m_fDisFade_Hor;		//水平淡出距离
	float			m_fDisFade_Ver;		//垂直淡出距离
	float			m_fHeightTop;		//上部高度
	float			m_fHeightBottom;	//下部高度

private:
	bool						m_bIsPlayingDay;
	bool						m_bIsPlayingNight;

	bool						m_bIsInAEBox;
	bool						m_bIsInFade;

	float						m_fVolume_Fade;
	float						m_fVolume_Normal;

	AudioEngine::EventInstance*	m_pDayAudioInst;
	AudioEngine::EventInstance*	m_pNightAudioInst;

	ECSceneModule*				m_pSceneModule;
	ECBezierUtil*				m_pBezierUtil;
};

#endif