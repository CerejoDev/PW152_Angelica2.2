#pragma once

//proplist implementation, haven't completed yet
namespace APhysXCommonDNet
{
	
	class PropItemImp : public PropItem 
	{
	public:
		virtual void SetValue(const _variant_t& varValue);
		virtual const _variant_t& GetValue() const;

		virtual BOOL AddSubItem(PropItem* pProp);
		virtual void RemoveSubItem(PropItem* pProp, BOOL bDel = TRUE);
		virtual int  GetSubItemsCount() const;
		virtual const _variant_t& GetSubItemValue(int nIndex) const; 

		virtual BOOL AddOption(LPCTSTR lpszOption, BOOL bInsertUnique = TRUE);
		virtual void RemoveAllOptions();
		virtual int  GetOptionCount() const;

		virtual BOOL IsEnabled() const;
		virtual void Enable(BOOL bEnable = TRUE);

		virtual void AllowEdit(BOOL bAllow = TRUE);
		virtual DWORD_PTR GetData () const;


	protected:
		APtrArray<PropItem*> m_subItem;
	};

	class PropListImp : public IPropList
	{
	public:
		virtual ~PropListImp() {}
		virtual int  AddProperty(PropItem* pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
		virtual BOOL DeleteProperty(PropItem*& pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
		virtual int  GetPropertyCount() const;

		virtual void RemoveAll();
		virtual BOOL RedrawWindow();
		virtual void AdjustLayout();

		virtual PropItem* CreatePropItem(const std_TString& strGroupName, DWORD_PTR dwData = 0,	BOOL bIsValueList = FALSE);
		virtual PropItem* CreatePropItem(const std_TString& strName, const _variant_t& varValue, 
			LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, BOOL bFilePicker = FALSE,
			LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
			LPCTSTR lpszValidChars = NULL);
		virtual void ReleasePropItem(PropItem* pItem);
	};

}