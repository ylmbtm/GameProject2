#ifndef _WORLD_CMD_HANDLER_H_
#define _WORLD_CMD_HANDLER_H_
#include "CmdHandler/CommonCmdHandler.h"
#include "PacketDef/ServerPacket.h"
#include "GameSvrMgr.h"
#include "PlayerObject.h"

class CWorldCmdHandler
{
public:
	CWorldCmdHandler();

	~CWorldCmdHandler();

	BOOL DispatchPacket(NetPacket *pNetPacket);

	BOOL OnUpdate( UINT32 dwTick );

	BOOL Init(UINT32 dwReserved);

	BOOL Uninit();

	//*********************消息处理定义开始******************************
public:
	BOOL OnCmdEnterGameReq(NetPacket *pNetPacket);
	BOOL OnCmdDBLoadCharAck(NetPacket *pNetPacket);
	BOOL OnCmdCreateSceneAck(NetPacket *pNetPacket);
	BOOL OnCmdLeaveGameReq(NetPacket *pNetPacket);

	//*********************消息处理定义结束******************************


	CGameSvrMgr			m_GameSvrMgr;

	CPlayerObjectMgr    m_PlayerObjectMgr;
};

#endif //_WORLD_CMD_HANDLER_H_
