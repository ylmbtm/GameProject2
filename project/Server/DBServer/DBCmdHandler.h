#ifndef _DB_CMD_HANDLER_H_
#define _DB_CMD_HANDLER_H_
#include "CmdHandler/CommonCmdHandler.h"
#include "PacketDef/ServerPacket.h"
#include "DBProcManager.h"
#include "DBPlayerObject.h"


class CDBCmdHandler
{
public:
	CDBCmdHandler();

	~CDBCmdHandler();

	BOOL Init(UINT32 dwReserved);

	BOOL Uninit();

	BOOL DispatchPacket(NetPacket *pNetPacket);

	BOOL OnUpdate( UINT32 dwTick );

	//*********************消息处理定义开始******************************
public:
	BOOL OnCmdDBNewAccountReq(NetPacket *pPacket);
	BOOL OnCmdDBNewCharReq(NetPacket *pPacket);
	BOOL OnCmdDBDelCharReq(NetPacket *pPacket);
	BOOL OnCmdDBPickCharReq(NetPacket *pPacket);
	BOOL OnCmdDBLoginReq(NetPacket *pPacket);
	BOOL OnCmdDBLoadCharReq(NetPacket *pPacket);
	BOOL OnCmdDBSaveCharReq(NetPacket *pPacket);
	
	//*********************消息处理定义结束******************************

public:

	//读取玩家的数据
	BOOL LoadPlayerBaseData(UINT64 u64CharID);


	//玩家数据管理器
	CDBPlayerObjectMgr  m_DBPlayerMgr;

	//数据库定义
	CDBProcManager   m_DBProcManager;
};

#endif //_DB_CMD_HANDLER_H_
