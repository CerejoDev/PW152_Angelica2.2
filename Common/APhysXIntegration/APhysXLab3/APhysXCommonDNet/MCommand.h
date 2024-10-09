#pragma once

//wrapper for McmdQueue
namespace APhysXCommonDNet
{
	public enum class MCmdType
	{
		CmdSelAppend,
		CmdSelRemove,
		CmdSelReplace,
		CmdPropChange,
		CmdCreateObj,
		CmdReleaseObj,
	};

	public ref class MCmdQueue
	{
	public:
		MCmdQueue();
		!MCmdQueue();
		~MCmdQueue();

		bool UndoEnable();
		bool RedoEnable();

		void Undo();
		void Redo();

		//void SetSelGroup(CPhysXObjSelGroup& selGroup) { m_pSelGroup = &selGroup; }
		bool SubmitBeforeExecution(MCmdType cmdtype);
		void CancelLastSubmit();
		void ClearAllCmds();

	internal:
		MCmdQueue(CmdQueue* pCmdQueue);

	private:
		CmdQueue* m_pCmdQueue;
		bool m_bNeedRelease;
	};

}