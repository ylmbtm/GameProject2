#ifndef _LOGIN_CMD_HANDLER_H_
#define _LOGIN_CMD_HANDLER_H_
#include "CmdHandler/CommonCmdHandler.h"
#include "PacketDef/ServerPacket.h"


class CLoginCmdHandler
{
public:
	CLoginCmdHandler();

	~CLoginCmdHandler();

	BOOL Init();

	BOOL Uninit();

	//*********************消息处理定义开始******************************
public:
	BOOL OnCmdNewAccountReq(NetPacket *pPacket);
	BOOL OnCmdNewCharReq(NetPacket *pPacket);
	BOOL OnCmdDelCharReq(NetPacket *pPacket);
	BOOL OnCmdPickCharReq(NetPacket *pPacket);
	BOOL OnCmdLoginReq(NetPacket *pPacket);

	BOOL OnCmdDBNewAccountAck(NetPacket *pPacket);
	BOOL OnCmdDBNewCharAck(NetPacket *pPacket);
	BOOL OnCmdDBDelCharAck(NetPacket *pPacket);
	BOOL OnCmdDBPickCharAck(NetPacket *pPacket);
	BOOL OnCmdDBLoginAck(NetPacket *pPacket);

	
	//*********************消息处理定义结束******************************
};

#endif //_LOGIN_CMD_HANDLER_H_
