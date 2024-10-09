/*
 * FILE: Command.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/02/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_COMMAND_H_
#define _APHYSXLAB_COMMAND_H_

#include <list>
#include <vector>

class ICommandEditor;
class IEngineUtility;

class CmdContext 
{
private:
	template <class T>
	struct DeletePtr : public std::unary_function<T, void>
	{
		void operator() (T pVal) const { delete pVal; }
	};

	class ObjInfo;
	typedef		std::vector<ObjInfo>	ObjContainer;

	class ObjInfo
	{
	public:
		ObjInfo(IObjBase& obj);
		~ObjInfo();

		IObjBase& GetObj() const { return *m_pObj; }
		IPropObjBase* GetProp() const { return m_pProp; }

		bool SaveAllProps(bool bToSavePaChs, bool bToSaveProps);
		void LoadAllProps();
		void LoadParent() const;

	private:
		bool SaveMyselfProp(bool bToSavePaChs, bool bToSaveProps);
		void LoadMyselfProp();

	private:
		IObjBase* m_pObj;
		IPropObjBase* m_pProp;
		ApxObjBase* m_pParent;
		ObjContainer m_Children;
	};

	class CmdInfo
	{
	public:
		CmdInfo(const ICommandEditor* pCmd);
		~CmdInfo();

		const ICommandEditor* GetCmd() const { return m_pCmd; }
		int GetLabel() const { return m_Label; }
		void SetLabel(int nLabel) { m_Label = nLabel; }

		bool SaveTargetObjInfo(int nImpacType, const CPhysXObjSelGroup& selGroup);
		void LoadTargetObjInfo(CPhysXObjSelGroup& selGroup);

		void UndoNotifyPreBackward() const;
		void UndoNotifyPostBackward(const ICommandEditor& theUndoCmd) const;

		void RedoNotifyPreForward(const ICommandEditor& theRedoCmd) const;
		void RedoNotifyPostForward() const;

	private:
		int GetTargetObjects(const CPhysXObjSelGroup& selGroup, std::vector<IObjBase*>& outObjs) const;
		void SaveSelSequence(const CPhysXObjSelGroup& selGroup);
		bool SaveInfo(bool bToSavePaChs, bool bToSaveProps);
		void LoadProps();
		bool IsEqual(const CPhysXObjSelGroup& selGroup) const;

	private:
		const ICommandEditor* m_pCmd;
		int m_Label;
		int m_nSavedType;
		std::vector<IObjBase*> m_SelSequence;
		ObjContainer m_info;
	};

public:
	static const int BEGINFLAG = -10;
	CmdContext();
	~CmdContext();

	bool HasBackwardContext() const;
	bool HasForwardContext() const;

	bool SaveCurrentContext(const ICommandEditor& cmd, const CPhysXObjSelGroup& selGroup);
	bool LoadForwardContext(CPhysXObjSelGroup& selGroup);
	bool LoadBackwardContext(CPhysXObjSelGroup& selGroup);
	void AbandonContext();
	void ClearAllContext();
	void RemoveLastSubmitContext(CPhysXObjSelGroup& selGroup);

	void AddLabel(int nLabel);
	void Rollback(int nLabel, bool bRemoveLabel, CPhysXObjSelGroup& selGroup);

private:
	typedef	  CmdInfo*					CC_Elem_Type;
	typedef	  std::list<CC_Elem_Type>	CIStore;
	typedef	  CIStore::iterator			CISItor;

	void ReleaseToEnd(CISItor& itFrom);

private:
	CIStore m_theContexts;
	CISItor m_itCurCmd;
};

class CmdQueue
{
public:
	CmdQueue();

	bool UndoEnable() const;
	bool RedoEnable() const;

	void Undo();
	void Redo();

	void AddLabelToCurrentCmd(int nLabel);
	void RollbackToLabel(int nLabel, bool bRemoveLabel);

	void SetSelGroup(CPhysXObjSelGroup& selGroup) { m_pSelGroup = &selGroup; }
	bool SubmitBeforeExecution(const ICommandEditor& cmd);
	void CancelLastSubmit();
	void ClearAllCmds();

private:
	CPhysXObjSelGroup* m_pSelGroup;
	CmdContext m_context;
};

class ICommandEditor
{
public:
	enum ImpactType
	{
		IT_SELGROUP		= (1<<0),
		IT_PROPERTIES	= (1<<1),
		IT_PARENT_CHILD = (1<<2),
	};

	virtual ~ICommandEditor() {}
	virtual int GetImpactType() const = 0;
	virtual int GetTargetObjects(std::vector<IObjBase*>& outObjs) const { return 0; }

	virtual void UndoNotifyObjsCmdStartExecution(IObjBase& obj, IPropObjBase* pCmdProp) const {}
	virtual void UndoNotifyObjsCmdEndExecution(IObjBase& obj, IPropObjBase* pCmdProp) const {}
	virtual void RedoNotifyObjsCmdStartExetution(IObjBase& obj, IPropObjBase* pCmdProp) const {}
	virtual void RedoNotifyObjsCmdEndExecution(IObjBase& obj, IPropObjBase* pCmdProp) const {}

protected:
	virtual ICommandEditor* Clone() const = 0;

protected:
	friend class CmdQueue;
};

class CmdSelAppend : public ICommandEditor
{
public:
	virtual int GetImpactType() const { return IT_SELGROUP; }

protected:
	virtual CmdSelAppend* Clone() const { return new CmdSelAppend(*this); };
};

class CmdSelRemove : public ICommandEditor
{
public:
	virtual int GetImpactType() const { return IT_SELGROUP; }

protected:
	virtual CmdSelRemove* Clone() const { return new CmdSelRemove(*this); };
};

class CmdSelReplace : public ICommandEditor
{
public:
	virtual int GetImpactType() const { return IT_SELGROUP; }

protected:
	virtual CmdSelReplace* Clone() const { return new CmdSelReplace(*this); };
};

class CmdPropChange : public ICommandEditor
{
public:
	virtual int GetImpactType() const { return IT_PROPERTIES; }

protected:
	virtual CmdPropChange* Clone() const { return new CmdPropChange(*this); };
};

class CmdCreateObj : public ICommandEditor
{
public:
	CmdCreateObj(IEngineUtility& eu) : m_eu(eu) {}
	virtual int GetImpactType() const { return IT_SELGROUP | IT_PROPERTIES; }

protected:
	virtual CmdCreateObj* Clone() const { return new CmdCreateObj(*this); };
	virtual void UndoNotifyObjsCmdStartExecution(IObjBase& obj, IPropObjBase* pCmdProp) const;
	virtual void RedoNotifyObjsCmdEndExecution(IObjBase& obj, IPropObjBase* pCmdProp) const;

private:
	IEngineUtility& m_eu;
};

class CmdReleaseObj : public ICommandEditor
{
public:
	CmdReleaseObj(IEngineUtility& eu) : m_eu(eu) {}
	virtual int GetImpactType() const { return IT_SELGROUP | IT_PARENT_CHILD | IT_PROPERTIES; }

protected:
	virtual CmdReleaseObj* Clone() const { return new CmdReleaseObj(*this); };
	virtual void UndoNotifyObjsCmdEndExecution(IObjBase& obj, IPropObjBase* pCmdProp) const;
	virtual void RedoNotifyObjsCmdStartExetution(IObjBase& obj, IPropObjBase* pCmdProp) const;

protected:
	IEngineUtility& m_eu;
};

class CmdParentChange : public ICommandEditor
{
public:
	CmdParentChange(IObjBase& obj) : m_pObj(&obj) {}
	virtual int GetImpactType() const { return IT_PARENT_CHILD; }
	virtual int GetTargetObjects(std::vector<IObjBase*>& outObjs) const
	{
		outObjs.push_back(m_pObj);
		return int(outObjs.size());
	}

protected:
	virtual CmdParentChange* Clone() const { return new CmdParentChange(*this); };
	virtual void UndoNotifyObjsCmdStartExecution(IObjBase& obj, IPropObjBase* pCmdProp) const;

private:
	IObjBase* m_pObj;
};

#endif
