// outputbar.h : interface of the COutputBar class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <ALogWrapper.h>

ref class LogOutputWnd : APhysXCommonDNet::MIOutputWnd
{
public:
	virtual void OutputText(const char* szMsg) override;

	static property LogOutputWnd^ Instance
	{
		LogOutputWnd^ get() 
		{
			if (_instance == nullptr)
				_instance = gcnew LogOutputWnd();
			return _instance;
		}
	}
private:
	LogOutputWnd(){}
	static LogOutputWnd^ _instance = nullptr;
};

class COutputBar : public CBCGPDockingControlBar, public IOutputWnd
{
public:
	COutputBar();

// Attributes
protected:

	CRichEditCtrl	m_wndEdit;
	CFont	m_font;

// Operations
public:
	virtual void OutputText(const char* szMsg);
	void AddText(const CString& strInfo, DWORD color);
	void Clear();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputBar();

// Generated message map functions
protected:
	//{{AFX_MSG(COutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
