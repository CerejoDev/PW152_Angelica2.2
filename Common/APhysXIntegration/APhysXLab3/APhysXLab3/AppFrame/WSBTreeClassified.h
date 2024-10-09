// WSBTreeClassified.h : interface of the CWSBTreeClassified.h class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WorkSpaceBar.h"

class CWSBTreeClassified : public CWorkSpaceBar
{
// Attributes
public:
	virtual void OnNotifyDie(const IObjBase& obj);

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWSBTreeParentChild)
	//}}AFX_VIRTUAL

// Implementation

private:
	virtual void SetupTreeContent();
	virtual HTREEITEM GetParentItem(const IObjBase& obj);

	void CreateOTTN(const ObjManager::ObjTypeID& nID);
	HTREEITEM GetOTTN(const ObjManager::ObjTypeID& nID) const;

private:
	struct ObjTypeTreeNode 
	{
		ObjTypeTreeNode(const ObjManager::ObjTypeID& nID, const HTREEITEM& nHTI)
		{
			tid = nID;
			hti = nHTI;
		}

		ObjManager::ObjTypeID tid;
		HTREEITEM hti;
	};

	typedef		std::vector<ObjTypeTreeNode>	OTTN;

	OTTN m_ottn;
};

/////////////////////////////////////////////////////////////////////////////
