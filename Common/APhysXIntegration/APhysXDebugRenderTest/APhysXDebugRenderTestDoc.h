// APhysXDebugRenderTestDoc.h : interface of the CAPhysXDebugRenderTestDoc class
//


#pragma once


class CAPhysXDebugRenderTestDoc : public CDocument
{
protected: // create from serialization only
	CAPhysXDebugRenderTestDoc();
	DECLARE_DYNCREATE(CAPhysXDebugRenderTestDoc)

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
	virtual ~CAPhysXDebugRenderTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


