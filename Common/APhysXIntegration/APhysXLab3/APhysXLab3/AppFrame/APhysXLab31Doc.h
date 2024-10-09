// APhysXLab31Doc.h : interface of the CAPhysXLab31Doc class
//


#pragma once


class CAPhysXLab31Doc : public CDocument
{
protected: // create from serialization only
	CAPhysXLab31Doc();
	DECLARE_DYNCREATE(CAPhysXLab31Doc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CAPhysXLab31Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


