#pragma once


// CPreviewWnd

class CPreviewWnd : public CWnd
{
	DECLARE_DYNAMIC(CPreviewWnd)

public:
	CPreviewWnd();
	virtual ~CPreviewWnd();

public:

	DWORD m_dwSize;
	DWORD m_dwBuffer[256 * 256];

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


