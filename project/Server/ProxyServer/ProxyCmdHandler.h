#ifndef _PROXY_SERVER_CMD_HANDLER_H_
#define _PROXY_SERVER_CMD_HANDLER_H_
#include "CmdHandler/CommonCmdHandler.h"
#include "PacketDef/ServerPacket.h"

class  CStaticPlayer;

class CProxyCmdHandler
{
public:
	CProxyCmdHandler();

	~CProxyCmdHandler();

	BOOL		Init(UINT32 dwReserved);

	BOOL		Uninit();

	BOOL		DispatchPacket(NetPacket *pNetPacket);

public:
	BOOL		RelayToGameServer(CStaticPlayer *pClientObj, IDataBuffer *pBuffer);

	BOOL		RelayToWorldServer(CStaticPlayer *pClientObj, IDataBuffer *pBuffer);

	BOOL		RelayToClient(CStaticPlayer *pClientObj, IDataBuffer *pBuffer);

	//*********************消息处理定义开始******************************
public:
	BOOL		OnCmdDisConnectNotify(UINT16 wCommandID, UINT64 u64ConnID, CBufferHelper *pBufferHelper);
};

#endif //_PROXY_SERVER_CMD_HANDLER_H_
