/*
* FILE: SimpleAsyncRecorder.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2012/02/15
*
* HISTORY: 
*
* Copyright (c) 2012 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"

SimpleAsyncRec::SimpleAsyncRec()
{
	Clear();
}

bool SimpleAsyncRec::LoadData(NxStream& stream, bool& outIsLowVersion)
{
	unsigned int nVersion = stream.readDword();
	if(!CheckVersionPrefix(nVersion))
		return false;
	if(nVersion < GetVersion())
		outIsLowVersion = true;

	int t, m, idx;
	int indicate = stream.readDword();
	for (int i = 0; i < gKeyZipSize; ++i)
	{
		t = 1 << i;
		m_KeyZip[i] = 0;
		if (t & indicate)
		{
			m_KeyZip[i] = stream.readDword();
			for (int j = 0; j < gUnitSize; ++j)
			{
				m = 1 << j;
				if (m & m_KeyZip[i])
				{
					idx = gUnitSize * i + j;
					m_Key[idx] = 1;
				}
			}
		}
	}
	return true;
}

void SimpleAsyncRec::SaveData(NxStream& stream)
{
	for (int i = 0; i < gKeyZipSize; ++i)
		m_KeyZip[i] = 0;

	int indicate = 0;
	int quo, rem, t;
	int* pGroup = 0; 
	for (int i = 0; i < gKeyCount; ++i)
	{
		if (0 < m_Key[i])
		{
			quo = i / gUnitSize;
			rem = i - (quo * gUnitSize);
			t = 1 << rem;

			pGroup = &(m_KeyZip[quo]);
			*pGroup |= t;
			indicate |= (1 << quo);
		}
	}

	stream.storeDword(GetVersion());
	stream.storeDword(indicate);
	for (int i = 0; i < gKeyZipSize; ++i)
	{
		if (0 != m_KeyZip[i])
			stream.storeDword(m_KeyZip[i]);
	}
}

void SimpleAsyncRec::Clear()
{
	for (int i = 0; i < gKeyCount; ++i)
		m_Key[i] = -1;
}

int SimpleAsyncRec::IsKeyDown(int vKey, SARFlag flag)
{
	int idx = vKey - 1;
	if (0 > idx)
		return 0;
	if (gKeyCount <= idx)
		return 0;

	if (SAR_CACHE_REPLAY == flag)
	{
		if (0 < m_Key[idx])
			return 1;
		return 0;
	}

	int nRes = GetAsyncKeyState(vKey) & 0x8000;
	if (SAR_QUERY_ONLY == flag)
		return nRes;

	if (-1 != m_Key[idx])
		return -1;
	m_Key[idx] = nRes;
	return m_Key[idx];
}
