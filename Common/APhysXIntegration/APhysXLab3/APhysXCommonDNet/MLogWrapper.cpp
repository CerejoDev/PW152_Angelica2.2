#include "Stdafx.h"
#include "MLogWrapper.h"

using namespace System;


namespace APhysXCommonDNet
{
	static void _LogOutput(const char* szMsg)
	{
		if (MLogWrapper::Instance == nullptr)
			return;

		assert(true == MLogWrapper::Instance->IsInited());
		MLogWrapper::Instance->GetALogPtr()->Log(szMsg);
		if (nullptr != MLogWrapper::Instance->GetOutputWnd())
			MLogWrapper::Instance->GetOutputWnd()->OutputText(szMsg);
	}

	MLogWrapper::MLogWrapper()
	{
		m_bIsInited = false;
		m_pOPWnd = nullptr;
		m_pLog = new ALog();
	}
	
	void MLogWrapper::Release()
	{
		a_RedirectDefLogOutput(NULL);

		m_pLog->Release();
		af_Finalize();
		m_bIsInited = false;
	}

	void MLogWrapper::LogText(System::String^ str)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(str);
		AString astr = WC2AS(wch);
		_LogOutput(astr);
	}

	bool MLogWrapper::Init(System::String^ logFileBasicName, MIOutputWnd^ pOP)
	{
		if (m_bIsInited)
			Release();

		char szCurPath[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szCurPath);

		if (!af_Initialize())
			return false;
		af_SetBaseDir(szCurPath);

		pin_ptr<const wchar_t> wch = PtrToStringChars(logFileBasicName);
		AString strfile = WC2AS(wch);
		AString fn;
		fn.Format("%s.log", strfile);
		AString msg;
		msg.Format("======%s Log======", logFileBasicName);
		if (!m_pLog->Init(fn, msg))
		{
			af_Finalize();
			return false;
		}

		a_RedirectDefLogOutput(_LogOutput);
		SetOutputWnd(pOP);
		m_bIsInited = true;
		return true;
	}
}