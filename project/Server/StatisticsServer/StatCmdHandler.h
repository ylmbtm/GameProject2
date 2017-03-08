#ifndef _STAT_CMD_HANDLER_H_
#define _STAT_CMD_HANDLER_H_
#include "CmdHandler/CommonCmdHandler.h"
#include "PacketDef/ServerPacket.h"

class CStatCmdHandler
{
public:
	CStatCmdHandler();

	~CStatCmdHandler();

	BOOL		Init(UINT32 dwReserved);

	BOOL		Uninit();

	BOOL		DispatchPacket( NetPacket *pNetPacket);
	//*********************消息处理定义开始******************************
public:
	BOOL OnCmdSvrRunningStateReq(NetPacket *pPacket);


	//*********************消息处理定义结束******************************
	//日志处理函数
	BOOL OnSvrReport_NormalLog(CBufferHelper *pBufferHelper);

};

#endif //_STAT_CMD_HANDLER_H_
