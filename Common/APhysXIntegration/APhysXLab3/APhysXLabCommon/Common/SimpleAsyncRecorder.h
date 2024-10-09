/*
* FILE: SimpleAsyncRecorder.h
*
* DESCRIPTION: 
*		This class is used to record-replay asynchronous keyboard state. Any key down state will be saved.
*
* Limitation:
*		Considering an extrame situation: in a frame, query the same key more than once and get different result.
*		Foe example, query 'A' state 3 times sequential in the same frame, and we get
*			times     result
*			  1		not down
*			  2		 down
*			  3		not down
*		So, 'A' key down state will be recorded and saved. Obviously, we get 3 times 'A' keydown state in replay mode.
*		That is different from recording mode.
*		For above case, we recommend log the information by CRecording.LogItemData() method.
*
* CREATED BY: Yang Liu, 2012/02/15
*
* HISTORY: 
*
* Copyright (c) 2012 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSXLAB_SASYNCREC_H_
#define _APHYSXLAB_SASYNCREC_H_

class SimpleAsyncRec
{
public:
	enum SARFlag
	{
		SAR_QUERY_ONLY   = 0,
		SAR_QUERY_SAVE   = 1,
		SAR_CACHE_REPLAY = 2,
	};

public:
	SimpleAsyncRec();
	APhysXU32 GetVersion() const { return 0xAB000001; }

	bool LoadData(NxStream& stream, bool& outIsLowVersion);
	void SaveData(NxStream& stream);

	void Clear();
	int IsKeyDown(int vKey, SARFlag flag);

private:
	static const int gKeyCount = 256;
	static const int gUnitSize = sizeof(int) * 8;  // total 32bit
	static const int gKeyZipSize = gKeyCount / gUnitSize;

	int m_Key[gKeyCount];
	int m_KeyZip[gKeyZipSize];
};

#endif