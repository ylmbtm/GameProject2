#ifndef _DB_CMD_HANDLER_H_
#define _DB_CMD_HANDLER_H_
#include "CmdHandler/CommonCmdHandler.h"
#include "PacketDef/ServerPacket.h"
#include "DBProcManager.h"
#include "DBPlayerObject.h"
#include "DBConnection.h"
#include "DBStoredProcMgr.h"


class CDBCmdHandler : public CCommonCmdHandler
{
public:
	CDBCmdHandler();

	~CDBCmdHandler();

	BOOL DispatchPacket(NetPacket *pNetPacket);

	BOOL OnUpdate( UINT32 dwTick );

	BOOL Init(UINT32 dwReserved);

	BOOL Uninit();

	BOOL OnThreadBegin();

	BOOL OnThreadEnd();

	//*********************消息处理定义开始******************************
public:

	BOOL OnCmdDBNewCharReq(NetPacket *pPacket);
	BOOL OnCmdDBDelCharReq(NetPacket *pPacket);
	BOOL OnCmdDBPickCharReq(NetPacket *pPacket);
	
	BOOL OnCmdDBLoadCharReq(NetPacket *pPacket);
	BOOL OnCmdDBSaveCharReq(NetPacket *pPacket);
	
	//*********************消息处理定义结束******************************

public:

	//读取玩家的数据
	BOOL LoadPlayerBaseData(UINT64 u64CharID);

	//真实的mysql
	CDBConnection   m_DBConnection;

	CDBStoredProcedureMgr m_DBProceduceMgr;

	//玩家数据管理器
	CDBPlayerObjectMgr  m_DBPlayerMgr;

	//数据库定义
	CDBProcManager   m_DBProcManager;
};

#endif //_DB_CMD_HANDLER_H_
