#pragma once

//state of keyboard, mouse ...
namespace APhysXCommonDNet
{
	public ref class MIOutputWnd abstract
	{
	public:
		virtual void OutputText(const char* szMsg) = 0;
	};
	public ref class MLogWrapper
	{
	public:
		static property MLogWrapper^ Instance
		{
			MLogWrapper^ get()
			{
				if (_instance == nullptr)
					_instance = gcnew MLogWrapper();
				return _instance;
			}
		}
		!MLogWrapper()
		{
			 if (m_bIsInited) Release();
			 delete m_pLog;
		}
		~MLogWrapper() { this->!MLogWrapper(); }

		void SetOutputWnd(MIOutputWnd^ pOP) { m_pOPWnd = pOP; }

		MIOutputWnd^ GetOutputWnd() { return m_pOPWnd; }

		void LogText(System::String^ str);

		bool Init(System::String^ logFileBasicName, MIOutputWnd^ pOP);

		bool Init(System::String^ logFileBasicName) { return Init(logFileBasicName, nullptr); }

		bool IsInited() { return m_bIsInited; }

		void Release();

	internal:
		ALog* GetALogPtr() { return m_pLog; }
	
	private:
		MLogWrapper();
		
		

		bool m_bIsInited;
		ALog* m_pLog;
		MIOutputWnd^ m_pOPWnd;
		static MLogWrapper^ _instance = nullptr;
	};

}