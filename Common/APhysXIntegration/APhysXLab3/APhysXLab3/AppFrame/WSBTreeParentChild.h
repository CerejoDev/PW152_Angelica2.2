// WSBTreeParentChild.h : interface of the CWSBTreeParentChild.h class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WorkSpaceBar.h"

class CWSBTreeParentChild : public CWorkSpaceBar, public IDragDropEvent, public ICloneReport, public ApxObjOperationReport
{
public:
	CWSBTreeParentChild();

	virtual void OnNotifyDie(const IObjBase& obj);
	virtual bool DragDropBegin(HTREEITEM htiDragItem);
	virtual bool DragDropMoving(HTREEITEM htiDragItem, HTREEITEM htiDropTarget);
	virtual void DragDropEnd(HTREEITEM htiDragItem, HTREEITEM htiDropTarget);
	virtual void OnCloneNotify(unsigned int nbEntities, CPair* entities);
	virtual void OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild);
	virtual void OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild);

// Attributes
protected:

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWSBTreeParentChild)
	//}}AFX_VIRTUAL

// Implementation

private:
	virtual void SetupTreeContent();
	virtual HTREEITEM GetParentItem(const IObjBase& obj);

	void AddItems(const IObjBase& obj);
	IObjBase* GetObject(HTREEITEM hti);

private:
	bool m_bEnableApxOptEvent;
	CImageList	  m_imgList;
};

/////////////////////////////////////////////////////////////////////////////
